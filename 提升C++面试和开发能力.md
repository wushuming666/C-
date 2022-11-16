![1668501969954.png](https://img1.imgtp.com/2022/11/15/TUpbVS7t.png)

# 一、内容提纲

![1668501686946.png](https://img1.imgtp.com/2022/11/15/pH6JXPGx.png)



# 二、对象被优化以后才是高效的C++编程

## 1. 对象使用过程中背后调用了哪些方法

```cpp
#include <iostream>

using namespace std;

class Test
{
public:
	Test(int a = 10) :ma(a) { cout << "Test()" << endl; }
	~Test() { cout << "~Test()" << endl; }
	Test(const Test& t) : ma(t.ma) { cout << "Test(const Test&)" << endl; }
	Test& operator=(const Test& t)
	{
		cout << "operator =" << endl;
		ma = t.ma;
		return *this;
	}
private:
	int ma;
};

int main()
{
	Test t1;
	Test t2(t1);
	Test t3 = t1;
	t3 = t1;
	cout << "-------t3-------" << endl;
	t3 = Test(30); //临时对象必须生成 生存周期:所在语句 
	cout << "-------t3-------" << endl;
	//显示生成临时对象 
	//C++编译器对于对象构造的优化:用临时对象生成新对象时,
	//临时对象就不产生了,直接构造新对象就可以了
	//隐式生成临时对象
	Test t4 = Test(20); //和 Test t4(20); 没有区别
	
	//显示生成临时对象
	//构造 =重载赋值 析构
	t4 = (Test)30; //int -> Test(int)
	t4 = 30; //int -> Test(int)
	cout << "----------" << endl;

	Test* p = &Test(40); //出了语句对象析构了 不应该这样 不安全
	const Test& ref = Test(50); //没有析构,相当于别名

	cout << "----------" << endl;
	return 0;
}
/*
Test()
Test(const Test&)
Test(const Test&)
operator =
-------t3-------
Test()
operator =
~Test()
-------t3-------
Test()
Test()
operator =
~Test()
Test()
operator =
~Test()
----------
Test()
~Test()
Test()
----------
~Test()
~Test()
~Test()
~Test()
~Test()
*/
```



```cpp
#include <iostream>

using namespace std;

class Test
{
public:
	//Test() / Test(10)只传了a / Test(int, int)
	Test(int a = 10, int b = 10) :ma(a), mb(b) { cout << "Test(int, int)" << endl; }
	~Test() { cout << "~Test()" << endl; }
	Test(const Test& t) : ma(t.ma), mb(t.mb) { cout << "Test(const Test&)" << endl; }
	Test& operator=(const Test& t)
	{
		cout << "operator =" << endl;
		ma = t.ma;
		mb = t.mb;
		return *this;
	}
private:
	int ma, mb;
};

Test t1(10, 10); //1. Test(int, int)
int main()
{
	Test t2(20, 20); //3. Test(int, int)
	Test t3 = t2; //Test (const Test&)
	static Test t4 = Test(30, 30); //static Test t4(30, 30);
	t2 = Test(40, 40); //Test(int, int) -> operator= -> ~Test()

	//类型强转 (a=50, b=10)
	//括号表达式是最后一个
	t2 = (Test)(50, 50); //Test(int, int) operator= ~Test()
	t2 = 60;
	
	Test* p1 = new Test(70, 70); //Test(int, int)
	Test* p2 = new Test[2]; //两次构造
	Test* p3 = &Test(80, 80); //指针指向临时对象 语句结束析构
	const Test& p4 = Test(90, 90); //Test(int, int)
	cout << "-----------" << endl;
	delete p1; //~Test()
	delete []p2; //两次~Test()
	cout << "-----------" << endl;
	return 0;
}
Test t5(100, 100); //2.Test(int, int)
/*
Test(int, int)
Test(int, int)
Test(int, int)
Test(const Test&)
Test(int, int)
Test(int, int)
operator =
~Test()
Test(int, int)
operator =
~Test()
Test(int, int)
operator =
~Test()
Test(int, int)
Test(int, int)
Test(int, int)
Test(int, int)
~Test()
Test(int, int)
-----------
~Test()
~Test()
~Test()
-----------
~Test()
~Test()
~Test()
~Test()
~Test()
~Test()
*/
```



## 2. 函数调用过程中对象背后调用的方法太多

```cpp
#include <iostream>

using namespace std;

class Test
{
public:
	Test(int data = 10)
		:ma(data)
	{
		cout << "Test(int)" << endl;
	}
	~Test()
	{
		cout << "~Test()" << endl;
	}
	Test(const Test& t) :ma(t.ma)
	{
		cout << "Test(const Test&)" << endl;
	}
	void operator=(const Test& t)
	{
		cout << "operator=" << endl;
		ma = t.ma;
	}
	int getData() const { return ma; }
private:
	int ma;
};

//不能返回局部或临时的指针或引用
Test GetObject(Test t) //3. Test(const Test&)
{
	int val = t.getData();
	Test tmp(val); //4. Test(int)
	return tmp; //5. Test(const Test&)  tmp不能被拿出来
	//6. ~Test() tmp
	//7. ~Test() t
}

int main()
{
	Test t1; //1. Test(int)
	Test t2; //2. Test(int)
	//实参到形参,初始化(两个对象)
	t2 = GetObject(t1); //8 operator= 
						//9 ~Test() 临时对象析构
	cout << "-----" << endl;
	return 0;
}
/*
Test(int)
Test(int)
Test(const Test&)
Test(int)
Test(const Test&)
~Test()
~Test()
operator=
~Test()
-----
~Test()
~Test()
*/
```



## 3. 总结三条对象优化的规则

1. 函数参数传递过程中，对象优先按引用传递，不要按值传递
2. 当函数返回对象的时候，应该优先返回一个临时对象，而不要返回一个定义过的对象
3. 接收返回值是对象的函数调用的时候，优先按初始化的方式接收，不要按赋值的方式接收



用临时对象拷贝构造同类型的新对象时有优化。从上面的 11 行优化成了 4 行

```cpp
Test GetObject(const Test& t)
{
	int val = t.getData();
	/*Test tmp(val);   //减少构造和析构
	return tmp;*/ 
	return Test(val); //直接构造main()上的临时对象
}

int main()
{
	Test t1; //1. Test()
	Test t2; //2. Test()
	
	t2 = GetObject(t1); //3. Test() 直接构造在main上
						//4. operator=
						//5. ~Test()
	cout << "-----" << endl;
	//6. ~Test()
	//7. ~Test()
	return 0;
}
```

```cpp
Test GetObject(const Test& t)
{
	int val = t.getData();
	return Test(val); //直接构造main()上的临时对象
}

int main()
{
	Test t1; //1. Test()
	//之前是先定义t2, t2要构造
	//这里是初始化,不是赋值
	Test t2 = GetObject(t1); //Test() 直接构造t2
	cout << "-----" << endl;
	//3. ~Test()
	//4. ~Test()
	return 0;
}
```



## 4. CMyString的代码问题

想法：B 找 A 要东西，如果 A 不要了，A 大可直接把东西给 B，而不是让 B 拷贝一份后， A 把原来的丢掉

当这个东西特别大时，原来的方法需要改变



## 5. 添加带右值引用参数的拷贝构造和赋值函数

通俗来讲：

左值：有内存、有名字

右值：没名字（临时量）**或** 没内存

```cpp
//数字没有内存，在寄存器
/*
	int tmp = 20;
	const int &&b = tmp;
	*/
int&& a = 10;
/*
	int tmp = 20;
	const int &b = tmp;
	*/
const int& b = 20;
```

```cpp
String&& a = String("aaa"); //String("aaa")是右值：没有名字
String& d = a; //右值本身是左值：既有名字又有内存
String b = String("bbb");
String& c = b; //左值引用
```



具体例子：用 **带右值引用参数的拷贝构造 和 带右值引用参数的赋值重载函数** 实现

```cpp
#include <iostream>

using namespace std;
#if 1
class String
{
public:
	String(const char* p = nullptr)
	{
		cout << "String()" << endl;
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
		cout << "~String()" << endl;
		delete[]_pstr;
		_pstr = nullptr;
	}
	//带左值引用参数的拷贝构造
	String(const String& str)
	{
		cout << "String(const String& str)" << endl;
		_pstr = new char[strlen(str._pstr) + 1];
		strcpy(_pstr, str._pstr);
	}
	//带右值引用参数的拷贝构造
	String(String&& str) //str引用的是一个临时变量
	{
		cout << "String(String&& str)" << endl;
		_pstr = str._pstr;
		str._pstr = nullptr;
	}
	//带左值引用参数的赋值重载函数
	String& operator=(const String& src)
	{
		cout << "String& operator=(const String&)" << endl;
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = new char[strlen(src._pstr) + 1];
		strcpy(_pstr, src._pstr);
		return *this;
	}
	//带右值引用参数的赋值重载函数
	String& operator=(String&& src)
	{
		cout << "String& operator=(String&&)" << endl;
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = src._pstr;
		src._pstr = nullptr;
		return *this;
	}


	const char* c_str() const { return _pstr; }
private:
	char* _pstr;
};

String GetString(String& str)
{
	const char* pstr = str.c_str();
	String tmpStr(pstr); //3. String()
	return tmpStr; 
	//4. String(&&) 带出去给main
	//5. ~String()
}

int main()
{
	String str1("aaaaa"); //1. String()
	String str2; //2. String()
	str2 = GetString(str1); //6.String& operator=(String&&)
							//7.~String() 析构传出来的
							
	cout << str2.c_str() << endl;
	return 0;
	//8. ~String()
	//9. ~String()
}
#endif
```



## 6. CMyString在vector上的应用

```cpp
#include <iostream>

using namespace std;
#if 1
class String
{
public:
	String(const char* p = nullptr)
	{
		cout << "String()" << endl;
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
		cout << "~String()" << endl;
		delete[]_pstr;
		_pstr = nullptr;
	}
	//带左值引用参数的拷贝构造
	String(const String& str)
	{
		cout << "String(const String& str)" << endl;
		_pstr = new char[strlen(str._pstr) + 1];
		strcpy(_pstr, str._pstr);
	}
	//带右值引用参数的拷贝构造
	String(String&& str) //str引用的是一个临时变量
	{
		cout << "String(String&& str)" << endl;
		_pstr = str._pstr;
		str._pstr = nullptr;
	}
	//带左值引用参数的赋值重载函数
	String& operator=(const String& src)
	{
		cout << "String& operator=(const String&)" << endl;
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = new char[strlen(src._pstr) + 1];
		strcpy(_pstr, src._pstr);
		return *this;
	}
	//带右值引用参数的赋值重载函数
	String& operator=(String&& src)
	{
		cout << "String& operator=(String&&)" << endl;
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = src._pstr;
		src._pstr = nullptr;
		return *this;
	}
	const char* c_str() const { return _pstr; }
private:
	char* _pstr;
	friend String operator+(const String& lhs, const String& rhs);
	friend ostream& operator<<(ostream& out, const String& str);
};

String operator+(const String& lhs,
	const String& rhs)
{
	String tmpStr;
	tmpStr._pstr = new char[strlen(lhs._pstr) + strlen(rhs._pstr) + 1];
	strcpy(tmpStr._pstr, lhs._pstr);
	strcat(tmpStr._pstr, rhs._pstr);
	return tmpStr; //右值引用, 存放数据的地方不改变
}

ostream& operator<<(ostream& out, const String& str)
{
	out << str._pstr;
	return out;
}

String GetString(String& str)
{
	const char* pstr = str.c_str();
	String tmpStr(pstr); 
	return tmpStr; 
}

int main()
{
	String str1 = "Hello ";
	String str2 = "world!";
	cout << "---------" << endl;
	String str3 = str1 + str2;
	cout << "---------" << endl;
	cout << str3 << endl;
	return 0;
}
#endif

/*
String()
String()
---------
String()
String(String&& str)
~String()
---------
Hello world!
~String()
~String()
~String()
*/
```

**右值引用**对效率的优化体验

```cpp
String str1 = "aaa";
vector<String>vec;
vec.reserve(10);
cout << "-----------" << endl;
vec.push_back(str1);
vec.push_back(String("bbb"));
cout << "-----------" << endl;

/*
String(const char*)
-----------
String(const String& str)
String(const char*)
String(String&& str)
~String()
-----------
~String()
~String()
~String()
*/
```



## 7.move移动语义和forward类型完美转发

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
	void construct(T* p, T&& val) //右值 负责对象构造
	{
		new (p) T(val); 
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

	////////////////////////////
	//左值
	void push_back(const T& val)
	{
		if (full())
			expand();
		_allocator.construct(_last, val);
		_last++;
	}
	//右值
	void push_back(T&& val) //一个右值引用变量本身还是一个左值
	{
		if (full())
			expand();
		_allocator.construct(_last, val);
		_last++;
	}
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

class String
{
public:
	String(const char* p = nullptr)
	{
		cout << "String()" << endl;
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
		cout << "~String()" << endl;
		delete[]_pstr;
		_pstr = nullptr;
	}
	//带左值引用参数的拷贝构造
	String(const String& str)
	{
		cout << "String(const String& str)" << endl;
		_pstr = new char[strlen(str._pstr) + 1];
		strcpy(_pstr, str._pstr);
	}
	//带右值引用参数的拷贝构造
	String(String&& str) //str引用的是一个临时变量
	{
		cout << "String(String&& str)" << endl;
		_pstr = str._pstr;
		str._pstr = nullptr;
	}
	//带左值引用参数的赋值重载函数
	String& operator=(const String& src)
	{
		cout << "String& operator=(const String&)" << endl;
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = new char[strlen(src._pstr) + 1];
		strcpy(_pstr, src._pstr);
		return *this;
	}
	//带右值引用参数的赋值重载函数
	String& operator=(String&& src)
	{
		cout << "String& operator=(String&&)" << endl;
		if (this == &src)
			return *this;
		delete[]_pstr;
		_pstr = src._pstr;
		src._pstr = nullptr;
		return *this;
	}
	const char* c_str() const { return _pstr; }
private:
	char* _pstr;
	friend String operator+(const String& lhs, const String& rhs);
	friend ostream& operator<<(ostream& out, const String& str);
};

String operator+(const String& lhs,
	const String& rhs)
{
	String tmpStr;
	tmpStr._pstr = new char[strlen(lhs._pstr) + strlen(rhs._pstr) + 1];
	strcpy(tmpStr._pstr, lhs._pstr);
	strcat(tmpStr._pstr, rhs._pstr);
	return tmpStr; //右值引用, 存放数据的地方不改变
}

ostream& operator<<(ostream& out, const String& str)
{
	out << str._pstr;
	return out;
}

String GetString(String& str)
{
	const char* pstr = str.c_str();
	String tmpStr(pstr);
	return tmpStr;
}

int main()
{
	String str1 = "aaa";
	vector<String>vec;
	cout << "-----------" << endl;
	vec.push_back(str1);
	vec.push_back(String("bbb"));
	cout << "-----------" << endl;
	return 0;
}
/*
String()
-----------
String(const String& str)
String()
String(const String& str)
~String()
-----------
~String()
~String()
~String()
*/
```

采用 `move()` 将左值强转为右值

```cpp
void construct(T* p, T&& val) //右值 负责对象构造
{
    new (p) T(move(val)); 
}

void push_back(T&& val) //一个右值引用变量本身还是一个左值
{
    if (full())
        expand();
    _allocator.construct(_last, std::move(val));
    _last++;
}
/*
String()
-----------
String(const String& str)
String()
String(String&& str)
~String()
-----------
~String()
~String()
~String()
*/
```

引用折叠：左值加右值为左值；右值加右值为右值

 ```cpp
 template<typename Ty>
 void construct(T* p, Ty&& val)
 {
     new (p) T(forward<Ty>(val));
 }
 
 template<typename Ty>
 void push_back(Ty&& val) //引用折叠
 {
     if (full())
         expand();
     //move：移动语义，得到右值类型
     //forward：类型的完美转发
     _allocator.construct(_last, std::forward<Ty>(val)); //左值变左值 右值变右值
     _last++;
 }
 ```



# 三、体验一下智能指针的强大

## 1. 基础知识

原来用的叫“裸指针”，在释放时需要手动释放。可能出现些问题。

智能指针 保证能做到资源的自动释放

利用栈上的对象出作用域自动析构的特征，来做到资源的自动释放。

```cpp
#include<iostream>

using namespace std;

template<typename T>
class CSmartPtr
{
public:
	CSmartPtr(T *ptr = nullptr)
		:mptr(ptr) {
		//cout << "CSmartPtr(T *ptr)" << endl;
	}
	~CSmartPtr() { 
		//cout << "~CSmartPtr()" << endl;
		delete mptr; 
	}
	T& operator*() { return *mptr; } //返回引用,可改变内存
	T* operator->() { return mptr; } //将->的返回值返回
private:
	T* mptr;
};

class Test
{
public:
	void test() { cout << "call Test::test()" << endl; }
};

int main()
{
	CSmartPtr<int>ptr1(new int);
	*ptr1 = 20;
	cout << *ptr1 << endl;
	CSmartPtr<Test>ptr2(new Test());
	(*ptr2).test();
	ptr2->test();
	return 0;
}
```



## 2. 不带引用计数的智能指针





## 3. 实现带引用指针计数的智能指针





## 4. shared_ptr的交叉引用问题





## 5. 多线程访问共享对象的线程安全问题





## 6. 自定义删除器



# 四、C++11中引入的bind绑定器和function函数对象

## 1. bind1st 和 bind2nd 什么时候会用到



## 2. bind1st 和 bind2nd 的底层实现原理



## 3. function函数对象类型的应用实例



## 4. 模板的完全特例化和部分特例化



## 5. function的实现原理



## 6. bind 和 function 实现线程池



## 7. lambda 表达式的实现原理



## 8. lambda 表达式的应用实践



# 五、C++11知识点汇总

## 1. C++11常用知识点整理总结



## 2. 通过thread类编写C++多线程程序



## 3. 线程间互斥-mutex互斥锁和lock_guard



## 4. 线程间同步通信-生产者消费者模型



## 5. 再谈lock_guard和unique_lock



## 6. 基于CAS操作的atomic原子类型



# 六、设计模式

## 1.单例模式代码设计



## 2. 线程安全的懒汉单例模式



## 3. 简单工厂和工厂方法



## 4. 抽象工厂



## 5. 代理模式



## 6. 装饰器模式



## 7. 适配器模式



## 8. 观察者模式



# 七、C++代码应用实践

## 1. dfs搜索迷宫路径



## 2. bfs最短路径



## 3. 大数的加减法



## 4. 海量数据查重问题解决方案汇总



## 5. 海量数据求top k问题解决方案汇总



## 6. 海量数据查重和top k综合应用



# 八、C++历年校招面经题目讲解



# 九、C++研发岗校招简历书写指导



# 十、C++11容器emplace方法原理剖析