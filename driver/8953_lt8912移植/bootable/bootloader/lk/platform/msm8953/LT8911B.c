#include <debug.h>
#include <platform/gpio.h>
#include <i2c_qup.h>
#include <blsp_qup.h>
#include <platform/irqs.h>
#include <kernel/thread.h>
#include <display_resource.h>
#include <qtimer.h>
#include <err.h>


/*******************************************************

   1、LT8911B的IIC地址：
   a)如果LT8911B的第31脚（S_ADR）为低，则LT8911B的I2C 地址为0x52; // bit0 是读写标志位；如果是Linux系统，IIC address 的 bit7作为读写标志位，则I2C_Adr 应该是 0x29

   b)如果LT8911B的第31脚（S_ADR）为高，则LT8911B的I2C 地址为0x5a; // bit0 是读写标志位；如果是Linux系统，IIC address 的 bit7作为读写标志位，则I2C_Adr 应该是 0x2d

   2、IIC速率不要超过100KHz。

   3、要确定MIPI信号给到LT8911B之后，再初始化LT8911B。

   4、必须由前端主控GPIO来复位LT8911B；刷寄存器之前，先Reset LT8911B ,用GPIO 先拉低LT8911B的复位脚 100ms左右，再拉高，保持100ms。


   5、LT8911B 对MIPI输入信号的要求：
   a) MIPI DSI
   b) Video mode
   c) non-burst mode（continue mode）--（MIPI 的CLK是要连续的）
   d) sync event

 *********************************************************/

//#define TEST_PATTERN_MODE // 输出黑白竖条的test pattern


// 设置输入的MIPI信号的Lane数
#define _MIPI_Lane_ 4                                   // MIPI Lane 1,2,3,4



//#define TEST_PATTERN_MODE
enum
{
	H_act = 0,
	V_act,
	H_tol,
	V_tol,
	H_bp,
	H_sync,
	V_sync,
	V_bp
};
#define MIPI_Lane	4
//static int MIPI_Timing[]  = {1280,720,1650,750,220,40,5,20};// 1280x720 Timing
static int MIPI_Timing[]  = {1024,600,1344,635,144,20,3,20};// 1280x720 Timing
//static int MIPI_Timing[]  = {1920,1080,2200,1125,148,44,5,36};// 1920x1080 Timing





static unsigned char slave_addr = 0x48;

static unsigned char HDMI_VIC = 0x00;

static struct qup_i2c_dev *i2c_dev;
static int LK_lt8911_i2c_init()
{
	i2c_dev = qup_blsp_i2c_init(BLSP_ID_2,QUP_ID_3,100000,19200000);
	if(!i2c_dev) {
		dprintf(CRITICAL, "mipi_dsi_i2c_device_init() failed\n");
		return ERR_NOT_VALID;
	}
	return NO_ERROR;
}

static unsigned char lt8911b_i2c_read(unsigned char reg_addr)
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

static int lt8911b_i2c_write(unsigned char reg_addr, unsigned char val)
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






void reset_lt8911(void)
{
	gpio_tlmm_config(61, 0, GPIO_OUTPUT, GPIO_NO_PULL,GPIO_8MA, GPIO_DISABLE);
	mdelay(150);
	
	gpio_set_dir(61,2);
	mdelay(150);//100
	gpio_set_dir(61,0);
	mdelay(150);//100
	gpio_set_dir(61,2);
	mdelay(300);
}

#ifdef TEST_PATTERN_MODE
static void Test_pattern_1280x720(void)
{
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x49;
	slave_addr = 0x49;

	lt8911b_i2c_write(0x72,0x12);
	lt8911b_i2c_write(0x73,0x04);//RGD_PTN_DE_DLY[7:0]
	lt8911b_i2c_write(0x74,0x01);//RGD_PTN_DE_DLY[11:8]  192
	lt8911b_i2c_write(0x75,0x19);//RGD_PTN_DE_TOP[6:0]  41
	lt8911b_i2c_write(0x76,0x00);//RGD_PTN_DE_CNT[7:0]
	lt8911b_i2c_write(0x77,0xd0);//RGD_PTN_DE_LIN[7:0]
	lt8911b_i2c_write(0x78,0x25);//RGD_PTN_DE_LIN[10:8],RGD_PTN_DE_CNT[11:8]
	lt8911b_i2c_write(0x79,0x72);//RGD_PTN_H_TOTAL[7:0]
	lt8911b_i2c_write(0x7a,0xee);//RGD_PTN_V_TOTAL[7:0]
	lt8911b_i2c_write(0x7b,0x26);//RGD_PTN_V_TOTAL[10:8],RGD_PTN_H_TOTAL[11:8]
	lt8911b_i2c_write(0x7c,0x28);//RGD_PTN_HWIDTH[7:0]
	lt8911b_i2c_write(0x7d,0x05);//RGD_PTN_HWIDTH[9:8],RGD_PTN_VWIDTH[5:0]

	
	lt8911b_i2c_write(0x70,0x80);
	lt8911b_i2c_write(0x71,0x17);

	// 74.25M CLK
	lt8911b_i2c_write(0x4e,0x93);
	lt8911b_i2c_write(0x4f,0x3e);
	lt8911b_i2c_write(0x50,0x69);
	lt8911b_i2c_write(0x51,0x80);

//-------------------------------------------------------//
}

