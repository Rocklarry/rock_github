/*  Copyright (c) 2010  Christoph Mair <christoph.mair@gmail.com>
 *  Copyright (c) 2012  Bosch Sensortec GmbH
 *  Copyright (c) 2012  Unixphere AB
 *
 *  This driver supports the bmp085 and bmp18x digital barometric pressure
 *  and temperature sensors from Bosch Sensortec. The datasheets
 *  are available from their website:
 *  http://www.bosch-sensortec.com/content/language1/downloads/BST-BMP085-DS000-05.pdf
 *  http://www.bosch-sensortec.com/content/language1/downloads/BST-BMP180-DS000-07.pdf
 *
 *  A pressure measurement is issued by reading from pressure0_input.
 *  The return value ranges from 30000 to 110000 pascal with a resulution
 *  of 1 pascal (0.01 millibar) which enables measurements from 9000m above
 *  to 500m below sea level.
 *
 *  The temperature can be read from temp0_input. Values range from
 *  -400 to 850 representing the ambient temperature in degree celsius
 *  multiplied by 10.The resolution is 0.1 celsius.
 *
 *  Because ambient pressure is temperature dependent, a temperature
 *  measurement will be executed automatically even if the user is reading
 *  from pressure0_input. This happens if the last temperature measurement
 *  has been executed more then one second ago.
 *
 *  To decrease RMS noise from pressure measurements, the bmp085 can
 *  autonomously calculate the average of up to eight samples. This is
 *  set up by writing to the oversampling sysfs file. Accepted values
 *  are 0, 1, 2 and 3. 2^x when x is the value written to this file
 *  specifies the number of samples used to calculate the ambient pressure.
 *  RMS noise is specified with six pascal (without averaging) and decreases
 *  down to 3 pascal when using an oversampling setting of 3.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include "bd37_audio.h"
#include <linux/kernel.h>
#include <linux/i2c.h>



#define BD37_INITIAL_SETUP          0x01
#define BD37_LPF_SETUP              0x02
#define BD37_MIXING_SETUP           0x03
#define BD37_INPUT_SELECTOR         0x05
#define BD37_INPUT_GAIN             0x06
#define BD37_VOLUME_GAIN            0x20
/*
#define BD37_FADER_1CH_F     0x28
#define BD37_FADER_2CH_F     0x29
#define BD37_FADER_2CH_R     0x2A
#define BD37_FADER_2CH_R     0x2B
#define BD37_FADER_2CH_SUB     0x2c
#define BD37_MIXING_2CH_SUB     0x30
#define BD37_BASS_SETUP         0x41
#define BD37_MIDDLE_SETUP         0x44
*/
#define BD37_SYSTEM_RESET         0xFE

struct bd37_audio_data {
	struct	device *dev;
	struct  regmap *regmap;
	struct	mutex lock;
};

static struct i2c_client *audio_client = NULL;
static int bd37_power_en_gpio = -1;
static int bd37_ahd_gpio = -1;
static int bd37_gps_gpio = -1;

#if 0

static int i2c_write_buff(struct i2c_client *client,unsigned char  *buf,int  len)
{
    int ret=-1; 
	struct i2c_msg msgs;
    
	int i=0;

	msgs.flags = !I2C_M_RD;
	msgs.addr  = client->addr;
	msgs.len   = len ;
	msgs.buf   = buf;

#if 1
	printk(KERN_EMERG"[%s][%d]=======> addr=0x%x len=0x%x flags =0x%x :",__func__, __LINE__,msgs.addr,msgs.len,msgs.flags);
	for(i = 0; i < msgs.len; i++)
	{
		printk(KERN_EMERG "  0x%x   ", msgs.buf[i] );  
	}
	printk(KERN_ALERT"  \n ");  
	
#endif	

	ret = i2c_transfer(client->adapter, &msgs, 1);
	
	return  ( ret == 1)?len:ret;
}

int  audio_i2c_write(unsigned char  addr, unsigned char value) 
{
	
	int ret ;
	unsigned char u8Data[2];
	u8Data[0] = addr;
	u8Data[1] = value;
	ret =i2c_write_buff(audio_client,u8Data, 2);
	if( ret  < 0 )
	{
		printk(KERN_EMERG  " audio  i2c1_write_buff failed\n");
		return -1;
	}

	return 0;
}


