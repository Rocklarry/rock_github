/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <debug.h>
#include <string.h>
#include <smem.h>
#include <err.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <qpnp_wled.h>
#include <board.h>
#include <mdp5.h>
#include <scm.h>
#include <regulator.h>
#include <platform/clock.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <target/display.h>
#include <mipi_dsi_autopll_thulium.h>
#include <qtimer.h>
#include <platform.h>

#include <i2c_qup.h>
#include <blsp_qup.h>

#include "include/panel.h"
#include "include/display_resource.h"
#include "gcdb_display.h"

#define MAX_POLL_READS 15
#define POLL_TIMEOUT_US 1000
#define STRENGTH_SIZE_IN_BYTES	10
#define REGULATOR_SIZE_IN_BYTES	5
#define LANE_SIZE_IN_BYTES		20
/*---------------------------------------------------------------------------*/
/* GPIO configuration        
        uint32_t pin_strength;
        uint32_t pin_direction;
        uint32_t pin_pull;
        uint32_t pin_state;
                                                 */
/*---------------------------------------------------------------------------*/
extern      int board_id_fibocom;
static struct gpio_pin reset_gpio = {
  "msmgpio", 61, 3, 1, 0, 1
};

static struct gpio_pin reset2_gpio = { 
  "msmgpio", 62, 3, 1, 0, 1
};

static struct gpio_pin bkl_gpio = {
	 "msmgpio", 136, 3, 1, 0, 1
};

static struct gpio_pin bkl1_gpio = {
	 "msmgpio", 136, 3, 1, 0, 1
};

static struct gpio_pin te_gpio = {
	 "msmgpio", 24, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio = {
  //"msmgpio", 12, 3, 1, 0, 1
  "msmgpio", 137, 3, 1, 0, 1
/*	 "msmgpio", 138, 3, 1, 0, 1*/
};


#define VCO_DELAY_USEC 1000
#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3
#define PMIC_WLED_SLAVE_ID 3

static uint32_t dsi_pll_lock_status(uint32_t pll_base, uint32_t off, uint32_t bit)
{
	uint32_t cnt, status;

	/* check pll lock first */
	for (cnt = 0; cnt < MAX_POLL_READS; cnt++) {
		status = readl(pll_base + off);
		dprintf(SPEW, "%s: pll_base=%x cnt=%d status=%x\n", __func__, pll_base, cnt, status);
		status &= BIT(bit); /* bit 5 */
		if (status)
			break;
		udelay(POLL_TIMEOUT_US);
	}

	return status;
}

static uint32_t dsi_pll_enable_seq(uint32_t phy_base, uint32_t pll_base)
{
	uint32_t pll_locked;

	writel(0x10, phy_base + 0x45c);
	writel(0x01, phy_base + 0x48);

	pll_locked = dsi_pll_lock_status(pll_base, 0xcc, 5);
	if (pll_locked)
		pll_locked = dsi_pll_lock_status(pll_base, 0xcc, 0);

	if (!pll_locked)
		dprintf(ERROR, "%s: DSI PLL lock failed\n", __func__);
	else
		dprintf(SPEW, "%s: DSI PLL lock Success\n", __func__);

	return  pll_locked;
}

static int wled_backlight_ctrl(uint8_t enable)
{
#ifdef PMI8950_SUPPORT
	uint8_t slave_id = PMIC_WLED_SLAVE_ID;	/* pmi */

	pm8x41_wled_config_slave_id(slave_id);
	qpnp_wled_enable_backlight(enable);
	qpnp_ibb_enable(enable);
#endif	
	return NO_ERROR;
}
//liyongfeng add for pwm backlight set brightness start  2018-0605
#define PWM_DUTY_US 15
#define PWM_PERIOD_US 27
#include "../../dev/pmic/pm8x41/include/pm8x41.h"
#include "../../dev/pmic/pm8x41/include/pm8x41_hw.h"

static int msm_pwm_backlight_ctrl(uint8_t enable)
{
 struct pm8x41_mpp mpp;
 int rc;

 mpp.base = PM8x41_MMP4_BASE;
 mpp.vin = MPP_VIN3;

 if(enable) {
 pm_pwm_enable(false);

 rc = pm_pwm_config(PWM_DUTY_US, PWM_PERIOD_US); //config pwm frequence
 if(rc < 0)
 mpp.mode = MPP_HIGH;
 else {
 mpp.mode = MPP_DTEST2; //notic: need config mpp4 choose dtest2
 pm_pwm_enable(true);
 }

 pm8x41_config_output_mpp(&mpp); //config mpp4 as digital output
 pm8x41_enable_mpp(&mpp, MPP_ENABLE);
 pm8x41_reg_write(0x1BCD0, 0xA5);
 pm8x41_reg_write(0x1BCE3, 0x02);//notic: config pwm choose dtest2

 } else {
 pm_pwm_enable(false);
 pm8x41_enable_mpp(&mpp, MPP_DISABLE);
 }

 mdelay(20);
 return 0;
}
//liyongfeng add for pwm backlight set brightness end  2018-0605
extern void INC6211_Initial();
int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	uint32_t ret = NO_ERROR;

