/*
 *
 * FocalTech ft5x06 TouchScreen driver.
 *
 * Copyright (c) 2010  Focal tech Ltd.
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/debugfs.h>
#include "ft5x06_ts.h"

#if defined(CONFIG_FB)
#include <linux/notifier.h>
#include <linux/fb.h>

#elif defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
/* Early-suspend level */
#define FT_SUSPEND_LEVEL 1
#endif

/* fts debug switch */
#undef DEBUG_MASK
//#define DEBUG_MASK

#ifdef DEBUG_MASK
#define DEBUG_TAG  "ft5x06_ts"
#define FT_DEBUG(fmt, args...)  printk(DEBUG_TAG" %s : "fmt, __FUNCTION__, ##args)
#else
#define FT_DEBUG(fmt, args...)
#endif

#define FT_HAVE_KEY 1

#define FT_DRIVER_VERSION   0x02

#define FT_META_REGS        3
#define FT_ONE_TCH_LEN      6
#define FT_TCH_LEN(x)       (FT_META_REGS + FT_ONE_TCH_LEN * x)

#define CFG_MAX_TOUCH_POINTS    5

#define FT_STARTUP_DLY      250
#define FT_RESET_DLY        20
#define FT_LPM_DLY       20
#define FT_DELAY_DFLT       20
#define FT_NUM_RETRY        10
#define FT_TOUCH_STEP  6

#define POINT_READ_BUF  (3 + FT_TOUCH_STEP * CFG_MAX_TOUCH_POINTS)

#define FT_PRESS        0x7F
#define FT_MAX_ID       0x0F
#define FT_TOUCH_X_H_POS    3
#define FT_TOUCH_X_L_POS    4
#define FT_TOUCH_Y_H_POS    5
#define FT_TOUCH_Y_L_POS    6
#define FT_TD_STATUS        2
#define FT_TOUCH_EVENT_POS  3
#define FT_TOUCH_ID_POS     5
#define FT_TOUCH_DOWN       0
#define FT_TOUCH_CONTACT    2

/*register address*/
#define FT_REG_DEV_MODE     0x00
#define FT_DEV_MODE_REG_CAL 0x02
#define FT_CHIP_ID       0xA3
#define FT_REG_PMODE        0xA5
#define FT_REG_FW_VER       0xA6
#define FT_VENDOR_ID        0xA8
#define FT_REG_POINT_RATE   0x88
#define FT_REG_THGROUP      0x80
#define FT_REG_ECC      0xCC
#define FT_REG_RESET_FW     0x07
#define FT_REG_FW_MIN_VER   0xB2
#define FT_REG_FW_SUB_MIN_VER   0xB3

/* power register bits*/
#define FT_PMODE_ACTIVE     0x00
#define FT_PMODE_MONITOR    0x01
#define FT_PMODE_STANDBY    0x02
#define FT_PMODE_HIBERNATE  0x03
#define FT_FACTORYMODE_VALUE    0x40
#define FT_WORKMODE_VALUE   0x00
#define FT_RST_CMD_REG1     0xFC
#define FT_RST_CMD_REG2     0xBC
#define FT_READ_ID_REG      0x90
#define FT_ERASE_APP_REG    0x61
#define FT_ERASE_PANEL_REG  0x63
#define FT_FW_START_REG     0xBF

#define FT_STATUS_NUM_TP_MASK   0x0F

#define FT_VTG_MIN_UV       2600000
#define FT_VTG_MAX_UV       3300000
#define FT_I2C_VTG_MIN_UV   1800000
#define FT_I2C_VTG_MAX_UV   1800000

#define FT_COORDS_ARR_SIZE  4

#define FT_8BIT_SHIFT       8
#define FT_4BIT_SHIFT       4
#define FT_FW_NAME_MAX_LEN  50

#define FT_UPGRADE_AA       0xAA
#define FT_UPGRADE_55       0x55

#define FT_FW_MIN_SIZE      8
#define FT_FW_MAX_SIZE      32768

/* Firmware file is not supporting minor and sub minor so use 0 */
#define FT_FW_FILE_MAJ_VER(x)   ((x)->data[(x)->size - 2])
#define FT_FW_FILE_MIN_VER(x)   0
#define FT_FW_FILE_SUB_MIN_VER(x) 0

#define FT_FW_CHECK(x)      \
    (((x)->data[(x)->size - 8] ^ (x)->data[(x)->size - 6]) == 0xFF \
    && (((x)->data[(x)->size - 7] ^ (x)->data[(x)->size - 5]) == 0xFF \
    && (((x)->data[(x)->size - 3] ^ (x)->data[(x)->size - 4]) == 0xFF)))


#define PROP_NAME_SIZE      32

#define FT_MAX_WR_BUF       10
#define FT_MAX_RD_BUF       2
#define FT_FW_PKT_LEN       128
#define FT_FW_PKT_META_LEN  6
#define FT_FW_PKT_DLY_MS    20
#define FT_FW_LAST_PKT      0x6ffa
#define FT_EARSE_DLY_MS     100
#define FT_55_AA_DLY_NS     5000

#define FT_UPGRADE_LOOP     30
#define FT_CAL_START        0x04
#define FT_CAL_FIN      0x00
#define FT_CAL_STORE        0x05
#define FT_CAL_RETRY        100
#define FT_REG_CAL      0x00
#define FT_CAL_MASK     0x70

#define FT_INFO_MAX_LEN     512

#define FT_BLOADER_SIZE_OFF 12
#define FT_BLOADER_NEW_SIZE 30
#define FT_DATA_LEN_OFF_OLD_FW  8
#define FT_DATA_LEN_OFF_NEW_FW  14
#define FT_FINISHING_PKT_LEN_OLD_FW 6
#define FT_FINISHING_PKT_LEN_NEW_FW 12
#define FT_MAGIC_BLOADER_Z7 0x7bfa
#define FT_MAGIC_BLOADER_LZ4    0x6ffa
#define FT_MAGIC_BLOADER_GZF_30 0x7ff4
#define FT_MAGIC_BLOADER_GZF    0x7bf4

#ifdef CONFIG_FT5X06_GESTURE
#include "ft_gesture_lib.h"

#define FTS_GESTURE_POINTS 255
#define FTS_GESTURE_POINTS_ONETIME  62
#define FTS_GESTURE_POINTS_HEADER 8
#define FTS_GESTURE_OUTPUT_ADRESS 0xD3
#define FTS_GESTURE_OUTPUT_UNIT_LENGTH 4

#endif

enum {
    FT_BLOADER_VERSION_LZ4 = 0,
    FT_BLOADER_VERSION_Z7 = 1,
    FT_BLOADER_VERSION_GZF = 2,
};

enum {
    FT_FT5336_FAMILY_ID_0x11 = 0x11,
    FT_FT5336_FAMILY_ID_0x12 = 0x12,
    FT_FT5336_FAMILY_ID_0x13 = 0x13,
    FT_FT5336_FAMILY_ID_0x14 = 0x14,
};

#define FT_STORE_TS_INFO(buf, id, name, max_tch, group_id, fw_vkey_support, \
            fw_name, fw_maj, fw_min, fw_sub_min) \
            snprintf(buf, FT_INFO_MAX_LEN, \
                "controller\t= focaltech\n" \
                "model\t\t= 0x%x\n" \
                "name\t\t= %s\n" \
                "max_touches\t= %d\n" \
                "drv_ver\t\t= 0x%x\n" \
                "group_id\t= 0x%x\n" \
                "fw_vkey_support\t= %s\n" \
                "fw_name\t\t= %s\n" \
                "fw_ver\t\t= %d.%d.%d\n", id, name, \
                max_tch, FT_DRIVER_VERSION, group_id, \
                fw_vkey_support, fw_name, fw_maj, fw_min, \
                fw_sub_min)

#define FT_DEBUG_DIR_NAME   "ts_debug"

static int version;
module_param(version, int, 0644);
MODULE_PARM_DESC(version, "ft5x06_ts device version");

static int vendorid;
module_param(vendorid, int, 0644);
MODULE_PARM_DESC(vendorid, "ft5x06_ts device vendorid");

static char device_name[32] = "ft5x06_ts";
module_param_string(name, device_name, sizeof(device_name), 0644);
MODULE_PARM_DESC(name, "ft5x06_ts device name");

struct ts_event {
    u16 x[CFG_MAX_TOUCH_POINTS];    /*x coordinate */
    u16 y[CFG_MAX_TOUCH_POINTS];    /*y coordinate */
    /* touch event: 0 -- down; 1-- up; 2 -- hold */
    u8 touch_event[CFG_MAX_TOUCH_POINTS];
    u8 finger_id[CFG_MAX_TOUCH_POINTS]; /*touch ID */
    u8 pressure[CFG_MAX_TOUCH_POINTS];
    u8 touch_point;
};

struct ft5x06_ts_data {
    struct i2c_client *client;
    struct input_dev *input_dev;
    struct ts_event event;
    struct ts_event last_event;
    struct ft5x06_ts_platform_data *pdata;
    struct regulator *vdd;
    struct regulator *vcc_i2c;
    char fw_name[FT_FW_NAME_MAX_LEN];
    bool loading_fw;
    u8 chip_id;
    u8 family_id;
    u8 vendor_id;
    u8 *fw_ifile_data;
    int fw_ifile_data_len;
    u8 current_fw_ver;
    struct dentry *dir;
    u16 addr;
    bool suspended;
    char *ts_info;
    u8 *tch_data;
    u32 tch_data_len;
    u8 fw_ver[3];
    u8 irq_disabled;
    u8 irq_need_enable;
    struct work_struct pen_event_work;
    struct workqueue_struct *ts_workqueue;
#if defined(CONFIG_FB)
    struct notifier_block fb_notif;
#elif defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend early_suspend;
#endif
#ifdef CONFIG_TS_GLOVE_SWITCH
    struct mutex glove_lock;
    int glove_state;
#endif
#ifdef CONFIG_FT5X06_GESTURE
    enum {
       FT5X06_GS_ACTIVE,
       FT5X06_GS_SUSPEND,
       FT5X06_GS_IRQPENDING,
       FT5X06_GS_DISABLE,
    }gs_state;
#endif
};

static const struct {int chip_id; const char *name;} ic_chip_type[] = {
    {0, "ft5436"},
    {1, "ft5336"},
    {2, "ft5446"},
    {3, "ft6x06"},
    {4, "ft6x36"},
    {5, "ft5406"},
    {-1, NULL},
};

static const char* get_ic_type_by_id(int chip_id)
{
    int i;

    for (i = 0; ic_chip_type[i].name != NULL; i++) {
        if (chip_id == ic_chip_type[i].chip_id) {
            return ic_chip_type[i].name;
        }
    }

    return "unknow type";
}

#ifdef CONFIG_TS_GLOVE_SWITCH
static ssize_t glove_switch_show(struct device *dev, struct device_attribute *attr,
        char *buf);
static ssize_t glove_switch_store(struct device *dev, struct device_attribute *attr,
         const char *buf, size_t count);

static DEVICE_ATTR(glove, S_IRUGO | S_IWUGO,
               glove_switch_show,
               glove_switch_store);
#endif

#ifdef CONFIG_FT5X06_GESTURE
static ssize_t gesture_switch_show(struct device *dev, struct device_attribute *attr,
        char *buf);
static ssize_t gesture_switch_store(struct device *dev, struct device_attribute *attr,
         const char *buf, size_t count);

static DEVICE_ATTR(gesture, S_IRUGO | S_IWUGO,
               gesture_switch_show,
               gesture_switch_store);
#endif
#if FT_HAVE_KEY
u32 tsp_keycodes[MAX_BUTTONS];
#define CFG_NUMOFKEYS  (sizeof(tsp_keycodes)/sizeof(int))

