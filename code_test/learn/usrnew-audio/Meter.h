#ifndef  _METER_H_
#define _METER_H_
#include "DataType.h"

#define METER_DUANLU_LIMIT                 350

#define ACVoltNormal                        1
#define ACVoltLow                           2 
#define ACVoltHigh                          3


#define GETADDR                             0
#define READCUR_A                           1
#define READCUR_B                           2
#define READCUR_C                           3
#define READVOL_A                           4
#define READVOL_B                           5
#define READVOL_C                           6
#define READTOTAL_POWER                     7
#define READ_POWER1                         8
#define READ_POWER2                         9
#define READ_POWER3                        10
#define READ_POWER4                        11
#define READ_STATUS_A                      12
#define READ_STATUS_B                      13
#define READ_STATUS_C                      14
#define READ_STATUS_97                     15

#define READ_CURENTTIME_SMH                16
#define READ_RATE_TO_TIME                  17

#define READ_CURENTTIME_WDMY               18



#define METER_STX                          0x68
#define METER_ETX                          0x16   


#define WXMK				0x0102			// 维修门开
#define KSCD				0x0103			// 开始充电
#define CZMGMGZ				0x0104			// 插座门关门故障
#define DBDSGZ				0x0105			// 电表对时故障
#define BYDCDLD				0x0130			// 备用电池电量低
#define DBTXGZ				0x0138			// 电表通讯故障
#define ZDTD				0x0141			// 终端停电
#define DBTXGZHF			0x01B8			// 电表通讯故障恢复
#define ZDLD				0x01C1			// 终端来电


#define CUR_AEXCESS			0x0143			// A相电流过负荷
#define CUR_BEXCESS			0x0144			// B相电流过负荷
#define CUR_CEXCESS			0x0145			// C相电流过负荷

#define AXDX				0x0146			// A相断相
#define BXDX				0x0147			// B相断相
#define CXDX				0x0148			// C相断相

#define VOL_AEXCESS			0x0170			// A相电压过压
#define VOL_BEXCESS			0x0171			// B相电压过压
#define VOL_CEXCESS			0x0172			// C相电压过压

#define VOL_ALOW			0x0173			// A相电压欠压
#define VOL_BLOW			0x0174			// B相电压欠压
#define VOL_CLOW			0x0175			// C相电压欠压




#define CUR_AEXCESSHF		0x01C3			// A相电流过负荷恢复
#define CUR_BEXCESSHF		0x01C4			// B相电流过负荷恢复
#define CUR_CEXCESSHF		0x01C5			// C相电流过负荷恢复

#define AXDXHF				0x01C6			// A相断相恢复
#define BXDXHF				0x01C7			// B相断相恢复
#define CXDXHF				0x01C8			// C相断相恢复

#define VOL_AEXCESSHF		0x01F0			// A相电压过压恢复
#define VOL_BEXCESSHF		0x01F1			// B相电压过压恢复
#define VOL_CEXCESSHF		0x01F2			// C相电压过压恢复

#define VOL_ALOWHF			0x01F3			// A相电压欠压恢复
#define VOL_BLOWHF			0x01F4			// B相电压欠压恢复
#define VOL_CLOWHF			0x01F5			// C相电压欠压恢复

#define PHASE_A_DUANLU		0x01F6			// A相短路
#define PHASE_B_DUANLU		0x01F7			// B相短路
#define PHASE_C_DUANLU		0x01F8			// C相短路

#define PHASE_A_DUANLU_HF		0x01F6			// A相短路
#define PHASE_B_DUANLU_HF		0x01F7			// B相短路
#define PHASE_C_DUANLU_HF		0x01F8			// C相短路

typedef struct 
{
	ST_CHAR RateIndex;
	ST_CHAR RateToTime_Min;
	ST_CHAR RateToTime_Hour;	
	
}RateToTimeTP;