	INC6211_Initial();
	printf("LHT INC6211_Initial----end----\n");
	mdelay(200);

	if (0x00 == board_id_fibocom)
	{
	    #ifdef PMI8950_SUPPORT
	    //lukai added: use board id detect for backlight control method
		if (bl->bl_interface_type == BL_DCS)
			return ret;

		ret = wled_backlight_ctrl(enable);
        #else
        dprintf(SPEW, "target_backlight_ctrl == msm_pwm_backlight_ctrl\n");
		ret = msm_pwm_backlight_ctrl(enable);
        #endif
	}
    //liyongfeng add for pwm backlight set brightness start 2018-0605
	else
	{
	    dprintf(SPEW, "target_backlight_ctrl msm_pwm_backlight_ctrl\n");
	    ret = msm_pwm_backlight_ctrl(enable);
	}
    //liyongfeng add for pwm backlight set brightness end   2018-0605
	return ret;
}


int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int32_t ret = 0, flags, dsi_phy_pll_out;
	struct dfps_pll_codes *pll_codes = &pinfo->mipi.pll_codes;
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;

	if (pinfo->dest == DISPLAY_2) {
		flags = MMSS_DSI_CLKS_FLAG_DSI1;
		if (pinfo->mipi.dual_dsi)
			flags |= MMSS_DSI_CLKS_FLAG_DSI0;
	} else {
		flags = MMSS_DSI_CLKS_FLAG_DSI0;
		if (pinfo->mipi.dual_dsi)
			flags |= MMSS_DSI_CLKS_FLAG_DSI1;
	}

	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdss_bus_clocks_enable();
		mdp_clock_enable();
		ret = restore_secure_cfg(SECURE_DEVICE_MDSS);
		if (ret) {
			dprintf(CRITICAL,
				"%s: Failed to restore MDP security configs",
				__func__);
			mdp_clock_disable();
			mdss_bus_clocks_disable();
			mdp_gdsc_ctrl(0);
			return ret;
		}

		mdss_dsi_auto_pll_thulium_config(pinfo);

		if (!dsi_pll_enable_seq(pinfo->mipi.phy_base,
			pinfo->mipi.pll_base)) {
			ret = ERROR;
			dprintf(CRITICAL, "PLL failed to lock!\n");
			mmss_dsi_clock_disable(flags);
			mdp_clock_disable();
			mdss_bus_clocks_disable();
			return ret;
		}

		pll_codes->codes[0] = readl_relaxed(pinfo->mipi.pll_base +
						MMSS_DSI_PHY_PLL_CORE_KVCO_CODE);
		pll_codes->codes[1] = readl_relaxed(pinfo->mipi.pll_base +
						MMSS_DSI_PHY_PLL_CORE_VCO_TUNE);
		dprintf(SPEW, "codes %d %d\n", pll_codes->codes[0],
						pll_codes->codes[1]);

		if (pinfo->mipi.use_dsi1_pll)
			dsi_phy_pll_out = DSI1_PHY_PLL_OUT;
		else
			dsi_phy_pll_out = DSI0_PHY_PLL_OUT;
		mmss_dsi_clock_enable(dsi_phy_pll_out, flags,
			pll_data->pclk_m, pll_data->pclk_n, pll_data->pclk_d);

	} else if(!target_cont_splash_screen()) {
		/* stop pll */
		writel(0x0, pinfo->mipi.phy_base + 0x48);

		mmss_dsi_clock_disable(flags);
		mdp_clock_disable();
		mdss_bus_clocks_disable();
		mdp_gdsc_ctrl(enable);
	}

	return 0;

}

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	uint32_t hw_id = board_hardware_id();

			dprintf(CRITICAL, "stephen in target_panel_reset\n");
	if (enable) {

			dprintf(CRITICAL, "stephen in target_panel_reset enable te_gpio.pin_id=%d\n",te_gpio.pin_id);


   gpio_tlmm_config(te_gpio.pin_id, 0,
                                 te_gpio.pin_direction, te_gpio.pin_pull,
                                 te_gpio.pin_strength,
                                 te_gpio.pin_state);
                         
                         gpio_set_dir(te_gpio.pin_id, 2);



		if (pinfo->mipi.use_enable_gpio) {
			dprintf(CRITICAL, "stephen in target_panel_reset enable use_enable_gpio enable_gpio.pin_id=%d\n",enable_gpio.pin_id);
			gpio_tlmm_config(enable_gpio.pin_id, 0,
				enable_gpio.pin_direction, enable_gpio.pin_pull,
				enable_gpio.pin_strength,
				enable_gpio.pin_state);

			gpio_set_dir(enable_gpio.pin_id, 2);
		}

		if (hw_id != HW_PLATFORM_QRD) {
			//if (0x03 == board_id_fibocom)
			{//lukai added: use board id detect for backlight control method
				gpio_tlmm_config(bkl_gpio.pin_id, 0,
				bkl_gpio.pin_direction, bkl_gpio.pin_pull,
				bkl_gpio.pin_strength, bkl_gpio.pin_state);	  
				gpio_set_dir(bkl_gpio.pin_id, 2);
				//liyongfeng add for lcd1 backlight	en	start 2018-02-26		
				gpio_tlmm_config(bkl1_gpio.pin_id, 0,
				bkl1_gpio.pin_direction, bkl1_gpio.pin_pull,
				bkl1_gpio.pin_strength, bkl1_gpio.pin_state);
				gpio_set_dir(bkl1_gpio.pin_id, 2);
				//liyongfeng add for lcd1 backlight	en	end 2018-02-26
			}
		}

		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);

		gpio_set_dir(reset_gpio.pin_id, 2);
		gpio_set_dir(reset2_gpio.pin_id, 2);

			dprintf(CRITICAL, "stephen in target_panel_reset reset_gpio.pin_id =%d\n",reset_gpio.pin_id);
			dprintf(CRITICAL, "stephen in target_panel_reset reset2_gpio.pin_id =%d\n",reset2_gpio.pin_id);
		/* reset */
		for (int i = 0; i < RESET_GPIO_SEQ_LEN; i++) {
			if (resetseq->pin_state[i] == GPIO_STATE_LOW){
				gpio_set_dir(reset_gpio.pin_id, GPIO_STATE_LOW);
				gpio_set_dir(reset2_gpio.pin_id, GPIO_STATE_LOW);
			}
			else{
				gpio_set_dir(reset_gpio.pin_id, GPIO_STATE_HIGH);
				gpio_set_dir(reset2_gpio.pin_id, GPIO_STATE_HIGH);
			}
			mdelay(resetseq->sleep[i]);
		}

	} else if(!target_cont_splash_screen()) {
		gpio_set_dir(reset_gpio.pin_id, 0);
		gpio_set_dir(reset2_gpio.pin_id, 0);
		gpio_set_dir(enable_gpio.pin_id, 0);
	}

	return ret;
}

