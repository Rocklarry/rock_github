
/********************************************************************************
  
文 件 名：	cdzChargeRecord.h
版    本：	1.0
概    述：	充电记录处理
作    者：	yansudan
日    期：	2011.02.6
修改记录：

*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "cdzChargeRecord.h"
#include "sql.h"

static ST_BOOL s_NewChargeRecordFlag = false;		/* 检查是否有新的充电记录产生 */	
static ST_UCHAR s_ChargeRecordTimes = 0;			/* 充电记录上报的次数 */	
static ChargeRecordTP s_cdzChargeRecord;			/* 内存中只保存一条充电记录 */

static ST_BOOL s_NewChargeRecordFlag_Gprs = false;		/* 检查是否有新的充电记录产生 */	

/********************************************************************************* 
 函数名称： IsNewChargeRecord
 功能描述：	判断是否有新的充电记录产生
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:产生新的充电记录
			SD_FAILURE:没有产生
 作    者：	yansudan
 日    期：	2010.2.6
 修改记录：2011.02.06
*********************************************************************************/
ST_RET IsNewChargeRecord(void)
{
	if(s_NewChargeRecordFlag) return SD_SUCCESS;
	return SD_FAILURE;	
}

/********************************************************************************* 
 函数名称： IsNewChargeRecord_Gprs
 功能描述：	判断是否有新的充电记录产生
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:产生新的充电记录
			SD_FAILURE:没有产生
 作    者：	yansudan
 日    期：	2010.2.6
 修改记录：2011.02.06
*********************************************************************************/
ST_RET IsNewChargeRecord_Gprs(void)
{
	if(s_NewChargeRecordFlag_Gprs) return SD_SUCCESS;
	return SD_FAILURE;
}
/********************************************************************************* 
 函数名称： InitNewChargeRecord
 功能描述：	初始化一条充电记录
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:产生新的充电记录
			SD_FAILURE:没有产生
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_RET InitNewChargeRecord(void)
{
	SaveChargeRecData(&s_cdzChargeRecord);
	s_NewChargeRecordFlag = true;
	s_NewChargeRecordFlag_Gprs = true;
	s_ChargeRecordTimes = 0;

	return true;

}


/********************************************************************************* 
 函数名称： GetNewChargeRecord
 功能描述：	获取一条充电记录
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:产生新的充电记录
			SD_FAILURE:没有产生
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_RET GetNewChargeRecord(ChargeRecordTP *pChargeRecord)
{
	if (s_NewChargeRecordFlag == false)  return SD_FAILURE;


	*pChargeRecord = s_cdzChargeRecord;

	return SD_SUCCESS;
}


/********************************************************************************* 
 函数名称： GetNewChargeRecord
 功能描述：	获取一条充电记录
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:产生新的充电记录
			SD_FAILURE:没有产生
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_RET GetNewChargeRecord_Gprs(ChargeRecordTP *pChargeRecord)
{
	if (s_NewChargeRecordFlag_Gprs == false)  return SD_FAILURE;


	*pChargeRecord = s_cdzChargeRecord;

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： ConfirmChargeRecord
 功能描述：	确认充电记录，表示不再上报充电记录或响应充电记录确认帧
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ConfirmChargeRecord(void)
{
	s_NewChargeRecordFlag = false;
	s_ChargeRecordTimes = 0;
}

/********************************************************************************* 
 函数名称： ConfirmChargeRecord
 功能描述：	确认充电记录，表示不再上报充电记录或响应充电记录确认帧
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ConfirmChargeRecord_Gprs(void)
{
	s_NewChargeRecordFlag_Gprs = false;
	s_ChargeRecordTimes = 0;
}

/********************************************************************************* 
 函数名称： GetChargeRecord
 功能描述：	获取n条充电记录
 输    入：	pChargeRecord,保存找到的最近n条充电记录，n：要读取的条数
 输	   出： 无
 返 回 值： SD_SUCCESS:找到最近充电记录。SD_FAILURE：未找到最近记录
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_RET GetChargeRecord(ChargeRecordTP *pChargeRecord,ST_INT32 *n)
{
	*n= readnlatest(*n, pChargeRecord,"message");
	if(*n<0) return SD_FAILURE;
	return SD_SUCCESS;
}


