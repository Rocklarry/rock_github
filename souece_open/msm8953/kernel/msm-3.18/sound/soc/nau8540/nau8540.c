/*
 * NAU85L40 ALSA SoC audio driver
 *
 * Copyright 2016 Nuvoton Technology Corp.
 * Author: John Hsu <KCHSU0@nuvoton.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include "nau8540.h"
static struct nau8540 *nau8540_back = NULL;

#define NAU_FREF_MAX 13500000
#define NAU_FVCO_MAX 100000000
#define NAU_FVCO_MIN 90000000

/* the maximum frequency of CLK_ADC */
#define CLK_ADC_MAX 6144000

/* scaling for mclk from sysclk_src output */
static const struct nau8540_fll_attr mclk_src_scaling[] = {
	{ 1, 0x0 },
	{ 2, 0x2 },
	{ 4, 0x3 },
	{ 8, 0x4 },
	{ 16, 0x5 },
	{ 32, 0x6 },
	{ 3, 0x7 },
	{ 6, 0xa },
	{ 12, 0xb },
	{ 24, 0xc },
};

/* ratio for input clk freq */
static const struct nau8540_fll_attr fll_ratio[] = {
	{ 512000, 0x01 },
	{ 256000, 0x02 },
	{ 128000, 0x04 },
	{ 64000, 0x08 },
	{ 32000, 0x10 },
	{ 8000, 0x20 },
	{ 4000, 0x40 },
};

static const struct nau8540_fll_attr fll_pre_scalar[] = {
	{ 1, 0x0 },
	{ 2, 0x1 },
	{ 4, 0x2 },
	{ 8, 0x3 },
};

/* over sampling rate */
static const struct nau8540_osr_attr osr_adc_sel[] = {
	{ 32, 3 },	/* OSR 32, SRC 1/8 */
	{ 64, 2 },	/* OSR 64, SRC 1/4 */
	{ 128, 1 },	/* OSR 128, SRC 1/2 */
	{ 256, 0 },	/* OSR 256, SRC 1 */
};

static const struct reg_default nau8540_reg_defaults[] = {
	{NAU8540_REG_POWER_MANAGEMENT, 0x000F},  //0x0000
	{NAU8540_REG_CLOCK_CTRL, 0x8002},
	{NAU8540_REG_CLOCK_SRC, 0x8083},
	{NAU8540_REG_FLL1, 0x1810},
	{NAU8540_REG_FLL2, 0x0000},
	{NAU8540_REG_FLL3, 0x0c80},
	{NAU8540_REG_FLL4, 0x0000},
	{NAU8540_REG_FLL5, 0xC000},
	{NAU8540_REG_FLL6, 0x6000},
	{NAU8540_REG_FLL_VCO_RSV, 0xF13C},
	{NAU8540_REG_PCM_CTRL0, 0x0002},
	{NAU8540_REG_PCM_CTRL1, 0x3013},
	{NAU8540_REG_PCM_CTRL2, 0x0800},
	{NAU8540_REG_PCM_CTRL3, 0x0000},
	{NAU8540_REG_PCM_CTRL4, 0x000F},
	{NAU8540_REG_ALC_CONTROL_1, 0x0000},
	{NAU8540_REG_ALC_CONTROL_2, 0x700B},
	{NAU8540_REG_ALC_CONTROL_3, 0x0022},
	{NAU8540_REG_ALC_CONTROL_4, 0x1010},
	{NAU8540_REG_ALC_CONTROL_5, 0x1010},
	{NAU8540_REG_NOTCH_FIL1_CH1, 0x0000},
	{NAU8540_REG_NOTCH_FIL2_CH1, 0x0000},
	{NAU8540_REG_NOTCH_FIL1_CH2, 0x0000},
	{NAU8540_REG_NOTCH_FIL2_CH2, 0x0000},
	{NAU8540_REG_NOTCH_FIL1_CH3, 0x0000},
	{NAU8540_REG_NOTCH_FIL2_CH3, 0x0000},
	{NAU8540_REG_NOTCH_FIL1_CH4, 0x0000},
	{NAU8540_REG_NOTCH_FIL2_CH4, 0x0000},
	{NAU8540_REG_HPF_FILTER_CH12, 0x0808},
	{NAU8540_REG_HPF_FILTER_CH34, 0x0808},
	{NAU8540_REG_HPF_FILTER_CH56, 0x0808},
	{NAU8540_REG_HPF_FILTER_CH78, 0x0808},
	{NAU8540_REG_ADC_SAMPLE_RATE, 0x4063},
	{NAU8540_REG_DIGITAL_GAIN_CH1, 0x0400},    //0x0490
	{NAU8540_REG_DIGITAL_GAIN_CH2, 0x0400},    //0x0490
	{NAU8540_REG_DIGITAL_GAIN_CH3, 0x0400},    //0x0400
	{NAU8540_REG_DIGITAL_GAIN_CH4, 0x0400},    //0x0400
	
  {NAU8540_REG_DIGITAL_GAIN_CH4, 0x0400},    //0x0490
	{NAU8540_REG_DIGITAL_GAIN_CH5, 0x0400},    //0x0490
	{NAU8540_REG_DIGITAL_GAIN_CH6, 0x0400},    //0x0400
	{NAU8540_REG_DIGITAL_GAIN_CH7, 0x0400},    //0x0400
	
	{NAU8540_REG_DIGITAL_MUX, 0x00B4},
	{NAU8540_REG_GPIO_CTRL, 0x0000},
	{NAU8540_REG_MISC_CTRL, 0x0000},
	{NAU8540_REG_I2C_CTRL, 0xEFFF},
	{NAU8540_REG_VMID_CTRL, 0x0060},
	{NAU8540_REG_MUTE, 0x0000},
	{NAU8540_REG_ANALOG_ADC1, 0x0011},
	{NAU8540_REG_ANALOG_ADC2, 0x0020},
	{NAU8540_REG_ANALOG_PWR, 0x000F},  //0x0000
	{NAU8540_REG_MIC_BIAS, 0x0f04},	//0x0004
	{NAU8540_REG_REFERENCE, 0x3000},
	{NAU8540_REG_FEPGA1, 0x0000},
	{NAU8540_REG_FEPGA2, 0x0000},
	{NAU8540_REG_FEPGA3, 0x0101},	//0x1313
	{NAU8540_REG_FEPGA4, 0x0101},	//0x0101
	{NAU8540_REG_FEPGA5, 0x0101},	//0x1313
	{NAU8540_REG_FEPGA6, 0x0101},	//0x0101
	{NAU8540_REG_PWR, 0xF000},	//0x0000
};
static int  nau8540_reg=0;	
static int  nau8540_reg_write=0;				      
static ssize_t nau8540_reg_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{

   return scnprintf(buf, PAGE_SIZE, "write reg=0x%02x val=0x%04x\n",nau8540_reg,nau8540_reg_write);
}


