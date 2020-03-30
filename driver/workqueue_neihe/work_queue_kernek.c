/*************************************************************************
	> File Name: workqueue.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年02月21日 星期五 14时21分20秒
 ************************************************************************/

#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/delay.h>

//循环上报
/*
1，需要睡眠，阻塞的，只能用工作队列。
2，短时间内中断数量很多的，任务队列，软中断会更好。例如网络。
3，对性能要求很高的话，软中断最好。
4，使用任务队列时，应该注意同一个任务被多次调用，同一个函数被多个任务队列注意。
5，软中断要注意SMP，函数的重入
*/

static struct work_struct test_work;
static struct delayed_work test_delayed_work;
 
static void test_func(struct work_struct *work)
{
	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);
 
}
 
static void test_delayed_func(struct work_struct *work)
{
	schedule_delayed_work(&test_delayed_work, 3 * HZ);
	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);
 

}

static int __init main_init(void)
{
	int ret;
	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);
	
	//初始化工作队列
	INIT_WORK(&test_work, test_func);//正常执行
	ret = schedule_work(&test_work);
 
	INIT_DELAYED_WORK(&test_delayed_work, test_delayed_func);
	ret = schedule_delayed_work(&test_delayed_work, 5 * HZ);//延后执行
 
	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);

    return 0;
}

static void __exit main_exit(void)
{
   	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);
	cancel_work_sync(&test_work);
	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);
	cancel_delayed_work_sync(&test_delayed_work);
	printk("%s, %d, %ld\n", __func__, __LINE__, jiffies);

}

module_init(main_init);
module_exit(main_exit);
MODULE_LICENSE("GPL");