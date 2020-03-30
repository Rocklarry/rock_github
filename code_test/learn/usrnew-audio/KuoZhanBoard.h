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
	ST_UCHAR SendKeyData[50];     //��ŷ��Ͱ�������������
	ST_UCHAR SendPrintData[200];  //��ŷ��ʹ�ӡ����������
	ST_UCHAR SenDataLen;          //���������򳤶�
	ST_UCHAR RcvKeyData[50];     //��Ž��հ�������������
	ST_UCHAR RcvPrintData[200];  //��Ž��մ�ӡ����������	
	ST_UCHAR RcvDataLen;         //���������򳤶�
	
	ST_CHAR  SendCmd;         //��ǰ��ѯ���͵����1�����̣�2�����3����ӡ
	ST_CHAR  SendCount;     //���ʹ�������
	ST_CHAR  SendFinishFlag; //���ݷ������־
	
	
}ExBoardMsgTP;




void InitExpandBoard(void);

          


























#endif
