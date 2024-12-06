---
title: 游戏设计-mission机制
tags: 副本机制,Lua
category: /小书匠/日记/2024-10
grammar_cjkRuby: true
---



----------

在游戏服务端开发的相关过程，如何简易化一些重复工作是十分重要的：例如副本流程

### 1. 实现一个基于 Lua 的简单类继承机制
<font color = "red">通过元表（metatable）和基类（tbBase）的递归调用实现了多层继承和构造函数的调用逻辑。</font>

```lua
function Lib:NewClass(tbBase, ...)
    local tbNew = { _tbBase = tbBase };        -- 基类
    setmetatable(tbNew, self._tbCommonMetatable);
    local tbRoot = tbNew;
    local tbInit = {};
    repeat                                        -- 寻找最基基类
        tbRoot = rawget(tbRoot, "_tbBase");
        local fnInit = rawget(tbRoot, "init");
        if (type(fnInit) == "function") then
            table.insert(tbInit, fnInit);         -- 放入构造函数栈
        end
    until (not rawget(tbRoot, "_tbBase"));
    for i = #tbInit, 1, -1 do
        local fnInit = tbInit[i];
        if fnInit then
            fnInit(tbNew, table.unpack({...}));           -- 从底向上调用构造函数
        end
    end
    return tbNew;
end
```

#### 工作原理分析：
<br>
1. <font color = "red">基类机制：</font>参数 tbBase 是新类的基类， tbNew 是创建的新类。<br>
新类 tbNew 通过 "_tbBase" 字段保存了对其基类的引用，形成了一个链式结构，方便查找继承层次。 <br>
2. <font color = "red">设置元表：</font>setmetatable(tbNew，self._tbCommonMetatable) 将新类 tbNew 的元素设置为公共元表，实现通用行为（如方法查找）。<br>
3. <font color = "red">构造函数查找：</font>3.1使用 repeat 循环从当前类向上查找所有基类的构造函数（假设构造函数以init命名）。3.2通过 rawget(tbRoot，"init")确保获取的基类自身的 init 方法，而不是从元表继承的方法。3.3 将找到的构造函数依次插入 tbInit 栈，构造函数的执行顺序从基类（根基类）到派生类（子类）。<br>
4. <font color = "red">调用构造函数：</font>4.1 构造函数按逆序调用：从最基类的构造函数开始，逐层向下调用，直到当前类的构造函数。4.2 使用 ` fnInit(tbNew, table.unpack({...})) ` 将参数传递给每个构造函数。<br>
5. 返回新创建的类。

---
<strong>关键点：支持继承，rawget 确保只从当前类查找字段，不会从元表中获取，保证继承结构的独立性。这样我们只需要把一些通用的方法包装起来，后面副本相关使用继承在加上一些方法的重写，可以大大减少开发难度。</strong>