static int i2c_read_buff(struct i2c_client *client,unsigned char *buf, int  len)
{
    struct i2c_msg msgs[2];
	int  ret = -1;
	
    msgs[0].flags = !I2C_M_RD;
    msgs[0].addr  = client->addr;
    msgs[0].len   = 1;
    msgs[0].buf   = &buf[0]; 

	printk("[%s][%d]=======> addr=0x%x len=0x%x flags =0x%x :",__func__, __LINE__,msgs[0].addr,msgs[0].len,msgs[0].flags);
    msgs[1].flags = I2C_M_RD;
    msgs[1].addr  = client->addr;
    msgs[1].len   = 1;
    msgs[1].buf   = &buf[1];  

	printk(KERN_EMERG"[%s][%d]=======> addr=0x%x len=0x%x flags =0x%x :",__func__, __LINE__,msgs[1].addr,msgs[1].len,msgs[1].flags);
	
    ret = i2c_transfer(client->adapter, msgs, 2);         
		
    return (ret == 1) ? len : ret; 
}

#endif
#if 0
static ssize_t bd37_audio_test_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
     //struct bd37_audio_data *private_data = dev_get_drvdata(dev);
    
      u32 getdata[8];
      u8 readdata[8];
      unsigned char regAddr;
      unsigned char data;
      char cmd;
      const char *buftmp = buf;
      int ret;

     ret = sscanf(buftmp, "%c ", &cmd);
     printk("------zhangqing: get cmd = %c\n", cmd);
     switch (cmd) {
         case 'w':
             ret = sscanf(buftmp, "%c %x %x ", &cmd, &getdata[0],
                      &getdata[1]);
             regAddr = (u8)(getdata[0] & 0xff);
             data = (u8)(getdata[1] & 0xff);
             printk("get value = %x\n", data);
             //regmap_write(private_data->regmap, regAddr,data);
             audio_i2c_write(regAddr,data);
             //regmap_read(private_data->regmap, regAddr, &data);
             readdata[0] = regAddr;
             i2c_read_buff(audio_client,readdata,2);
             printk("%x   %x\n", readdata[0], readdata[1]);
             break;
         case 'r':
             ret = sscanf(buftmp, "%c %x ", &cmd, &getdata[0]);
             printk("CMD : %c %x\n", cmd, getdata[0]);
     
             regAddr = (u8)(getdata[0] & 0xff);
             //regmap_read(private_data->regmap, regAddr, &data);
             readdata[0] = regAddr;
             i2c_read_buff(audio_client,readdata,2);
             printk("\n%x %x\n", readdata[0], readdata[1]);
             break;
         default:
             printk("Unknown command\n");
             break;
     }
    
     return count;

}
#endif
//smbus

static int bd37_read_reg(struct i2c_client *client, u8 reg)
{
	int ret;

	ret = i2c_smbus_write_byte(client, reg);
	if (ret) {
		dev_err(&client->dev,
			"couldn't send request. Returned %d\n", ret);
		return ret;
	}

	ret = i2c_smbus_read_byte(client);
	if (ret < 0) {
		dev_err(&client->dev,
			"couldn't read register. Returned %d\n", ret);
		return ret;
	}

	return ret;
}

static int bd37_write_reg(struct i2c_client *client, u8 reg, u8 data)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, data);
     	printk("------: reg:0x%x data:0x%x ----\n",reg,data );
	if (ret < 0)
		dev_err(&client->dev,
			"couldn't write data. Returned %d\n", ret);

	return ret;
}

static ssize_t bd37_audio_test_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
     //struct bd37_audio_data *private_data = dev_get_drvdata(dev);
    
      u32 getdata[8];
      u8 readdata[8];
      unsigned char regAddr;
      unsigned char data;
      char cmd;
      const char *buftmp = buf;
      int ret;

     ret = sscanf(buftmp, "%c ", &cmd);
     printk("------zhangqing: get cmd = %c\n", cmd);
     switch (cmd) {
         case 'w':
             ret = sscanf(buftmp, "%c %x %x ", &cmd, &getdata[0],
                      &getdata[1]);
             regAddr = (u8)(getdata[0] & 0xff);
             data = (u8)(getdata[1] & 0xff);
             printk("get value = %x\n", data);

             bd37_write_reg(audio_client,regAddr,data);

             readdata[0] = bd37_read_reg(audio_client,regAddr);
             printk("%x   %x\n", regAddr, readdata[0]);
             break;
         case 'r':
             ret = sscanf(buftmp, "%c %x ", &cmd, &getdata[0]);
             printk("CMD : %c %x\n", cmd, getdata[0]);
     
             regAddr = (u8)(getdata[0] & 0xff);
             readdata[0] = bd37_read_reg(audio_client,regAddr);
             printk("\n%x %x\n", regAddr, readdata[0]);
             break;
         default:
             printk("Unknown command\n");
             break;
     }
    
     return count;

}