struct i2c_client *nau8540_client;

static int nau8540_i2c_read(u8 *addr,u8 *buf)
{
	int ret = -EIO;
	u8 retries;
	struct i2c_client *client=nau8540_client;
	struct i2c_msg msgs[2] = {
		{
			.flags	= !I2C_M_RD,
			.addr	= client->addr,
			.len	= 2,
			.buf	= &addr[0],
		},
		{
			.flags	= I2C_M_RD,
			.addr	= client->addr,
			.len	= 2,
			.buf	= &buf[0],
		},
	};

	for (retries = 0; retries < 5; retries++) {
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret == 2)
			break;
		dev_err(&client->dev, "I2C retry: %d\n", retries + 1);
	}
	return ret;
}

static ssize_t nau8540_reg_show_all(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint reg;
	uint def_val;
	uint cur_val;
	uint index;
	unsigned char addr_data[2];
	unsigned char read_data[2];
	char buff1[20] = { 0 };
	char buff[1024] = { 0 };

	strcpy(buff, (char *)"nau8540 reg value:\nreg   def  cur\n");

	for (index=0; index<ARRAY_SIZE(nau8540_reg_defaults); index++)
	{
		reg = nau8540_reg_defaults[index].reg;
		def_val = nau8540_reg_defaults[index].def;
		//regmap_read(nau8540_back->regmap, reg, &cur_val );
		addr_data[0]=0;
		addr_data[1]=reg;
		nau8540_i2c_read(&addr_data[0],&read_data[0]);
		cur_val=(read_data[0]<<8&0xff00)|read_data[1];
		scnprintf(buff1, PAGE_SIZE, "0x%02x: %04x %04x\n", reg, def_val, cur_val);
		strncat(buff, buff1, strlen(buff1));
	}
	
	return scnprintf(buf, PAGE_SIZE, "%s\n", buff);
}


static ssize_t nau8540_reg_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	
	unsigned long reg,write;
	unsigned char buf1[50]={0};
	unsigned char buf2[50]={0};
	int i,j,k;
	int find=0;
	int ret;
  for(i=0;i<20;i++)
  {
  	if(buf[i]>=0x30&&buf[i]<=0x39)
  		{
  			buf1[i]=buf[i];
  			find=1;
  		}
  	 if((0x20==buf[i])&&(1==find))
  	 	break;
  }
  buf1[i]=0;
  k=0;
  find=0;
  for(j=i;j<20;j++)
  {
  	if(buf[j]>=0x30&&buf[j]<=0x39)
  		{
  			 buf2[k++]=buf[j];
  			 find=1;
  	  }
  		if((0x20==buf[j])&&(1==find))
  		  break;
  }
  buf2[k]=0;
	ret = kstrtoul(buf1, 0, &reg);
	if (ret)
		pr_debug("%s is not in hex or decimal form.\n", buf);
	else
		pr_debug("%s reg %ld\n",__func__,reg);
		
	ret = kstrtoul(buf2, 0, &write);
	if (ret)
		pr_debug("%s is not in hex or decimal form.\n", buf);
	else
		pr_debug("%s write %ld\n",__func__,write);
		nau8540_reg=reg;
		nau8540_reg_write=write;
  regmap_write(nau8540_back->regmap, reg, write);
	return strnlen(buf, count);
}
			      
static struct device_attribute attrs[] = {
		__ATTR(nau8540_reg_ctl, (S_IRUGO | S_IWUSR | S_IWGRP),
				nau8540_reg_show,
				nau8540_reg_store),
		__ATTR(nau8540_reg_all, (S_IRUGO),
				nau8540_reg_show_all,
				NULL),
};


static bool nau8540_readable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case NAU8540_REG_POWER_MANAGEMENT ... NAU8540_REG_FLL_VCO_RSV:
	case NAU8540_REG_PCM_CTRL0 ... NAU8540_REG_PCM_CTRL4:
	case NAU8540_REG_ALC_CONTROL_1 ... NAU8540_REG_ALC_CONTROL_5:
	case NAU8540_REG_ALC_GAIN_CH12 ... NAU8540_REG_ADC_SAMPLE_RATE:
	case NAU8540_REG_DIGITAL_GAIN_CH1 ... NAU8540_REG_DIGITAL_MUX:
	case NAU8540_REG_P2P_CH1 ... NAU8540_REG_I2C_CTRL:
	case NAU8540_REG_I2C_DEVICE_ID:
	case NAU8540_REG_VMID_CTRL ... NAU8540_REG_MUTE:
	case NAU8540_REG_ANALOG_ADC1 ... NAU8540_REG_PWR:
  case NAU8540_REG_DIGITAL_GAIN_CH5 ... NAU8540_REG_FEPGA6:
		return true;
	default:
		return false;
	}

}

