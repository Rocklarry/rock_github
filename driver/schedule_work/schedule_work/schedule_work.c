/*************************************************************************
	> File Name: schedule_work.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年04月03日 星期五 10时25分45秒
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>
#include <linux/delay.h>

MODULE_AUTHOR("lcw");
MODULE_LICENSE("GPL");


static struct workqueue_struct *test_workqueue;
atomic_t wq_run_times;
unsigned int failed_cnt = 0;


static void do_work(void *);
static void do_work1(void *);

static struct work_struct test_work;
static struct work_struct test_work1;


static void do_work(void *arg)
{
    printk("goodbye\n");
    mdelay(2000);
    schedule_work(&test_work1);
}

static void do_work1(void *arg)
{
    printk("hello world\n");
    mdelay(2000);
    schedule_work(&test_work);
}
int wq_init(void)
{
    INIT_WORK(&test_work, do_work); //初始化工作队列函数
    INIT_WORK(&test_work1, do_work1);

    test_workqueue = create_singlethread_workqueue("test-wq");//创建单线程的工作队列
    schedule_work(&test_work);//添加入队列的工作完成后会自动从队列中删除
    printk("test-wq init success\n");
    printk("jiffies: %lu\n", jiffies);

    return 0;
}

void wq_exit(void)
{
    destroy_workqueue(test_workqueue);
    printk("wq exit success\n");
}

module_init(wq_init);
module_exit(wq_exit);
