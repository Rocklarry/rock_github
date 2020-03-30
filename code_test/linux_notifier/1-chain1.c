/*************************************************************************
	> File Name: 1-chain1.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: Tuesday, November 05, 2019 PM02:19:18 HKT
 ************************************************************************/

#include <linux/notifier.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h> /* everything() */

extern int register_test_notifier (struct notifier_block *nb);//条用0-chain0中的函数注册 事件

#define TESTCHAIN_INIT 0x52U
/* realize the notifier_call func */

int test_init_event (struct notifier_block *nb, unsigned long event, void *v)
{
	switch (event)
	{
		case TESTCHAIN_INIT://接收到 0x52的通知
			printk(KERN_DEBUG "获得chain事件: test_chain_2 在初始化！\n");
			break;
		default:
			break;
	}
	return NOTIFY_DONE;
}
/* define a notifier_block */
static struct notifier_block test_init_notifier = {
	.notifier_call = test_init_event,//注册通知链
};
static int __init test_chain_1_init (void)
{
	printk (KERN_DEBUG "这个是 test_chain_1\n");
	register_test_notifier (&test_init_notifier);
	return 0;
}
static void __exit test_chain_1_exit (void)
{
	printk (KERN_DEBUG "Goodbye to test_clain_l\n");
}
MODULE_LICENSE ("GPL"); 

module_init (test_chain_1_init);
module_exit (test_chain_1_exit);
