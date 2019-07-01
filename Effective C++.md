---
title: Effective C++
tags: C++, 改善程序设计，提高代码质量
grammar_cjkRuby: true
---

#### 尽量以const，enum，inline替换 #define
这里需要说一下==宏替换是在预处理时期==，它不会进行错误的处理（当然不是不处理是不方便查错），而==const，enum，inline都在编译时期 #F44336==有查询错误的机制。
##### 第一点 const代替宏常量
如果我们使用`#define ASP 1.623`,记号名称可能从未被编译器记录；而在编译器开始处理源码之前它就被预处理器给移走了。于是记号ASP就没有被记入记号表内。这样我们看见的错误就会变成1.623而不是ASP。再者如果这个记号被定义在一个非你所写的头文件中。这就会给你改错带来大量的时间浪费
解决办法：`const double ASP = 1.623`

当我们使用常量代替宏时候有两点：

 1. 宏定义的字符串必需是被两个const修饰的指针，如：`const char* const authorName = "helloWorld"; 或者C++：const std::string authorName("helloWorld");`
 2. 就是class的专属常量