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







## 1. 类和对象、this指针





## 2. 构造函数和构析函数





## 3. 对象的深拷贝和浅拷贝





## 4. 代码应用实践





## 5. 构造函数的初始化列表





## 6. 类的各类成员方法以及区别





## 7.指向类成员的指针