static bool nau8540_writeable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case NAU8540_REG_SW_RESET ... NAU8540_REG_FLL_VCO_RSV:
	case NAU8540_REG_PCM_CTRL0 ... NAU8540_REG_PCM_CTRL4:
	case NAU8540_REG_ALC_CONTROL_1 ... NAU8540_REG_ALC_CONTROL_5:
	case NAU8540_REG_NOTCH_FIL1_CH1 ... NAU8540_REG_ADC_SAMPLE_RATE:
	case NAU8540_REG_DIGITAL_GAIN_CH1 ... NAU8540_REG_DIGITAL_MUX:
	case NAU8540_REG_GPIO_CTRL ... NAU8540_REG_I2C_CTRL:
	case NAU8540_REG_RST:
	case NAU8540_REG_VMID_CTRL ... NAU8540_REG_MUTE:
	case NAU8540_REG_ANALOG_ADC1 ... NAU8540_REG_PWR:
	case NAU8540_REG_DIGITAL_GAIN_CH5 ... NAU8540_REG_FEPGA6:
		return true;
	default:
		return false;
	}
}

static bool nau8540_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case NAU8540_REG_SW_RESET:
	case NAU8540_REG_ALC_GAIN_CH12 ... NAU8540_REG_ALC_STATUS:
	case NAU8540_REG_P2P_CH1 ... NAU8540_REG_PEAK_CH4:
	case NAU8540_REG_I2C_DEVICE_ID:
	case NAU8540_REG_RST:
		return true;
	default:
		return false;
	}
}

//TLV: Type-Lenght-Value,意思：数组的第0各元素代表数据的类型，第1个元素代表数据的长度，第三个元素和之后的元素保存该变量的数据。
static const DECLARE_TLV_DB_MINMAX(adc_vol_tlv, -12800, 3600);
static const DECLARE_TLV_DB_MINMAX(fepga_gain_tlv, -100, 3600);
static const DECLARE_TLV_DB_MINMAX(hfp_filter, 112, 612);

static const struct snd_kcontrol_new nau8540_snd_controls[] = {
	SOC_SINGLE_TLV("Mic1 Volume", NAU8540_REG_DIGITAL_GAIN_CH1,
		0, 0x520, 0, adc_vol_tlv),
	SOC_SINGLE_TLV("Mic2 Volume", NAU8540_REG_DIGITAL_GAIN_CH2,
		0, 0x520, 0, adc_vol_tlv),
	SOC_SINGLE_TLV("Mic3 Volume", NAU8540_REG_DIGITAL_GAIN_CH3,
		0, 0x520, 0, adc_vol_tlv),
	SOC_SINGLE_TLV("Mic4 Volume", NAU8540_REG_DIGITAL_GAIN_CH4,
		0, 0x520, 0, adc_vol_tlv), //(xname, reg, shift, max, invert, tlv_array)
		

	SOC_SINGLE("Mic1 Filter Enable", NAU8540_REG_HPF_FILTER_CH12,
		4, 1, 0),
	SOC_SINGLE("Mic2 Filter Enable", NAU8540_REG_HPF_FILTER_CH12,
		12, 1, 0),
	SOC_SINGLE("Mic3 Filter Enable", NAU8540_REG_HPF_FILTER_CH34,
		4, 1, 0),
	SOC_SINGLE("Mic4 Filter Enable", NAU8540_REG_HPF_FILTER_CH34,
		12, 1, 0),
		

	SOC_SINGLE_TLV("Mic1 Filter", NAU8540_REG_HPF_FILTER_CH12,
		0, 7, 0, hfp_filter),
	SOC_SINGLE_TLV("Mic2 Filter", NAU8540_REG_HPF_FILTER_CH12,
		8, 7, 0, hfp_filter),
	SOC_SINGLE_TLV("Mic3 Filter", NAU8540_REG_HPF_FILTER_CH34,
		0, 7, 0, hfp_filter),
	SOC_SINGLE_TLV("Mic4 Filter", NAU8540_REG_HPF_FILTER_CH34,
		8, 7, 0, hfp_filter),
		
		
	SOC_SINGLE_TLV("Frontend PGA1 Volume", NAU8540_REG_FEPGA3,
		0, 0x25, 0, fepga_gain_tlv),
	SOC_SINGLE_TLV("Frontend PGA2 Volume", NAU8540_REG_FEPGA3,
		8, 0x25, 0, fepga_gain_tlv),
	SOC_SINGLE_TLV("Frontend PGA3 Volume", NAU8540_REG_FEPGA4,
		0, 0x25, 0, fepga_gain_tlv),
	SOC_SINGLE_TLV("Frontend PGA4 Volume", NAU8540_REG_FEPGA4,
		8, 0x25, 0, fepga_gain_tlv),
		
};

static const char * const adc_channel[] = {
	"ADC channel 1", "ADC channel 2", "ADC channel 3", "ADC channel 4"
};
static SOC_ENUM_SINGLE_DECL(
	digital_ch4_enum, NAU8540_REG_DIGITAL_MUX, 6, adc_channel);

static const struct snd_kcontrol_new digital_ch4_mux =
	SOC_DAPM_ENUM("Digital CH4 Select", digital_ch4_enum);

static SOC_ENUM_SINGLE_DECL(
	digital_ch3_enum, NAU8540_REG_DIGITAL_MUX, 4, adc_channel);

static const struct snd_kcontrol_new digital_ch3_mux =
	SOC_DAPM_ENUM("Digital CH3 Select", digital_ch3_enum);

static SOC_ENUM_SINGLE_DECL(
	digital_ch2_enum, NAU8540_REG_DIGITAL_MUX, 2, adc_channel);

static const struct snd_kcontrol_new digital_ch2_mux =
	SOC_DAPM_ENUM("Digital CH2 Select", digital_ch2_enum);