#if 0
static ssize_t bd37_audio_test_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
     struct bd37_audio_data *private_data = dev_get_drvdata(dev);
    
      u32 getdata[8];
      u32 regAddr;
      u32 data;
      char cmd;
      const char *buftmp = buf;
      int ret;

     ret = sscanf(buftmp, "%c ", &cmd);
     printk("------zhangqing: get cmd = %c\n", cmd);
     switch (cmd) {
         case 'w':
             ret = sscanf(buftmp, "%c %x %x ", &cmd, &getdata[0],
                      &getdata[1]);
             regAddr = (getdata[0] & 0xff);
             data = (getdata[1] & 0xff);
             printk("get value = %x\n", data);
             regmap_write(private_data->regmap, regAddr,data);
             regmap_read(private_data->regmap, regAddr, &data);
             printk("%x   %x\n", getdata[1], data);
             break;
         case 'r':
             ret = sscanf(buftmp, "%c %x ", &cmd, &getdata[0]);
             printk("CMD : %c %x\n", cmd, getdata[0]);
     
             regAddr = (getdata[0] & 0xff);
             regmap_read(private_data->regmap, regAddr, &data);
             printk("\n%x %x\n", getdata[0], data);
             break;
         default:
             printk("Unknown command\n");
             break;
     }
    
     return count;

}
#endif
static ssize_t bd37_audio_test_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	char *s = buf;

	buf = "hello";
	return sprintf(s, "%s\n", buf);
}

/* sysfs callbacks */
static ssize_t bd37_audio_input_select_write(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct bd37_audio_data *data = dev_get_drvdata(dev);

    int ret;
	long val;

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		goto error_ret;
    mutex_lock(&data->lock);
	ret = regmap_write(data->regmap, BD37_INPUT_SELECTOR, val);
    mutex_unlock(&data->lock);

error_ret:
	return ret ? ret : count;
}

static ssize_t bd37_audio_input_select_read(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
    int status;
    unsigned int val = 0;
    struct bd37_audio_data *data = dev_get_drvdata(dev);

	mutex_lock(&data->lock);
	status = regmap_read(data->regmap, BD37_INPUT_SELECTOR,&val);
	if (status < 0) {
		dev_err(data->dev,"Error regmap_read.\n");
	}
    mutex_unlock(&data->lock);

    return sprintf(buf, "%u\n", val);
}
static DEVICE_ATTR(input_select, S_IWUSR | S_IRUGO,
					bd37_audio_input_select_read, bd37_audio_input_select_write);

static DEVICE_ATTR(com_test, S_IWUSR | S_IRUGO,
                    bd37_audio_test_show, bd37_audio_test_store);


static struct attribute *bd37_audio_attributes[] = {
	&dev_attr_input_select.attr,
    &dev_attr_com_test.attr,
	NULL
};

static const struct attribute_group bd37_audio_attr_group = {
	.attrs = bd37_audio_attributes,
};

#if 0
static void bd37_audio_reset_system(struct bd37_audio_data *data)
{
    unsigned int val = 0x81;
    int status;
    
 	status = regmap_write(data->regmap, BD37_SYSTEM_RESET,val);
	if (status < 0) {
		dev_err(data->dev,"Error bd37_audio_reset_system.%d\n",status);
	}
}
#endif

unsigned char   bd37_reg_addr[] ={0x05, 0x06, 0x20, 0x28, 0x29, 0x2a, 0x2b};
unsigned char   bd37_reg_val[]  ={0x02, 0x02, 0x7a, 0x80, 0x80, 0x80, 0x80};

#define BD37_NUM_REG    (sizeof(bd37_reg_addr)/sizeof(bd37_reg_addr[0]))

static void bd37_audio_init_regs(void)
{
    int i;

    for(i =0; i<BD37_NUM_REG;i++)
    {
        bd37_write_reg(audio_client,bd37_reg_addr[i],bd37_reg_val[i]);
    }
}


