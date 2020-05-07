#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include "bq2560x.h"

#define BQ2560x_SLAVE_ADDR_WRITE   0xD6
#define BQ2560x_SLAVE_ADDR_READ    0xD7


kal_bool chargin_hw_init_done = KAL_FALSE;

kal_bool HL7019_EXIT_FALG = KAL_FALSE;
kal_bool BQ25601_EXIT_FALG = KAL_FALSE;

unsigned char bq2560x_register[bq2560x_REG_NUM] = { 0 };

struct bq2560x_device {
	struct device *dev;
	struct i2c_client *client;
	int irq;
	struct work_struct irq_work;
#if defined(CONFIG_MTK_GAUGE_VERSION) && (CONFIG_MTK_GAUGE_VERSION == 30)
	struct charger_device *chg_dev;
#endif /* #if (CONFIG_MTK_GAUGE_VERSION == 30) */

};

struct bq2560x_platform_data {
	const char *chg_name;
	u32 ichg;
	u32 aicr;
	u32 mivr;
	u32 ieoc;
	u32 voreg;
	u32 vmreg;
	u8 enable_te:1;
	u8 enable_eoc_shdn:1;
};

static struct bq2560x_platform_data  bq2560x_pdata = {
	.chg_name = "primary_chg",
	.ichg = 2000000, /* unit: uA */
	.aicr = 500000, /* unit: uA */
	.mivr = 4500000, /* unit: uV */
	.ieoc = 150000, /* unit: uA */
	.voreg = 4350000, /* unit : uV */
	.vmreg = 4350000, /* unit : uV */
};

static DEFINE_MUTEX(bq2560x_i2c_access);

int bq2560x_read_byte(struct bq2560x_device *bq,unsigned char cmd, unsigned char *returnData);
int bq2560x_write_byte(struct bq2560x_device *bq,unsigned char cmd, unsigned char writeData);

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
unsigned int bq2560x_read_interface(struct bq2560x_device *bq,unsigned char RegNum, unsigned char*val, unsigned char MASK,
				  unsigned char SHIFT);

unsigned int bq2560x_config_interface(struct bq2560x_device *bq,unsigned char RegNum, unsigned char val, unsigned char MASK,
				    unsigned char SHIFT);

/* write one register directly */
unsigned int bq2560x_reg_config_interface(struct bq2560x_device *bq,unsigned char RegNum, unsigned char val);



/**********************************************************
  *
  *   [Internal Functions]
  *
  *********************************************************/
/* CON0---------------------------------------------------- */

void bq2560x_set_en_hiz(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_iinlim(struct bq2560x_device *bq,unsigned char val);
//void bq2560x_set_stat_ctrl(unsigned char val)

/* CON1---------------------------------------------------- */
void HL701x_set_reg_rst(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_reg_rst(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_wdt_rst(struct bq2560x_device *bq,unsigned char val);
int bq2560x_set_otg_config(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_chg_config(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_sys_min(struct bq2560x_device *bq,unsigned char val);
void HL701x_set_batlowv(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_batlowv(struct bq2560x_device *bq,unsigned char val);

/* CON2---------------------------------------------------- */
void HL701x_set_boost_lim(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_boost_lim(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_ichg(struct bq2560x_device *bq,unsigned char val);
unsigned char bq2560x_get_ichg(struct bq2560x_device *bq);

/* CON3---------------------------------------------------- */

void bq2560x_set_iprechg(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_iterm(struct bq2560x_device *bq,unsigned char val);

/* CON4---------------------------------------------------- */

void bq2560x_set_vreg(struct bq2560x_device *bq,unsigned char val);
void HL701x_set_topoff_timer(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_topoff_timer(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_vrechg(struct bq2560x_device *bq,unsigned char val);
unsigned char bq2560x_get_vreg(struct bq2560x_device *bq);

/* CON5---------------------------------------------------- */

void bq2560x_set_en_term(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_watchdog(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_en_timer(struct bq2560x_device *bq,unsigned char val);
int bq2560x_is_timer_enabled(struct bq2560x_device *bq);
void bq2560x_set_chg_timer(struct bq2560x_device *bq,unsigned char val);

/* CON6---------------------------------------------------- */
int bq2560x_set_vindpm(struct bq2560x_device *bq,unsigned char val);


int bq2560x_set_ovp(struct bq2560x_device *bq,unsigned char val);

void bq2560x_set_boostv(struct bq2560x_device *bq,unsigned char val);

/* CON7---------------------------------------------------- */

void bq2560x_set_tmr2x_en(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_batfet_disable(struct bq2560x_device *bq,unsigned char val);


void bq2560x_set_batfet_delay(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_batfet_reset_enable(struct bq2560x_device *bq,unsigned char val);

/* CON8---------------------------------------------------- */

unsigned int bq2560x_get_system_status(struct bq2560x_device *bq);
unsigned int bq2560x_get_vbus_stat(struct bq2560x_device *bq);
unsigned int bq2560x_get_chrg_stat(struct bq2560x_device *bq);
unsigned int bq2560x_get_vsys_stat(struct bq2560x_device *bq);
unsigned int bq2560x_get_pg_stat(struct bq2560x_device *bq);

/*CON10----------------------------------------------------------*/
void HL701x_set_int_mask(struct bq2560x_device *bq,unsigned char val);
void bq2560x_set_int_mask(struct bq2560x_device *bq,unsigned char val);
unsigned char bq2560x_get_vindpm_state(struct bq2560x_device *bq);

/**********************************************************
  *
  *   [Internal Functions End]
  *
  *********************************************************/
void bq2560x_dump_register(struct bq2560x_device *bq);
static int bq2560x_init_device(struct bq2560x_device *bq,struct bq2560x_platform_data *pdata);

/*functions*/
int charging_value_to_parameter(const unsigned int *parameter, const unsigned int array_size,
				       const unsigned int val);
int charging_parameter_to_value(const unsigned int *parameter, const unsigned int array_size,
				       const unsigned int val);
static int bmt_find_closest_level(const unsigned int *pList, unsigned int number,
					 unsigned int level);

static int charging_hw_init(struct bq2560x_device *bq);
static int charge_enable(struct bq2560x_device *bq,bool en);
static int charging_set_cv_voltage(struct bq2560x_device *bq, unsigned int data);
static int charging_get_cv_voltage(struct bq2560x_device *bq,void *data);
static int is_charging_enabled(struct bq2560x_device *bq);
static int charging_get_current(struct bq2560x_device *bq,void *data);
static int charging_set_current(struct bq2560x_device *bq,unsigned int data);
static int charging_set_input_current(struct bq2560x_device *bq,unsigned int data);
static int charging_get_charging_status(struct bq2560x_device *bq,void *data);
static int charging_set_ta_current_pattern(struct bq2560x_device *bq,bool is_increase);

static int bq2560x_charger_enable(struct charger_device *chg_dev, bool en);
//static int charging_reset_watch_dog_timer(struct bq2560x_device *bq);
//static int charging_set_hv_threshold(struct bq2560x_device *bq,void *data);
//static int charging_get_hv_status(struct bq2560x_device *bq,void *data);
//static int charging_get_battery_status(struct bq2560x_device *bq,void *data);
//static int charging_get_charger_det_status(struct bq2560x_device *bq,void *data);
//kal_bool charging_type_detection_done(struct bq2560x_device *bq);
//static int charging_get_charger_type(struct bq2560x_device *bq,void *data);
//static int charging_get_is_pcm_timer_trigger(struct bq2560x_device *bq);
//static int charging_set_platform_reset(struct bq2560x_device *bq);
//static int charging_get_platfrom_boot_mode(struct bq2560x_device *bq,void *data);
//static int charging_set_power_off(struct bq2560x_device *bq);
//static int charging_get_power_source(struct bq2560x_device *bq,void *data);
//static int charging_get_csdac_full_flag(struct bq2560x_device *bq);

#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
void set_diso_otg(struct bq2560x_device *bq,bool enable);
void set_vusb_auxadc_irq(bool enable, bool flag);
void set_vdc_auxadc_irq(bool enable, bool flag);

#if !defined(MTK_AUXADC_IRQ_SUPPORT)
static void diso_polling_handler(struct work_struct *work);
#else
static int irqreturn_t diso_auxadc_irq_handler(int irq, void *dev_id);
#endif

#if defined(MTK_DISCRETE_SWITCH) && defined(MTK_DSC_USE_EINT)
void vdc_eint_handler(void);
#endif

static int diso_get_current_voltage(struct bq2560x_device *bq,int Channel);
static void _get_diso_interrupt_state(struct bq2560x_device *bq);

#if !defined(MTK_AUXADC_IRQ_SUPPORT)
int _get_irq_direction(int pre_vol, int cur_vol);
static void _get_polling_state(void);
enum hrtimer_restart diso_kthread_hrtimer_func(struct hrtimer *timer);
int diso_thread_kthread(void *x);
#endif

#endif	/* ONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT */

//static int charging_diso_init(struct bq2560x_device *bq,void *data);
//static int charging_get_diso_state(struct bq2560x_device *bq,void *data);
static int charging_get_error_state(struct bq2560x_device *bq);
//static int charging_set_error_state(struct bq2560x_device *bq,void *data);
//static int charging_set_vindpm(struct bq2560x_device *bq);
//static int charging_set_vbus_ovp_en(struct bq2560x_device *bq);
//static int charging_get_bif_vbat(struct bq2560x_device *bq);
//static int charging_set_chrind_ck_pdn(struct bq2560x_device *bq);
//static int charging_sw_init(struct bq2560x_device *bq);
//static int charging_enable_safetytimer(struct bq2560x_device *bq);
//static int charging_set_hiz_swchr(struct bq2560x_device *bq);
//static int charging_get_bif_tbat(struct bq2560x_device *bq);

/**********************************************************
  *
  *   [I2C Function For Read/Write bq2560x]
  *
  *********************************************************/
int bq2560x_read_byte(struct bq2560x_device *bq,unsigned char reg, unsigned char *data)
{
	char cmd_buf[1] = { 0x00 };
	int ret = 0;

	mutex_lock(&bq2560x_i2c_access);
	ret = i2c_smbus_read_i2c_block_data(bq->client, reg, 1, &cmd_buf[0]);
	if (ret < 0) {
		mutex_unlock(&bq2560x_i2c_access);
		return ret;
	}
	
	*data = cmd_buf[0];
	mutex_unlock(&bq2560x_i2c_access);
	return 0;
}

int bq2560x_write_byte(struct bq2560x_device *bq,unsigned char reg, unsigned char data)
{
	char write_data[1] = { 0 };
	int ret = 0;

	mutex_lock(&bq2560x_i2c_access);
	
	write_data[0] = data;
	ret = i2c_smbus_write_i2c_block_data(bq->client, reg, 1, &write_data[0]);
	if (ret < 0) {
		mutex_unlock(&bq2560x_i2c_access);
		return ret;
	}
	mutex_unlock(&bq2560x_i2c_access);
	return 0;
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/

unsigned int bq2560x_read_interface(struct bq2560x_device *bq,unsigned char RegNum, unsigned char*val, unsigned char MASK,
				  unsigned char SHIFT)
{
	unsigned char bq2560x_reg = 0;
	int ret = 0;

	pr_err("[CHRG]--------------------------------------------------\n");

	ret = bq2560x_read_byte(bq,RegNum, &bq2560x_reg);

	bq2560x_reg &= (MASK << SHIFT);
	*val = (bq2560x_reg >> SHIFT);

	return ret;
}

unsigned int bq2560x_config_interface(struct bq2560x_device *bq,unsigned char RegNum, unsigned char val, unsigned char MASK,
				    unsigned char SHIFT)
{
	unsigned char bq2560x_reg = 0;
	int ret = 0;
	
	pr_err("[CHRG]--------------------------------------------------\n");

	ret = bq2560x_read_byte(bq,RegNum, &bq2560x_reg);
	//pr_err("[CHRG][bq2560x_config_interface] Reg[%x]=0x%x\n", RegNum, bq2560x_reg);
	
	bq2560x_reg &= ~(MASK << SHIFT);
	bq2560x_reg |= (val << SHIFT);

	ret = bq2560x_write_byte(bq,RegNum, bq2560x_reg);
	//pr_err("[CHRG][bq2560x_config_interface] Write Reg[%x]=0x%x\n", RegNum, bq2560x_reg);
	/* Check */
	 bq2560x_read_byte(bq,RegNum, &bq2560x_reg); 
	//pr_err("[CHRG][bq2560x_config_interface] Check Reg[%x]=0x%x\n", RegNum, bq2560x_reg);
	return ret;
}

/* write one register directly */
unsigned int bq2560x_reg_config_interface(struct bq2560x_device *bq,unsigned char RegNum, unsigned char val)
{
	unsigned char ret = 0;

	ret = bq2560x_write_byte(bq,RegNum, val);

	return ret;
}

/**********************************************************
  *
  *   [Internal Functions]
  *
  *********************************************************/
/* CON0---------------------------------------------------- */

void bq2560x_set_en_hiz(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON0),
				       (unsigned char) (val),
				       (unsigned char) (CON0_EN_HIZ_MASK),
				       (unsigned char) (CON0_EN_HIZ_SHIFT)
	    );

}

void bq2560x_set_iinlim(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;


	printk("[xiaozhicai][%s][%d]:\n",__func__,val);
	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON0),
				       (unsigned char) (val),
				       (unsigned char) (CON0_IINLIM_MASK),
				       (unsigned char) (CON0_IINLIM_SHIFT)
	    );
}

/* CON1---------------------------------------------------- */


void HL701x_set_reg_rst(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_REG_RST_MASK),
				       (unsigned char) (CON1_REG_RST_SHIFT)
	    );
}


 void bq2560x_set_reg_rst(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON11),
				       (unsigned char) (val),
				       (unsigned char) (CON11_REG_RST_MASK),
				       (unsigned char) (CON11_REG_RST_SHIFT)
	    );
}

void bq2560x_set_wdt_rst(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_WDT_RST_MASK),
				       (unsigned char) (CON1_WDT_RST_SHIFT)
	    );
}

int bq2560x_set_otg_config(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_OTG_CONFIG_MASK),
				       (unsigned char) (CON1_OTG_CONFIG_SHIFT)
	    );
	return ret;
}


void bq2560x_set_chg_config(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_CHG_CONFIG_MASK),
				       (unsigned char) (CON1_CHG_CONFIG_SHIFT)
	    );
}

void bq2560x_set_sys_min(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_SYS_MIN_MASK),
				       (unsigned char) (CON1_SYS_MIN_SHIFT)
	    );
}


