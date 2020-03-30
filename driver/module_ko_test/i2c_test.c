#include  <linux/module.h>  
#include  <linux/init.h> 
#include  <linux/delay.h>
#include  <linux/i2c.h>
#include  <linux/interrupt.h>  
#include  <linux/slab.h>  
#include  <linux/gpio.h>  
#include  <linux/debugfs.h>  
#include  <linux/seq_file.h>  
#include  <linux/regulator/consumer.h>  
#include  <linux/string.h>  
#include  <linux/of_gpio.h>  
 
#ifdef  CONFIG_OF //Open firmware must be defined for dts usage  
static  struct  of_device_id qcom_i2c_test_table[] = {  
  { .compatible =  "qcom,i2c-qup",}, //Compatible node must   
                                          //match dts  
  { }, 
}; 
#else 
#define  qcom_i2c_test_table NULL 
#endif  
 
//I2C slave id supported by driver 
static  const struct  i2c_device_id qcom_id[] = { 
  { "qcom_i2c_qup", 0 },  
  { }  
}; 
 
static  int  i2c_test_test_transfer( struct  i2c_client *client) 
{  
  struct  i2c_msg xfer; //I2C transfer structure 
  u8 *buf = kmalloc(1, GFP_ATOMIC); //allocate buffer from Heap since i2c_transfer() isnon -blocking call  
  buf[0] = 0x55;   //data to transfer  
  xfer.addr = client -> addr;  
  xfer.flags = 0; 
  xfer.len = 1;  
  xfer.buf = buf; 
   
  return  i2c_transfer(client->adapter, &xfer, 1); 
}  
 
static  int  i2c_test_probe(struct  i2c_client *client, 
    const  struct  i2c_device_id *id)  
{  
  int   irq_gpio =  - 1; 
  int   irq; 
  int   addr; 
  //Parse data using dt. 
  //if(client- >dev.of_node){ 
  //  irq_gpio = of_get_named_gpio_flags(client - >dev.of_node, 
//"qcom_i2c_qup,irq -gpio", 0, NULL);  
  //}  
  //irq = client->irq;  //GPIO irq #. already converted to gpio_to_irq  
  addr = client -> addr; //Slave Addr 
  //dev_err(&client-> dev,  "gpio [%d] irq [%d] gpio_irq [%d] Slaveaddr [%x] \ n", irq_gpio, irq, gpio_to_irq(irq_gpio), addr); 
   //You can initiate a I2C transfer anytime  
  //using i2c_client *client structure 
  i2c_test_test_transfer(client);  
     
  return  0; 
}  
 
//I2C Driver Info 
static  struct  i2c_driver i2c_test_driver = { 
  .driver = {  
    .name  = "qcom_i2c_test",  
    .owner  = THIS_MODULE, 
    .of_match_table = qcom_i2c_test_table,  
  }, 
  .probe    = i2c_test_probe, 
  .id_table       = qcom_id,  
}; 
 
static int __init i2c_test_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = i2c_add_driver(&i2c_test_driver);
	if (result)
		printk("i2c_test failed. Error number %d", result);

	return result;
}

static void __exit i2c_test_exit(void)
{
	/* deregister this driver with the USB subsystem */
	i2c_del_driver(&i2c_test_driver);
}

module_init(i2c_test_init);
module_exit(i2c_test_exit);


MODULE_AUTHOR("Christoph");
MODULE_DESCRIPTION("BMP280 driver");
MODULE_LICENSE("GPL");