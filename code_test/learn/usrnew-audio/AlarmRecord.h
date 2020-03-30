#ifndef __ALARMRECORD_H__
#define __ALARMRECORD_H__

#include "AlarmRecord.h"

#include "DataType.h"

typedef struct
{
	ST_CHAR SendFlag;               //上传标记
	ST_CHAR MeaPointNum;            //告警发生测量点号（TN）
	ST_UCHAR Time[5];             //告警发生时间（Year）（Mon）（Day）（Hour）（Min）
	ST_UCHAR AlarmCode[2];            //告警编码
	ST_UCHAR AlarmContent[15];      //告警参数,存放ABC三相电压和三相电流 电压：NNN.N,电流:NNN.NNN	
	ST_UCHAR AlarmContentLen;       //告警参数长度，为0时则不带参数
														
}AlarmRecordTP;

int InitNewAlarmRecord(ST_CHAR MeaPointNo,ST_INT32 AlarmCode,ST_UCHAR *AlarmContent,ST_CHAR AlarmContentlen);
ST_RET GetNewAlarmRecord(AlarmRecordTP *pAlarmRecord);
void ConfirmAlarmRecord(void);
ST_RET GetAlarmRecord(ST_UCHAR *time,const ST_UCHAR *AlarmCode, AlarmRecordTP *pChargeRecord,ST_INT32 *num);
int IsNewAlarmRecord_Gprs(void);
ST_RET GetNewAlarmRecord_Gprs(AlarmRecordTP *pAlarmRecord);
void ConfirmAlarmRecord_Gprs(void);
int IsNewAlarmRecord(void);


#endif