void HL701x_set_batlowv(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON4),
				       (unsigned char) (val),
				       (unsigned char) (CON4_MIN_VBAT_SEL_HL_MASK),
				       (unsigned char) (CON4_MIN_VBAT_SEL_HL_SHIFT)
	    );
}

void bq2560x_set_batlowv(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_MIN_VBAT_SEL_MASK),
				       (unsigned char) (CON1_MIN_VBAT_SEL_SHIFT)
	    );
}

/* CON2---------------------------------------------------- */

void HL701x_set_boost_lim(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_BOOST_LIM_HL_MASK),
				       (unsigned char) (CON1_BOOST_LIM_HL_SHIFT)
	    );
}

void bq2560x_set_boost_lim(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_BOOST_LIM_MASK),
				       (unsigned char) (CON2_BOOST_LIM_SHIFT)
	    );
}

void bq2560x_set_ichg(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	printk("[xiaozhicai][%s][%d]:\n",__func__,val);
	battery_log(BAT_LOG_FULL, "[xiaozhicai][%s] [%d]set value %d\n", __func__,__LINE__,val);


	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_ICHG_MASK), (unsigned char) (CON2_ICHG_SHIFT)
	    );
}

unsigned char bq2560x_get_ichg(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON2),
				     (&val),
				     (unsigned char) (CON2_ICHG_MASK),
				     (unsigned char) (CON2_ICHG_SHIFT)
	    );
	return val;
}


/* CON3---------------------------------------------------- */

void bq2560x_set_iprechg(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON3),
				       (unsigned char) (val),
				       (unsigned char) (CON3_IPRECHG_MASK),
				       (unsigned char) (CON3_IPRECHG_SHIFT)
	    );
}

void bq2560x_set_iterm(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON3),
				       (unsigned char) (val),
				       (unsigned char) (CON3_ITERM_MASK), (unsigned char) (CON3_ITERM_SHIFT)
	    );
}

/* CON4---------------------------------------------------- */

void bq2560x_set_vreg(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON4),
				       (unsigned char) (val),
				       (unsigned char) (CON4_VREG_MASK), (unsigned char) (CON4_VREG_SHIFT)
	    );
}

void HL701x_set_topoff_timer(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON5),
				       (unsigned char) (val),
				       (unsigned char) (CON5_TOPOFF_TIMER_HL_MASK), (unsigned char) (CON5_TOPOFF_TIMER_HL_SHIFT)
	    );

}
void bq2560x_set_topoff_timer(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON4),
				       (unsigned char) (val),
				       (unsigned char) (CON4_TOPOFF_TIMER_MASK), (unsigned char) (CON4_TOPOFF_TIMER_SHIFT)
	    );

}

void bq2560x_set_vrechg(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON4),
				       (unsigned char) (val),
				       (unsigned char) (CON4_VRECHG_MASK),
				       (unsigned char) (CON4_VRECHG_SHIFT)
	    );
}


unsigned char bq2560x_get_vreg(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON4),
				     (&val),
				     (unsigned char) (CON4_VREG_MASK),
				     (unsigned char) (CON4_VREG_SHIFT)
	    );
	return val;
}

/* CON5---------------------------------------------------- */

void bq2560x_set_en_term(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON5),
				       (unsigned char) (val),
				       (unsigned char) (CON5_EN_TERM_MASK),
				       (unsigned char) (CON5_EN_TERM_SHIFT)
	    );
}

void bq2560x_set_watchdog(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON5),
				       (unsigned char) (val),
				       (unsigned char) (CON5_WATCHDOG_MASK),
				       (unsigned char) (CON5_WATCHDOG_SHIFT)
	    );
}

void bq2560x_set_en_timer(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON5),
				       (unsigned char) (val),
				       (unsigned char) (CON5_EN_TIMER_MASK),
				       (unsigned char) (CON5_EN_TIMER_SHIFT)
	    );
}

int bq2560x_is_timer_enabled(struct bq2560x_device *bq)
{
	unsigned char bq2560x_reg = 0;
	int ret = 0;


	ret = bq2560x_read_byte(bq,bq2560x_CON5, &bq2560x_reg);
	pr_err("[CHRG][bq2560x_config_interface] Reg[%x]=0x%x\n", bq2560x_CON5, bq2560x_reg);

	bq2560x_reg &= (CON5_EN_TIMER_MASK << CON5_EN_TIMER_SHIFT);
	
	
	return (bq2560x_reg>>CON5_EN_TIMER_SHIFT);
}

void bq2560x_set_chg_timer(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON5),
				       (unsigned char) (val),
				       (unsigned char) (CON5_CHG_TIMER_MASK),
				       (unsigned char) (CON5_CHG_TIMER_SHIFT)
	    );
}

/* CON6---------------------------------------------------- */

int HL701x_set_vindpm(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON0),
				       (unsigned char) (val),
				       (unsigned char) (CON0_VINDPM_MASK),
				       (unsigned char) (CON0_VINDPM_SHIFT)
	    );
	return ret;
}

int bq2560x_set_vindpm(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON6),
				       (unsigned char) (val),
				       (unsigned char) (CON6_VINDPM_MASK),
				       (unsigned char) (CON6_VINDPM_SHIFT)
	    );
	return ret;
}

int bq2560x_set_ovp(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON6),
				       (unsigned char) (val),
				       (unsigned char) (CON6_OVP_MASK),
				       (unsigned char) (CON6_OVP_SHIFT)
	    );

	return ret;

}


void bq2560x_set_boostv(struct bq2560x_device *bq,unsigned char val)
{

	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON6),
				       (unsigned char) (val),
				       (unsigned char) (CON6_BOOSTV_MASK),
				       (unsigned char) (CON6_BOOSTV_SHIFT)
	    );
}

/* CON7---------------------------------------------------- */

void bq2560x_set_tmr2x_en(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON7),
				       (unsigned char) (val),
				       (unsigned char) (CON7_TMR2X_EN_MASK),
				       (unsigned char) (CON7_TMR2X_EN_SHIFT)
	    );
}

void bq2560x_set_batfet_disable(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON7),
				       (unsigned char) (val),
				       (unsigned char) (CON7_BATFET_Disable_MASK),
				       (unsigned char) (CON7_BATFET_Disable_SHIFT)
	    );
}



void bq2560x_set_batfet_delay(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON7),
				       (unsigned char) (val),
				       (unsigned char) (CON7_BATFET_DLY_MASK),
				       (unsigned char) (CON7_BATFET_DLY_SHIFT)
	    );
}
void bq2560x_set_batfet_reset_enable(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON7),
				       (unsigned char) (val),
				       (unsigned char) (CON7_BATFET_RST_EN_MASK),
				       (unsigned char) (CON7_BATFET_RST_EN_SHIFT)
	    );
}