static SOC_ENUM_SINGLE_DECL(
	digital_ch1_enum, NAU8540_REG_DIGITAL_MUX, 0, adc_channel);

static const struct snd_kcontrol_new digital_ch1_mux =
	SOC_DAPM_ENUM("Digital CH1 Select", digital_ch1_enum);

static const struct snd_soc_dapm_widget nau8540_dapm_widgets[] = {
	SND_SOC_DAPM_SUPPLY("MICBIAS2", NAU8540_REG_MIC_BIAS, 11, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("MICBIAS1", NAU8540_REG_MIC_BIAS, 10, 0, NULL, 0),

	SND_SOC_DAPM_INPUT("MIC1"),
	SND_SOC_DAPM_INPUT("MIC2"),
	SND_SOC_DAPM_INPUT("MIC3"),
	SND_SOC_DAPM_INPUT("MIC4"),

	SND_SOC_DAPM_PGA("Frontend PGA1", NAU8540_REG_PWR, 12, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Frontend PGA2", NAU8540_REG_PWR, 13, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Frontend PGA3", NAU8540_REG_PWR, 14, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Frontend PGA4", NAU8540_REG_PWR, 15, 0, NULL, 0),

	SND_SOC_DAPM_ADC("ADC1", NULL,
		NAU8540_REG_POWER_MANAGEMENT, 0, 0),
	SND_SOC_DAPM_ADC("ADC2", NULL,
		NAU8540_REG_POWER_MANAGEMENT, 1, 0),
	SND_SOC_DAPM_ADC("ADC3", NULL,
		NAU8540_REG_POWER_MANAGEMENT, 2, 0),
	SND_SOC_DAPM_ADC("ADC4", NULL,
		NAU8540_REG_POWER_MANAGEMENT, 3, 0),

	SND_SOC_DAPM_PGA("ADC CH1", NAU8540_REG_ANALOG_PWR, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("ADC CH2", NAU8540_REG_ANALOG_PWR, 1, 0, NULL, 0),
	SND_SOC_DAPM_PGA("ADC CH3", NAU8540_REG_ANALOG_PWR, 2, 0, NULL, 0),
	SND_SOC_DAPM_PGA("ADC CH4", NAU8540_REG_ANALOG_PWR, 3, 0, NULL, 0),

	SND_SOC_DAPM_MUX("Digital CH4 Mux",
		SND_SOC_NOPM, 0, 0, &digital_ch4_mux),
	SND_SOC_DAPM_MUX("Digital CH3 Mux",
		SND_SOC_NOPM, 0, 0, &digital_ch3_mux),
	SND_SOC_DAPM_MUX("Digital CH2 Mux",
		SND_SOC_NOPM, 0, 0, &digital_ch2_mux),
	SND_SOC_DAPM_MUX("Digital CH1 Mux",
		SND_SOC_NOPM, 0, 0, &digital_ch1_mux),

	//SND_SOC_DAPM_AIF_OUT("AIFTX", "Capture", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_IN("AIFTX8540", "Capture8540", 0, SND_SOC_NOPM, 0, 0),
};

static const struct snd_soc_dapm_route nau8540_dapm_routes[] = {
	{"Frontend PGA1", NULL, "MIC1"},
	{"Frontend PGA2", NULL, "MIC2"},
	{"Frontend PGA3", NULL, "MIC3"},
	{"Frontend PGA4", NULL, "MIC4"},

	{"ADC1", NULL, "Frontend PGA1"},
	{"ADC2", NULL, "Frontend PGA2"},
	{"ADC3", NULL, "Frontend PGA3"},
	{"ADC4", NULL, "Frontend PGA4"},

	{"ADC CH1", NULL, "ADC1"},
	{"ADC CH2", NULL, "ADC2"},
	{"ADC CH3", NULL, "ADC3"},
	{"ADC CH4", NULL, "ADC4"},

	{"ADC1", NULL, "MICBIAS1"},
	{"ADC2", NULL, "MICBIAS1"},
	{"ADC3", NULL, "MICBIAS2"},
	{"ADC4", NULL, "MICBIAS2"},

	{"Digital CH1 Mux", "ADC channel 1", "ADC CH1"},
	{"Digital CH1 Mux", "ADC channel 2", "ADC CH2"},
	{"Digital CH1 Mux", "ADC channel 3", "ADC CH3"},
	{"Digital CH1 Mux", "ADC channel 4", "ADC CH4"},

	{"Digital CH2 Mux", "ADC channel 1", "ADC CH1"},
	{"Digital CH2 Mux", "ADC channel 2", "ADC CH2"},
	{"Digital CH2 Mux", "ADC channel 3", "ADC CH3"},
	{"Digital CH2 Mux", "ADC channel 4", "ADC CH4"},

	{"Digital CH3 Mux", "ADC channel 1", "ADC CH1"},
	{"Digital CH3 Mux", "ADC channel 2", "ADC CH2"},
	{"Digital CH3 Mux", "ADC channel 3", "ADC CH3"},
	{"Digital CH3 Mux", "ADC channel 4", "ADC CH4"},

	{"Digital CH4 Mux", "ADC channel 1", "ADC CH1"},
	{"Digital CH4 Mux", "ADC channel 2", "ADC CH2"},
	{"Digital CH4 Mux", "ADC channel 3", "ADC CH3"},
	{"Digital CH4 Mux", "ADC channel 4", "ADC CH4"},

	{"AIFTX8540", NULL, "Digital CH1 Mux"},
	{"AIFTX8540", NULL, "Digital CH2 Mux"},
	{"AIFTX8540", NULL, "Digital CH3 Mux"},
	{"AIFTX8540", NULL, "Digital CH4 Mux"},
};

static int nau8540_clock_check(struct nau8540 *nau8540, int rate, int osr)
{
	int osrate;

	if (osr >= ARRAY_SIZE(osr_adc_sel))
		return -EINVAL;
	osrate = osr_adc_sel[osr].osr;

	if (rate * osr > CLK_ADC_MAX) {
		dev_err(nau8540->dev, "exceed the maximum frequency of CLK_ADC\n");
		return -EINVAL;
	}

	return 0;
}

static int nau8540_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct nau8540 *nau8540 = snd_soc_codec_get_drvdata(codec);
	unsigned int val_len = 0, osr;


	/* CLK_ADC = OSR * FS
	 * ADC clock frequency is defined as Over Sampling Rate (OSR)
	 * multiplied by the audio sample rate (Fs). Note that the OSR and Fs
	 * values must be selected such that the maximum frequency is less
	 * than 6.144 MHz.
	 */

	//pr_debug("[fanhang]%s rate=%d\n", __func__, params_rate(params));
	
	regmap_read(nau8540->regmap, NAU8540_REG_ADC_SAMPLE_RATE, &osr);
	osr &= NAU8540_ADC_OSR_MASK;
	if (nau8540_clock_check(nau8540, params_rate(params), osr))
		return -EINVAL;

	//pr_debug("[fanhang]%s osr=%d\n", __func__, osr);

	
	regmap_update_bits(nau8540->regmap, NAU8540_REG_CLOCK_SRC,
	NAU8540_CLK_SRC_MASK,
	0<<15);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_CLOCK_SRC,
	NAU8540_CLK_SRC_MASK,
	1<<15);
	regmap_write(nau8540->regmap,NAU8540_REG_RST,1);
	regmap_write(nau8540->regmap,NAU8540_REG_RST,0);
	mdelay(5);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_CLOCK_SRC,
		NAU8540_CLK_ADC_SRC_MASK,
		osr_adc_sel[osr].clk_src << NAU8540_CLK_ADC_SRC_SFT);
  //pr_debug("%s [fanhang] params_width(params)=%d!\n",__func__,params_width(params));
	switch (params_width(params)) {
	case 16:
		val_len |= NAU8540_I2S_DL_16;
		break;
	case 20:
		val_len |= NAU8540_I2S_DL_20;
		break;
	case 24:
		val_len |= NAU8540_I2S_DL_24;
		break;
	case 32:
		val_len |= NAU8540_I2S_DL_32;
		break;
	default:
		return -EINVAL;
	}
	
//	pr_debug("[fanhang] nau8540_hw_params enter2 !\n");

	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL0,
		NAU8540_I2S_DL_MASK, val_len);

	return 0;
}

