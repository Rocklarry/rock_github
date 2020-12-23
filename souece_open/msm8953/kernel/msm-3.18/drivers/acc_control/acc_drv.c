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

//#include <linux/time.h>
#include <linux/timer.h>
#include <linux/time.h>

#include <linux/timex.h>
#include <linux/kthread.h>
 #include <linux/input.h>
#include <linux/syscalls.h>
#include <linux/workqueue.h>

struct input_dev *acc_input_dev;
//struct work_struct acc_wq;
static struct delayed_work acc_wq;
void aac_do_work(struct work_struct *work); 
int acc_detect_gpio = -1;
int old_gpio_value = -1;

//从低电平到高电平
static void report_acc_rising(void)
{
	printk(" zjy_______________________________report_acc_rising \n");
	 
	
	input_report_key(acc_input_dev, KEY_F11, 1);
	input_sync(acc_input_dev);
	udelay(10);
	input_report_key(acc_input_dev, KEY_F11, 0);
	input_sync(acc_input_dev);
	
}

//从高电平到低电平
static void report_acc_falling(void)
{
	printk(" zjy_______________________________report_acc_falling \n");
         
    input_report_key(acc_input_dev, KEY_F12, 1);
    input_sync(acc_input_dev);
    udelay(10);
    input_report_key(acc_input_dev, KEY_F12, 0);
    input_sync(acc_input_dev);
}
 
//static int irq_enum;
//中断处理上半部
static irqreturn_t acc_interrupt(int irq, void *dev_id)
{

	//int irq_enum = gpio_to_irq(acc_detect_gpio);
	printk(" zjy_____________acc_interrupt__________________ 中断上半部    \n");
	//disable_irq_nosync(irq_enum); //进入中断时，屏蔽中断做防抖

	//schedule_work(&acc_wq);
	
	
	schedule_delayed_work(&acc_wq, msecs_to_jiffies(1000)); 
    //schedule_delayed_work(&acc_wq, msecs_to_jiffies(3000));//3s

   //disable_irq(irq_enum); //进入中断时，屏蔽中断做防抖
	return IRQ_HANDLED;
}

/*中断处理底半部*/
void acc_do_work(struct work_struct *work)
{
	int g_value;
	int irq_enum = gpio_to_irq(acc_detect_gpio);
	g_value = gpio_get_value(acc_detect_gpio);
	
	disable_irq_nosync(irq_enum); //进入中断时，屏蔽中断做防抖
 	if(old_gpio_value != g_value) {	
	    old_gpio_value = g_value;
		if(0 == g_value)
		{
			report_acc_falling();//上报按键
			printk(" zjy_____________acc_do_work__________________   g_value = %d \n",g_value);
		}else if(1==g_value)
		{

			report_acc_rising();//上报按键
			printk(" zjy_____________acc_do_work__________________   g_value = %d \n",g_value);
		}
	}
	printk(" zjy_____________acc_do_work__________________   before_enable_irq\n");
	enable_irq(irq_enum); //使能中断时
}


static int setup_acc_detect(struct device_node *np)
{

	int ret =0;

	/*根据dtsi文件查找名为acc-detect的gpio号*/

	acc_detect_gpio = of_get_named_gpio(np,"gpio_acc",0);
	printk(" zjy_______________________________setup_acc_detect acc_detect_gpio = %d \n",acc_detect_gpio);

	if(gpio_is_valid(acc_detect_gpio))
	{  //判断gpio是否有效

		ret = gpio_request(acc_detect_gpio,"gpio_acc");
	}
	printk(" zjy_______________________________gpio_request acc_detect_gpio=ret　= %d  \n",ret);
	
	/*if(ret)input_set_capability
	{

		printk(" zjy_______________________________gpio_acc 1 \n");
		return -EINVAL;

	}*/

	gpio_direction_input(acc_detect_gpio);  //设置为输入
	gpio_set_debounce(acc_detect_gpio,20);//gpio管脚防抖
	
	//ret =request_threaded_irq(gpio_to_irq(acc_detect_gpio),NULL,acc_detect_irq,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING| IRQF_ONESHOT,"acc_detect",NULL);
	ret =request_irq(gpio_to_irq(acc_detect_gpio),acc_interrupt,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING| IRQF_ONESHOT,"acc_detect",NULL);
	
	
	
	INIT_DELAYED_WORK(&acc_wq,acc_do_work);
	//schedule_delayed_work(&acc_wq, msecs_to_jiffies(3000));
	
	
	//NIT_WORK(&acc_wq,acc_do_work);
	printk(" zjy_______________________________setup_acc_detect request_threaded_irq 中断返回　= %d \n",ret);
	if(ret)
	{
		return ret;

	}


	
	return 0;
}



static int acc_probe(struct platform_device *pdev)
{
        int ret = 0;

	struct device_node *node = pdev->dev.of_node;
	
	setup_acc_detect(node);   //初始化中断	
	//	1， 构建一个struct input_device对象	
	acc_input_dev = input_allocate_device();

	//	2，初始化struct input_device对象
	acc_input_dev->evbit[0] = BIT_MASK(EV_KEY);
	acc_input_dev->name = "venus_gpio_fibocom";
	acc_input_dev->phys = "venus/input0";
	input_set_capability(acc_input_dev, EV_KEY, KEY_F12);
	input_set_capability(acc_input_dev, EV_KEY, KEY_F11);
	// 3， 注册struct input_device对象
	ret = input_register_device(acc_input_dev);

    printk(" zjy_____________acc_probe  \n");
    return 0;
}

static int acc_remove(struct platform_device *pdev)
{
  // int ret =gpio_to_irq(acc_detect_gpio);
 //  gpio_free(acc_detect_gpio);
 //  free_irq(ret,acc_interrupt);
	printk(" zjy_______________________________ acc_remove\n");
    return 0;
}

static const struct of_device_id acc_dt_match[] = {
        {
                .compatible = "qcom,acc",
        },
        {},
};

//device tree
static struct platform_driver acc_driver = {
        .probe = acc_probe,
        .remove = acc_remove,
        .driver = {
                .name = "acc",
                .owner = THIS_MODULE,
                .of_match_table = acc_dt_match,
        },
};

static int __init acc_init(void)
{
	printk(" zjy______________acc_init_________________ %s()  \n",__func__);
    if (platform_driver_register(&acc_driver)) {
        printk("failed to register led driver module\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit acc_exit(void)
{
   //int ret =gpio_to_irq(acc_detect_gpio);
  // gpio_free(acc_detect_gpio);
   //free_irq(ret,acc_interrupt);
   platform_driver_unregister(&acc_driver);
   printk(" zjy__________________acc_exit_____________ %s()  \n",__func__);

}
subsys_initcall(acc_init);
module_exit(acc_exit);


MODULE_DESCRIPTION("Meson acc Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("18814303937@163.com");