/* CON8---------------------------------------------------- */

unsigned int bq2560x_get_system_status(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON8),
				     (&val), (unsigned char) (0xFF), (unsigned char) (0x0)
	    );
	return val;
}

unsigned int bq2560x_get_vbus_stat(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON8),
				     (&val),
				     (unsigned char) (CON8_VBUS_STAT_MASK),
				     (unsigned char) (CON8_VBUS_STAT_SHIFT)
	    );
	return val;
}

unsigned int bq2560x_get_chrg_stat(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON8),
				     (&val),
				     (unsigned char) (CON8_CHRG_STAT_MASK),
				     (unsigned char) (CON8_CHRG_STAT_SHIFT)
	    );
	return val;
}

unsigned int bq2560x_get_vsys_stat(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON8),
				     (&val),
				     (unsigned char) (CON8_VSYS_STAT_MASK),
				     (unsigned char) (CON8_VSYS_STAT_SHIFT)
	    );
	return val;
}

unsigned int bq2560x_get_pg_stat(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON8),
				     (&val),
				     (unsigned char) (CON8_PG_STAT_MASK),
				     (unsigned char) (CON8_PG_STAT_SHIFT)
	    );
	return val;
}

/*CON10----------------------------------------------------------*/

void HL701x_set_int_mask(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON7),
				       (unsigned char) (val),
				       (unsigned char) (CON7_INT_MASK_MASK),
				       (unsigned char) (CON7_INT_MASK_SHIFT)
	    );
}

void bq2560x_set_int_mask(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_config_interface(bq,(unsigned char) (bq2560x_CON10),
				       (unsigned char) (val),
				       (unsigned char) (CON10_INT_MASK_MASK),
				       (unsigned char) (CON10_INT_MASK_SHIFT)
	    );
}



unsigned char bq2560x_get_vindpm_state(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON10),
				     (&val),
				     (unsigned char) (CON10_VINDPM_STAT_MASK),
				     (unsigned char) (CON10_VINDPM_STAT_SHIFT)
	    );
	return val;
}

	

/*CON11----------------------------------------------------------*/
void bq2560x_set_register_reset(struct bq2560x_device *bq,unsigned char val)
{
	unsigned int ret = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON11),
				     (&val),
				     (unsigned char) (CON11_REG_RST_MASK),
				     (unsigned char) (CON11_REG_RST_SHIFT)
	    );
}

unsigned int HL701x_get_verson_nb(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON10),
				     (&val),
				     (unsigned char) (CON10_PN_MASK),
				     (unsigned char) (CON10_PN_SHIFT)
	    );
	return val;
}

unsigned int bq2560x_get_verson_nb(struct bq2560x_device *bq)
{
	unsigned int ret = 0;
	unsigned char val = 0;

	ret = bq2560x_read_interface(bq,(unsigned char) (bq2560x_CON11),
				     (&val),
				     (unsigned char) (CON11_PN_MASK),
				     (unsigned char) (CON11_PN_SHIFT)
	    );
	return val;
}

/**********************************************************
  *
  *   [Internal Functions End]
  *
  *********************************************************/
void bq2560x_dump_register(struct bq2560x_device *bq)
{
	unsigned char i = 0;
	pr_err("[CHRG][%s]  start :",__func__);
	for (i = 0; i < bq2560x_REG_NUM; i++) {
		bq2560x_read_byte(bq,i, &bq2560x_register[i]);
		pr_err("[0x%x]=0x%x ", i, bq2560x_register[i]);
	}
	pr_err("end \n");
	
}

void bq25601_get_id(struct bq2560x_device *bq){

      unsigned int ret; 
      unsigned int status = STATUS_OK;
	  
      ret=HL701x_get_verson_nb(bq);
	  if(ret==1)
	  	HL7019_EXIT_FALG=KAL_TRUE;
	 else
	 	HL7019_EXIT_FALG=KAL_FALSE;

	ret=bq2560x_get_verson_nb(bq);
	if(ret==2)
	  	BQ25601_EXIT_FALG=KAL_TRUE;
	 else
	 	BQ25601_EXIT_FALG=KAL_FALSE;
	 
	 return status;
}


static int bq2560x_init_device(struct bq2560x_device *bq,
			struct bq2560x_platform_data *pdata)
{
	
	return charging_hw_init(bq);
	
}

#if defined(CONFIG_MTK_GAUGE_VERSION) && (CONFIG_MTK_GAUGE_VERSION == 30)
static int bq2560x_charger_is_charging_done(struct charger_device *chg_dev,
					   bool *done)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	return charging_get_charging_status(bq,done);
}

				    
static int bq2560x_charger_enable_otg(struct charger_device *chg_dev, bool en)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);

	printk("%s:%d en:[%d] ",__func__,__LINE__,en);

	if (en) {

		bq2560x_set_otg_config(bq,0x1);
		bq2560x_set_boostv(bq,0x2);		/* Set boostv 5.15v */
	if(HL7019_EXIT_FALG)
	      HL701x_set_boost_lim(bq,0x1);  /* 1.2A on Vbus*/
	else
		bq2560x_set_boost_lim(bq,0x1);  /* 1.2A on Vbus*/

	} else {
		bq2560x_set_otg_config(bq,0x0);	
	}

	return 0;
}

static int bq2560x_charger_enable_te(struct charger_device *chg_dev, bool en)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);	
	bq2560x_set_en_term(bq,en);
	return 0;
}

static int bq2560x_charger_enable_timer(struct charger_device *chg_dev, bool en)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	bq2560x_set_en_timer(bq,0x1);
	return 0;
}

static int bq2560x_charger_is_timer_enabled(struct charger_device *chg_dev,
					   bool *en)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);	
	
	(*en) =  bq2560x_is_timer_enabled(bq);
	return 0;
}

static int bq2560x_charger_get_min_aicr(struct charger_device *chg_dev, u32 *uA)
{
	*uA = 100000;
	return 0;
}

static int bq2560x_charger_set_aicr(struct charger_device *chg_dev, u32 uA)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	return charging_set_input_current(bq, uA);
}

static int bq2560x_charger_get_aicr(struct charger_device *chg_dev, u32 *uA)
{
	/* no limit */
	*uA = U32_MAX;
	return 0;
}

static int bq2560x_charger_get_cv(struct charger_device *chg_dev, u32 *uV)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	return charging_get_cv_voltage(bq,uV);
}

static int bq2560x_charger_set_cv(struct charger_device *chg_dev, u32 uV)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);

	return charging_set_cv_voltage(bq, uV);
}

static int bq2560x_charger_get_min_ichg(struct charger_device *chg_dev, u32 *uA)
{
	*uA = 51200;
	return 0;
}

static int bq2560x_charger_set_ichg(struct charger_device *chg_dev, u32 uA)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	return charging_set_current(bq, uA);
}

static int bq2560x_charger_get_ichg(struct charger_device *chg_dev, u32 *uA)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	return charging_get_current(bq,uA);
}

static int bq2560x_charger_is_enabled(struct charger_device *chg_dev, bool *en)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	int ret = 0;

	ret = is_charging_enabled(bq);
	if(ret < 0)
		*en = false;
	else 
		*en = true;
	return 0;
}

static int bq2560x_charger_enable(struct charger_device *chg_dev, bool en)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);

	return charge_enable(bq, en);
}

static int bq2560x_set_mivr(struct charger_device *chg_dev, u32 uV)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
if(HL7019_EXIT_FALG)
	return HL701x_set_vindpm(bq,uV);
else
	return bq2560x_set_vindpm(bq,uV);
}

static int bq2560x_get_mivr(struct charger_device *chg_dev, bool *in_loop)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);	
	(*in_loop) = bq2560x_get_vindpm_state(bq);	
	return 0;
}


static inline int ncp1854_get_ieoc(struct bq2560x_device *bq, u32 *uA)
{
	/* TBD */
	*uA = 200000;
	return 0;
}

static int bq2560x_charger_dump_registers(struct charger_device *chg_dev)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);

	int status = STATUS_OK;

	bq2560x_dump_register(bq);

	return status;
}

static int bq2560x_charger_do_event(struct charger_device *chg_dev, u32 event,
				   u32 args)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);

	switch (event) {
	case EVENT_EOC:
		dev_info(bq->dev, "do eoc event\n");
		charger_dev_notify(bq->chg_dev, CHARGER_DEV_NOTIFY_EOC);
		break;
	case EVENT_RECHARGE:
		dev_info(bq->dev, "do recharge event\n");
		charger_dev_notify(bq->chg_dev, CHARGER_DEV_NOTIFY_RECHG);
		break;
	default:
		break;
	}
	return 0;
}
static int bq2560x_send_ta_current_pattern(struct charger_device *chg_dev, bool is_increase)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);
	
	return charging_set_ta_current_pattern(bq,is_increase);

}

static int bq2560x_charger_plug_in(struct charger_device *chg_dev)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);	

	return charging_hw_init(bq);		
}

static int bq2560x_charger_plug_out(struct charger_device *chg_dev)
{
	struct bq2560x_device *bq = charger_get_data(chg_dev);

	/*disable charging*/
	 bq2560x_set_ichg(bq,0x00);
	/*disable wdt*/
	bq2560x_set_watchdog(bq,0x00);	/* WDT 40s */
	bq2560x_set_en_timer(bq,0x00);	/* Enable charge timer */

	/*enable hz mode of secondary charger*/
	return 0;
}

static const struct charger_ops bq2560x_chg_ops = {
	/* cable plug in/out */
	.plug_in = bq2560x_charger_plug_in,
	.plug_out = bq2560x_charger_plug_out,
	/* enable */
	.enable = bq2560x_charger_enable,
	.is_enabled = bq2560x_charger_is_enabled,
	/* charging current */
	.get_charging_current = bq2560x_charger_get_ichg,
	.set_charging_current = bq2560x_charger_set_ichg,
	.get_min_charging_current = bq2560x_charger_get_min_ichg,
	/* charging voltage */
	.set_constant_voltage = bq2560x_charger_set_cv,
	.get_constant_voltage = bq2560x_charger_get_cv,
	/* charging input current */
	.get_input_current = bq2560x_charger_get_aicr,//
	.set_input_current = bq2560x_charger_set_aicr,
	.get_min_input_current = bq2560x_charger_get_min_aicr, 
	/* safety timer */
	.is_safety_timer_enabled = bq2560x_charger_is_timer_enabled,
	.enable_safety_timer = bq2560x_charger_enable_timer,
	.set_mivr = bq2560x_set_mivr,
	.get_mivr_state = bq2560x_get_mivr,
	//.enable_vbus_ovp = pmic_enable_vbus_ovp,
	/* charing termination */
	.enable_termination = bq2560x_charger_enable_te,
	.send_ta_current_pattern = bq2560x_send_ta_current_pattern,
	/* OTG */
	.enable_otg = bq2560x_charger_enable_otg,	
		
	/* misc */
	.is_charging_done = bq2560x_charger_is_charging_done,
	.dump_registers = bq2560x_charger_dump_registers, 
	/* event */
	.event = bq2560x_charger_do_event,
};