static bool tsp_keystatus[CFG_NUMOFKEYS];
static bool key_flag = 1;

static void ft_irq_enable(struct ft5x06_ts_data *data)
{
    if(data->irq_need_enable) {
        if(data->irq_disabled) {
            enable_irq(data->client->irq);
            data->irq_disabled = 0;
        }
    }
}

static void ft_irq_disable(struct ft5x06_ts_data *data)
{
    if (!data->irq_disabled) {
        disable_irq_nosync(data->client->irq);
        data->irq_disabled = 1;
    }
}

int ft5x0x_touch_key_process(struct input_dev *dev, int x, int y, int touch_event)
{
    struct ft5x06_ts_data *data = input_get_drvdata(dev);
    struct ft5x06_ts_platform_data *pdata = data->pdata;
    int i;
    int key_id;

    if (x == pdata->button_xcoords[0])
    {
        key_id = 0;
    }
    else if (x == pdata->button_xcoords[1])
    {
        key_id = 1;
    }
     else if (x == pdata->button_xcoords[2])
    {
        key_id = 2;
    }
    else
    {
        key_id = 0xf;
    }

    for(i = 0; i < CFG_NUMOFKEYS; i++ )
    {
        FT_DEBUG("key_id : %d, tsp_keystatus[i] : %d\n", key_id, tsp_keycodes[i]);
        if(tsp_keystatus[i]&&(key_flag ==0))
        {
            input_report_key(dev, tsp_keycodes[i], 0);
            FT_DEBUG("Keycode : %d is release.\n", tsp_keycodes[i]);
            tsp_keystatus[i] = 0;
        }
        else if( key_id == i )
        {
            FT_DEBUG("touch_event  %d\n",touch_event);
            if( touch_event == 0)                   // detect
            {
                input_report_key(dev, tsp_keycodes[i], 1);
                FT_DEBUG( "Keycode : %d is pressed.\n", tsp_keycodes[i]);
                tsp_keystatus[i] = 1;
            }
        }
    }
    return 0;
}

#endif

static int ft5x06_i2c_read(struct i2c_client *client, char *writebuf,
               int writelen, char *readbuf, int readlen)
{
    int ret;

    if (writelen > 0) {
        struct i2c_msg msgs[] = {
            {
                 .addr = client->addr,
                 .flags = 0,
                 .len = writelen,
                 .buf = writebuf,
             },
            {
                 .addr = client->addr,
                 .flags = I2C_M_RD,
                 .len = readlen,
                 .buf = readbuf,
             },
        };
        ret = i2c_transfer(client->adapter, msgs, 2);
        if (ret < 0)
            dev_err(&client->dev, "%s: i2c read error.\n",
                __func__);
    } else {
        struct i2c_msg msgs[] = {
            {
                 .addr = client->addr,
                 .flags = I2C_M_RD,
                 .len = readlen,
                 .buf = readbuf,
             },
        };
        ret = i2c_transfer(client->adapter, msgs, 1);
        if (ret < 0)
            dev_err(&client->dev, "%s:i2c read error.\n", __func__);
    }
    return ret;
}

static int ft5x06_i2c_write(struct i2c_client *client, char *writebuf,
                int writelen)
{
    int ret;

    struct i2c_msg msgs[] = {
        {
             .addr = client->addr,
             .flags = 0,
             .len = writelen,
             .buf = writebuf,
         },
    };
    ret = i2c_transfer(client->adapter, msgs, 1);
    if (ret < 0)
        dev_err(&client->dev, "%s: i2c write error.\n", __func__);

    return ret;
}

static int ft5x0x_write_reg(struct i2c_client *client, u8 addr, const u8 val)
{
    u8 buf[2] = {0};
    buf[0] = addr;
    buf[1] = val;

    return ft5x06_i2c_write(client, buf, sizeof(buf));
}

static int ft5x0x_read_reg(struct i2c_client *client, u8 addr, u8 *val)
{
    return ft5x06_i2c_read(client, &addr, 1, val, 1);
}

#ifdef CONFIG_TOUCHSCREEN_FT5X06_CTP_UPG
typedef enum
{
    ERR_OK,
    ERR_MODE,
    ERR_READID,
    ERR_ERASE,
    ERR_DATA,
    ERR_ECC
}E_UPGRADE_ERR_TYPE;

#include "ft6x06_upgrade.c"
#include "ft6x36_upgrade.c"
#include "ft5x46_upgrade.c"
#include "ft5x36_upgrade.c"
#endif

static void ft5x06_update_fw_ver(struct ft5x06_ts_data *data)
{
    struct i2c_client *client = data->client;
    u8 reg_addr;
    int err;

    reg_addr = FT_REG_FW_VER;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &data->fw_ver[0], 1);
    if (err < 0)
        dev_err(&client->dev, "fw major version read failed");

    reg_addr = FT_REG_FW_MIN_VER;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &data->fw_ver[1], 1);
    if (err < 0)
        dev_err(&client->dev, "fw minor version read failed");

    reg_addr = FT_REG_FW_SUB_MIN_VER;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &data->fw_ver[2], 1);
    if (err < 0)
        dev_err(&client->dev, "fw sub minor version read failed");

    dev_info(&client->dev, "Firmware version = %d.%d.%d\n",
        data->fw_ver[0], data->fw_ver[1], data->fw_ver[2]);
}

#ifdef CONFIG_FT5X06_GESTURE
static void check_gesture(struct ft5x06_ts_data *data, int gesture_id)
{
    unsigned int code = 0;

    code = data->pdata->keymap[gesture_id];
    if (code > 0) {
        input_report_key(data->input_dev, code, 1);
        input_sync(data->input_dev);
        input_report_key(data->input_dev, code, 0);
        input_sync(data->input_dev);
    }
    dev_info(&data->client->dev,
            "fts gesture_id = 0x%x, code = %d\n", gesture_id, code);
}

static int fts_read_Gestruedata(struct ft5x06_ts_data *data)
{
    unsigned char buf[FTS_GESTURE_POINTS * 3] = { 0 };
    int ret = -1;
    int gestrue_id = 0;
    short pointnum = 0;

    buf[0] = 0xd3;
    ret = ft5x06_i2c_read(data->client, buf, 1, buf, FTS_GESTURE_POINTS_HEADER);
    //printk( "tpd read FTS_GESTURE_POINTS_HEADER.\n");
    if (ret < 0)
    {
        printk( "%s read touchdata failed.\n", __func__);
        return ret;
    }

    /* FW */
    if (data->chip_id == FT5X46_ID)
    {
        gestrue_id = buf[0];
        pointnum = (short)(buf[1]) & 0xff;
        buf[0] = 0xd3;

        if((pointnum * 4 + 8)<255)
        {
            ret = ft5x06_i2c_read(data->client, buf, 1, buf, (pointnum * 4 + 8));
        }
        else
        {
            ret = ft5x06_i2c_read(data->client, buf, 1, buf, 255);
            ret = ft5x06_i2c_read(data->client, buf, 0, buf+255, (pointnum * 4 + 8) -255);
        }
        if (ret < 0)
        {
            printk( "%s read touchdata failed.\n", __func__);
            return ret;
        }
        check_gesture(data, gestrue_id);

        return 0;
    }

    if (0x24 == buf[0])
    {
        gestrue_id = 0x24;
        check_gesture(data, gestrue_id);
        return 0;
    }

    pointnum = (short)(buf[1]) & 0xff;
    buf[0] = 0xd3;
    if((pointnum * 4 + 8)<255)
    {
        ret = ft5x06_i2c_read(data->client, buf, 1, buf, (pointnum * 4 + 8));
    }
    else
    {
        ret = ft5x06_i2c_read(data->client, buf, 1, buf, 255);
        ret = ft5x06_i2c_read(data->client, buf, 0, buf+255, (pointnum * 4 + 8) -255);
    }
    if (ret < 0)
    {
        printk( "%s read touchdata failed.\n", __func__);
        return ret;
    }

    gestrue_id = fetch_object_sample(buf, pointnum);
    check_gesture(data, gestrue_id);

    return 0;
}
#endif

static void ft5x06_report_value(struct ft5x06_ts_data *data)
{
    struct ts_event *event = &data->event;
    struct ts_event *last_event = &data->last_event;
    int i, j;
    int fingerdown = 0;

    for (i = 0; i < event->touch_point; i++) {
        if ((event->touch_event[i] == 0) ||( event->touch_event[i] == 2)) {
            event->pressure[i] = FT_PRESS;
            fingerdown++;
            #if FT_HAVE_KEY
            key_flag = 1 ;
            #endif
        } else {
            event->pressure[i] = 0;
            #if FT_HAVE_KEY
            key_flag = 0 ;
            #endif
        }

        input_mt_slot(data->input_dev, event->finger_id[i]);
        input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER,
                        !!event->pressure[i]);
        FT_DEBUG("event->pressure %d\n", event->pressure[i]);

        if (event->pressure[i] == FT_PRESS) {
            input_report_abs(data->input_dev, ABS_MT_POSITION_X,
                    event->x[i]);
            input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
                    event->y[i]);
            input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
                    event->pressure[i]);
        }
        FT_DEBUG("report finger_id[%d]= %d x= %d , y= %d ,%s\n",
                i, event->finger_id[i], event->x[i], event->y[i],
                event->pressure[i] ? "DOWN":"UP");

#if FT_HAVE_KEY
        if (event->y[i] == data->pdata->button_ycoord)
        {
            FT_DEBUG("key_flag = %d\n", key_flag);
            ft5x0x_touch_key_process(data->input_dev, event->x[i], event->y[i], event->touch_event[i]);
        }
#endif
    }

    if (last_event->touch_point > event->touch_point) {
        for (i = 0; i < last_event->touch_point; i++) {
            int need_up = 1;

            FT_DEBUG("report last finger_id[%d]= %d x= %d , y= %d ,%s\n",
                    i, last_event->finger_id[i], last_event->x[i], last_event->y[i],
                    last_event->pressure[i] ? "DOWN":"UP");

            for (j = 0; j < event->touch_point; j++) {
                if (last_event->finger_id[i] == event->finger_id[j]) {
                    need_up = 0;
                    break;
                }
            }

            if (need_up && last_event->pressure[i]) {
                FT_DEBUG("error found not release ts event finger_id %d.\n",
                        last_event->finger_id[i]);
                input_mt_slot(data->input_dev, last_event->finger_id[i]);
                input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER,
                        !last_event->pressure[i]);
            }
        }
    }

    input_report_key(data->input_dev, BTN_TOUCH, !!fingerdown);
    input_sync(data->input_dev);

    memset(last_event, 0, sizeof(struct ts_event));
    *last_event = *event;
}

