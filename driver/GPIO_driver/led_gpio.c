#include <linux/pm.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fsl_devices.h>
#include <asm/setup.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/stat.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/regulator/consumer.h>

int gpio_ldo_pin = -1;
int gpio_flag = -1;
static struct class *gpio_cameral_flash_class = NULL;
static struct device *gpio_cameral_flash_dev = NULL;

#define CTL_POWER_ON    "1"
#define CTL_POWER_OFF   "0"

//cat
static ssize_t gpio_105_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    printk("%s\n", __func__);
    sprintf(buf, "gpio_105 is %d\n", gpio_flag);
    return strlen(buf);
}
//echo
static ssize_t gpio_105_store(struct device *dev,
        struct device_attribute *attr, const char *buf,
        size_t count)
{
    if(!strncmp(buf, CTL_POWER_ON, strlen(CTL_POWER_ON))) {
        printk("%s: to enable gpio_105\n", __func__);
        gpio_set_value(gpio_ldo_pin, 1);
        gpio_flag = 1;

    } else if(!strncmp(buf, CTL_POWER_OFF, strlen(CTL_POWER_OFF))) {
        printk("%s: to disable gpio_105\n", __func__);
        gpio_set_value(gpio_ldo_pin, 0);
        gpio_flag = 0;
    }

    return count;
}

static struct device_attribute gpio_105_dev_attr = {    //  /sys/class/gpio_cameral_flash下生成gpio_105设备节点
    .attr = {
        .name = "gpio_105",
        .mode = S_IRWXU|S_IRWXG|S_IRWXO,   //节点读写权限设置
    },
    .show = gpio_105_show,        //节点读方法
    .store = gpio_105_store,        //节点写方法
};



static int gpio_cameral_flash_probe(struct platform_device *pdev)
{
    int ret = 0;

    printk("qcom enter gpio_cameral_flash_probe \n");

    gpio_ldo_pin = of_get_named_gpio(pdev->dev.of_node, "qcom,gpio_cameral_flash", 0);
    if (gpio_ldo_pin < 0)
        printk("xcz gpio_ldo_pin is not available \n");

    ret = gpio_request(gpio_ldo_pin, "gpio_cameral_flash");//取名
    if(0 != ret) {
        printk("qcom gpio request %d failed.", gpio_ldo_pin);
        goto fail1;
    }

    gpio_direction_output(gpio_ldo_pin, 0);

    gpio_set_value(gpio_ldo_pin, 0);
    gpio_flag = 0;

    gpio_cameral_flash_class = class_create(THIS_MODULE, "gpio_cameral_flash");
    if(IS_ERR(gpio_cameral_flash_class))
    {
        ret = PTR_ERR(gpio_cameral_flash_class);
        printk("Failed to create class.\n");
        return ret;
    }

    gpio_cameral_flash_dev = device_create(gpio_cameral_flash_class, NULL, 0, NULL, "gpio_gpio_105");//取名
    if (IS_ERR(gpio_cameral_flash_dev))
    {
        ret = PTR_ERR(gpio_cameral_flash_class);
        printk("Failed to create device(gpio_cameral_flash_dev)!\n");
        return ret;
    }

    ret = device_create_file(gpio_cameral_flash_dev, &gpio_105_dev_attr);
    if(ret)
    {
        pr_err("%s: gpio_105 creat sysfs failed\n",__func__);
        return ret;
    }

    printk("xcz enter  gpio_cameral_flash_probe, ok \n");

fail1:
    return ret;
}
//硬件卸载时调用
static int gpio_cameral_flash_remove(struct platform_device *pdev)
{
    device_destroy(gpio_cameral_flash_class, 0);
    class_destroy(gpio_cameral_flash_class);
    device_remove_file(gpio_cameral_flash_dev, &gpio_105_dev_attr);

    return 0;
}

static int gpio_cameral_flash_suspend(struct platform_device *pdev,pm_message_t state)
{
    return 0;
}

static int gpio_cameral_flash_resume(struct platform_device *pdev)
{
    return 0;
}

static struct of_device_id gpio_cameral_flash_dt_match[] = {
    { .compatible = "qcom,gpio_led_flash",},
    { },
};
MODULE_DEVICE_TABLE(of, gpio_cameral_flash_dt_match);

static struct platform_driver gpio_flash_driver = {          //  /sys/class总线下生成设备节点gpio_cameral_flash
    .driver = {
        .name = "gpio_led_flash",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(gpio_cameral_flash_dt_match),
    },
    .probe = gpio_cameral_flash_probe,
    .remove = gpio_cameral_flash_remove,
    .suspend = gpio_cameral_flash_suspend,
    .resume = gpio_cameral_flash_resume,
};

static __init int gpio_flash_init(void)
{
    return platform_driver_register(&gpio_flash_driver);
}

static void __exit gpio_flash_exit(void)
{
    platform_driver_unregister(&gpio_flash_driver);
}

module_init(gpio_flash_init);
module_exit(gpio_flash_exit);
MODULE_AUTHOR("GPIO_LED_FLASH, Inc.");
MODULE_DESCRIPTION("QCOM GPIO_LED_FLASH");
MODULE_LICENSE("GPL");
