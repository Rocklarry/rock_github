#ifndef _MYCOM_H_ 
#define _MYCOM_H_
#include "DataType.h"


#define NUM                   4

#define EXBOARDCOM            0
#define CARDCOM               1
#define SCREENCOM             2
#define GPRSWAI               3
#define Com_Delay			  10



typedef struct 
{
	
	ST_INT32  rxtime;      // 接收时间
	ST_INT32  rxtimelong;  // 判断一帧接收完成时间长度	
	ST_INT32  rxnum;       // 接收的字节数
	ST_UCHAR  revbuf[200];    // 接收数据
	ST_CHAR rxfinish;

	
	ST_INT32  txtime;      // 接收时间
	ST_INT32  txtimelong;  // 判断一帧接收完成时间长度	
	ST_UCHAR  sendbuf[200];    // 发送数据
	ST_INT32  Comfd;
	
	
}UARTMODE;


void set_speed(ST_INT32 fd, ST_INT32 speed);
ST_INT32 set_Parity(ST_INT32 fd,ST_INT32 databits,ST_INT32 stopbits,ST_INT32 parity);

void OpenCom(ST_CHAR *Dev);

void InitCom(void);
ssize_t ReadCom(ST_INT32 fd, ST_UCHAR *buf, size_t nbytes, ST_UINT32 timout);

extern ST_CHAR *ExBoardCom;
extern ST_CHAR *CardCom;
extern ST_CHAR *ScreenCom ;
extern ST_CHAR *GPRSCom ;
extern UARTMODE Uart[NUM];
extern ST_INT32 ExBoardfd;
extern ST_INT32 Cardfd;
extern ST_INT32 Screenfd;
extern ST_INT32 GPRSfd;

#endif
