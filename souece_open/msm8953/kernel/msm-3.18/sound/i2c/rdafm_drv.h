

//#ifndef __RDAFM_DRV_H__
//#define __RDAFM_DRV_H__

#include <linux/ioctl.h>
#include <linux/time.h>

//Version 2.1

#define FM_NAME             "RDAFM"
#define FM_DEVICE_NAME      "/dev/RDAFM"


//customer need customize the I2C channel
#define RDAFM_I2C_CHANNEL             8 //old is 0

#define FMDEBUG                     1



#define FM_SCAN_TBL_SIZE             40//FM radio scan max channel size


// errno
#define FM_SUCCESS                  0
#define FM_FAILED                   1
#define FM_EPARM                    2
#define FM_BADSTATUS                3
#define FM_TUNE_FAILED              4
#define FM_SEEK_FAILED              5
#define FM_BUSY                     6
#define FM_SCAN_FAILED              7

// band

#define FM_BAND_UNKNOWN             0
#define FM_BAND_UE                  1 // US/Europe band  87.5MHz ~ 108MHz (DEFAULT)
#define FM_BAND_JAPAN               2 // Japan band      76MHz   ~ 90MHz
#define FM_BAND_JAPANW              3 // Japan wideband  76MHZ   ~ 108MHz
#define FM_BAND_SPECIAL             4 // special   band  between 76MHZ   and  108MHz
#define FM_BAND_DEFAULT FM_BAND_UE
#define FM_BAND_FREQ_L              875
#define FM_BAND_FREQ_H              1080
#define FM_FREQ_MIN                 FM_BAND_FREQ_L
#define FM_FREQ_MAX                 FM_BAND_FREQ_H
#define FM_RAIDO_BAND               FM_BAND_UE
// space
#define FM_SPACE_UNKNOWN            0
#define FM_SPACE_100K               1
#define FM_SPACE_200K               2
#define FM_SPACE_DEFAULT            FM_SPACE_100K
#define FM_SEEK_SPACE               FM_SPACE_100K




// auto HiLo
#define FM_AUTO_HILO_OFF            0
#define FM_AUTO_HILO_ON             1

// seek direction
#define FM_SEEK_UP                  0
#define FM_SEEK_DOWN                1
#define FM_SCAN_UP                  0
#define FM_SCAN_DOWN                1


// seek threshold
#define FM_SEEKTH_LEVEL_DEFAULT     6

#define FM_IOC_MAGIC                0xF5 // FIXME: any conflict?

struct fm_tune_parm {
    uint8_t err;
    uint8_t band;
    uint8_t space;
    uint8_t hilo;
    uint16_t freq; // IN/OUT parameter
};

struct fm_seek_parm {
    uint8_t err;
    uint8_t band;
    uint8_t space;
    uint8_t hilo;
    uint8_t seekdir;
    uint8_t seekth;
    uint16_t freq; // IN/OUT parameter
};

struct fm_scan_parm {
    uint8_t  err;
    uint8_t  band;
    uint8_t  space;
    uint8_t  hilo;
    uint16_t freq; // OUT parameter
    uint16_t ScanTBL[FM_SCAN_TBL_SIZE]; //need no less than the chip
    uint16_t ScanTBLSize; //IN/OUT parameter
};

struct fm_ch_rssi{
    uint16_t freq;
    uint16_t rssi;
};

struct fm_rssi_req{
    uint16_t num;
    uint16_t read_cnt;
    struct fm_ch_rssi cr[FM_SCAN_TBL_SIZE*16];
};


//#define FM_RDS_ENABLE            1


//#ifdef FM_RDS_ENABLE
//For RDS feature
typedef struct
{
   uint8_t TP;
   uint8_t TA;
   uint8_t Music;
   uint8_t Stereo;
   uint8_t Artificial_Head;
   uint8_t Compressed;
   uint8_t Dynamic_PTY;
   uint8_t Text_AB;
   uint32_t flag_status;
}RDSFlag_Struct;

typedef struct
{
   uint16_t Month;
   uint16_t Day;
   uint16_t Year;
   uint16_t Hour;
   uint16_t Minute;
   uint8_t Local_Time_offset_signbit;
   uint8_t Local_Time_offset_half_hour;
}CT_Struct;