#ifdef PMI8950_SUPPORT
static void wled_init(struct msm_panel_info *pinfo)
{
	struct qpnp_wled_config_data config = {0};
	struct labibb_desc *labibb;
	int display_type = 0;
	bool swire_control = 0;
	bool wled_avdd_control = 0;

	labibb = pinfo->labibb;

	if (labibb)
		display_type = labibb->amoled_panel;

	if (display_type) {
		swire_control = labibb->swire_control;
		wled_avdd_control = true;
	} else {
		swire_control = false;
		wled_avdd_control = false;
	}

	config.display_type = display_type;
	config.lab_init_volt = 4600000;	/* fixed, see pmi register */
	config.ibb_init_volt = 1400000;	/* fixed, see pmi register */
	config.lab_ibb_swire_control = swire_control;
	config.wled_avdd_control = wled_avdd_control;

	if (!swire_control) {
		if (labibb && labibb->force_config) {
			config.lab_min_volt = labibb->lab_min_volt;
			config.lab_max_volt = labibb->lab_max_volt;
			config.ibb_min_volt = labibb->ibb_min_volt;
			config.ibb_max_volt = labibb->ibb_max_volt;
			config.pwr_up_delay = labibb->pwr_up_delay;
			config.pwr_down_delay = labibb->pwr_down_delay;
			config.ibb_discharge_en = labibb->ibb_discharge_en;
		} else {
			/* default */
			config.pwr_up_delay = 3;
			config.pwr_down_delay =  3;
			config.ibb_discharge_en = 1;
			if (display_type) {	/* amoled */
				config.lab_min_volt = 4600000;
				config.lab_max_volt = 4600000;
				config.ibb_min_volt = 4000000;
				config.ibb_max_volt = 4000000;
			} else { /* lcd */
				config.lab_min_volt = 5500000;
				config.lab_max_volt = 5500000;
				config.ibb_min_volt = 5500000;
				config.ibb_max_volt = 5500000;
			}
		}
	}

	dprintf(SPEW, "%s: %d %d %d %d %d %d %d %d %d %d\n", __func__,
		config.display_type,
		config.lab_min_volt, config.lab_max_volt,
		config.ibb_min_volt, config.ibb_max_volt,
		config.lab_init_volt, config.ibb_init_volt,
		config.pwr_up_delay, config.pwr_down_delay,
		config.ibb_discharge_en);

	/* QPNP WLED init for display backlight */
	pm8x41_wled_config_slave_id(PMIC_WLED_SLAVE_ID);

	qpnp_wled_init(&config);
}
#endif

