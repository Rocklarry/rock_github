/*************************************************************************
	> File Name: 0-chain0.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: Tuesday, November 05, 2019 PM02:19:07 HKT
 ************************************************************************/

#include<linux/notifier.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h> /* printk() */
#include<linux/fs.h> /* everything() */

#define TESTCHAIN_INIT 0x52U

static RAW_NOTIFIER_HEAD(test_chain);

/* define our own notifier_call_chain */
static int call_test_notifiers(unsigned long val, void *v)
{
	return raw_notifier_call_chain(&test_chain, val, v);
}
EXPORT_SYMBOL(call_test_notifiers);

/* define our own notifier_chain_register func */
static int register_test_notifier(struct notifier_block *nb)
{
	int err;
	err = raw_notifier_chain_register(&test_chain, nb);

	if(err)
		goto out;

out:
	return	err;
}

EXPORT_SYMBOL(register_test_notifier);

static int __init test_chain_0_init(void)
{	printk(KERN_DEBUG "I'm in test_chain_0\n");
	return 0;
}

static void __exit test_chain_0_exit(void)
{
	printk(KERN_DEBUG "Goodbye to test_chain_0\n");
}

MODULE_LICENSE("GPL");
module_init(test_chain_0_init);
module_exit(test_chain_0_exit);