static int nau8540_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct nau8540 *nau8540 = snd_soc_codec_get_drvdata(codec);
	unsigned int ctrl1_val = 0, ctrl2_val = 0;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		ctrl2_val |= NAU8540_I2S_MS_MASTER;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_NF:
		ctrl1_val |= NAU8540_I2S_BP_INV;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		ctrl1_val |= NAU8540_I2S_DF_I2S;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		ctrl1_val |= NAU8540_I2S_DF_LEFT;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		ctrl1_val |= NAU8540_I2S_DF_RIGTH;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		ctrl1_val |= NAU8540_I2S_DF_PCM_AB;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		ctrl1_val |= NAU8540_I2S_DF_PCM_AB;
		ctrl1_val |= NAU8540_I2S_PCMB_EN;
		break;
	default:
		return -EINVAL;
	}

	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL0,
		NAU8540_I2S_DL_MASK | NAU8540_I2S_DF_MASK |
		NAU8540_I2S_BP_INV | NAU8540_I2S_PCMB_EN, ctrl1_val);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL1,
		NAU8540_I2S_MS_MASK | NAU8540_I2S_DO12_OE, ctrl2_val);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL2,
		NAU8540_I2S_DO34_OE, 0);

	return 0;
}

/**
 * nau8540_set_tdm_slot - configure DAI TX TDM.
 * @dai: DAI
 * @tx_mask: bitmask representing active TX slots. Ex.
 *                 0xf for normal 4 channel TDM.
 *                 0xf0 for shifted 4 channel TDM
 * @rx_mask: no used.
 * @slots: Number of slots in use.
 * @slot_width: Width in bits for each slot.
 *
 * Configures a DAI for TDM operation. Only support 4 slots TDM.
 */
static int nau8540_set_tdm_slot(struct snd_soc_dai *dai,
	unsigned int tx_mask, unsigned int rx_mask, int slots, int slot_width)
{
	struct snd_soc_codec *codec = dai->codec;
	struct nau8540 *nau8540 = snd_soc_codec_get_drvdata(codec);
	unsigned int ctrl2_val = 0, ctrl4_val = 0;

	if (slots > 4 || ((tx_mask & 0xf0) && (tx_mask & 0xf)))
		return -EINVAL;

	ctrl4_val |= (NAU8540_TDM_MODE | NAU8540_TDM_OFFSET_EN);
	if (tx_mask & 0xf0) {
		ctrl2_val = 4 * slot_width;
		ctrl4_val |= (tx_mask >> 4);
	} else {
		ctrl4_val |= tx_mask;
	}
	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL4,
		NAU8540_TDM_MODE | NAU8540_TDM_OFFSET_EN |
		NAU8540_TDM_TX_MASK, ctrl4_val);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL1,
		NAU8540_I2S_DO12_OE, NAU8540_I2S_DO12_OE);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_PCM_CTRL2,
		NAU8540_I2S_DO34_OE | NAU8540_I2S_TSLOT_L_MASK,
		NAU8540_I2S_DO34_OE | ctrl2_val);

	return 0;
}


static const struct snd_soc_dai_ops nau8540_dai_ops = {
	.hw_params = nau8540_hw_params,
	.set_fmt = nau8540_set_fmt,
	.set_tdm_slot = nau8540_set_tdm_slot,
};

#define NAU8540_RATES SNDRV_PCM_RATE_8000_48000
#define NAU8540_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE \
	 | SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S32_LE)

