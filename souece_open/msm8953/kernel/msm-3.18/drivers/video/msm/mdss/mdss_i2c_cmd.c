/*
*author :shaomingliang (Eliot shao)
*version:1.0
*data:2016.9.9
*function description : an i2c interface for mipi bridge or others i2c slave
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/errno.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>

#include "mdss_i2c_cmd.h"
#include "mdss_dsi.h"


#define TEST_DEMO 0

struct mdss_i2c_interface {
	unsigned short flags;
	struct i2c_client *mdss_mipi_i2c_client;
	unsigned int slave_addr ;
	struct mutex lock;
	struct mutex i2c_lock;
	int gpio_rstn ;
	struct pinctrl		*pinctrl;
	struct pinctrl_state	*pin_default;
	struct pinctrl_state	*pin_sleep;
};


static struct mdss_i2c_interface *my_mipi_i2c ;



static int I2C_TEST_OK  = 0 ; //default
	
int HDMI_WriteI2C_Byte(int reg, int val)
{
	int rc = 0;
	rc = i2c_smbus_write_byte_data(my_mipi_i2c->mdss_mipi_i2c_client,reg,val);
	//printk(" ==rrd== HDMI_WriteI2C_Byte  reg=0x%x var=0x%x \n", reg,val);
	if (rc < 0) {
		printk("eliot :HDMI_WriteI2C_Byte fail \n");
        return rc;
		}
	return rc ;
}
int HDMI_ReadI2C_Byte(int reg)
{
	int val = 0;
	val = i2c_smbus_read_byte_data(my_mipi_i2c->mdss_mipi_i2c_client, reg);
    if (val < 0) {
        dev_err(&my_mipi_i2c->mdss_mipi_i2c_client->dev, "i2c read fail: can't read from %02x: %d\n", 0, val);
        return val;
    } 
	return val ;
}
/*
int Reset_chip(void)
{
	// 拉低LT8912 的reset pin，delay 150 ms左右，再拉高
	
	pr_err("stephen start chip\n");
	gpio_direction_output(my_mipi_i2c->gpio_rstn, 1);
	mdelay(50);

	gpio_direction_output(my_mipi_i2c->gpio_rstn, 0);
	mdelay(150);

	gpio_direction_output(my_mipi_i2c->gpio_rstn, 1);

	return 0;
	
}
*/


