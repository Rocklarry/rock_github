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
#include "../pinctrl/qcom/pinctrl-msm.h"
struct delayed_work g_poll_work;

struct input_dev *input_dev;
extern void __iomem *base_iomem(void);
static int pwr_probe(struct platform_device *pdev)
{      
	int gpio_106;
	struct device_node *node = pdev->dev.of_node; 
        void __iomem *regs;
	regs = base_iomem();
	writel(0x201, regs + 0x08000);//gpio8--->0x8
	writel(0x03, regs + 0x08004);//gpio8--->0x8
        //int ret = 0;
        //int gpio_106;
         //struct device_node *node = pdev->dev.of_node;
        gpio_106 = of_get_named_gpio(node, "gpio_106_5vd", 0);
        if(gpio_106 < 0)
        {
                printk(" chappier  get gpio faile  \n");
        }else{
                gpio_request(gpio_106, "gpio_106_5vd");
                printk(" chappier  get gpio success  \n");
                gpio_direction_output(gpio_106, 1);
                gpio_set_value(gpio_106,1);
		
                gpio_free(gpio_106);
        }
        printk(" chappier @@@@@@@@@____________________in %s() gpio_106 = %d \n",__func__,gpio_106);
        return 0;
}

static int pwr_remove(struct platform_device *pdev)
{
    return 0;
}



static const struct of_device_id pwr_dt_match[] = {
        {
                .compatible = "qcom,pwr",
        },
        {},
};

//device tree
static struct platform_driver pwr_driver = {
        .probe = pwr_probe,
        .remove = pwr_remove,
        .driver = {
                .name = "pwr",
                .owner = THIS_MODULE,
                .of_match_table = pwr_dt_match,
        },
};

static int __init pwr_init(void)
{

    if (platform_driver_register(&pwr_driver)) {
        printk("failed to register led driver module\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit pwr_exit(void)
{
   platform_driver_unregister(&pwr_driver);

}

subsys_initcall(pwr_init);

module_exit(pwr_exit);

MODULE_DESCRIPTION("Meson pwr Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("18814303937@163.com");