static const struct charger_properties bq2560x_chg_props = {
	.alias_name = "bq2560x",
};
#endif /* #if (CONFIG_MTK_GAUGE_VERSION == 30) */


/******************************************add functions***********************************************/

int charging_value_to_parameter(const unsigned int *parameter, const unsigned int array_size,
				       const unsigned int val)
{
	if (val < array_size) {
		return parameter[val];
	} else {
		pr_err("[CHRG]Can't find the parameter \r\n");
		return parameter[0];
	}
}

int charging_parameter_to_value(const unsigned int *parameter, const unsigned int array_size,
				       const unsigned int val)
{
	unsigned int i;

	pr_err("[CHRG]array_size = %d \r\n", array_size);

	for (i = 0; i < array_size; i++) {
		//pr_err("[CHRG][%s] parameter[%d]:%d ,val:%d \n",__func__,i,*(parameter + i),val);
		if (val == *(parameter + i)){
		pr_err("[CHRG] i = %d \n",i);			
		return i;
	}

	}

	pr_err("[CHRG]NO register value match. val=%d\r\n", val);
	/* TODO: ASSERT(0);      // not find the value */
	return 0;
}

static int bmt_find_closest_level(const unsigned int *pList, unsigned int number,
					 unsigned int level)
{
	unsigned int i;
	unsigned int max_value_in_last_element;

	if (pList[0] < pList[1])
		max_value_in_last_element = KAL_TRUE;
	else
		max_value_in_last_element = KAL_FALSE;

	if (max_value_in_last_element == KAL_TRUE) {
		for (i = (number - 1); i != 0; i--) {/* max value in the last element */

			if (pList[i] <= level){
				return pList[i];
				printk("[xiaozhicai][%s][%d][pList:%d]\n",__func__,__LINE__,pList[i]);
			}
		}

		pr_err("[CHRG]Can't find closest level, small value first \r\n");
		return pList[0];
		/* return CHARGE_CURRENT_0_00_MA; */
	} else {
		for (i = 0; i < number; i++) {/* max value in the first element */

			if (pList[i] <= level)
				return pList[i];

		}

		pr_err("[CHRG]Can't find closest level, large value first \r\n");
		return pList[number - 1];
		/* return CHARGE_CURRENT_0_00_MA; */
	}
}
#if 0
static int is_chr_det(struct bq2560x_device *bq)
{
	unsigned int val = 0;
	val = pmic_get_register_value(PMIC_RGS_CHRDET);
//	battery_log(BAT_LOG_CRTI, "[is_chr_det] %d\n", val);

	return val;
}
#endif

static int charging_hw_init(struct bq2560x_device *bq)
{
	unsigned int status = STATUS_OK;

	
if (HL7019_EXIT_FALG)
{
	bq2560x_set_en_hiz(bq,0x0);	
	bq2560x_set_vindpm(bq,0x7);		/* VIN DPM check 4.6V */
	HL701x_set_reg_rst(bq,0x0);
	bq2560x_set_wdt_rst(bq,0x0);	/* Kick watchdog */
	bq2560x_set_sys_min(bq,0x5);	/* Minimum system voltage 3.5V */
	bq2560x_set_iprechg(bq,0x3);	/* Precharge current 540mA */
	bq2560x_set_iterm(bq,0x1);		/* Termination current 180mA */

	#if defined(HIGH_BATTERY_VOLTAGE_SUPPORT)
		bq2560x_set_vreg(bq,0x36);		/* VREG 4.352V */
	#else
		bq2560x_set_vreg(bq,0x2c);		/* VREG 4.208V */
	#endif

	HL701x_set_batlowv(bq,0x1);	/* BATLOWV 3.0V */
	bq2560x_set_vrechg(bq,0x0);		/* VRECHG 0.1V (4.108V) */
	bq2560x_set_en_term(bq,0x1);	/* Enable termination */
	//bq2560x_set_stat_ctrl(0x0);	/* Enable STAT pin function */
	bq2560x_set_watchdog(bq,0x0);	/* WDT 40s */
	bq2560x_set_en_timer(bq,0x1);	/* Enable charge timer */
	//bq2560x_set_chg_timer(0x02);	/*set charge time 12h*/
	HL701x_set_int_mask(bq,0x0);}	/* Disable fault interrupt */
else{
	bq2560x_set_en_hiz(bq,0x0);	
	bq2560x_set_vindpm(bq,0x7);		/* VIN DPM check 4.6V */
	bq2560x_set_reg_rst(bq,0x0);
	bq2560x_set_wdt_rst(bq,0x1);	/* Kick watchdog */
	bq2560x_set_sys_min(bq,0x5);	/* Minimum system voltage 3.5V */
	bq2560x_set_iprechg(bq,0x3);	/* Precharge current 540mA */
	bq2560x_set_iterm(bq,0x2);		/* Termination current 180mA */

	#if defined(HIGH_BATTERY_VOLTAGE_SUPPORT)
		bq2560x_set_vreg(bq,0x0f);		/* VREG 4.352V */
	#else
		bq2560x_set_vreg(bq,0x07);		/* VREG 4.208V */
	#endif

	bq2560x_set_batlowv(bq,0x1);	/* BATLOWV 3.0V */
	bq2560x_set_vrechg(bq,0x0);		/* VRECHG 0.1V (4.108V) */
	bq2560x_set_en_term(bq,0x1);	/* Enable termination */
	//bq2560x_set_stat_ctrl(0x0);	/* Enable STAT pin function */
	bq2560x_set_watchdog(bq,0x1);	/* WDT 40s */
	bq2560x_set_en_timer(bq,0x1);	/* Enable charge timer */
	//bq2560x_set_chg_timer(0x02);	/*set charge time 12h*/
	bq2560x_set_int_mask(bq,0x0);	/* Disable fault interrupt */
}

#if defined(MTK_WIRELESS_CHARGER_SUPPORT)
	if (wireless_charger_gpio_number != 0) {
		mt_set_gpio_mode(wireless_charger_gpio_number, 0);	/* 0:GPIO mode */
		mt_set_gpio_dir(wireless_charger_gpio_number, 0);	/* 0: input, 1: output */
	}
#endif
	
#ifdef CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT
	mt_set_gpio_mode(vin_sel_gpio_number, 0);	/* 0:GPIO mode */
	mt_set_gpio_dir(vin_sel_gpio_number, 0);	/* 0: input, 1: output */
#endif

	return status;
}

static int charge_enable(struct bq2560x_device *bq,bool en)
{
	unsigned int status = STATUS_OK;
	kal_bool static hiz_flag = 0;

	if (en) {
		bq2560x_set_en_hiz(bq,0x0);
		bq2560x_set_chg_config(bq,0x1);	/* charger enable */
		hiz_flag = 0;
	} else {
#if defined(CONFIG_USB_MTK_HDRC_HCD)
	if (mt_usb_is_device())
#endif
		{
			bq2560x_set_chg_config(bq,0x0);
			if(hiz_flag == 1){
				bq2560x_set_en_hiz(bq,0x1);
			}
			if (charging_get_error_state(bq)) {
				pr_err("[CHRG][charging_enable] bq2560x_set_hz_mode(0x1)\n");
				bq2560x_set_en_hiz(bq,0x1);	/* disable power path */
			}
		}
#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
		bq2560x_set_chg_config(bq,0x0);
		bq2560x_set_en_hiz(bq,0x1);	/* disable power path */
#endif
	}

	return status;
}

static int charging_set_cv_voltage(struct bq2560x_device *bq,unsigned int data)
{
	unsigned int status = STATUS_OK;
	unsigned int array_size;
	unsigned int set_cv_voltage;
	unsigned short register_value;
	unsigned int cv_value = data;
	static short pre_register_value = -1;

#if defined(HIGH_BATTERY_VOLTAGE_SUPPORT)
	/* highest of voltage will be 4.3V, because powerpath limitation */
	if (cv_value >= 4300000)
		cv_value = 4400000;
#endif

	/* use nearest value */
	if (4200000 == cv_value)
		cv_value = 4208000;
if(HL7019_EXIT_FALG){
	array_size = GETARRAYNUM(VBAT_CV_VTH_HL);
	pr_err("[CHRG]%s:cv_voltage = %d\n",__func__, data);
	set_cv_voltage = bmt_find_closest_level(VBAT_CV_VTH_HL, array_size, cv_value);
	register_value = charging_parameter_to_value(VBAT_CV_VTH_HL, array_size, set_cv_voltage) ;
	pr_err("[CHRG]charging_set_cv_voltage  set_cv_voltage   :%d  register_value=0x%x\n", set_cv_voltage ,register_value);
	bq2560x_set_vreg(bq,register_value);

	bq2560x_dump_register(bq);
	
	if (pre_register_value != register_value)
		bq2560x_set_chg_config(bq,0x1);
	pre_register_value = register_value;
}else if (BQ25601_EXIT_FALG){
	array_size = GETARRAYNUM(VBAT_CV_VTH);
	pr_err("[CHRG]%s:cv_voltage = %d\n",__func__, data);
	set_cv_voltage = bmt_find_closest_level(VBAT_CV_VTH, array_size, cv_value);
	register_value = charging_parameter_to_value(VBAT_CV_VTH, array_size, set_cv_voltage) ;
	pr_err("[CHRG]charging_set_cv_voltage  set_cv_voltage   :%d  register_value=0x%x\n", set_cv_voltage ,register_value);
	bq2560x_set_vreg(bq,register_value);

	bq2560x_dump_register(bq);
	
	if (pre_register_value != register_value)
		bq2560x_set_chg_config(bq,0x1);
	pre_register_value = register_value;

}else{
	array_size = GETARRAYNUM(VBAT_CV_VTH);
	pr_err("[CHRG]%s:cv_voltage = %d\n",__func__, data);
	set_cv_voltage = bmt_find_closest_level(VBAT_CV_VTH, array_size, cv_value);
	register_value = charging_parameter_to_value(VBAT_CV_VTH, array_size, set_cv_voltage) ;
	pr_err("[CHRG]charging_set_cv_voltage  set_cv_voltage   :%d  register_value=0x%x\n", set_cv_voltage ,register_value);
	bq2560x_set_vreg(bq,register_value);

	bq2560x_dump_register(bq);
	
	if (pre_register_value != register_value)
		bq2560x_set_chg_config(bq,0x1);
	pre_register_value = register_value;
	}
	return status;
}