static int ft5x06_handle_touchdata(struct ft5x06_ts_data *data)
{
    struct ts_event *event = &data->event;
    struct ft5x06_ts_platform_data *pdata = data->pdata;
    int ret, i;
    u8 buf[POINT_READ_BUF] = { 0 };
    u8 pointid = FT_MAX_ID;

#ifdef CONFIG_FT5X06_GESTURE
    u8 state;
    ft5x0x_read_reg(data->client, 0xD0, &state);
    //printk("tpd fts_read_Gestruedata state=%d\n",state);
    if(state == 1)
    {
        fts_read_Gestruedata(data);
        return 0;
    }
#endif

    ret = ft5x06_i2c_read(data->client, buf, 1, buf, POINT_READ_BUF);
    if (ret < 0) {
        dev_err(&data->client->dev, "%s read touchdata failed.\n",
            __func__);
        return ret;
    }
    memset(event, 0, sizeof(struct ts_event));

    event->touch_point = 0;
    for (i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
        pointid = (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
        if (pointid >= FT_MAX_ID) {
            break;
        } else {
            event->touch_point++;
        }
        event->x[i] =
            (s16) (buf[FT_TOUCH_X_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
            8 | (s16) buf[FT_TOUCH_X_L_POS + FT_TOUCH_STEP * i];
        event->y[i] =
            (s16) (buf[FT_TOUCH_Y_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
            8 | (s16) buf[FT_TOUCH_Y_L_POS + FT_TOUCH_STEP * i];

        if (pdata->x_negative) {
            event->x[i] = pdata->x_max - event->x[i];
        }
        if (pdata->y_negative) {
            event->y[i] = pdata->y_max - event->y[i];
        }

        event->touch_event[i] =
            buf[FT_TOUCH_EVENT_POS + FT_TOUCH_STEP * i] >> 6;
        event->finger_id[i] =
            (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
    }

    ft5x06_report_value(data);

    return 0;
}

static void ft5x0x_ts_pen_irq_work(struct work_struct *work)
{
    int ret = -1;
    struct ft5x06_ts_data *data = container_of(work, struct ft5x06_ts_data,
                      pen_event_work);

    ret = ft5x06_handle_touchdata(data);
    if (ret) {
        pr_err("%s: handling touchdata failed\n", __func__);
    }

    ft_irq_enable(data);

#ifdef CONFIG_FT5X06_GESTURE
    pm_relax(&data->client->dev);
#endif
}

static irqreturn_t ft5x06_ts_interrupt(int irq, void *dev_id)
{
    struct ft5x06_ts_data *ft5x0x_ts = dev_id;

#ifdef CONFIG_FT5X06_GESTURE
    pm_stay_awake(&ft5x0x_ts->client->dev);
    if (ft5x0x_ts->gs_state == FT5X06_GS_SUSPEND) {
        ft5x0x_ts->gs_state = FT5X06_GS_IRQPENDING;
        return IRQ_HANDLED;
    }
#endif

    ft_irq_disable(ft5x0x_ts);


    queue_work(ft5x0x_ts->ts_workqueue, &ft5x0x_ts->pen_event_work);

    return IRQ_HANDLED;
}

static int ft5x06_power_on(struct ft5x06_ts_data *data, bool on)
{
    int rc;

    if (!on) {
        rc = regulator_disable(data->vdd);
        if (rc) {
            dev_err(&data->client->dev,
                "Regulator vdd disable failed rc=%d\n", rc);
            return rc;
        }

        rc = regulator_disable(data->vcc_i2c);
        if (rc) {
            dev_err(&data->client->dev,
                "Regulator vcc_i2c disable failed rc=%d\n", rc);
            rc = regulator_enable(data->vdd);
            if (rc) {
                dev_err(&data->client->dev,
                    "Regulator vdd enable failed rc=%d\n", rc);
            }
        }
    } else {
        rc = regulator_enable(data->vdd);
        if (rc) {
            dev_err(&data->client->dev,
                "Regulator vdd enable failed rc=%d\n", rc);
            return rc;
        }

        rc = regulator_enable(data->vcc_i2c);
        if (rc) {
            dev_err(&data->client->dev,
                "Regulator vcc_i2c enable failed rc=%d\n", rc);
            regulator_disable(data->vdd);
        }

    }

    return rc;

}

static int ft5x06_power_init(struct ft5x06_ts_data *data, bool on)
{
    int rc;

    if (!on) {
        if (regulator_count_voltages(data->vdd) > 0) {
            regulator_set_voltage(data->vdd, 0, FT_VTG_MAX_UV);
        }
        regulator_put(data->vdd);

        if (regulator_count_voltages(data->vcc_i2c) > 0) {
            regulator_set_voltage(data->vcc_i2c, 0, FT_I2C_VTG_MAX_UV);
        }
        regulator_put(data->vcc_i2c);

    } else {
        data->vdd = regulator_get(&data->client->dev, "vdd");
        if (IS_ERR(data->vdd)) {
            rc = PTR_ERR(data->vdd);
            dev_err(&data->client->dev,
                "Regulator get failed vdd rc=%d\n", rc);
            return rc;
        }

        if (regulator_count_voltages(data->vdd) > 0) {
            rc = regulator_set_voltage(data->vdd, FT_VTG_MIN_UV,
                           FT_VTG_MAX_UV);
            if (rc) {
                dev_err(&data->client->dev,
                    "Regulator set_vtg failed vdd rc=%d\n", rc);
                goto reg_vdd_put;
            }
        }

        data->vcc_i2c = regulator_get(&data->client->dev, "vcc_i2c");
        if (IS_ERR(data->vcc_i2c)) {
            rc = PTR_ERR(data->vcc_i2c);
            dev_err(&data->client->dev,
                "Regulator get failed vcc_i2c rc=%d\n", rc);
            goto reg_vdd_set_vtg;
        }

        if (regulator_count_voltages(data->vcc_i2c) > 0) {
            rc = regulator_set_voltage(data->vcc_i2c, FT_I2C_VTG_MIN_UV,
                           FT_I2C_VTG_MAX_UV);
            if (rc) {
                dev_err(&data->client->dev,
                "Regulator set_vtg failed vcc_i2c rc=%d\n", rc);
                goto reg_vcc_i2c_put;
            }
        }
    }

    return 0;

reg_vcc_i2c_put:
    regulator_put(data->vcc_i2c);
reg_vdd_set_vtg:
    if (regulator_count_voltages(data->vdd) > 0)
        regulator_set_voltage(data->vdd, 0, FT_VTG_MAX_UV);
reg_vdd_put:
    regulator_put(data->vdd);
    return rc;

}

#ifdef CONFIG_PM
static int ft5x06_ts_suspend(struct device *dev)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    char txbuf[2], i;
    int err;

    if (data->loading_fw) {
        dev_info(dev, "Firmware loading in process...\n");
        return 0;
    }

    if (data->suspended) {
        dev_info(dev, "Already in suspend state\n");
        return 0;
    }

#ifdef CONFIG_FT5X06_GESTURE
    if (data->gs_state != FT5X06_GS_DISABLE) {
        ft5x0x_write_reg(data->client, 0xD0, 0x01);
        if (data->chip_id == FT5X46_ID)
        {
            ft5x0x_write_reg(data->client, 0xd1, 0xff);
            ft5x0x_write_reg(data->client, 0xd2, 0xff);
            ft5x0x_write_reg(data->client, 0xd5, 0xff);
            ft5x0x_write_reg(data->client, 0xd6, 0xff);
            ft5x0x_write_reg(data->client, 0xd7, 0xff);
            ft5x0x_write_reg(data->client, 0xd8, 0xff);
        }

        data->suspended = true;
        return 0;
    }
#endif

    data->irq_need_enable = 0;
    ft_irq_disable(data);

    cancel_work_sync(&data->pen_event_work);

    /* release all touches */
    for (i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
        input_mt_slot(data->input_dev, i);
        input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, 0);
    }
    input_report_key(data->input_dev, BTN_TOUCH, 0);
    input_sync(data->input_dev);

    if (gpio_is_valid(data->pdata->reset_gpio)) {
        txbuf[0] = FT_REG_PMODE;
        txbuf[1] = FT_PMODE_HIBERNATE;
        ft5x06_i2c_write(data->client, txbuf, sizeof(txbuf));
    }

    msleep(FT_LPM_DLY);       //more time to release the irq before lpm

    if (data->pdata->power_on) {
        err = data->pdata->power_on(false);
        if (err) {
            dev_err(dev, "power off failed");
            goto pwr_off_fail;
        }
    } else {
        err = ft5x06_power_on(data, false);
        if (err) {
            dev_err(dev, "power off failed");
            goto pwr_off_fail;
        }
    }

    data->suspended = true;

    return 0;

pwr_off_fail:
    if (gpio_is_valid(data->pdata->reset_gpio)) {
        gpio_direction_output(data->pdata->reset_gpio, 0);
        msleep(data->pdata->hard_rst_dly);
        gpio_direction_output(data->pdata->reset_gpio, 1);
    }

    ft_irq_enable(data);
    return err;
}

static int ft5x06_ts_resume(struct device *dev)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    int err;
#ifdef CONFIG_TS_GLOVE_SWITCH
    u8 reg_val;
#endif

    if (!data->suspended) {
        dev_dbg(dev, "Already in awake state\n");
        return 0;
    }

#ifdef CONFIG_FT5X06_GESTURE
    if (data->gs_state != FT5X06_GS_DISABLE) {
        ft5x0x_write_reg(data->client,0xD0,0x00);
        data->suspended = false;
        return 0;
    }
#endif

    if (data->pdata->power_on) {
        err = data->pdata->power_on(true);
        if (err) {
            dev_err(dev, "power on failed");
            return err;
        }
    } else {
        err = ft5x06_power_on(data, true);
        if (err) {
            dev_err(dev, "power on failed");
            return err;
        }
    }

    if (gpio_is_valid(data->pdata->reset_gpio)) {
        gpio_direction_output(data->pdata->reset_gpio, 0);
        msleep(data->pdata->hard_rst_dly);
        gpio_direction_output(data->pdata->reset_gpio, 1);
    }

    msleep(data->pdata->soft_rst_dly);

#ifdef CONFIG_TS_GLOVE_SWITCH
    ft5x0x_read_reg(data->client, 0xc0, &reg_val);
    if (reg_val != data->glove_state) {
        ft5x0x_write_reg(data->client, 0xc0, data->glove_state);
    }
#endif
    data->irq_need_enable = 1;
    ft_irq_enable(data);

    data->suspended = false;

    return 0;
}

#if defined(CONFIG_FB)
static int fb_notifier_callback(struct notifier_block *self,
                 unsigned long event, void *data)
{
    struct fb_event *evdata = data;
    int *blank;
    struct ft5x06_ts_data *ft5x06_data =
        container_of(self, struct ft5x06_ts_data, fb_notif);

    if (evdata && evdata->data && event == FB_EVENT_BLANK &&
            ft5x06_data && ft5x06_data->client) {
        blank = evdata->data;
        if (*blank == FB_BLANK_UNBLANK)
            ft5x06_ts_resume(&ft5x06_data->client->dev);
        else if (*blank == FB_BLANK_POWERDOWN)
            ft5x06_ts_suspend(&ft5x06_data->client->dev);
    }

    return 0;
}
#elif defined(CONFIG_HAS_EARLYSUSPEND)
static void ft5x06_ts_early_suspend(struct early_suspend *handler)
{
    struct ft5x06_ts_data *data = container_of(handler,
                           struct ft5x06_ts_data,
                           early_suspend);

    ft5x06_ts_suspend(&data->client->dev);
}

static void ft5x06_ts_late_resume(struct early_suspend *handler)
{
    struct ft5x06_ts_data *data = container_of(handler,
                           struct ft5x06_ts_data,
                           early_suspend);

    ft5x06_ts_resume(&data->client->dev);
}
#endif

#ifdef CONFIG_FT5X06_GESTURE
static int ft5x06_gesture_suspend_prepare(struct device *dev)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    if (data->gs_state != FT5X06_GS_DISABLE) {
        if (device_may_wakeup(dev)) {
            enable_irq_wake(data->client->irq);
        }
        data->gs_state = FT5X06_GS_SUSPEND;
    }
    return 0;
}

static void ft5x06_gesture_resume_complete(struct device *dev)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    if (data->gs_state != FT5X06_GS_DISABLE) {
        if (data->gs_state == FT5X06_GS_IRQPENDING) {
            queue_work(data->ts_workqueue, &data->pen_event_work);
        }
        if (device_may_wakeup(dev)) {
            disable_irq_wake(data->client->irq);
        }
        data->gs_state = FT5X06_GS_ACTIVE;
    }
}
#endif

static const struct dev_pm_ops ft5x06_ts_pm_ops = {
#if (!defined(CONFIG_FB) && !defined(CONFIG_HAS_EARLYSUSPEND))
    .suspend = ft5x06_ts_suspend,
    .resume = ft5x06_ts_resume,
#endif
#ifdef CONFIG_FT5X06_GESTURE
    .prepare = ft5x06_gesture_suspend_prepare,
    .complete = ft5x06_gesture_resume_complete,
#endif
};
#endif

#ifdef CONFIG_TS_GLOVE_SWITCH
static int ts_glove_switch(struct ft5x06_ts_data *data, int glove_state)
{
   if (!data->suspended) {
       mutex_lock(&data->glove_lock);
       if (glove_state != data->glove_state) {
           if (glove_state) {
               ft5x0x_write_reg(data->client, 0xc0, 0x01);
               data->glove_state = 1;
           } else {
               ft5x0x_write_reg(data->client, 0xc0, 0x0);
               data->glove_state = 0;
           }
       }
       mutex_unlock(&data->glove_lock);
   }

   return 0;
}

static ssize_t glove_switch_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    return sprintf(buf, "%s\n", data->glove_state ? "enable" : "disable");
}

