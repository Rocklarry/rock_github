#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h> // udelay()
#include <linux/device.h> // device_create()
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/version.h>      /* constant of kernel version */
#include <asm/uaccess.h> // get_user()
#include <linux/semaphore.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include "../../drivers/pinctrl/qcom/pinctrl-msm.h"

//#include <linux/fm.h>
#define FM_RX_BAND            1
//flag

#include "rdafm_drv.h"
//Version 2.1


//#define REGISTER_DEVICE 1


#define FM_ALERT(f, s...) \
    do { \
        printk(KERN_ALERT "RDAFM " f, ## s); \
    } while(0)

#ifdef FMDEBUG
#define FM_DEBUG(f, s...) \
    do { \
        printk("RDAFM " f, ## s); \
    } while(0)
#else
#define FM_DEBUG(f, s...)
#endif



typedef signed char         int8;
typedef unsigned char       uint8;
typedef signed short        int16;
typedef unsigned short      uint16;
typedef signed int            int32;
typedef unsigned int        uint32;
/******************************************************************************
 * CONSTANT DEFINITIONS
 *****************************************************************************/
#define RDAFM_SLAVE_ADDR        (0x11)    //RDA FM Chip address

#define RDAFM_MASK_RSSI         0X7F // RSSI
#define RDAFM_DEV_NAME          "RDAFM"

#define ID_RDA580XN             0x5808
#define ID_RDA5820              0x5805
#define ID_RDA5820NS            0x5820


//static struct proc_dir_entry *g_fm_proc = NULL; //add_by_frank
static struct fm *g_fm_struct = NULL;
static atomic_t scan_complete_flag;

#define FM_PROC_FILE "fm"


struct i2c_client *fm_client = NULL;

/******************************************************************************
 * STRUCTURE DEFINITIONS
 *****************************************************************************/





typedef struct
{
    uint8_t        address;
    uint16_t    value;
}RDA_FM_REG_T;   

typedef struct
{
    bool        byPowerUp;
    struct fm_tune_parm parm;
}FM_TUNE_T;

static FM_TUNE_T fm_tune_data = {false, {}};

typedef enum
{
    FM_RECEIVER,                //5800,5802,5804
    FM_TRANSMITTER,                //5820
}RDA_RADIO_WORK_E;


struct fm {
    uint32 ref;
    bool powerup;
    uint16_t chip_id;
    uint16_t device_id;
    dev_t dev_t;
    uint16_t min_freq; // KHz
    uint16_t max_freq; // KHz
    uint8_t band;   // TODO
    struct class *cls;
    struct device *dev;
    struct cdev cdev;
    struct i2c_client *i2c_client;
};




/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
/*/
static void fm_poweron_pa(int on)
{
}
*/

static int RDAFM_clear_hmute(struct i2c_client *client);
static int RDAFM_enable_hmute(struct i2c_client *client);
// static int RDAFM_clear_tune(struct i2c_client *client); //
//static int RDAFM_enable_tune(struct i2c_client *client); //
//static int RDAFM_clear_seek(struct i2c_client *client);  //
//static int RDAFM_enable_seek(struct i2c_client *client); //a
// static void fm_test_tune(int mode, int freq);// open when you want test fm  //add_by_frank
static int RDAFM_SetStereo(struct i2c_client *client,uint8_t b);
static int RDAFM_SetRSSI_Threshold(struct i2c_client *client,uint8_t RssiThreshold);
static int RDAFM_SetDe_Emphasis(struct i2c_client *client,uint8_t index);
static bool RDAFM_Scan(struct i2c_client *client,
        uint16_t min_freq, uint16_t max_freq,
        uint16_t *pFreq, //get the valid freq after scan
        uint16_t *pScanTBL,
        uint16_t *ScanTBLsize,
        uint16_t scandir,
        uint16_t space);
static int RDAFM_read(struct i2c_client *client, uint8_t addr, uint16_t *val);
static int RDAFM_write(struct i2c_client *client, uint8_t addr, uint16_t val);
static void RDAFM_em_test(struct i2c_client *client, uint16_t group_idx, uint16_t item_idx, uint32_t item_value);
//static int RDAFM_set_bits(struct i2c_client *client, uint8_t addr,
//                            uint16_t bits, uint16_t mask)

static int fm_setup_cdev(struct fm *fm);
static long fm_ops_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static loff_t fm_ops_lseek(struct file *filp, loff_t off, int whence);
static int fm_ops_open(struct inode *inode, struct file *filp);
static int fm_ops_release(struct inode *inode, struct file *filp);

static int fm_init(struct i2c_client *client);
static int fm_destroy(struct fm *fm); //
static int fm_powerup(struct fm *fm, struct fm_tune_parm *parm);
static int fm_powerdown(struct fm *fm);

static int fm_tune(struct fm *fm, struct fm_tune_parm *parm);
static int fm_seek(struct fm *fm, struct fm_seek_parm *parm);
static int fm_scan(struct fm *fm, struct fm_scan_parm *parm);
static int fm_setvol(struct fm *fm, uint32_t vol);
static int fm_getvol(struct fm *fm, uint32_t *vol);
static int fm_getrssi(struct fm *fm, uint32_t *rssi);
//static int fm_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data); //add_by_frank 


static int fm_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
//static int fm_i2c_detect(struct i2c_client *client, struct i2c_board_info *info); //add_by_frank
static int fm_i2c_remove(struct i2c_client *client);


/******************************************************************************
 * GLOBAL DATA
 *****************************************************************************/


static uint16 RDAFM_CHIP_ID = 0x0;
static RDA_RADIO_WORK_E RDA_RADIO_WorkType = FM_RECEIVER;




//RDA5807M RDA5807NN RDA5807NP
static uint16 RDA580XN_initialization_reg[]={
    0xC005, //02h 
    0x0000,
    0x0400,
    0xC6ED, //05h
    0x6000,
    0x721A,
    0x0000,
    0x0000,
    0x0000,  //0x0ah
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,  //0x10h
    0x0019,
    0x2A11,
    0xB042,  
    0x2A11,  
    0xB831,  //0x15h 
    0xC000,
    0x2A91,
    0x9400,
    0x00A8,
    0xC400,  //0x1ah
    0xF7CF,
    0x2414, //0x2ADC,  //0x1ch
    0x806F, 
    0x4608,
    0x0086,
    0x0661, //0x20H
    0x0000,
    0x109E,
    0x23C8,
    0x0406,
    0x0E1C, //0x25H
};



static RDA_FM_REG_T RDA5820_RX_initialization_reg[]={
    {0x02, 0xC001},  //Power Up 
    {0xFF, 1000},    // wait
    {0x03, 0x0000},    
    {0x04, 0x0400},
    {0x05, 0x864F}, 
    {0x07, 0x7800}, 
    {0x13, 0x0008}, 
    {0x15, 0x1430}, 
    {0x16, 0xC000}, 
    {0x1C, 0x3126}, 
    {0x22, 0x9C24}, 
    {0x25, 0x1B2A},
    {0x47, 0xF660}, 
};


static RDA_FM_REG_T RDA5820_TX_initialization_reg[]={
    {0x02, 0xC001},  //Power Up 
    {0xFF, 1000},    // wait
    {0x03, 0x0000},    
    {0x04, 0x0400},  
    {0x05, 0x864F},    
    {0x07, 0x7800},    
    {0x13, 0x0008},    
    {0x15, 0x1430},    
    {0x16, 0xC000},        
    {0x1C, 0x3126},    
    {0x22, 0x9C24},  
    {0x25, 0x1B2A},  
    {0x47, 0xF660}, 
    {0x40, 0x0001},
};



static RDA_FM_REG_T RDA5820NS_TX_initialization_reg[]={
    {0x02, 0xC001},  //Power Up 
    {0xFF, 1000},    // wait
    {0x03, 0x0000},    
    {0x04, 0x0400},
    {0x05, 0x864F},
    {0x14, 0x3A00},
    {0x15, 0x88FE},
    {0x16, 0x4C00},
    {0x1A, 0x0400},
    {0x1C, 0x23DC},
    {0x27, 0xBB6C},
    {0x5C, 0x175C},
    {0x68, 0x01F0},
    {0x6A, 0x2846},
    {0x40, 0x0001},
    {0x41, 0x41FF},
};


static RDA_FM_REG_T RDA5820NS_RX_initialization_reg[]={
    {0x02, 0xC001},  //Power Up 
    {0xFF, 1000},    // wait
    {0x03, 0x0000},    
    {0x04, 0x0400},
    {0x05, 0x88EF},
    {0x14, 0x2000},
    {0x15, 0x88FE},
    {0x16, 0x4C00},
    {0x1C, 0x221c},
    {0x25, 0x0E1C},
    {0x27, 0xBB6C},
    {0x5C, 0x175C},
};

static struct file_operations fm_ops = {
    .owner = THIS_MODULE,
    //.unlocked_ioctl = fm_ops_ioctl,
    .compat_ioctl = fm_ops_ioctl,
    .llseek = fm_ops_lseek,
    .open = fm_ops_open,
    .release = fm_ops_release,
};


DEFINE_SEMAPHORE(fm_ops_mutex);


/******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *****************************************************************************/



static int RDAFM_GetChipID(struct i2c_client *client, uint16_t *pChipID)
{
    int ret = -1;
    uint16 regVal = 0x0002;

    //Reset RDA FM
    ret = RDAFM_write(client, 0x02, regVal);
    if(ret < 0){
#ifdef FMDEBUG
        FM_DEBUG("RDAFM_GetChipID: reset FM chip failed!\n");
#endif
        return ret;
    }
    msleep(80);

    regVal = 0;
    ret = RDAFM_read(client, 0x0C, &regVal);
    if (ret == 0)
    {
        if ((0x5802 == regVal) || (0x5803 == regVal))
        {
            ret = RDAFM_read(client, 0x0E, &regVal);

            if (ret == 0)
                *pChipID = regVal;
            else
                *pChipID = 0x5802;

            FM_DEBUG("RDAFM_GetChipID: Chip ID = %04X\n", regVal);
        }
        else if ((0x5805 == regVal) || (0x5820 == regVal))
        {
            *pChipID = regVal;
        }
        else
        {
            FM_DEBUG("RDAFM_GetChipID: get chip ID failed! get value = %04X\n", regVal);
            ret = -1;
        }
    }
    else
    {
        FM_DEBUG("RDAFM_GetChipID: get chip ID failed!\n");
            ret = -1;
    }

    return ret;

}



/*
 *  RDAFM_read
 */
static int RDAFM_read(struct i2c_client *client, uint8_t addr, uint16_t *returnData)
{
    int ret = 0;
    char b[2] = {0};

    // first, send addr to RDAFM
    ret = i2c_master_send(client, (char*)&addr, 1);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_read send:0x%X err:%d\n", addr, ret);
        return -1;
    }

    // second, receive two byte from RDAFM
    ret = i2c_master_recv(client, b, 2);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_read recv:0x%X err:%d\n", addr, ret);
        return -1;
    }

    *returnData = ((uint16)b[0] << 8 | (uint16)b[1]);

	printk("=== %s add:0x%x \n",__func__,addr);
    return 0;
}


