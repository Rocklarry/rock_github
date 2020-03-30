/*************************************************************************
	> File Name: hello.c
	> Author: rock
	> Mail: rock@163.com 
	> Created Time: 2017年09月11日 星期一 11时46分16秒
 ************************************************************************/

//#include<stdio.h>

#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
static int hello_init(void)
{
	printk(KERN_ALERT "Hello World enter\n");
	return 0;
}
static void hello_exit(void)
{
	printk(KERN_ALERT "Hello World exit\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("Song YanNa");
MODULE_DESCRIPTION("A Sample Hello World Module");
MODULE_ALIAS("A Sample module");