typedef struct 
{	
	ST_FLOAT fVol_A;							// A相电压 XXX.X
	ST_FLOAT fVol_B;							// B相电压 XXX.X
	ST_FLOAT fVol_C;							// C相电压 XXX.X

	ST_FLOAT fCur_A;							// A相电流 XXX.XXX
	ST_FLOAT fCur_B;							// B相电流 XXX.XXX
	ST_FLOAT fCur_C;							// C相电流 XXX.XXX
	
	ST_FLOAT fPositiveTotalPower;				// 正向有功总电能 XXXXXX.XX
	ST_FLOAT fPositivePower1;					// 正向有功费率1电能 XXXXXX.XX
	ST_FLOAT fPositivePower2;					// 正向有功费率2电能 XXXXXX.XX
	ST_FLOAT fPositivePower3;					// 正向有功费率3电能 XXXXXX.XX
	ST_FLOAT fPositivePower4;					// 正向有功费率4电能 XXXXXX.XX

	ST_FLOAT fAlarmVol_AExcessPoint;			// A相电压过压门限	
	ST_FLOAT fAlarmVol_BExcessPoint;			// B相电压过压门限	
	ST_FLOAT fAlarmVol_CExcessPoint;			// C相电压过压门限

	ST_FLOAT fAlarmCur_AExcessPoint;			// A相电流过流门限
	ST_FLOAT fAlarmCur_BExcessPoint;			// B相电流过流门限
	ST_FLOAT fAlarmCur_CExcessPoint;			// C相电流过流门限

	ST_FLOAT fAlarmVol_ALowPoint;				// A相电压欠压门限	
	ST_FLOAT fAlarmVol_BLowPoint;				// B相电压欠压门限	
	ST_FLOAT fAlarmVol_CLowPoint;				// C相电压欠压门限

	ST_FLOAT fAlarmCur_ALowPoint;				// A相电流欠流门限
	ST_FLOAT fAlarmCur_BLowPoint;				// B相电流欠流门限
	ST_FLOAT fAlarmCur_CLowPoint;				// C相电流欠流门限
	
	ST_CHAR bGetVol_ACommand[14];					// 抄读A相电压的命令帧
	ST_CHAR bGetVol_BCommand[14];					// 抄读B相电压的命令帧
	ST_CHAR bGetVol_CCommand[14];					// 抄读C相电压的命令帧
	ST_CHAR bGetCur_ACommand[14];					// 抄读A相电流的命令帧
	ST_CHAR bGetCur_BCommand[14];					// 抄读B相电流的命令帧
	ST_CHAR bGetCur_CCommand[14];					// 抄读C相电流的命令帧
	ST_CHAR bGetPositiveTotalPowerCommand[14];		// 抄读正向有功总电能的命令帧
	ST_CHAR bGetPositivePower1Command[14];			// 抄读A正向有功费率1电能的命令帧
	ST_CHAR bGetPositivePower2Command[14];			// 抄读A正向有功费率2电能的命令帧
	ST_CHAR bGetPositivePower3Command[14];			// 抄读A正向有功费率3电能的命令帧
	ST_CHAR bGetPositivePower4Command[14];			// 抄读A正向有功费率4电能的命令帧
	ST_CHAR bGetCutPaseErrCommand[14];				// 抄读故障状态字命令帧
	
	ST_CHAR bAlarmFlag[12];						// 报警标识 (重复报警)
	ST_CHAR SendCount;
	ST_UCHAR MeterCmd;
	ST_UCHAR bAddrCmd[12];
	ST_UCHAR bAddress[6];
	ST_UCHAR Rcvbuffer[100];
	ST_UCHAR bBaud;
	RateToTimeTP RateToTime[14];//存放电表当前费率号所对应的时间 nn mm hh 费率号，分，时
	ST_CHAR MeterCurrentTime[7];//存放电表的当前时间，ss mm hh 周 日月 年

	ST_UCHAR Rcvfinishflag;
	
}MeterTP;

extern MeterTP MeterMsg;
extern ST_INT16 DefalutPhaseFlag;

void InitMeter(void);

unsigned char  DealOver_Low_Vol(const float fACVolt,ST_INT32 VolPhase);


ST_INT32 GetProtocolType(void);
void MeterComRcv(void);
void MeterComSend(void);








#endif
