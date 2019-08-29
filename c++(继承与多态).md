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

其实，以上的方法指的是普通方法，对于特殊方法---虚函数的调用则完全不一样！

#### 虚函数

首先，我们看一下当Base* 指向Derive对象时，而Base类中含有虚函数时，基类和派生类大小、基类和派生类指针(引用)的类型。

```c++
#include <typeinfo>
using namespace std;

class Base
{
public:
	Base(int a = 1)
	{
		ma = a;
	}

	virtual void show()
	{
		cout<<"base show 1"<<endl;
	}

	void show(int b)
	{
		cout<<"base show 2"<<endl;
	}

private:
	int ma;
};

 

class Derive : public Base
{
public:
	Derive(int b = 2):Base(b)
	{
		mb = b;
	}

	void show()
	{
		cout<<"derive show 1"<<endl;
	}

private:
	int mb;
};

int main()
{
	Base b;
	Derive d;
	Base* p = &d;

	cout<<"base size:"<<" "<<sizeof(b)<<endl;
	cout<<"derive size:"<<" "<<sizeof(d)<<endl;
	cout<<"p type:"<<" "<<typeid(p).name()<<endl;
	cout<<"*p type:"<<" "<<typeid(*p).name()<<endl;
	return 0;
}

```

![运行结果](./images/1566921450033.png)

分析：当Base类中有虚函数时，不论是Base类还是Derive类，它们的大小都增加了4个字节。并且当Base* 指向Derive对象时，* Base的类型却变为Derive，不再和指针本身的类型相关，

**1.虚函数指针**

实际上，Base和Derive类增加的4个字节就是虚函数指针的大小，每一个类只要有虚函数（包括继承而来的），它就有且只有一个虚函数指针。类的大小就是总的成员变量的大小加上一个虚函数指针的大小。虚函数指针指向的是一张虚表，里面是这个类所有虚函数的地址，一个类对应一张虚函数表，而虚函数指针存在于每一个对象中，并且永远占据对线内存的前四个字节。

以下含有虚函数的Base类为例，下面是它的内存布局：
```c++
class Base
{
public:
	Base(int a = 1)
	{
		ma = a;
	}
	
	virtual void show()
	{
		cout << "base show 1" << endl;
	}
	
	virtual int getnum()
	{
		return ma;
	}
private:
	int ma;
}
```

![运行结果](./images/1566925958389.png)

**2.虚函数表**

虚函数表又称为 ”虚表“， 它在编译期间就已经确定，在程序运行时就会呗装载到只读数据段，在整个程序运行期间都会一直存在。一个类实例化的多个对象，它们的虚函数指针指向的是同一个表。

以上Base类中虚函数指针vfptr指向的虚函数表---vftable如下所示：

![enter description here](./images/1566927719892.png)

**3.虚表的合并**
Base类的派生类Derive类定义：

```c++
class Derive : public Base
{
public:
	Derive(int b = 2):Base(b)
	{
		mb = b;
	}
	
	void show()
	{
		cout << "derive show 1" << endl;
	}
	
	virtual int getval()
	{
		return mb;
	}

private:
	int mb;
}
```
上面已经给出了Base类的内存布局，以下是仿照其画出的Derive类的内存布局：

![派生类的内存布局](./images/1566930238946.png)

实际上，Derive类的内存布局并不是这样，前面明确提到 ” 只要有虚函数，就一定有且只有一个虚函数指针“ ， 那么派生类中只能有一个虚函数指针，又根据 “ 虚函数指针永远占据对象的前四个字节 ” 原则，那么正确的内存布局见下：

![真正派生类的内存布局](./images/1566933408937.png)

相应地，派生类地虚函数表也有变化。如果派生类中实现了同名覆盖函数，则派生类虚表中同名覆盖函数地地址会将基类该同名地地址覆盖。派生类中如果没有实现覆盖，则里面的同名函数地址还是基类方法的地址。Derive类中对show方法实现了同名方法覆盖，则它的虚函数表为：

![enter description here](./images/1567046716774.png)

**4.那些方法不能实现为虚函数？**

成员函数能实现为虚函数需要满足两个前提条件：1.成员方法能取地址  2.成员方法依赖于对像。
第一点毋庸置疑，虚函数表中需要存储虚函数的地址。第二点，通过虚函数指针来找到虚函数表从而调用其中的方法，而虚函数指针又存在于对像中，所以这就意味着虚函数的调用需要依赖对象。