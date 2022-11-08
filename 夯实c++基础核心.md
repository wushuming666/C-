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

同样一份代码，C++ 结果为 `10 20 10`

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