static int charging_get_cv_voltage(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int array_size;
	unsigned char register_value;

	register_value = bq2560x_get_vreg(bq);
if(HL7019_EXIT_FALG){
	array_size = GETARRAYNUM(VBAT_CV_VTH_HL); 
	*(unsigned int *)data = charging_value_to_parameter(VBAT_CV_VTH_HL,array_size,register_value); 
}else if (BQ25601_EXIT_FALG){
	array_size = GETARRAYNUM(VBAT_CV_VTH); 
	*(unsigned int *)data = charging_value_to_parameter(VBAT_CV_VTH,array_size,register_value); 
}else{
	array_size = GETARRAYNUM(VBAT_CV_VTH); 
	*(unsigned int *)data = charging_value_to_parameter(VBAT_CV_VTH,array_size,register_value); 
         }
	return status;
}

static int is_charging_enabled(struct bq2560x_device *bq)
{
	unsigned char ret_val = 0;

	bq2560x_read_interface(bq,bq2560x_CON2, &ret_val, CON2_ICHG_MASK, CON2_ICHG_SHIFT);

	if(ret_val == 0 )
		return -1;
	else
		return 0;
}

static int charging_get_current(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int array_size; 
	unsigned char ret_val = 0;

	/* Get current level */
	bq2560x_read_interface(bq,bq2560x_CON2, &ret_val, CON2_ICHG_MASK, CON2_ICHG_SHIFT);

	/* Get current level */
if(HL7019_EXIT_FALG){	
	array_size = GETARRAYNUM(CS_VTH_HL); 
	*(unsigned int *)data = charging_value_to_parameter(CS_VTH_HL,array_size,ret_val); 
  }else if (BQ25601_EXIT_FALG){
  	array_size = GETARRAYNUM(CS_VTH); 
	*(unsigned int *)data = charging_value_to_parameter(CS_VTH,array_size,ret_val); 
}else{
  	array_size = GETARRAYNUM(CS_VTH); 
	*(unsigned int *)data = charging_value_to_parameter(CS_VTH,array_size,ret_val); 
  	}
	return status;
}

static int charging_set_current(struct bq2560x_device *bq,unsigned int data)
{
	unsigned int status = STATUS_OK;
	unsigned int set_chr_current;
	unsigned int array_size;
	unsigned int register_value;
	unsigned int current_value =  data;
	
	battery_log(BAT_LOG_FULL, "%s:%d set output current %d\n", __func__,__LINE__,data);


	pr_err("[CHRG]%s  data :%d \n",__func__,data);
	
if(HL7019_EXIT_FALG){		
	array_size = GETARRAYNUM(CS_VTH_HL);
	set_chr_current = bmt_find_closest_level(CS_VTH_HL, array_size, current_value);
	register_value = charging_parameter_to_value(CS_VTH_HL, array_size, set_chr_current);
	pr_err("[CHRG]%s  set_chr_current:%d ,register_value :0x%x \n",__func__,set_chr_current,register_value);
}else if (BQ25601_EXIT_FALG){
	array_size = GETARRAYNUM(CS_VTH);
	set_chr_current = bmt_find_closest_level(CS_VTH, array_size, current_value);
	register_value = charging_parameter_to_value(CS_VTH, array_size, set_chr_current);
	pr_err("[CHRG]%s  set_chr_current:%d ,register_value :0x%x \n",__func__,set_chr_current,register_value);

}else{
	array_size = GETARRAYNUM(CS_VTH);
	set_chr_current = bmt_find_closest_level(CS_VTH, array_size, current_value);
	register_value = charging_parameter_to_value(CS_VTH, array_size, set_chr_current);
	pr_err("[CHRG]%s  set_chr_current:%d ,register_value :0x%x \n",__func__,set_chr_current,register_value);
	}

	bq2560x_set_ichg(bq,register_value);
	bq2560x_dump_register(bq);
	return status;
}

static int charging_set_input_current(struct bq2560x_device *bq,unsigned int data)
{
	unsigned int status = STATUS_OK;
	unsigned int current_value = data;
	unsigned int set_chr_current;
	unsigned int array_size;
	unsigned int register_value;
	
	battery_log(BAT_LOG_FULL, "%s:%d set input current %d\n", __func__,__LINE__,data);
if(HL7019_EXIT_FALG){
	array_size = GETARRAYNUM(INPUT_CS_VTH_HL);
	set_chr_current = bmt_find_closest_level(INPUT_CS_VTH_HL, array_size, current_value);
	register_value = charging_parameter_to_value(INPUT_CS_VTH_HL, array_size, set_chr_current);
	bq2560x_set_iinlim(bq,register_value);
}else if (BQ25601_EXIT_FALG){
       array_size = GETARRAYNUM(INPUT_CS_VTH);
	set_chr_current = bmt_find_closest_level(INPUT_CS_VTH, array_size, current_value);
	register_value = charging_parameter_to_value(INPUT_CS_VTH, array_size, set_chr_current);
	bq2560x_set_iinlim(bq,register_value);
}else{
       array_size = GETARRAYNUM(INPUT_CS_VTH);
	set_chr_current = bmt_find_closest_level(INPUT_CS_VTH, array_size, current_value);
	register_value = charging_parameter_to_value(INPUT_CS_VTH, array_size, set_chr_current);
	bq2560x_set_iinlim(bq,register_value);
}
	return status;
}

static int charging_get_charging_status(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int ret_val;

	ret_val = bq2560x_get_chrg_stat(bq);

	if (ret_val == 0x3)
		*(unsigned int *) data = KAL_TRUE;
	else
		*(unsigned int *) data = KAL_FALSE;

	return status;
}

#if defined (ZC_DEF)
static int charging_set_hv_threshold(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int set_hv_voltage;
	unsigned int array_size;
	unsigned short register_value;
	unsigned int voltage = *(unsigned int *) (data);

	array_size = GETARRAYNUM(VCDT_HV_VTH);
	set_hv_voltage = bmt_find_closest_level(VCDT_HV_VTH, array_size, voltage);
	register_value = charging_parameter_to_value(VCDT_HV_VTH, array_size, set_hv_voltage);
	pmic_set_register_value(PMIC_RG_VCDT_HV_VTH, register_value);

	return status;
}

static int charging_reset_watch_dog_timer(struct bq2560x_device *bq)
{
	unsigned int status = STATUS_OK;

	pr_err("[CHRG]charging_reset_watch_dog_timer\r\n");

	bq2560x_set_wdt_rst(bq,0x1);	/* Kick watchdog */

	return status;
}
static int charging_get_hv_status(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;

	*(kal_bool *) (data) = pmic_get_register_value(PMIC_RGS_VCDT_HV_DET);

	return status;
}
static int charging_get_battery_status(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	*(kal_bool *) (data) = 0;	/* battery exist */
	pr_err("[CHRG][charging_get_battery_status] battery exist for bring up.\n");
#else
	unsigned int val = 0;
	val = pmic_get_register_value(PMIC_BATON_TDET_EN);
//	battery_log(BAT_LOG_FULL, "[charging_get_battery_status] BATON_TDET_EN = %d\n", val);
	if (val) {
		pmic_set_register_value(PMIC_BATON_TDET_EN, 1);
		pmic_set_register_value(PMIC_RG_BATON_EN, 1);
		*(kal_bool *) (data) = pmic_get_register_value(PMIC_RGS_BATON_UNDET);
	} else {
		*(kal_bool *) (data) = KAL_FALSE;
	}
#endif

	return status;
}


static int charging_get_charger_det_status(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_MTK_FPGA)
	*(kal_bool *) (data) = 1;
//	battery_log(BAT_LOG_CRTI, "chr exist for fpga\n");
#else
	*(kal_bool *) (data) = pmic_get_register_value(PMIC_RGS_CHRDET);
#endif
	return status;
}

static int charging_get_charger_type(struct bq2560x_device *bq,void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	*(CHARGER_TYPE *) (data) = STANDARD_HOST;
#else
#if defined(MTK_WIRELESS_CHARGER_SUPPORT)
	int wireless_state = 0;
	if (wireless_charger_gpio_number != 0) {
		wireless_state = mt_get_gpio_in(wireless_charger_gpio_number);
		if (wireless_state == WIRELESS_CHARGER_EXIST_STATE) {
			*(CHARGER_TYPE *) (data) = WIRELESS_CHARGER;
			pr_err("[CHRG]WIRELESS_CHARGER!\n");
			return status;
		}
	} else {
		pr_err("[CHRG]wireless_charger_gpio_number=%d\n", wireless_charger_gpio_number);
	}

	if (g_charger_type != CHARGER_UNKNOWN && g_charger_type != WIRELESS_CHARGER) {
		*(CHARGER_TYPE *) (data) = g_charger_type;
		pr_err("[CHRG]return %d!\n", g_charger_type);
		return status;
	}
#endif

	if (is_chr_det(bq) == 0) {
		g_charger_type = CHARGER_UNKNOWN;
		*(CHARGER_TYPE *) (data) = CHARGER_UNKNOWN;
		pr_err("[CHRG][charging_get_charger_type] return CHARGER_UNKNOWN\n");
		return status;
	}

	charging_type_det_done = KAL_FALSE;
	*(CHARGER_TYPE *) (data) = hw_charging_get_charger_type();
	charging_type_det_done = KAL_TRUE;
	g_charger_type = *(CHARGER_TYPE *) (data);

#endif

	return status;
}

#endif

kal_bool charging_type_detection_done(struct bq2560x_device *bq)
{
	return charging_type_det_done;
}
#if 0
static int charging_get_is_pcm_timer_trigger(struct bq2560x_device *bq)
{
	int status = STATUS_OK;

	return status;
}


static int charging_set_platform_reset(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
#else
	pr_err("[CHRG]charging_set_platform_reset\n");
	/* arch_reset(0,NULL); */
#endif
	return status;
}



