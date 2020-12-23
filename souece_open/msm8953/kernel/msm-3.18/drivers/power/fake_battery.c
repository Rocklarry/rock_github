/* Copyright (c) 2014-2016 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) "FAKE_BATTERY: %s: " fmt, __func__

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/power_supply.h>
#include <linux/of.h>
#include <linux/printk.h>
#include <linux/ratelimit.h>
#include <linux/debugfs.h>
#include <linux/ktime.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>

struct fake_batt_data {
	struct power_supply *usb_psy;
	struct power_supply *typec_psy;
	struct power_supply batt_psy;
       struct platform_device *pdev;   //added by chenjing 20190612 for typec-otg mantis 0023020
	bool typec_dfp;
       bool typec_otg_support;  //added by chenjing 20190612 for typec-otg mantis 0023020
};


//added by chenjing 20190612 for typec-otg mantis 0023020
static void  fake_batt_get_usb_psy(struct fake_batt_data *fb_data)
{
    if (!fb_data->usb_psy)
    {
        fb_data->usb_psy = power_supply_get_by_name("usb");
    }
}

static void  fake_batt_get_typec_psy(struct fake_batt_data *fb_data)
{
    if (!fb_data->typec_psy)
    {
        struct device_node *of_node = fb_data->pdev->dev.of_node;
        const char *typec_psy_name_str = "ght,typec-psy-name";
	 const char *typec_psy_name;
    
        if (!of_property_read_string(of_node, typec_psy_name_str, &typec_psy_name))
        {
            fb_data->typec_psy = power_supply_get_by_name(typec_psy_name);
        }
    }
}




static void update_typec_otg_status(struct fake_batt_data *fb_data, int mode, bool force)
{
	pr_debug("update typec mode: %d, force:%d\n", mode, force);

       //added by chenjing 20190612 for typec-otg mantis 0023020
       if (fb_data->typec_otg_support)
       {
           fake_batt_get_usb_psy(fb_data);
       }
       
	if (mode == POWER_SUPPLY_TYPE_DFP) {
		fb_data->typec_dfp = true;

              if (fb_data->usb_psy)   //added by chenjing 20190612 for typec-otg mantis 0023020
              {
		    power_supply_set_usb_otg(fb_data->usb_psy, fb_data->typec_dfp);
              }
	} else if (force || fb_data->typec_dfp) {
		fb_data->typec_dfp = false;

              if (fb_data->usb_psy)   //added by chenjing 20190612 for typec-otg mantis 0023020
              {
		    power_supply_set_usb_otg(fb_data->usb_psy, fb_data->typec_dfp);
              }
	}
}

static enum power_supply_property fake_batt_properties[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_CAPACITY,
};

static int fake_batt_get_property(struct power_supply *psy,
				       enum power_supply_property prop,
				       union power_supply_propval *val)
{
	switch(prop)
	{
		case POWER_SUPPLY_PROP_STATUS:
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
			break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = 0;
			break;
		case POWER_SUPPLY_PROP_CAPACITY:
			val->intval = 100;
			break;
		default:
			val->intval = 0;
			break;
	}

	return 0;
}

static int fake_batt_set_property(struct power_supply *psy,
				       enum power_supply_property prop,
				       const union power_supply_propval *val)
{
	struct fake_batt_data *fb_data = container_of(psy,
			struct fake_batt_data, batt_psy);

	switch(prop)
	{
		case POWER_SUPPLY_PROP_TYPEC_MODE:
                    //added by chenjing 20190612 for typec-otg mantis 0023020
                    if (fb_data->typec_otg_support)
                    {
                        fake_batt_get_typec_psy(fb_data);
                    }
                    
			if(fb_data->typec_psy)
				update_typec_otg_status(fb_data, val->intval, false);
			break;
		default:
			break;
	}

	return 0;
}

static int determine_initial_status(struct fake_batt_data *fb_data)
{
	union power_supply_propval val;

	if(fb_data->typec_psy)
	{
		fb_data->typec_psy->get_property(fb_data->typec_psy, POWER_SUPPLY_PROP_TYPE, &val);
		update_typec_otg_status(fb_data, val.intval, true);
	}

	return 0;
}

static int fake_batt_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct fake_batt_data *fb_data;
	const char *typec_psy_name_str = "ght,typec-psy-name";
	const char *typec_psy_name;
	struct device_node *of_node = pdev->dev.of_node;

	fb_data = devm_kzalloc(&pdev->dev, sizeof(*fb_data), GFP_KERNEL);
       fb_data->pdev = pdev;   //added by chenjing 20190612 for typec-otg mantis 0023020

	fb_data->usb_psy = power_supply_get_by_name("usb");
	if(!of_property_read_string(of_node, typec_psy_name_str, &typec_psy_name))
		fb_data->typec_psy = power_supply_get_by_name(typec_psy_name);

       //added by chenjing 20190612 for typec-otg mantis 0023020
      fb_data->typec_otg_support = of_property_read_bool(of_node, "ght,typec-otg-enable");

	fb_data->batt_psy.name = "battery";
	fb_data->batt_psy.type = POWER_SUPPLY_TYPE_BATTERY;
	fb_data->batt_psy.get_property = fake_batt_get_property;
	fb_data->batt_psy.set_property = fake_batt_set_property;
	fb_data->batt_psy.properties = fake_batt_properties;
	fb_data->batt_psy.num_properties = ARRAY_SIZE(fake_batt_properties);

	ret = power_supply_register(&pdev->dev, &fb_data->batt_psy);
	if(ret < 0)
	{
		pr_err("register battery power supply failed: ret = %d\n", ret);
		return ret;
	}

	ret = determine_initial_status(fb_data);

	pr_info("%s: success\n", __func__);

	return ret;
}

static struct of_device_id fake_batt_match_table[] = {
	{ .compatible = "ght,fake-battery", },
	{ },
};

static struct platform_driver fake_batt_drv = {
	.driver = {
		.name = "fake_battery",
		.owner = THIS_MODULE,
		.of_match_table = fake_batt_match_table,
	},
	.probe = fake_batt_probe,
};

static int __init fake_batt_init(void)
{
	return platform_driver_register(&fake_batt_drv);
}

static void __exit fake_batt_exit(void)
{
	platform_driver_unregister(&fake_batt_drv);
}

module_init(fake_batt_init);
module_exit(fake_batt_exit);