/*
 *  RDAFM_write
 */
static int RDAFM_write(struct i2c_client *client, uint8_t addr, uint16_t val)
{
    int ret = 0;
    char b[3];

    b[0] = addr;
    b[1] = (char)(val >> 8);
    b[2] = (char)(val & 0xFF);

	printk("=== %s add:0x%x val0x%x \n",__func__,addr,val);
    ret = i2c_master_send(client, b, 3);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_write send:0x%X err:%d\n", addr, ret);
        return -1;
    }

    return 0;
}


/*
static int RDAFM_set_bits(struct i2c_client *client, uint8_t addr,
                             uint16_t bits, uint16_t mask)
{
    int err;
    uint16_t val;

    err = RDAFM_read(client, addr, &val);
    if (err)
        return err;

    val = ((val & (~mask)) | bits);

    err = RDAFM_write(client, addr, val);
    if (err)
        return err;

    return 0;
}
*/




static int RDAFM_clear_hmute(struct i2c_client *client)
{
    int ret = 0;
    uint16_t tRegValue = 0;

    FM_DEBUG("RDAFM_clear_hmute\n");

    ret = RDAFM_read(client, 0x02, &tRegValue);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_clear_hmute  read register failed!\n"); 
        return -1;
    }

    tRegValue |= (1 << 14);

    ret = RDAFM_write(client, 0x02, tRegValue);

    if (ret < 0)
    {
        FM_ALERT("RDAFM_clear_hmute  write register failed!\n"); 
        return -1;
    }

    if(fm_tune_data.byPowerUp){
        if (fm_tune(g_fm_struct, &(fm_tune_data.parm)) < 0)
        {
            fm_tune_data.byPowerUp = false;
            memset(&fm_tune_data.parm, 0, sizeof(fm_tune_data.parm));
            return -EPERM;
        }
        fm_tune_data.byPowerUp = false;
        memset(&fm_tune_data.parm, 0, sizeof(fm_tune_data.parm));
    }

    return 0;

}



static int RDAFM_enable_hmute(struct i2c_client *client)
{
    int ret = 0;
    uint16_t tRegValue = 0;

    FM_DEBUG("RDAFM_enable_hmute\n");

    ret = RDAFM_read(client, 0x02, &tRegValue);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_enable_hmute  read register failed!\n"); 
        return -1;
    }

    tRegValue &= (~(1 << 14));

    ret = RDAFM_write(client, 0x02, tRegValue);

    if (ret < 0)
    {
        FM_ALERT("RDAFM_enable_hmute  write register failed!\n"); 
        return -1;
    }

    return 0;

}


/* 
static int RDAFM_clear_tune(struct i2c_client *client)
{
    //Don't need it
    return 0;
}



static int RDAFM_enable_tune(struct i2c_client *client)
{
    //Don't need it
    return 0;
}

static int RDAFM_clear_seek(struct i2c_client *client)
{
    //Don't need it
    return 0;
}



static int RDAFM_enable_seek(struct i2c_client *client)
{
    //Don't need it
    return 0;
}
//
*/

//b=true set stereo else set mono
static int RDAFM_SetStereo(struct i2c_client *client, uint8_t b)
{
    int ret = 0;
    uint16_t tRegValue = 0;

    FM_DEBUG("RDAFM_SetStereo\n");

    ret = RDAFM_read(client, 0x02, &tRegValue);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_SetStereo  read register failed!\n"); 
        return -1;
    }
    if (b)
        tRegValue &= (~(1 << 13));//set stereo
    else
        tRegValue |= (1 << 13); //set mono

    ret = RDAFM_write(client, 0x02, tRegValue);

    if (ret < 0)
    {
        FM_ALERT("RDAFM_SetStereo  write register failed!\n"); 
        return -1;
    }


    return 0;

}



static int RDAFM_SetRSSI_Threshold(struct i2c_client *client, uint8_t RssiThreshold)
{
    int ret = 0;
    uint16_t tRegValue = 0;

    FM_DEBUG("RDAFM_SetRSSI_Threshold\n");

    ret = RDAFM_read(client, 0x05, &tRegValue);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_SetRSSI_Threshold  read register failed!\n"); 
        return -1;
    }

    tRegValue &= 0x80FF;//clear valume
    tRegValue |= ((RssiThreshold & 0x7f) << 8); //set valume

    ret = RDAFM_write(client, 0x05, tRegValue);

    if (ret < 0)
    {
        FM_ALERT("RDAFM_SetRSSI_Threshold  write register failed!\n"); 
        return -1;
    }


    return 0;

}