static int charging_get_platfrom_boot_mode(struct bq2560x_device *bq,void *data)
{
	int status = STATUS_OK;
#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
#else
	*(int *) (data) = get_boot_mode();
	pr_err("[CHRG]get_boot_mode=%d\n", get_boot_mode());
#endif
	return status;
}
static int charging_get_power_source(struct bq2560x_device *bq,void *data)
{
	int status = STATUS_OK;

#if 0				/* #if defined(MTK_POWER_EXT_DETECT) */
	if (MT_BOARD_PHONE == mt_get_board_type())
		*(kal_bool *) data = KAL_FALSE;
	else
		*(kal_bool *) data = KAL_TRUE;
#else
	*(kal_bool *) data = KAL_FALSE;
#endif

	return status;
}

static int charging_set_power_off(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
#else
	pr_err("[CHRG]charging_set_power_off\n");
	mt_power_off();
#endif

	return status;
}


static int charging_get_csdac_full_flag(struct bq2560x_device *bq)
{
	return STATUS_UNSUPPORTED;
}

#endif




static int charging_set_ta_current_pattern(struct bq2560x_device *bq,bool is_increase)
{
	int increase = is_increase;
	int charging_status = KAL_FALSE;
	
	
#if defined(HIGH_BATTERY_VOLTAGE_SUPPORT)
	BATTERY_VOLTAGE_ENUM cv_voltage = 4400000;
#else
	BATTERY_VOLTAGE_ENUM cv_voltage = 4200000;	
#endif


	pr_err("[CHRG] %s  is_increase :%d  \n",__func__,is_increase);

	charging_get_charging_status(bq,&charging_status);
if (KAL_FALSE == charging_status) {	
	
if (HL7019_EXIT_FALG){
		pr_err("[CHRG] %s  HL7019_CHARGER_IC  cv_voltage  :%d  \n",__func__,cv_voltage);
        	bq2560x_set_en_hiz(bq,0x0);
		charging_set_cv_voltage(bq,cv_voltage);	/* Set CV */
		bq2560x_set_ichg(bq,0x0);				/* Set charging current 500ma */
		bq2560x_set_chg_config(bq,0x1);			/* Enable Charging */

		bq2560x_dump_register(bq);}
else{
           	bq2560x_set_en_hiz(bq,0x0);
		charging_set_cv_voltage(bq,cv_voltage);	/* Set CV */
		bq2560x_set_ichg(bq,0x8);				/* Set charging current 500ma */
		bq2560x_set_chg_config(bq,0x1);			/* Enable Charging */
		bq2560x_set_ovp(bq,0x2);

	}/* Set OVP 9V input */
}

	
if (HL7019_EXIT_FALG){
	if (increase == KAL_TRUE) {
		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 1\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 1\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 2\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 2\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 3\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 3\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 4\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 4\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 5\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 5\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 6\n");
		msleep(485);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 6\n");
		msleep(50);

		pr_err("[CHRG]mtk_ta_increase() end\n\n");

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		msleep(200);
	} else {
		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 1\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 1\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 2\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 2\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 3\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 3\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 4\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 4\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 5\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 5\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 6\n");
		msleep(485);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 6\n");
		msleep(50);

		pr_err("[CHRG]mtk_ta_decrease() end\n");

		bq2560x_set_iinlim(bq,0x2);	/* 500mA */
	
	}
}else{
   if (increase == KAL_TRUE) {
		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 1\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 1\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 2\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 2\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 3\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 3\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 4\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 4\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 5\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 5\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_increase() on 6\n");
		msleep(485);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_increase() off 6\n");
		msleep(50);

		pr_err("[CHRG]mtk_ta_increase() end\n\n");

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		msleep(200);
	} else {
		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 1\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 1\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 2\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 2\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 3\n");
		msleep(281);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 3\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 4\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 4\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 5\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 5\n");
		msleep(85);

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
		pr_err("[CHRG]mtk_ta_decrease() on 6\n");
		msleep(485);

		bq2560x_set_iinlim(bq,0x0);	/* 100mA */
		pr_err("[CHRG]mtk_ta_decrease() off 6\n");
		msleep(50);

		pr_err("[CHRG]mtk_ta_decrease() end\n");

		bq2560x_set_iinlim(bq,0x4);	/* 500mA */
	}
}
	return STATUS_OK;
}





#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
#if 0
void set_diso_otg(struct bq2560x_device *bq,bool enable)
{
	g_diso_otg = enable;
}
#endif

void set_vusb_auxadc_irq(bool enable, bool flag)
{
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
	hrtimer_cancel(&diso_kthread_timer);

	DISO_Polling.reset_polling = KAL_TRUE;
	DISO_Polling.vusb_polling_measure.notify_irq_en = enable;
	DISO_Polling.vusb_polling_measure.notify_irq = flag;

	hrtimer_start(&diso_kthread_timer, ktime_set(0, MSEC_TO_NSEC(SW_POLLING_PERIOD)),
		      HRTIMER_MODE_REL);
#else
	unsigned short threshold = 0;
	if (enable) {
		if (flag == 0)
			threshold = DISO_IRQ.vusb_measure_channel.falling_threshold;
		else
			threshold = DISO_IRQ.vusb_measure_channel.rising_threshold;

		threshold =
		    (threshold * R_DISO_VBUS_PULL_DOWN) / (R_DISO_VBUS_PULL_DOWN +
							   R_DISO_VBUS_PULL_UP);
		mt_auxadc_enableBackgroundDection(DISO_IRQ.vusb_measure_channel.number, threshold,
						  DISO_IRQ.vusb_measure_channel.period,
						  DISO_IRQ.vusb_measure_channel.debounce, flag);
	} else {
		mt_auxadc_disableBackgroundDection(DISO_IRQ.vusb_measure_channel.number);
	}
#endif
	pr_err("[CHRG] [%s] enable: %d, flag: %d!\n", __func__, enable, flag);
}

void set_vdc_auxadc_irq(bool enable, bool flag)
{
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
	hrtimer_cancel(&diso_kthread_timer);

	DISO_Polling.reset_polling = KAL_TRUE;
	DISO_Polling.vdc_polling_measure.notify_irq_en = enable;
	DISO_Polling.vdc_polling_measure.notify_irq = flag;

	hrtimer_start(&diso_kthread_timer, ktime_set(0, MSEC_TO_NSEC(SW_POLLING_PERIOD)),
		      HRTIMER_MODE_REL);
#else
	unsigned short threshold = 0;
	if (enable) {
		if (flag == 0)
			threshold = DISO_IRQ.vdc_measure_channel.falling_threshold;
		else
			threshold = DISO_IRQ.vdc_measure_channel.rising_threshold;

		threshold =
		    (threshold * R_DISO_DC_PULL_DOWN) / (R_DISO_DC_PULL_DOWN + R_DISO_DC_PULL_UP);
		mt_auxadc_enableBackgroundDection(DISO_IRQ.vdc_measure_channel.number, threshold,
						  DISO_IRQ.vdc_measure_channel.period,
						  DISO_IRQ.vdc_measure_channel.debounce, flag);
	} else {
		mt_auxadc_disableBackgroundDection(DISO_IRQ.vdc_measure_channel.number);
	}
#endif
	pr_err("[CHRG] [%s] enable: %d, flag: %d!\n", __func__, enable, flag);
}

#if !defined(MTK_AUXADC_IRQ_SUPPORT)
static void diso_polling_handler(struct work_struct *work)
{
	int trigger_channel = -1;
	int trigger_flag = -1;

	if (DISO_Polling.vdc_polling_measure.notify_irq_en)
		trigger_channel = AP_AUXADC_DISO_VDC_CHANNEL;
	else if (DISO_Polling.vusb_polling_measure.notify_irq_en)
		trigger_channel = AP_AUXADC_DISO_VUSB_CHANNEL;

	pr_err("[CHRG][DISO]auxadc handler triggered\n");
	switch (trigger_channel) {
	case AP_AUXADC_DISO_VDC_CHANNEL:
		trigger_flag = DISO_Polling.vdc_polling_measure.notify_irq;
		pr_err("[CHRG][DISO]VDC IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel,
			 trigger_flag);
#ifdef MTK_DISCRETE_SWITCH	/*for DSC DC plugin handle */
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_FALLING);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[2]);
		}
#else				/* for load switch OTG leakage handle */
		set_vdc_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_ONLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[5]);
		} else if (trigger_flag == DISO_IRQ_FALLING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_otg_state = DISO_ONLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[1]);
		} else
			pr_err("[CHRG][%s] wrong trigger flag!\n", __func__);
#endif
		break;
	case AP_AUXADC_DISO_VUSB_CHANNEL:
		trigger_flag = DISO_Polling.vusb_polling_measure.notify_irq;
		pr_err("[CHRG][DISO]VUSB IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel,
			 trigger_flag);
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		if (trigger_flag == DISO_IRQ_FALLING) {
			DISO_data.diso_state.pre_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[4]);
		} else if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[6]);
		} else
			pr_err("[CHRG][%s] wrong trigger flag!\n", __func__);
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		break;
	default:
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		pr_err("[CHRG][DISO]VUSB auxadc IRQ triggered ERROR OR TEST\n");
		return;		/* in error or unexecpt state just return */
	}

	g_diso_state = *(int *)&DISO_data.diso_state;
	pr_err("[CHRG][DISO]g_diso_state: 0x%x\n", g_diso_state);
	DISO_data.irq_callback_func(0, NULL);

	return;
}
#else
static int irqreturn_t diso_auxadc_irq_handler(int irq, void *dev_id)
{
	int trigger_channel = -1;
	int trigger_flag = -1;
	trigger_channel = mt_auxadc_getCurrentChannel();
	pr_err("[CHRG][DISO]auxadc handler triggered\n");
	switch (trigger_channel) {
	case AP_AUXADC_DISO_VDC_CHANNEL:
		trigger_flag = mt_auxadc_getCurrentTrigger();
		pr_err("[CHRG][DISO]VDC IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel,
			 trigger_flag);
#ifdef MTK_DISCRETE_SWITCH	/*for DSC DC plugin handle */
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_FALLING);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[2]);
		}
#else				/* for load switch OTG leakage handle */
		set_vdc_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_ONLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[5]);
		} else {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_otg_state = DISO_ONLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[1]);
		}
#endif
		break;
	case AP_AUXADC_DISO_VUSB_CHANNEL:
		trigger_flag = mt_auxadc_getCurrentTrigger();
		pr_err("[CHRG][DISO]VUSB IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel,
			 trigger_flag);
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		if (trigger_flag == DISO_IRQ_FALLING) {
			DISO_data.diso_state.pre_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[4]);
		} else {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			pr_err("[CHRG] cur diso_state is %s!\n", DISO_state_s[6]);
		}

		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		break;
	default:
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		pr_err("[CHRG][DISO]VUSB auxadc IRQ triggered ERROR OR TEST\n");
		return IRQ_HANDLED;	/* in error or unexecpt state just return */
	}
	g_diso_state = *(int *)&DISO_data.diso_state;
	return IRQ_WAKE_THREAD;
}
#endif

