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
<font>
1. 基类机制：参数 tbBase 是新类的基类， tbNew 是创建的新类。
</font>