int target_dsi_phy_config(struct mdss_dsi_phy_ctrl *phy_db)
{
	memcpy(phy_db->strength, panel_strength_ctrl, STRENGTH_SIZE_IN_BYTES *
		sizeof(uint32_t));
	memcpy(phy_db->regulator, panel_regulator_settings,
		REGULATOR_SIZE_IN_BYTES * sizeof(uint32_t));
	memcpy(phy_db->laneCfg, panel_lane_config, LANE_SIZE_IN_BYTES);
	return NO_ERROR;
}


int target_display_get_base_offset(uint32_t base)
{
	return 0;
}

int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t ldo_num = REG_LDO6 | REG_LDO3 | REG_SMPS3;
	uint32_t hw_id = board_hardware_id();

	if (enable) {
		if (hw_id != HW_PLATFORM_QRD){
			ldo_num |= REG_LDO17;
        dprintf(CRITICAL, "stephen target_ldo_ctrl  PMI8950_SUPPORT | ldo17\n");
		
		}
		regulator_enable(ldo_num);
		mdelay(10);
dprintf(CRITICAL, "%s: enable",__func__);
	#ifdef PMI8950_SUPPORT
        dprintf(CRITICAL, "stephen target_ldo_ctrl  PMI8950_SUPPORT\n");
	if (0x00 == board_id_fibocom)
	{//lukai added: use board id detect for backlight control method
	//	wled_init(pinfo);
	//	qpnp_ibb_enable(true); /*5V boost*/
                dprintf(CRITICAL, "stephen ldo on\n");
	}
	#endif	
		mdelay(50);
	} else {
		/*
		 * LDO6, LDO3 and SMPS3 are shared with other subsystems.
		 * Do not disable them.
		 */
		if (hw_id != HW_PLATFORM_QRD)
			regulator_disable(REG_LDO17);
	}

	return NO_ERROR;
}

