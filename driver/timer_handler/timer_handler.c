/*************************************************************************
	> File Name: timer_handler.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年01月18日 星期六 17时40分12秒
 ************************************************************************/

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/kernel.h>
#include <linux/inetdevice.h>
#include <linux/string.h>
#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/checksum.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <net/udp.h>
#include <linux/time.h>
#include <linux/timer.h>


MODULE_LICENSE("GPL");

struct timer_list g_timer;

void timer_handler(unsigned long data)
{
    struct timespec ts;
    ts = current_kernel_time();
    printk(KERN_DEBUG"time_handler 时间戳: %ld", ts.tv_sec);

    printk(KERN_DEBUG"测试数据:%ld", data);

    g_timer.expires = jiffies + 3*HZ;
    add_timer(&g_timer);
    //mod_timer(&g_timer, jiffies + 5*HZ);
}



static int init_marker(void)
{
    struct timespec ts;
    ts = current_kernel_time();
    printk(KERN_DEBUG"setup_timer time:%ld", ts.tv_sec);

    setup_timer(&g_timer, timer_handler, 8);
    g_timer.expires = jiffies + 10*HZ;
    add_timer(&g_timer);

    return 0;
}

static void exit_marker(void)
{
    del_timer(&g_timer);
}

module_init(init_marker);
module_exit(exit_marker);
/*
1.2 代码说明

      setup_timer() 初始化定时器，设置定时器回调函数，参数。

      add_timer()添加定时器到系统。 回调函数只调用一次。

      add_timer()修改定时器参数，让定时器继续执行。

      del_timer()删除定时器。

上面函数代码意义：启动10秒后执行回调函数，之后每隔3秒再执行一次回调函数。

*/