static int bd37_audio_get_of_properties(struct bd37_audio_data *data)
{
        struct device_node *np = data->dev->of_node;
        int r;

        if (!np)
            return -EINVAL;

        bd37_power_en_gpio = of_get_named_gpio(np,"audio_pwren_gpio", 0);
        if (bd37_power_en_gpio < 0) 
        {
            dev_err(data->dev,"%s: missing %d in dt node\n", __func__, bd37_power_en_gpio);
            return -EINVAL;
        } 
        else 
        {
           	r = gpio_request(bd37_power_en_gpio, "audio_en_gpio");
            if (r)
            {
        	    dev_err(data->dev,"%s: Fail gpio_request%d \n", __func__, bd37_power_en_gpio);
            }
            else
            {
                gpio_direction_output(bd37_power_en_gpio, 1);
                gpio_export(bd37_power_en_gpio, 0);
            }
         }
        
        bd37_ahd_gpio = of_get_named_gpio(np,"ahd_pwren_gpio", 0);
        if (bd37_ahd_gpio < 0) 
        {
            dev_err(data->dev,"%s: missing %d in dt node\n", __func__, bd37_ahd_gpio);
           // return -EINVAL;
        } 
        else 
        {
           	r = gpio_request(bd37_ahd_gpio, "ahd_en_gpio");
            if (r)
            {
        	    dev_err(data->dev,"%s: Fail gpio_request%d \n", __func__, bd37_ahd_gpio);
            }
            else
            {
                gpio_direction_output(bd37_ahd_gpio, 1);
                gpio_export(bd37_ahd_gpio, 0);
            }
         }

        bd37_gps_gpio = of_get_named_gpio(np,"gps_pwren_gpio", 0);
        if (bd37_gps_gpio < 0) 
        {
            dev_err(data->dev,"%s: missing %d in dt node\n", __func__, bd37_gps_gpio);
            return -EINVAL;
        } 
        else 
        {
           	r = gpio_request(bd37_gps_gpio, "gps_en_gpio");
            if (r)
            {
                dev_err(data->dev,"%s: Fail gpio_request%d \n", __func__, bd37_gps_gpio);
            }
            else
            {
                gpio_direction_output(bd37_gps_gpio, 1);
                gpio_export(bd37_gps_gpio, 0);
            }
         }
        return 0;
}

static int bd37_audio_init_client(struct bd37_audio_data *data)
{
    mutex_init(&data->lock);

	/* default settings */
	bd37_audio_get_of_properties(data);

    //waiting power on
    msleep(500);

    bd37_audio_init_regs();

	return 0;
}

struct regmap_config bd37_audio_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x75,
};
EXPORT_SYMBOL_GPL(bd37_audio_regmap_config);

int bd37_audio_probe(struct device *dev, struct regmap *regmap, struct i2c_client *client)
{
	struct bd37_audio_data *data;
	int err = 0;

	data = kzalloc(sizeof(struct bd37_audio_data), GFP_KERNEL);
	if (!data) {
		err = -ENOMEM;
		goto exit;
	}

    audio_client = client;

	dev_set_drvdata(dev, data);
	data->dev = dev;
	data->regmap = regmap;

	/* Initialize the BMP085 chip */
	err = bd37_audio_init_client(data);
	if (err < 0)
		goto exit_free;

	/* Register sysfs hooks */
	err = sysfs_create_group(&dev->kobj, &bd37_audio_attr_group);
	if (err)
		goto exit_free;

	printk("Successfully initialized %s!\n", BD37_AUDIO_NAME);

	return 0;

exit_free:
	kfree(data);
exit:
	return err;
}
EXPORT_SYMBOL_GPL(bd37_audio_probe);

int bd37_audio_remove(struct device *dev)
{
	struct bd37_audio_data *data = dev_get_drvdata(dev);
    gpio_free(bd37_power_en_gpio);
    gpio_free(bd37_ahd_gpio);
    gpio_free(bd37_gps_gpio);
	sysfs_remove_group(&data->dev->kobj, &bd37_audio_attr_group);
	kfree(data);

	return 0;
}
EXPORT_SYMBOL_GPL(bd37_audio_remove);

MODULE_AUTHOR("Christoph Mair <christoph.mair@gmail.com>");
MODULE_DESCRIPTION("BMP085 driver");
MODULE_LICENSE("GPL");