typedef struct
{
   int16_t AF_Num;
   int16_t AF[2][25];  //100KHz
   uint8_t Addr_Cnt;
   uint8_t isMethod_A;
   uint8_t isAFNum_Get;
}AF_Info;

typedef struct
{
   uint8_t PS[4][8];
   uint8_t Addr_Cnt;
}PS_Info;

typedef struct
{
   uint8_t TextData[4][64];
   uint8_t GetLength;
   uint8_t isRTDisplay;
   uint8_t TextLength;
   uint8_t isTypeA;
   uint8_t BufCnt;
   uint16_t Addr_Cnt;
}RT_Info;

#define FM_IOCTL_POWERUP       _IOWR(FM_IOC_MAGIC, 0, struct fm_tune_parm*)
#define FM_IOCTL_POWERDOWN     _IOWR(FM_IOC_MAGIC, 1, int32_t*)
#define FM_IOCTL_TUNE          _IOWR(FM_IOC_MAGIC, 2, struct fm_tune_parm*)
#define FM_IOCTL_SEEK          _IOWR(FM_IOC_MAGIC, 3, struct fm_seek_parm*)
#define FM_IOCTL_SETVOL        _IOWR(FM_IOC_MAGIC, 4, uint32_t*)
#define FM_IOCTL_GETVOL        _IOWR(FM_IOC_MAGIC, 5, uint32_t*)
#define FM_IOCTL_MUTE          _IOWR(FM_IOC_MAGIC, 6, uint32_t*)
#define FM_IOCTL_GETRSSI       _IOWR(FM_IOC_MAGIC, 7, uint32_t*)
#define FM_IOCTL_SCAN          _IOWR(FM_IOC_MAGIC, 8, struct fm_scan_parm*)
#define FM_IOCTL_STOP_SCAN     _IO(FM_IOC_MAGIC,   9)

//IOCTL and struct for test
#define FM_IOCTL_GETCHIPID     _IOWR(FM_IOC_MAGIC, 10, uint16_t*)
#define FM_IOCTL_EM_TEST       _IOWR(FM_IOC_MAGIC, 11, struct fm_em_parm*)
#define FM_IOCTL_RW_REG        _IOWR(FM_IOC_MAGIC, 12, struct fm_ctl_parm*)
#define FM_IOCTL_GETMONOSTERO  _IOWR(FM_IOC_MAGIC, 13, uint16_t*)
#define FM_IOCTL_GETCURPAMD    _IOWR(FM_IOC_MAGIC, 14, uint16_t*)
#define FM_IOCTL_GETGOODBCNT   _IOWR(FM_IOC_MAGIC, 15, uint16_t*)
#define FM_IOCTL_GETBADBNT     _IOWR(FM_IOC_MAGIC, 16, uint16_t*)
#define FM_IOCTL_GETBLERRATIO  _IOWR(FM_IOC_MAGIC, 17, uint16_t*)
#define FM_IOCTL_IS_FM_POWERED_UP  _IOWR(FM_IOC_MAGIC, 24, uint32_t*)


#define FM_IOCTL_DUMP_REG   _IO(FM_IOC_MAGIC, 0xFF)


enum group_idx {
    mono=0,
    stereo,
    RSSI_threshold,
    HCC_Enable,
    PAMD_threshold,
    Softmute_Enable,
    De_emphasis,
    HL_Side,
    Demod_BW,
    Dynamic_Limiter,
    Softmute_Rate,
    AFC_Enable,
    Softmute_Level,
    Analog_Volume,
    GROUP_TOTAL_NUMS
};
    
enum item_idx {
    Sblend_OFF=0,
    Sblend_ON,  
    ITEM_TOTAL_NUMS
};

struct fm_ctl_parm {
    uint8_t err;
    uint8_t addr;
    uint16_t val;
    uint16_t rw_flag;//0:write, 1:read
};

struct fm_em_parm {
    uint16_t group_idx;
    uint16_t item_idx;
    uint32_t item_value;    
};
//#endif // __RDAFM_DRV_H__
