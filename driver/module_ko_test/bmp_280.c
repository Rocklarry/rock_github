
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/hwmon.h>
#include <linux/input-polldev.h>
#include <linux/workqueue.h>



#define BMP280_I2C_ADDRESS		0x76
#define BMP280_CHIP_ID			0x55

#define BMP280_CALIBRATION_DATA_START	0xAA
#define BMP280_CALIBRATION_DATA_LENGTH	11	/* 16 bit values */
#define BMP280_CHIP_ID_REG		0xD0
#define BMP280_VERSION_REG		0xD1
#define BMP280_CTRL_REG			0xF4
#define BMP280_TEMP_MEASUREMENT		0x2E
#define BMP280_PRESSURE_MEASUREMENT	0x34
#define BMP280_CONVERSION_REGISTER_MSB	0xF6
#define BMP280_CONVERSION_REGISTER_LSB	0xF7
#define BMP280_CONVERSION_REGISTER_XLSB	0xF8
#define BMP280_TEMP_CONVERSION_TIME	5

#define ABS_MIN_PRESSURE	30000
#define ABS_MAX_PRESSURE	120000
#define BMP_DELAY_DEFAULT   200

#define BMP280_CLIENT_NAME		"bmp280"



/* Each client has this additional data */
struct bmp280_data {
	struct i2c_client *client;
	struct mutex  lock;
	struct input_polled_dev *poll_dev;
	struct input_dev *input;
	struct delayed_work work;

	u32	delay;
	u32 raw_temperature;
	u32 raw_pressure;

};





static int bmp280_get_pressure(struct bmp280_data *data, int *pressure)
{
	int status;
	int  baro_raw_data[6] = {0,0,0,0,0,0};

	i2c_smbus_read_i2c_block_data(data->client, 0xF7, 6, baro_raw_data);



	printk("bmp280_get_pressure  m%0x, %0x, %0x, %0x, %0x, %0x,",baro_raw_data[0],baro_raw_data[1],
		baro_raw_data[2],baro_raw_data[3],baro_raw_data[4],baro_raw_data[5]);

	data ->raw_pressure = baro_raw_data[3];
	return baro_raw_data[3];
}

static ssize_t show_pressure(struct device * dev, struct device_attribute * attr, char * buf)
{
	int pressure;
	int status;
	
	struct i2c_client *client = to_i2c_client(dev);
	struct bmp280_data *data = i2c_get_clientdata(client);

	status = bmp280_get_pressure(data,&pressure);
	if(status != 0){
		return status;
	}else{
		return sprintf(buf, "%s\n",pressure);
	}
}

static DEVICE_ATTR(pressure0_input, S_IRUGO,show_pressure, NULL);



static struct attribute *bmp280_attributes[]={
	//&dev_attr_temp0_input.attr,
	&dev_attr_pressure0_input.attr,
	NULL
	};

static const struct attribute_group bmp280_attr_group = {

	.attrs = bmp280_attributes,
};

static void bmp280_work_func(struct work_struct *work)
{
	struct bmp280_data *client_data = container_of((struct delayed_work *)work,
		struct bmp280_data, work);
	unsigned long delay = msecs_to_jiffies(client_data->delay);
	unsigned long j1 = jiffies;
	int pressure;
	int status;

	status = bmp280_get_pressure(client_data, &pressure);

	if (status == 0) {
		input_report_abs(client_data->input, ABS_PRESSURE, pressure);
		input_sync(client_data->input);
	}

	schedule_delayed_work(&client_data->work, delay-(jiffies-j1));
}


static int bmp280_input_init(struct bmp280_data *data)
{
	struct input_dev *dev;
	int err;

	dev = input_allocate_device();
	if (!dev)
		return -ENOMEM;
	dev->name = "bmp280";
	dev->id.bustype = BUS_I2C;

	input_set_capability(dev, EV_ABS, ABS_MISC);
	input_set_abs_params(dev, ABS_PRESSURE,
		ABS_MIN_PRESSURE, ABS_MAX_PRESSURE, 0, 0);
	input_set_drvdata(dev, data);

	err = input_register_device(dev);
	if (err < 0) {
		input_free_device(dev);
		return err;
	}
	data->input = dev;

	return 0;
}

static void bmp280_init_client(struct i2c_client *client)
{
	unsigned char version;
	s32  res;
	//int status;
	struct  bmp280_data *data = i2c_get_clientdata(client);
	
	data -> client = client;
	
	
	version = i2c_smbus_read_byte_data(data->client,0xD0);
	res = i2c_smbus_read_byte_data(data ->client,0x88);

	//version = i2c_smbus_write_byte_data(const struct i2c_client * client, u8 command, u8 value)
	i2c_smbus_write_byte_data(client, 0xF4, 0x57);
	i2c_smbus_write_byte_data(client, 0x54, 0x00);
	
	
	mutex_init(&data->lock);
	
	printk( "BMP280 ver. %d.%d  %d found.\n",(version & 0x0F), (version & 0xF0) >> 4,res);

}


static int  bmp280_probe(struct i2c_client * client, const struct i2c_device_id * id)
{
	struct bmp280_data *data;
	int err = 0;
	data = kzalloc(sizeof(struct bmp280_data), GFP_KERNEL);
	if(!data){
		err = -ENOMEM;
		goto exit;
	}


	i2c_set_clientdata(client, data);
	
	bmp280_init_client(client);
	bmp280_input_init(data);

	err = sysfs_create_group(&client -> dev.kobj, &bmp280_attr_group);


		/* workqueue init */
	INIT_DELAYED_WORK(&data->work, bmp280_work_func);
	data->delay  = BMP_DELAY_DEFAULT;
	//data->enable = 0;
	

	//printk(&data->client ->dev,"  bmp280");
	printk("  probe\n");
	goto exit;
	

//exit_free:
//	kfree(data);
exit:
	return err;
	
}

static int  bmp280_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client ->dev.kobj,&bmp280_attr_group);
	kfree(i2c_get_clientdata(client));
	return 0;
}

static const struct i2c_device_id bmp280_id[] ={
		{"bmp280",0},
		{},
};
static const struct of_device_id  bmp280_of_match[] = {
	{.compatible = "qcom,bmp280"},
    	{ },
};

static struct i2c_driver  bmp280_driver = {

		.driver = {
			.owner = THIS_MODULE,
			.name = "bmp280",
			.of_match_table = bmp280_of_match,
			//.pm = &bmp280_i2c_pm_ops,//电源休眠唤醒管理 暂不作
		},
	.probe = bmp280_probe,
	.remove = bmp280_remove,
	.id_table    = bmp280_id,
	};



static void __init bmp280_init(void)
{
	
	i2c_add_driver(&bmp280_driver);
}

static void __exit bmp280_exit(void)
{
	i2c_del_driver(&bmp280_driver);
}
	

module_init(bmp280_init);
module_exit(bmp280_exit);


MODULE_AUTHOR("Christoph");
MODULE_DESCRIPTION("BMP280 driver");
MODULE_LICENSE("GPL");
