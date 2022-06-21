### Project 1 Linux 内核模块简介

> **下面的内容摘自教材的介绍**	
>
> ​		在这个项目中，您将学习如何创建内核模块并将其加载到 Linux 内核中。然后，您将修改内核模块，以便它在 `/proc` 文件系统中创建一个条目。该项目可以使用随本文提供的 Linux 虚拟机完成。尽管您可以使用任何文本编辑器来编写这些 C 程序，但您必须使用终端应用程序来编译程序，并且您必须在命令行上输入命令来管理内核中的模块。
>
> ​		正如您将发现的那样，开发内核模块的优势在于它是一种与内核交互的相对简单的方法，因此您可以编写直接调用内核函数的程序。请务必记住，您确实在编写与内核直接交互的内核代码。这通常意味着代码中的任何错误都可能导致系统崩溃！但是，由于您将使用虚拟机，因此任何故障最多只需要重新启动系统即可。

#### 第一部分 安装虚拟机

- 我这里使用的是 `CentOS-7` 的操作系统

- 安装方法在另外的一门课程中有详细的实验报告，可以参阅这个链接 [如何安装Linux在虚拟机](https://github.com/Musicminion/2021-2022-2-Linux-and-Shell-Programming/blob/main/%E4%BD%9C%E4%B8%9A1%EF%BC%9A%E8%AE%B0%E5%BD%95%E5%AE%89%E8%A3%85Linux%E7%9A%84%E8%BF%87%E7%A8%8B.md))。

- 安装后可能还需要安装一些必备的软件，例如 `yum install` `yum install emacs` 以及`gcc`等，视情况而定。

  

#### 第二部分 内核模块概述

- 该项目的第一部分涉及到创建模块并将其插入 Linux 内核的一系列步骤。您可以通过输入命令`lsmod`列出当前加载的所有内核模块。此命令将在三列中列出当前内核模块：名称、大小和模块正在使用的位置。
- 具体可以如下图所示：

<img src="Project 1 Linux 内核模块简介.assets\image-20220228162125940.png" alt="image-20220228162125940" style="zoom: 67%;" />



- 然后创建一个`simple.c` 的文件夹，写入代码：

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>
#include <asm/param.h>

/* This function is called when the module is loaded. */
int simple_init(void)
{     
        printk(KERN_INFO "GOLDEN_RATIO_PRIM value is: %lu\n",GOLDEN_RATIO_PRIME);
        //直接输出变量值
        printk(KERN_INFO "gcd(3300,24)value is: %lu\n",gcd(3300,24));
        //调用gcd函数 返回一个整数 然后打印输出
        printk(KERN_INFO "jiffies value is: %lu\n",jiffies);
        //在启动的时候打印输出信息
        
        printk(KERN_INFO "Loading Module\n");
        return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	
        printk(KERN_INFO "GOLDEN_RATIO_PRIM value is: %lu\n",gcd(3300,24));
        printk(KERN_INFO "jiffies value is: %lu\n",jiffies);
	printk(KERN_INFO "HZ value is: %d\n",HZ);
        //在终止的时候打印输出信息
        printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");


```

- 当然还有必不可少的 `makefile` 文件，如果没有这个文件的话，在执行 `make` 的时候程序会报错，就是说找不到 `include`对应的头文件，所以推测这个文件是有关编译过程的配置文件，定义了使用到的一些库文件，这样编译的时候才能定位到相应的位置。

```
obj-m += simple.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

```



- 创建好了这两个文件之后（第一个文件的后缀是`.c`，第二个文件没有后缀），进入到这两个文件的父目录下，用终端命令行打开，然后输入命令 `make` 执行编译，编译后会生成一些列的文件：文件 `simple.ko` 代表编译后的内核模块。（编译后的命令行如下所示）

<img src="Project 1 Linux 内核模块简介.assets\image-20220228163256952.png" alt="image-20220228163256952" style="zoom:67%;" />

- 当然在正式开始前，我们前执行 `sudo dmesg -c` 命令，清除日志中的缓存区，通常来说，日志中的缓存区会很容易被填满，所以在开始前及时清除很有必要，清除的时候会打印已有的日志。

<img src="Project 1 Linux 内核模块简介.assets\image-20220228163456368.png" alt="image-20220228163456368" style="zoom:67%;" />

- 加载这个内核模块只需要执行下面的命令即可：

```
sudo insmod simple.ko
```

- 要检查模块是否已加载，只需要输入`lsmod` 命令并在列表中搜索模块 `simple`。 显然就在第一个，如下图所示。

![image-20220228163724587](Project 1 Linux 内核模块简介.assets\image-20220228163724587.png)



- 加载后，检查加载成功，然后再卸载模块，执行命令 `sudo rmmod simple`，即可卸载模块。
- 卸载后，输入命令`dmesg`检测日志，即可发现打印输出的一些数值，如下图所示。

![image-20220228163822728](Project 1 Linux 内核模块简介.assets\image-20220228163822728.png)



- **注释：** 在 1.4.3 节中，我们学习了定时器的作用以及定时器中断处理程序。 在 Linux 中，计时器滴答的速率（滴答速率）是在 `<asm/param.h> `中定义的值 HZ。 HZ 的值决定了定时器中断的频率，它的值因机器类型和架构而异。 例如，如果`HZ` 的值为$ 100$，则定时器中断每秒发生 $100$ 次，或每 $10$  毫秒发生一次。 此外，内核跟踪全局变量  `jiffies`，它维护自系统启动以来发生的定时器中断的数量。 `jiffies` 变量在文件 `<linux/jiffies.h>` 中声明。



#### 第三部分 proc 文件系统

- 先看课本给的样例

```c
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128			// 缓冲区大小128
#define PROC_NAME "hello"		// 定义这个进程名字为hello

/*forward Function declartion*/
ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos);
// 声明了一个proc_read读进程函数

/*Declare the ower and read function*/
static struct file_operations proc_ops ={
    .owner = THIS_MODULE,
    .read = proc_read,   
};

/*Begin call this function when the module loaded*/
int proc_init(void)
{
    proc_create(PROC_NAME,0666,NULL,&proc_ops);
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(PROC_NAME,NULL);
    printk(KERN_INFO "The /proc/hello unloaded!\n");
}

/*Implemention of proc_read*/
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv=0;
    char buffer[BUFFER_SIZE];
    static int completed=0;
    if (completed){
      completed=0;
      return 0;  
      /*proc_read逢0才停止，因此这个return 0保证只输出一次值，否则将无限循环输出*/
    }
    completed=1;
    rv=sprintf(buffer, "Hello World\n", );
    copy_to_user(usr_buf,buffer,rv);	
    return rv;
}


