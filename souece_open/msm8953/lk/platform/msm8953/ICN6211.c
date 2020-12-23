#include <debug.h>
#include <platform/gpio.h>
#include <i2c_qup.h>
#include <blsp_qup.h>
#include <platform/irqs.h>
#include <kernel/thread.h>
#include <display_resource.h>
#include <qtimer.h>
#include <err.h>





static unsigned char slave_addr = 0x2C;


static struct qup_i2c_dev *i2c_dev;
static int icn6211_i2c_init()
{
	i2c_dev = qup_blsp_i2c_init(BLSP_ID_2,QUP_ID_3,100000,19200000);
	if(!i2c_dev) {
		dprintf(CRITICAL, "mipi_dsi_i2c_device_init() failed\n");
		return ERR_NOT_VALID;
	}
	return NO_ERROR;
}

static unsigned char icn6211_i2c_read(unsigned char reg_addr)
{
    int ret = 0;
    unsigned char val = 0 ;
    struct i2c_msg msg_buf[] = {
        //{0x48, I2C_M_WR, 1, &reg_addr},
        //{0x48, I2C_M_RD, 1, &val}

		{slave_addr, I2C_M_WR, 1, &reg_addr},
        {slave_addr, I2C_M_RD, 1, &val}
    };
    ret = qup_i2c_xfer(i2c_dev, msg_buf, 2);
    if(ret < 0) {
        dprintf(CRITICAL, "qup_i2c_xfer error %d\n", ret);
        return ret;
    }
    return val;
}

static int icn6211_i2c_write(unsigned char reg_addr, unsigned char val)
{
    int ret = 0;
    unsigned char data_buf[] = { reg_addr, val };
    struct i2c_msg msg_buf[] = {
             //  {0x48, I2C_M_WR, 2, data_buf}
		{slave_addr, I2C_M_WR, 2, data_buf}
    };
    ret = qup_i2c_xfer(i2c_dev, msg_buf, 1);
	
	dprintf(CRITICAL, " ==rrd== qup_i2c_xfera slave=0x%x add=0x%x var=0x%x \n",slave_addr, reg_addr,val);
    if(ret < 0) {
        dprintf(CRITICAL, "qup_i2c_xfer error %d\n", ret);
        return ret;
    }
    return 0;
}


void reset_icn6211(void)
{
	gpio_tlmm_config(61, 0, GPIO_OUTPUT, GPIO_NO_PULL,GPIO_8MA, GPIO_DISABLE);
	mdelay(150);
	
	gpio_set_dir(61,2);
	mdelay(150);//100
	gpio_set_dir(61,0);
	mdelay(150);//100
	gpio_set_dir(61,2);
	mdelay(50);
}







void icn6211_reg_init()
{
	reset_icn6211(); // 刷寄存器之前，先Reset LT8911/B ,用GPIO 先拉低LT8911B的复位脚 100ms左右，再拉高，保持100ms

	icn6211_i2c_write(0x20, 0x20);// Register address : 0x08;	Value : 0xff
	icn6211_i2c_write(0x21, 0xE0);
	icn6211_i2c_write(0x22, 0x13);
	icn6211_i2c_write(0x23, 0xC8);
	icn6211_i2c_write(0x24, 0x57);
	icn6211_i2c_write(0x25, 0x01);
	icn6211_i2c_write(0x26, 0x00);
	icn6211_i2c_write(0x27, 0xC8);
	icn6211_i2c_write(0x28, 0x03);
	icn6211_i2c_write(0x29, 0x1D);
	icn6211_i2c_write(0x34, 0x80);
	icn6211_i2c_write(0x36, 0xC8);
	icn6211_i2c_write(0xB5, 0xA0);
	icn6211_i2c_write(0x5C, 0xFF);
	icn6211_i2c_write(0x2A, 0x05);
	icn6211_i2c_write(0x56, 0x92);
	icn6211_i2c_write(0x6B, 0x51);
	icn6211_i2c_write(0x69, 0x16);
	icn6211_i2c_write(0x10, 0x40);
	icn6211_i2c_write(0x11, 0x88);
	icn6211_i2c_write(0xB6, 0x20);
	icn6211_i2c_write(0x51, 0x20);
								
	icn6211_i2c_write(0x1C, 0x44);
	icn6211_i2c_write(0x1D, 0x44);
	//icn6211_i2c_write(0x14 , 0x43);
	//icn6211_i2c_write(0x2A , 0x49);

	icn6211_i2c_write(0x09 , 0x10);



	dprintf(CRITICAL,"eliot :mdss_mipi_i2c_init end \n");
	
}
//---------------------------------------------//

// MIPI输入点 eDP屏的 LT8911B 寄存器设置：

//**************************************************//
// 确定要在给LT8911B 提供MIPI信号之后，再初始化LT8911B寄存器。
//*************************************************** //

void INC6211_Initial(void)
{	
	//unsigned char count;

	//count=0;
	icn6211_i2c_init();//初始化i2c

//L1: 
	icn6211_reg_init();
	
    /*
	if(DPCD0202H!=0x77)
	{
		count++;
		if(count<=5)
			goto L1;
		else	
		{
			printf("LT8911B init fail,reboot!!!\n");
			reboot_device(0);
		}
	}
	*/
}
/************************************** The End Of File **************************************/
