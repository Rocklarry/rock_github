/*************************************************************************
	> File Name: 2-chain2.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: Tuesday, November 05, 2019 PM02:45:55 HKT
 ************************************************************************/

#include <linux/notifier.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h> /* everything() */

extern int call_test_notifiers(unsigned long val, void *v);//调用0-chain0中的函数

#define TESTCHAIN_INIT 0x52U

static int __init test_chain_2_init(void)
{
	printk(KERN_DEBUG "I'm in test_chain_2\n");
	call_test_notifiers(TESTCHAIN_INIT, "没用");
	return 0;
}

static void __exit test_chain_2_exit(void)
{
	printk(KERN_DEBUG "Goodbye to test_chain_2\n");
}

MODULE_LICENSE("GPL");
module_init(test_chain_2_init);
module_exit(test_chain_2_exit);