static int RDAFM_SetDe_Emphasis(struct i2c_client *client, uint8_t index)
{
    int ret = 0;
    uint16_t tRegValue = 0;

    FM_DEBUG("RDAFM_SetRSSI_Threshold\n");

    ret = RDAFM_read(client, 0x04, &tRegValue);
    if (ret < 0)
    {
        FM_ALERT("RDAFM_SetRSSI_Threshold  read register failed!\n"); 
        return -1;
    }

    if (0 == index)
    {
        tRegValue &= (~(1 << 11));//De_Emphasis=75us
    }
    else if (1 == index)
    {
        tRegValue |= (1 << 11);//De_Emphasis=50us
    }


    ret = RDAFM_write(client, 0x04, tRegValue);

    if (ret < 0)
    {
        FM_ALERT("RDAFM_SetRSSI_Threshold  write register failed!\n"); 
        return -1;
    }


    return 0;


}






static void RDAFM_em_test(struct i2c_client *client, uint16_t group_idx, uint16_t item_idx, uint32_t item_value)
{
    FM_DEBUG("RDAFM_em_test  %d:%d:%d\n", group_idx, item_idx, item_value); 
    switch (group_idx)
    {
        case mono:
            if(item_value == 1)
            {
                RDAFM_SetStereo(client, 0); //force mono
            }
            else
            {
                RDAFM_SetStereo(client, 1); //stereo
            
            }

            break;
        case stereo:
            if(item_value == 0)
            {
                RDAFM_SetStereo(client, 1); //stereo
            }
            else
            {
                RDAFM_SetStereo(client, 0); //force mono    
            }
            break;
        case RSSI_threshold:
            item_value &= 0x7F;
            RDAFM_SetRSSI_Threshold(client, item_value);
            break;            
        case Softmute_Enable:
            if (item_idx)
            {
                RDAFM_enable_hmute(client);
            }
            else
            {
                RDAFM_clear_hmute(client);
            }
            break;
       case De_emphasis:
            if(item_idx >= 2) //0us
            {
                FM_ALERT("not support De_emphasis 0\n");        
            }
            else
            {
                RDAFM_SetDe_Emphasis(client,item_idx);//0=75us,1=50us
            }
            break;
            
       case HL_Side:

            break;
        default:
            FM_ALERT("not support this setting\n");
            break;   
    }
}

static bool RDAFM_Scan(struct i2c_client *client, 
        uint16_t min_freq, uint16_t max_freq,
        uint16_t *pFreq,
        uint16_t *pScanTBL, 
        uint16_t *ScanTBLsize, 
        uint16_t scandir, 
        uint16_t space)
{
    uint16_t tFreq, tRegValue = 0;
    uint16_t tmp_scanTBLsize = *ScanTBLsize;
    int ret = -1;
    bool isTrueStation = false;
    uint16_t oldValue = 0;
    int channel = 0;
    
    FM_DEBUG("%s: in\n", __func__);
    if((!pScanTBL) || (tmp_scanTBLsize == 0)) {
        FM_DEBUG("%s: pScanTBL is null or tmp_scanTBLsize == 0: tmp_scanTBLsize = %d\n", __func__, tmp_scanTBLsize);
        return ret;
    }

    //clear the old value of pScanTBL
    memset(pScanTBL, 0, sizeof(uint16_t)*FM_SCAN_TBL_SIZE);

    if(tmp_scanTBLsize > FM_SCAN_TBL_SIZE)
    {
        tmp_scanTBLsize = FM_SCAN_TBL_SIZE;
    }

    //scan up
    if(scandir == FM_SCAN_UP){ // now, only support scan up
        tFreq = min_freq;
    }else{ //scan down
        tFreq = max_freq;//max_freq compare need or not   
    }

    //mute FM
    RDAFM_enable_hmute(client);

    atomic_set(&scan_complete_flag, 1);
    do {
        if(atomic_read(&scan_complete_flag) == 0)
            break;
        isTrueStation = false;

        //set channel and enable TUNE
        tRegValue = 0;
        RDAFM_read(client, 0x03, &tRegValue);
        tRegValue &= (~(0x03ff<<6)) | (tRegValue & 0x0f); //clear bit[15:6]
        channel = tFreq - min_freq; 
        tRegValue |= ((channel << 6) | (1 << 4)); //set bit[15:6] and bit[4]
        ret = RDAFM_write(client, 0x03, tRegValue);
        msleep(40);

        //read 0x0B and check FM_TRUE(bit[8])
        tRegValue = 0;
        ret = RDAFM_read(client, 0x0B, &tRegValue);
        if(!ret){
            if((tRegValue & 0x0100) == 0x0100){
                isTrueStation = true;
            }
        }

        //if this freq is a true station, read the channel
        if(isTrueStation){
                //FM_DEBUG("%s: channel %d is true station\n", __func__, channel);
            if((channel >= 0) && (channel != 85)){
                oldValue = *(pScanTBL+(channel/16));
                oldValue |= (1<<(channel%16));
                *(pScanTBL+(channel/16)) = oldValue;
            }
        }
        //else
            //FM_DEBUG("%s: channel %d is false station\n", __func__, channel);

        //increase freq
        tFreq += space;
    }while( tFreq <= max_freq );

    *ScanTBLsize = tmp_scanTBLsize;
    *pFreq = 0;

    //clear FM mute
    RDAFM_clear_hmute(client);


    return 0;
}


static int fm_setup_cdev(struct fm *fm)
{
    int ret = 0;

    ret = alloc_chrdev_region(&fm->dev_t, 0, 1, FM_NAME);
    if (ret) {
        FM_ALERT("alloc dev_t failed\n");
        return -1;
    }

    FM_DEBUG("alloc %s:%d:%d\n", FM_NAME,
            MAJOR(fm->dev_t), MINOR(fm->dev_t));

    cdev_init(&fm->cdev, &fm_ops);

    fm->cdev.owner = THIS_MODULE;
    fm->cdev.ops = &fm_ops;

    ret = cdev_add(&fm->cdev, fm->dev_t, 1);
    if (ret) {
        FM_ALERT("alloc dev_t failed\n");
        return -1;
    }

    fm->cls = class_create(THIS_MODULE, FM_NAME);
    if (IS_ERR(fm->cls)) {
        ret = PTR_ERR(fm->cls);
        FM_ALERT("class_create err:%d\n", ret);
        return ret;            
    }    
    fm->dev = device_create(fm->cls, NULL, fm->dev_t, NULL, FM_NAME);

    return ret;
}