static ssize_t glove_switch_store(struct device *dev, struct device_attribute *attr,
         const char *buf, size_t count)
{
    int glove_state;
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    sscanf(buf, "%d", &glove_state);
    if (glove_state < 0) {
        dev_err(&data->client->dev, "switch to glove_state : %d error\n", glove_state);
        return -EINVAL;
    }
    ts_glove_switch(data, !!glove_state);

    return count;
}
#endif

#ifdef CONFIG_FT5X06_GESTURE
static ssize_t gesture_switch_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    return sprintf(buf, "%s\n", (data->gs_state != FT5X06_GS_DISABLE) ?
                   "enable" : "disable");
}

static ssize_t gesture_switch_store(struct device *dev, struct device_attribute *attr,
         const char *buf, size_t count)
{
    int gesture_state;
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    if (data->suspended == false) {
        sscanf(buf, "%d", &gesture_state);
        if (gesture_state < 0) {
            dev_err(&data->client->dev, "gesture_state switch to : %d error\n", gesture_state);
            return -EINVAL;
        }

        if(!!gesture_state) {
            data->gs_state = FT5X06_GS_ACTIVE;
        } else {
            data->gs_state = FT5X06_GS_DISABLE;
        }
    }

    return count;
}
#endif

static int ft5x06_auto_cal(struct i2c_client *client)
{
    struct ft5x06_ts_data *data = i2c_get_clientdata(client);
    u8 temp = 0, i;

    /* set to factory mode */
    msleep(2 * data->pdata->soft_rst_dly);
    ft5x0x_write_reg(client, FT_REG_DEV_MODE, FT_FACTORYMODE_VALUE);
    msleep(data->pdata->soft_rst_dly);

    /* start calibration */
    ft5x0x_write_reg(client, FT_DEV_MODE_REG_CAL, FT_CAL_START);
    msleep(2 * data->pdata->soft_rst_dly);
    for (i = 0; i < FT_CAL_RETRY; i++) {
        ft5x0x_read_reg(client, FT_REG_CAL, &temp);
        /*return to normal mode, calibration finish */
        if (((temp & FT_CAL_MASK) >> FT_4BIT_SHIFT) == FT_CAL_FIN)
            break;
    }

    /*calibration OK */
    msleep(2 * data->pdata->soft_rst_dly);
    ft5x0x_write_reg(client, FT_REG_DEV_MODE, FT_FACTORYMODE_VALUE);
    msleep(data->pdata->soft_rst_dly);

    /* store calibration data */
    ft5x0x_write_reg(client, FT_DEV_MODE_REG_CAL, FT_CAL_STORE);
    msleep(2 * data->pdata->soft_rst_dly);

    /* set to normal mode */
    ft5x0x_write_reg(client, FT_REG_DEV_MODE, FT_WORKMODE_VALUE);
    msleep(2 * data->pdata->soft_rst_dly);

    return 0;
}

static int ft5x06_fw_upgrade_start(struct i2c_client *client,
            const u8 *data, u32 data_len)
{
    struct ft5x06_ts_data *ts_data = i2c_get_clientdata(client);
    struct fw_upgrade_info info = ts_data->pdata->info;
    u8 reset_reg;
    u8 w_buf[FT_MAX_WR_BUF] = {0}, r_buf[FT_MAX_RD_BUF] = {0};
    u8 pkt_buf[FT_FW_PKT_LEN + FT_FW_PKT_META_LEN];
    int i, j, temp;
    u32 pkt_num, pkt_len;
    u8 is_5336_new_bootloader = false;
    u8 is_5336_fwsize_30 = false;
    u8 fw_ecc;

    /* determine firmware size */
    if (*(data + data_len - FT_BLOADER_SIZE_OFF) == FT_BLOADER_NEW_SIZE)
        is_5336_fwsize_30 = true;
    else
        is_5336_fwsize_30 = false;

    for (i = 0, j = 0; i < FT_UPGRADE_LOOP; i++) {
        msleep(FT_EARSE_DLY_MS);
        /* reset - write 0xaa and 0x55 to reset register */
        if (ts_data->chip_id == FT6X06_ID)
            reset_reg = FT_RST_CMD_REG2;
        else
            reset_reg = FT_RST_CMD_REG1;

        ft5x0x_write_reg(client, reset_reg, FT_UPGRADE_AA);
        msleep(info.delay_aa);

        ft5x0x_write_reg(client, reset_reg, FT_UPGRADE_55);
        if (i <= (FT_UPGRADE_LOOP / 2))
            msleep(info.delay_55 + i * 3);
        else
            msleep(info.delay_55 - (i - (FT_UPGRADE_LOOP / 2)) * 2);

        /* Enter upgrade mode */
        w_buf[0] = FT_UPGRADE_55;
        ft5x06_i2c_write(client, w_buf, 1);
        usleep(FT_55_AA_DLY_NS);
        w_buf[0] = FT_UPGRADE_AA;
        ft5x06_i2c_write(client, w_buf, 1);

        /* check READ_ID */
        msleep(info.delay_readid);
        w_buf[0] = FT_READ_ID_REG;
        w_buf[1] = 0x00;
        w_buf[2] = 0x00;
        w_buf[3] = 0x00;

        ft5x06_i2c_read(client, w_buf, 4, r_buf, 2);

        if (r_buf[0] != info.upgrade_id_1
            || r_buf[1] != info.upgrade_id_2) {
            dev_err(&client->dev, "Upgrade ID mismatch(%d), IC=0x%x 0x%x, info=0x%x 0x%x\n",
                i, r_buf[0], r_buf[1],
                info.upgrade_id_1, info.upgrade_id_2);
        } else
            break;
    }

    if (i >= FT_UPGRADE_LOOP) {
        dev_err(&client->dev, "Abort upgrade\n");
        return -EIO;
    }

    w_buf[0] = 0xcd;
    ft5x06_i2c_read(client, w_buf, 1, r_buf, 1);

    if (r_buf[0] <= 4)
        is_5336_new_bootloader = FT_BLOADER_VERSION_LZ4;
    else if (r_buf[0] == 7)
        is_5336_new_bootloader = FT_BLOADER_VERSION_Z7;
    else if (r_buf[0] >= 0x0f &&
        ((ts_data->family_id == FT_FT5336_FAMILY_ID_0x11) ||
        (ts_data->family_id == FT_FT5336_FAMILY_ID_0x12) ||
        (ts_data->family_id == FT_FT5336_FAMILY_ID_0x13) ||
        (ts_data->family_id == FT_FT5336_FAMILY_ID_0x14)))
        is_5336_new_bootloader = FT_BLOADER_VERSION_GZF;
    else
        is_5336_new_bootloader = FT_BLOADER_VERSION_LZ4;

    dev_dbg(&client->dev, "bootloader type=%d, r_buf=0x%x, family_id=0x%x\n",
        is_5336_new_bootloader, r_buf[0], ts_data->family_id);
    /* is_5336_new_bootloader = FT_BLOADER_VERSION_GZF; */

    /* erase app and panel paramenter area */
    w_buf[0] = FT_ERASE_APP_REG;
    ft5x06_i2c_write(client, w_buf, 1);
    msleep(info.delay_erase_flash);

    if (is_5336_fwsize_30) {
        w_buf[0] = FT_ERASE_PANEL_REG;
        ft5x06_i2c_write(client, w_buf, 1);
    }
    msleep(FT_EARSE_DLY_MS);

    /* program firmware */
    if (is_5336_new_bootloader == FT_BLOADER_VERSION_LZ4
        || is_5336_new_bootloader == FT_BLOADER_VERSION_Z7)
        data_len = data_len - FT_DATA_LEN_OFF_OLD_FW;
    else
        data_len = data_len - FT_DATA_LEN_OFF_NEW_FW;

    pkt_num = (data_len) / FT_FW_PKT_LEN;
    pkt_len = FT_FW_PKT_LEN;
    pkt_buf[0] = FT_FW_START_REG;
    pkt_buf[1] = 0x00;
    fw_ecc = 0;

    for (i = 0; i < pkt_num; i++) {
        temp = i * FT_FW_PKT_LEN;
        pkt_buf[2] = (u8) (temp >> FT_8BIT_SHIFT);
        pkt_buf[3] = (u8) temp;
        pkt_buf[4] = (u8) (pkt_len >> FT_8BIT_SHIFT);
        pkt_buf[5] = (u8) pkt_len;

        for (j = 0; j < FT_FW_PKT_LEN; j++) {
            pkt_buf[6 + j] = data[i * FT_FW_PKT_LEN + j];
            fw_ecc ^= pkt_buf[6 + j];
        }

        ft5x06_i2c_write(client, pkt_buf,
                FT_FW_PKT_LEN + FT_FW_PKT_META_LEN);
        msleep(FT_FW_PKT_DLY_MS);
    }

    /* send remaining bytes */
    if ((data_len) % FT_FW_PKT_LEN > 0) {
        temp = pkt_num * FT_FW_PKT_LEN;
        pkt_buf[2] = (u8) (temp >> FT_8BIT_SHIFT);
        pkt_buf[3] = (u8) temp;
        temp = (data_len) % FT_FW_PKT_LEN;
        pkt_buf[4] = (u8) (temp >> FT_8BIT_SHIFT);
        pkt_buf[5] = (u8) temp;

        for (i = 0; i < temp; i++) {
            pkt_buf[6 + i] = data[pkt_num * FT_FW_PKT_LEN + i];
            fw_ecc ^= pkt_buf[6 + i];
        }

        ft5x06_i2c_write(client, pkt_buf, temp + FT_FW_PKT_META_LEN);
        msleep(FT_FW_PKT_DLY_MS);
    }

    /* send the finishing packet */
    if (is_5336_new_bootloader == FT_BLOADER_VERSION_LZ4 ||
        is_5336_new_bootloader == FT_BLOADER_VERSION_Z7) {
        for (i = 0; i < FT_FINISHING_PKT_LEN_OLD_FW; i++) {
            if (is_5336_new_bootloader  == FT_BLOADER_VERSION_Z7)
                temp = FT_MAGIC_BLOADER_Z7 + i;
            else if (is_5336_new_bootloader ==
                        FT_BLOADER_VERSION_LZ4)
                temp = FT_MAGIC_BLOADER_LZ4 + i;
            pkt_buf[2] = (u8)(temp >> 8);
            pkt_buf[3] = (u8)temp;
            temp = 1;
            pkt_buf[4] = (u8)(temp >> 8);
            pkt_buf[5] = (u8)temp;
            pkt_buf[6] = data[data_len + i];
            fw_ecc ^= pkt_buf[6];

            ft5x06_i2c_write(client,
                pkt_buf, temp + FT_FW_PKT_META_LEN);
            msleep(FT_FW_PKT_DLY_MS);
        }
    } else if (is_5336_new_bootloader == FT_BLOADER_VERSION_GZF) {
        for (i = 0; i < FT_FINISHING_PKT_LEN_NEW_FW; i++) {
            if (is_5336_fwsize_30)
                temp = FT_MAGIC_BLOADER_GZF_30 + i;
            else
                temp = FT_MAGIC_BLOADER_GZF + i;
            pkt_buf[2] = (u8)(temp >> 8);
            pkt_buf[3] = (u8)temp;
            temp = 1;
            pkt_buf[4] = (u8)(temp >> 8);
            pkt_buf[5] = (u8)temp;
            pkt_buf[6] = data[data_len + i];
            fw_ecc ^= pkt_buf[6];

            ft5x06_i2c_write(client,
                pkt_buf, temp + FT_FW_PKT_META_LEN);
            msleep(FT_FW_PKT_DLY_MS);

        }
    }

    /* verify checksum */
    w_buf[0] = FT_REG_ECC;
    ft5x06_i2c_read(client, w_buf, 1, r_buf, 1);
    if (r_buf[0] != fw_ecc) {
        dev_err(&client->dev, "ECC error! dev_ecc=%02x fw_ecc=%02x\n",
                    r_buf[0], fw_ecc);
        return -EIO;
    }

    /* reset */
    w_buf[0] = FT_REG_RESET_FW;
    ft5x06_i2c_write(client, w_buf, 1);
    msleep(ts_data->pdata->soft_rst_dly);

    dev_info(&client->dev, "Firmware upgrade successful\n");

    return 0;
}