#endif

void lt8911_reg_init()
{
	
	reset_lt8911(); // 刷寄存器之前，先Reset LT8911/B ,用GPIO 先拉低LT8911B的复位脚 100ms左右，再拉高，保持100ms

	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48; // IIC address
	slave_addr= 0x48;

	lt8911b_i2c_write(0x08,0xff);// Register address : 0x08; 	Value : 0xff
	lt8911b_i2c_write(0x09,0x81);
	lt8911b_i2c_write(0x0a,0xff);
	lt8911b_i2c_write(0x0b,0x64);//
	lt8911b_i2c_write(0x0c,0xff);

	lt8911b_i2c_write(0x44,0x31);// Close LVDS ouput
	lt8911b_i2c_write(0x51,0x1f);
	
	//******************************************//
	
	//	TxAnalog();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==2\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48;

	slave_addr= 0x48;
	lt8911b_i2c_write(0x31,0xa1);
	//lt8911b_i2c_write(0x32,0xa1);
	lt8911b_i2c_write(0x32,0xbf);
	//lt8911b_i2c_write(0x33,0x03);// 0x03 Open HDMI Tx； 0x00 Close HDMI Tx
	lt8911b_i2c_write(0x33,0x17);// 0x03 Open HDMI Tx； 0x00 Close HDMI Tx
	lt8911b_i2c_write(0x37,0x00);
	lt8911b_i2c_write(0x38,0x22);
	lt8911b_i2c_write(0x60,0x82);
	lt8911b_i2c_write(0x3a,0x00);
	
	//******************************************//
	
	//	CbusAnalog();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==3\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48;

	slave_addr= 0x48;
	lt8911b_i2c_write(0x39,0x45);
	lt8911b_i2c_write(0x3b,0x00);
	//******************************************//
	//*****//
	//MIPIAnalog()
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48;
	slave_addr= 0x48;
	lt8911b_i2c_write(0x3e,0xc6);
	lt8911b_i2c_write(0x41,0x7c); //HS_eq current
	
	//	HDMIPllAnalog();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==4\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48;

	slave_addr= 0x48;
	lt8911b_i2c_write(0x44,0x31);
	lt8911b_i2c_write(0x55,0x44);
	lt8911b_i2c_write(0x57,0x01);
	lt8911b_i2c_write(0x5a,0x02);
	
	//******************************************//
	
	//	MipiBasicSet();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==5\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x49;
	
	slave_addr= 0x49;
	lt8911b_i2c_write(0x10,0x01); // 0x05 
	lt8911b_i2c_write(0x11,0x08); // 0x12 
	lt8911b_i2c_write(0x12,0x04);  
	lt8911b_i2c_write(0x13,MIPI_Lane%0x04);  // 00 4 lane  // 01 lane // 02 2 lane //03 3 lane
	lt8911b_i2c_write(0x14,0x00);  
	lt8911b_i2c_write(0x15,0x00);
	lt8911b_i2c_write(0x1a,0x03);  
	lt8911b_i2c_write(0x1b,0x03);  
	
	
	//	MIPIDig1280x720();
	//MIPIDig();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==6\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x49;
	slave_addr= 0x49;
		/*
		lt8911b_i2c_write(0x18,0x28); // hwidth
		lt8911b_i2c_write(0x19,0x05); // vwidth 6
		lt8911b_i2c_write(0x1c,0x00); // H_active[7:0]
		lt8911b_i2c_write(0x1d,0x05); // H_active[15:8]
		lt8911b_i2c_write(0x1e,0x67); // hs/vs/de pol hdmi sel pll sel
		lt8911b_i2c_write(0x2f,0x0c); // fifo_buff_length 12
		lt8911b_i2c_write(0x34,0x72); // H_total[7:0]
		lt8911b_i2c_write(0x35,0x06); // H_total[15:8]
		lt8911b_i2c_write(0x36,0xEE); // V_total[7:0]
		lt8911b_i2c_write(0x37,0x02); // V_total[15:8]
		lt8911b_i2c_write(0x38,0x14); // VBP[7:0]
		lt8911b_i2c_write(0x39,0x00); // VBP[15:8]
		lt8911b_i2c_write(0x3a,0x05); // VFP[7:0]
		lt8911b_i2c_write(0x3b,0x00); // VFP[15:8]
		lt8911b_i2c_write(0x3c,0xDC); // HBP[7:0]
		lt8911b_i2c_write(0x3d,0x00); // HBP[15:8]
		lt8911b_i2c_write(0x3e,0x6E); // HFP[7:0]
		lt8911b_i2c_write(0x3f,0x00); // HFP[15:8]
	*/
	lt8911b_i2c_write(0x18,(unsigned char)(MIPI_Timing[H_sync]%256)); // hwidth
	lt8911b_i2c_write(0x19,(unsigned char)(MIPI_Timing[V_sync]%256)); // vwidth
	lt8911b_i2c_write(0x1c,(unsigned char)(MIPI_Timing[H_act]%256)); // H_active[7:0]
	lt8911b_i2c_write(0x1d,(unsigned char)(MIPI_Timing[H_act]/256)); // H_active[15:8]

	lt8911b_i2c_write(0x1e,0x67); // hs/vs/de pol hdmi sel pll sel
	lt8911b_i2c_write(0x2f,0x0c); // fifo_buff_length 12

	lt8911b_i2c_write(0x34,(unsigned char)(MIPI_Timing[H_tol]%256)); // H_total[7:0]
	lt8911b_i2c_write(0x35,(unsigned char)(MIPI_Timing[H_tol]/256)); // H_total[15:8]
	lt8911b_i2c_write(0x36,(unsigned char)(MIPI_Timing[V_tol]%256)); // V_total[7:0]
	lt8911b_i2c_write(0x37,(unsigned char)(MIPI_Timing[V_tol]/256)); // V_total[15:8]
	lt8911b_i2c_write(0x38,(unsigned char)(MIPI_Timing[V_bp]%256)); // VBP[7:0]
	lt8911b_i2c_write(0x39,(unsigned char)(MIPI_Timing[V_bp]/256)); // VBP[15:8]
	lt8911b_i2c_write(0x3a,(unsigned char)((MIPI_Timing[V_tol]-MIPI_Timing[V_act]-MIPI_Timing[V_bp]-MIPI_Timing[V_sync])%256)); // VFP[7:0]
	lt8911b_i2c_write(0x3b,(unsigned char)((MIPI_Timing[V_tol]-MIPI_Timing[V_act]-MIPI_Timing[V_bp]-MIPI_Timing[V_sync])/256)); // VFP[15:8]
	lt8911b_i2c_write(0x3c,(unsigned char)(MIPI_Timing[H_bp]%256)); // HBP[7:0]
	lt8911b_i2c_write(0x3d,(unsigned char)(MIPI_Timing[H_bp]/256)); // HBP[15:8]
	lt8911b_i2c_write(0x3e,(unsigned char)((MIPI_Timing[H_tol]-MIPI_Timing[H_act]-MIPI_Timing[H_bp]-MIPI_Timing[H_sync])%256)); // HFP[7:0]
	lt8911b_i2c_write(0x3f,(unsigned char)((MIPI_Timing[H_tol]-MIPI_Timing[H_act]-MIPI_Timing[H_bp]-MIPI_Timing[H_sync])/256)); // HFP[15:8]
	//	DDSConfig();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==7\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x49;

	slave_addr= 0x49;
	//lt8911b_i2c_write(0x4e,0x6A);
	lt8911b_i2c_write(0x4e,0x52);
	//lt8911b_i2c_write(0x4f,0x4D);
	lt8911b_i2c_write(0x4f,0xDE);
	//lt8911b_i2c_write(0x50,0xF3);
	lt8911b_i2c_write(0x50,0xc0);
	lt8911b_i2c_write(0x51,0x80);
	lt8911b_i2c_write(0x51,0x00);

	//lt8911b_i2c_write(0x1f,0x90);
	lt8911b_i2c_write(0x1f,0x5E);
	lt8911b_i2c_write(0x20,0x01);
	//lt8911b_i2c_write(0x21,0x68);
	lt8911b_i2c_write(0x21,0x2C);
	lt8911b_i2c_write(0x22,0x01);
	//lt8911b_i2c_write(0x23,0x5E);
	lt8911b_i2c_write(0x23,0xFA);
	//lt8911b_i2c_write(0x24,0x01);
	lt8911b_i2c_write(0x24,0x00);
	//lt8911b_i2c_write(0x25,0x54);
	lt8911b_i2c_write(0x25,0xC8);
	//lt8911b_i2c_write(0x26,0x01);
	lt8911b_i2c_write(0x26,0x00);

	//lt8911b_i2c_write(0x27,0x90);
	lt8911b_i2c_write(0x27,0x5E);
	lt8911b_i2c_write(0x28,0x01);
	//lt8911b_i2c_write(0x29,0x68);
	lt8911b_i2c_write(0x29,0x2C);
	lt8911b_i2c_write(0x2a,0x01);
	//lt8911b_i2c_write(0x2b,0x5E);
	lt8911b_i2c_write(0x2b,0xFA);
	//lt8911b_i2c_write(0x2c,0x01);
	lt8911b_i2c_write(0x2c,0x00);
	//lt8911b_i2c_write(0x2d,0x54);
	lt8911b_i2c_write(0x2d,0xC8);
	//lt8911b_i2c_write(0x2e,0x01);
	lt8911b_i2c_write(0x2e,0x00);

	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48;

	slave_addr= 0x48;
	lt8911b_i2c_write(0x03,0x7F);
	mdelay(10);
	lt8911b_i2c_write(0x03,0xFF);


	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x49;
	slave_addr= 0x49;
	lt8911b_i2c_write(0x42,0x64);
	lt8911b_i2c_write(0x43,0x00);
	lt8911b_i2c_write(0x44,0x04);
	lt8911b_i2c_write(0x45,0x00);
	lt8911b_i2c_write(0x46,0x59);
	lt8911b_i2c_write(0x47,0x00);
	lt8911b_i2c_write(0x48,0xf2);
	lt8911b_i2c_write(0x49,0x06);
	lt8911b_i2c_write(0x4a,0x00);
	lt8911b_i2c_write(0x4b,0x72);
	lt8911b_i2c_write(0x4c,0x45);
	lt8911b_i2c_write(0x4d,0x00);
	lt8911b_i2c_write(0x52,0x08);
	lt8911b_i2c_write(0x53,0x00);
	lt8911b_i2c_write(0x54,0xb2);
	lt8911b_i2c_write(0x55,0x00);
	lt8911b_i2c_write(0x56,0xe4);
	lt8911b_i2c_write(0x57,0x0d);
	lt8911b_i2c_write(0x58,0x00);
	lt8911b_i2c_write(0x59,0xe4);
	lt8911b_i2c_write(0x5a,0x8a);
	lt8911b_i2c_write(0x5b,0x00);
	lt8911b_i2c_write(0x5c,0x34);
	lt8911b_i2c_write(0x1e,0x4f);
	lt8911b_i2c_write(0x51,0x00);
	
	
	//	AudioIIsEn();
	dprintf(CRITICAL,"stephen go to mdss_mipi_i2c_init==8\n");
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x48;
	slave_addr= 0x48;
	lt8911b_i2c_write(0xB2,0x01);
	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x4a;
	slave_addr= 0x4a;
	lt8911b_i2c_write(0x06,0x08);
	lt8911b_i2c_write(0x07,0xF0);
	lt8911b_i2c_write(0x34,0xD2);
		
	
	lt8911b_i2c_write(0x3c,0x41);//add
		

	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x4a; //add
	slave_addr= 0x4a;
	lt8911b_i2c_write(0x3e,0x0A);//add --

	//HDMI_VIC = 0x00; 
	HDMI_VIC = 0x10; // 0x04:720P 60; 0x10:1080P 60; Corresponding to the resolution to be output 
	lt8911b_i2c_write(0x43,0x46 - HDMI_VIC);
	lt8911b_i2c_write(0x44,0x10);
	lt8911b_i2c_write(0x45,0x19);
	lt8911b_i2c_write(0x47,0x00 + HDMI_VIC);//add ++
	

		
	//	MIPIRxLogicRes();
	lt8911b_i2c_write(0x03,0x7f);
	//mdelay(100);
	mdelay(10); //modify
	lt8911b_i2c_write(0x03,0xff);


	//my_mipi_i2c->mdss_mipi_i2c_client->addr = 0x49;
	slave_addr= 0x49;
	lt8911b_i2c_write(0x51,0x80);
	mdelay(10); 
	lt8911b_i2c_write(0x51,0x00);
		
#ifdef TEST_PATTERN_MODE

	Test_pattern_1280x720();

#endif


	dprintf(CRITICAL,"eliot :mdss_mipi_i2c_init end \n");
	
}
//---------------------------------------------//

// MIPI输入点 eDP屏的 LT8911B 寄存器设置：

//**************************************************//
// 确定要在给LT8911B 提供MIPI信号之后，再初始化LT8911B寄存器。
//*************************************************** //

void LT8911B_Initial(void)
{	
	//unsigned char count;

	//count=0;
	LK_lt8911_i2c_init();//初始化i2c

//L1: 
lt8911_reg_init();
	
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
