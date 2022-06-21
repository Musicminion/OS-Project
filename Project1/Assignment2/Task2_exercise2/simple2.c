/*Create a proc named seconds*/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>/*For jiffies*/
#include <asm/param.h> /*For HZ*/

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

/*volatile:每次都实时读取该变量的值,而不是读取该变量在寄存器中的值,
volatile为了防止那些非线程安全的变量的变更导致的结果错误,比如多个线程竞争a变量的修改，
得到的某个时刻的a变量的值就不一定是当前的a变量了,而有可能是某一时刻a变量的值*/

unsigned long int volatile start_jiffies,intime_jiffies;
const int hz=HZ;

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
    start_jiffies = jiffies;
    // 读取实时的数值jiffies
    printk(KERN_INFO "The /proc/seconds loaded!\n");
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(PROC_NAME,NULL);
    printk(KERN_INFO "The /proc/seconds unloaded!\n");
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
    rv=sprintf(buffer, "The running time is %d s\n", ((intime_jiffies-start_jiffies)/hz)); 
    /*将jiffies的值存入buffer，rv为写入的字符总数，不包括字符串追加在字符串末尾的空字符*/ 
    copy_to_user(usr_buf,buffer,rv);	
    /*从内核的buffer拷贝到用户的usr_buf*/
    return rv;
}


module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Show the program execution time");
MODULE_AUTHOR("ZhangZiqian");
