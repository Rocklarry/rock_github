#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "AlarmRecord.h"
#include "convertion.h"
#include "sql.h"

static AlarmRecordTP AlarmRecord;            //保存当前的告警记录
static ST_BOOL  s_NewAlarmRecordFlag = false; //标记是否发生新记录
static ST_BOOL  s_NewAlarmRecordFlag_Gprs = false; //标记是否发生新记录

/********************************************************************************* 
 函数名称： IsNewAlarmRecord
 功能描述：	检查是否有告警记录
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS：有告警记录，SD_FAILURE：无告警记录
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

int IsNewAlarmRecord(void)
{
	if (s_NewAlarmRecordFlag) return SD_SUCCESS;
	
	return SD_FAILURE;
}

/********************************************************************************* 
 函数名称： IsNewAlarmRecord
 功能描述：	检查是否有告警记录
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS：有告警记录，SD_FAILURE：无告警记录
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

int IsNewAlarmRecord_Gprs(void)
{
	if (s_NewAlarmRecordFlag_Gprs) return SD_SUCCESS;
	
	return SD_FAILURE;
}

/********************************************************************************* 
 函数名称： InitNewAlarmRecord
 功能描述：	初始化告警记录
 输    入：	MeaPointNo：测量点编号，AlarmCode：告警编码，AlarmContent：告警内容，AlarmContentlen，告警内容长度
 输	   出： 无
 返 回 值： SD_SUCCESS：成功初始化
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

int InitNewAlarmRecord(ST_CHAR MeaPointNo,ST_INT32 AlarmCode,ST_UCHAR *AlarmContent,ST_CHAR AlarmContentlen)
{
	struct tm *stShowNowTime;
	time_t now;
	time(&now);
	stShowNowTime = localtime(&now);

	AlarmRecord.MeaPointNum = MeaPointNo;
	
	AlarmRecord.Time[0] = CDZ_HEXTOBCD((ST_UCHAR)(stShowNowTime->tm_year % 100));
	AlarmRecord.Time[1] = CDZ_HEXTOBCD((ST_UCHAR)(stShowNowTime->tm_mon+1));
	AlarmRecord.Time[2] = CDZ_HEXTOBCD(((ST_UCHAR)stShowNowTime->tm_mday));
	AlarmRecord.Time[3] = CDZ_HEXTOBCD((ST_UCHAR)(stShowNowTime->tm_hour));
	AlarmRecord.Time[4] = CDZ_HEXTOBCD((ST_UCHAR)(stShowNowTime->tm_min));
	
	AlarmRecord.AlarmCode[0] = (ST_UCHAR)AlarmCode;
	AlarmRecord.AlarmCode[1] = (ST_UCHAR)(AlarmCode>>8);
	printf("month is%2x\n",stShowNowTime->tm_mon+1);
	int j;
	printf("paracontent is\n");
	for(j=0;j<AlarmContentlen;j++)
	{
		printf("%2X\n",AlarmContent[j]);
	}
	printf("AlarmCode is %4x\n",AlarmCode);
	printf("AlarmCode is %2x\n",AlarmRecord.AlarmCode[0]);
	printf("AlarmCode is %2x\n",AlarmRecord.AlarmCode[1]);
	printf("AlarmRecord.AlarmContentLen is %2X \n",AlarmContentlen);
	if(AlarmContentlen!=0)
	{
		AlarmRecord.AlarmContentLen =  AlarmContentlen;
		memcpy(AlarmRecord.AlarmContent,AlarmContent,AlarmContentlen);
	}  
	else
	{
		AlarmRecord.AlarmContentLen = 0;
		memset(AlarmRecord.AlarmContent,0,sizeof(AlarmRecord.AlarmContent));
		
	}
	 printf("AlarmRecord.AlarmContentLen is %2X \n",AlarmRecord.AlarmContentLen);

	printf("AlarmRecord.AlarmContent\n");
	int i;
	for(i=0;i<AlarmContentlen;i++)
	{
		printf("%2x\n",AlarmRecord.AlarmContent[i]);
	}
	insertAlarmRecord(&AlarmRecord, "alarm");
//	rm_alarm_db();
//	init_alarm_db("alarm.db");	
	
	s_NewAlarmRecordFlag = true;
	s_NewAlarmRecordFlag_Gprs = true;
	return SD_SUCCESS;
}


/********************************************************************************* 
 函数名称： GetNewAlarmRecord
 功能描述：	获取一条新告警记录
 输    入： pAlarmRecord：当前告警记录
 输	   出： 无
 返 回 值： SD_SUCCESS：得到一条新纪录，SD_FAILURE：当前没有新纪录产生
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/



ST_RET GetNewAlarmRecord(AlarmRecordTP *pAlarmRecord)
{
	if (s_NewAlarmRecordFlag == false)  return SD_FAILURE;


	*pAlarmRecord = AlarmRecord;

	return SD_SUCCESS;
}


/********************************************************************************* 
 功能描述：	获取一条新告警记录
 输    入： pAlarmRecord：当前告警记录
 输	   出： 无
 返 回 值： SD_SUCCESS：得到一条新纪录，SD_FAILURE：当前没有新纪录产生
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/



ST_RET GetNewAlarmRecord_Gprs(AlarmRecordTP *pAlarmRecord)
{
	if (s_NewAlarmRecordFlag_Gprs == false)  return SD_FAILURE;


	*pAlarmRecord = AlarmRecord;

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： ConfirmAlarmRecord
 功能描述：	告警确认
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ConfirmAlarmRecord(void)
{
	s_NewAlarmRecordFlag = false;

}

/********************************************************************************* 
 函数名称： ConfirmAlarmRecord
 功能描述：	告警确认
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ConfirmAlarmRecord_Gprs(void)
{
	s_NewAlarmRecordFlag_Gprs = false;

}

/********************************************************************************* 
 函数名称： GetAlarmRecord
 功能描述：	从告警记录中查找符合条件的记录
 输    入： time：告警发生时间，AlarmCode：告警编码，pAlarmRecord：告警记录，num：告警数量
 输	   出： SD_SUCCESS：找到相应的记录，SD_FAILURE：未找到符合条件的记录
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_RET GetAlarmRecord(ST_UCHAR *time,const ST_UCHAR *AlarmCode, AlarmRecordTP *pAlarmRecord,ST_INT32 *num)
{
	*num = SearchAlarmRecord((ST_CHAR *)time, AlarmCode, pAlarmRecord, *num, "alarm");

	if(*num<0) return SD_FAILURE;
	return SD_SUCCESS;
}