module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("None");
MODULE_AUTHOR("Book");

```



- 在模块入口点 `proc_init()` 中，我们使用 `proc_create()` 函数创建新的 `/proc/hello` 条目。这个函数被传递给 `proc_ops`，其中包含对结构文件操作的引用。这个结构初始化了 `.owner` 和 `.read` 成员。 `.read` 的值是函数 `proc read()` 的名称，每当读取 `/proc/hello` 时都会调用该函数。
- 在看 `proc_read()` 函数，字符串`“Hello World∖n”`被写入变量缓冲区，其中缓冲区存在于内核内存中。由于 `/proc/hello` 可以从用户空间访问，我们必须使用内核函数 `copy to user()` 将缓冲区的内容复制到用户空间。该函数将内核内存缓冲区的内容复制到存在于用户空间中的变量 `usr_buf`。每次读取 `/proc/hello` 文件时，`proc_read()` 函数都会被重复调用，直到它返回 0，因此必须有逻辑确保该函数在收集到数据后返回 $0$（在这种情况下，字符串 `"Hello World∖n"`) 将进入相应的 /proc/hello 文件。最后，注意 /proc/hello 文件在模块退出点 `proc_exit()` 中使用函数 `remove_proc_entry()` 被删除。



- 基于以上的提示文件，我们适当修改，即可得到第一题的代码：

- 执行代码还是要经过一下步骤

  - `su` 获取`root`权限
  - `make` 编译，注意这里的是`sample1` 所以对应的 `makedfile` 文件要改一下。
  - 加载内核模块，`sudo insmod sample1`
  - `cat /proc/jiffies` 执行，即可获得相应的 `jiffies` 数值。

  

```c
/*Create a proc named seconds*/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"

unsigned long int volatile intime_jiffies;


/*forward Function declartion*/
ssize_t proc_read(struct file *file,char __user *usr_buf,size_t count,loff_t *pos);

/*Declare the ower and read function*/
static struct file_operations proc_ops ={
    .owner = THIS_MODULE,
    .read = proc_read,   
/*.read is the name of thefunction proc read() that is to be called whenever /proc/hello is read.*/ 
};

/*Begin call this function when the module loaded*/
int proc_init(void)
{
    proc_create(PROC_NAME,0666,NULL,&proc_ops);
    printk(KERN_INFO "The module has been loaded!\n");
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(PROC_NAME,NULL);
    printk(KERN_INFO "The module has been unloaded!\n");
}

/*Implemention of proc_read*/
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	intime_jiffies = jiffies;
    int rv=0;
    char buffer[BUFFER_SIZE];
    static int completed=0;
    if (completed){
        completed=0;
        return 0;  
      /*proc_read逢0才停止，因此这个return 0保证只输出一次值，否则将无限循环输出*/
    }
    completed=1;
    rv=sprintf(buffer, "jiffies value is %d s\n", intime_jiffies); 
    /*将jiffies的值存入buffer，rv为写入的字符总数，不包括字符串追加在字符串末尾的空字符*/ 
    copy_to_user(usr_buf,buffer,rv);	
    /*从内核的buffer拷贝到用户的usr_buf*/
    return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("jiffies");
MODULE_AUTHOR("ZhangZiqian");
```

- 运行的参考图如下，可以看到，数值是实时的数值，这个数值会不断地增大。

![image-20220228181629037](Project 1 Linux 内核模块简介.assets\image-20220228181629037.png)

- 同样的道理，第二题只需要记录开始和最后的 `jiffies` 的数值：

$$
时间=\frac{jiffies_{终止}-jiffies_{开始}}{HZ}
$$

![image-20220228182622103](Project 1 Linux 内核模块简介.assets\image-20220228182622103.png)

- 运行的效果如上所示。