static int ft5x06_fw_upgrade(struct device *dev, bool force)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    const struct firmware *fw = NULL;
    int rc;
    u8 fw_file_maj, fw_file_min, fw_file_sub_min;
    bool fw_upgrade = false;

    if (data->suspended) {
        dev_info(dev, "Device is in suspend state: Exit FW upgrade\n");
        return -EBUSY;
    }

    rc = request_firmware(&fw, data->fw_name, dev);
    if (rc < 0) {
        dev_err(dev, "Request firmware failed - %s (%d)\n",
                        data->fw_name, rc);
        return rc;
    }

    if (fw->size < FT_FW_MIN_SIZE || fw->size > FT_FW_MAX_SIZE) {
        dev_err(dev, "Invalid firmware size (%d)\n", fw->size);
        rc = -EIO;
        goto rel_fw;
    }

    fw_file_maj = FT_FW_FILE_MAJ_VER(fw);
    fw_file_min = FT_FW_FILE_MIN_VER(fw);
    fw_file_sub_min = FT_FW_FILE_SUB_MIN_VER(fw);

    dev_info(dev, "Current firmware: %d.%d.%d", data->fw_ver[0],
                data->fw_ver[1], data->fw_ver[2]);
    dev_info(dev, "New firmware: %d.%d.%d", fw_file_maj,
                fw_file_min, fw_file_sub_min);

    if (force)
        fw_upgrade = true;
    else if (data->fw_ver[0] < fw_file_maj)
        fw_upgrade = true;

    if (!fw_upgrade) {
        dev_info(dev, "Exiting fw upgrade...\n");
        rc = -EFAULT;
        goto rel_fw;
    }

    /* start firmware upgrade */
    if (FT_FW_CHECK(fw)) {
        rc = ft5x06_fw_upgrade_start(data->client, fw->data, fw->size);
        if (rc < 0)
            dev_err(dev, "update failed (%d). try later...\n", rc);
        else if (data->pdata->info.auto_cal)
            ft5x06_auto_cal(data->client);
    } else {
        dev_err(dev, "FW format error\n");
        rc = -EIO;
    }

    ft5x06_update_fw_ver(data);

    FT_STORE_TS_INFO(data->ts_info, data->family_id, data->pdata->name,
            data->pdata->num_max_touches, data->pdata->group_id,
            data->pdata->fw_vkey_support ? "yes" : "no",
            data->pdata->fw_name, data->fw_ver[0],
            data->fw_ver[1], data->fw_ver[2]);
rel_fw:
    release_firmware(fw);
    return rc;
}

static ssize_t ft5x06_update_fw_show(struct device *dev,
                struct device_attribute *attr, char *buf)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    return snprintf(buf, 2, "%d\n", data->loading_fw);
}

static ssize_t ft5x06_update_fw_store(struct device *dev,
                struct device_attribute *attr,
                const char *buf, size_t size)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    unsigned long val;
    int rc;

    if (size > 2)
        return -EINVAL;

    rc = kstrtoul(buf, 10, &val);
    if (rc != 0)
        return rc;

    if (data->suspended) {
        dev_info(dev, "In suspend state, try again later...\n");
        return size;
    }

    mutex_lock(&data->input_dev->mutex);
    if (!data->loading_fw  && val) {
        data->loading_fw = true;
        ft5x06_fw_upgrade(dev, false);
        data->loading_fw = false;
    }
    mutex_unlock(&data->input_dev->mutex);

    return size;
}

static DEVICE_ATTR(update_fw, 0664, ft5x06_update_fw_show,
                ft5x06_update_fw_store);

static ssize_t ft5x06_force_update_fw_store(struct device *dev,
                struct device_attribute *attr,
                const char *buf, size_t size)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    unsigned long val;
    int rc;

    if (size > 2)
        return -EINVAL;

    rc = kstrtoul(buf, 10, &val);
    if (rc != 0)
        return rc;

    mutex_lock(&data->input_dev->mutex);
    if (!data->loading_fw  && val) {
        data->loading_fw = true;
        ft5x06_fw_upgrade(dev, true);
        data->loading_fw = false;
    }
    mutex_unlock(&data->input_dev->mutex);

    return size;
}

static DEVICE_ATTR(force_update_fw, 0664, ft5x06_update_fw_show,
                ft5x06_force_update_fw_store);

static ssize_t ft5x06_fw_name_show(struct device *dev,
                struct device_attribute *attr, char *buf)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    return snprintf(buf, FT_FW_NAME_MAX_LEN - 1, "%s\n", data->fw_name);
}

static ssize_t ft5x06_fw_name_store(struct device *dev,
                struct device_attribute *attr,
                const char *buf, size_t size)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    if (size > FT_FW_NAME_MAX_LEN - 1)
        return -EINVAL;

    strlcpy(data->fw_name, buf, size);
    if (data->fw_name[size-1] == '\n')
        data->fw_name[size-1] = 0;

    return size;
}

static DEVICE_ATTR(fw_name, 0664, ft5x06_fw_name_show, ft5x06_fw_name_store);

static ssize_t ft5x06_ic_info_show(struct device *dev,
                struct device_attribute *attr, char *buf)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);

    return sprintf(buf, "focaltech ic info\nchip_id:0x%x\nic_type:%s\n"
                "family_id:0x%x\nvendor_id:0x%x\ncurrent_fw_ver:0x%x\n",
                data->chip_id,
                get_ic_type_by_id(data->chip_id),
                data->family_id,
                data->vendor_id,
                data->current_fw_ver);
}

static DEVICE_ATTR(ic_info, 0444, ft5x06_ic_info_show, NULL);

static bool ft5x06_debug_addr_is_valid(int addr)
{
    if (addr < 0 || addr > 0xFF) {
        pr_err("FT reg address is invalid: 0x%x\n", addr);
        return false;
    }

    return true;
}

static int ft5x06_debug_data_set(void *_data, u64 val)
{
    struct ft5x06_ts_data *data = _data;

    mutex_lock(&data->input_dev->mutex);

    if (ft5x06_debug_addr_is_valid(data->addr))
        dev_info(&data->client->dev,
            "Writing into FT registers not supported\n");

    mutex_unlock(&data->input_dev->mutex);

    return 0;
}

static int ft5x06_debug_data_get(void *_data, u64 *val)
{
    struct ft5x06_ts_data *data = _data;
    int rc;
    u8 reg;

    mutex_lock(&data->input_dev->mutex);

    if (ft5x06_debug_addr_is_valid(data->addr)) {
        rc = ft5x0x_read_reg(data->client, data->addr, &reg);
        if (rc < 0)
            dev_err(&data->client->dev,
                "FT read register 0x%x failed (%d)\n",
                data->addr, rc);
        else
            *val = reg;
    }

    mutex_unlock(&data->input_dev->mutex);

    return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(debug_data_fops, ft5x06_debug_data_get,
            ft5x06_debug_data_set, "0x%02llX\n");

static int ft5x06_debug_addr_set(void *_data, u64 val)
{
    struct ft5x06_ts_data *data = _data;

    if (ft5x06_debug_addr_is_valid(val)) {
        mutex_lock(&data->input_dev->mutex);
        data->addr = val;
        mutex_unlock(&data->input_dev->mutex);
    }

    return 0;
}

static int ft5x06_debug_addr_get(void *_data, u64 *val)
{
    struct ft5x06_ts_data *data = _data;

    mutex_lock(&data->input_dev->mutex);

    if (ft5x06_debug_addr_is_valid(data->addr))
        *val = data->addr;

    mutex_unlock(&data->input_dev->mutex);

    return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(debug_addr_fops, ft5x06_debug_addr_get,
            ft5x06_debug_addr_set, "0x%02llX\n");

static int ft5x06_debug_suspend_set(void *_data, u64 val)
{
    struct ft5x06_ts_data *data = _data;

    mutex_lock(&data->input_dev->mutex);

    if (val)
        ft5x06_ts_suspend(&data->client->dev);
    else
        ft5x06_ts_resume(&data->client->dev);

    mutex_unlock(&data->input_dev->mutex);

    return 0;
}

static int ft5x06_debug_suspend_get(void *_data, u64 *val)
{
    struct ft5x06_ts_data *data = _data;

    mutex_lock(&data->input_dev->mutex);
    *val = data->suspended;
    mutex_unlock(&data->input_dev->mutex);

    return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(debug_suspend_fops, ft5x06_debug_suspend_get,
            ft5x06_debug_suspend_set, "%lld\n");

static int ft5x06_debug_dump_info(struct seq_file *m, void *v)
{
    struct ft5x06_ts_data *data = m->private;

    seq_printf(m, "%s\n", data->ts_info);

    return 0;
}

static int debugfs_dump_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, ft5x06_debug_dump_info, inode->i_private);
}

static const struct file_operations debug_dump_info_fops = {
    .owner      = THIS_MODULE,
    .open       = debugfs_dump_info_open,
    .read       = seq_read,
    .release    = single_release,
};

static unsigned char ft5x0x_read_fw_ver(struct i2c_client *client)
{
    unsigned char ver;
    ft5x0x_read_reg(client, FT_REG_FW_VER, &ver);
    return(ver);
}

#ifdef CONFIG_OF
static int ft5x06_get_dt_coords(struct device *dev, char *name,
                struct ft5x06_ts_platform_data *pdata)
{
    u32 coords[FT_COORDS_ARR_SIZE];
    struct property *prop;
    struct device_node *np = dev->of_node;
    int coords_size, rc;

    prop = of_find_property(np, name, NULL);
    if (!prop)
        return -EINVAL;
    if (!prop->value)
        return -ENODATA;

    coords_size = prop->length / sizeof(u32);
    if (coords_size != FT_COORDS_ARR_SIZE) {
        dev_err(dev, "invalid %s\n", name);
        return -EINVAL;
    }

    rc = of_property_read_u32_array(np, name, coords, coords_size);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read %s\n", name);
        return rc;
    }

    if (!strcmp(name, "focaltech,panel-coords")) {
        pdata->panel_minx = coords[0];
        pdata->panel_miny = coords[1];
        pdata->panel_maxx = coords[2];
        pdata->panel_maxy = coords[3];
    } else if (!strcmp(name, "focaltech,display-coords")) {
        pdata->x_min = coords[0];
        pdata->y_min = coords[1];
        pdata->x_max = coords[2];
        pdata->y_max = coords[3];
    } else {
        dev_err(dev, "unsupported property %s\n", name);
        return -EINVAL;
    }

    return 0;
}

