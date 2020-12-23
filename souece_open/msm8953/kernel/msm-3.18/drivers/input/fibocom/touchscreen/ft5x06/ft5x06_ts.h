/*
 *
 * FocalTech ft5x06 TouchScreen driver header file.
 *
 * Copyright (c) 2010  Focal tech Ltd.
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
#ifndef __LINUX_FT5X06_TS_H__
#define __LINUX_FT5X06_TS_H__

#ifdef CONFIG_FT5X06_GESTURE
#define FTS_GESTURE_MAX_ROWS 16
#define FTS_GESTURE_MAX_COLS 16
#define FTS_GESTURE_MAX_KEYS (FTS_GESTURE_MAX_ROWS*FTS_GESTURE_MAX_COLS)
#endif

#define MAX_BUTTONS     4

struct fw_upgrade_info {
    bool auto_cal;
    u16 delay_aa;       /*delay of write FT_UPGRADE_AA */
    u16 delay_55;       /*delay of write FT_UPGRADE_55 */
    u8 upgrade_id_1;    /*upgrade id 1 */
    u8 upgrade_id_2;    /*upgrade id 2 */
    u16 delay_readid;   /*delay of read id */
    u16 delay_erase_flash; /*delay of earse flash*/
};

struct ft_id_info {
    u32 chip_id;
    u32 *vendor_id;
    u32 num_vendor_id;
    u32 family_id;
};

struct ft5x06_ts_platform_data {
    struct fw_upgrade_info info;
    const char *name;
    const char *fw_name;
    u32 irqflags;
    u32 irq_gpio;
    u32 irq_gpio_flags;
    u32 reset_gpio;
    u32 reset_gpio_flags;
    u32 x_max;
    u32 y_max;
    u32 x_min;
    u32 y_min;
    u32 panel_minx;
    u32 panel_miny;
    u32 panel_maxx;
    u32 panel_maxy;
    u32 group_id;
    u32 hard_rst_dly;
    u32 soft_rst_dly;
    u32 num_max_touches;
    bool fw_vkey_support;
    bool no_force_update;
    bool i2c_pull_up;
    bool ignore_id_check;
    bool x_negative;
    bool y_negative;
    struct ft_id_info *id_info;
    u8 num_chip_id;
    u8 ***fw_ifile_data;
    int **fw_ifile_data_len;
    int (*power_init) (bool);
    int (*power_on) (bool);
#ifdef CONFIG_FT5X06_GESTURE
    int time_slot;
    u8 keymap[FTS_GESTURE_MAX_KEYS];
#endif
    u32 button_xcoords[MAX_BUTTONS];
    u32 button_ycoord;
};

#define MAX_DEVICE_ID      0xFF

#define FT5436_ID      0x00
#define FT5336_ID      0x01
#define FT5X46_ID      0x02
#define FT6X06_ID      0x03
#define FT6X36_ID      0x04
#define FT5406_ID      0x05

#ifdef CONFIG_TOUCHSCREEN_FT5X06_CTP_UPG

#define FT_UPGRADE_AA   0xAA
#define FT_UPGRADE_55   0x55

/***********************0705 mshl*/
#define    BL_VERSION_LZ4        0
#define    BL_VERSION_Z7        1
#define    BL_VERSION_GZF        2

/*upgrade config of FT6306*/
#define FT6306_UPGRADE_AA_DELAY         100
#define FT6306_UPGRADE_55_DELAY         50
#define FT6306_UPGRADE_ID_1             0x79
#define FT6306_UPGRADE_ID_2             0x08
#define FT6306_UPGRADE_READID_DELAY     10
#define FT6306_UPGRADE_EARSE_DELAY      2000

/*upgrade config of FT6436*/
#define FT6436_UPGRADE_AA_DELAY         10
#define FT6436_UPGRADE_55_DELAY         10
#define FT6436_UPGRADE_ID_1             0x79
#define FT6436_UPGRADE_ID_2             0x18
#define FT6436_UPGRADE_READID_DELAY     10
#define FT6436_UPGRADE_EARSE_DELAY      2000

/*upgrade config of FT5446*/
#define FT5446_UPGRADE_AA_DELAY         10
#define FT5446_UPGRADE_55_DELAY         10
#define FT5446_UPGRADE_ID_1             0x54
#define FT5446_UPGRADE_ID_2             0x2b
#define FT5446_UPGRADE_READID_DELAY     20
#define FT5446_UPGRADE_EARSE_DELAY      1500

/*upgrade config of FT5x36*/
#define FT5X36_UPGRADE_AA_DELAY         30
#define FT5X36_UPGRADE_55_DELAY         30
#define FT5X36_UPGRADE_ID_1             0x79
#define FT5X36_UPGRADE_ID_2             0x11
#define FT5X36_UPGRADE_READID_DELAY     10
#define FT5X36_UPGRADE_EARSE_DELAY      1500

/*upgrade config of FT5406*/
#define FT5406_UPGRADE_AA_DELAY         50
#define FT5406_UPGRADE_55_DELAY         30
#define FT5406_UPGRADE_ID_1             0x79
#define FT5406_UPGRADE_ID_2             0x3
#define FT5406_UPGRADE_READID_DELAY     10
#define FT5406_UPGRADE_EARSE_DELAY      1500

#define FTS_PACKET_LENGTH        128
#define FTS_UPGRADE_LOOP    10
#define FT_APP_INFO_ADDR     0xd7f8

#endif

#endif
