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
        printk(KERN_INFO "GOLDEN_RATIO_PRIM value is: %llu\n",GOLDEN_RATIO_PRIME);
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
	
        printk(KERN_INFO "GOLDEN_RATIO_PRIM value is: %llu\n",gcd(3300,24));
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