static long fm_ops_ioctl(struct file *filp,
        unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    struct fm *fm = container_of(filp->f_dentry->d_inode->i_cdev, struct fm, cdev);

    FM_DEBUG("%s cmd(%x)\n", __func__, cmd);

    switch(cmd)
    {
        case FM_IOCTL_POWERUP:
        {
            struct fm_tune_parm parm;
            FM_DEBUG("FM_IOCTL_POWERUP\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//                return -EPERM;

            if (copy_from_user(&parm, (void*)arg, sizeof(struct fm_tune_parm)))
                return -EFAULT;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
                RDA_RADIO_WorkType = FM_RECEIVER;
            ret = fm_powerup(fm, &parm);
            up(&fm_ops_mutex);
            if (copy_to_user((void*)arg, &parm, sizeof(struct fm_tune_parm)))
                return -EFAULT;
            //fm_poweron_pa(1);
            break;
        }

        case FM_IOCTL_POWERDOWN:
        {
            FM_DEBUG("FM_IOCTL_POWERDOWN\n");
// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//                return -EPERM;
            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            ret = fm_powerdown(fm);
            up(&fm_ops_mutex);
//            fm_poweron_pa(0);
            break;
        }

         // tune (frequency, auto Hi/Lo ON/OFF )
        case FM_IOCTL_TUNE:
        {
            struct fm_tune_parm parm;
            FM_DEBUG("FM_IOCTL_TUNE\n");
// FIXME!
//            if (!capable(CAP_SYS_ADMIN))
//                return -EPERM;

            if (copy_from_user(&parm, (void*)arg, sizeof(struct fm_tune_parm)))
                return -EFAULT;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            ret = fm_tune(fm, &parm);
            up(&fm_ops_mutex);

            if (copy_to_user((void*)arg, &parm, sizeof(struct fm_tune_parm)))
                return -EFAULT;

            break;
        }

        case FM_IOCTL_SEEK:
        {
            struct fm_seek_parm parm;
            FM_DEBUG("FM_IOCTL_SEEK\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//              return -EPERM;

            if (copy_from_user(&parm, (void*)arg, sizeof(struct fm_seek_parm)))
                return -EFAULT;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            ret = fm_seek(fm, &parm);
            up(&fm_ops_mutex);

            if (copy_to_user((void*)arg, &parm, sizeof(struct fm_seek_parm)))
                return -EFAULT;

            break;
        }
        
        case FM_IOCTL_SETVOL:
        {
            uint32_t vol;
            FM_DEBUG("FM_IOCTL_SETVOL\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//              return -EPERM;

            if(copy_from_user(&vol, (void*)arg, sizeof(uint32_t))) {
                FM_ALERT("copy_from_user failed\n");
                return -EFAULT;
            }
             //FM_DEBUG("FM_IOCTL_SETVOL arg = %p, vol= %d\n", arg, vol);
            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            ret = fm_setvol(fm, vol);
            up(&fm_ops_mutex);

            break;
        }
        
        case FM_IOCTL_GETVOL:
        {
            uint32_t vol;
            FM_DEBUG("FM_IOCTL_GETVOL\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//              return -EPERM;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            ret = fm_getvol(fm, &vol);
            up(&fm_ops_mutex);

            if (copy_to_user((void*)arg, &vol, sizeof(uint32_t)))
                return -EFAULT;

            break;
        }

        case FM_IOCTL_MUTE:
        {
                uint32_t bmute;
                FM_DEBUG("FM_IOCTL_MUTE\n");

                // FIXME!!
                //            if (!capable(CAP_SYS_ADMIN))
                //              return -EPERM;
                if (copy_from_user(&bmute, (void*)arg, sizeof(uint32_t)))
                {
                    FM_DEBUG("copy_from_user mute failed!\n");
                    return -EFAULT;    
                }

                FM_DEBUG("FM_IOCTL_MUTE:%d\n", bmute); 
                if (down_interruptible(&fm_ops_mutex))
                    return -EFAULT;

            if (bmute)
                ret = RDAFM_enable_hmute(fm->i2c_client);
            else
                ret = RDAFM_clear_hmute(fm->i2c_client);

            up(&fm_ops_mutex);

            break;
        }

        case FM_IOCTL_GETRSSI:
        {
            uint32_t rssi;
            FM_DEBUG("FM_IOCTL_GETRSSI\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//              return -EPERM;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;

            ret = fm_getrssi(fm, &rssi);
            up(&fm_ops_mutex);

            if (copy_to_user((void*)arg, &rssi, sizeof(uint32_t)))
                return -EFAULT;

            break;
        }
        
        case FM_IOCTL_RW_REG:
        {
            struct fm_ctl_parm parm_ctl;
            FM_DEBUG("FM_IOCTL_RW_REG\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//              return -EPERM;

            if (copy_from_user(&parm_ctl, (void*)arg, sizeof(struct fm_ctl_parm)))
                return -EFAULT;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            
            if(parm_ctl.rw_flag == 0) //write
            {
                ret = RDAFM_write(fm->i2c_client, parm_ctl.addr, parm_ctl.val);
            }
            else
            {
                ret = RDAFM_read(fm->i2c_client, parm_ctl.addr, &parm_ctl.val);
            }
            
            up(&fm_ops_mutex);
            if ((parm_ctl.rw_flag == 0x01) && (!ret)) // Read success.
            { 
                if (copy_to_user((void*)arg, &parm_ctl, sizeof(struct fm_ctl_parm)))
                    return -EFAULT;
            }
            break;
        }

        case FM_IOCTL_GETCHIPID:
        {
            uint16_t chipid;            

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;

                RDAFM_GetChipID(fm->i2c_client, &chipid);
                //fm->chip_id = chipid;
                FM_DEBUG("FM_IOCTL_GETCHIPID:%04x\n", chipid);   
                up(&fm_ops_mutex);

                if (copy_to_user((void*)arg, &chipid, sizeof(uint16_t)))
                return -EFAULT;
                            
            break;
        }

        case FM_IOCTL_EM_TEST:
        {
            struct fm_em_parm parm_em;
            FM_DEBUG("FM_IOCTL_EM_TEST\n");

// FIXME!!
//            if (!capable(CAP_SYS_ADMIN))
//              return -EPERM;

            if (copy_from_user(&parm_em, (void*)arg, sizeof(struct fm_em_parm)))
                return -EFAULT;

            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
           
            RDAFM_em_test(fm->i2c_client, parm_em.group_idx, parm_em.item_idx, parm_em.item_value);
            
            up(&fm_ops_mutex);
            
            break;
        }
        case FM_IOCTL_IS_FM_POWERED_UP:
        {
            uint32 powerup;
            FM_DEBUG("FM_IOCTL_IS_FM_POWERED_UP");
            if (fm->powerup) {
                powerup = 1;
            } else {
                powerup = 0;
            }
            if (copy_to_user((void*)arg, &powerup, sizeof(uint32_t)))
                return -EFAULT;
            break;
        }

        #ifdef FMDEBUG
        case FM_IOCTL_DUMP_REG:
        {
            uint16_t chipid = 0;
            if (down_interruptible(&fm_ops_mutex))
                return -EFAULT;
            RDAFM_GetChipID(fm->i2c_client, &chipid);
            up(&fm_ops_mutex);

            break;
        }
        #endif

        case FM_IOCTL_SCAN:
            {
                struct fm_scan_parm parm;
                FM_DEBUG("FM_IOCTL_SCAN\n");
                if (false == fm->powerup){
                    return -EFAULT;
                }
                if(copy_from_user(&parm, (void*)arg, sizeof(struct fm_scan_parm))){
                    return -EFAULT;
                }
                if (down_interruptible(&fm_ops_mutex)){
                    return -EFAULT;
                }
                fm_scan(fm, &parm);
                up(&fm_ops_mutex);

                if(copy_to_user((void*)arg, &parm, sizeof(struct fm_scan_parm))){
                    return -EFAULT;
                }

                break;
            }

        case FM_IOCTL_STOP_SCAN:
            {
                FM_DEBUG("FM_IOCTL_STOP_SCAN\n");
                break;
            }

        default:
            {
                FM_DEBUG("default\n");
                break;
            }
    }

    return ret;
}
static loff_t fm_ops_lseek(struct file *filp, loff_t off, int whence)
{
//    struct fm *fm = filp->private_data;

    if(whence == SEEK_END){
        //fm_hwscan_stop(fm);
        atomic_set(&scan_complete_flag, 0);
    }else if(whence == SEEK_SET){  
        //FM_EVENT_SEND(fm->rds_event, FM_RDS_DATA_READY);
    }   
    return off;    
}

static int fm_ops_open(struct inode *inode, struct file *filp)
{
    struct fm *fm = container_of(inode->i_cdev, struct fm, cdev);

    FM_DEBUG("%s\n", __func__);

    if (down_interruptible(&fm_ops_mutex))
        return -EFAULT;
        
    // TODO: only have to set in the first time?
    // YES!!!!

    fm->ref++;

    up(&fm_ops_mutex);

    filp->private_data = fm;

    // TODO: check open flags

    return 0;
}

static int fm_ops_release(struct inode *inode, struct file *filp)
{
    int err = 0;
    struct fm *fm = container_of(inode->i_cdev, struct fm, cdev);

    FM_DEBUG("%s\n", __func__);

    if (down_interruptible(&fm_ops_mutex))
        return -EFAULT;
    fm->ref--;
    if(fm->ref < 1) {
        if(fm->powerup == true) {
            fm_powerdown(fm);           
        }
    }
    
        up(&fm_ops_mutex);

    return err;
}

extern void __iomem *base_iomem(void);
static int fm_init(struct i2c_client *client)
{
    int err;
    struct fm *fm = NULL;
    int ret = -1;
	
	void __iomem *regs;
	regs = base_iomem();
	//writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	//writel(0x03, regs + 0x8a004);//gpio138--->0x8a
	writel(0x201, regs + 0x00000);//gpio0--->0x8a
	writel(0x03, regs + 0x00004);//gpio0--->0x8a
	FM_ALERT("fm_init,powerup......\n");
	msleep(50);

    FM_DEBUG("%s\n", __func__);
    if (!(fm = kzalloc(sizeof(struct fm), GFP_KERNEL)))
    {
        FM_ALERT("-ENOMEM\n");
        err = -ENOMEM;
        goto ERR_EXIT;
    }

    fm->ref = 0;
    fm->powerup = false;
    atomic_set(&scan_complete_flag, 0);
    // read RDAFM chip ID
    FM_DEBUG("%s read RDAFM chip ID\n", __func__);
    ret = RDAFM_GetChipID(client, &RDAFM_CHIP_ID);
    if(ret < 0){
        FM_DEBUG("%s read RDA chip ID failed\n", __func__);
        err = -ENOMEM;
        goto ERR_EXIT;
    }
    else{
        fm->chip_id = RDAFM_CHIP_ID;
    }

    FM_DEBUG("%s RDAFM chip ID = 0x%04x\n", __func__, RDAFM_CHIP_ID);
    if ((err = fm_setup_cdev(fm)))
    {
        goto ERR_EXIT;
    }

    g_fm_struct = fm;
    fm->i2c_client = client;
    i2c_set_clientdata(client, fm);
	//----------------------------------------------
        //mode: 0: FM_RECEIVER; 1: FM_TRANSMI;
        //freq: 875~1080
        //fm_test_tune(0,971); // open when you want test fm //add_by_frank

    /***********Add porc file system*************/
    /* //add_by_frank top 
  // g_fm_proc = proc_create_data(FM_PROC_FILE, 0444, NULL,&fm_ops);
    g_fm_proc = create_proc_entry(FM_PROC_FILE, 0444, NULL);
    if (g_fm_proc == NULL) {
        FM_ALERT("create_proc_entryte_proc_entry failed\n");
        err = -ENOMEM;
        goto ERR_EXIT;
    } else {
        g_fm_proc->read_proc = fm_proc_read;
        g_fm_proc->write_proc = NULL;
        //g_fm_proc->owner = THIS_MODULE;
        FM_ALERT("create_proc_entry success\n");
    }
    //add_by_frank end
    */
    /********************************************/

    FM_DEBUG("fm_init is ok!\n");

    return 0;

ERR_EXIT:
    kfree(fm);

    return err;
}
/*
//add_by_frank top 
static int fm_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int cnt= 0;
    struct fm *fm  = g_fm_struct;
    FM_DEBUG("Enter fm_proc_read.\n");
    if(off != 0)
        return 0;
    if (fm != NULL && fm->powerup) {
        cnt = sprintf(page, "1\n");
    } else {
        cnt = sprintf(page, "0\n");
    }
    *eof = 1;
    FM_DEBUG("Leave fm_proc_read. cnt = %d\n", cnt);
    return cnt;
}
//add_by_frank end
*/
static int fm_destroy(struct fm *fm)
{
    int err = 0;

    FM_DEBUG("%s\n", __func__);

    device_destroy(fm->cls, fm->dev_t);
    class_destroy(fm->cls);

    cdev_del(&fm->cdev);
    unregister_chrdev_region(fm->dev_t, 1);

    fm_powerdown(fm);

    remove_proc_entry(FM_PROC_FILE, NULL);
    

    // FIXME: any other hardware configuration ?

    // free all memory
    kfree(fm);

    return err;
}

/*
 *  fm_powerup
 */
static int fm_powerup(struct fm *fm, struct fm_tune_parm *parm)
{
    int i;
    uint16_t tRegValue = 0x0002;
    int ret = -1;

    struct i2c_client *client = fm->i2c_client;
	
	void __iomem *regs;
	regs = base_iomem();
	//writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	//writel(0x03, regs + 0x8a004);//gpio138--->0x8a
	writel(0x201, regs + 0x00000);//gpio0--->0x8a
	writel(0x03, regs + 0x00004);//gpio0--->0x8a
	FM_ALERT("fm_powerup......\n");
	msleep(100);
	
    if (fm->powerup)
    {
        parm->err = FM_BADSTATUS;
        return -EPERM;
    }

    
    FM_DEBUG("%s\n", __func__);


    if (0 == RDAFM_CHIP_ID)
    {
        ret = RDAFM_GetChipID(client, &RDAFM_CHIP_ID);
        if(ret < 0){
            FM_DEBUG("%s read RDA chip ID failed\n", __func__);
            parm->err = FM_FAILED;
            return -EPERM;
        }

    }
    FM_DEBUG("%s RDAFM chip ID = 0x%04x\n", __func__, RDAFM_CHIP_ID);

    //Reset RDA FM
    tRegValue = 0x0002;
    RDAFM_write(client, 0x02, tRegValue);
    msleep(100);

    tRegValue = 0xC001;
    RDAFM_write(client, 0x02, tRegValue);
    msleep(600);

    if (ID_RDA580XN == RDAFM_CHIP_ID) {
        FM_DEBUG("%s RDA580XN init\n", __func__);
        for (i=0; i<((sizeof(RDA580XN_initialization_reg)) / (sizeof(uint16_t))); i++)
        {
            ret = RDAFM_write(client, i+2, RDA580XN_initialization_reg[i]);

            if (ret < 0)
            {
               FM_ALERT("%s RDA580XN init failed!\n", __func__);
                parm->err = FM_FAILED;

                return -EPERM;
            }
        }
    }
    else if (ID_RDA5820NS == RDAFM_CHIP_ID){
        if(RDA_RADIO_WorkType == FM_RECEIVER){
            FM_DEBUG("%s RDA5820NS FM_RECEIVER\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820NS_RX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820NS_RX_initialization_reg[i].address == 0xFF)
                    msleep(RDA5820NS_RX_initialization_reg[i].value);
                else{
                    ret = RDAFM_write(client, RDA5820NS_RX_initialization_reg[i].address, RDA5820NS_RX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("RDA5820NS FM_RECEIVER init failed!\n");
                        parm->err = FM_FAILED;
                        return -EPERM;
                    }
                }
            }
        }else{
            FM_DEBUG("%s RDA5820NS FM_TRANSMI\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820NS_TX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820NS_TX_initialization_reg[i].address == 0xFF){
                    msleep(RDA5820NS_TX_initialization_reg[i].value);
                }else{
                    ret = RDAFM_write(client, RDA5820NS_TX_initialization_reg[i].address, RDA5820NS_TX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("RDA5820NS FM_TRANSMI init failed!\n");
                        parm->err = FM_FAILED;
                        return -EPERM;
                    }
                }
            }
        }

    }
    else if (ID_RDA5820 == RDAFM_CHIP_ID){
        if(RDA_RADIO_WorkType == FM_RECEIVER){
            FM_DEBUG("%s RDA5820 FM_RECEIVER\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820_RX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820_RX_initialization_reg[i].address == 0xFF)
                    msleep(RDA5820_RX_initialization_reg[i].value);
                else{
                    ret = RDAFM_write(fm_client, RDA5820_RX_initialization_reg[i].address, RDA5820_RX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("%s RDA5820 FM_RECEIVER init failed!\n", __func__);
                        return -EPERM;
                    }
                }
            }
        }else{
            FM_DEBUG("%s RDA5820 FM_TRANSMI\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820_TX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820_TX_initialization_reg[i].address == 0xFF){
                    msleep(RDA5820_TX_initialization_reg[i].value);
                }else{
                    ret = RDAFM_write(fm_client, RDA5820_TX_initialization_reg[i].address, RDA5820_TX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("%s RDA5820 FM_TRANSMI init failed!\n", __func__);
                        return -EPERM;
                    }
                }
            }
        }

    }    


    FM_DEBUG("pwron ok\n");
    fm->powerup = true;

    if (fm_tune(fm, parm) < 0)
    {
        return -EPERM;
    }
    fm_tune_data.byPowerUp = true;
    memcpy(&fm_tune_data.parm, parm, sizeof(fm_tune_data.parm));

    parm->err = FM_SUCCESS;

    return 0;
}

/*
 *  fm_powerdown
 */
static int fm_powerdown(struct fm *fm)
{
    uint16_t tRegValue = 0;
    //int ret = -1;
    struct i2c_client *client = fm->i2c_client;

    RDAFM_read(client, 0x02, &tRegValue);
    tRegValue &= (~(1 << 0));
    RDAFM_write(client, 0x02, tRegValue);

    fm->powerup = false;
    FM_DEBUG("fm_powerdown ok\n");

    return 0;
}


/*
 *  fm_seek
 */
static int fm_seek(struct fm *fm, struct fm_seek_parm *parm)
{
    int ret = 0;
    uint16_t val = 0;
    uint8_t spaec = 1;
    uint16_t tFreq = 870;
    uint16_t tRegValue = 0;
    uint16_t bottomOfBand = 870;
    int falseStation = -1;
    u32 i = 0;
    u32 j = 0;

    struct i2c_client *client = fm->i2c_client;

    if (!fm->powerup)
    {
        parm->err = FM_BADSTATUS;
        return -EPERM;
    }

    if (parm->space == FM_SPACE_100K)
    {
        spaec = 1;
        val &= (~((1<<0) | (1<<1)));
    }
    else if (parm->space == FM_SPACE_200K)
    {
        spaec = 2;
        val &= (~(1<<1));
        val |= (1<<0);
    }
    else
    {
        parm->err = FM_EPARM;
        return -EPERM;
    }

    if (parm->band == FM_BAND_UE)
    {
        val &= (~((1<<2) | (1<<3)));
        bottomOfBand = 870;
        fm->min_freq = 870;
        fm->max_freq = 1080;
    }
    else if (parm->band == FM_BAND_JAPAN) 
    {
        val &= (~(1<<3));
        val |= (1 << 2);
        bottomOfBand = 760;
        fm->min_freq = 760;
        fm->max_freq = 910;
    }
    else if (parm->band == FM_BAND_JAPANW) {
        val &= (~(1<<2));
        val |= (1 << 3);
        bottomOfBand = 760;
        fm->min_freq = 760;
        fm->max_freq = 1080;
    }
    else
    {
        FM_ALERT("band:%d out of range\n", parm->band);
        parm->err = FM_EPARM;
        return -EPERM;
    }

    if (parm->freq < fm->min_freq || parm->freq > fm->max_freq) {
        FM_ALERT("freq:%d out of range\n", parm->freq);
        parm->err = FM_EPARM;
        return -EPERM;
    }

    if (parm->seekth > 0x0B) {
        FM_ALERT("seekth:%d out of range\n", parm->seekth);
        parm->err = FM_EPARM;
        return -EPERM;
    }
#if 0
    RDAFM_read(client, 0x05, &tRegValue);
    tRegValue &= (~(0x7f<<8));
    //tRegValue |= ((parm->seekth & 0x7f) << 8);
    tRegValue |= ((0x8 & 0x7f) << 8);
    RDAFM_write(client, 0x05, tRegValue);
#endif

#ifdef FMDEBUG
    if (parm->seekdir == FM_SEEK_UP)
        FM_DEBUG("seek %d up\n", parm->freq);
    else
        FM_DEBUG("seek %d down\n", parm->freq);
#endif

    // (1) set hmute bit
    RDAFM_enable_hmute(client);

    tFreq = parm->freq;

    do {
        if (parm->seekdir == FM_SEEK_UP)
            tFreq += spaec;
        else
            tFreq -= spaec;

        if (tFreq > fm->max_freq)
            tFreq = fm->min_freq;
        if (tFreq < fm->min_freq)
            tFreq = fm->max_freq;

        val = (((tFreq - bottomOfBand) << 6) | (1 << 4) | (val & 0x0f));
        RDAFM_write(client, 0x03, val);
        //msleep(40);
        for (i=0; i<8; i++)
        {
            j++;
            msleep(5);
        }
        ret = RDAFM_read(client, 0x0B, &tRegValue);
        if (ret < 0)
        {
            FM_DEBUG("fm_seek: read register failed tunning freq = %4X\n", tFreq);
            falseStation = -1;
        }
        else
        {
            if ((tRegValue & 0x0100) == 0x0100)
                falseStation = 0;
            else
                falseStation = -1;
        }

        if(falseStation == 0)
            break;
    }while(tFreq != parm->freq);


    //clear hmute
    RDAFM_clear_hmute(client);

    if (falseStation == 0) // seek successfully
    {    
        parm->freq = tFreq;
        FM_ALERT("fm_seek success, freq:%d\n", parm->freq);
        parm->err = FM_SUCCESS;
    }
    else
    {
        FM_ALERT("fm_seek failed, invalid freq\n");
        parm->err = FM_SEEK_FAILED;
        ret = -1;
    }

    return ret;
}
static int  fm_scan(struct fm *fm, struct fm_scan_parm *parm)
{
    int ret = 0;
    uint16_t tRegValue = 0;
    uint16_t scandir = FM_SCAN_UP; //scandir
    uint8_t space = 1; 
    struct i2c_client *client = fm->i2c_client;

    if (!fm->powerup){
        parm->err = FM_BADSTATUS;
        return -EPERM;
    }
    FM_DEBUG("%s\n", __func__);
    
        if((!parm->ScanTBL) || (parm->ScanTBLSize == 0)) {
        FM_DEBUG("%s: pScanTBL is null or tmp_scanTBLsize == 0: tmp_scanTBLsize = %d\n", __func__, parm->ScanTBLSize);
        return ret;
    }

    RDAFM_read(client, 0x03, &tRegValue);

    if (parm->space == FM_SPACE_100K){
        space = 1;
        tRegValue &= (~((1<<0) | (1<<1))); //set 03H's bit[1:0] to 00
    }else if (parm->space == FM_SPACE_200K) {
        space = 2;
        tRegValue &= (~(1<<1)); //clear bit[1]
        tRegValue |= (1<<0);    //set bit[0]
    }else{
        //default
        space = 1;
        tRegValue &= (~((1<<0) | (1<<1))); //set 03H's bit[1:0] to 00
    }

    if(parm->band == FM_BAND_UE){
        tRegValue &= (~((1<<2) | (1<<3)));
        fm->min_freq = 870;
        fm->max_freq = 1080;
    }else if(parm->band == FM_BAND_JAPAN){
        tRegValue &= (~(1<<3));
        tRegValue |= (1 << 2);
        fm->min_freq = 760;
        fm->max_freq = 900;
    }else if(parm->band == FM_BAND_JAPANW){
        tRegValue &= (~(1<<2));
        tRegValue |= (1 << 3);
        fm->min_freq = 760;
        fm->max_freq = 1080;
    }else{
        parm->err = FM_EPARM;
        return -EPERM;
    }

    //set space and band
    RDAFM_write(client, 0x03, tRegValue);
    msleep(40);

    if (RDAFM_Scan(client, fm->min_freq, fm->max_freq, &(parm->freq), parm->ScanTBL, &(parm->ScanTBLSize), scandir, space))
    {
        parm->err = FM_SCAN_FAILED;
        ret = -1;
        FM_DEBUG("%s: faild\n", __func__);
    }else{
        parm->err = FM_SUCCESS;
        FM_DEBUG("%s: success\n", __func__);
        ret = 0;
    }

    return ret;
}


static int fm_setvol(struct fm *fm, uint32_t vol)
{
    int ret = 0;
    uint16_t tRegValue = 0;
    struct i2c_client *client = fm->i2c_client;

    if (vol > 15)
        vol = 15;

    FM_DEBUG("fm_setvol:%d\n", vol);

    ret = RDAFM_read(client, 0x05, &tRegValue);
    if (ret)
        return -EPERM;
    FM_DEBUG("fm_setvol: read reg val = %x\n", tRegValue);
    tRegValue &= ~(0x000f);
    tRegValue |= vol;
    FM_DEBUG("fm_setvol: write reg val = %x\n", tRegValue);
    ret = RDAFM_write(client, 0x05, tRegValue);
    if (ret)
        return -EPERM;

    return 0;
}

static int fm_getvol(struct fm *fm, uint32_t *vol)
{
    int ret = 0;
    uint16_t tRegValue;
    struct i2c_client *client = fm->i2c_client;

    ret = RDAFM_read(client, 0x05, &tRegValue);
    if (ret)
        return -EPERM;


    *vol = (tRegValue & 0x000F);

    return 0;
}

static int fm_getrssi(struct fm *fm, uint32_t *rssi)
{
    int ret = 0;
    uint16_t tRegValue;
    struct i2c_client *client = fm->i2c_client;

    ret = RDAFM_read(client, 0x0B, &tRegValue);
    if (ret)
        return -EPERM;


    *rssi = (uint32_t)((tRegValue >> 9) & RDAFM_MASK_RSSI);

    FM_DEBUG("rssi value:%d\n", *rssi);

    return 0;
}

/*
 *  fm_tune
 */
static int fm_tune(struct fm *fm, struct fm_tune_parm *parm)
{
    int ret;
    uint16 val = 0;
    uint8 space = 1;
    uint16 bottomOfBand = 870;
    u32 i = 0;
    u32 j = 0;

    struct i2c_client *client = fm->i2c_client;

    FM_DEBUG("%s\n", __func__);

    if (!fm->powerup)
    {
        parm->err = FM_BADSTATUS;
        return -EPERM;
    } 


    if (parm->space == FM_SPACE_100K)
    {
        space = 1;
        val &= (~((1<<0) | (1<<1)));
    }
    else if (parm->space == FM_SPACE_200K)
    {
        space = 2;
        val |= (1<<0);
        val &= (~(1<<1));
    }
    else
    {
        parm->err = FM_EPARM;
        return -EPERM;
    }

    if (parm->band == FM_BAND_UE)
    {
        val &= (~((1<<2) | (1<<3)));
        bottomOfBand = 870;
        fm->min_freq = 870;
        fm->max_freq = 1080;
    }
    else if (parm->band == FM_BAND_JAPAN) 
    {
        val &= (~(1<<3));
        val |= (1 << 2);
        bottomOfBand = 760;
        fm->min_freq = 760;
        fm->max_freq = 910;
    }
    else if (parm->band == FM_BAND_JAPANW) {
        val &= (~(1<<2));
        val |= (1 << 3);
        bottomOfBand = 760;
        fm->min_freq = 760;
        fm->max_freq = 1080;
    }
    else
    {
        FM_ALERT("band:%d out of range\n", parm->band);
        parm->err = FM_EPARM;
        return -EPERM;
    }

    if (parm->freq < fm->min_freq || parm->freq > fm->max_freq) {
        FM_ALERT("freq:%d out of range\n", parm->freq);
        parm->err = FM_EPARM;
        return -EPERM;
    }


    //RDAFM_enable_hmute(client);

    val = (((parm->freq - bottomOfBand) << 6) | (1 << 4) | (val & 0x0f));
    FM_DEBUG("fm_tune, freq:%d, reg value%d\n", parm->freq, val);

    ret = RDAFM_write(client, 0x03, val);
    if (ret < 0)
    {
        FM_ALERT("fm_tune write freq failed\n");
        parm->err = FM_SEEK_FAILED;
        return ret;
    }

    for (i=0; i<8; i++)
    {
        j++;
        msleep(5);
    }

#ifdef FM_DEBUG
    RDAFM_read(client, 0x0A, &val);
    FM_DEBUG("%s read freq reg value = 0x%04x\n", __func__, val);
#endif

    return ret;
}

static int fm_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int err = -1;
    FM_DEBUG("fm_i2c_probe\n");    
    //client->timing = 50;
    //client->timing = 200;
    fm_client = client;
    if ((err = fm_init(client)))
    {
        FM_ALERT("fm_init ERR:%d\n", err);
        goto ERR_EXIT;
    }   
    
    return 0;   
    
ERR_EXIT:
    return err;    
}
/*
static int fm_i2c_detect(struct i2c_client *client, struct i2c_board_info *info)
{
    struct i2c_adapter *adapter = client->adapter;
    if(adapter->nr == 1)
    {
          FM_DEBUG("fm_i2c_detect\n");
        strcpy(info->type, RDAFM_DEV_NAME);
        return 0;
    }
    else
    {
        return -ENODEV;
    }
}
*/
static int fm_i2c_remove(struct i2c_client *client)
{
    int err = 0;
    struct fm *fm = i2c_get_clientdata(client);
    
    FM_DEBUG("fm_i2c_remove\n");
    if(fm)
    {    
        fm_destroy(fm);
        fm = NULL;
    }
    
    return err;
}


static const struct of_device_id fm_match_table[] = {
      { .compatible = "rda5807,fm", },
    { },
};
static const struct i2c_device_id fm_i2c_table[] = {
    {RDAFM_DEV_NAME, RDAFM_I2C_CHANNEL},
    {}
};


static unsigned short addr_data[] = {RDAFM_SLAVE_ADDR, I2C_CLIENT_END, I2C_CLIENT_END};

MODULE_DEVICE_TABLE(i2c,fm_i2c_table);

static struct i2c_driver RDAFM_i2c_driver = {
    .driver = {
	.owner  = THIS_MODULE,
        .name = RDAFM_DEV_NAME,
        .of_match_table= fm_match_table,
    },
    .probe = fm_i2c_probe,
    .remove = fm_i2c_remove,
    .id_table = fm_i2c_table,
    .address_list = (const unsigned short*) addr_data,
};



#if defined(REGISTER_DEVICE)

static int fm_probe(struct platform_device *pdev)
{
    int ret = -1;
    FM_DEBUG("fm_probe\n");
    
    ret = i2c_add_driver(&RDAFM_i2c_driver);
    if (ret)
    {
        FM_ALERT("fm_probe add driver failed\n");
    }
      
    return ret;   
} 
    
static int fm_remove(struct platform_device *pdev)
{
    FM_DEBUG("fm_remove\n");
    i2c_del_driver(&RDAFM_i2c_driver); 
    
    return 0; 
}


static struct platform_driver fm_dev_drv =
{
    .probe   = fm_probe,
    .remove  = fm_remove,
#if 0//def CONFIG_PM //Not need now   
    .suspend = fm_suspend,
    .resume  = fm_resume,
#endif    
    .driver = {
        .name   = FM_NAME,
        .owner  = THIS_MODULE,    
    }
};


static struct platform_device fm_device = {
    .name   = FM_NAME,
    .id = -1, 
};
#endif

/*
//mode: 0: FM_RECEIVER; 1: FM_TRANSMI;
//freq: 875~1080
static void fm_test_tune(int mode, int freq)
{
    int i;
    uint16_t tRegValue = 0x0002;
    uint16_t val_tune = 0x0002;
    int ret = -1;

    FM_DEBUG("%s: mode: %d; freq: %d\n", __func__, mode, freq);
    // if chip_id is ID_RDA5820NS, enable the FM chip in combo
#ifdef RDA599X_FM_SUPPORT
        ret = rda_fm_power_on();
        if(ret < 0){
       	    FM_ALERT("%s rda_fm_power_on failed!\n", __func__);
            return;
        }
        msleep(100);
#endif


    //Reset RDA FM
    tRegValue = 0x0002;
    RDAFM_write(fm_client, 0x02, tRegValue);
    msleep(100);

    tRegValue = 0xC001;
    RDAFM_write(fm_client, 0x02, tRegValue);    
    msleep(600);

    if (ID_RDA580XN == RDAFM_CHIP_ID) {
        FM_DEBUG("%s RDA580XN init\n", __func__);
        for (i=0; i<((sizeof(RDA580XN_initialization_reg)) / (sizeof(uint16_t))); i++)
        {
            ret = RDAFM_write(fm_client, i+2, RDA580XN_initialization_reg[i]);

            if (ret < 0)
            {
                FM_ALERT("%s RDA580XN init failed!\n", __func__);

                return;
            }
        }
    }
    else if (ID_RDA5820 == RDAFM_CHIP_ID){
        if(0 == mode){
            FM_DEBUG("%s RDA5820 FM_RECEIVER\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820_RX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820_RX_initialization_reg[i].address == 0xFF)
                    msleep(RDA5820_RX_initialization_reg[i].value);
                else{
                    ret = RDAFM_write(fm_client, RDA5820_RX_initialization_reg[i].address, RDA5820_RX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("%s RDA5820 FM_RECEIVER init failed!\n", __func__);
                        return;
                    }
                }
            }
        }
        else{
            FM_DEBUG("%s RDA5820 FM_TRANSMI\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820_TX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820_TX_initialization_reg[i].address == 0xFF){
                    msleep(RDA5820_TX_initialization_reg[i].value);
                }else{
                    ret = RDAFM_write(fm_client, RDA5820_TX_initialization_reg[i].address, RDA5820_TX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("%s RDA5820 FM_TRANSMI init failed!\n", __func__);
                        return;
                    }
                }
            }
        }

    }    
    else if (ID_RDA5820NS == RDAFM_CHIP_ID){
        if(0 == mode){
            FM_DEBUG("%s RDA5820NS FM_RECEIVER\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820NS_RX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820NS_RX_initialization_reg[i].address == 0xFF)
                    msleep(RDA5820NS_RX_initialization_reg[i].value);
                else{
                    ret = RDAFM_write(fm_client, RDA5820NS_RX_initialization_reg[i].address, RDA5820NS_RX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("%s RDA5820NS FM_RECEIVER init failed!\n", __func__);
                        return;
                    }
                }
            }
        }else{
            FM_DEBUG("%s RDA5820NS FM_TRANSMI\n", __func__);
            for (i = 0; i < ((sizeof(RDA5820NS_TX_initialization_reg)) / (sizeof(RDA_FM_REG_T))); i++)
            {
                if(RDA5820NS_TX_initialization_reg[i].address == 0xFF){
                    msleep(RDA5820NS_TX_initialization_reg[i].value);
                }else{
                    ret = RDAFM_write(fm_client, RDA5820NS_TX_initialization_reg[i].address, RDA5820NS_TX_initialization_reg[i].value);
                    if (ret < 0)
                    {
                        FM_DEBUG("%s RDA5820NS FM_TRANSMI init failed!\n", __func__);
                        return;
                    }
                }
            }
        }

    }

    tRegValue = (((freq - 870) << 6) | (1 << 4) );
    FM_DEBUG("%s: freq:%d, reg value: %x\n", __func__, freq, tRegValue);

    ret = RDAFM_write(fm_client, 0x03, tRegValue);
    if (ret < 0)
    {
        FM_ALERT("%s: write freq failed\n", __func__);
    }



    msleep(100);
#ifdef FM_DEBUG
    RDAFM_read(fm_client, 0x0A, &tRegValue);
    FM_DEBUG("%s ----test read freq reg value    = 0x%04x----------\n", __func__, tRegValue);
    RDAFM_read(fm_client, 0x0B, &val_tune);
    FM_DEBUG("%s ----test read freq reg val_tune = 0x%04x-------\n", __func__, val_tune);

#endif
    FM_DEBUG("%s ok\n", __func__);
}
*/

#if 0
static struct i2c_board_info rdafm_i2c_board_info[] __initdata = {
	{ I2C_BOARD_INFO(FM_DEVICE_NAME, 0x11), },
};


extern int __init
i2c_register_board_info(int busnum,
	struct i2c_board_info const *info, unsigned len);
#endif
static int __init fm_module_init(void)
{
    int err = 0;

    FM_DEBUG("fm_module_init chappier \n");

#if 0
	err = i2c_register_board_info(1, rdafm_i2c_board_info, ARRAY_SIZE(rdafm_i2c_board_info));
	if (err < 0)
		printk("%s: rdafm i2c board info init failed!\n", __func__);

	//printk("rdafm i2c board info registered\n");
#endif

#if defined(REGISTER_DEVICE)
    err = platform_device_register(&fm_device);
    if(err){
        FM_ALERT("platform_device_register  fail\n");
        return err;
    }else{
        FM_DEBUG("platform_device_register  success\n");
    }

    err = platform_driver_register(&fm_dev_drv);
    if (err)
    {
        FM_ALERT("platform_driver_register failed\n");
    }else{
        FM_DEBUG("platform_driver_register success\n");
    }

#else

    err = i2c_add_driver(&RDAFM_i2c_driver);
    if (err)
    {
        FM_ALERT("fm_module_init add driver failed\n");
    }else{
        FM_DEBUG("fm_module_init add driver success chappier---09-23-16-34\n");
    }


#endif
    return err;
}


static void __exit fm_module_exit(void)
{
    FM_DEBUG("fm_module_exit\n");

#if defined(REGISTER_DEVICE)

    platform_driver_unregister(&fm_dev_drv);
    platform_device_unregister(&fm_device);
    
#else

    i2c_del_driver(&RDAFM_i2c_driver); 
    
#endif
}

module_init(fm_module_init);
module_exit(fm_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RDA FM Driver");
MODULE_AUTHOR("Anson Tseng <rianzeng@rdamicro.com>");


