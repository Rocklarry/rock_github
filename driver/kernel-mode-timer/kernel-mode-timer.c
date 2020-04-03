/*************************************************************************
	> File Name: kernel-mode-timer.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年01月18日 星期六 15时23分29秒
 ************************************************************************/

#define DMESG_ALIAS "timerlog"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DMESG_ALIAS, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DMESG_ALIAS, ##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DMESG_ALIAS, ##args)

#include <linux/kernel.h>	     //kernel mode library, used for dmesg facility
#include <linux/module.h>            //kernel module library
#include <linux/timer.h>             //kernel mode timer library
#include <linux/sched.h>             //for module infos
#include <linux/moduleparam.h>       //for module parameters
#include <linux/vermagic.h>          //for getting current kernel and process info
#include <linux/init.h>              //for freeing shared memories


//定时回调
 /**
 * @brief module descriptions
 */
MODULE_ALIAS("timerModule");     //it will be module alias
MODULE_LICENSE("GPL");           //module license (must be define)
MODULE_AUTHOR("murat demirtas <muratdemirtastr@gmail.com>");   //module author
MODULE_DESCRIPTION("Example kernel mode timer usage");         //module descriptoin

 
static int timer_delay = 2000;//延时时间
static struct timer_list my_timer;
static long int timer_data = 40;
#define DEBUG_MODE

static long int  tmp=0;

void my_timer_callback( unsigned long data )
{
	volatile int ret_val;
	tmp++;
        /* print log */
	PINFO("kernel timer callback executing,data is %ld     const  %ld *******************   \n",data,tmp);
	/* 设置计时器间隔为毫秒*/
	ret_val = mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_delay));
	if (ret_val)
	    	    PERR("Cant set timer, error occurred\n");
	if (tmp >100000000)
	tmp=0;
}


static void debug_when_init(void)
{
	#ifdef DEBUG_MODE
	PINFO("Kernel version is: %s ",UTS_RELEASE);
	PINFO("Process ID is: %d",current->pid);
	PINFO("Module called with:%s ",current->comm);
	PINFO("Received parameter: %d ",timer_delay);
	#endif
	return;
}


static int __init kernel_timer_init(void)
{
	int ret_val;
	PINFO("Timer module initializing\n");
	debug_when_init();

	setup_timer(&my_timer, my_timer_callback, timer_data);//设置回调函数 参数

	ret_val = mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_delay));

	if(ret_val)
	    	    PERR("Cant set timer, error occurred\n");
	else
		    PINFO("Timer armed with %d ms\n", timer_delay);
	return 0;
}


static void __exit kernel_timer_exit(void)
{	
	/* remove kernel timer when unloading module */
	del_timer(&my_timer);
	PINFO("Timer module removing\n");
	return;
}


module_init(kernel_timer_init);
module_exit(kernel_timer_exit);
module_param(timer_delay, int, S_IRUGO);