/*This function para Get from FAE*/
int mdss_mipi_i2c_init(struct mdss_dsi_ctrl_pdata *ctrl, int from_mdp)
{
	//pr_err("eliot :mdss_mipi_i2c_init start \n");
	//Reset_chip();
	if(I2C_TEST_OK  == 1)
		return 0;
	// 往LT8912寄存器写值：
	//******************************************//
	mutex_lock(&my_mipi_i2c->i2c_lock);
/*
	 HDMI_WriteI2C_Byte(0x20 , 0x00);// Register address : 0x08; Value : 0xff
	 //printk(KERN_ERR "eliot :read  0x02=0x%x \n",HDMI_ReadI2C_Byte(0x20));
	 HDMI_WriteI2C_Byte(0x21 , 0xD0);
	 //printk(KERN_ERR "eliot :read  0x02=0x%x \n",HDMI_ReadI2C_Byte(0x21));
	 HDMI_WriteI2C_Byte(0x22 , 0x25);
	 //printk(KERN_ERR "eliot :read  0x02=0x%x \n",HDMI_ReadI2C_Byte(0x22));

	 HDMI_WriteI2C_Byte(0x23 , 0x6E);
	 HDMI_WriteI2C_Byte(0x24 , 0x28);
	 HDMI_WriteI2C_Byte(0x25 , 0xDC);
	 HDMI_WriteI2C_Byte(0x26 , 0x00);
	 HDMI_WriteI2C_Byte(0x27 , 0x05);
	 HDMI_WriteI2C_Byte(0x28 , 0x05);
	 HDMI_WriteI2C_Byte(0x29 , 0x14);
	 HDMI_WriteI2C_Byte(0x34 , 0x80);
	 HDMI_WriteI2C_Byte(0x36 , 0x6E);
	 HDMI_WriteI2C_Byte(0xB5 , 0xA0);
	 HDMI_WriteI2C_Byte(0x5C , 0xFF);
	 HDMI_WriteI2C_Byte(0x2A , 0x07);
	 HDMI_WriteI2C_Byte(0x56 , 0x92);
	 HDMI_WriteI2C_Byte(0x6B , 0x52);
	 HDMI_WriteI2C_Byte(0x69 , 0x2C);
	 HDMI_WriteI2C_Byte(0x10 , 0x40);
	 HDMI_WriteI2C_Byte(0x11 , 0x88);
	 HDMI_WriteI2C_Byte(0xB6 , 0x20);
	 HDMI_WriteI2C_Byte(0x51 , 0x20);

#if TEST_DEMO
	 HDMI_WriteI2C_Byte(0x14 , 0x43);
	 HDMI_WriteI2C_Byte(0x2A , 0x49);
#endif 

	 HDMI_WriteI2C_Byte(0x09 , 0x10);

0x20 = 0x20
0x21 = 0xE0
0x22 = 0x13
0x23 = 0xC8
0x24 = 0x57
0x25 = 0x01
0x26 = 0x00
0x27 = 0xC8
0x28 = 0x03
0x29 = 0x1D
0x34 = 0x80
0x36 = 0xC8
0xB5 = 0xA0
0x5C = 0xFF
0x2A = 0x01
0x56 = 0x92
0x6B = 0x51
0x69 = 0x16
0x10 = 0x40
0x11 = 0x88
0xB6 = 0x20
0x51 = 0x20
0x09 = 0x10
*/

	 HDMI_WriteI2C_Byte(0x20, 0x20);
	 HDMI_WriteI2C_Byte(0x21, 0xE0);
	 HDMI_WriteI2C_Byte(0x22, 0x13);
	 HDMI_WriteI2C_Byte(0x23, 0xC8);
	 HDMI_WriteI2C_Byte(0x24, 0x57);
	 HDMI_WriteI2C_Byte(0x25, 0x01);
	 HDMI_WriteI2C_Byte(0x26, 0x00);
	 HDMI_WriteI2C_Byte(0x27, 0xC8);
	 HDMI_WriteI2C_Byte(0x28, 0x03);
	 HDMI_WriteI2C_Byte(0x29, 0x1D);
	 HDMI_WriteI2C_Byte(0x34, 0x80);
	 HDMI_WriteI2C_Byte(0x36, 0xC8);
	 HDMI_WriteI2C_Byte(0xB5, 0xA0);
	 HDMI_WriteI2C_Byte(0x5C, 0xFF);
	 HDMI_WriteI2C_Byte(0x2A, 0x05);
	 HDMI_WriteI2C_Byte(0x56, 0x92);
	 HDMI_WriteI2C_Byte(0x6B, 0x51);
	 HDMI_WriteI2C_Byte(0x69, 0x16);
	 HDMI_WriteI2C_Byte(0x10, 0x40);
	 HDMI_WriteI2C_Byte(0x11, 0x88);
	 HDMI_WriteI2C_Byte(0xB6, 0x20);
	 HDMI_WriteI2C_Byte(0x51, 0x20);

	HDMI_WriteI2C_Byte(0x1C, 0x44);
	HDMI_WriteI2C_Byte(0x1D, 0x44);

	 HDMI_WriteI2C_Byte(0x09, 0x10);

	mutex_unlock(&my_mipi_i2c->i2c_lock);

	pr_err("eliot :mdss_mipi_i2c_init end \n");
	return 0 ;
	
}

EXPORT_SYMBOL_GPL(mdss_mipi_i2c_init);

static int lt_pinctrl_init(struct mdss_i2c_interface *lt)
{
	struct i2c_client *client = lt->mdss_mipi_i2c_client;

	lt->pinctrl = devm_pinctrl_get(&client->dev);
	if (IS_ERR_OR_NULL(lt->pinctrl)) {
		dev_err(&client->dev, "Failed to get pinctrl\n");
		return PTR_ERR(lt->pinctrl);
	}

	lt->pin_default = pinctrl_lookup_state(lt->pinctrl, "default");
	if (IS_ERR_OR_NULL(lt->pin_default)) {
		dev_err(&client->dev, "Failed to look up default state\n");
		return PTR_ERR(lt->pin_default);
	}

	lt->pin_sleep = pinctrl_lookup_state(lt->pinctrl, "sleep");
	if (IS_ERR_OR_NULL(lt->pin_sleep)) {
		dev_err(&client->dev, "Failed to look up sleep state\n");
		return PTR_ERR(lt->pin_sleep);
	}

	return 0;
}

