#ifndef  _EXPANDBORAD_H_
#define  _EXPANDBORAD_H_
#include "DataType.h"


#define STATION_ID           1
#define KEY_ID               1
#define PRINT_ID             2

#define KEY_CMD              1
#define METER_CMD            2
#define PRINT_CMD            3


typedef struct 
{
	ST_UCHAR SendKeyData[50];     //存放发送按键数据域内容
	ST_UCHAR SendPrintData[200];  //存放发送打印数据域内容
	ST_UCHAR SenDataLen;          //发送数据域长度
	ST_UCHAR RcvKeyData[50];     //存放接收按键数据域内容
	ST_UCHAR RcvPrintData[200];  //存放接收打印数据域内容	
	ST_UCHAR RcvDataLen;         //接收数据域长度
	
	ST_CHAR  SendCmd;         //当前轮询发送的命令，1：键盘；2：电表；3：打印
	ST_CHAR  SendCount;     //发送次数计数
	ST_CHAR  SendFinishFlag; //数据发送完标志
	
	
}ExBoardMsgTP;




void InitExpandBoard(void);

          


























#endif
