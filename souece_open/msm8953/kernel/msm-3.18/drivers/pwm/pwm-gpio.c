/*******************************************/

#include <linux/module.h> 
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#if defined(CONFIG_FB)
#include <linux/notifier.h>
#include <linux/fb.h>
#endif


typedef enum {
    PWM_DISABLE = 0,
    PWM_ENABLE,
}PWM_STATUS_t;

struct gpio_demo_priv{
	int count;
	int pwm_gpio; 
	struct mutex mtx;
	int mode;

	unsigned long period; //PWM周期
    unsigned long duty;    //PWM占空比
    struct hrtimer mytimer;
    ktime_t kt; //设置定时时间
    PWM_STATUS_t status;

};


struct gpio_demo_priv *priv;
static int value=0;

#if defined(CONFIG_FB)
    struct notifier_block pwm_gpio_fb_notif;
#endif

static int pwm_gpio_init(void)
{
    int ret = -1;
	//初始化占空比
    priv->period = 1000000000;
    priv->duty = 50000000;

    if(gpio_request(priv->pwm_gpio, "pwm_gpio")){
        printk("error pwm_gpio_init");
        return ret;
    }else{
        gpio_direction_output(priv->pwm_gpio, 1);
        gpio_set_value(priv->pwm_gpio, 0);
        priv->status = PWM_DISABLE;
    }

    return 0;

}
/*
unsigned int pwm_count=0;

static enum hrtimer_restart hrtimer_handler(struct hrtimer *timer)
{    
    if (gpio_get_value(priv->pwm_gpio) == 1) {
        if (priv->duty != priv->period) {     //占空比100%的情况下没必要拉低
            gpio_set_value(priv->pwm_gpio, 0);
            priv->kt = ktime_set(0, priv->period-priv->duty);
        }
        hrtimer_forward_now(&priv->mytimer, priv->kt);
    } else {
        if (priv->duty != 0) {                   //占空比０%的情况下没必要拉高
            gpio_set_value(priv->pwm_gpio, 1);
            priv->kt = ktime_set(0, priv->duty);
        }
        hrtimer_forward_now(&priv->mytimer, priv->kt);
    }
	pwm_count++;
	//printk("================pwm_count=%d ==============\n",pwm_count++);
	if(pwm_count>10){
		hrtimer_cancel(&priv->mytimer);
		priv->status = PWM_DISABLE;
		pwm_count=0;
	}

    return HRTIMER_RESTART;    
}


static void pwm_gpio_start(int value)
{    
	if(value)
	{
		//高精度定时器
		hrtimer_init(&priv->mytimer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
		priv->mytimer.function = hrtimer_handler;
		priv->kt = ktime_set(0, priv->period-priv->duty);
		hrtimer_start(&priv->mytimer,priv->kt,HRTIMER_MODE_REL);
	}else{
		hrtimer_cancel(&priv->mytimer);

	}

}
*/

//对应应用层的write
static int value;
static ssize_t pwm_gpio_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)   
{  
	char *end;

	pr_info(" line=%d    %s \n", __LINE__,__func__);
	pr_info("enter, node: %s\n", attr->attr.name);
    value = simple_strtoul(buf, &end, 10);
    pr_info("value:%d  end:%s\n", value,end);

	if(value==1){
		pr_info("*************gpio test 1************\n");
		gpio_set_value(priv->pwm_gpio, 1);
	}
	else if(value==2){
		pr_info("*************gpio test 2************\n");
		gpio_set_value(priv->pwm_gpio, 1);
		udelay(5);
		gpio_set_value(priv->pwm_gpio, 0);
		udelay(5);
		gpio_set_value(priv->pwm_gpio, 1);
	}
	else if(value==3){
		pr_info("*************gpio test 3************\n");
		gpio_set_value(priv->pwm_gpio, 1);
		udelay(5);
		gpio_set_value(priv->pwm_gpio, 0);
		udelay(5);
		gpio_set_value(priv->pwm_gpio, 1);
		udelay(5);
		gpio_set_value(priv->pwm_gpio, 0);
		udelay(5);
		gpio_set_value(priv->pwm_gpio, 1);
	}
	else if(value==4){
		pr_info("*************gpio test 4************\n");
		gpio_set_value(priv->pwm_gpio, 1);
		udelay(3);
		gpio_set_value(priv->pwm_gpio, 0);
		udelay(3);
		gpio_set_value(priv->pwm_gpio, 1);
		udelay(3);
		gpio_set_value(priv->pwm_gpio, 0);
		udelay(3);
		gpio_set_value(priv->pwm_gpio, 1);
		udelay(3);
		gpio_set_value(priv->pwm_gpio, 0);
		udelay(3);
		gpio_set_value(priv->pwm_gpio, 1);
	

	//priv->status = PWM_ENABLE;
	//pwm_gpio_start(1);
		
	}else{
	priv->status = PWM_DISABLE;
	//pwm_gpio_start(0);
	gpio_set_value(priv->pwm_gpio, 0);
	pr_info("*************gpio test error************\n");
	}
    return count;  
}  
//对应应用层的read
static ssize_t pwm_gpio_show(struct kobject *kobj,  struct kobj_attribute *attr,  char *buf)  
{  
        int val=0;
		pr_info(" line=%d    %s \n", __LINE__,__func__);
		pr_info("enter, node: %s\n", attr->attr.name);
        val=sprintf(buf, "%d\n",value);
        return val;     
}  