bool target_display_panel_node(char *pbuf, uint16_t buf_size)
{
	return gcdb_display_cmdline_arg(pbuf, buf_size);
}
void  lcd0_on(void)
{
      
 struct oem_panel_data oem;
        int32_t ret = 0;

        set_panel_cmd_string("prim:truly_1080p_video:sec:sec_video");//
        oem = mdss_dsi_get_oem_data();

target_force_cont_splash_disable(false);///wj add
                dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                ret = gcdb_display_init(oem.panel, MDP_REV_50, (void *)MIPI_FB_ADDR);
                if (!ret || ret == ERR_NOT_SUPPORTED) {
                                dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                } else {
                                dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                        target_force_cont_splash_disable(true); dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                        msm_display_off();      dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                }
                 dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);


}

void  lcd1_on(void)
{

 struct oem_panel_data oem;
        int32_t ret = 0;

        set_panel_cmd_string("prim:truly_1080p_video:sec:sec_video");//
        oem = mdss_dsi_get_oem_data();

target_force_cont_splash_disable(false);///wj add
                dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                ret = gcdb_display_init(oem.sec_panel, MDP_REV_50, (void *)MIPI_FB_ADDR);
                if (!ret || ret == ERR_NOT_SUPPORTED) {
                                dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                } else {
                                dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                        target_force_cont_splash_disable(true); dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                        msm_display_off();      dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
                }
                 dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);


}



void target_display_init(const char *panel_name)
{
	struct oem_panel_data oem;
	int32_t ret = 0;
	uint32_t panel_loop = 0;
		dprintf(CRITICAL, "wjsec: %s %d %s\n",__FUNCTION__,__LINE__,panel_name);

	set_panel_cmd_string(panel_name);//
	oem = mdss_dsi_get_oem_data();

		dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
	if (!strcmp(oem.panel, NO_PANEL_CONFIG)
		|| !strcmp(oem.panel, SIM_VIDEO_PANEL)
		|| !strcmp(oem.panel, SIM_DUALDSI_VIDEO_PANEL)
		|| !strcmp(oem.panel, SIM_CMD_PANEL)
		|| !strcmp(oem.panel, SIM_DUALDSI_CMD_PANEL)
		|| oem.skip) {
		dprintf(CRITICAL, "Selected panel: %s\nSkip panel configuration\n",
			oem.panel);
		oem.cont_splash = true;///false;
		dprintf(CRITICAL, "stephen target_display_init to oem.cont_splash true\n");
	}
		if(board_id_fibocom==0xa){
			//strcpy(oem.panel,"truly_1080p_video");
			strcpy(oem.panel,"ek79007_1024p_video");
			strcpy(oem.sec_panel,"sec_video");
		}else{
		 	 //strcpy(oem.panel,"ili_1080p_video");
		 	 strcpy(oem.panel,"ek79007_1024p_video");
                	 strcpy(oem.sec_panel,"ilisec_video");
		}
		dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
	do {  /*
		dprintf(CRITICAL, "rini wjsec: %s %s %s  %s  %d \n",__FUNCTION__, oem.panel, oem.sec_panel,oem.dsi_config,__LINE__);
		target_force_cont_splash_disable(false);
		
		 	ret = gcdb_display_init(oem.sec_panel, MDP_REV_50, (void *)MIPI_FB_ADDR);
		if (!ret || ret == ERR_NOT_SUPPORTED) {
				dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
			 ;///break;
		} else {
				dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
			target_force_cont_splash_disable(true);	dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
			msm_display_off();
		} */
			dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
	   target_force_cont_splash_disable(false);///wj add
	   	dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
		dprintf(CRITICAL, "stephen to gcdb_display_init\n");
		ret = gcdb_display_init(oem.panel, MDP_REV_50, (void *)MIPI_FB_ADDR);
		if (!ret || ret == ERR_NOT_SUPPORTED) {
		dprintf(CRITICAL, "stephen to gcdb_display_init not support\n");
				dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
			break;
		} else {
				dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
		dprintf(CRITICAL, "stephen to to target_force_cont_splash_disable\n");
			target_force_cont_splash_disable(true);	dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
			msm_display_off();	dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
		}
		 dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
		
	} while (++panel_loop <= oem_panel_max_auto_detect_panels());

		dprintf(CRITICAL, "wjsec: %s %d \n",__FUNCTION__,__LINE__);
	if (!oem.cont_splash) {
		dprintf(INFO, "Forcing continuous splash disable\n");
		target_force_cont_splash_disable(true);
	}

}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
