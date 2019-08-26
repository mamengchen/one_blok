---
title: c++(继承与多态)
grammar_cjkRuby: true
---

#### 类继承
 C++是一种面向对象的语言，最重要的一个目的就是——提供可重用的代码，而类继承就是C++提供来扩展和修改类的方法。类继承就是从已有的类中派生出新的类，派生类继承了基类的特性，同时可以添加自己的特性。实际上，类与类之间的关系分为三种：代理、组合和继承。以下是三种关系的图解：(为了更好的理解）
 
![20180517235653424](./images/20180517235653424.png)

基类可以派生出派生类，基类也叫做“父类”，派生类也称为“子类”。

那么，派生类从基类中继承了哪些东西呢？分为两个方面：1. 变量——派生类继承了基类中所有的成员变量，并从基类中继承了基类作用域，即使子类中的变量和父类中的同名，有了作用域，两者也不冲突。2.方法——派生类继承了基类中除去构造函数、析构函数以外的所有方法。

#### 继承方式和访问限定符

 继承方式有三种——public、protected和private，不同的继承方式对继承到派生类中的基类成员有什么影响？见下图：
 
 ![继承方式与访问限定符的关系](./images/1566487315436.png)
 
 总的来说，父类成员的访问限定符通过继承派生到子类中之后，访问限定符的权限小于、等于原权限。其中，父类中的private成员只有父类本身及其友元可以访问，通过其他方式都不能进行访问，当然就包括继承。protected多用于继承当中，如果对父类成员的要求是——子类可访问而外部不可访问，则可以选择protected继承方式。
 
 #### 派生类对象的构造方式
 
 前面也提到，派生类将基类中除去构造函数和析构函数的其他方法继承了过来，那么对于派生类对象中自己的成员变量和来自基类的成员变量，它们的构造方式是怎样的呢？
 
 答案是：1.先调用基类构造函数，构造基类部分成员变量，再调用派生类构造函数构造派生类部分的成员变量。2.基类部分成员的初始化方式在派生类构造函数的初始化列表中指定。3.若基类中还有成员对象，则先调用成员对象的构造函数，再调用基类构造函数，最后是派生类构造函数。析构顺序和构造顺序相反。见下：

```c++

#include <iostream>
using namespace std;


class Test
{
public:
	Test()
	{
		cout<<"Test::Test()"<<endl;
	}
private:
	int mc;
};

class Base
{
public:
	Base(int a)
	{
		ma = a;
		cout<<"Base::base()"<<endl;
	}

	~Base()
	{
		cout<<"Base::~base()"<<endl;
	}
private:
	int ma;
	Test t;
};


class Derive : public Base
{
public:
	Derive(int b):Base(b)
	{
		mb = b;
		cout<<"Derive::derive()"<<endl;
	}
	~Derive()
	{
		cout<<"Derive::~derive()"<<endl;
	}
private:
	int mb;
};


int main()
{
	Derive d(2);
	return 0;
}

```

![结构如此](./images/1566487642466.png)

#### 基类和派生类中同名成员的关系

派生类从基类中继承过来的成员（函数，变量）可能和派生类部分成员重名。
1.前面提到，派生类从基类中继承了基类作用域，所以同成员名变量可以靠作用域区分开（隐藏）。
2.同名成员函数则有三种关系：重载、隐藏和覆盖。

**1.重载_overload**
函数重载有三个条件，一函数名相同，二形参类型、个数、顺序不同，三相同作用域。根据第三个条件，可知函数重载只可能发生在一个类中，见下：

```c++
class Base
{
public:
		Base(int a)
		{
			ma = a;
		}
		
		void show()
		{
			cout << "base show 1" << endl;
		}
		
		void show(int b)
		{
			cout << "base show 2" << endl;
		}
		
private:
	int ma;
};
```
其中，两个show函数构成函数重载。

**2.函数隐藏_overhide**
在派生类中将基类中的同名成员方法隐藏，要想在派生类对象中访问基类同名成员得加上基类作用域。（注意，如果该同名方法在基类中实现了重载，在派生类中同样需要指定作用域，而不能通过简单得传参，调用带参重载方法）

```c++
#include <iostream>
using namespace std;

class Base
{
public:
		Base(int a)
		{
			ma = a;
		}
		
		void show()
		{
			cout << "base show 1" << endl;
		}
		
		void show(int b)
		{
			cout << "base show 2" << endl;
		}
		
private:
	int ma;
};

class Derive : public Base
{
public:
	Derive(int b) : Base(b)
	{
		mb = b;
	}
	
	void show()
	{
		cout << "derive show 1" << endl;
	}
	
private:
	int mb;
};

int main()
{
	Derive d(1);
	d.show();
	d.Base::show();
	d.Base::show(2);
	
	return 0;
}
```

![运行结果](./images/1566748361098.png)

**3.函数覆盖_override**
基类、派生类中的同名方法 函数头相同(参数、返回值)，且基类中该方法为虚函数，则派生类中的同名方法将基类中方法覆盖。这里涉及到了虚函数的问题，在后续进行讲解。函数隐藏和函数覆盖都是发生在基类和派生类之间的，可以这么理解：基类和派生类中的同名函数，除去是覆盖的情况，其他都是隐藏的情况。


#### 指针和引用

**1.基类对象和派生类对象**
- 1.1 派生类对象可以赋值给基类对象
- 1.2 基类对象不可以赋值给基类对象   

![enter description here](./images/1566789174469.png)

对于基类对象和派生类对象，编译器默认支持从下到上的转换，上是基类，下是派生类。

![enter description here](./images/1566789348083.png)

**2.基类指针（引用）和派生类指针（引用）**
- 2.1基类指针（引用）可以指向派生类对象，但只能访问派生类中基类部分的方法，不能访问派生类部分方法
- 2.2派生类指针（引用）不可以指向基类对象，解引用可能出错，因为派生类的一些方法可能基类没有

![enter description here](./images/1566790292638.png)

编译器只支持从上到下的转换，即只能允许基类指针去指向派生类类对象。

![enter description here](./images/1566796922397.png)

以上对于方法的访问都是基于指针的类型。我们可以看一下基类和派生类的大小，以及基类，派生类的指针（引用）的类型。

```c++
#include <iostream>
using namespace std;

class Base
{
	public:
	Base(int a = 1)
	{
		ma = a;
	}
	
	void show()
	{
		cout << "base show 1" << endl;
	}
	
	void show(int b)
	{
		cout << "base show 2" << endl;
	}
	
private:
	int ma;
}

class Derive : public Base
{
public:
	Derive(int b = 2) : Base(b)
	{
		mb = b;
	}
	
	void show()
	{
		cout << "derive show 1" << endl;
	}
	
private:
	int mb;
}

int main()
{
	Base b;
	Derive d;
	
	Base * p = &d;
	
	cout << "base size" << sizeof(b) << endl;
	cout << "derive size" << sizeof(d) << endl;
	cout << "p type" << typeid(p).name() << endl;
	cout << "*p type" << typeid(*p).name() << endl;
	return 0;
}
```

![运行结果](./images/1566807752591.png)

分析：Base类和Derive类的大小就是他们各自包含的成员变量的总大小，Derive类继承了Base类中的成员变量，所以要比Base类大4个字节。在上面提到，这里的方法调用都是依据指针的类型，所以我们可以看到对基类指针p解引用得到的类型只和指针本身的类型相关。