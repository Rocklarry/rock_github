/*************************************************************************
	> File Name: queue_delayed_work.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年01月18日 星期六 14时27分59秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>

static void mykmod_work_handler(struct work_struct *w);

static struct workqueue_struct *wq = 0;
static DECLARE_DELAYED_WORK(mykmod_work, mykmod_work_handler);
static unsigned long onesec;

static void
mykmod_work_handler(struct work_struct *w)
{
        pr_info("mykmod work %u jiffies\n", (unsigned)onesec);
		printk("%s *******test*******",__func__);
}


static int  mykmod_init(void)
{
	
        onesec = msecs_to_jiffies(100);
        pr_info("mykmod loaded %u jiffies\n", (unsigned)onesec);

        if (!wq)
                wq = create_singlethread_workqueue("mykmod");
        if (wq)
                queue_delayed_work(wq, &mykmod_work, onesec);

        return 0;
}

static void  mykmod_exit(void)
{
        if (wq)
                destroy_workqueue(wq);
        pr_info("mykmod exit\n");
}

module_init(mykmod_init);
module_exit(mykmod_exit);

MODULE_DESCRIPTION("mykmod");
MODULE_LICENSE("GPL");


/*
static struct workqueue_struct *goodix_wq;
probe()
{
	INIT_WORK(&ts->work, goodix_ts_work_func);//初始化工作队列
	gtp_request_irq(ts)
}

static s8 gtp_request_irq(struct goodix_ts_data *ts)
{
   ret  = request_irq(ts->client->irq,
                       goodix_ts_irq_handler,
                       irq_table[ts->int_trigger_type],
                       ts->client->name,
                       ts);

}

goodix_ts_irq_handler()
{
	queue_work(goodix_wq, &ts->work);//加入工作队列
}

goodix_ts_work_func()
{
	//	数据处理
}

*/