static int ft5x06_parse_dt(struct device *dev,
            struct ft5x06_ts_platform_data *pdata)
{
    int rc, i, j;
    struct device_node *np = dev->of_node;
    struct property *prop;
    char prop_name[PROP_NAME_SIZE] = {0};
    u32 temp_val, num_buttons, num_data;
    u32 *vendor_id, num_vendor_id, num_id = 0;

    struct id_map {
        u32 chip_id;
        u32 family_id;
    } *id_map;

    pdata->name = "focaltech";
    rc = of_property_read_string(np, "focaltech,name", &pdata->name);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read name\n");
        return rc;
    }

    rc = ft5x06_get_dt_coords(dev, "focaltech,panel-coords", pdata);
    if (rc && (rc != -EINVAL))
        return rc;

    rc = ft5x06_get_dt_coords(dev, "focaltech,display-coords", pdata);
    if (rc)
        return rc;

    pdata->i2c_pull_up = of_property_read_bool(np,
                        "focaltech,i2c-pull-up");

    pdata->no_force_update = of_property_read_bool(np,
                        "focaltech,no-force-update");
    /* reset, irq gpio info */
    pdata->reset_gpio = of_get_named_gpio_flags(np, "focaltech,reset-gpio",
                0, &pdata->reset_gpio_flags);
    if (pdata->reset_gpio < 0)
        return pdata->reset_gpio;

    pdata->irq_gpio = of_get_named_gpio_flags(np, "focaltech,irq-gpio",
                0, &pdata->irq_gpio_flags);
    if (pdata->irq_gpio < 0)
        return pdata->irq_gpio;

    pdata->fw_name = "ft_fw.bin";
    rc = of_property_read_string(np, "focaltech,fw-name", &pdata->fw_name);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw name\n");
        return rc;
    }

    rc = of_property_read_u32(np, "focaltech,group-id", &temp_val);
    if (!rc)
        pdata->group_id = temp_val;
    else
        return rc;

    rc = of_property_read_u32(np, "focaltech,hard-reset-delay-ms",
                            &temp_val);
    if (!rc)
        pdata->hard_rst_dly = temp_val;
    else
        return rc;

    rc = of_property_read_u32(np, "focaltech,soft-reset-delay-ms",
                            &temp_val);
    if (!rc)
        pdata->soft_rst_dly = temp_val;
    else
        return rc;

    rc = of_property_read_u32(np, "focaltech,num-max-touches", &temp_val);
    if (!rc)
        pdata->num_max_touches = temp_val;
    else
        return rc;

    rc = of_property_read_u32(np, "focaltech,fw-delay-aa-ms", &temp_val);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw delay aa\n");
        return rc;
    } else if (rc != -EINVAL)
        pdata->info.delay_aa =  temp_val;

    rc = of_property_read_u32(np, "focaltech,fw-delay-55-ms", &temp_val);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw delay 55\n");
        return rc;
    } else if (rc != -EINVAL)
        pdata->info.delay_55 =  temp_val;

    rc = of_property_read_u32(np, "focaltech,fw-upgrade-id1", &temp_val);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw upgrade id1\n");
        return rc;
    } else if (rc != -EINVAL)
        pdata->info.upgrade_id_1 =  temp_val;

    rc = of_property_read_u32(np, "focaltech,fw-upgrade-id2", &temp_val);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw upgrade id2\n");
        return rc;
    } else if (rc != -EINVAL)
        pdata->info.upgrade_id_2 =  temp_val;

    rc = of_property_read_u32(np, "focaltech,fw-delay-readid-ms",
                            &temp_val);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw delay read id\n");
        return rc;
    } else if (rc != -EINVAL)
        pdata->info.delay_readid =  temp_val;

    rc = of_property_read_u32(np, "focaltech,fw-delay-era-flsh-ms",
                            &temp_val);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read fw delay erase flash\n");
        return rc;
    } else if (rc != -EINVAL)
        pdata->info.delay_erase_flash =  temp_val;

    pdata->info.auto_cal = of_property_read_bool(np,
                    "focaltech,fw-auto-cal");

    pdata->fw_vkey_support = of_property_read_bool(np,
                        "focaltech,fw-vkey-support");

    pdata->ignore_id_check = of_property_read_bool(np,
                        "focaltech,ignore-id-check");

    pdata->x_negative = of_property_read_bool(np,
                        "focaltech,x_negative");

    pdata->y_negative = of_property_read_bool(np,
                        "focaltech,y_negative");

    prop = of_find_property(np, "focaltech,button-map", NULL);
    if (prop) {
        num_buttons = prop->length / sizeof(temp_val);
        if (num_buttons > MAX_BUTTONS)
            return -EINVAL;

        rc = of_property_read_u32_array(np,
            "focaltech,button-map", tsp_keycodes,
            num_buttons);
        if (rc) {
            dev_err(dev, "Unable to read key codes\n");
            return rc;
        }
    }

    prop = of_find_property(np, "focaltech,button-xcoords", NULL);
    if (prop) {
        num_buttons = prop->length / sizeof(temp_val);
        if (num_buttons > MAX_BUTTONS)
            return -EINVAL;

        rc = of_property_read_u32_array(np,
            "focaltech,button-xcoords", pdata->button_xcoords,
            num_buttons);
        if (rc) {
            dev_err(dev, "Unable to read button xcoords\n");
            return rc;
        }
    } else {
        pdata->button_xcoords[0] = 80;
        pdata->button_xcoords[1] = 120;
        pdata->button_xcoords[2] = 260;
    }

    rc = of_property_read_u32(np, "focaltech,button-ycoord", &pdata->button_ycoord);
    if (rc && (rc != -EINVAL)) {
        dev_err(dev, "Unable to read button-ycoord\n");
        return rc;
    } else {
        pdata->button_ycoord = 1350;
    }

#ifdef CONFIG_FT5X06_GESTURE
    {
        int key_map_len = 0;

        prop = of_find_property(np, "gesture,key-code-map", &key_map_len);

        if (!prop || !prop->value) {
            dev_err(dev, "key-code-map not specified\n");
            return  -EINVAL;
        }

        if (key_map_len != FTS_GESTURE_MAX_KEYS) {
            dev_warn(dev, "key-code-map should be a %dx%d matrix\n",
                    FTS_GESTURE_MAX_COLS, FTS_GESTURE_MAX_ROWS);
        }

        if (key_map_len > FTS_GESTURE_MAX_KEYS)
            key_map_len = FTS_GESTURE_MAX_KEYS;

        memcpy(&pdata->keymap, prop->value, key_map_len);

        rc = of_property_read_u32(np, "gesture,time-slot",
                &temp_val);
        if (!rc)
            pdata->time_slot = temp_val;
        else
            pdata->time_slot = 60;
    }
#endif

    prop = of_find_property(np, "focaltech,id-map", NULL);
    if (prop) {
        num_data = prop->length / sizeof(temp_val);
        num_id = num_data/2;
        if (num_data % 2 || num_id == 0) {
            dev_err(dev, "Found the id-map table in wrong format or not exist.\n");
            return -EINVAL;
        }

        pdata->num_chip_id = num_id;

        id_map = devm_kzalloc(dev, num_id*sizeof(struct id_map),
                GFP_KERNEL);

        rc = of_property_read_u32_array(np,
            "focaltech,id-map", (u32 *)id_map,
            num_data);
        if (rc) {
            dev_err(dev, "Unable to read id-map.\n");
            return rc;
        }

        pdata->id_info = devm_kzalloc(dev, num_id*sizeof(struct ft_id_info),
                GFP_KERNEL);
        memset(pdata->id_info, 0, num_id*sizeof(struct ft_id_info));

        for (i = 0; i < num_id; i++) {
            pdata->id_info[i].chip_id = id_map[i].chip_id;
            pdata->id_info[i].family_id = id_map[i].family_id;
            dev_info(dev, "found ic chip_id = %d, family_id = 0x%x.\n",
                    pdata->id_info[i].chip_id, pdata->id_info[i].family_id);
        }

        pdata->fw_ifile_data = devm_kzalloc(dev, num_id*sizeof(&temp_val),
                GFP_KERNEL);

        pdata->fw_ifile_data_len = devm_kzalloc(dev, num_id*sizeof(&temp_val),
                GFP_KERNEL);

        devm_kfree(dev, id_map);
    }

    for (i = 0; i < num_id; i++) {
        snprintf(prop_name, sizeof(prop_name), "focaltech,chip%d-vendor-id",pdata->id_info[i].chip_id);
        prop = of_find_property(np, prop_name, NULL);
        if (prop) {
            num_vendor_id = prop->length / sizeof(temp_val);

            vendor_id = devm_kzalloc(dev, num_vendor_id*sizeof(temp_val),
                    GFP_KERNEL);

            rc = of_property_read_u32_array(np,
                prop_name, vendor_id,
                num_vendor_id);
            if (rc) {
                dev_err(dev, "Unable to get '%s'.\n", prop_name);
                return rc;
            }

            pdata->id_info[i].num_vendor_id = num_vendor_id;
            pdata->id_info[i].vendor_id = vendor_id;
            for (j = 0;j <pdata->id_info[i].num_vendor_id;j++)
            dev_info(dev, "found chip_id = %d, vendor_id = 0x%x.\n",
                    pdata->id_info[i].chip_id, pdata->id_info[i].vendor_id[j]);
            pdata->fw_ifile_data[i] = devm_kzalloc(dev, num_vendor_id*sizeof(&temp_val),
                    GFP_KERNEL);
            pdata->fw_ifile_data_len[i] = devm_kzalloc(dev, num_vendor_id*sizeof(temp_val),
                    GFP_KERNEL);
        }
    }

    for (i = 0; i < num_id; i++) {
        for (j = 0; j < pdata->id_info[i].num_vendor_id; j++) {
            temp_val = 0;
            snprintf(prop_name, sizeof(prop_name), "focaltech,fw_i_file_data%d-%d",pdata->id_info[i].chip_id, j);
            prop = of_find_property(np, prop_name, &temp_val);
            if (prop && temp_val) {

                pdata->fw_ifile_data[i][j] = devm_kzalloc(dev, temp_val,
                        GFP_KERNEL);
                memcpy(pdata->fw_ifile_data[i][j], prop->value,
                        temp_val);
                pdata->fw_ifile_data_len[i][j] = temp_val;
                dev_info(dev, "Get '%s', length : %d.\n", prop_name, temp_val);
            } else {
                pdata->fw_ifile_data[i][j] = NULL;
                pdata->fw_ifile_data_len[i][j] = 0;
            }
        }
    }

    return 0;
}
#else
static int ft5x06_parse_dt(struct device *dev,
            struct ft5x06_ts_platform_data *pdata)
{
    return -ENODEV;
}
#endif