static struct snd_soc_dai_driver nau8540_dai = {
	.name = "nau8540-hifi",
	.capture = {
		.stream_name = "Capture8540",
		.channels_min = 1,
		.channels_max = 8,
		.rates = NAU8540_RATES,
		.formats = NAU8540_FORMATS,
	},
	.ops = &nau8540_dai_ops,
};

/**
 * nau8540_calc_fll_param - Calculate FLL parameters.
 * @fll_in: external clock provided to codec.
 * @fs: sampling rate.
 * @fll_param: Pointer to structure of FLL parameters.
 *
 * Calculate FLL parameters to configure codec.
 *
 * Returns 0 for success or negative error code.
 */
static int nau8540_calc_fll_param(unsigned int fll_in,
	unsigned int fs, struct nau8540_fll *fll_param)
{
	u64 fvco, fvco_max;
	unsigned int fref, i, fvco_sel;

	/* Ensure the reference clock frequency (FREF) is <= 13.5MHz by dividing
	 * freq_in by 1, 2, 4, or 8 using FLL pre-scalar.
	 * FREF = freq_in / NAU8540_FLL_REF_DIV_MASK
	 */
	for (i = 0; i < ARRAY_SIZE(fll_pre_scalar); i++) {
		fref = fll_in / fll_pre_scalar[i].param;
		if (fref <= NAU_FREF_MAX)
			break;
	}
	if (i == ARRAY_SIZE(fll_pre_scalar))
		return -EINVAL;
	fll_param->clk_ref_div = fll_pre_scalar[i].val;

	/* Choose the FLL ratio based on FREF */
	for (i = 0; i < ARRAY_SIZE(fll_ratio); i++) {
		if (fref >= fll_ratio[i].param)
			break;
	}
	if (i == ARRAY_SIZE(fll_ratio))
		return -EINVAL;
	fll_param->ratio = fll_ratio[i].val;

	/* Calculate the frequency of DCO (FDCO) given freq_out = 256 * Fs.
	 * FDCO must be within the 90MHz - 124MHz or the FFL cannot be
	 * guaranteed across the full range of operation.
	 * FDCO = freq_out * 2 * mclk_src_scaling
	 */
	fvco_max = 0;
	fvco_sel = ARRAY_SIZE(mclk_src_scaling);
	for (i = 0; i < ARRAY_SIZE(mclk_src_scaling); i++) {
		fvco = 256 * fs * 2 * mclk_src_scaling[i].param;
		if (fvco > NAU_FVCO_MIN && fvco < NAU_FVCO_MAX &&
			fvco_max < fvco) {
			fvco_max = fvco;
			fvco_sel = i;
		}
	}
	if (ARRAY_SIZE(mclk_src_scaling) == fvco_sel)
		return -EINVAL;
	fll_param->mclk_src = mclk_src_scaling[fvco_sel].val;

	/* Calculate the FLL 10-bit integer input and the FLL 16-bit fractional
	 * input based on FDCO, FREF and FLL ratio.
	 */
	fvco = div_u64(fvco_max << 16, fref * fll_param->ratio);
	fll_param->fll_int = (fvco >> 16) & 0x3FF;
	fll_param->fll_frac = fvco & 0xFFFF;
	return 0;
}

static void nau8540_fll_apply(struct regmap *regmap,
	struct nau8540_fll *fll_param)
{
	regmap_update_bits(regmap, NAU8540_REG_CLOCK_SRC,
		NAU8540_CLK_SRC_MASK | NAU8540_CLK_MCLK_SRC_MASK,
		NAU8540_CLK_SRC_MCLK | fll_param->mclk_src);
	regmap_update_bits(regmap, NAU8540_REG_FLL1,
		NAU8540_FLL_RATIO_MASK, fll_param->ratio);
	/* FLL 16-bit fractional input */
	regmap_write(regmap, NAU8540_REG_FLL2, fll_param->fll_frac);
	/* FLL 10-bit integer input */
	regmap_update_bits(regmap, NAU8540_REG_FLL3,
		NAU8540_FLL_INTEGER_MASK, fll_param->fll_int);
	/* FLL pre-scaler */
	regmap_update_bits(regmap, NAU8540_REG_FLL4,
		NAU8540_FLL_REF_DIV_MASK,
		fll_param->clk_ref_div << NAU8540_FLL_REF_DIV_SFT);
	regmap_update_bits(regmap, NAU8540_REG_FLL5,
		NAU8540_FLL_CLK_SW_MASK, NAU8540_FLL_CLK_SW_REF);
	regmap_update_bits(regmap,
		NAU8540_REG_FLL6, NAU8540_DCO_EN, 0);
	if (fll_param->fll_frac) {
		regmap_update_bits(regmap, NAU8540_REG_FLL5,
			NAU8540_FLL_PDB_DAC_EN | NAU8540_FLL_LOOP_FTR_EN |
			NAU8540_FLL_FTR_SW_MASK,
			NAU8540_FLL_PDB_DAC_EN | NAU8540_FLL_LOOP_FTR_EN |
			NAU8540_FLL_FTR_SW_FILTER);
		regmap_update_bits(regmap, NAU8540_REG_FLL6,
			NAU8540_SDM_EN, NAU8540_SDM_EN);
	} else {
		regmap_update_bits(regmap, NAU8540_REG_FLL5,
			NAU8540_FLL_PDB_DAC_EN | NAU8540_FLL_LOOP_FTR_EN |
			NAU8540_FLL_FTR_SW_MASK, NAU8540_FLL_FTR_SW_ACCU);
		regmap_update_bits(regmap,
			NAU8540_REG_FLL6, NAU8540_SDM_EN, 0);
	}
}