``` lua
-- 从Mission的基类产生新的Mission    
function Mission:New()
    return Lib:NewClass(self.tbClassBase);
end

-- 自己副本的mission实例
local tbMission = Mission:New();

-- 设定自己要重写的函数，当执行到当前函数会调用自己重写的部分
tbMission.tbMisCfg = {
    nForbidAcross           = 1;
    nClearCombatCounter     = 1;
    nEnterScene             = 1;
    nLeavedScene            = 1;

    nDisconnect             = 1;
    nReconnect              = 1;
    nOnKillNpc              = 1;
    nOnDeath                = 1;
    nForbidRush             = 1;
	....
}

--[[
--  在调用tbMission:Open()之前，可以在tbMission.tbMisCfg中设定一些配置参数，
--  以便这个Mission具有一些扩展功能。
--  如无特殊说明，变量为nil表示不使用此功能，否则均表示启用。
-- tbMisCfg.tbEnterPos               入口坐标：    [nGroupId] = {nMapId, instanceId, nX, nY} 或 [nGroupId] = {tbPos, tbPos, ...} 玩家加入时到达指定的地图坐标；如果是一组坐标，会随机选取一个。
-- tbMisCfg.tbLeavePos               出口坐标：    [nGroupId] = {nMapId, instanceId, nX, nY} 或 [nGroupId] = {tbPos, tbPos, ...} 玩家离开时到达指定的地图坐标（中途下线也会到这里）；如果是一组坐标，会随机选取一个。
-- tbMisCfg.nFightState              设定战斗状态：加入时设定为战斗，离开时设定为非战斗；
-- tbMisCfg.nPkState                 PK状态：加入时设定为nPkState状态，禁止切换；离开时设定为练功，允许切换，PK状态枚举参见\data\script\character\playerscript.lua
-- tbMisCfg.tbCamp                   改变当前阵营：[nGroupId] = nCamp 加入时阻止当前阵营改变，如果存在tbCamp[nGroupId] or tbCamp[0]，则设定为指定的当前阵营。  离开时恢复当前阵营，允许当前阵营改变。
-- tbMisCfg.nForbidTeam              禁止队伍操作：
-- tbMisCfg.nForbidChangeRole        禁止玩家切换角色
-- tbMisCfg.nLogOutRV                设置恢复状态，异常登出下次上线时自动恢复进入Mission
-- tbMisCfg.nIsCloseDelay            延迟关闭time
-- tbMisCfg.nCloseNotKick            结束后不主动传出角色
-- tbMisCfg.nNoLogOut                是否不注册logout事件，默认注册，登出时会自动调用KickFamily
-- tbMisCfg.nForbidAcross            禁止轻功穿越障碍
-- tbMisCfg.nForbidRush              禁止疾跑
-- tbMisCfg.nAutoCloseTime           自动超时关闭Mission的时间
-- tbMisCfg.nClearCombatCounter      进入副本时清除伤害统计数据
-- tbMisCfg.nMengXinCounter          是否记录副本里的萌新个数数据 
-- tbMisCfg.nFakeTeamScene           是否允许队伍存在假人
-- tbMisCfg.nHeartBookSkill          设置使用心法技能模板

--                                    (萌新Join前调用 tbMission:OnMengXinJoinPre)
--                                    (萌新Join后调用 tbMission:OnMengXinJoinPost)
--                                    (萌新Leave前调用 tbMission:OnMengXinLeavePre)
--                                    (萌新Leave后调用 tbMission:OnMengXinLeavePost)
--                                    (添加第一个萌新buff时调用 tbMission:OnAddFirstMengXinBuff)
--                                    (移除最后的萌新buff时调用 tbMission:OnRemoveLastMengXinBuff)
-- tbMisCfg.nOnRobotDeath            开启机器人死亡回调：   tbMission:OnRobotDeath
-- tbMisCfg.nOnDeath                 开启玩家死亡回调：     tbMission:OnDeath
-- tbMisCfg.nOnRevival               开启玩家疗伤回调：     tbMission:OnRevival
-- tbMisCfg.nOnKillNpc               开启玩家杀怪回调：     tbMission:OnKillNpc（如果是怪物死亡计数不要用这个事件，可能遗漏计数，请在Npc死亡事件处理）
-- tbMisCfg.nEnterScene              是否注册进入场景事件:  tbMission:OnEnterScene, tbMission:OnClientEnterScene
-- tbMisCfg.nLeavedScene             是否注册离开场景事件:  tbMission:OnLeavedScene
-- tbMisCfg.nTransferIn              是否自定义进入传送:    tbMission:OnTransferIn, 自定义传入接口
-- tbMisCfg.nTransferOut             是否自定义离开传送:    tbMission:OnTransferOut, 自定义传出接口
-- tbMisCfg.nDisconnect              是否注册断连事件:      tbMission:OnDisConnect
-- tbMisCfg.nReconnect               是否注册重连事件:      tbMission:OnReConnect
-- tbMisCfg.nElementalChainEvent     是否注册五行共鸣事件:  tbMission:OnElementalChainEvent
-- tbMisCfg.nRegJoinTeam             是否注册加入队伍事件:  tbMission:OnJoinTeam
-- tbMisCfg.nRegLeaveTeam            是否注册离开队伍事件:  tbMission:OnLeaveTeam
-- tbMisCfg.nEnterTrap               是否注册进入Trap回调   tbMission:OnEnterTrap
-- tbMisCfg.nLeaveTrap               是否注册离开Trap回调   tbMission:OnLeaveTrap
-- tbMisCfg.nJoinKin                 是否注册加入家族回调   tbMission:OnJoinKin
-- tbMisCfg.nQuitKin                 是否注册退出家族回调   tbMission:OnQuitKin
-- tbMisCfg.nRegClientRelogin        是否注册客户端点击退出登录回调   tbMission:OnClientClickRelogin
-- tbMisCfg.nRegClientBackCity       是否注册客户端点击返回主城回调   tbMission:OnClientClickBackCity

-- tbMisCfg.OnPlayerSkeletonLoaded   是否注册角色模型加载完成回调 tbMission:OnPlayerSkeletonLoaded
-- 时间事件表配置
-- tbMisEventList                    Mission中时间事件表，格式为 { {状态标识, 状态持续时间, 状态回调函数[, 参数]}， }  调用一次GoNextState后才会开始生效
--]]

-- 当前GameServer 上面缓存你这个副本实例
Mission.tbSceneMission = Mission.tbSceneMission or {};
Mission.tbSceneMission["SceneTempId_SceneInstancId"] = tbMission;

```

这种方法的好处大大降低了副本实现的难度，如果策划不需要新机制，基本通过配置action就可以完成副本流程的开发，不过这种方法多消耗性能。