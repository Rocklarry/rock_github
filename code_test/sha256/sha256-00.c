/*
 * \file atsha204a-user.c
 *
 *
 */

/*
* 
*/
/*

*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA224_BLOCK_SIZE  SHA256_BLOCK_SIZE

typedef struct {
    uint8_t slot;
    uint8_t key[32];
    uint8_t digest[32];
}atsha204a_cmd_write_key_t;

typedef struct {
    uint8_t in_mode;
    uint8_t in_numin[20];
    uint8_t out_response[32];
}atsha204a_cmd_nonce_t;

typedef struct {
    uint8_t in_mode;
    uint8_t in_slot;
    uint8_t in_challenge_len;
    uint8_t in_challenge[32];
    uint8_t out_response[32];
}atsha204a_cmd_mac_t;

typedef struct {
    uint32_t tot_len;
    uint32_t len;
    uint8_t block[2 * SHA256_BLOCK_SIZE];
    uint32_t h[8];
} sha256_ctx;

atsha204a_cmd_mac_t cmd_mac;

atsha204a_cmd_nonce_t       nonce;

atsha204a_cmd_write_key_t   key;
uint8_t sn[9];


//随机数，自己定义
uint8_t in_challenge[39] = {
	0x24,0x66,0x17,0xc8,0xba,0x55,0xf9,0x56,
	0x71,0x93,0x16,0xdc,0x5d,0xa2,0x76,0x46,
	0x14,0x85,0x46,0x95,0x31,0xaa,0x82,0x37,
	0x43,0xa6,0xe7,0xe8,0x39,0xd7,0x23,0x88};

//
unsigned char challenge_key[20] = {
	0x13, 0x68, 0x72, 0x20, 0x82,
	0xe9, 0x14, 0x21, 0x87, 0xf5,
	0xdd, 0xaa, 0x5c, 0xd5, 0x44,
	0x35, 0xdf, 0xdb, 0x13, 0x67
};

#if 1
uint8_t slot0[32] = {0x11,0x11,0x23,0xB6,0xCC,0x53,0xB7,0xB9,0xE9,0xBB,0x51,0xFD,0x2F,0x74,0xCD,0x0E,0x91,0xD9,0x7F,0xEB,0x84,0x7B,0x98,0x09,0xF4,0xCD,0x93,0x6A,0xB6,0x48,0x11,0x11};

uint8_t slot1[32] = {0x11,0x11,0x23,0xB6,0xCC,0x53,0xB7,0xB9,0xE9,0xBB,0x51,0xFD,0x2F,0x74,0xCD,0x0E,0x91,0xD9,0x7F,0xEB,0x84,0x7B,0x98,0x09,0xF4,0xCD,0x93,0x6A,0xB6,0x48,0x11,0x11};
	
uint8_t slot2[32] = {0x22,0x22,0xC1,0x7C,0x1C,0x4D,0x56,0x89,0xAA,0x00,0x43,0xE3,0x9C,0xFB,0x6B,0x0B,0x68,0x49,0xE3,0x2C,0x24,0xA4,0x1B,0x06,0x34,0x49,0x1E,0x90,0x6B,0x62,0x22,0x22};

uint8_t slot3[32] = {0xc1,0x25,0x35,0xd4,0x85,0x74,0x45,0x54,0x64,0x71,0x14,0x19,0xb4,0xa5,0xff,0x54,0x74,0xf9,0x2b,0xb7,0xa4,0x61,0x7f,0xa3,0x32,0x12,0x28,0x76,0xc9,0x84,0x46,0x3f};

uint8_t slot4[32] = {0xa5,0xc8,0x02,0x10,0x54,0x40,0xfb,0x41,0xa6,0x22,0xa7,0x16,0x84,0x76,0x00,0x43,0x31,0xb7,0xaa,0x45,0x73,0x92,0x17,0x93,0x13,0x73,0x18,0x43,0x43,0x18,0x16,0x67};

uint8_t slot5[32] = {0x45,0x87,0x46,0x92,0x20,0xb6,0x46,0x16,0xf5,0x56,0x96,0xfc,0x13,0xa6,0xa6,0x73,0xb9,0x84,0x71,0x43,0x38,0x16,0x56,0xd9,0xe9,0xe4,0xa6,0xff,0x41,0xee,0xa6,0xf8};

uint8_t slot6[32] = {0xd6,0x23,0x45,0x12,0x56,0x89,0x74,0x89,0x46,0x24,0x63,0x71,0x46,0x16,0x75,0x89,0xd5,0x13,0xe9,0x76,0x14,0x53,0x42,0x89,0x82,0x46,0x13,0xd5,0x11,0x25,0x69,0x46};

uint8_t slot7[32] = {0xc3,0xfb,0x39,0x47,0x56,0x84,0x14,0x26,0x84,0x43,0x49,0x11,0x44,0x13,0x34,0x31,0x98,0x35,0x34,0x11,0x47,0x36,0x44,0x36,0x47,0x62,0x34,0x74,0xd5,0xe9,0xac,0xfd};

uint8_t slot8[32] = {0x46,0x48,0x14,0xca,0x15,0x78,0x91,0x68,0xc6,0x4f,0xc7,0xa6,0x14,0x22,0x47,0xdc,0x63,0x34,0x52,0x18,0xf4,0x7d,0x12,0x00,0x1f,0xaa,0x13,0x31,0x25,0x19,0x67,0x94};

uint8_t slot9[32] ={0xce,0xe3,0x24,0xa6,0x18,0xf6,0x66,0x44,0xa5,0xc6,0xd8,0xcc,0x77,0xe9,0xf6,0xd5,0x5f,0x2a,0x36,0x71,0x24,0x46,0xfc,0xaa,0x12,0xc6,0x13,0xa9,0x16,0xff,0x69,0x82};

uint8_t slot10[32] ={0x45,0x95,0xd6,0xf6,0x21,0x71,0x24,0xff,0x22,0x22,0x22,0xa3,0x75,0xf6,0x24,0xd6,0xa3,0xb6,0x84,0x78,0x89,0x34,0xd6,0x54,0x5f,0x2a,0x8d,0xe5,0x6e,0x22,0x33,0x46};

uint8_t slot11[32] ={0x13,0x74,0xef,0xfc,0x1a,0x51,0x61,0x38,0x94,0x2b,0x7b,0x56,0x69,0x43,0x27,0x73,0x81,0xa6,0x12,0xf6,0xd2,0x21,0xcb,0xbf,0xcd,0xcf,0xad,0x00,0xff,0x46,0x33,0x44};

uint8_t slot12[32] ={0x56,0xd4,0x32,0xac,0xcd,0x56,0x77,0xd9,0x46,0x17,0xed,0xd6,0xac,0x13,0x74,0x58,0x13,0xed,0x13,0xd6,0x11,0x78,0x98,0x58,0x68,0x41,0x28,0xdc,0xcd,0xca,0x13,0x12};

uint8_t slot13[32] ={0x46,0x78,0xaa,0x56,0xfc,0xed,0xef,0x2e,0x3e,0x16,0x34,0xaa,0xcd,0xac,0x83,0x17,0x16,0x54,0x83,0x27,0x19,0x94,0x67,0x49,0x35,0x44,0x87,0x53,0x46,0x24,0x35,0x68};

uint8_t slot14[32] ={0xd6,0xa3,0xd2,0xe6,0xa2,0xc6,0x34,0x24,0x86,0x98,0x37,0xdd,0x3a,0x2c,0x14,0xd6,0x2c,0x7f,0xee,0xff,0x3a,0x68,0x10,0x00,0x00,0xad,0x43,0x2e,0xd7,0xd3,0x24,0xa6};

uint8_t slot15[32] ={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
#else
uint8_t slot0[32] = {   0xED, 0x3E, 0xFF, 0xD0, 0xEF, 0xBF, 0x5B, 0xF3,
                        0xC9, 0x2C, 0xD0, 0x71, 0xB9, 0x64, 0xBC, 0x10,
                        0x6E, 0x87, 0x45, 0x0E, 0x24, 0xE9, 0x75, 0x2F,
                        0x3C, 0xF3, 0xD3, 0xEF, 0x74, 0xA5, 0xE5, 0xAA
					    };

uint8_t slot1[32] = {	0x63, 0xD9, 0xBA, 0xA3, 0x4D, 0xB8, 0x23, 0xA7,
						0xC9, 0x2C, 0x70, 0x71, 0xB9, 0x64, 0xBC, 0x10,
						0x90, 0x4B, 0xA8, 0x07, 0x86, 0xCF, 0xA8, 0xF4,
						0xF4, 0xCD, 0x93, 0x6A, 0xB6, 0x48, 0x11, 0x11
						};

uint8_t slot2[32] = {   0xD4, 0x74, 0x79, 0xF2, 0xBE, 0x15, 0xA5, 0xDF,
                        0xB1, 0x10, 0x88, 0x36, 0x9C, 0xDC, 0x4F, 0xC0,
                        0x90, 0x4B, 0xA8, 0x47, 0x86, 0xCF, 0xA8, 0xF4,
                        0xD8, 0x7F, 0x53, 0x40, 0x7B, 0x80, 0x69, 0x54
						};

uint8_t slot3[32] = {   0x1D, 0x25, 0x28, 0xE6, 0x06, 0x16, 0x27, 0x29,
                        0x51, 0x72, 0x10, 0xDB, 0x93, 0xAD, 0x40, 0x83,
                        0x63, 0xD9, 0xBA, 0xA3, 0x44, 0xB8, 0x23, 0xA7,
                        0x66, 0x82, 0x49, 0x58, 0x6C, 0x61, 0x3E, 0x56
                        };

uint8_t slot4[32] = {   0x90, 0x5E, 0x20, 0x9A, 0x9E, 0x05, 0x2C, 0x75,
                        0x97, 0x63, 0xED, 0x76, 0xE6, 0xC6, 0xB9, 0x85,
                        0x06, 0x10, 0xE1, 0x44, 0xE0, 0x7B, 0x91, 0x0D,
                        0x72, 0x6E, 0x39, 0x5A, 0x02, 0x9B, 0x2C, 0xE6
                        };

uint8_t slot5[32] = {   0xE5, 0x82, 0x31, 0x48, 0x15, 0x4B, 0x78, 0xD0,
                        0xB9, 0x49, 0x92, 0xD2, 0x11, 0x0D, 0x2D, 0x42,
                        0xC0, 0x09, 0xD7, 0x40, 0xF4, 0xAF, 0x72, 0xA3,
                        0x0F, 0x7A, 0x1B, 0x6A, 0x89, 0x2C, 0xC4, 0x7E
                        };

uint8_t slot6[32] = {   0x10, 0x28, 0x8C, 0x42, 0xD9, 0x6F, 0xC0, 0x4A,
                        0xB7, 0xE9, 0x63, 0xBD, 0xBC, 0xEA, 0x0D, 0xE6,
                        0xB5, 0x9F, 0x94, 0x90, 0xB0, 0x70, 0xCE, 0x01,
                        0xC6, 0xBB, 0x0A, 0x78, 0x6C, 0x86, 0x99, 0x3C
                        };

uint8_t slot7[32] = {   0xCF, 0xD1, 0x68, 0x00, 0xE2, 0xA5, 0x1D, 0x0F,
                        0x5C, 0xC9, 0x65, 0x8D, 0x31, 0x09, 0x20, 0xA6,
                        0x55, 0xC7, 0x6B, 0x8A, 0x5B, 0x6E, 0x68, 0xC7,
                        0x65, 0x6D, 0x36, 0x02, 0x48, 0x47, 0xAF, 0x5F
                        };

uint8_t slot8[32] = {  	0x9E, 0xCA, 0x95, 0x76, 0xC1, 0x3D, 0xA8, 0x8D,
                        0x10, 0x2B, 0xE2, 0xBD, 0xAE, 0x0C, 0xF3, 0x9B,
                        0x80, 0x42, 0x77, 0x4D, 0x8D, 0xD6, 0xA9, 0xCD,
                        0xF2, 0x25, 0xFB, 0x06, 0xD7, 0x00, 0x80, 0xD9
                        };

uint8_t slot9[32] = { 	0x02, 0xBA, 0x1F, 0x7D, 0x82, 0xC6, 0x87, 0x89,
                        0xC1, 0xDE, 0xD3, 0x2C, 0x28, 0xF1, 0xEE, 0x2D,
                        0x6D, 0xEB, 0xBF, 0x5B, 0x60, 0x89, 0x36, 0xF0,
                        0x43, 0x06, 0x98, 0xB6, 0xAB, 0xFC, 0xD5, 0x82
                        };

uint8_t slot10[32] = {  0x88, 0xB7, 0x47, 0x2C, 0x12, 0x57, 0xED, 0xEB,
                        0x9E, 0x86, 0x41, 0xAB, 0xCB, 0xD2, 0xF8, 0xF5,
                        0x88, 0xEE, 0x1C, 0xD8, 0x1A, 0xF9, 0xC7, 0x5A,
                        0x89, 0xA9, 0x1B, 0x3A, 0x1D, 0x80, 0x3E, 0xEB
                        };

uint8_t slot11[32] = {  0x77, 0xDC, 0xDD, 0x14, 0x99, 0x89, 0x28, 0xFF,
                        0x51, 0xCE, 0xEC, 0xE5, 0xE1, 0x36, 0x00, 0xE6,
                        0xBD, 0xF7, 0xCA, 0x9E, 0x62, 0x3F, 0x0E, 0x18,
                        0xCE, 0xC7, 0xF4, 0xF8, 0x85, 0xD3, 0x03, 0x16
                        };

uint8_t slot12[32] = {  0xC5, 0x48, 0xEF, 0x69, 0x19, 0x27, 0xC1, 0x17,
                        0xBE, 0xDF, 0xAC, 0x7B, 0x61, 0x72, 0xB9, 0x70,
                        0x49, 0x14, 0x12, 0x0B, 0x22, 0x06, 0xC4, 0x88,
                        0xD4, 0xE7, 0x93, 0x0E, 0x1F, 0xF5, 0xC1, 0x0A
                        };

uint8_t slot13[32] = {  0x2C, 0xAE, 0x7C, 0xC3, 0x07, 0x68, 0x80, 0x95,
                        0xCD, 0xCE, 0xFE, 0x0B, 0xD3, 0xD6, 0xCC, 0x71,
                        0x4F, 0x8D, 0xFC, 0x28, 0x96, 0x57, 0x3E, 0x40,
                        0x81, 0xC7, 0x5A, 0xB4, 0xc6, 0xF2, 0x67, 0x0E
                        };

uint8_t slot14[32] = {  0xCF, 0x8B, 0xFF, 0x91, 0x7D, 0x88, 0x69, 0x5B,
                        0xCF, 0x43, 0xFE, 0xB0, 0x01, 0x3F, 0x7A, 0x45,
                        0x52, 0x4C, 0x81, 0x19, 0xB5, 0x54, 0x67, 0xC8,
                        0x4F, 0x31, 0xF3, 0xFC, 0x5B, 0x14, 0x2D, 0x51
                        };

uint8_t slot15[32] = {  0x3F, 0x59, 0xA1, 0x4B, 0x04, 0xA3, 0x3F, 0xA2,
						0x0E, 0xCF, 0xFA, 0x2B, 0x13, 0xF8, 0x8B, 0x2A,
						0xDC, 0x0F, 0xB4, 0x98, 0x70, 0x32, 0x5D, 0x23,
						0x79, 0x02, 0x0E, 0x62, 0x16, 0x9E, 0x33, 0x2F
						};	

#endif

#if 1
uint8_t config_zone[68] =
{
    0xc8, 0x00, 0x55, 0x00,  //I2C_Addr  CheckMacConfig  OTP_Mode  SelectorMode

    0x8f, 0x80, 0x80, 0xa1,  //SlotConfig  0  1
    0x82, 0xe0, 0xa3, 0x60,  //SlotConfig  2  3
    0x94, 0x40, 0xa0, 0x85,  //SlotConfig  4  5
    0x86, 0x40, 0x87, 0x07,  //SlotConfig  6  7
    0x0f, 0x00, 0x89, 0xf2,  //SlotConfig  8  9
    0x8a, 0x7a, 0x0b, 0x8b,  //SlotConfig 10 11
    0x0c, 0x4c, 0x0d, 0x4d,  //SlotConfig 12 13
    0xc2, 0x42, 0xaf, 0x8f,  //SlotConfig 14 15

    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 0 1
    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 2 3
    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 4 5
    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 6 7

    0xff, 0xff, 0xff, 0xff,  //LastKeyUse  0 - 3
    0xff, 0xff, 0xff, 0xff,  //LastKeyUse  4 - 7
    0xff, 0xff, 0xff, 0xff,  //LastKeyUse  8 -11
    0xff, 0xff, 0xff, 0xff   //LastKeyUse 12 -15
};
#else
uint8_t config_zone[68] =
{
    0xc8, 0x00, 0x55, 0x00,  //I2C_Addr  CheckMacConfig  OTP_Mode  SelectorMode

    0x80, 0x80, 0x80, 0x80,  //SlotConfig  0  1  //data锁定后不可读，也不可写
    0x80, 0x80, 0x80, 0x80,  //SlotConfig  2  3  //data锁定后不可读，也不可写
    0x00, 0x00, 0x00, 0x00,  //SlotConfig  4  5  //data锁定可明文读，也可写
    0x00, 0x00, 0x00, 0x00,  //SlotConfig  6  7
    0x00, 0x00, 0x00, 0x00,  //SlotConfig  8  9
    0x00, 0x00, 0x00, 0x00,  //SlotConfig 10 11
    0x00, 0x00, 0x00, 0x00,  //SlotConfig 12 13
    0x00, 0x00, 0x00, 0x00,  //SlotConfig 14 15

    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 0 1
    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 2 3
    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 4 5
    0xff, 0x00, 0xff, 0x00,  //UseFlag UpdateCount 6 7

    0xff, 0xff, 0xff, 0xff,  //LastKeyUse  0 - 3
    0xff, 0xff, 0xff, 0xff,  //LastKeyUse  4 - 7
    0xff, 0xff, 0xff, 0xff,  //LastKeyUse  8 -11
    0xff, 0xff, 0xff, 0xff   //LastKeyUse 12 -15
};	
#endif

uint8_t otp_zone[64] =
{
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,  
    0xff, 0xff, 0xff, 0xff,  
    0xff, 0xff, 0xff, 0xff,
	
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,  
    0xff, 0xff, 0xff, 0xff,  
    0xff, 0xff, 0xff, 0xff
};

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

#define UNPACK32(x, str)                      \
{                                             \
    *((str) + 3) = (uint8_t) ((x)      );       \
    *((str) + 2) = (uint8_t) ((x) >>  8);       \
    *((str) + 1) = (uint8_t) ((x) >> 16);       \
    *((str) + 0) = (uint8_t) ((x) >> 24);       \
}

#define PACK32(str, x)                        \
{                                             \
    *(x) =   ((uint32_t) *((str) + 3)      )    \
           | ((uint32_t) *((str) + 2) <<  8)    \
           | ((uint32_t) *((str) + 1) << 16)    \
           | ((uint32_t) *((str) + 0) << 24);   \
}


/* Macros used for loops unrolling */

