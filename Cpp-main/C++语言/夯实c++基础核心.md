# 第一章 内存模型和编译链接

![在这里插入图片描述](https://img-blog.csdnimg.cn/0f8334c819a044d79f07c6f2e8fc2124.png)

## 1. 掌握进程虚拟地址空间区域划分

编程语言产生：指令+数据

exe 磁盘加载到内存，不可能直接加载到内存。

x86系统：linux系统会给当前进程分配一个 2^32 大小的空间 4G

它不存在，你却看得见，它是虚拟的。



0x00000000 ~ 0xC0000000 用户空间 user space 3G 

0xC0000000 ~ 0xFFFFFFFF kernal space 内核空间 1G



0x00000000 ~ 0x08048000 不能被访问

.text段（指令）； .rodata(只读数据段)； .data； .bss； .heap； 加载共享库 *.dll *.so； stack；命令行参数和环境变量；ZONE_DMA/ZONE_NORMAL/ZONE_HIGHMEM。

比如 `char *s = "asd";` 只能读、不能写。

```c
//空指针
char *s = 0;
printf("%ld", strlen(s));
//段错误 (核心已转储)
```

`int a = 12;` 指令在 text 段，运行时在栈。

每一个进程的用户空间是私有的，但是内核空间是共享的。



## 2. 从指令的角度掌握函数调用堆栈的详细过程

ebp栈底 和 esp栈顶 这两个指针秀操作

汇编里面相对地址 往上是减法操作、往下是加法（不理解）

mov 赋值 和 ebp 有关

push 压栈 和 esp 有关

左右括号：左括号申请栈空间、右括号还原。

形参直接往esp上 push  返回答案直接用eax寄存器

<= 4 字节 eax

`>4 && <= 8` eax edx

`> 8` 产生临时量



## 3. 从编译器角度理解C++代码的编译和链接原理

编译过程 + 链接过程

---

编译过程：

1. 预编译：#开头的命令（#pragma除外 链接阶段）

2. 编译：gcc g++

3. 汇编：x86 AT&T(unix系统)

4. .o ：二进制可重定位的目标文件

---

链接：编译完成的所有 .o 文件+静态库文件

1. 所有.o文件段的合并。符号表合并后，进行符号解析。（符号解析：所有对符号的引用，都要找到该符号定义的地方。）
2. 核心，符号的重定向。符号解析成功后给所有的符号分配虚拟地址，并将指令填上具体地址。

---

![在这里插入图片描述](https://img-blog.csdnimg.cn/0e67258074aa46c08b7128e37723f8b9.png)

l 是 local ，g 是 全局可见

`*UND*` 是对符号的引用



readelf -S main.o 把段打印出来看

```
// 生成调试信息
g++ -c main.cpp -g
objdump -S main.o
```

编译过程中，符号不分配虚拟地址，会发现地址是 0，所以 .o 运行不了  

----

.o 文件各个相同的段合并

---

![在这里插入图片描述](https://img-blog.csdnimg.cn/caafb7f2ce1849e89c5f624e6e3f29a3.png)

但是直接 g++ 两个文件没有问题。应该是不详细吧。

----

代码段数据段被加载到内存(load告诉系统)

```
elf header <= 程序的入口地址
program headers <= .text .data
```



# 第二章 C++基础知识

![在这里插入图片描述](https://img-blog.csdnimg.cn/e5536852336b42fda6c0bfcf0261bda6.png)

## 1. 形参带默认值的参数

函数的参数从右往左压

```cpp
#include <iostream>

using namespace std;

int sum (int a, int b = 20)
{
    return a + b;
}

int main()
{
    int a = 10;
    cout << sum(a) << endl;
    return 0;
}
```

形参默认值从右往左给

定义、声明处都可以带默认参数。默认值只能出现一次。

## 2. inline内联函数

在编译过程中，没有函数的调用开销了，在函数的调用点直接把函数的代码进行展开处理了。

不是所有的inline都会被编译器处理成内联函数，比如“递归”。

inline只是建议编译器把这个函数处理成内联函数。

debug版本上，inline是不起作用的；inline只有在release版本下才能出现。符号表中不生成符号了。

## 3. 函数重载

什么是函数重载？

一组函数，其中函数名相同，参数列表的个数或者类型不同。

1. C++支持函数重载，C不支持。

   C++代码产生函数符号时，函数名+参数列表；C函数名决定。

2. 函数重载需要注意什么？

   1. 局部优于全局，如果同名的函数在局部内声明，不会再去全局找。即重载要在同一作用域内。

   2. const 或者 volatile 的时候，是怎么影响形参类型的？

      const int 的类型是 int

3. C++和C语言代码之间如何互相调用？

   C++和C的符号生成规则不同，所以无法直接调用。注意初学要用大型编译器搞，命令行让其跑起来的方式我还不会。

   ```cpp
   //C++调用C 和 C调用C++ 都是在C++里面写 extern
   //告诉编译器这个符号用C的规则生成
   #ifdef __cplusplus
   extern "C"
   {
   #endif // __cplusplus
   	int sum(int a, int b)
   	{
   		return a + b;
   	}
   #ifdef __cplusplus
   }
   #endif // __cplusplus
   ```

   

```cpp
#include <iostream>
#include <typeinfo>

using namespace std;

int main()
{
    int a = 10;
    const int b = 20;
    int *c;
    const int *d;
    cout << typeid(a).name() << endl;
    cout << typeid(b).name() << endl;
    cout << typeid(c).name() << endl;
    cout << typeid(d).name() << endl;
    return 0;
}
/*
i
i
Pi
PKi
*/
```

静态时期的多态有函数重载。

## 4. const的用法

C里面：const**可以不初始化**。const 修饰的量不是常量，是**常变量**。不能作为左值。`const int a = 10;` 仅仅是语法上的不能被修改（a 这个符号不能被改变，内存可以变）

C++：const 必须初始化，初始值是立即数的叫做**常量**；是一个变量的**可以被看成常变量**。所以可以被用来定义数组的大小。

C中const就是被当作一个变量来编译生成指令的。

C++中，所有出现const变量名字的地方，都被常量的初始化替换了。

```c
//c语言
#include <stdio.h>

int main()
{
    const int a = 10;
    int *p = (int *)&a;
    *p = 20;
    printf("%d %d %d\n", a, *p, *(&a)); //20 20 20
    return 0;
}
```

同样一份代码，C++ 结果为 `10 20 10` 但是**内存被改变了**。

*(&a) 是被编译器优化了，直接是 a

让C++ 变为 C 的结果方式为将 const 由常量变为常变量。

```cpp
#include <stdio.h>

int main()
{
    int b = 10;
    const int a = b;
    int *p = (int *)&a;
    *p = 20;
    printf("%d %d %d\n", a, *p, *(&a)); //20 20 20
    return 0;
}
```

可以理解成将所有的 a 变成了 变量 b。

## 5. const和一二级指针的结合应用

### 5.1 const 和一级指针

const 修饰的量常出现的错误：

1. 常量不能再作为左值
2. 不能把常量的地址泄露给一个普通的指针或者普通的引用变量

C++语言规范：const修饰的是离它最近的类型。

```cpp
const int *p;  //去掉int, *p被修饰为const *p不能被修改，但是p可以被修改
int const* p; //和上面的一样
```

```cpp
#include <iostream>

using namespace std;

int main()
{
	int a = 1, b = 2;
	const int* p = &a;
	p = &b;
	cout << *p;
	return 0;
}
```

------



```cpp
int *const p; //被修饰的是int *      const修饰的是p本身
```

```cpp
int a = 1, b = 2;
int* const p = &a;
p = &b; //error!!!!!
```

但是*p没有被修饰

```cpp
int a = 1, b = 2;
int* const p = &a;
*p = 3;
cout << *p; //3
```

----



```cpp
const int *const p; //第一个const修饰*p 第二个const修饰p本身   严格模式
```

-----



不想把常量的地址泄露给一个普通指针，用这种方法：

```cpp
const int a = 10;
const int *p = &a; //*p不能被修改
```

-----

const 和指针的类型转换公式：

int* <= const int* **错**

const int* <= int* **对**



**const 如果右边没有指针 * 的话，不参与类型**

```cpp
#include <iostream>
#include <typeinfo>

using namespace std;

int main()
{
	int a = 1;
	int* const p = &a;
	cout << typeid(p).name() << endl; //int *
	return 0;
}
```



### 5.2 const 和二级指针

```cpp
int a = 10;
int* p = &a;
const int** q = &p; //error!!!

//*q 是 const int *
//不能把常量的地址泄露给一个普通的指针 普通指针一解引用，就把常量的值改了

//可以这样
int a = 10;
const int* p = &a;
const int** q = &p;
//也可以这样
int a = 10;
int* p = &a;
const int*const* q = &p; //锁死*q
```

```cpp
const int **q; //**q锁定
int *const* q; //*q锁定
int **const q; //q锁定
```

int** <= const int ** **错误**

const int** <= int ** **错误**

`int ** <= int*const*` **错误** const修饰右边的指针，相当于一级指针的 `int* <= const int*`

`int*const* <= int **` **正确** 相当于一级指针的`const int* <= int*`

 

## 6.左值引用和初识右值引用

有一种说法是：引用是更安全的指针。

引用和指针的区别？

1. 引用必须初始化的，指针可以不初始化。反汇编：引用和指针的底层一样。

   ```
   	int* p = &a;
   00D71FF9  lea         eax,[a]  
   00D71FFC  mov         dword ptr [p],eax  
   	int& b = a;
   00D71FFF  lea         eax,[a]  
   00D72002  mov         dword ptr [b],eax
   ```

2. 引用只有一级引用，没有多级引用。指针可以有多级指针。

3. 定义一个引用变量和定义一个指针变量，其汇编指令是一模一样的。都可以修改内存的值。

-------

```cpp
int array[5];
int (*p)[5] = &array;
int(&q)[5] = array;
cout << sizeof(p) << ' ' << sizeof(q);  //4 20
```

使用引用变量时会解引用。array 和 q 是一回事儿。

----

**左值**：有内存，有名字，值可以被修改

**右值**：没内存（立即数，放在寄存器里面），没名字

C++11提供了右值引用。

1. `int &&c = 10;` 指令上，可以自动产生临时量然后直接引用临时量

2. 一个右值引用变量本身是一个左值。
3. 右值引用不能引用左值，左值引用可以引用右值。 

```cpp
int&& a = 10;
a = 30;
const int &b = 20;
```

这两种方式的指令一样

```
00391FF2  mov         dword ptr [ebp-18h],14h  
00391FF9  lea         eax,[ebp-18h]  
00391FFC  mov         dword ptr [a],eax
```



## 7.const、指针、引用的结合使用

```cpp
int *p1 = (int*)0x0018ff44;
int*&&p2 = (int *)0x0018ff44;
int *const &p3 = (int*)0x0018ff44;
```

&和* 右边的一个`&`将左边的一个 `*` 变为`&`

```cpp
int a = 10;
int *p = &a;
int **q = &p; //等于 int*&q = p;
```

---

```cpp
int a = 10;
int* const p = &a;
int *& q = p; //error 把常量地址泄露给普通指针   int *const& q = p;
```



## 8.new 和 delete

1. malloc 和 free，称作 C 的库函数；new 和 delete，称作运算符

2. new 不仅可以做内存开辟，还可以做内存初始化操作

3. malloc开辟内存失败，是通过返回值和nullptr作比较；new开辟内存失败，是通过抛出bad_alloc类型的异常来判断的。

```cpp
try
{
    int* p = new int(20);
    cout << *p;
    delete p;
}
catch (const bad_alloc &e)
{

}
```

```cpp
int* q1 = new int[20](); //new int[20] 不初始化
for (int i = 0; i < 20; i++) cout << q1[i] << ' ';
delete []q1;
```

**new有多少种？**

```cpp
int *p1 = new int(20);
int *p2 = new (nothrow) int; //不抛出异常版
const int *p3 = new const int(40);
//定位new 在指定的内存上new
int data = 0;
int *p4 = new (&data) int(50); 
cout << data; //50
```



# 第三章 类和对象

基础部分告一段落，开始 oop 部分

## 1. 类和对象、this指针

类：实体的抽象类型

实体（属性、行为）   ->    ADT(abstract data type) （抽象数据类型）

对象  <-（实例化）类（属性：成员变量；行为：成员方法）

> **oop 语言的四大特征是什么？**
>
> 抽象 封装/隐藏 继承 多态

```cpp
#include <iostream>

using namespace std;

const int NAME_LEN = 20;
class CGoods //类名最好用C开始 
{
	//类的成员方法一经编译,所有的方法参数都会加一个this指针,接收该方法的对象的地址
public: //给外部提供共有的方法来访问私有的属性
	void init(const char* name, double price, int amount); //商品数据初始化
	void show(); //打印商品信息
	//给成员变量提供一组getxxx或setxxx的方法
	void setName(const char* name) { strcpy(_name, name); } //类体内实现的方法,自动处理成inline内联函数
	const char* getName() { return _name; }//防止解引用修改
private: //属性一般都是私有的
	//总共40个字节
	char _name[NAME_LEN]; //按最长的8对齐  20 + 4
	double _price; //8
	int _amount; //4 + 4
};

void CGoods::init(const char* name, double price, int amount)
{
	strcpy(_name, name);
	_price = price;
	_amount = amount;
}

void CGoods::show()
{
	cout << _name << endl;
	cout << _price << endl;
	cout << _amount << endl;
}

int main()
{
	//对象的内存大小只和成员变量有关
	CGoods good;
	good.init("面包", 10.5, 200);
	good.setName("小面包");
	good.show();
	return 0;
}
```



## 2. 构造函数和构析函数

自动 init 和 release

先构造的后析构。

析构函数不带参数，所以析构函数只能有一个；构造函数可以提供多个，叫做**重载**。

堆上的对象要手动析构

```cpp
#include <iostream>

using namespace std;
class CTest
{
public:
	CTest()
	{
		cout << this << "构造" << endl;
	}
	~CTest()
	{
		cout << this << "析构" << endl;
	}
};

int main()
{
	CTest t1;
	CTest* t2 = new CTest();
	return 0;
}
//006FF833构造
//00A7EFA8构造
//006FF833析构
```



## 3. 对象的深拷贝和浅拷贝

有**指针**指向对象的**外部资源**时，**浅拷贝**的析构会出问题：先构造的指针变成了野指针。

`CTest t3 = t1;` 默认拷贝构造函数，浅拷贝

拷贝时扩容用 for 循环， 不用 memcpy 原因：避免指向外部的指针指向同一块

```cpp
#include <iostream>

using namespace std;
class CTest
{
public:
	CTest(int size)
	{
		this->_size = size;
		_p = new int[size];
		cout << this << "构造" << endl;
	}
	//自定义拷贝构造函数，对象的浅拷贝有问题了
	CTest(const CTest& t)
	{
		_p = new int[t._size];
		for (int i = 0; i < t._size; i++) { //扩容用for循环，不用memcpy 原因：仍然是指针
			_p[i] = t._p[i];
		}
		cout << this << "深拷贝" << endl;
	}
	~CTest()
	{
		delete[] _p;
		_p = nullptr;
		cout << this << "析构" << endl;
	}
private:
	int _size;
	int* _p;
};

int main()
{
	CTest t1(10);
	CTest t3 = t1;
	return 0;
}
```



## 4. 代码应用实践

```cpp
//string
#include <iostream>

using namespace std;

class String
{
public:
	//普通构造
	String(const char* str = nullptr)
	{
		cout << this << ' ' << "const char*构造" << endl;
		if (str != nullptr)
		{
			m_data = new char[strlen(str) + 1];
			strcpy(this->m_data, str);
		}
		else 
		{
			m_data = new char[1];
			*m_data = '\0'; //保证底层有内存
		}
	}
	//拷贝构造
	String(const String& other)
	{
		cout << this << "拷贝构造" << endl;
		m_data = new char[strlen(other.m_data) + 1];
		strcpy(m_data, other.m_data);
	}
	//构析
	~String(void)
	{
		delete[]m_data;
		m_data = nullptr;
	}
	//赋值函数
	String& operator = (const String & other) //返回值产生临时变量,返回引用开销小
	{
		if (this == &other)
		{
			return *this; //赋完值把当前对象返回回去
		}
		cout << this << "赋值重载" << endl;
		delete[]m_data;
		m_data = new char[strlen(other.m_data) + 1];
		strcpy(m_data, other.m_data);
		return *this;
	}
private:
	//保存字符串
	char* m_data;
};
int main()
{
	//带const char*的构造
	String str1;
	String str2("hello");
	String str3 = "world";

	//拷贝构造
	String str4 = str3;
	String str5(str3);

	//赋值重载
	str1 = str2;

	return 0;
}
```

```cpp
//循环队列
#include <iostream>

using namespace std;

class Queue
{
public:
	Queue(int size = 20)
	{
		_pQue = new int[size];
		_front = _rear = 0;
		_size = size;
	}
	//手写深拷贝
	Queue(const Queue& src)
	{
		_size = src._size;
		_front = src._front;
		_rear = src._rear;
		_pQue = new int[_size];
		for (int i = _front; i != _rear; i = (i + 1) % _size)
		{
			_pQue[i] = src._pQue[i];
		}
	}
	~Queue()
	{
		delete[]_pQue;
		_pQue = nullptr;
	}
	void push(int val)
	{
		if (full())
			resize();
		_pQue[_rear] = val;
		_rear = (_rear + 1) % _size;
	}
	void pop()
	{
		if (empty())
			return;
		_front = (_front + 1) % _size;
	}
	int front() //获取队头元素
	{
		return _pQue[_front];
	}
	bool full()
	{
		return (_rear + 1) % _size == _front;
	}
	bool empty()
	{
		return _front == _rear;
	}
	void resize()
	{
		int index = 0;
		int* ptmp = new int[2 * _size];
		for (int i = _front; i != _rear; i = (i + 1) % _size)
		{
			ptmp[index++] = _pQue[i];
		}
		delete[]_pQue;
		_pQue = ptmp;
		_front = 0;
		_rear = index;
		_size *= 2;
	}
private:
	int* _pQue; //申请队列的数组空间
	int _front; //指示队头的位置
	int _rear; //指示队尾的位置
	int _size; //队列扩容的总大小
};

int main()
{
	Queue queue;
	for (int i = 0; i < 30; i++)
	{
		queue.push(rand() % 100);
	}
	while (!queue.empty())
	{
		cout << queue.front() << ' ';
		queue.pop();
	}
	cout << endl;

	Queue q2 = queue;

	return 0;
}
```

**有指针指向==类外资源==时深拷贝**



## 5. 构造函数的初始化列表

成员对象

```cpp
#include <iostream>

using namespace std;

class CDate
{
public:
	CDate(int y, int m, int d)
	{
		_year = y;
		_month = m;
		_day = d;
	}
	void show()
	{
		printf("%d/%d/%d\n", _year, _month, _day);
	}
private:
	int _year;
	int _month;
	int _day;
};

class CGoods
{
public:
	CGoods(const char* name, int price, int y, int m, int d):
		_date(y, m, d)//没有默认构造函数，只能这样。写在里面要先默认。
		,_price(price) //int _price = price;
	{
		strcpy(_name, name);
		_price = price; //int _price; _price = price;
	}
	void show()
	{
		printf("%s %d\n", _name, _price);
		_date.show();
	}
private:
	char _name[20];
	int _price;
	CDate _date;
};

int main()
{
	CGoods good("牛奶",100,2022,11,11);
	good.show();
	return 0;
}
```

```cpp
#include <iostream>

using namespace std;

class Test
{
public:
	Test(int data = 10) : mb(data),ma(mb){}
	void show() { cout << "ma: " << ma << " mb: " << mb << endl; }
private:
	int ma; //按定义的顺序初始化
	int mb;
};

int main()
{
	Test t;
	t.show(); //ma: -858993460(0xCCCCCCCC) mb : 10
	return 0;
}
```



## 6. 类的各类成员方法以及区别

普通的成员方法 =》编译器会添加一个 this 形参变量

1. 属于类的作用域
2. 调用该方法时，需要依赖一个对象
3. 可以任意访问对象的私有成员变量

static静态成员方法 =》 不会生成 this 形参

1. 属于类的作用域
2. 用类名作用域来调用方法
3. 可以任意访问对象的私有对象，仅限于不依赖对象的成员（只能调用其它的static静态成员）

const常成员方法 =》 const CGoods *this

1. 属于类的作用域
2. 调用依赖一个对象，普通对象或者常对象都可以
3. 可以任意访问对象的私有成员，但是只能读不能写

```cpp
#include <iostream>

using namespace std;

class Test
{
public:
	Test()
	{
		cnt++;
	}
	static int staticgetCnt() //静态成员方法，调用时不需要this指针
	{
		return cnt;
	}
	//常成员方法 只要是只读操作，一律实现为const方法
	int getCnt() const
	{
		return cnt;
	}
private:
	static int cnt; //声明 不属于对象，属于类级别的
};

int Test::cnt = 0;

int main()
{
	Test t1, t2;
	cout << t1.getCnt() << endl;
	cout << Test::staticgetCnt() << endl; //直接用类的作用域调
	const Test t3; //常对象无法调用普通方法，因为传的形参无法从const转为普通
	cout << t3.getCnt() << endl;
	return 0;
}
```



## 7.指向类成员的指针

```cpp
#include <iostream>

using namespace std;

class Test
{
public:
	void func() { cout << "call Test::func" << endl; }
	static void static_func() { cout << "static_func" << endl; }
	int ma;
	static int mb;
};

int Test::mb; 

int main()
{
	Test t1;
	Test* t2 = new Test();
	int Test::* p = &Test::ma; //类的指针

	t1.*p = 20;
	cout << t1.ma << endl;

	t2->*p = 30;
	cout << t2->*p << endl;

	int* p1 = &Test::mb; //不依赖于对象
	*p1 = 40;
	cout << t1.mb << endl;

	//指向成员方法的指针
	void(Test::*pfunc)() = &Test::func;
	(t1.*pfunc)(); //要依赖于对象调用

	//指针指向类的static成员方法
	void(*pfunc2)() = &Test::static_func;
	(*pfunc2)();

	delete t2;
	return 0;
}
```



# 第四章 模板编程

## 1. 理解函数模板

模板只定义，不编译。如果将其放到另外一个文件中，如果没有对应的特例化将出错。

所以模板代码都是放在头文件中，然后在源文件中 #include 包含。

```cpp
#include <iostream>

using namespace std;

template<typename T> //typename 或 class
bool compare(T a, T b) //compare是一个函数模板 是无法编译的
{
	cout << "template compare" << endl;
	return a > b;
}

//模板特例化(比如比较字符串大小,原方案只能比较地址大小)
template<>
bool compare<const char*>(const char* a, const char* b)
{
	cout << "const char * compare" << endl;
	return strcmp(a, b) > 0;
}

//非模板函数   
bool compare(const char* a, const char* b)
{
	cout << "非模板函数" << endl;
	return true;
}

/*
在函数调用点,编译器用用户指定的类型,从原模板实例化一份函数代码出来
*/
int main()
{
	cout << compare<int>(1, 2) << endl; //函数的调用点
	cout << compare<double>(1.2, 1.1) << endl;
	cout << compare(2, 3) << endl; //模板的实参推演 
	//compare(10, 10.1);  //error 推导不出类型
	cout << compare<int>(10, 10.1) << endl; //强制转化 或 模板函数那里两个T
	cout << compare("aaa", "bbb") << endl; //优先普通函数
	cout << compare<const char*>("aaa", "bbb") << endl; //强制模板函数
	return 0;
}
```



## 2. 理解类模板 

演示下 template 用 T 以外的其它参数

可以加默认 T 参数。 但是声明时 `<>` 要写

```cpp
#include<iostream>
using namespace std;

template<typename T, int SIZE>
void sort(T* arr) {}

int main()
{
	int arr[] = { 1,2,5,4,3 };
	const int size = sizeof(arr) / sizeof(arr[0]);
	sort<int, size>(arr);
	return 0;
}
```

构造和析构函数名不用加`<T>`，其它出现模板的地方都要加上类型参数列表。

类模板无非就是将存数据的类型改为 T

```cpp
template<typename T>
class SeqStack
{
public:
	void push(const T& val);
};

template<typename T>
void SeqStack<T>::push(const T& val)
{

}
```



## 3. 实现STL向量容器vector代码

```cpp
#include <iostream>

using namespace std;

/*
实现 vector 向量容器
*/
template<typename T>
class vector
{
public:
	vector(int size = 10)
	{
		_first = new T[size];
		_last = _first;
		_end = _first + size;
	}
	~vector()
	{
		delete[]_first;
		_first = _last = _end = nullptr;
	}
	vector(const vector<T>& rhs)
	{
		int size = rhs._end - rhs._first;
		_first = new T[size];
		int len = rhs._last - rhs._first;
		for (int i = 0; i < len; ++i)
		{
			_first[i] = rhs._first[i];
		}
		_last = _first + len;
		_end = _first + size;
	}
	vector<T>& operator=(const vector<T>& rhs)
	{
		if (this == &rhs)
			return *this;
		delete[]_first;
		int size = rhs._end - rhs._first;
		_first = new T[size];
		int len = rhs._last - rhs._first;
		for (int i = 0; i < len; ++i)
		{
			_first[i] = rhs._first[i];
		}
		_last = _first + len;
		_end = _first + size;
		return *this;
	}
	void push_back(const T &val)
	{
		if (full())
			expand();
		*_last++ = val;
	}
	void pop_back()
	{
		if (empty())
			return;
		--_last;
	}
	T back() const
	{
		return *(_last - 1); //空的情况没写
	}
	bool full() const { return _last == _end; }
	bool empty() const { return _first == _last; }
	int size() const { return _last - _first; }
private:
	T* _first; //起始
	T* _last; //有效元素的后继位置
	T* _end; //数组空间的后继位置
	void expand() 
	{
		int size = _end - _first;
		T* ptmp = new T[2 * size];
		for (int i = 0; i < size; i++)
		{
			ptmp[i] = _first[i];
		}
		delete[]_first;
		_first = ptmp;
		_last = _first + size;
		_end = _first + size * 2;
	}
};


int main()
{
	vector<int>vec;
	for (int i = 0; i < 20; i++)
		vec.push_back(rand() % 100);
	while (!vec.empty())
	{
		cout << vec.back() << ' ';
		vec.pop_back();
	}
	return 0;
}
```



## 4. 理解容器空间配置器allocator的重要性

上个代码存在以下问题：

构造：需要把内存开辟和对象构造分开处理

析构：析构容器有效的元素，然后释放_first指针指向的堆内存

pop_back：只需要析构对象。要把对象的析构和内存释放分开

----

容器的空间配置器 allocator 做四件事 =》内存开辟/内存释放	对象构造/对象析构

```cpp
#include <iostream>

using namespace std;

/*
容器的空间配置器
*/
template<typename T>
class Allocator
{
public:
	T* allocate(size_t size) //负责内存开辟
	{
		return (T*)malloc(sizeof(T) * size);
	}
	void deallocate(void* p) //负责内存释放
	{
		free(p);
	}
	void construct(T* p, const T& val) //负责对象构造
	{
		new (p) T(val); //定位new  指定内存上构造
	}
	void destroy(T* p) //负责对象析构
	{
		p->~T(); //~T()代表了T类型的析构函数
	}
}
;


/*
实现 vector 向量容器
容器底层内存开辟,内存释放,对象构造和析构,都通过allocator空间配置器来实现
*/
template<typename T, typename Alloc = Allocator<T>>
class vector
{
public:
	vector(int size = 10)
	{
		//需要把内存开辟和对象构造分开处理
		//_first = new T[size];
		_first = _allocator.allocate(size);
		_last = _first;
		_end = _first + size;
	}
	~vector()
	{
		//delete[]_first;
		for (T* p = _first; p != _last; p++)
		{
			_allocator.destroy(p); //把_first指针指向的有效元素析构
		}
		_allocator.deallocate(_first); //释放堆上的内存
		_first = _last = _end = nullptr;
	}
	vector(const vector<T>& rhs)
	{
		int size = rhs._end - rhs._first;
		//_first = new T[size];
		_first = _allocator.allocate(size);
		int len = rhs._last - rhs._first;
		for (int i = 0; i < len; ++i)
		{
			//_first[i] = rhs._first[i];
			_allocator.construct(_first + 1, rhs._first[i]);
		}
		_last = _first + len;
		_end = _first + size;
	}
	vector<T>& operator=(const vector<T>& rhs)
	{
		if (this == &rhs)
			return *this;
		//delete[]_first;
		for (T* p = _first; p != _last; p++)
		{
			_allocator.destroy(p); //把_first指针指向的有效元素析构
		}
		_allocator.deallocate(_first);

		int size = rhs._end - rhs._first;
		//_first = new T[size];
		_first = _allocator.allocate(size);
		int len = rhs._last - rhs._first;
		for (int i = 0; i < len; ++i)
		{
			//_first[i] = rhs._first[i];
			_allocator.construct(_first + 1, rhs._first[i]);
		}
		_last = _first + len;
		_end = _first + size;

		return *this;
	}
	void push_back(const T &val)
	{
		if (full())
			expand();
		//*_last++ = val;
		_allocator.construct(_last, val);
		_last++;
	}
	void pop_back()
	{
		if (empty())
			return;
		//--_last;
		//析构删除的元素
		--_last;
		_allocator.destroy(_last);
	}
	T back() const
	{
		return *(_last - 1); //空的情况没写
	}
	bool full() const { return _last == _end; }
	bool empty() const { return _first == _last; }
	int size() const { return _last - _first; }
private:
	T* _first; //起始
	T* _last; //有效元素的后继位置
	T* _end; //数组空间的后继位置
	Alloc _allocator; //定义容器的空间配置器对象

	void expand() 
	{
		int size = _end - _first;
		//T* ptmp = new T[2 * size];
		T* ptmp = _allocator.allocate(2 * size);
		for (int i = 0; i < size; i++)
		{
			_allocator.construct(ptmp + i, _first[i]);
			//ptmp[i] = _first[i];
		}
		//delete[]_first;
		for (T* p = _first; p != _last; ++p)
		{
			_allocator.destroy(p);
		}
		_allocator.deallocate(_first);
		_first = ptmp;
		_last = _first + size;
		_end = _first + size * 2;
	}
};

class Test
{
public:
	Test() { cout << "Test()" << endl; }
	~Test() { cout << "~Test()" << endl; }
	Test(const Test&) { cout << "Test(const Test&)" << endl; }
};


int main()
{
	Test t1, t2, t3;
	cout << "----" << endl;
	vector<Test>vec;
	vec.push_back(t1);
	vec.push_back(t2);
	vec.push_back(t3);
	cout << "----" << endl;
	vec.pop_back();
	cout << "----" << endl;
	return 0;
}
```





# 第五章 运算符重载

C++运算符重载：使对象的运算表现得和编译器内置类型一样。

## 1. 学习复数类CComplex

编译器做对象运算的时候，会调用对象的运算符重载函数（有限调用成员方法）；如果没有成员方法，将在全局作用域找合适的运算符重载函数。

```cpp
#include <iostream>

using namespace std;

class CComplex
{
public:
	CComplex(int r = 0, int i = 0)
		:mreal(r), mimage(i){}
	//指导编译器怎么做CComplex类对象的加法操作
	CComplex operator+(const CComplex& src)
	{
		//CComplex comp;
		//comp.mreal = this->mreal + src.mreal;
		//comp.mimage = this->mimage + src.mimage;
		//return comp;
		return CComplex(this->mreal + src.mreal,
			this->mimage + src.mimage);
	}
	//i++
	CComplex operator++(int)
	{
		//CComplex comp = *this;
		//mreal += 1;
		//mimage += 1;
		//return comp;
		return CComplex(mreal++, mimage++);
	}
	//++i
	CComplex& operator++()
	{
		mreal += 1;
		mimage += 1;
		return *this;
	}
	void show()
	{
		cout << "real: " << mreal << " image: " << mimage << endl;
	}
private:
	int mreal;
	int mimage;
	friend CComplex operator+(const CComplex& lhs, const CComplex& rhs);
	friend ostream& operator<<(ostream& out, const CComplex& src);
	friend istream& operator>>(istream& in, CComplex& src);
};

ostream& operator<<(ostream& out, const CComplex& src)
{
	out << "mreal: " << src.mreal << " mimage " << src.mimage;
	return out;
}

istream& operator>>(istream& in, CComplex& src)
{
	cout << "start in" << endl;
	in >> src.mreal >> src.mimage;
	return in;
}

CComplex operator+(const CComplex& lhs, const CComplex& rhs)
{
	return CComplex(lhs.mreal + rhs.mreal, lhs.mimage + rhs.mimage);
}

int main()
{
	CComplex comp1(10, 10);
	CComplex comp2(20, 20);
	//comp1.operator+(comp2) 加法运算符的重载函数
	CComplex comp3 = comp1 + comp2;
	CComplex comp4 = comp1.operator+(comp2);
	comp3.show();
	comp4.show();
	CComplex comp5 = comp1 + 20; //20 寻找构造函数 是因为 "+" 被激活
	comp5.show();
	CComplex comp6 = 30 + comp1;  //调不了成员方法,调全局
	comp6.show();

	comp5 = comp1++;
	comp1.show();
	comp5.show();
	comp5 = ++comp1;
	comp1.show();
	comp5.show();

	cout << comp4 << endl;

	CComplex comp8;
	cin >> comp8;
	cout << comp8;
	return 0;
}
```



## 2. 模拟实现C++的string类代码

```cpp
#include <iostream>

using namespace std;

class String
{
public:
	String(const char* p = nullptr)
	{
		if (p != nullptr)
		{
			_pstr = new char[strlen(p) + 1];
			strcpy(_pstr, p);
		}
		else 
		{
			_pstr = new char[1];
			*_pstr = '\0';
		}
	}
	~String()
	{
		delete[]_pstr;
		_pstr = nullptr;
	}
	String(const String& str)
	{
		_pstr = new char[strlen(str._pstr) + 1];
		strcpy(_pstr, str._pstr);
	}
	String& operator=(const String& src)
	{
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = new char[strlen(src._pstr) + 1];
		strcpy(_pstr, src._pstr);
		return *this; 
	}
	bool operator>(const String& str) const
	{
		return strcmp(_pstr, str._pstr) > 0;
	}
	bool operator<(const String& str) const
	{
		return strcmp(_pstr, str._pstr) < 0;
	}
	bool operator==(const String& str) const
	{
		return strcmp(_pstr, str._pstr) == 0;
	}
	int length()
	{
		return strlen(_pstr);
	}
	char& operator[](int index)
	{
		return _pstr[index];
	}
	const char& operator[](int index) const
	{
		return _pstr[index];
	}
	const char* c_str() const { return _pstr; }
private:
	char* _pstr;
	friend ostream& operator<<(ostream& out, const String& str);
	friend String operator+(const String& lhs, const String& rhs);
};

String operator+(const String& lhs, const String& rhs)
{
	String tmp;
	tmp._pstr = new char[strlen(lhs._pstr) + strlen(rhs._pstr) + 1];
	strcpy(tmp._pstr, lhs._pstr);
	strcat(tmp._pstr, rhs._pstr);
	return tmp; 
}

ostream& operator<<(ostream& out, const String& str)
{
	out << str._pstr;
	return out;
}

int main()
{
	String s1;
	String s2 = "aa";
	String s3 = s2;
	String s4 = s3 + "bb";
	String s5 = "cc" + s2;
	cout << s4 << endl;
	cout << s5 << endl;
	cout << (s3 < s4) << endl;
	return 0;
}
```



## 3. string字符串对象的迭代器iterator实现

```cpp
#include <iostream>

using namespace std;

class String
{
public:
	String(const char* p = nullptr)
	{
		if (p != nullptr)
		{
			_pstr = new char[strlen(p) + 1];
			strcpy(_pstr, p);
		}
		else 
		{
			_pstr = new char[1];
			*_pstr = '\0';
		}
	}
	~String()
	{
		delete[]_pstr;
		_pstr = nullptr;
	}
	String(const String& str)
	{
		_pstr = new char[strlen(str._pstr) + 1];
		strcpy(_pstr, str._pstr);
	}
	String& operator=(const String& src)
	{
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = new char[strlen(src._pstr) + 1];
		strcpy(_pstr, src._pstr);
		return *this; 
	}
	bool operator>(const String& str) const
	{
		return strcmp(_pstr, str._pstr) > 0;
	}
	bool operator<(const String& str) const
	{
		return strcmp(_pstr, str._pstr) < 0;
	}
	bool operator==(const String& str) const
	{
		return strcmp(_pstr, str._pstr) == 0;
	}
	int length()
	{
		return strlen(_pstr);
	}
	char& operator[](int index)
	{
		return _pstr[index];
	}
	const char& operator[](int index) const
	{
		return _pstr[index];
	}
	const char* c_str() const { return _pstr; }

	//给string提供迭代器的实现
	class iterator
	{
	public:
		iterator(char*p = nullptr):_p(p){}
		bool operator !=(const iterator& it)
		{
			return _p != it._p;
		}
		void operator++()
		{
			++_p;
		}
		char& operator*()
		{
			return *_p;
		}
	private:
		char* _p;
	};
	//首元素迭代器的表示
	iterator begin() 
	{
		return iterator(_pstr);
	}
	iterator end()
	{
		return iterator(_pstr + length());
	}

private:
	char* _pstr;
	friend ostream& operator<<(ostream& out, const String& str);
	friend String operator+(const String& lhs, const String& rhs);
};

String operator+(const String& lhs, const String& rhs)
{
	String tmp;
	tmp._pstr = new char[strlen(lhs._pstr) + strlen(rhs._pstr) + 1];
	strcpy(tmp._pstr, lhs._pstr);
	strcat(tmp._pstr, rhs._pstr);
	return tmp; 
}

ostream& operator<<(ostream& out, const String& str)
{
	out << str._pstr;
	return out;
}

int main()
{
	String str1 = "Hello World";
	String::iterator it = str1.begin();
	for (; it != str1.end(); ++it)
	{
		cout << *it;
	}
	cout << endl;

	for (char ch : str1) //begin() 和 end()实现
	{
		cout << ch; 
	}
	return 0;
}
```



## 4.vector容器的迭代器iterator实现

泛型算法：参数接收的都是容器的迭代器

内置类

```cpp
class iterator
{
    public:
    iterator(T*ptr = nullptr)
        :_ptr(ptr){}
    bool operator !=(const iterator& it) const
    {
        return _ptr != it._ptr;
    }
    //前置++  即++it
    void operator++()
    {
        _ptr++;
    }
    T& operator*() { return *_ptr; }
    const T& operator*() const { return *_ptr; }
    private:
    T* _ptr;
};
iterator begin()
{
    return iterator(_first);
}
iterator end()
{
    return iterator(_last);
}
```



## 5. 什么是容器的迭代器失效问题

迭代器在 erase 后失效；迭代器在 insert 之后失效

**失效原因**

1. 当容器调用 erase 后，当前位置到容器末尾元素的所有的迭代器全部失效了
2. 当容器调用 insert 后，当前位置到容器末尾元素的所有的迭代器全部失效了。
3. 如果 insert 引起扩容，原来容器的所有的迭代器全部失效。
4. 不同容器的迭代器不能进行比较运算 

**解决方案**

对删除/插入点的迭代器进行更新操作。

```cpp
//删除
auto it = a.begin();
while (it != a.end())
{
    if (*it % 2 == 0)
        it = a.erase(it); //更新删除位置的迭代器
    else
        ++it;
}
```

```cpp
//插入
auto it = a.begin();
while (it != a.end())
{
    if (*it % 2 == 0)
    {
        it = a.insert(it, *it - 1); //要后移两个
        ++it; 
    }
    ++it;
}
```



## 6. 输入理解new和delete的原理

**malloc 和 new 的区别？**

1. malloc 按字节开辟内存；new 开辟内存时需要指定类型

   所以malloc 开辟内存返回的都是 void*

2. malloc 只负责开辟空间，new 还可以进行数据的初始化

3. malloc 开辟内存失败返回 nullptr 指针，new 抛出的是 bad_alloc 类型的异常



**free 和 delete 区别？**

delete：调用析构函数

------

new 和 delete 内部用 malloc 和 free 实现。

**C++为什么区分单个元素和数组的内存分配和释放呢？**

 对于普通的变量，没有析构，可以不按规范来

```cpp
int *a = new int;
delete[]a;
```

自定义的类类型，有析构函数。数组时会多开4个来存个数。

当开辟数组时，会将对象的个数存下来，但是返回给用户的是开始存对象的地址（比存个数的+4）



## 7. new和delete重载实现的对象池应用

运算符重载：成员方法、全局方法

内存池 进程池 线程池 对象池

解决短时间内频繁调用同一区域的 malloc free

```cpp
#include <iostream>
//#include <string>

using namespace std;

template<typename T>
class Queue
{
public:
	Queue()
	{
		_front = _rear = new QueueItem();
	}
	~Queue()
	{
		QueueItem* cur = _front;
		while (cur != nullptr)
		{
			_front = _front->_next;
			delete cur;
			cur = _front;
		}
	}
	void push(const T& val)
	{
		QueueItem *item = new QueueItem(val);
		_rear->_next = item;
		_rear = item;
	}
	void pop()
	{
		if (empty())
			return;
		QueueItem* first = _front->_next;
		_front->_next = first->_next;
		if (_front->_next == nullptr)
		{
			_rear = _front;
		}
		delete first;
	}
	T front() const
	{
		return _front->_next->_data;
	}
	bool empty() const { return _front == _rear; }
private:
	struct QueueItem //节点
	{
		QueueItem(T data = T()):_data(data), _next(nullptr){}
		//给QueueItem提供自定义内存管理
		void* operator new(size_t size)
		{
			if (_itemPool == nullptr) //用满了itemPool就是nullptr了
			{
				_itemPool = (QueueItem*)new char[POOL_ITEM_SIZE * sizeof(QueueItem)];
				QueueItem* p = _itemPool;
				for (; p < _itemPool + POOL_ITEM_SIZE - 1; ++p) //最后一个是nullptr
				{
					p->_next = p + 1;
				}
				p->_next = nullptr;
			}

			QueueItem* p = _itemPool;
			_itemPool = _itemPool->_next;
			return p;
		}
		void operator delete(void* ptr)
		{
			QueueItem* p = (QueueItem*)ptr;
			p->_next = _itemPool;
			_itemPool = p;
		}
		T _data;
		QueueItem* _next;
		static QueueItem* _itemPool; 
		static const int POOL_ITEM_SIZE = 1000000;
	};

	QueueItem* _front; //头
	QueueItem* _rear; //尾
};

template<typename T>
typename Queue<T>::QueueItem *Queue<T>::QueueItem::_itemPool = nullptr;

int main()
{
	Queue<int> que;
	for (int i = 0; i < 10000000; ++i)
	{
		que.push(i);
		que.pop();
	}
	cout << que.empty() << endl;
	return 0;
}
```



# 第六章 继承与多态

## 1. 继承的基本意义

继承的本质：a. 代码的复用 b. 

类和类之间的关系：

组合：a part of ...一部分的关系

继承：a kind of ...一种的关系

1. 基类的成员的访问限定，在派生类里面是不可能超过继承方式的

2. protected 在派生（public、protected）类里可以被访问

3. private只有自己和友元可以访问
4. 外部只能访问 public 

![image-20221113161322108](C:\Users\lemer\AppData\Roaming\Typora\typora-user-images\image-20221113161322108.png)

继承来源于自己的上一级



## 2. 派生类的构造过程

派生类的构造函数不能派生类自己去初始化它。

派生类从基类可以从基类继承来所有的成员（变量和方法），除构造函数和析构函数（这两个继承了也没有意义）



派生类怎么初始化从基类继承来的成员变量呢？

答：通过调用基类相应的构造函数来初始化



派生类对象构造和析构的过程是：

1. 派生类调用基类的构造函数，初始化从基类继承来的成员
2. 调用派生类自己的构造函数
3. 调用派生类的构析
4. 调用基类的构析

```cpp
#include<iostream>

using namespace std;

class Base
{
public:
	Base(int data)
	{
		y = data;
		cout << "Base()" << endl;
	}
	~Base()
	{
		cout << "~Base()" << endl;
	}
	int y;
};

class Derive : public Base
{
public:
	Derive(int data) :
		Base(data), x(data)
	{
		cout << "Device()" << endl;
	}
	~Derive()
	{
		cout << "~Device()" << endl;
	}
	void show()
	{
		cout << "xy " << x << " " << y << endl;
	}
private:
	int x;
};

int main()
{
	Derive x(1);
	x.show();
	return 0;
}

/*
Base()
Device()
xy 1 1
~Device()
~Base()
*/
```



## 3. 重载、隐藏、覆盖

子类可以调用基类的函数，但是一旦子类自定义同名函数，父类的该函数名不能被用。



**1.重载关系**

一组函数要重载，必须处在**同一个作用域**当中；并且函数名字相同，参数列表不同

**2.隐藏（作用域隐藏）关系**

在继承结构当中，派生类的同名成员，把基类的同名成员给隐藏调用了

**3.覆盖关系**

基类和派生类的方法，返回值、函数名以及参数列表都相同，而且基类的方法是虚函数，那么派生类的方法就自动处理成虚函数，它们之间成为覆盖关系。



基类对象 -> 派生类对象 N

派生类对象 -> 基类对象 Y

基类指针（引用） -> 派生类对象 N

派生类指针（引用） -> 基类对象 Y



**在继承结构中进行上下的类型转换，默认只支持从下到上的转换**

```cpp
#include<iostream>

using namespace std;

class Base
{
public:
	Base(int data = 10) : ma(data) {}
	void show() { cout << "Base::show()" << endl; }
	void show(int) { cout << "Base::show(int)" << endl; }
protected:
	int ma;
};

class Derive : public Base
{
public:
	Derive(int data = 20):Base(data), mb(data){}
	void show() { cout << "Derive::show()" << endl; }
private:
	int mb;
};

int main()
{
	Base b;
	Derive d;
	//d.show();
	//d.show(10); //error
	//d.Base::show();
	
	//基类 <- 派生 类型从下到上 Y
	//b = d;
	//派生 <- 基类 类型从上到下 N
	//d = b;

	//基类指针(引用) <- 派生类对象 类型从下到上 Y
	Base* pb = &d; //指针的类型的基类,限制了指针访问的内容只是派生类里面基类的内容
	pb->show();
	//((Derive*)pb)->show();  //非常危险 涉及内存的非法访问
	pb->show(10); 

	//派生类指针（引用）<- 基类对象 类型从上到下 N
	//指针解引用后非法内存越界访问
	//Derive* pd = &b; //error

	return 0;
}
```



## 4. 虚函数、静态绑定和动态绑定

**静态绑定**

```cpp
#include<iostream>
#include <typeinfo>

using namespace std;

class Base
{
public:
	Base(int data = 10) : ma(data) {}
	void show() { cout << "Base::show()" << endl; }
	void show(int) { cout << "Base::show(int)" << endl; }
protected:
	int ma;
};

class Derive : public Base
{
public:
	Derive(int data = 20):Base(data), mb(data){} //给基类继承来的初始化下
	void show() { cout << "Derive::show()" << endl; }
	void getma() { cout << "ma " << ma << endl; }
private:
	int mb;
};

int main()
{
	Derive d(50);
	Base* pb = &d;
	pb->show(); //静态(编译时期)的绑定(函数的调用) call Base::show(010112DAh)
	pb->show(10); //静态绑定 call Base::show (010112B2h)
	
	cout << sizeof(Base) << endl << sizeof(Derive) << endl; 
	cout << typeid(pb).name() << endl;
	cout << typeid(*pb).name() << endl;

	return 0;
}

/*
Base::show()
Base::show(int)
4
8
class Base *
class Base
*/
```

**动态绑定(虚函数)**

```cpp
#include<iostream>
#include <typeinfo>

using namespace std;
/*
一个类添加了虚函数,对这个类有什么影响?
总结一
一个类里面定义了虚函数,那么编译阶段,编译器给这个类类型产生
一个唯一的vftable虚函数表,虚函数表中主要存储的内容就是RTTI指针和虚函数的地址
当程序运行时,每一张虚函数表都会加载到.rodata区(只读 不能写)

总结二
一个类里面定义了虚函数,那么这个类定义的对象,其运行时,内存中开始部分,
多存储一个vfptr虚函数指针,指向相应类型的虚函数表vftable。
一个类型定义的n个对象，它们的额外vfptr指向的都是同一张虚函数表

总结三
一个类里面虚函数的个数，不影响对象内存的大小（vfptr），影响的是虚函数表的大小
*/
#if 1
class Base
{
public:
	Base(int data = 10) : ma(data) {}
	virtual void show() { cout << "Base::show()" << endl; }
	virtual void show(int) { cout << "Base::show(int)" << endl; }
protected:
	int ma;
};

class Derive : public Base
{
public:
	Derive(int data = 20):Base(data), mb(data){} //给基类继承来的初始化下
	/*
	总结四
	如果派生类中的方法，和基类继承来的某个方法，
	返回值、函数名、参数列表都相同，
	而且基类的方法是virtual虚函数，
	那么这个派生类的这个方法，自动处理为虚函数
	重写《=》覆盖
	*/
	void show() { cout << "Derive::show()" << endl; }
private:
	int mb;
};

int main()
{
	Derive d(50);
	Base* pb = &d; //vfptr指针 + ma ,8个字节

	/*
	如果发现show()是普通函数，就静态绑定
	如果发现show()是虚函数，就进行动态绑定
	*/
	/*
	006A294F  mov         ecx,dword ptr [pb]     vfptr
	006A2952  mov         eax,dword ptr [edx+4]  虚函数表
	006A2955  call        eax
	*/
	//需要在编译时确定调用哪个函数
	pb->show(); //动态的绑定
	pb->show(10); //动态绑定 虽然没重写
	
	cout << sizeof(Base) << endl << sizeof(Derive) << endl;
	cout << typeid(pb).name() << endl;
	//Base有虚函数,*pb实现的是运行时期的类型
	cout << typeid(*pb).name() << endl; 

	return 0;
}

#endif
```



## 5. 虚构析函数

**哪些函数不能实现成虚函数？**

1. 构造函数不能用 virtual
2. 构造函数中调用的任何函数都是静态绑定，不会发生动态绑定
3. static 静态，因为static 不依赖对象



**虚函数依赖：**

1. 虚函数能产生地址，存储在vftable当中
2. 对象必须存在（vfptr -> vftable -> 虚函数地址）



**虚析构函数**

析构函数可以成为虚函数。因为析构函数调用时对象存在。

基类的指针指向堆上 new 出来的派生类对象时，它调用析构函数的时候必须发生动态绑定，否则会导致派生类的析构无法调用。

```cpp
#include <iostream>
#include <typeinfo>

using namespace std;
class Base
{
public:
	Base(int data)
	{
		ma = data;
		cout << "Base()" << endl;
	}
	~Base()
	{
		cout << "~Base()" << endl;
	}
	virtual void show() 
	{
		cout << "Base::show()" << endl;
	}
private:
	int ma;
};

class Derive : public Base
{
public:
	Derive(int data) :
		Base(data), mb(data)
	{
		cout << "Device()" << endl;
	}
	~Derive()
	{
		cout << "~Device()" << endl;
	}
	void show()
	{
		cout << "Derive::show()" << endl;
	}
private:
	int mb;
};

int main()
{
	Base* pb = new Derive(10);
	pb->show(); //派生类的析构函数没有被调用

	delete pb;
	return 0;
}
/*
Base()
Device()
Derive::show()
~Base()
*/
```

pb 的类型是 Base，析构函数是普通函数，静态绑定。

解决方案：将基类的析构函数变为虚函数。

```cpp
virtual ~Base()
{
    cout << "~Base()" << endl;
}
```

再跑一遍，结果为

```cpp
Base()
Device()
Derive::show()
~Device()
~Base()
```



## 6. 再谈动态绑定

是不是虚函数的调用一定就是动态绑定？ 不是

1. 在类的构造函数中，调用虚函数，也是静态绑定（构造函数中不会发生动态）
2. 如果不是通过指针或引用变量来调用虚函数，那就是静态绑定



```cpp
#include <iostream>
#include <typeinfo>

using namespace std;
class Base
{
public:
	Base(int data = 10)
	{
		ma = data;
		cout << "Base()" << endl;
	}
	virtual ~Base()
	{
		cout << "~Base()" << endl;
	}
	virtual void show() 
	{
		cout << "Base::show()" << endl;
	}
private:
	int ma;
};

class Derive : public Base
{
public:
	Derive(int data = 10) :
		Base(data), mb(data)
	{
		cout << "Device()" << endl;
	}
	~Derive()
	{
		cout << "~Device()" << endl;
	}
	void show()
	{
		cout << "Derive::show()" << endl;
	}
private:
	int mb;
};

int main()
{
	Base b;
	Derive d;

	//不涉及前四个字节的指针
	//静态绑定 用对象本身调用虚函数，是静态绑定
	b.show(); //虚函数  call Base::show (06B1451h)
	d.show(); //虚函数

	//move move call 动态绑定（必须由指针调用虚函数）
	Base* pb1 = &b;
	pb1->show();
	Base* pb2 = &d;
	pb2->show();

	//仍然是动态绑定
	Base& rb1 = b;
	rb1.show();
	Base& rb2 = d;
	rb2.show();

	//仍然是动态绑定
	Derive* pd1 = &d;
	pd1->show();
	Derive& rd1 = d;
	rd1.show();

	//流氓强转类型
	Derive* pd2 = (Derive*)&b; //只能访问基类的表
	pd2->show(); //b的里面只有Base的函数

	return 0;
}
```



## 7. 理解多态到底是什么

静态（编译时期）的多态：函数重载、模板（函数模板和类模板）

动态（运行时期）的多态：在继承结构中，基类指针（引用）指向派生类对象，通过该指针（引用）调用同名覆盖 方法（**虚函数**）。**基类指针**指向哪个派生类对象，就会调用哪个派生类对象的同名覆盖方法，称为多态。

多态底层是通过动态绑定来实现的。pbase 访问谁的 vfptr 就继续访问谁的 vftable，当然调用的是对应的派生类对象的方法了。

软件设计“开-闭”原则：对修改关闭，对拓展开放



继承的好处

1. 代码的复用
2. 在基类中提供统一的虚函数接口，让派生类进行重写，然后就可以使用多态了

```cpp
#include <iostream>
#include <string>

using namespace std;

class Animal
{
public:
	Animal(string name):_name(name){}
	virtual void bark(){}
protected:
	string _name;
};

class Cat : public Animal
{
public:
	Cat(string name):Animal(name){}
	void bark() {
		cout << "miao" << endl;
	}
};

class Dog : public Animal
{
public:
	Dog(string name) :Animal(name) {}
	void bark() { cout << "wang" << endl; }
};

void bark(Animal* p)
{
	p->bark(); //虚函数 动态绑定
}

int main()
{
	Cat cat("加菲");
	Dog dog("二哈");

	bark(&cat);
	bark(&dog);
	return 0;
}
```



## 8. 理解抽象类

拥有纯虚函数的类叫做抽象类
抽象类不能再实例化对象了，但是可以定义指针和引用变量 

```cpp
/*
1. 让所有的动物实体类通过继承Animal直接复用该属性
2. 给所有的派生类保留统一的覆盖/重写接口
*/
class Animal
{
public:
	Animal(string name):_name(name){}
	virtual void bark() = 0; //纯虚函数
protected:
	string _name;
};
```



## 9. 笔试实战

**第一题**

前四个字节是 vfptr ，指向的是当前对象的 vftable



**第二题**

```cpp
#include<iostream>

using namespace std;

class Base
{
public:
	virtual void show (int i = 10)
	{
		cout << "Base::show i:" << i << endl;
	}
};

class Derive : public Base
{
public:
	void show(int i = 20)
	{
		cout << "Derive::show i:" << i << endl;
	}
};

int main()
{
	Base* p = new Derive();
	p->show();
	delete p;
	return 0;
}
/*
Derive::show i:10
*/
```

这个输出我第一次看到感到难以理解。**为什么**调用`show()`函数输出的是派生类，而输出的 i 是 10 ？

参数是编译时期压栈

```cpp
008B2923  push        0Ah   =>  函数调用,参数压栈是在编译时期就确定好的
008B2925  mov         eax,dword ptr [p]  
008B2928  mov         edx,dword ptr [eax]  
008B292A  mov         ecx,dword ptr [p]  
008B292D  mov         eax,dword ptr [edx]  
008B292F  call        eax  
```

**派生类的构造函数的默认值没有用**



**第三题**

派生类的构造函数为 private

```cpp
#include<iostream>

using namespace std;

class Base
{
public:
	virtual void show ()
	{
		cout << "Base::show" << endl;
	}
};

class Derive : public Base
{
private:
	void show()
	{
		cout << "Derive::show" << endl;
	}
};

int main()
{
	Base* p = new Derive();
	p->show(); 
	delete p;
	return 0;
}
```

正常调用

最终能调用带Derive::show()，是在**运行时期**才确定的。

成员方法能不能调用，就是说方法的访问权限是不是public的，是在**编译阶段**就需要确定好的

如果把基类的构造函数标为private将编译出错

```cpp
#include<iostream>

using namespace std;

class Base
{
private:
	virtual void show ()
	{
		cout << "Base::show" << endl;
	}
};

class Derive : public Base
{
public:
	void show()
	{
		cout << "Derive::show" << endl;
	}
};

int main()
{
	Base* p = new Derive();
	p->show();  //error C2248: “Base::show”: 无法访问 private 成员(在“Base”类中声明)
	delete p;
	return 0;
}
```



**第四题**

构造函数的左大括号写入的虚指针

```cpp
#include<iostream>

using namespace std;

class Base
{
public:
	Base()
	{
		/*
		push ebp  压栈
		mov ebp, esp
		sub esp, 4Ch
		rep stos esp <-> ebp 0xCCCCCCCC(windows vs)
		vfptr <- &Base::vftable
		*/
		cout << "Base()" << endl;
		clear();
	}
	//~Base()
	//{
	//	cout << "~Base()";
	//}
	void clear()
	{
		memset(this, 0, sizeof(*this));
	}
	virtual void show ()
	{
		cout << "Base::show" << endl;
	}
};

class Derive : public Base
{
public :
	Derive()
	{
		/*
		vfptr <- &Derive::vftable
		*/
		cout << "Derive()" << endl;
	}
	//~Derive()
	//{
	//	cout << "~Derive()" << endl;
	//}
	void show()
	{
		cout << "Derive::show" << endl;
	}
};

int main()
{
	/*
	第一种情况虚函数指针为空,动态绑定时程序崩溃
	*/
	//Base* pb1 = new Base(); //error
	//pb1->show(); //动态绑定
	//delete pb1;

	/*
	vfptr里面存储的是vftable的地址
	vfptr <- vftable 要有这个指令写入指针
	*/
	Base* pb2 = new Derive();
	pb2->show();
	delete pb2;
	return 0;
}
```



# 第七章 多重继承的那些坑

多重继承：代码的复用  一个派生类有多个基类

```cpp
class C: public A, public B
{
};
```



## 1. 理解虚基类和虚继承

抽象类：有纯虚函数的类

虚基类：被虚继承的类，称作虚基类

virtual：

1. 修饰成员方法是虚函数
2. 可以修饰继承方式，是虚继承。被虚继承的类，称作虚基类

当一个类有虚函数，这个类生成 vfptr ，指向 vftable

vbptr 指向 vbtable，派生类虚继承而来

```cpp
#include <iostream>

using namespace std;
class A
{
public:
private:
	int ma;
};

class B : virtual public A
{
public:
private:
	int mb;
};

class C
{
	virtual void fun(){}
};

class D : virtual public C
{

};

int main()
{
	A a;
	B b;
	C c;
	D d;
	cout << sizeof(a) << endl;
	cout << sizeof(b) << endl; //12 有虚继承时：基类的数据要被搬到最后面
	cout << sizeof(c) << endl << sizeof(d) << endl;
	return 0;
}
```



```cpp
#include <iostream>

using namespace std;

class A
{
public:
	virtual void func() { cout << "A::func" << endl; }
private:
	int ma;
};

class B : virtual public A
{
public:
	void func() { cout << "B::func" << endl; }
private:
	int mb;
};

int main()
{
	//基类指针指向派生类对象时,永远指向的是派生类基类部分数据的起始地址
	A* p = new B();
	p->func();
	delete p;
	return 0;
}
```

运行时报错。p指向的是派生类基类部分数据的起始地址，但是应该从vbptr删除

![](https://img1.imgtp.com/2022/11/14/mf1BYDw5.png)

解决方法：不用堆，用栈。

```cpp
B b;
A* p = &b;
p->func();
```



## 2. 菱形继承的问题

**很少有多重继承。**

采用虚继承解决菱形继承

好处：

1. 可以做更多代码的复用  
2. D -> B, C    B *p = new D()  C *p = new D()  使用起来更灵活



使 ma 在 D 里面只存在一份

```cpp
#include <iostream>

using namespace std;

class A
{
public:
	A(int data) :ma(data) { cout << "A()" << endl; }
	~A(){ cout << "~A()"<< endl;  }
protected:
	int ma;
};

class B : virtual public A  //virtual public
{
public:
	B(int data) :A(data), mb(data) { cout << "B()" << endl; }
	~B() { cout << "~B()" << endl; }
protected:
	int mb;
};

class C : virtual public A  //virtual public
{
public:
	C(int data) :A(data), mc(data) { cout << "C()" << endl; }
	~C() { cout << "~C()" << endl; }
protected:
	int mc;
};

class D : public B, public C
{
public:
	D(int data) :A(data), B(data), C(data), md(data) { cout << "D()" << endl; } //A(data)
	~D() { cout << "~D()" << endl; }
protected:
	int md;
};

int main()
{
	D d(10);
	return 0;
}
```



## 3. C++的四种类型转换

C++**语言级别**提供的四种类型转换方式

const_cast : 去掉（指针或引用）常量属性的一个类型转换

**static_cast** : 提供编译器认为安全的类型转换（没有任何**联系**的类型之间的转换就被否定了）。**编译时期**的类型转换

reinterpret_cast : 类似于C风格的强制类型转换，谈不上安全

dynamic_cast : 主要用在继承结构中，可以支持RTTI类型识别的上下转换。**运行时期**的类型转换

 ```cpp
 #include <iostream>
 
 using namespace std;
 
 //dynamic_cast
 class Base
 {
 public:
 	virtual void func() = 0;
 };
 
 class Derive1 : public Base
 {
 public:
 	void func() { cout << "Derive1::func" << endl; }
 };
 
 class Derive2 : public Base
 {
 public:
 	void func() { cout << "Derive2::func" << endl; }
 	//Derive2实现新功能的API接口函数
 	void drive02func() { cout << "Derive2::drive02func" << endl; }
 };
 
 void showFunc(Base* p)
 {
 	//dynamic_cast会检查p指针是否指向的是一个Derive2类型的对象
 	//p->vfptr->vftable RTTI信息,
 	//如果是,dynamic_cast转换类型成功,返回Derive2对象的地址
 	//否则返回nullptr
 	Derive2* pd2 = dynamic_cast<Derive2*>(p); //static_cast放这里不安全
 	if (pd2 != nullptr)
 	{
 		pd2->drive02func();
 	}
 	else 
 		p->func(); //动态绑定
 }
 
 int main()
 {
 	Derive1 d1;
 	Derive2 d2;
 	showFunc(&d1);
 	showFunc(&d2);
 
 	//const int a = 10;
 	//int* p2 = const_cast<int*>(&a); //去掉常量属性的一个类型转换
 	//*p2 = 20;
 
 	//const_cast<这里面必须是指针或引用类型 如 int* int&>
 	//int b = const_cast<int>a; //error
 
 	//static_cast  
 	//int a = 97;
 	//char b = static_cast<char>(a);
 	//cout << b;
 
 	//reinterpret_cast C风格的强制转换
 	//int* p = nullptr;
 	//double* p2 = reinterpret_cast<double*>(p);
 	return 0;
 }
 ```



# 第八章 STL 6大组件

## 1. 简介

![1668446261783.png](https://img1.imgtp.com/2022/11/15/RxpZxgrL.png)

![1668446205404.png](https://img1.imgtp.com/2022/11/15/Z6MyDUIZ.png)



## 2. vector

 扩容是两倍

迭代器失效问题要注意

```cpp
vector<int>vec;
vec.reserve(20); //只给容器底层开辟指定大小的空间，并不会添加新的元素
cout << vec.size(); //0
```

resize()不仅给容器底层开辟指定大小的空间，还会添加新的元素



## 3. deque 容器和 list 容器

deque 二维，一维按二倍扩容，扩容的放在中间（oldsize / 2 的地方开始放）。第二维大小为  4096/sizeof(T) 

```
底层数据结构：动态开辟的二维数组，一维数组从2开始，以2倍的方式进行扩容，
每次扩容后，原来第二位的数组，从oldsize/2下标开始存放
```



list 双向循环列表

头节点的前一个就是尾节点



## 4. vector、deque、list 对比

deque底层内存是否是连续的？并不是。每一个第二维是连续的，但是第二维是动态new出来的

vector 和 deque之间的区别？

1. 底层数据结构
2. 前中后插入删除元素的时间复杂度：deque前O(1) vector O(n)
3. 对于内存的使用效率：vector 需要的内存空间必须是连续的；deque 可以分块进行数据存储。所以deque更高。
4. 在中间进行insert或者erase，vector和deque它们的效率谁更好一点？vector更好挪动 



vector 和 list 之间的区别？数组和链表



## 5. 容器适配器

1. 适配器底层没有自己的数据结构，它是另外一个容器的封装，它的方法，全部由底层依赖的容器进行实现的。
2. 没有实现自己的迭代器



为什么stack和queue依赖deque；priority_queue依赖vector？

1. vector的初始内存使用效率太低了，没有deque好
2. 对于queue来说，需要尾部插入、头部删除。
3. vector需要大片的连续内存，而deque只需要分段的内存。当存储大量的数据时，显然deque对于内存的利用率更好一些

堆可以用数组实现

```cpp
template<typename T, typename Container=deque<T>>
class Stack
{
public:
	void push(const T& val) { con.push_back(val); }
	void pop() { con.pop_back(); }
	T top() const { return con.back(); }
private:
	Container con;
};
```



## 6. 无序关联容器

要注意迭代器失效

```cpp
unordered_set<int>s;
for (int i = 0; i < 20; i++) s.insert(rand() % 100);
auto it = s.begin();
while (it != s.end()) {
    it = s.erase(it);
}
cout << s.size() << endl;
```



## 7. 有序关联容器

```cpp
#include <iostream>
#include <set>
#include <map>

using namespace std;

class Student
{
public:
	Student(int id, string name)
		:_id(id), _name(name){}
	bool operator<(const Student s) const //告诉编译器怎样排序
	{
		return _id < s._id;
	}
private:
	int _id;
	string _name;
	friend ostream& operator<<(ostream& out, const Student& stu);
};
ostream& operator<<(ostream& out, const Student& stu)
{
	out << "id:" << stu._id << " name: " << stu._name;
	return out;
}
int main()
{
	set<Student>s;
	s.insert({ 1000, "Bob" });
	s.insert({ 2000, "Sam" });
	for (const auto& stu : s)
	{
		cout << stu << endl;
	}
	return 0;
}
```



```cpp
#include <iostream>
#include <set>
#include <map>

using namespace std;

class Student
{
public:
	Student(int id = 0, string name = "")
		:_id(id), _name(name){}
private:
	int _id;
	string _name;
	friend ostream& operator<<(ostream& out, const Student& stu);
};
ostream& operator<<(ostream& out, const Student& stu)
{
	out << "id:" << stu._id << " name: " << stu._name;
	return out;
}
int main()
{
	map<int, Student> stuMap;
	stuMap.insert({ 1000, Student(1000, "张三") });
	stuMap.insert({ 1200, Student(2000, "李四") });
	for (auto it = stuMap.begin(); it != stuMap.end(); ++it)
	{
		cout << it->first << " " << it->second << endl;
	}
	cout << stuMap[1200] << endl;
	return 0;
}
```



## 8. 迭代器 iterator

容器的嵌套类型

常量类型：const_iterator

反向迭代器：

```cpp
//rbegin()：返回的是最后一个元素的反向迭代器表示
//rend()：返回的是首元素前驱位置的迭代其表示
vector<int>::reverse_iterator rit = a.rbegin();
for (; rit != a.rend(); ++rit)
{
    cout << *rit << ' ';
}
```



## 9. 函数对象

```cpp
#include <iostream>

using namespace std;

class Sum
{
public:
	int operator()(int a, int b)
	{
		return a + b;
	}
};

int main()
{
	Sum sum;
	/*
	把有operator()小括号运算符重载函数的对象,称作函数对象或者称作仿函数
	*/
	int ret = sum(10, 20); 
	cout << ret;
} 
```



```cpp
#include <iostream>

using namespace std;

template<typename T>
bool mygreater(T a, T b)
{
	return a > b;
}

template<typename T>
bool myless(T a, T b)
{
	return a < b;
}

template<typename T, typename Compare>
bool compare(T a, T b, Compare comp)
{
    //通过函数指针调用函数,是没有办法内联的,效率很低,因为有函数调用开销
    //编译时无法知道调用哪个函数,没有办法内联
	return comp(a, b);
}

int main()
{
	cout << compare(10, 20, mygreater<int>) << endl; //函数指针
	cout << compare(10, 20, myless<int>) << endl;
	return 0;
}
```

解决方法：用函数对象代替函数指针。

1. 通过函数对象调用operator()，可以省略函数的调用开销，比通过函数指针调用函数（不能够inline内联调用）效率高
2. 对象，可以添加一些属性。 



可以 ctrl 点开看定义。如以下修改成了从大到小

```cpp
set<int, greater<int>>s;
```



## 10. 泛型函数和绑定器

algorithm 里面的

泛型算法 = template + 迭代器 + 函数对象

1. 泛型算法的参数接收的都是迭代器
2. 参数还可以接收函数对象（C函数指针）



find_if 用条件进行查询

```cpp
#include<iostream>
#include<algorithm>
#include<vector>
#include <functional> //函数对象和绑定器

using namespace std;

/*
绑定器 + 二元函数对象 -》一元函数对象
bind1st:把二元函数对象的operator()的第一个新参绑定起来
bind2nd:把二元函数对象的operator()的第二个新参绑定起来
*/

int main()
{
	int arr[] = { 11,22,33,44,55 };
	vector<int>vec(arr, arr + sizeof(arr) / sizeof(arr[0]));	
	//find_if需要一元函数对象
	auto it2 = find_if(vec.begin(), vec.end(),
		//bind2nd(greater<int>(), 48)); //a > b 把b绑定成48
		//bind1st(less<int>(), 48)); //a < b 把a绑定成48
		[](int val)->bool {return val > 48; });
	vec.insert(it2, 48);
	for (int i : vec) cout << i << ' '; cout << endl;

	for_each(vec.begin(), vec.end(), 
		[](int val)->void
		{
			if (val % 2 == 0) {
				cout << val << ' ';
			}
		});

	return 0;
}
```



