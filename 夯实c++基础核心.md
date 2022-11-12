![在这里插入图片描述](https://img-blog.csdnimg.cn/5a55edebde7647848d373f6072e8c2ac.png)

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





# 第六章 继承与多态



## 1. 继承的基本意义





## 2. 派生类的构造过程





## 3. 重载、隐藏、覆盖





## 4. 虚函数、静态绑定和动态绑定





## 5. 虚构析函数





## 6. 再谈动态绑定





## 7. 理解多态到底是什么





## 8. 理解抽象类





## 9. 笔试实战



# 第七章 多重继承的那些坑



## 1. 理解虚基类和虚继承





## 2. 菱形继承的问题





## 3. C++的四种类型转换





# 第八章 STL 6大组件





## 1. 简介





## 2. vector





## 3. deque 容器和 list 容器





## 4. vector、deque、list 对比





## 5. 容器适配器





## 6. 无序关联容器





## 7. 有序关联容器





## 8. 迭代器 iterator





## 9. 函数对象





## 10. 泛型函数和绑定器