#define SHA256_SCR(i)                         \
{                                             \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]  \
          + SHA256_F3(w[i - 15]) + w[i - 16]; \
}

#define SHA256_EXP(a, b, c, d, e, f, g, h, j)               \
{                                                           \
    t1 = wv[h] + SHA256_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
         + sha256_k[j] + w[j];                              \
    t2 = SHA256_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[h] = t1 + t2;                                        \
}

//flash uint32_t sha256_h0[8] =
uint32_t sha256_h0[8] =
            {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

//flash uint32_t sha256_k[64] =
uint32_t sha256_k[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/* SHA-256 functions */

// some helper macros for TPM implementation
//#define zram32  ((uint32_t*) zram)
//#define eob32(buf) ((sizeof(buf)/sizeof(uint32_t)))

void sha256_transf(sha256_ctx *ctx, const uint8_t *message,
                   uint32_t block_nb)
{
    uint32_t w[64];
    uint32_t wv[8];
//	uint32_t *wv = &zram32[eob32(zram)-8];
//	uint32_t *w = &zram32[eob32(zram)-(8+64)];

    uint32_t t1, t2;
    const uint8_t *sub_block;
    int i;

#ifndef UNROLL_LOOPS
    int j;
#endif

    for (i = 0; i < (int) block_nb; i++) {
        sub_block = message + (i << 6);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++) {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            ctx->h[j] += wv[j];
        }
#else
        PACK32(&sub_block[ 0], &w[ 0]); PACK32(&sub_block[ 4], &w[ 1]);
        PACK32(&sub_block[ 8], &w[ 2]); PACK32(&sub_block[12], &w[ 3]);
        PACK32(&sub_block[16], &w[ 4]); PACK32(&sub_block[20], &w[ 5]);
        PACK32(&sub_block[24], &w[ 6]); PACK32(&sub_block[28], &w[ 7]);
        PACK32(&sub_block[32], &w[ 8]); PACK32(&sub_block[36], &w[ 9]);
        PACK32(&sub_block[40], &w[10]); PACK32(&sub_block[44], &w[11]);
        PACK32(&sub_block[48], &w[12]); PACK32(&sub_block[52], &w[13]);
        PACK32(&sub_block[56], &w[14]); PACK32(&sub_block[60], &w[15]);

        SHA256_SCR(16); SHA256_SCR(17); SHA256_SCR(18); SHA256_SCR(19);
        SHA256_SCR(20); SHA256_SCR(21); SHA256_SCR(22); SHA256_SCR(23);
        SHA256_SCR(24); SHA256_SCR(25); SHA256_SCR(26); SHA256_SCR(27);
        SHA256_SCR(28); SHA256_SCR(29); SHA256_SCR(30); SHA256_SCR(31);
        SHA256_SCR(32); SHA256_SCR(33); SHA256_SCR(34); SHA256_SCR(35);
        SHA256_SCR(36); SHA256_SCR(37); SHA256_SCR(38); SHA256_SCR(39);
        SHA256_SCR(40); SHA256_SCR(41); SHA256_SCR(42); SHA256_SCR(43);
        SHA256_SCR(44); SHA256_SCR(45); SHA256_SCR(46); SHA256_SCR(47);
        SHA256_SCR(48); SHA256_SCR(49); SHA256_SCR(50); SHA256_SCR(51);
        SHA256_SCR(52); SHA256_SCR(53); SHA256_SCR(54); SHA256_SCR(55);
        SHA256_SCR(56); SHA256_SCR(57); SHA256_SCR(58); SHA256_SCR(59);
        SHA256_SCR(60); SHA256_SCR(61); SHA256_SCR(62); SHA256_SCR(63);

        wv[0] = ctx->h[0]; wv[1] = ctx->h[1];
        wv[2] = ctx->h[2]; wv[3] = ctx->h[3];
        wv[4] = ctx->h[4]; wv[5] = ctx->h[5];
        wv[6] = ctx->h[6]; wv[7] = ctx->h[7];

        SHA256_EXP(0,1,2,3,4,5,6,7, 0); SHA256_EXP(7,0,1,2,3,4,5,6, 1);
        SHA256_EXP(6,7,0,1,2,3,4,5, 2); SHA256_EXP(5,6,7,0,1,2,3,4, 3);
        SHA256_EXP(4,5,6,7,0,1,2,3, 4); SHA256_EXP(3,4,5,6,7,0,1,2, 5);
        SHA256_EXP(2,3,4,5,6,7,0,1, 6); SHA256_EXP(1,2,3,4,5,6,7,0, 7);
        SHA256_EXP(0,1,2,3,4,5,6,7, 8); SHA256_EXP(7,0,1,2,3,4,5,6, 9);
        SHA256_EXP(6,7,0,1,2,3,4,5,10); SHA256_EXP(5,6,7,0,1,2,3,4,11);
        SHA256_EXP(4,5,6,7,0,1,2,3,12); SHA256_EXP(3,4,5,6,7,0,1,2,13);
        SHA256_EXP(2,3,4,5,6,7,0,1,14); SHA256_EXP(1,2,3,4,5,6,7,0,15);
        SHA256_EXP(0,1,2,3,4,5,6,7,16); SHA256_EXP(7,0,1,2,3,4,5,6,17);
        SHA256_EXP(6,7,0,1,2,3,4,5,18); SHA256_EXP(5,6,7,0,1,2,3,4,19);
        SHA256_EXP(4,5,6,7,0,1,2,3,20); SHA256_EXP(3,4,5,6,7,0,1,2,21);
        SHA256_EXP(2,3,4,5,6,7,0,1,22); SHA256_EXP(1,2,3,4,5,6,7,0,23);
        SHA256_EXP(0,1,2,3,4,5,6,7,24); SHA256_EXP(7,0,1,2,3,4,5,6,25);
        SHA256_EXP(6,7,0,1,2,3,4,5,26); SHA256_EXP(5,6,7,0,1,2,3,4,27);
        SHA256_EXP(4,5,6,7,0,1,2,3,28); SHA256_EXP(3,4,5,6,7,0,1,2,29);
        SHA256_EXP(2,3,4,5,6,7,0,1,30); SHA256_EXP(1,2,3,4,5,6,7,0,31);
        SHA256_EXP(0,1,2,3,4,5,6,7,32); SHA256_EXP(7,0,1,2,3,4,5,6,33);
        SHA256_EXP(6,7,0,1,2,3,4,5,34); SHA256_EXP(5,6,7,0,1,2,3,4,35);
        SHA256_EXP(4,5,6,7,0,1,2,3,36); SHA256_EXP(3,4,5,6,7,0,1,2,37);
        SHA256_EXP(2,3,4,5,6,7,0,1,38); SHA256_EXP(1,2,3,4,5,6,7,0,39);
        SHA256_EXP(0,1,2,3,4,5,6,7,40); SHA256_EXP(7,0,1,2,3,4,5,6,41);
        SHA256_EXP(6,7,0,1,2,3,4,5,42); SHA256_EXP(5,6,7,0,1,2,3,4,43);
        SHA256_EXP(4,5,6,7,0,1,2,3,44); SHA256_EXP(3,4,5,6,7,0,1,2,45);
        SHA256_EXP(2,3,4,5,6,7,0,1,46); SHA256_EXP(1,2,3,4,5,6,7,0,47);
        SHA256_EXP(0,1,2,3,4,5,6,7,48); SHA256_EXP(7,0,1,2,3,4,5,6,49);
        SHA256_EXP(6,7,0,1,2,3,4,5,50); SHA256_EXP(5,6,7,0,1,2,3,4,51);
        SHA256_EXP(4,5,6,7,0,1,2,3,52); SHA256_EXP(3,4,5,6,7,0,1,2,53);
        SHA256_EXP(2,3,4,5,6,7,0,1,54); SHA256_EXP(1,2,3,4,5,6,7,0,55);
        SHA256_EXP(0,1,2,3,4,5,6,7,56); SHA256_EXP(7,0,1,2,3,4,5,6,57);
        SHA256_EXP(6,7,0,1,2,3,4,5,58); SHA256_EXP(5,6,7,0,1,2,3,4,59);
        SHA256_EXP(4,5,6,7,0,1,2,3,60); SHA256_EXP(3,4,5,6,7,0,1,2,61);
        SHA256_EXP(2,3,4,5,6,7,0,1,62); SHA256_EXP(1,2,3,4,5,6,7,0,63);

        ctx->h[0] += wv[0]; ctx->h[1] += wv[1];
        ctx->h[2] += wv[2]; ctx->h[3] += wv[3];
        ctx->h[4] += wv[4]; ctx->h[5] += wv[5];
        ctx->h[6] += wv[6]; ctx->h[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}

void sha256_init(sha256_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    int i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha256_h0[i];
    }
#else
    ctx->h[0] = sha256_h0[0]; ctx->h[1] = sha256_h0[1];
    ctx->h[2] = sha256_h0[2]; ctx->h[3] = sha256_h0[3];
    ctx->h[4] = sha256_h0[4]; ctx->h[5] = sha256_h0[5];
    ctx->h[6] = sha256_h0[6]; ctx->h[7] = sha256_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

void sha256_update(sha256_ctx *ctx, const uint8_t *message,
                   uint32_t len)
{
    uint32_t block_nb;
    uint32_t new_len, rem_len, tmp_len;
    const uint8_t *shifted_message;

    tmp_len = SHA256_BLOCK_SIZE - ctx->len;
    rem_len = len < tmp_len ? len : tmp_len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA256_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA256_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha256_transf(ctx, ctx->block, 1);
    sha256_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA256_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

void sha256_final(sha256_ctx *ctx, uint8_t *digest)
{
    uint32_t block_nb;
    uint32_t pm_len;
    uint32_t len_b;

#ifndef UNROLL_LOOPS
    int i;
#endif

    block_nb = (1 + ((SHA256_BLOCK_SIZE - 9)
                     < (ctx->len % SHA256_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha256_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 8; i++) {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
#else
   UNPACK32(ctx->h[0], &digest[ 0]);
   UNPACK32(ctx->h[1], &digest[ 4]);
   UNPACK32(ctx->h[2], &digest[ 8]);
   UNPACK32(ctx->h[3], &digest[12]);
   UNPACK32(ctx->h[4], &digest[16]);
   UNPACK32(ctx->h[5], &digest[20]);
   UNPACK32(ctx->h[6], &digest[24]);
   UNPACK32(ctx->h[7], &digest[28]);
#endif /* !UNROLL_LOOPS */
}


int main(int argc,char *argv[])
{
	int fd;
	int cmd;
    int arg = 0;
	int i = 0;
	int len = 0x58;
	sha256_ctx ctx;
	uint8_t Digest[32] = {0};
	uint8_t MsgBuf[128] = {0};
	uint32_t MsgBuf_updates  = 1000000/sizeof(MsgBuf);
	
	
	//7.调用官方库文件带的SHA256算法计算出自己的Digest
	
	memcpy(MsgBuf, slot0 , 32);//key<SlotID>; 32byte:Challenge or TempKey,这里是Challenge;
	memcpy(MsgBuf + 32, in_challenge , 32); //32byte:Challenge or TempKey,这里是Challenge;
	MsgBuf[64] = 0x08;
	MsgBuf[65] = 0x00;
	MsgBuf[66] = 0x01;
	MsgBuf[67] = 0x00;
	
	MsgBuf[79] = 0xee;//SN[8] 0xee
	
	MsgBuf[84] = 0x01;//SN[0] 0x01
	MsgBuf[85] = 0x23;//SN[1] 0x23
	
	
	sha256_init(&ctx);
	//for(i=0; i < MsgBuf_updates; i++)
		sha256_update(&ctx, MsgBuf, len);
	sha256_final(&ctx, Digest);
	for(i = 0;i < 32;i++){
		printf(">>>>>> atsha204a digest is 0x%02x\n",Digest[i]);
		if(cmd_mac.out_response[i+1] != Digest[i]){
			printf(">>>>>>> atsha204a burn fail <<<<<<<<<<<<\n");
			if(i == 31){
				printf(">>>>>>> atsha204a burn success <<<<<<<<<<<<\n");
			}
		}
	}
	
	return 0;
}


/* --------------------------------------- End of File --------------------------------*/