#if defined(MTK_DISCRETE_SWITCH) && defined(MTK_DSC_USE_EINT)
void vdc_eint_handler(void)
{
	pr_err("[CHRG][diso_eint] vdc eint irq triger\n");
	DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
	mt_eint_mask(CUST_EINT_VDC_NUM);
	do_chrdet_int_task();
}
#endif

static int diso_get_current_voltage(struct bq2560x_device *bq,int Channel)
{
	int ret = 0, data[4], i, ret_value = 0, ret_temp = 0, times = 5;

	if (IMM_IsAdcInitReady() == 0) {
		pr_err("[CHRG][DISO] AUXADC is not ready\n");
		return 0;
	}

	i = times;
	while (i--) {
		ret_value = IMM_GetOneChannelValue(Channel, data, &ret_temp);

		if (ret_value == 0) {
			ret += ret_temp;
		} else {
			times = times > 1 ? times - 1 : 1;
			pr_err("[CHRG][diso_get_current_voltage] ret_value=%d, times=%d\n",
				 ret_value, times);
		}
	}

	ret = ret * 1500 / 4096;
	ret = ret / times;

	return ret;
}

static void _get_diso_interrupt_state(struct bq2560x_device *bq)
{
	int vol = 0;
	int diso_state = 0;
	int check_times = 30;
	kal_bool vin_state = KAL_FALSE;

#ifndef VIN_SEL_FLAG
	mdelay(AUXADC_CHANNEL_DELAY_PERIOD);
#endif

	vol = diso_get_current_voltage(bq,AP_AUXADC_DISO_VDC_CHANNEL);
	vol = (R_DISO_DC_PULL_UP + R_DISO_DC_PULL_DOWN) * 100 * vol / (R_DISO_DC_PULL_DOWN) / 100;
	pr_err("[CHRG][DISO]  Current DC voltage mV = %d\n", vol);

#ifdef VIN_SEL_FLAG
	/* set gpio mode for kpoc issue as DWS has no default setting */
	mt_set_gpio_mode(vin_sel_gpio_number, 0);	/* 0:GPIO mode */
	mt_set_gpio_dir(vin_sel_gpio_number, 0);	/* 0: input, 1: output */

	if (vol > VDC_MIN_VOLTAGE / 1000 && vol < VDC_MAX_VOLTAGE / 1000) {
		/* make sure load switch already switch done */
		do {
			check_times--;
#ifdef VIN_SEL_FLAG_DEFAULT_LOW
			vin_state = mt_get_gpio_in(vin_sel_gpio_number);
#else
			vin_state = mt_get_gpio_in(vin_sel_gpio_number);
			vin_state = (~vin_state) & 0x1;
#endif
			if (!vin_state)
				mdelay(5);
		} while ((!vin_state) && check_times);
		pr_err("[CHRG][DISO] i==%d  gpio_state= %d\n",
			 check_times, mt_get_gpio_in(vin_sel_gpio_number));

		if (0 == check_times)
			diso_state &= ~0x4;	/* SET DC bit as 0 */
		else
			diso_state |= 0x4;	/* SET DC bit as 1 */
	} else {
		diso_state &= ~0x4;	/* SET DC bit as 0 */
	}
#else
	mdelay(SWITCH_RISING_TIMING + LOAD_SWITCH_TIMING_MARGIN);
	/* force delay for switching as no flag for check switching done */
	if (vol > VDC_MIN_VOLTAGE / 1000 && vol < VDC_MAX_VOLTAGE / 1000)
		diso_state |= 0x4;	/* SET DC bit as 1 */
	else
		diso_state &= ~0x4;	/* SET DC bit as 0 */
#endif


	vol = diso_get_current_voltage(bq,AP_AUXADC_DISO_VUSB_CHANNEL);
	vol =
	    (R_DISO_VBUS_PULL_UP +
	     R_DISO_VBUS_PULL_DOWN) * 100 * vol / (R_DISO_VBUS_PULL_DOWN) / 100;
	pr_err("[CHRG][DISO]  Current VBUS voltage  mV = %d\n", vol);

	if (vol > VBUS_MIN_VOLTAGE / 1000 && vol < VBUS_MAX_VOLTAGE / 1000) {
		if (!mt_usb_is_device()) {
			diso_state |= 0x1;	/* SET OTG bit as 1 */
			diso_state &= ~0x2;	/* SET VBUS bit as 0 */
		} else {
			diso_state &= ~0x1;	/* SET OTG bit as 0 */
			diso_state |= 0x2;	/* SET VBUS bit as 1; */
		}

	} else {
		diso_state &= 0x4;	/* SET OTG and VBUS bit as 0 */
	}
	pr_err("[CHRG][DISO] DISO_STATE==0x%x\n", diso_state);
	g_diso_state = diso_state;
	return;
}

#if !defined(MTK_AUXADC_IRQ_SUPPORT)
int _get_irq_direction(int pre_vol, int cur_vol)
{
	int ret = -1;

	/* threshold 1000mv */
	if ((cur_vol - pre_vol) > 1000)
		ret = DISO_IRQ_RISING;
	else if ((pre_vol - cur_vol) > 1000)
		ret = DISO_IRQ_FALLING;

	return ret;
}

static void _get_polling_state(void)
{
	int vdc_vol = 0, vusb_vol = 0;
	int vdc_vol_dir = -1;
	int vusb_vol_dir = -1;

	DISO_polling_channel *VDC_Polling = &DISO_Polling.vdc_polling_measure;
	DISO_polling_channel *VUSB_Polling = &DISO_Polling.vusb_polling_measure;

	vdc_vol = diso_get_current_voltage(bq,AP_AUXADC_DISO_VDC_CHANNEL);
	vdc_vol =
	    (R_DISO_DC_PULL_UP + R_DISO_DC_PULL_DOWN) * 100 * vdc_vol / (R_DISO_DC_PULL_DOWN) / 100;

	vusb_vol = diso_get_current_voltage(bq,AP_AUXADC_DISO_VUSB_CHANNEL);
	vusb_vol =
	    (R_DISO_VBUS_PULL_UP +
	     R_DISO_VBUS_PULL_DOWN) * 100 * vusb_vol / (R_DISO_VBUS_PULL_DOWN) / 100;

	VDC_Polling->preVoltage = VDC_Polling->curVoltage;
	VUSB_Polling->preVoltage = VUSB_Polling->curVoltage;
	VDC_Polling->curVoltage = vdc_vol;
	VUSB_Polling->curVoltage = vusb_vol;

	if (DISO_Polling.reset_polling) {
		DISO_Polling.reset_polling = KAL_FALSE;
		VDC_Polling->preVoltage = vdc_vol;
		VUSB_Polling->preVoltage = vusb_vol;

		if (vdc_vol > 1000)
			vdc_vol_dir = DISO_IRQ_RISING;
		else
			vdc_vol_dir = DISO_IRQ_FALLING;

		if (vusb_vol > 1000)
			vusb_vol_dir = DISO_IRQ_RISING;
		else
			vusb_vol_dir = DISO_IRQ_FALLING;
	} else {
		/* get voltage direction */
		vdc_vol_dir = _get_irq_direction(VDC_Polling->preVoltage, VDC_Polling->curVoltage);
		vusb_vol_dir =
		    _get_irq_direction(VUSB_Polling->preVoltage, VUSB_Polling->curVoltage);
	}

	if (VDC_Polling->notify_irq_en && (vdc_vol_dir == VDC_Polling->notify_irq)) {
		schedule_delayed_work(&diso_polling_work, 10 * HZ / 1000);	/* 10ms */
		pr_err("[CHRG][%s] ready to trig VDC irq, irq: %d\n",
			 __func__, VDC_Polling->notify_irq);
	} else if (VUSB_Polling->notify_irq_en && (vusb_vol_dir == VUSB_Polling->notify_irq)) {
		schedule_delayed_work(&diso_polling_work, 10 * HZ / 1000);
		pr_err("[CHRG][%s] ready to trig VUSB irq, irq: %d\n",
			 __func__, VUSB_Polling->notify_irq);
	} else if ((vdc_vol == 0) && (vusb_vol == 0)) {
		VDC_Polling->notify_irq_en = 0;
		VUSB_Polling->notify_irq_en = 0;
	}

	return;
}

enum hrtimer_restart diso_kthread_hrtimer_func(struct hrtimer *timer)
{
	diso_thread_timeout = KAL_TRUE;
	wake_up(&diso_polling_thread_wq);

	return HRTIMER_NORESTART;
}

int diso_thread_kthread(void *x)
{
	/* Run on a process content */
	while (1) {
		wait_event(diso_polling_thread_wq, (diso_thread_timeout == KAL_TRUE));

		diso_thread_timeout = KAL_FALSE;

		mutex_lock(&diso_polling_mutex);

		_get_polling_state();

		if (DISO_Polling.vdc_polling_measure.notify_irq_en ||
		    DISO_Polling.vusb_polling_measure.notify_irq_en)
			hrtimer_start(&diso_kthread_timer,
				      ktime_set(0, MSEC_TO_NSEC(SW_POLLING_PERIOD)),
				      HRTIMER_MODE_REL);
		else
			hrtimer_cancel(&diso_kthread_timer);

		mutex_unlock(&diso_polling_mutex);
	}

	return 0;
}
#endif
#endif


