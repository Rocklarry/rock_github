#include <linux/input.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/of_gpio.h>
#include <linux/io.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/i2c/mms114.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include "../pinctrl/qcom/pinctrl-msm.h"
unsigned int timer_count=0;
struct hrtimer hrtimer_trunk_timer;
struct input_dev *trunk_input_dev;
ktime_t   m_kt;
int value=2000;
int trunk_detect_gpio = -1;
int old_trunk_value = -1;
static void report_trunk_rising(void)
{
	printk(" zjy_______________________________report_trunk_rising \n");
	input_report_key(trunk_input_dev, KEY_F15, 1);
	input_sync(trunk_input_dev);
	udelay(10);
	input_report_key(trunk_input_dev, KEY_F15, 0);
	input_sync(trunk_input_dev);
	
}
static void report_trunk_falling(void)
{
	 printk(" zjy_______________________________report_trunk_falling \n");
	 input_report_key(trunk_input_dev, KEY_F16, 1);
	 input_sync(trunk_input_dev);
	 udelay(10);
         input_report_key(trunk_input_dev, KEY_F16, 0);
         input_sync(trunk_input_dev);
}
static enum hrtimer_restart  hrtimer_trunk_timer_poll(struct hrtimer *timer)
{
	int g_value;
	void __iomem *regs;
    regs = base_iomem();
	g_value = readl(regs + 0x2b004);
	if(old_trunk_value != g_value) {	
	    old_trunk_value = g_value;
		if(3==g_value || 1==g_value)
		{
			report_trunk_rising();//上报按键
			printk(" zjy_____________trunk_do_work rising_________________   g_value = %d \n",g_value);
		} else if(2==g_value) {
			report_trunk_falling();
			printk(" zjy_____________trunk_do_work falling__________________   g_value = %d \n",g_value);
		}
	}
	hrtimer_forward(timer, timer->base->get_time(), m_kt);//hrtimer_forward(timer, now, tick_period);
	//return HRTIMER_NORESTART;
	return HRTIMER_RESTART;
}


 static int trunk_probe(struct platform_device *pdev)
{
	int ret;
	trunk_input_dev = input_allocate_device();
	//	2，初始化struct input_device对象
	trunk_input_dev->evbit[0] = BIT_MASK(EV_KEY);
	trunk_input_dev->name = "venus_trunk_fibocom";
	trunk_input_dev->phys = "venus/input15";
	input_set_capability(trunk_input_dev, EV_KEY, KEY_F15);
	input_set_capability(trunk_input_dev, EV_KEY, KEY_F16);
    hrtimer_init(&hrtimer_trunk_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	m_kt=ktime_set(value / 1000, (value % 1000) * 1000000);
    hrtimer_start(&hrtimer_trunk_timer,m_kt, HRTIMER_MODE_REL);
	hrtimer_trunk_timer.function = hrtimer_trunk_timer_poll;
	ret = input_register_device(trunk_input_dev);
    printk(" zjy_____________trunk_probe  \n");
	return 0;
}
 
static int trunk_remove(struct platform_device *pdev)
{
	printk(" zjy_______________________________ trunk_remove\n");
    return 0;
}

static const struct of_device_id trunk_dt_match[] = {
        {
                .compatible = "qcom,trunk",
        },
        {},
};

//device tree
static struct platform_driver trunk_driver = {
        .probe = trunk_probe,
        .remove = trunk_remove,
        .driver = {
                .name = "trunk",
                .owner = THIS_MODULE,
                .of_match_table = trunk_dt_match,
        },
};
 
static int __init hrtimer_trunk_init(void)
{
	
	printk(" zjy______________trunk_init_________________ %s()  \n",__func__);
    if (platform_driver_register(&trunk_driver)) { 
        printk("failed to register trunk driver module\n");
        return -ENODEV;
    }

    return 0;
 
}
static void __exit hrtimer_trunk_exit(void)
{
	hrtimer_cancel(&hrtimer_trunk_timer);
	platform_driver_unregister(&trunk_driver);
}
subsys_initcall(hrtimer_trunk_init);
module_exit(hrtimer_trunk_exit);
MODULE_DESCRIPTION("Meson trunk Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("18814303937@163.com");