/*
	1.get reset pin and request gpio ;
	2.get i2c client ,check i2c function and test read i2c slave ;
	3.send lt8912 i2c configuration table
*/
static int mipi_i2c_probe(struct i2c_client *client,
        const struct i2c_device_id *id) 
{
    int err;
	struct mdss_dsi_ctrl_pdata *ctrl=NULL;
	int from_mdp=0 ;

	printk("eliot :mipi_i2c_probe start.....");
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev,
				"%s: check_functionality failed.", __func__);
		err = -ENODEV;
		goto exit0;
	}

	/* Allocate memory for driver data */
	my_mipi_i2c = kzalloc(sizeof(struct mdss_i2c_interface), GFP_KERNEL);
	if (!my_mipi_i2c) {
		dev_err(&client->dev,
				"%s: memory allocation failed.", __func__);
		err = -ENOMEM;
		goto exit1;
	}
	
	mutex_init(&my_mipi_i2c->lock);
	mutex_init(&my_mipi_i2c->i2c_lock);
/*
	if (client->dev.of_node) {
	my_mipi_i2c->gpio_rstn = of_get_named_gpio_flags(client->dev.of_node,
			"lt,gpio_rstn", 0, NULL);
	//printk("eliot:my_mipi_i2c->gpio_rstn=%d\n",my_mipi_i2c->gpio_rstn);
	}
	else
	{
		printk("eliot:client->dev.of_node not exit!\n");
	}
*/
	/***** I2C initialization *****/
	my_mipi_i2c->mdss_mipi_i2c_client = client;
	
	/* set client data */
	i2c_set_clientdata(client, my_mipi_i2c);

	/* initialize pinctrl */
	if (!lt_pinctrl_init(my_mipi_i2c)) {
		pr_err("stephen hdmi to pin default\n");
		err = pinctrl_select_state(my_mipi_i2c->pinctrl, my_mipi_i2c->pin_default);
		if (err) {
			dev_err(&client->dev, "Can't select pinctrl state\n");
		}
	}
	
//modify stephen
	mutex_lock(&my_mipi_i2c->lock);

	/* Pull up the reset pin */
	/* request  GPIO  */
	/*
	err = gpio_request(my_mipi_i2c->gpio_rstn, "lt8912_rsrn");
	if (err < 0) {
		mutex_unlock(&my_mipi_i2c->lock);
		printk("Failed to request GPIO:%d, ERRNO:%d",
			  my_mipi_i2c->gpio_rstn, err);
		err = -ENODEV;
	}else{
		gpio_direction_output(my_mipi_i2c->gpio_rstn, 0);
		mdelay(250);
		gpio_direction_output(my_mipi_i2c->gpio_rstn, 1);

		}
		*/

    err = i2c_smbus_read_byte_data(client, 0x08);
    if (err < 0) {
		pr_err("stephen ====== i2c test bad==============\n");
		I2C_TEST_OK = 1 ;
		mutex_unlock(&my_mipi_i2c->lock);
		//gpio_direction_output(my_mipi_i2c->gpio_rstn, 0);
		dev_err(&client->dev, "i2c read fail: can't read from %02x: %d\n", 0, err);

    } 
	mdss_mipi_i2c_init(ctrl,from_mdp);


	mutex_unlock(&my_mipi_i2c->lock);
	printk("eliot :mipi_i2c_probe end ....\n");
	return 0;
/*
exit3:
	gpio_free(my_mipi_i2c->gpio_rstn);
exit2:
	kfree(my_mipi_i2c);
*/
exit1:
exit0:
	return err;

}

static const struct of_device_id mipi_i2c_of_match[] = {
    { .compatible = "qcom,icn6211",},
    {},
};

static const struct i2c_device_id mipi_i2c_id[] = {
    {"qcom,icn6211", 0},
    {},
};
static int mipi_i2c_resume(struct device *tdev) {
//	Reset_chip(); //modify stgephen
    return 0;
}

static int mipi_i2c_remove(struct i2c_client *client) {
	
    return 0;
}

static int mipi_i2c_suspend(struct device *tdev) {
//	gpio_direction_output(my_mipi_i2c->gpio_rstn, 0); //modify stephen
    return 0;
}

static const struct dev_pm_ops mipi_i2c_pm_ops =
{ 
    .suspend = mipi_i2c_suspend,
    .resume = mipi_i2c_resume, 
};


static struct i2c_driver mipi_i2c_driver = {
    .driver = {
        .name = "qcom,icn6211",
        .owner    = THIS_MODULE,
        .of_match_table = mipi_i2c_of_match,
        .pm = &mipi_i2c_pm_ops,
    },
    .probe    = mipi_i2c_probe,
    .remove   = mipi_i2c_remove,
    .id_table = mipi_i2c_id,
};

module_i2c_driver(mipi_i2c_driver);

MODULE_LICENSE("GPL");

