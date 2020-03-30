/*************************************************************************
	> File Name: test.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年01月14日 星期二 15时59分24秒
 ************************************************************************/

#include<stdio.h>

#define SGP_CMD(cmd_word)			cpu_to_be16(cmd_word)


#define   LCM_DSI_CMD_MODE	 0

int main()
{

	typedef unsigned long   u32t;
	unsigned char buffer[]={
							#if 1
							0x43,0xDB,
							0x8C,0x2E,
							0x41,0xD9,
							0xE7,0xFF,
							0x42,0x43,
							0x3A,0x1B
							#else 
							0x00,0xAF, 
							0x70,0x26, 
							0x09,0x6F, 
							0x81,0x00, 
							0x6B,0xD0, 
							0x7A,0x24
							#endif
							};




	unsigned long tempU32;


	float co2Concentration; 
	float temperature;
	float humidity;

  tempU32 = (u32t)((((u32t)buffer[0]) << 24) | (((u32t)buffer[1]) << 16) | (((u32t)buffer[2]) << 8) | ((u32t)buffer[3]));
  co2Concentration = *(float*)&tempU32;
  tempU32 = (u32t)((((u32t)buffer[4]) << 24) | (((u32t)buffer[5]) << 16) | (((u32t)buffer[6]) << 8) | ((u32t)buffer[7]));
  temperature = *(float*)&tempU32;
  tempU32 = (u32t)((((u32t)buffer[8]) << 24) | (((u32t)buffer[9]) << 16) | (((u32t)buffer[10]) << 8) | ((u32t)buffer[11]));
  humidity = *(float*)&tempU32;
	
	printf("\b\b co2Concentration=%.2f  temperature=%.2f   humidity=%.2f \n\n",co2Concentration,temperature,humidity);


#if (LCM_DSI_CMD_MODE)
	printf("\b\b  test=====  LCM_DSI_CMD_MODE  \n\n");
#else
	printf("\b\b  *************** LCM_DSI_CMD_MODE  \n\n");
#endif

/*
// CO2 concentration 
float co2Concentration; 
unsigned int tempU32; 
 
// read data is in a buffer. In case of I2C CRCs have been removed    
// beforehand. Content of the buffer is the following 
unsigned char buffer[4]; 
	

buffer[0] = 0x43; //  MMSB CO2  
buffer[1] = 0xDB; //  MLSB CO2  
buffer[2] = 0x8C; //  LMSB CO2  
buffer[3] = 0x2E; //  LLSB CO2 
 
// cast 4 bytes to one unsigned 32 bit integer 
tempU32 = (unsigned int)((((unsigned int)buffer[0]) << 24) |  
                         (((unsigned int)buffer[1]) << 16) |  
                         (((unsigned int)buffer[2]) << 8)  |  
                          ((unsigned int)buffer[3])); 
 
// cast unsigned 32 bit integer to 32 bit float 
co2Concentration = *(float*)&tempU32; // co2Concentration = 439.09f  
printf("\b\b co2Concentration=%f   \n\n",co2Concentration);
*/     


return 0;
}