/* freq_out must be 256*Fs in order to achieve the best performance */
static int nau8540_set_pll(struct snd_soc_codec *codec, int pll_id, int source,
		unsigned int freq_in, unsigned int freq_out)
{
	struct nau8540 *nau8540 = snd_soc_codec_get_drvdata(codec);
	struct nau8540_fll fll_param;
	int ret, fs;

	fs = freq_out / 256;
	ret = nau8540_calc_fll_param(freq_in, fs, &fll_param);
	if (ret < 0) {
		dev_err(nau8540->dev, "Unsupported input clock %d\n", freq_in);
		return ret;
	}
	dev_dbg(nau8540->dev, "mclk_src=%x ratio=%x fll_frac=%x fll_int=%x clk_ref_div=%x\n",
		fll_param.mclk_src, fll_param.ratio, fll_param.fll_frac,
		fll_param.fll_int, fll_param.clk_ref_div);

	nau8540_fll_apply(nau8540->regmap, &fll_param);
	mdelay(2);
	regmap_update_bits(nau8540->regmap, NAU8540_REG_CLOCK_SRC,
		NAU8540_CLK_SRC_MASK, NAU8540_CLK_SRC_VCO);

	return 0;
}

static int nau8540_config_sysclk(struct nau8540 *nau8540,
	int clk_id, unsigned int freq)
{
	struct regmap *regmap = nau8540->regmap;

	switch (clk_id) {
	case NAU8540_CLK_DIS:
	case NAU8540_CLK_MCLK:
		regmap_update_bits(regmap, NAU8540_REG_CLOCK_SRC,
			NAU8540_CLK_SRC_MASK, NAU8540_CLK_SRC_MCLK);
		regmap_update_bits(regmap, NAU8540_REG_FLL6,
			NAU8540_DCO_EN, 0);
		break;

	case NAU8540_CLK_INTERNAL:
		regmap_update_bits(regmap, NAU8540_REG_FLL6,
			NAU8540_DCO_EN, NAU8540_DCO_EN);
		regmap_update_bits(regmap, NAU8540_REG_CLOCK_SRC,
			NAU8540_CLK_SRC_MASK, NAU8540_CLK_SRC_VCO);
		break;

	case NAU8540_CLK_FLL_MCLK:
		regmap_update_bits(regmap, NAU8540_REG_FLL3,
			NAU8540_FLL_CLK_SRC_MASK, NAU8540_FLL_CLK_SRC_MCLK);
		break;

	case NAU8540_CLK_FLL_BLK:
		regmap_update_bits(regmap, NAU8540_REG_FLL3,
			NAU8540_FLL_CLK_SRC_MASK, NAU8540_FLL_CLK_SRC_BLK);
		break;

	case NAU8540_CLK_FLL_FS:
		regmap_update_bits(regmap, NAU8540_REG_FLL3,
			NAU8540_FLL_CLK_SRC_MASK, NAU8540_FLL_CLK_SRC_FS);
		break;

	default:
		dev_err(nau8540->dev, "Invalid clock id (%d)\n", clk_id);
		return -EINVAL;
	}

	dev_dbg(nau8540->dev, "Sysclk is %dHz and clock id is %d\n", freq,
		clk_id);

	return 0;
}

static int nau8540_set_sysclk(struct snd_soc_codec *codec,
	int clk_id, int source, unsigned int freq, int dir)
{
	struct nau8540 *nau8540 = snd_soc_codec_get_drvdata(codec);

	return nau8540_config_sysclk(nau8540, clk_id, freq);
}

static void nau8540_reset_chip(struct regmap *regmap)
{
	regmap_write(regmap, NAU8540_REG_SW_RESET, 0x00);
	regmap_write(regmap, NAU8540_REG_SW_RESET, 0x00);
}

static void nau8540_init_regs(struct nau8540 *nau8540)
{
	struct regmap *regmap = nau8540->regmap;
	/* Enable Bias/VMID/VMID Tieoff */
	regmap_update_bits(regmap, NAU8540_REG_VMID_CTRL,
		NAU8540_VMID_EN | NAU8540_VMID_SEL_MASK,
		NAU8540_VMID_EN | (0x2 << NAU8540_VMID_SEL_SFT));
	regmap_update_bits(regmap, NAU8540_REG_REFERENCE,
		NAU8540_PRECHARGE_DIS | NAU8540_GLOBAL_BIAS_EN,
		NAU8540_PRECHARGE_DIS | NAU8540_GLOBAL_BIAS_EN);
	mdelay(2);
	regmap_update_bits(regmap, NAU8540_REG_MIC_BIAS,
		NAU8540_PU_PRE, NAU8540_PU_PRE);
	regmap_update_bits(regmap, NAU8540_REG_CLOCK_CTRL,
		NAU8540_CLK_ADC_EN | NAU8540_CLK_I2S_EN,
		NAU8540_CLK_ADC_EN | NAU8540_CLK_I2S_EN);
	/* ADC OSR selection, CLK_ADC = Fs * OSR */
	regmap_update_bits(regmap, NAU8540_REG_ADC_SAMPLE_RATE,
		NAU8540_ADC_OSR_MASK, NAU8540_ADC_OSR_256);
}

static struct snd_soc_codec_driver nau8540_codec_driver = {
	.set_sysclk = nau8540_set_sysclk,
	.set_pll = nau8540_set_pll,
	.suspend_bias_off = true,

	.component_driver = {
		.controls = nau8540_snd_controls,
		.num_controls = ARRAY_SIZE(nau8540_snd_controls),
		.dapm_widgets = nau8540_dapm_widgets,
		.num_dapm_widgets = ARRAY_SIZE(nau8540_dapm_widgets),
		.dapm_routes = nau8540_dapm_routes,
		.num_dapm_routes = ARRAY_SIZE(nau8540_dapm_routes),
	},
};