//static DEVICE_ATTR(pwm_gpio_value,0664,pwm_gpio_show,pwm_gpio_store);

static struct kobj_attribute pwm_gpio_value_attr = {
 .attr = {
  .name = "pwm_gpio",
  .mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH,
 },
 .show = pwm_gpio_show,
 .store = pwm_gpio_store,
};

static struct attribute *gpio_pwm_attributes[]={
	&pwm_gpio_value_attr.attr,
	NULL
	};

static const struct attribute_group gpio_pwm_attr_group = {
	.attrs = gpio_pwm_attributes,
};


#if defined(CONFIG_FB)
static int pwm_gpio_suspend(void)
{
	printk("==rrd== %s line:%d \n",__func__,__LINE__);
	return 0;
}

static int pwm_gpio_resume(void)
{
	printk("==rrd== %s line:%d \n",__func__,__LINE__);
	return 0;	
}
#endif



#if defined(CONFIG_FB)
static int fb_notifier_callback(struct notifier_block *self,
                 unsigned long event, void *data)
{
    struct fb_event *evdata = data;
    int *blank;
	
	/*
    struct goodix_ts_data *ts =
        container_of(self, struct goodix_ts_data, fb_notif);

    if (evdata && evdata->data && event == FB_EVENT_BLANK &&
            ts && ts->client) {*/
        blank = evdata->data;
		
		printk("%s: ====rrd===  blank:%d\n", __func__,*blank);

        if (*blank == FB_BLANK_UNBLANK)
            pwm_gpio_resume();
        else if (*blank == FB_BLANK_POWERDOWN)
           pwm_gpio_suspend();

    return 0;
}
#endif


static int pwm_gpio_probe(struct platform_device *pdev){

	int ret=0;
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;

	struct kobject *pwm_kobj;

	if (!node)
		return -EINVAL;
	ret = of_gpio_count(node);
	if (ret == 0){
		return -EINVAL;
	}
	priv = devm_kzalloc(dev, sizeof(*priv) + sizeof(int) * ret, GFP_KERNEL);
	
	if (!priv){
		return -ENOMEM;
	}
	priv->count = ret;
	mutex_init(&priv->mtx);

	priv->pwm_gpio = of_get_named_gpio(dev->of_node,"gpio_pwm",0);
	gpio_direction_output(priv->pwm_gpio, 0);
	//gpio_direction_output(priv->pwm_gpio, 1); //设置输出的电平

	platform_set_drvdata(pdev,priv);


	pwm_gpio_init();

	pwm_kobj = kobject_create_and_add("pwm_gpio", NULL);

	ret = sysfs_create_group(pwm_kobj, &gpio_pwm_attr_group);
	//ret = sysfs_create_group(&dev->kobj, &gpio_pwm_attr_group);
	//ret = device_create_file(dev, &dev_attr_pwm_gpio_value);
	
    if (ret < 0)
        dev_warn(dev, "===pwm_gpio_probe== attr group create failed\n");
    else
        dev_info(dev, "==pwm_gpio_probe== attr group create success!\n");

#if defined(CONFIG_FB)
    pwm_gpio_fb_notif.notifier_call = fb_notifier_callback;
    ret = fb_register_client(&pwm_gpio_fb_notif);
	dev_warn(dev," to register fb_notifier: %d\n",ret);
#endif

	return 0;
}



static int pwm_gpio_remove(struct platform_device *pdev)
{
    gpio_free(priv->pwm_gpio);
    return 0;
}



/*
static const struct dev_pm_ops pwm_gpio_dev_pm_ops = {
	.suspend = pwm_gpio_suspend,
	.resume = pwm_gpio_resume,
};
	*/

//static UNIVERSAL_DEV_PM_OPS(pwm_gpio_dev_pm_ops, pwm_gpio_suspend, pwm_gpio_resume,NULL);

static struct of_device_id pwm_gpio_dt_match[] = {
    { .compatible = "qcom,pwm_gpio", },
    { },
};


static struct platform_driver pwm_gpio_driver = {
        .probe = pwm_gpio_probe,
        .remove = pwm_gpio_remove,
/*
#ifdef CONFIG_HAS_EARLYSUSPEND		
		.suspend = pwm_gpio_suspend,
		.resume = pwm_gpio_resume,
#endif
*/
        .driver = {
                .name = "pwm_gpio",
                .owner = THIS_MODULE,
					/*
#if CONFIG_PM
				.pm	= &pwm_gpio_dev_pm_ops,
#endif
					*/
                .of_match_table = pwm_gpio_dt_match,
        },
};

static int __init pwm_drv_init(void)
{
	if (platform_driver_register(&pwm_gpio_driver)) {
		printk("pwm gpio  failed to register  driver module\n");
		return -ENODEV;
	}
	return 0;
}

static void __exit pwm_drv_exit(void)
{
	platform_driver_unregister(&pwm_gpio_driver);
}

module_init(pwm_drv_init);
module_exit(pwm_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("rock");