#if 0
static int charging_diso_init(struct bq2560x_device *bq,void *data)
{
	int status = STATUS_OK;

#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
	DISO_ChargerStruct *pDISO_data = (DISO_ChargerStruct *) data;

	/* Initialization DISO Struct */
	pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
	pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
	pDISO_data->diso_state.cur_vdc_state = DISO_OFFLINE;

	pDISO_data->diso_state.pre_otg_state = DISO_OFFLINE;
	pDISO_data->diso_state.pre_vusb_state = DISO_OFFLINE;
	pDISO_data->diso_state.pre_vdc_state = DISO_OFFLINE;

	pDISO_data->chr_get_diso_state = KAL_FALSE;
	pDISO_data->hv_voltage = VBUS_MAX_VOLTAGE;

#if !defined(MTK_AUXADC_IRQ_SUPPORT)
	hrtimer_init(&diso_kthread_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	diso_kthread_timer.function = diso_kthread_hrtimer_func;
	INIT_DELAYED_WORK(&diso_polling_work, diso_polling_handler);

	kthread_run(diso_thread_kthread, NULL, "diso_thread_kthread");
	pr_err("[CHRG][%s] done\n", __func__);
#else
	struct device_node *node;
	int ret;

	/* Initial AuxADC IRQ */
	DISO_IRQ.vdc_measure_channel.number = AP_AUXADC_DISO_VDC_CHANNEL;
	DISO_IRQ.vusb_measure_channel.number = AP_AUXADC_DISO_VUSB_CHANNEL;
	DISO_IRQ.vdc_measure_channel.period = AUXADC_CHANNEL_DELAY_PERIOD;
	DISO_IRQ.vusb_measure_channel.period = AUXADC_CHANNEL_DELAY_PERIOD;
	DISO_IRQ.vdc_measure_channel.debounce = AUXADC_CHANNEL_DEBOUNCE;
	DISO_IRQ.vusb_measure_channel.debounce = AUXADC_CHANNEL_DEBOUNCE;

	/* use default threshold voltage, if use high voltage,maybe refine */
	DISO_IRQ.vusb_measure_channel.falling_threshold = VBUS_MIN_VOLTAGE / 1000;
	DISO_IRQ.vdc_measure_channel.falling_threshold = VDC_MIN_VOLTAGE / 1000;
	DISO_IRQ.vusb_measure_channel.rising_threshold = VBUS_MIN_VOLTAGE / 1000;
	DISO_IRQ.vdc_measure_channel.rising_threshold = VDC_MIN_VOLTAGE / 1000;

	node = of_find_compatible_node(NULL, NULL, "mediatek,AUXADC");
	if (!node) {
		pr_err("[CHRG][diso_adc]: of_find_compatible_node failed!!\n");
	} else {
		pDISO_data->irq_line_number = irq_of_parse_and_map(node, 0);
		pr_err("[CHRG][diso_adc]: IRQ Number: 0x%x\n", pDISO_data->irq_line_number);
	}

	mt_irq_set_sens(pDISO_data->irq_line_number, MT_EDGE_SENSITIVE);
	mt_irq_set_polarity(pDISO_data->irq_line_number, MT_POLARITY_LOW);

	ret = request_threaded_irq(pDISO_data->irq_line_number, diso_auxadc_irq_handler,
				   pDISO_data->irq_callback_func, IRQF_ONESHOT, "DISO_ADC_IRQ",
				   NULL);

	if (ret) {
		pr_err("[CHRG][diso_adc]: request_irq failed.\n");
	} else {
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		pr_err("[CHRG][diso_adc]: diso_init success.\n");
	}
#endif

#if defined(MTK_DISCRETE_SWITCH) && defined(MTK_DSC_USE_EINT)
	pr_err("[CHRG][diso_eint]vdc eint irq registitation\n");
	mt_eint_set_hw_debounce(CUST_EINT_VDC_NUM, CUST_EINT_VDC_DEBOUNCE_CN);
	mt_eint_registration(CUST_EINT_VDC_NUM, CUST_EINTF_TRIGGER_LOW, vdc_eint_handler, 0);
	mt_eint_mask(CUST_EINT_VDC_NUM);
#endif
#endif

	return status;
}
#endif
#if 0

static int charging_get_diso_state(struct bq2560x_device *bq,void *data)
{
	int status = STATUS_OK;

#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
	int diso_state = 0x0;
	DISO_ChargerStruct *pDISO_data = (DISO_ChargerStruct *) data;

	_get_diso_interrupt_state(bq);
	diso_state = g_diso_state;
	pr_err("[CHRG][do_chrdet_int_task] current diso state is %s!\n", DISO_state_s[diso_state]);
	if (((diso_state >> 1) & 0x3) != 0x0) {
		switch (diso_state) {
		case USB_ONLY:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
#ifdef MTK_DISCRETE_SWITCH
#ifdef MTK_DSC_USE_EINT
			mt_eint_unmask(CUST_EINT_VDC_NUM);
#else
			set_vdc_auxadc_irq(DISO_IRQ_ENABLE, 1);
#endif
#endif
			pDISO_data->diso_state.cur_vusb_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
			break;
		case DC_ONLY:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_RISING);
			pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
			break;
		case DC_WITH_USB:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_FALLING);
			pDISO_data->diso_state.cur_vusb_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
			break;
		case DC_WITH_OTG:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
			pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_ONLINE;
			break;
		default:	/* OTG only also can trigger vcdt IRQ */
			pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_ONLINE;
			pr_err("[CHRG] switch load vcdt irq triggerd by OTG Boost!\n");
			break;	/* OTG plugin no need battery sync action */
		}
	}

	if (DISO_ONLINE == pDISO_data->diso_state.cur_vdc_state)
		pDISO_data->hv_voltage = VDC_MAX_VOLTAGE;
	else
		pDISO_data->hv_voltage = VBUS_MAX_VOLTAGE;
#endif

	return status;
}
#endif

static int charging_get_error_state(struct bq2560x_device *bq)
{
	return charging_error;
}
#if 0
static int charging_set_error_state(struct bq2560x_device *bq,void *data)
{
	int status = STATUS_OK;
	charging_error = *(unsigned int *) (data);

	return status;
}

static int charging_set_vindpm(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_set_vbus_ovp_en(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_get_bif_vbat(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_set_chrind_ck_pdn(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_sw_init(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_enable_safetytimer(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_set_hiz_swchr(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}

static int charging_get_bif_tbat(struct bq2560x_device *bq)
{
	int status = STATUS_OK;
	return status;
}
#endif

static void rt_parse_dt(struct device *dev, struct bq2560x_platform_data *pdata)
{
	/* just used to prevent the null parameter */
	if (!dev || !pdata)
		return;
	if (of_property_read_string(dev->of_node, "charger_name",
				    &pdata->chg_name) < 0)
		dev_warn(dev, "not specified chg_name\n");
	if (of_property_read_u32(dev->of_node, "ichg", &pdata->ichg) < 0)
		dev_warn(dev, "not specified ichg value\n");
	if (of_property_read_u32(dev->of_node, "aicr", &pdata->aicr) < 0)
		dev_warn(dev, "not specified aicr value\n");
	if (of_property_read_u32(dev->of_node, "mivr", &pdata->mivr) < 0)
		dev_warn(dev, "not specified mivr value\n");
	if (of_property_read_u32(dev->of_node, "ieoc", &pdata->ieoc) < 0)
		dev_warn(dev, "not specified ieoc_value\n");
	if (of_property_read_u32(dev->of_node, "cv", &pdata->voreg) < 0)
		dev_warn(dev, "not specified cv value\n");
	if (of_property_read_u32(dev->of_node, "vmreg", &pdata->vmreg) < 0)
		dev_warn(dev, "not specified vmreg value\n");
	pdata->enable_te = of_property_read_bool(dev->of_node, "enable_te");
	pdata->enable_eoc_shdn = of_property_read_bool(dev->of_node,
						       "enable_eoc_shdn");
}

static int bq2560x_i2c_probe(struct i2c_client *client,
			    const struct i2c_device_id *id)
{
	struct bq2560x_device *bq;
	struct bq2560x_platform_data *pdata  = dev_get_platdata(&client->dev);
	int ret;
	bool use_dt = client->dev.of_node;

	bq = kzalloc(sizeof(struct bq2560x_device), GFP_KERNEL);
	if (!bq){			
		return -ENOMEM;
	}

	bq->dev = &client->dev;
	bq->client = client;
	bq->irq = client->irq;

	i2c_set_clientdata(client, bq);

	ret=bq25601_get_id(bq);
	if(ret){


	}

	/*init device*/
	ret = bq2560x_init_device(bq, pdata);
	if (ret) {
		dev_err(bq->dev, "[%s][%d]device init failure: %d\n",__func__,__LINE__, ret);
		goto err_dev;
	}

	bq2560x_dump_register(bq);

	if (use_dt) {
		pdata = devm_kzalloc(&client->dev, sizeof(*pdata), GFP_KERNEL);
		if (!pdata){
			return -ENOMEM;
		}
		memcpy(pdata, &bq2560x_pdata, sizeof(*pdata));

		client->dev.platform_data = pdata;
		rt_parse_dt(&client->dev, pdata);
	} else {
		if (!pdata) {
			dev_info(&client->dev, "no pdata specify\n");
			return -EINVAL;
		}
	}
	chargin_hw_init_done = KAL_TRUE;

#if defined(CONFIG_MTK_GAUGE_VERSION) && (CONFIG_MTK_GAUGE_VERSION == 30)
	/* charger class register */
	bq->chg_dev = charger_device_register(pdata->chg_name, bq->dev, bq, 
			&bq2560x_chg_ops, &bq2560x_chg_props);
	if (IS_ERR(bq->chg_dev)) {
		dev_info(bq->dev, "charger device register fail\n");
		return PTR_ERR(bq->chg_dev);
	}
#endif /* #if (CONFIG_MTK_GAUGE_VERSION == 30) */
	return 0;

err_dev:
	kfree(bq);
	return ret;

}

static int bq25601_i2c_remove(struct i2c_client *client)
{
	struct bq2560x_device *bq = i2c_get_clientdata(client);
	
	#if defined(CONFIG_MTK_GAUGE_VERSION) && (CONFIG_MTK_GAUGE_VERSION == 30)
	charger_device_unregister(bq->chg_dev);
	#endif /* #if (CONFIG_MTK_GAUGE_VERSION == 30) */

	kfree(bq);
	return 0;
}

static const struct i2c_device_id i2c_id_table[] = {
	{ "bq2560x", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, i2c_id_table);

static const struct of_device_id of_id_table[] = {
	{ .compatible = "bq2560x"},
	{},
};
MODULE_DEVICE_TABLE(of, of_id_table);

static int bq2560x_i2c_suspend(struct device *dev)
{
	return 0;
}

static int bq2560x_i2c_resume(struct device *dev)
{
	return 0;
}
static SIMPLE_DEV_PM_OPS(bq2560x_pm_ops, bq2560x_i2c_suspend, bq2560x_i2c_resume);

static struct i2c_driver bq2560x_i2c_driver = {
	.driver = {
		.name = "bq2560x",
		.owner = THIS_MODULE,
		.pm = &bq2560x_pm_ops,
		.of_match_table = of_match_ptr(of_id_table),
	},
	.probe = bq2560x_i2c_probe,
	.remove = bq25601_i2c_remove,
	.id_table = i2c_id_table,
};

module_i2c_driver(bq2560x_i2c_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("BQ2560X Charger driver");
MODULE_AUTHOR("ZC <XXX@droi.com>");
MODULE_VERSION("1.0.0");



