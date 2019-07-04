---
title: Effective C++
tags: C++, 改善程序设计，提高代码质量
grammar_cjkRuby: true
---

#### 尽量以const，enum，inline替换 #define
这里需要说一下==宏替换是在预处理时期==，它不会进行错误的处理（当然不是不处理是不方便查错），而==const，enum，inline都在编译时期 #F44336==有查询错误的机制。
##### 第一点 const,enum代替宏常量
如果我们使用`#define ASP 1.623`,记号名称可能从未被编译器记录；而在编译器开始处理源码之前它就被预处理器给移走了。于是记号ASP就没有被记入记号表内。这样我们看见的错误就会变成1.623而不是ASP。再者如果这个记号被定义在一个非你所写的头文件中。这就会给你改错带来大量的时间浪费
解决办法：`const double ASP = 1.623`

当我们使用常量代替宏时候有两点：

 1. 宏定义的字符串必需是被两个const修饰的指针，如：`const char* const authorName = "helloWorld"; 或者C++：const std::string authorName("helloWorld");`
 2. 就是class的专属常量。为了将常量的作用域限制于class内，你必须让它成为class的一个成员；而为确保此常量至多只有一份实体，你必须让它成为一个static成员：

``` c++
class GamePlayer {
private:	
	static const int Number = 5;	//常量声明式
	int scores[Number];					//使用该常量
	....
};
```
但是注意这个是声明时候获取到一个参数5，如果你取某个class专属常量的地址，或纵使你不取其地址而你的编译器却坚持要看到一个定义式，你就必须另外提供定义式：`const int GamePlayer::NumTurns;`
因为在声明时候我们给了初值，则定义时候不能在带初值了。这个就会在一些旧的编译器上它不允许static成员在其声明式上获得初值。
一般只能写成如下形式：

``` c++
class ConstEstimate {
private:
	static const double FudgeFactor;
	....
};
const double ConstEstimate::FudgeFactor = 1.35;
```
当你的class中有个使用常量的数组如上面，但是编译器又不支持初值的设定，我们可以使用枚举来完成这项任务:

``` c++
class GamePlayer {
private:	
	enum { Number = 5 }; 			  
	int scores[Number];					//使用该常量
	....
};
```
##### 第二点 inline替换宏函数
当我们使用如下宏函数：`#define CALL_WITH_MAX(a,b) f((a) > (b) ? (a):(b))`时必须记住为宏中的所有参加的变量加上小括号，否则别人用的时候，或者自己过很长时间在使用的时候，都可能出现和自己想法有出入的运算结果，但是就算是都加括号也可能出现和自己想要的不符合的结果，例如下面这个程序：
``` c++
#include <stdio.h>
#define CALL_WITH_MAX(a,b) f((a) > (b) ? (a):(b))
void f()
{}
int main()
{
	int a = 5, b = 0;
	CALL_WITH_MAX(++a, b);
	CALL_WITH_MAX(++a, b + 10);
	printf("a = %d; b = %d", a, b);
	return 0;
}
```
![enter description here](./images/1562229209908.png)
宏在调用的时候根据它选择的不同，它每次a自增的次数也就不相同，
而下面则是C++提供的一种解决上面问题的方法：inline函数

``` c++
template<typename T>
inline void callWithMax(const T& a, const T& b)
{
	f(a > b ?a : b);
}
```
这里因为加入了限制条件const，所有就不允许变量改变，这样就保证了，程序的运算出错大大减少。

#### 尽可能使用const
关键字const，你可以用它在class外部修饰global或namespace作用域中的常量，或修饰文件、函数、或区块作用域中被声明为static的对象。你也可以用它修饰class内部的static和non-static成员变量。面对指针