static int ft5x06_ts_probe(struct i2c_client *client,
               const struct i2c_device_id *id)
{
    struct ft5x06_ts_platform_data *pdata;
    struct ft5x06_ts_data *data;
    struct input_dev *input_dev;
    struct dentry *temp;
    u8 reg_value;
    u8 reg_addr;
    int err, len, i, j;
    u8 reg_fw;

#ifdef CONFIG_TOUCHSCREEN_FT5X06_CTP_UPG
    unsigned char update_ver;
#endif

    if (client->dev.of_node) {
        pdata = devm_kzalloc(&client->dev,
                sizeof(struct ft5x06_ts_platform_data), GFP_KERNEL);
        if (!pdata) {
            dev_err(&client->dev, "Failed to allocate memory\n");
            return -ENOMEM;
        }

        memset(pdata, 0, sizeof(struct ft5x06_ts_platform_data));

        err = ft5x06_parse_dt(&client->dev, pdata);
        if (err) {
            dev_err(&client->dev, "DT parsing failed\n");
            goto free_pdata;
        }
    } else {
        pdata = client->dev.platform_data;
    }

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        err =  -ENODEV;
        dev_err(&client->dev, "I2C not supported\n");
        goto free_pdata_id_info;
    }

    data = devm_kzalloc(&client->dev,
            sizeof(struct ft5x06_ts_data), GFP_KERNEL);
    if (!data) {
        err = -ENOMEM;
        dev_err(&client->dev, "Not enough memory\n");
        goto free_pdata_id_info;
    }

    if (pdata->fw_name) {
        len = strlen(pdata->fw_name);
        if (len > FT_FW_NAME_MAX_LEN - 1) {
            err = -EINVAL;
            dev_err(&client->dev, "Invalid firmware name\n");
            goto err_invaild_fw;
        }

        strlcpy(data->fw_name, pdata->fw_name, len + 1);
    }

    data->tch_data_len = FT_TCH_LEN(pdata->num_max_touches);
    data->tch_data = devm_kzalloc(&client->dev,
                data->tch_data_len, GFP_KERNEL);
    if (!data->tch_data) {
        err = -ENOMEM;
        dev_err(&client->dev, "Not enough memory\n");
        goto err_tch_data;
    }

    input_dev = input_allocate_device();
    if (!input_dev) {
        err = -ENOMEM;
        dev_err(&client->dev, "failed to allocate input device\n");
        goto free_mem;
    }

    data->irq_disabled = 0;
    data->irq_need_enable = 1;
    data->input_dev = input_dev;
    data->client = client;
    data->pdata = pdata;

    input_dev->name = "ft5x06_ts";
    input_dev->id.bustype = BUS_I2C;
    input_dev->dev.parent = &client->dev;

    INIT_WORK(&data->pen_event_work, ft5x0x_ts_pen_irq_work);

    data->ts_workqueue = create_singlethread_workqueue("ft5x06_ts");
    if (!data->ts_workqueue) {
        err = -ESRCH;
        dev_err(&client->dev, "%s: fail to create wq\n", __func__);
        goto exit_create_singlethread;
    }

    input_set_drvdata(input_dev, data);
    i2c_set_clientdata(client, data);

#ifdef CONFIG_FT5X06_GESTURE
    init_para(pdata->x_max, pdata->y_max, pdata->time_slot, 0, 0);

    for (i=0; i<FTS_GESTURE_MAX_KEYS; i++) {
        if (pdata->keymap[i] != 0) {
            input_set_capability(input_dev, EV_KEY, pdata->keymap[i]);
        }
    }
#endif

    __set_bit(EV_KEY, input_dev->evbit);
    __set_bit(EV_ABS, input_dev->evbit);
    __set_bit(BTN_TOUCH, input_dev->keybit);
    __set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

    input_mt_init_slots(input_dev, pdata->num_max_touches, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_X, pdata->x_min,
                 pdata->x_max, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_Y, pdata->y_min,
                 pdata->y_max, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, FT_PRESS, 0, 0);

    err = input_register_device(input_dev);
    if (err) {
        dev_err(&client->dev, "Input device registration failed\n");
        goto free_inputdev;
    }

#ifdef CONFIG_TS_GLOVE_SWITCH
    data->glove_state = 0;
    err = device_create_file(&input_dev->dev, &dev_attr_glove);
    if (err) {
        dev_err(&client->dev, "device create glove file failed.\n");
        goto free_inputdev;
    }
    mutex_init(&data->glove_lock);
#endif

    if (pdata->power_init) {
        err = pdata->power_init(true);
        if (err) {
            dev_err(&client->dev, "power init failed");
            goto unreg_inputdev;
        }
    } else {
        err = ft5x06_power_init(data, true);
        if (err) {
            dev_err(&client->dev, "power init failed");
            goto unreg_inputdev;
        }
    }

    if (pdata->power_on) {
        err = pdata->power_on(true);
        if (err) {
            dev_err(&client->dev, "power on failed");
            goto pwr_deinit;
        }
    } else {
        err = ft5x06_power_on(data, true);
        if (err) {
            dev_err(&client->dev, "power on failed");
            goto pwr_deinit;
        }
    }

    if (gpio_is_valid(pdata->irq_gpio)) {
        err = gpio_request(pdata->irq_gpio, "ft5x06_irq_gpio");
        if (err) {
            dev_err(&client->dev, "irq gpio request failed");
            goto pwr_off;
        }
        err = gpio_direction_input(pdata->irq_gpio);
        if (err) {
            dev_err(&client->dev,
                "set_direction for irq gpio failed\n");
            goto free_irq_gpio;
        }
    }

    if (gpio_is_valid(pdata->reset_gpio)) {
        err = gpio_request(pdata->reset_gpio, "ft5x06_reset_gpio");
        if (err) {
            dev_err(&client->dev, "reset gpio request failed");
            goto free_irq_gpio;
        }

        err = gpio_direction_output(pdata->reset_gpio, 0);
        if (err) {
            dev_err(&client->dev,
                "set_direction for reset gpio failed\n");
            goto free_reset_gpio;
        }
        msleep(data->pdata->hard_rst_dly);
        gpio_direction_output(data->pdata->reset_gpio, 1);
    }

    /* make sure CTP already finish startup process */
    msleep(data->pdata->soft_rst_dly);

    /* check the controller id */
    reg_addr = FT_CHIP_ID;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
    if (err < 0) {
        dev_err(&client->dev, "chip id read failed");
        goto free_reset_gpio;
    }

    dev_info(&client->dev, "Device ID = 0x%x\n", reg_value);
    data->family_id = reg_value;

    /* check the vendor id*/
    reg_addr = FT_VENDOR_ID;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
    if (err < 0) {
        dev_err(&client->dev, "vendor id read failed");
        goto free_reset_gpio;
    }

    data->vendor_id = reg_value;
    dev_info(&client->dev, "Vendor ID = 0x%x\n", reg_value);

    err = -1;
    data->chip_id = -1;
    for (i = 0; i < pdata->num_chip_id; i++) {
        if (pdata->id_info[i].family_id == data->family_id) {
            data->chip_id = pdata->id_info[i].chip_id;
            for(j = 0; j < pdata->id_info[i].num_vendor_id; j++) {
                if (pdata->id_info[i].vendor_id[j] == data->vendor_id) {
                    if (pdata->fw_ifile_data[i][j]) {
                        data->fw_ifile_data = pdata->fw_ifile_data[i][j];
                        data->fw_ifile_data_len = pdata->fw_ifile_data_len[i][j];
                        dev_info(&client->dev,
                                "choose fw data focaltech,fw_i_file_data%d-%d, len %d.\n",
                                data->chip_id, j, data->fw_ifile_data_len);
                        err = 0;
                        break;
                    } else {
                        break;
                    }
                }
            }
            break;
        }
    }

    if (err < 0) {
        dev_err(&client->dev,
                "%s:Can't found matching fw for ic(family_id 0x%x vendor_id 0x%x).\n",
                __func__, data->family_id, data->vendor_id);
        data->fw_ifile_data = NULL;
        data->fw_ifile_data_len = 0;
    }

    if (pdata->num_chip_id == 1 &&
            pdata->id_info[0].num_vendor_id == 1) {
        data->chip_id = pdata->id_info[0].chip_id;
        dev_info(&client->dev,
                "Config only one chip id 0x%x.\n",
                data->chip_id);
        data->fw_ifile_data = pdata->fw_ifile_data[0][0];
        data->fw_ifile_data_len = pdata->fw_ifile_data_len[0][0];
        if (pdata->fw_ifile_data[0][0] != NULL) {
            dev_info(&client->dev,
                    "There is only one fw_data, default use 'focaltech,fw_i_file_data%d-0'.\n",
                    data->chip_id);
        }
    }

#if FT_HAVE_KEY
    input_dev->keycode = tsp_keycodes;
    for(i = 0; i < CFG_NUMOFKEYS; i++)
    {
        input_set_capability(input_dev, EV_KEY, ((int*)input_dev->keycode)[i]);
        tsp_keystatus[i] = 0;
    }
#endif


    err = request_threaded_irq(client->irq, NULL,
                ft5x06_ts_interrupt,
                pdata->irqflags | IRQF_ONESHOT,
                client->dev.driver->name, data);
    if (err) {
        dev_err(&client->dev, "request irq failed\n");
        goto free_reset_gpio;
    }

    err = device_create_file(&client->dev, &dev_attr_fw_name);
    if (err) {
        dev_err(&client->dev, "sys file creation failed\n");
        goto irq_free;
    }

    err = device_create_file(&client->dev, &dev_attr_update_fw);
    if (err) {
        dev_err(&client->dev, "sys file creation failed\n");
        goto free_fw_name_sys;
    }

    err = device_create_file(&client->dev, &dev_attr_force_update_fw);
    if (err) {
        dev_err(&client->dev, "sys file creation failed\n");
        goto free_update_fw_sys;
    }

    err = device_create_file(&client->dev, &dev_attr_ic_info);
    if (err) {
        dev_err(&client->dev, "sys file creation failed\n");
        goto free_force_update_fw_sys;
    }

    data->dir = debugfs_create_dir(FT_DEBUG_DIR_NAME, NULL);
    if (data->dir == NULL || IS_ERR(data->dir)) {
        pr_err("debugfs_create_dir failed(%ld)\n", PTR_ERR(data->dir));
        err = PTR_ERR(data->dir);
        goto free_ic_info_sys;
    }

    temp = debugfs_create_file("addr", S_IRUSR | S_IWUSR, data->dir, data,
                   &debug_addr_fops);
    if (temp == NULL || IS_ERR(temp)) {
        pr_err("debugfs_create_file failed: rc=%ld\n", PTR_ERR(temp));
        err = PTR_ERR(temp);
        goto free_debug_dir;
    }

    temp = debugfs_create_file("data", S_IRUSR | S_IWUSR, data->dir, data,
                   &debug_data_fops);
    if (temp == NULL || IS_ERR(temp)) {
        pr_err("debugfs_create_file failed: rc=%ld\n", PTR_ERR(temp));
        err = PTR_ERR(temp);
        goto free_debug_dir;
    }

    temp = debugfs_create_file("suspend", S_IRUSR | S_IWUSR, data->dir,
                    data, &debug_suspend_fops);
    if (temp == NULL || IS_ERR(temp)) {
        pr_err("debugfs_create_file failed: rc=%ld\n", PTR_ERR(temp));
        err = PTR_ERR(temp);
        goto free_debug_dir;
    }

    temp = debugfs_create_file("dump_info", S_IRUSR | S_IWUSR, data->dir,
                    data, &debug_dump_info_fops);
    if (temp == NULL || IS_ERR(temp)) {
        pr_err("debugfs_create_file failed: rc=%ld\n", PTR_ERR(temp));
        err = PTR_ERR(temp);
        goto free_debug_dir;
    }

    data->ts_info = devm_kzalloc(&client->dev,
                FT_INFO_MAX_LEN, GFP_KERNEL);
    if (!data->ts_info) {
        err = -ENOMEM;
        dev_err(&client->dev, "Not enough memory\n");
        goto free_debug_dir;
    }

    /*get some register information */
    reg_addr = FT_REG_POINT_RATE;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
    if (err < 0) {
        dev_err(&client->dev, "report rate read failed");
    }

    dev_info(&client->dev, "report rate = %dHz\n", reg_value * 10);

    reg_addr = FT_REG_THGROUP;
    err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
    if (err < 0) {
        dev_err(&client->dev, "threshold read failed");
    }
    dev_info(&client->dev, "touch threshold = %d\n", reg_value * 4);

    reg_fw = ft5x0x_read_fw_ver(client);
    dev_info(&client->dev, "Firmware i_file version = 0x%x\n", reg_fw);
    data->current_fw_ver = reg_fw;

    ft5x06_update_fw_ver(data);

    FT_STORE_TS_INFO(data->ts_info, data->family_id, data->pdata->name,
            data->pdata->num_max_touches, data->pdata->group_id,
            data->pdata->fw_vkey_support ? "yes" : "no",
            data->pdata->fw_name, data->fw_ver[0],
            data->fw_ver[1], data->fw_ver[2]);

