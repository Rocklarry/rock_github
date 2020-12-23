#include <linux/init.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

#include <linux/gpio.h>
#include <linux/of_gpio.h>


#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/of.h>

#include <linux/timer.h>
#include <linux/time.h>

#include <linux/timex.h>
#include <linux/kthread.h>
 #include <linux/input.h>
#include <linux/syscalls.h>
#include <linux/workqueue.h>

struct input_dev *warn_input_dev;
static struct delayed_work warn_wq;
void warn_do_work(struct work_struct *work); 
int warn_detect_gpio = -1;

//从低电平到高电平
static void report_warn_rising(void)
{
	printk(" zjy_______________________________report_warn_rising \n");
	input_report_key(warn_input_dev, KEY_F14, 1);
	input_sync(warn_input_dev);
	udelay(10);
	input_report_key(warn_input_dev, KEY_F14, 0);
	input_sync(warn_input_dev);
	
}
static void report_warn_falling(void)
{
	 printk(" zjy_______________________________report_warn_down \n");
	 input_report_key(warn_input_dev, KEY_F17, 1);
         input_sync(warn_input_dev);
	 udelay(10);
         input_report_key(warn_input_dev, KEY_F17, 0);
         input_sync(warn_input_dev);

}
 
//static int irq_enum;
//中断处理上半部
static irqreturn_t warn_interrupt(int irq, void *dev_id)
{
	printk(" zjy_____________warn_interrupt__________________ 中断上半部    \n");
	schedule_delayed_work(&warn_wq, msecs_to_jiffies(1000)); 
	return IRQ_HANDLED;
}

/*中断处理底半部*/
void warn_do_work(struct work_struct *work)
{
	int g_value;
	int irq_enum = gpio_to_irq(warn_detect_gpio);
	g_value = gpio_get_value(warn_detect_gpio);
	
	disable_irq_nosync(irq_enum); //进入中断时，屏蔽中断做防抖
 	if(1==g_value){
		report_warn_rising();//上报按键
		printk(" zjy_____________warn_do_work__________________   g_value = %d \n",g_value);
	} else if(0==g_value) {
	         report_warn_falling();
		 printk(" zjy_____________warn_do_work__________________   g_value = %d \n",g_value);
	}
	printk(" zjy_____________warn_do_work__________________   before_enable_irq\n");
	enable_irq(irq_enum); //使能中断时
}


static int setup_warn_detect(struct device_node *np)
{
	int ret =0;
	/*根据dtsi文件查找名为warn-detect的gpio号*/

	warn_detect_gpio = of_get_named_gpio(np,"gpio_warn",0);
	printk(" zjy_______________________________setup_warn_detect warn_detect_gpio = %d \n",warn_detect_gpio);

	if(gpio_is_valid(warn_detect_gpio))
	{  //判断gpio是否有效

		ret = gpio_request(warn_detect_gpio,"gpio_warn");
	}
	printk(" zjy_______________________________gpio_request warn_detect_gpio=ret　= %d  \n",ret);
	gpio_direction_input(warn_detect_gpio);  //设置为输入
	gpio_set_debounce(warn_detect_gpio,20);//gpio管脚防抖
	ret =request_irq(gpio_to_irq(warn_detect_gpio),warn_interrupt,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING| IRQF_ONESHOT,"warn_detect",NULL);
	INIT_DELAYED_WORK(&warn_wq,warn_do_work);
	printk(" zjy_______________________________setup_warn_detect request_threaded_irq 中断返回　= %d \n",ret);
	if(ret)
	{
		return ret;
	}
	return 0;
}

static int warn_probe(struct platform_device *pdev)
{
    int ret = 0;
	struct device_node *node = pdev->dev.of_node;
	setup_warn_detect(node);   //初始化中断	
	//	1， 构建一个struct input_device对象	
	warn_input_dev = input_allocate_device();
	//	2，初始化struct input_device对象
	warn_input_dev->evbit[0] = BIT_MASK(EV_KEY);
	warn_input_dev->name = "venus_gpio_fibocom";
	warn_input_dev->phys = "venus/input0";
	input_set_capability(warn_input_dev, EV_KEY, KEY_F14);
	input_set_capability(warn_input_dev, EV_KEY, KEY_F17);
	// 3， 注册struct input_device对象
	ret = input_register_device(warn_input_dev);
    printk(" zjy_____________warn_probe  \n");
    return 0;
}

static int warn_remove(struct platform_device *pdev)
{
  // int ret =gpio_to_irq(warn_detect_gpio);
 //  gpio_free(warn_detect_gpio);
 //  free_irq(ret,warn_interrupt);
	printk(" zjy_______________________________ warn_remove\n");
    return 0;
}

static const struct of_device_id warn_dt_match[] = {
        {
                .compatible = "qcom,warn",
        },
        {},
};

//device tree
static struct platform_driver warn_driver = {
        .probe = warn_probe,
        .remove = warn_remove,
        .driver = {
                .name = "warn",
                .owner = THIS_MODULE,
                .of_match_table = warn_dt_match,
        },
};

static int __init warn_init(void)
{
	printk(" zjy______________warn_init_________________ %s()  \n",__func__);
    if (platform_driver_register(&warn_driver)) { 
        printk("failed to register warn driver module\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit warn_exit(void)
{
   //int ret =gpio_to_irq(warn_detect_gpio);
  // gpio_free(warn_detect_gpio);
   //free_irq(ret,warn_interrupt);
   platform_driver_unregister(&warn_driver);
   printk(" zjy__________________warn_exit_____________ %s()  \n",__func__);

}
subsys_initcall(warn_init);
module_exit(warn_exit);
MODULE_DESCRIPTION("Meson warn Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("18814303937@163.com");