static const struct regmap_config nau8540_regmap_config = {
	.val_bits = 16,
	.reg_bits = 16,

	.max_register = NAU8540_REG_MAX,
	.readable_reg = nau8540_readable_reg,
	.writeable_reg = nau8540_writeable_reg,
	.volatile_reg = nau8540_volatile_reg,

	.cache_type = REGCACHE_RBTREE,
	.reg_defaults = nau8540_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(nau8540_reg_defaults),
};

#define PINCTRL_STATE_ACTIVE	"codec_power_en"
#define PINCTRL_STATE_SUSPEND	"codec_power_off"
static struct pinctrl *codec_en_pinctrl;

static struct pinctrl_state *pinctrl_state_active;
static struct pinctrl_state *pinctrl_state_suspend;
extern int have_init_nau8540;
static int nau8540_i2c_probe(struct i2c_client *i2c,
	const struct i2c_device_id *id)
{
	struct device *dev = &i2c->dev;
	struct nau8540 *nau8540 = dev_get_platdata(dev);
	int ret, value;
	int retval;
	int attr_count;
	int index=0;
	uint reg;
	uint def_val;

	codec_en_pinctrl = devm_pinctrl_get(&i2c->dev);
        if (IS_ERR_OR_NULL(codec_en_pinctrl)) {       
        retval = PTR_ERR(codec_en_pinctrl);
                pr_err("Target does not use pinctrl %d\n", retval);
        }
	pinctrl_state_active= pinctrl_lookup_state(codec_en_pinctrl,PINCTRL_STATE_ACTIVE);
	if (IS_ERR_OR_NULL(pinctrl_state_active)) {
                retval = PTR_ERR(pinctrl_state_active);
                pr_err("Can not lookup %s pinstate %d\n", PINCTRL_STATE_ACTIVE, retval);
        }

    pinctrl_state_suspend = pinctrl_lookup_state(codec_en_pinctrl,PINCTRL_STATE_SUSPEND);
   	if (IS_ERR_OR_NULL(pinctrl_state_suspend)) {
                retval = PTR_ERR(pinctrl_state_suspend);
                pr_err("Can not lookup %s pinstate %d\n", PINCTRL_STATE_SUSPEND, retval);
        }
    ret=pinctrl_select_state(codec_en_pinctrl,pinctrl_state_active);
    if (ret < 0)
      pr_err( "Cannot get codec_en_pinctrl state\n");

	if (!nau8540) {
		nau8540 = devm_kzalloc(dev, sizeof(*nau8540), GFP_KERNEL);
		if (!nau8540)
			{
				 pr_err( "Cannot alloc nau8540\n");
			   return -ENOMEM;
		  }
	}
	i2c_set_clientdata(i2c, nau8540);
	nau8540->regmap = devm_regmap_init_i2c(i2c, &nau8540_regmap_config);
	if (IS_ERR(nau8540->regmap))
		return PTR_ERR(nau8540->regmap);
	nau8540->dev = dev;
	nau8540_reset_chip(nau8540->regmap);
	ret = regmap_read(nau8540->regmap, NAU8540_REG_I2C_DEVICE_ID, &value);
	if (ret < 0) {
		dev_err(dev, "Failed to read device id from the NAU85L40: %d\n",ret);
		goto fail;
	}
	else{
	dev_err(dev, "NAU85L40 device ID: 0x%x\n", value);
	
	nau8540_init_regs(nau8540);
  for (index=0; index<ARRAY_SIZE(nau8540_reg_defaults); index++)
	{
		reg = nau8540_reg_defaults[index].reg;
		def_val = nau8540_reg_defaults[index].def;
		pr_debug("write index =%d reg=0x0%2x def_val=0x%04x",index,reg,def_val);
		regmap_write(nau8540->regmap, reg, def_val);
	}
	nau8540_back=nau8540;
  	/* Creation of secure nau8450 sysfs files */
	for (attr_count = 0; attr_count < ARRAY_SIZE(attrs); attr_count++) {
		retval = sysfs_create_file(&i2c->dev.kobj,
						&attrs[attr_count].attr);
		if (retval < 0) {
			dev_err(&i2c->dev,"%s: Failed to create sysfs attributes\n",__func__);
		}
	}
  	nau8540_client=i2c;
  	have_init_nau8540=0xff;
  	pr_debug("enter NAU85L40 probe end \n");
	}

	fail:	return snd_soc_register_codec(dev,
		&nau8540_codec_driver, &nau8540_dai, 1);
}

static int nau8540_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	return 0;
}


static const struct i2c_device_id nau8540_i2c_ids[] = {
	{ "nau8540", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, nau8540_i2c_ids);

#ifdef CONFIG_OF
static const struct of_device_id nau8540_of_ids[] = {
	{ .compatible = "nuvoton,nau8540" },
	{}
};
MODULE_DEVICE_TABLE(of, nau8540_of_ids);
#endif

static struct i2c_driver nau8540_i2c_driver = {
	.driver = {
		.name = "nau8540",
		.of_match_table = of_match_ptr(nau8540_of_ids),
	},
	.probe = nau8540_i2c_probe,
	.remove = nau8540_i2c_remove,
	.id_table = nau8540_i2c_ids,
};
module_i2c_driver(nau8540_i2c_driver);

////////////////////
/*
static int __init nau8540_codec_init(void)
{
    int ret;
	pr_debug("nau8540 init entry\n");
    ret = i2c_add_driver(&nau8540_i2c_driver);
	pr_debug("nau8540 i2c add ret =%d\n",ret);
    return ret;
}

static void __exit nau8540_codec_exit(void)
{
    pr_debug("nau8540 exit entry\n");
	i2c_del_driver(&nau8540_i2c_driver);
}
module_init(nau8540_codec_init);
module_exit(nau8540_codec_exit);
*/
////////////////////

MODULE_DESCRIPTION("ASoC NAU85L40 driver");
MODULE_AUTHOR("John Hsu <KCHSU0@nuvoton.com>");
MODULE_LICENSE("GPL v2");