#if defined(CONFIG_FB)
    data->fb_notif.notifier_call = fb_notifier_callback;

    err = fb_register_client(&data->fb_notif);

    if (err) {
        dev_err(&client->dev, "Unable to register fb_notifier: %d\n",
            err);
    }
#elif defined(CONFIG_HAS_EARLYSUSPEND)
    data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN +
                            FT_SUSPEND_LEVEL;
    data->early_suspend.suspend = ft5x06_ts_early_suspend;
    data->early_suspend.resume = ft5x06_ts_late_resume;
    register_early_suspend(&data->early_suspend);
#endif

#ifdef CONFIG_FT5X06_GESTURE
    data->gs_state = FT5X06_GS_DISABLE;
    device_init_wakeup(&client->dev, 1);
    err = device_create_file(&input_dev->dev, &dev_attr_gesture);
    if (err) {
        dev_err(&client->dev, "device create gesture file failed.\n");
    }
#endif

    version = data->current_fw_ver;
    vendorid = data->vendor_id;

#ifdef CONFIG_TOUCHSCREEN_FT5X06_CTP_UPG
    msleep(200);
    //get some register information

    if (data->fw_ifile_data == NULL ||
            data->fw_ifile_data_len == 0) {
        printk("[FST] Firmware data is NULL, the upgrade exit.\n");
        return 0;
    }

    switch(data->chip_id) {
        case FT6X06_ID:
            update_ver = ft6x06_ctpm_get_i_file_ver(data);
            printk("[FST] Firmware version = 0x%x, new Firmware version = 0x%x\n", reg_fw, update_ver);
            if (update_ver <= reg_fw) {
                printk("[FST] Firmware is the latest version, do not upgrade!\n");
                return 0;
            }
            printk("[FST] Firmware Upgrade begin\n");
            ft_irq_disable(data);
            ft6x06_ctpm_fw_upgrade_with_i_file(client);
            break;
        case FT6X36_ID:
            update_ver = ft6x36_ctpm_get_i_file_ver(data);
            printk("[FST] Firmware version = 0x%x, new Firmware version = 0x%x\n", reg_fw, update_ver);
            if (update_ver <= reg_fw) {
                printk("[FST] Firmware is the latest version, do not upgrade!\n");
                return 0;
            }
            printk("[FST] Firmware Upgrade begin\n");
            ft_irq_disable(data);
            ft6x36_ctpm_fw_upgrade_with_i_file(client);
            break;
        case FT5X46_ID:
            update_ver = ft5x46_ctpm_get_i_file_ver(data);
            printk("[FST] Firmware version = 0x%x, new Firmware version = 0x%x\n", reg_fw, update_ver);
            if (update_ver <= reg_fw) {
                printk("[FST] Firmware is the latest version, do not upgrade!\n");
                return 0;
            }
            printk("[FST] Firmware Upgrade begin\n");
            ft_irq_disable(data);
            ft5x46_ctpm_fw_upgrade_with_i_file(client);
            break;
        case FT5436_ID:
        case FT5336_ID:
        case FT5406_ID:
            update_ver = ft5x36_ctpm_get_i_file_ver(data);
            printk("[FST] Firmware version = 0x%x, new Firmware version = 0x%x\n", reg_fw, update_ver);
            if (update_ver <= reg_fw) {
                printk("[FST] Firmware is the latest version, do not upgrade!\n");
                return 0;
            }
            printk("[FST] Firmware Upgrade begin\n");
            ft_irq_disable(data);
            ft5x36_ctpm_fw_upgrade_with_i_file(client);
            break;
        default:
            printk("[FST] Unknow device type!\n");
            return 0;
    }

    //wake the CTPM
    if (pdata->reset_gpio >= 0) {
        gpio_set_value(pdata->reset_gpio, 0);        //set wake = 0,base on system
        msleep(100);
        gpio_set_value(pdata->reset_gpio, 1);        //set wake = 1,base on system
        msleep(100);
    }
    msleep(50);
    printk("Firmware Upgrade finish\n");

    data->current_fw_ver = ft5x0x_read_fw_ver(client);
    version = data->current_fw_ver;
    ft_irq_enable(data);
#endif

    return 0;

free_debug_dir:
    debugfs_remove_recursive(data->dir);
free_ic_info_sys:
    device_remove_file(&client->dev, &dev_attr_ic_info);
free_force_update_fw_sys:
    device_remove_file(&client->dev, &dev_attr_force_update_fw);
free_update_fw_sys:
    device_remove_file(&client->dev, &dev_attr_update_fw);
free_fw_name_sys:
    device_remove_file(&client->dev, &dev_attr_fw_name);
irq_free:
    free_irq(client->irq, data);
free_reset_gpio:
    if (gpio_is_valid(pdata->reset_gpio))
        gpio_free(pdata->reset_gpio);
free_irq_gpio:
    if (gpio_is_valid(pdata->irq_gpio))
        gpio_free(pdata->irq_gpio);
pwr_off:
    if (pdata->power_on)
        pdata->power_on(false);
    else
        ft5x06_power_on(data, false);
pwr_deinit:
    if (pdata->power_init)
        pdata->power_init(false);
    else
        ft5x06_power_init(data, false);
unreg_inputdev:
#ifdef CONFIG_TS_GLOVE_SWITCH
    device_remove_file(&input_dev->dev, &dev_attr_glove);
    mutex_destroy(&data->glove_lock);
#endif
    input_unregister_device(input_dev);
    input_dev = NULL;
free_inputdev:
    input_free_device(input_dev);
    cancel_work_sync(&data->pen_event_work);
    if (data->ts_workqueue)
    {
        destroy_workqueue(data->ts_workqueue);
    }
exit_create_singlethread:
free_mem:
    devm_kfree(&client->dev, data->tch_data);
err_tch_data:
err_invaild_fw:
    devm_kfree(&client->dev, data);
free_pdata_id_info:
    for (i = 0; i < pdata->num_chip_id; i++) {
        if (pdata->id_info) {
            if (pdata->id_info[i].vendor_id) {
                devm_kfree(&client->dev, pdata->id_info[i].vendor_id);
            }

            for (j = 0; j < pdata->id_info[i].num_vendor_id; j++) {
                if (pdata->fw_ifile_data[i][j]) {
                    devm_kfree(&client->dev, pdata->fw_ifile_data[i][j]);
                }
            }
        }
    }
    if (pdata->id_info) {
        devm_kfree(&client->dev, pdata->id_info);
    }
free_pdata:
    devm_kfree(&client->dev, pdata);
    version = -1;
    vendorid = -1;

    return err;
}

static int ft5x06_ts_remove(struct i2c_client *client)
{
    struct ft5x06_ts_data *data = i2c_get_clientdata(client);
    struct ft5x06_ts_platform_data *pdata = data->pdata;
    int i, j;

#ifdef CONFIG_FT5X06_GESTURE
    device_init_wakeup(&client->dev, 0);
#endif

    debugfs_remove_recursive(data->dir);
    device_remove_file(&client->dev, &dev_attr_ic_info);
    device_remove_file(&client->dev, &dev_attr_force_update_fw);
    device_remove_file(&client->dev, &dev_attr_update_fw);
    device_remove_file(&client->dev, &dev_attr_fw_name);

#if defined(CONFIG_FB)
    if (fb_unregister_client(&data->fb_notif))
        dev_err(&client->dev, "Error occurred while unregistering fb_notifier.\n");
#elif defined(CONFIG_HAS_EARLYSUSPEND)
    unregister_early_suspend(&data->early_suspend);
#endif
    free_irq(client->irq, data);

    if (gpio_is_valid(data->pdata->reset_gpio))
        gpio_free(data->pdata->reset_gpio);

    if (gpio_is_valid(data->pdata->irq_gpio))
        gpio_free(data->pdata->irq_gpio);

    if (data->pdata->power_on)
        data->pdata->power_on(false);
    else
        ft5x06_power_on(data, false);

    if (data->pdata->power_init)
        data->pdata->power_init(false);
    else
        ft5x06_power_init(data, false);

#ifdef CONFIG_TS_GLOVE_SWITCH
    device_remove_file(&data->input_dev->dev, &dev_attr_glove);
    mutex_destroy(&data->glove_lock);
#endif

#ifdef CONFIG_FT5X06_GESTURE
    device_remove_file(&data->input_dev->dev, &dev_attr_gesture);
#endif

    input_unregister_device(data->input_dev);

    cancel_work_sync(&data->pen_event_work);
    if (data->ts_workqueue)
    {
        destroy_workqueue(data->ts_workqueue);
    }

    devm_kfree(&client->dev, data->tch_data);

    for (i = 0; i < pdata->num_chip_id; i++) {
        if (pdata->id_info) {
            if (pdata->id_info[i].vendor_id) {
                devm_kfree(&client->dev, pdata->id_info[i].vendor_id);
            }

            for (j = 0; j < pdata->id_info[i].num_vendor_id; j++) {
                if (pdata->fw_ifile_data[i][j]) {
                    devm_kfree(&client->dev, pdata->fw_ifile_data[i][j]);
                }
            }
        }
    }
    if (pdata->id_info) {
        devm_kfree(&client->dev, pdata->id_info);
    }
    devm_kfree(&client->dev, pdata);
    devm_kfree(&client->dev, data);

    return 0;
}

static const struct i2c_device_id ft5x06_ts_id[] = {
    {"ft5x06_ts", 0},
    {},
};

MODULE_DEVICE_TABLE(i2c, ft5x06_ts_id);

#ifdef CONFIG_OF
static struct of_device_id ft5x06_match_table[] = {
    { .compatible = "focaltech,5x06",},
    { },
};
#else
#define ft5x06_match_table NULL
#endif

static struct i2c_driver ft5x06_ts_driver = {
    .probe = ft5x06_ts_probe,
    .remove = ft5x06_ts_remove,
    .driver = {
           .name = "ft5x06_ts",
           .owner = THIS_MODULE,
           .of_match_table = ft5x06_match_table,
#ifdef CONFIG_PM
           .pm = &ft5x06_ts_pm_ops,
#endif
           },
    .id_table = ft5x06_ts_id,
};

static int __init ft5x06_ts_init(void)
{
    return i2c_add_driver(&ft5x06_ts_driver);
}
module_init(ft5x06_ts_init);

static void __exit ft5x06_ts_exit(void)
{
    i2c_del_driver(&ft5x06_ts_driver);
}
module_exit(ft5x06_ts_exit);

MODULE_DESCRIPTION("FocalTech ft5x06 TouchScreen driver");
MODULE_LICENSE("GPL v2");
