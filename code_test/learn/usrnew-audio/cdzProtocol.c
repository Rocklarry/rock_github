

/********************************************************************************

文 件 名：	cdzProtocol.cpp
版    本：	1.0
概    述：	深圳充电站主站规约，TCPIP通信方式
			本协议暂时不支持分帧处理，全部数据必须一帧处理完成。
作    者：	yansudan
日    期：	2011.2.6
修改记录：  
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "cdzProtocol.h"
#include "cdzDataType.h"
#include "cdzChargeRecord.h"
#include "AlarmRecord.h"
#include "public.h"
#include "mytcpip.h"
//#include "Gprs.h"

extern ChargeMsgTP ChargeMsg;

static CDZ_PROTOCOL_PARAM s_cdzProtocolParam;    
static ST_BOOL s_ProtocolExitFlag = false;          /* 规约线程退出标志 */   


static ST_UCHAR sFSeq = 1;
static ST_UCHAR sLastFSeq = 0;
static ST_BOOL uKeeALiveFlag;


ST_INT32 CurrentKeepLiveCount;

static ST_UCHAR s_RtuA[4] = {0x82, 0x00, 0x00, 0x35};

/********************************************************************************* 
 函数名称： BCDToByte
 功能描述：	BCD转换Byte
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2010.2.6
 修改记录：
*********************************************************************************/
static BYTE BCDToByte(BYTE BCD)
{
    return ((BCD >> 4) * 10) + (BCD & 0x0f);
}

/********************************************************************************* 
 函数名称： cdzGetTimeSec
 功能描述：	获取当前的时间(秒)
 输    入：	无
 输	   出： 无
 返 回 值： 时间秒
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static ST_BOOL setTimeValue(char *pbRx, unsigned char dwValidMinute)
{
//	ST_INT32 Cmdtime,Nowtime;
	ST_INT32 Cmdh,Cmdm,Cmds,Nowh,Nowm,Nows;
	ST_INT32 AbsoluteValue;
	struct tm *CmdTime,*CurrentTime;
	time_t now;
	now = time(0);
	CurrentTime = localtime(&now);
	CmdTime->tm_year = BCDToByte((*pbRx++))+2000;
	CmdTime->tm_mon = BCDToByte(*pbRx++);
	CmdTime->tm_mday = BCDToByte(*pbRx++);
	CmdTime->tm_hour = BCDToByte(*pbRx++);
	CmdTime->tm_min = BCDToByte(*pbRx++);
	CmdTime->tm_sec = 0;
	
	Cmdh = CmdTime->tm_mday*24;
	Cmdm = Cmdh*60+CmdTime->tm_hour;
	Cmds = Cmdm*60+CmdTime->tm_sec;
	
	Nowh = CurrentTime->tm_mday*24;
	Nowm = Cmdh*60+CurrentTime->tm_hour;
	Nows = Cmdm*60+CurrentTime->tm_sec;
	

	if (Cmds >= Nows)
	{
		AbsoluteValue = Cmds-Nows;
	}
	else
	{
		AbsoluteValue = Nows - Cmds;
	}

	dwValidMinute = BCDToByte(dwValidMinute);

	if (AbsoluteValue >= dwValidMinute*60)
	{
		return false;
	}
	else
	{
	   return true;
	}
}

/********************************************************************************* 
 函数名称： cdcCalcFSeq
 功能描述：	计算流水号
 输    入：	无
 输	   出： 无
 返 回 值： 新的流水号
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UCHAR cdcCalcFSeq(ST_UCHAR sSFeq)
{
	ST_UCHAR sNewSFeq = 0;

	if (sSFeq == 0x7F)
	{
		sNewSFeq = 0x01;
	}
	else
	{
		sNewSFeq = sSFeq+1;
	}

	return sNewSFeq;
}

/********************************************************************************* 
 函数名称： cdzCalcCS
 功能描述：	计算CS校验和
 输    入：	sDataRecvbuf:接收到数据存放在此处
			sDataLen:接收到的数据长度
 输	   出： 无
 返 回 值： 校验
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_UCHAR cdzCalcCS(const ST_UCHAR *sDataBuf, ST_UINT16 sDataLen)
{
	ST_UINT16 i = 0;
	ST_UCHAR uCsVal = 0;

	if (sDataBuf == NULL) return 0;

	for (i=0; i<sDataLen; i++)
	{
		uCsVal += sDataBuf[i];
	}

	return uCsVal;

}

/********************************************************************************* 
 函数名称： cdzProtocolCheckCS
 功能描述：	CS校验和校验
 输    入：	sDataRecvbuf:接收到数据存放在此处
			sDataLen:接收到的数据长度
 输	   出： 无
 返 回 值： SD_SUCCESS:CS校验码正确
			SD_FAILURE:CS校验码错误
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static ST_RET cdzProtocolCheckCS(const ST_UCHAR *sDataBuf, ST_UINT16 sDataLen)
{
	//ST_UCHAR uCsVal = 0;

	if (sDataBuf == NULL || sDataLen == 0) return SD_FAILURE;


	if (cdzCalcCS(sDataBuf, (sDataLen-2)) == sDataBuf[sDataLen-2]) return SD_SUCCESS;

	return SD_FAILURE;
}


/********************************************************************************* 
 函数名称： cdzProtocolCheckDataBuf
 功能描述：	数据DataBuf校验
 输    入：	sDataRecvbuf:接收到数据存放在此处
			sDataLen:接收到的数据长度
 输	   出： 无
 返 回 值： SD_SUCCESS:正确帧，需要进一步解析
			SD_FAILURE:错误帧，直接丢弃，不做任何处理
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_RET cdzProtocolCheckDataBuf(const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen)
{
	ST_UINT16 sDataDomainLen = 0;

	ST_UCHAR sFuncCode = 0;

	if (sDatabuf == NULL || sDataLen == 0) return SD_SUCCESS;


	if (sDatabuf[0] != CDZ_FRAME_START 
		|| sDatabuf[sDataLen - 1] != CDZ_FRAME_END
		|| sDatabuf[7] != CDZ_FRAME_START) return SD_FAILURE;

	if (cdzProtocolCheckCS(sDatabuf, sDataLen) == SD_FAILURE)
	{
	printf("failed is crc\n");
	 return SD_FAILURE;
	}

	sFuncCode = sDatabuf[8] & 0x3F;

	sDataDomainLen = sDatabuf[9] + sDatabuf[10] * 256;

	if (sFuncCode == KEEP_ALIVE
		|| sFuncCode == KEEP_ALIVE_RES)
	{
		if (sDataDomainLen != 0) return SD_FAILURE;  
	}
	else 
	{
		if ((sDataDomainLen + 13) != sDataLen)
		{
		printf("failed is datalen\n");
		 return SD_FAILURE;
		}
	}


	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzProtocolUpload
 功能描述：	主动上传的数据处理，主要有告警类和充电记录数据两类
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static void cdzProtocolUpload(ST_UCHAR *psFSeq, ST_UCHAR *pLastFeq)
{
	if(!IsNewChargeRecord())
	{
	   ChargeRecordTP  pChargeRecord;
	   static int ResendChargeRecordCount = 3;
	   //static ST_UCHAR OldsExchangeSeq[5] = {0};
	   static ST_UINT32 OldsInterSeq = 0xFFFFFFFF;

	  // if(memcmp(OldsExchangeSeq, pChargeRecord.sExchangeSeq, 5) != 0)
	  // {
			//ResendChargeRecordCount = 3;
			//memcpy(OldsExchangeSeq,pChargeRecord.sExchangeSeq,5);  
	  // }
	  // else
	  // {
		 //  if(ResendChargeRecordCount == 0)
		 //  {
			//   ConfirmChargeRecord();
			//   return;
		 //  }
	  // }
	   
	   if(!GetNewChargeRecord(&pChargeRecord))
	   {
	        ST_UINT16 sDataLen;
	        ST_UCHAR sSendResbuf[LENGTH];

			if(OldsInterSeq != ChargeMsg.InerSeq)
			{
				ResendChargeRecordCount = 3;
				OldsInterSeq = ChargeMsg.InerSeq;
			}
			else
			{
				if(ResendChargeRecordCount == 0)
				{
					ConfirmChargeRecord();
					return;
				}
			}

			int sSendLen=0;
			
			sSendResbuf[sSendLen++] = CDZ_FRAME_START;
			
		    		/*	sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;*/
			sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
			sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1];
	        sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
		sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;
		sSendResbuf[sSendLen++] = ((*psFSeq & 0x03) << 6);
	        sSendResbuf[sSendLen++] = ((*psFSeq & 0x7C) >> 2);
                sSendResbuf[sSendLen++] = CDZ_FRAME_START;
			
			sSendResbuf[sSendLen++] = 0x83;
	
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
        
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
	
			sSendResbuf[sSendLen++] = 0x01;
        
            sSendResbuf[sSendLen++] =pChargeRecord.sExchangeType;
        		    ST_UCHAR i = 0;
		    for (i=0; i<5; i++)
		    {
				 sSendResbuf[sSendLen++] = pChargeRecord.sExchangeSeq[i] ;
		    }
        
		    for (i=0; i<3; i++)
		    {
				sSendResbuf[sSendLen++] = pChargeRecord.sAreaCode[i];
		    }
        
			for (i=0; i<8; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartCardNo[i];
			}
			for (i=0; i<8; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndCardNo[i];
			}
				for (i=0; i<2; i++)
	        	{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartCardType[i];
			}
			for (i=0; i<2; i++)
	        	{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndCardType[i];
			}
				for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge1[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge2[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge3[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge4[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge1[i];
			}
				for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge2[i];
			}
        			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge3[i];
			}            
			
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge4[i];
			}
			
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice1[i];
			}
           			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice2[i];
			}            
           
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice3[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice4[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sParkFeePrice[i];
			}
        
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[0];
		 	sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[1];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[2];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[3];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[4];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[5];
	
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[0];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[1];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[2];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[3];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[4];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[5];
	
            		for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sParkFee[i];
			}
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sGasPrice[i];
			}
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sGasFee[i];
			}
			
            for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sMoneyBefore[i];
			}
	
            for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sMoneyAfter[i];
			}
	
            for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sCardCounter[i];
			}
	
            for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sTermNo[i];
			}
	
            for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sCardVer[i];
			}
	
            for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPosNo[i];
			}
			sSendResbuf[sSendLen++] = pChargeRecord.sCardStatus;
	
			sSendResbuf[9]=(sSendLen-11)%256;
			sSendResbuf[10]=(sSendLen-11)/256;    
	
			sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_END;
			if(sSendLen < LENGTH)
			{
				ResendChargeRecordCount--;
			    SendData(s_cdzProtocolParam.sSocket, &sSendResbuf[0], sSendLen);
			    *pLastFeq = *psFSeq;
			    *psFSeq = cdcCalcFSeq(*psFSeq); 
			    sleep(2);
				//cdzRecvData

			   // return;
			}
	   }
	}
  
   if(!IsNewAlarmRecord())
   {
   		AlarmRecordTP vAlarmRecord,oldAlarmRecord; 		
	    static int AlarmResendCount = 3;
	   
	   	if(!GetNewAlarmRecord(&vAlarmRecord))
	   	{
		    if(memcmp(&oldAlarmRecord,&vAlarmRecord,sizeof(AlarmRecordTP)) != 0)
		    {
				AlarmResendCount = 3;
				memcpy(&oldAlarmRecord,&vAlarmRecord,sizeof(AlarmRecordTP));  
		    }
			else 
			{
				if(AlarmResendCount==0)
				{
					ConfirmAlarmRecord();
					return;
				}
			}
			
	     //   ST_UINT16 sDataLen;
			ST_UCHAR sSendResbuf[LENGTH];
			int sSendLen=0;
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_START;
	
			sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
	        sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
		    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
	        sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;
	        
			sSendResbuf[sSendLen++] = ((*psFSeq & 0x03) << 6);
			sSendResbuf[sSendLen++] = ((*psFSeq & 0x7C) >> 2);
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_START;
			
			
		
			sSendResbuf[sSendLen++] = 0x89;
	
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x01;              //告警数量
				
			sSendResbuf[sSendLen++] = vAlarmRecord.MeaPointNum; //测量点号
			
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[0];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[1];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[2];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[3];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[4];
			
			sSendResbuf[sSendLen++] = vAlarmRecord.AlarmCode[0];
			sSendResbuf[sSendLen++] = vAlarmRecord.AlarmCode[1];
			ST_UCHAR i ;
			for(i = 0;i<vAlarmRecord.AlarmContentLen;i++)
			{
				sSendResbuf[sSendLen++] = vAlarmRecord.AlarmContent[i];
			}
			sSendResbuf[9]=(sSendLen-11)%256;
			sSendResbuf[10]=(sSendLen-11)/256;    
	
			sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_END;
			if((sSendLen < LENGTH) && (sSendLen > 13))
			{
				AlarmResendCount--;
				SendData(s_cdzProtocolParam.sSocket, &sSendResbuf[0], sSendLen);
				*pLastFeq = *psFSeq;
				*psFSeq = cdcCalcFSeq(*psFSeq); 
				sleep(2);
				return;
			}
		}
   }

}

/********************************************************************************* 
 函数名称： cdzProtocolSendErrFrame
 功能描述：	发送否定帧
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocolSendErrFrame(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, ST_UCHAR *sSendResbuf,char ErrCode)
{
	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	/*sSendResbuf[sSendLen++] = sRtuAddr[0];
	sSendResbuf[sSendLen++] = sRtuAddr[1];
	sSendResbuf[sSendLen++] = sRtuAddr[2];
	sSendResbuf[sSendLen++] = sRtuAddr[3];*/

	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;

	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = sFuncCode | (DIR_C_T_M<<7) | (FRAME_ERR << 6);

	sSendResbuf[sSendLen++] = 0x01;
	sSendResbuf[sSendLen++] = 0x00;

	sSendResbuf[sSendLen++] = ErrCode;

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;

	return sSendLen;
}


/********************************************************************************* 
 函数名称： cdzProtocolSendErrFrame
 功能描述：	发送心跳帧
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocolSendKeepAliveFrame(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, ST_UCHAR *sSendResbuf)
{
	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;
	

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	/*sSendResbuf[sSendLen++] = sRtuAddr[0];
	sSendResbuf[sSendLen++] = sRtuAddr[1];
	sSendResbuf[sSendLen++] = sRtuAddr[2];
	sSendResbuf[sSendLen++] = sRtuAddr[3];*/

	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3];

	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = KEEP_ALIVE | (DIR_C_T_M<<7) | (FRAME_OK << 6);

	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x00;

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;

	return sSendLen;
}


/********************************************************************************* 
 函数名称： cdzProtocolSendLoadFrame
 功能描述：	发送登陆主站帧
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 void cdzProtocolSendLoadFrame()
{
	ST_UCHAR sSendResbuf[LENGTH];
	ST_UINT16 sSendLen = 0;
	

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3];


	sSendResbuf[sSendLen++] = ((0x00 & 0x03) << 6);
	sSendResbuf[sSendLen++] = ((0x00 & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = 0xA1;

	sSendResbuf[sSendLen++] = 0x03;
	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x11;
	sSendResbuf[sSendLen++] = 0x11;
	sSendResbuf[sSendLen++] = 0x11;

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;

	if(sSendLen < LENGTH)
	{
	    SendData(s_cdzProtocolParam.sSocket, &sSendResbuf[0], sSendLen);
		
		//s_ProtolLandFlag = true;
	    usleep(20*1000);
	    return;
	}
}


/********************************************************************************* 
 函数名称： cdzProtocosReadCurDataRes
 功能描述：	读当前数据响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosReadCurDataRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	if(sDataLen<10||(sDataLen%2)!=0)
	{
	    return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}

	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;
	

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	/*sSendResbuf[sSendLen++] = sRtuAddr[0];
	sSendResbuf[sSendLen++] = sRtuAddr[1];
	sSendResbuf[sSendLen++] = sRtuAddr[2];
	sSendResbuf[sSendLen++] = sRtuAddr[3];*/

	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;
    
	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	//sSendResbuf[sSendLen++] =(sFSeq & 0x1F); //((sFSeq & 0x7C) >> 2);
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = sFuncCode | (DIR_C_T_M<<7) | (FRAME_OK << 6);

	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x00;

    memcpy(&sSendResbuf[sSendLen], sDatabuf, 8);
    sSendLen=sSendLen+8;

	unsigned char TNM[8];
	memcpy(TNM, sDatabuf, 8);
    int RecCount=8;
    int i;
	
	for(i=8;i<sDataLen;i=i+2)
	{
		//cdzGetDataItemLenth(const ST_UCHAR *cdzMeaPointFlag, ST_UINT16 cdzIdentCode)
	  unsigned char dataitemContent[256];
	  unsigned short DataItem=sDatabuf[RecCount]+sDatabuf[RecCount+1]*256;

      RecCount += 2;

      short ContentLen=cdzReadDataItemContent(TNM, DataItem,dataitemContent);
	  if(ContentLen>0)
	  {
	      sSendResbuf[sSendLen++]=DataItem%256;
          sSendResbuf[sSendLen++]=DataItem/256;
	  int k;
		  for(k=0;k<ContentLen;k++)
		  {
		     sSendResbuf[sSendLen++]=dataitemContent[k];
		  } 
	  }
	  else if(ContentLen==0)
	  {

	  }
	  else
	  {
	     return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	  }
	}

	sSendResbuf[9]=(sSendLen-11)%256;
    sSendResbuf[10]=(sSendLen-11)/256;
    

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;
	if(sSendLen>LENGTH)
	{
	   return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	return sSendLen;
}

/********************************************************************************* 
 函数名称： cdzProtocosReadTaskDataRes
 功能描述：	读任务数据响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosReadTaskDataRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	return 0;
}


/********************************************************************************* 
 函数名称： cdzProtocosReadChargeRecordRes
 功能描述：	读充电记录数据响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosReadChargeRecordRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;
	
	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	/*sSendResbuf[sSendLen++] = sRtuAddr[0];
	sSendResbuf[sSendLen++] = sRtuAddr[1];
	sSendResbuf[sSendLen++] = sRtuAddr[2];
	sSendResbuf[sSendLen++] = sRtuAddr[3];*/


	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;

	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	//sSendResbuf[sSendLen++] = (sFSeq & 0x1F);//((sFSeq & 0x7C) >> 2);
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = sFuncCode | (DIR_C_T_M<<7) | (FRAME_OK << 6);

	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x00;
	


	short StartDataPointPos=sDatabuf[0]+sDatabuf[1]*256; 
	ST_INT32 RecordCount=sDatabuf[2];	
    

	ChargeRecordTP  pChargeRecord[READRECORD_MAXLEN];
	if(!GetChargeRecord(pChargeRecord,&RecordCount))
	{
		ST_UCHAR j;
		
		sSendResbuf[sSendLen++] = 0x01;
		sSendResbuf[sSendLen++] = 0x00;

		sSendResbuf[sSendLen++] = RecordCount;
		for(j=0;j<RecordCount;j++)
		{
			
			sSendResbuf[sSendLen++] =pChargeRecord[j].sExchangeType;
	
			ST_UCHAR i = 0;
			for (i=0; i<5; i++)
			{
				 sSendResbuf[sSendLen++] = pChargeRecord[j].sExchangeSeq[i] ;
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sAreaCode[i];
			}
	
			for (i=0; i<8; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sStartCardNo[i];
			}
			for (i=0; i<8; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sEndCardNo[i];
			}
		
			for (i=0; i<2; i++)
			{
			  sSendResbuf[sSendLen++] = pChargeRecord[j].sStartCardType[i];
			}
			for (i=0; i<2; i++)
			{
			  sSendResbuf[sSendLen++] = pChargeRecord[j].sEndCardType[i];
			}
			
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sStartEnerge[i];
			}
			
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sStartEnerge1[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sStartEnerge2[i];
			}
		
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sStartEnerge3[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sStartEnerge4[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sEndEnerge[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sEndEnerge1[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sEndEnerge2[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sEndEnerge3[i];
			}    
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sEndEnerge4[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sPrice1[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sPrice2[i];
			}   
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sPrice3[i];
			}
		
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sPrice4[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sParkFeePrice[i];
			}
	
			sSendResbuf[sSendLen++] = pChargeRecord[j].sStartTime[0];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sStartTime[1];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sStartTime[2];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sStartTime[3];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sStartTime[4];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sStartTime[5];
	
			sSendResbuf[sSendLen++] = pChargeRecord[j].sEndTime[0];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sEndTime[1];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sEndTime[2];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sEndTime[3];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sEndTime[4];
			sSendResbuf[sSendLen++] = pChargeRecord[j].sEndTime[5];
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sParkFee[i];
			}
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sGasPrice[i];
			}
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sGasFee[i];
			}
			
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sMoneyBefore[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sMoneyAfter[i];
			}
	
			for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sCardCounter[i];
			}
	
			for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sTermNo[i];
			}
	
			for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sCardVer[i];
			}
	
			for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord[j].sPosNo[i];
			}
			sSendResbuf[sSendLen++] = pChargeRecord[j].sCardStatus;
		}		
		sSendResbuf[9]=(sSendLen-11)%256;
		sSendResbuf[10]=(sSendLen-11)/256;

		sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

		sSendResbuf[sSendLen++] = CDZ_FRAME_END;
		if(sSendLen>LENGTH)
		{
		   return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
		}
		else
		{
		return sSendLen;
		}
	}
	else
	{
        return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
    }	
}


/********************************************************************************* 
 函数名称： cdzProtocosReadProgramLogRes
 功能描述：	读编程日志响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosReadProgramLogRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	return 0;
}

/********************************************************************************* 
 函数名称： cdzProtocosWriteParamRes
 功能描述：	写对象参数响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosWriteParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{

	if(sDataLen<5)
	{
	    return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	ST_UINT16 sSendLen = 0;
	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;



	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;

	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = sFuncCode | (DIR_C_T_M<<7) | (FRAME_OK << 6);

	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x00;
	unsigned char TN=sDatabuf[0];
	unsigned char AUT=sDatabuf[1];
    unsigned char PassWord[3];
	PassWord[0]=sDatabuf[2];
    PassWord[1]=sDatabuf[3];
    PassWord[2]=sDatabuf[4];

	sSendResbuf[sSendLen++] = TN;
    int i;
    for(i=5;i<sDataLen;)
	{
	   unsigned short dataitem=sDatabuf[i]+sDatabuf[i+1]*256;
	   i=i+2;

	   ST_UINT16 dataLen=cdzGetDataItemLenthMeaPoint(TN,dataitem);
	   if((dataLen>0) && (dataLen+i<=sDataLen))
	   {
	      unsigned char datacontent[256];
	      int k;
		  for(k=0;k<dataLen;k++)
		  {
             datacontent[k]=sDatabuf[i++];
		  }
		  int setResult=cdzWriteDataItemContentAuth(TN, dataitem,datacontent, AUT,PassWord);
		  sSendResbuf[sSendLen++]=dataitem%256;
		  sSendResbuf[sSendLen++]=dataitem/256;
          sSendResbuf[sSendLen++]=setResult;
	   }
	   else
	   {
		printf("set para failed\n");
	     return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	   }

	}
	printf("set para compelete\n");
	sSendResbuf[9]=(sSendLen-11)%256;
    sSendResbuf[10]=(sSendLen-11)/256;
    

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;
	if(sSendLen>LENGTH)
	{
	   return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	return sSendLen;
}

/********************************************************************************* 
 函数名称： cdzProtocosRtWriteParamRes
 功能描述：	实时写对象参数响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosRtWriteParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	if(sDataLen<14)
	{
	    return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;
	

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;

	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = sFuncCode | (DIR_C_T_M<<7) | (FRAME_OK << 6);

	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x00;

	unsigned char TN=sDatabuf[0];
	unsigned char AUT=sDatabuf[1];
    unsigned char PassWord[3];
	PassWord[0]=sDatabuf[2];
    PassWord[1]=sDatabuf[3];
    PassWord[2]=sDatabuf[4];

	char sDataTime[5];
    sDataTime[0]=sDatabuf[5];
	sDataTime[1]=sDatabuf[6];
	sDataTime[2]=sDatabuf[7];
	sDataTime[3]=sDatabuf[8];
	sDataTime[4]=sDatabuf[9];
	unsigned char ValidTime=sDatabuf[10];
    if(!setTimeValue(sDataTime,ValidTime))
	{
	   return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
    int i; 
    for(i=11;i<sDataLen;)
	{
	   unsigned short dataitem=sDatabuf[i]+sDatabuf[i+1]*256;
	   i=i+2;

	   ST_UINT16 dataLen=cdzGetDataItemLenthMeaPoint(TN,dataitem);
	   if(sDataLen-i<dataLen||dataLen>256)
	   {
	      return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	   }
	   if(dataLen>0)
	   {
	      unsigned char datacontent[256];
	      int k;
		  for(k=0;k<dataLen;k++)
		  {
             datacontent[k]=sDatabuf[i++];
		  }
		  int setResult=cdzWriteDataItemContentAuth(TN, dataitem,datacontent, AUT,PassWord);
		  sSendResbuf[sSendLen++]=dataitem%256;
		  sSendResbuf[sSendLen++]=dataitem/256;
          sSendResbuf[sSendLen++]=setResult;
	   }
	   else
	   {
	     return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	   }
	}

	sSendResbuf[9]=(sSendLen-13)%256;
    sSendResbuf[10]=(sSendLen-13)/256;
    

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;
	if(sSendLen>LENGTH)
	{
	   return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	return sSendLen;
}

/********************************************************************************* 
 函数名称： cdzProtocosReadAlarmRes
 功能描述：	异常告警
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosReadAlarmRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{

	if(sDataLen!=9)
	{
	    return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;
	
	sSendResbuf[sSendLen++] = CDZ_FRAME_START;


	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[0] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[1] ;
	sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[2] ;
    sSendResbuf[sSendLen++] = s_cdzProtocolParam.TermAdd[3] ;

	sSendResbuf[sSendLen++] = ((sFSeq & 0x03) << 6) | sMstaAddr;
	sSendResbuf[sSendLen++] = ((sFSeq & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = sFuncCode | (DIR_C_T_M<<7) | (FRAME_OK << 6);

	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x00;
	

//	char TN=sDatabuf[0];
	//short ALR=sDatabuf[1]+sDatabuf[2]*256;
	ST_UCHAR sAlrTime[5];
	sAlrTime[0]=sDatabuf[3];
	sAlrTime[1]=sDatabuf[4];
	sAlrTime[2]=sDatabuf[5];
	sAlrTime[3]=sDatabuf[6];
	sAlrTime[4]=sDatabuf[7];
	ST_INT32 AlrCount=sDatabuf[8];
	
	sSendResbuf[sSendLen++] = AlrCount;
	
	AlarmRecordTP pAlarmRecord;

	if(GetAlarmRecord(sAlrTime,&sDatabuf[1],&pAlarmRecord,&AlrCount)==SD_SUCCESS)
	{
		int i;
		for(i=0;i<AlrCount;i++)
		{		
			sSendResbuf[sSendLen++] = pAlarmRecord.MeaPointNum;
			sSendResbuf[sSendLen++] = pAlarmRecord.Time[0];
			sSendResbuf[sSendLen++] = pAlarmRecord.Time[1];
			sSendResbuf[sSendLen++] = pAlarmRecord.Time[2];
			sSendResbuf[sSendLen++] = pAlarmRecord.Time[3];
			sSendResbuf[sSendLen++] = pAlarmRecord.Time[4];
			
			sSendResbuf[sSendLen++] = pAlarmRecord.AlarmCode[0];
			sSendResbuf[sSendLen++] = pAlarmRecord.AlarmCode[1];	
			memcpy(sSendResbuf,pAlarmRecord.AlarmContent,pAlarmRecord.AlarmContentLen); //参数内容长度貌似也要写到数据库
			sSendLen = sSendLen + pAlarmRecord.AlarmContentLen;			
		}
	}
	sSendResbuf[9]=(sSendLen-13)%256;
    sSendResbuf[10]=(sSendLen-13)/256;	
	
	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;
	if(sSendLen>LENGTH)
	{
	   return cdzProtocolSendErrFrame(sFSeq, sFuncCode,sMstaAddr,sRtuAddr,sSendResbuf,SET_PARAM_INVALID);
	}
	return sSendLen;
}

/********************************************************************************* 
 函数名称： cdzProtocosGetEsamRes
 功能描述：	获取ESAM随机数响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosGetEsamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	return 0;
}

/********************************************************************************* 
 函数名称： cdzProtocosWriteKeyParamRes
 功能描述：	以密钥加密的方式写参数响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosWriteKeyParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	return 0;
}

/********************************************************************************* 
 函数名称： cdzProtocosRtWriteKeyParamRes
 功能描述：	实时以密钥加密的方式写参数响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosRtWriteKeyParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	return 0;
}

/********************************************************************************* 
 函数名称： cdzProtocosAlarmConfirmRes
 功能描述：	告警确认帧响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosAlarmConfirmRes(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	
//	ST_UINT16 sSendLen = 0;
	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;
    
	ConfirmAlarmRecord();
	return 0;
}


/********************************************************************************* 
 函数名称： cdzProtocosChargeRecConfirmRes
 功能描述：	充电记录确认帧响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT16 cdzProtocosChargeRecConfirmRes(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	ConfirmChargeRecord();
	return 0;
}

/********************************************************************************* 
 函数名称： cdzProtocosKeepALiveRes
 功能描述：	告警确认帧响应
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
 ST_UINT16 cdzProtocosKeepALiveRes(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
//	ST_UINT16 sSendLen = 0;

	if (sRtuAddr == NULL || sSendResbuf == NULL) return 0;


	//if (sLastFSeq == sFSeq)
	{
		return 0xFFFF;
	}
	
}

/********************************************************************************* 
 函数名称： cdzProtocosConfirmAlarm
 功能描述：	告警确认的处理
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

static ST_UINT16 cdzProtocosConfirmAlarm(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	return 0;
}

/********************************************************************************* 
 函数名称： cdzProtocosConfirmLand
 功能描述：	登陆确认
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sDatabuf:数据域的数据
			sDataLen:数据域的长度
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
extern ST_BOOL ProtolLandFlag_Gprs;
void cdzProtocosConfirmLand(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf)
{
	//ST_UINT16 sSendLen = 0;

	s_ProtolLandFlag = true;
	ProtolLandFlag_Gprs = true;
	
}

/********************************************************************************* 
 函数名称： cdzProtocolDataProcess
 功能描述：	规约数据处理主函数
 输    入：	sDataRecvbuf:接收到数据存放在此处
			sDataLen:接收到的数据长度
 输	   出： 无
 返 回 值： 发送数据的长度
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static ST_UINT16 cdzProtocolDataProcess(ST_UCHAR sLastFSeq, const ST_UCHAR *sDataRecvbuf, ST_UINT16 sDataLen,
										ST_UCHAR *sDataResbuf)
{
	ST_UCHAR sCityCode;					
	ST_UCHAR sCountryCode;				
	ST_UINT16 sConsoleAddr;				
	ST_UCHAR sMstaAddr;					
	ST_UCHAR sISeq;						
	ST_UCHAR sFSeq;						
	ST_UCHAR sFuncCode;					
	ST_UINT16 sDataDomainLen;			

	if (sDataRecvbuf == NULL || sDataLen == 0 || sDataResbuf == NULL) 
	{
		printf("failed sDataLen is 0\n");
		return 0 ;
	}


	if (cdzProtocolCheckDataBuf(sDataRecvbuf, sDataLen) == SD_FAILURE)
	{
		printf("failed is checkbuf\n");
	 	return 0;
	}

	sCityCode = sDataRecvbuf[1];
	sCountryCode = sDataRecvbuf[2];
	sConsoleAddr = sDataRecvbuf[3]*256+sDataRecvbuf[4];

	sMstaAddr = sDataRecvbuf[5] & 0x3F;	
	sISeq = (sDataRecvbuf[6]&0xE0)>>5;
	sFSeq = ((sDataRecvbuf[5]&0xC0)>>6) + ((sDataRecvbuf[6]&0x1F)<<2);

	sFuncCode = sDataRecvbuf[8] & 0x3F;

	if ((sDataRecvbuf[8] & (1<<7)) != DIR_M_T_C)
	{
		return cdzProtocolSendErrFrame(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], sDataResbuf,SET_PARAM_INVALID);
	}

	if ((sDataRecvbuf[8] & (1<<6)) != FRAME_OK)
	{
		printf("failed is frame is not ok\n");
		return 0;
	}


	if (sISeq != 0)
	{
		return cdzProtocolSendErrFrame(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], sDataResbuf,SET_PARAM_INVALID);
	}

	sDataDomainLen = sDataRecvbuf[9] + sDataRecvbuf[10] * 256;


	switch(sFuncCode)
	{
	case READ_CURRENT_DATA:				
		return cdzProtocosReadCurDataRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		printf("read8010 is OK!\n");
		break;
	case READ_TASK_DATA:				
		return cdzProtocosReadTaskDataRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case READ_CHARGE_RECORD:			
		return cdzProtocosReadChargeRecordRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case READ_PROGRAM_LOG:		
		return cdzProtocosReadProgramLogRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case RT_WRITE_OBJECT_PARAM:	
		return cdzProtocosRtWriteParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case WRITE_OBJECT_PARAM:
		printf("set para frame\n");
		return cdzProtocosWriteParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case READ_ALARM:
		return cdzProtocosReadAlarmRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case GET_ESAM_RAND:	
		return cdzProtocosGetEsamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case WRITE_KEY_PARAM:
		return cdzProtocosWriteKeyParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case RT_WRITE_KEY_PARAM:
		return cdzProtocosRtWriteKeyParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case ALARM_CONFIRM:	
		cdzProtocosAlarmConfirmRes(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case CHARGEREC_CONFIRM:
		cdzProtocosChargeRecConfirmRes(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		printf("ChargeRecord OK!\n");
		break;
	case KEEP_ALIVE:
		return cdzProtocosKeepALiveRes(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;

	case LAND_CONFIRM:
		cdzProtocosConfirmLand(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		printf("Land OK!\n");
		break;
	default:
		break;
	}


	return 0;

}

/********************************************************************************* 
 函数名称： cdzProtocolProcess
 功能描述：	规约处理接收线程
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static void RcvProtocolProcess(void)
{
	ST_INT32 RecvLen,SendLen; 
	ST_UCHAR RecvBuf[LENGTH];	
	ST_UCHAR SendBuf[LENGTH];	
	
	while(!s_ProtocolExitFlag)
	{
	
		RecvLen = ReciveData(s_cdzProtocolParam.sSocket, RecvBuf, LENGTH);
		
		if (RecvLen == 0)
		{
			continue;
		}

		else if (RecvLen > 0)
		{
			printf("received data\n");
			SendLen = cdzProtocolDataProcess(sLastFSeq, RecvBuf, RecvLen, SendBuf);
			printf("SendLen is %d\n",SendLen);
			if (SendLen == 0xFFFF)
			{
				uKeeALiveFlag = true;
			}
			else if (SendLen > 0)  //直接返回接收到的数据，测试用，本应做规约处理
			{
				SendBuf[5] = RecvBuf[5];
				SendBuf[6] = RecvBuf[6];
				SendBuf[SendLen-2] =  cdzCalcCS(SendBuf, (SendLen-2));
				SendData(s_cdzProtocolParam.sSocket, SendBuf, SendLen);
				printf("send data\n");
			}
		}
		
		usleep(10 * 1000);	
	}
}

/********************************************************************************* 
 函数名称： cdzProtocolProcess
 功能描述：	规约处理发送线程
 输    入：	lparam:用户参数
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static void cdzProtocolProcess(void *lparam)
{
	static ST_INT32 sLastSendKeepAliveTimeSec;
//	ST_UCHAR sRecvBuf[LENGTH];
	ST_UCHAR sSendBuf[LENGTH];
	ST_UINT16 sRecvLen;
	ST_UINT16 sTotalLen;
	ST_SOCKET sSocket;
	ST_UINT16 sSendLen = 0;
//	ST_BOOL sSendFlag;
	
	while(!s_ProtocolExitFlag)
	{

		sSocket = CreateSocket(s_cdzProtocolParam.iProtoType);
		if (sSocket == -1)
		{
			
			sleep(TRY_CONNECT_TIME);
			continue;
		}
		s_cdzProtocolParam.sSocket = sSocket;

		if (ConnectServer(s_cdzProtocolParam.remoteIP) == SD_FAILURE)
		{

			CloseSocket();
			sleep(TRY_CONNECT_TIME);
			continue;
		}
		printf("connect success\n");		
		uKeeALiveFlag = true;
		sLastSendKeepAliveTimeSec = CurrentKeepLiveCount;
		sTotalLen = 0;
		sRecvLen = 0;

		s_ProtolLandFlag = false;
		
		while(!s_ProtocolExitFlag)
		{
	            if(!s_ProtolLandFlag) 
		    {
        	        cdzProtocolSendLoadFrame();
                
			 usleep(200*1000);
		    }
			if(s_ProtolLandFlag)
			{

				cdzProtocolUpload(&sFSeq, &sLastFSeq);
			}

			if ((CurrentKeepLiveCount - sLastSendKeepAliveTimeSec) > s_cdzProtocolParam.keepAliveVal)
			{
				if (uKeeALiveFlag == false)
				{
					CloseSocket();
					sleep(TRY_CONNECT_TIME);
					break;
				}

				sSendLen = cdzProtocolSendKeepAliveFrame(sFSeq, KEEP_ALIVE, UPLOAD_MSTA, s_RtuA, sSendBuf);
				sLastFSeq = sFSeq;
				sFSeq = cdcCalcFSeq(sFSeq);
				
				SendData(s_cdzProtocolParam.sSocket, &sSendBuf[0], sSendLen);

				uKeeALiveFlag = false;
				sLastSendKeepAliveTimeSec = CurrentKeepLiveCount;
			}
			usleep(10 * 1000);
		}
	}
}


/********************************************************************************* 
 函数名称： cdzProtocolExit
 功能描述：	规约线程退出
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
			SD_FAILURE:失败
 作    者：	wantao
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_RET cdzProtocolInit(void)
{
	int result;
	pthread_t hcdzProtocol,RcvProtocol;  
					
//	ST_SOCKET sSocket = -1;
//	DWORD cdzProtocolThreadID;
	ST_UCHAR dataitemContent[9];
	ST_UCHAR i = 0;
	ST_INT16 ContentLen = 0;

	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8010,dataitemContent);

	if (ContentLen <=0 ) return SD_FAILURE;

	if (dataitemContent[0] != 0x04) return SD_FAILURE;

	i ++;

	while(dataitemContent[i] == 0xAA)
	{
		i++;
	}
	if (i > 3) return SD_FAILURE;

	sprintf((char *)s_cdzProtocolParam.remoteIP, "%d.%d.%d.%d", dataitemContent[i], dataitemContent[i+1], dataitemContent[i+2], dataitemContent[i+3]);
	printf("remoteip:%s",s_cdzProtocolParam.remoteIP);

	s_cdzProtocolParam.remotePort = dataitemContent[i+4] * 256 + dataitemContent[i+5];

	ST_UCHAR TermCityCode[2];
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8016,TermCityCode);
	if (ContentLen <= 0 ) return SD_FAILURE;
	printf("citycode is\n");
	printf("%2x\n",TermCityCode[0]);
	printf("%2x\n",TermCityCode[1]);

	ST_UCHAR TermAdd[2];
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8017,TermAdd);
	if (ContentLen <= 0 ) return SD_FAILURE;
	printf("Termadd is\n");
	printf("%2X\n",TermAdd[0]);
	printf("%2X\n",TermAdd[1]);

	s_cdzProtocolParam.TermAdd[0] = TermCityCode[0];
	s_cdzProtocolParam.TermAdd[1] = TermCityCode[1];
	s_cdzProtocolParam.TermAdd[2] = TermAdd[0];
	s_cdzProtocolParam.TermAdd[3] = TermAdd[1];
	printf("cdztermadd is\n");
	printf("%2x\n",s_cdzProtocolParam.TermAdd[0]);
	printf("%2x\n",s_cdzProtocolParam.TermAdd[1]);
	printf("%2x\n",s_cdzProtocolParam.TermAdd[2]);
	printf("%2x\n",s_cdzProtocolParam.TermAdd[3]);
	



    ST_UCHAR heartContent;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8018, &heartContent);
	if (ContentLen <=0 ) return SD_FAILURE;
	printf("heartcontent is\n");
	printf("%2x\n",heartContent);
	s_cdzProtocolParam.keepAliveVal = CDZ_BCDTOHEX(heartContent);


	ST_UCHAR TCPUDPIdent;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x801C, &TCPUDPIdent);
	if (ContentLen <=0 ) return SD_FAILURE;
    if(TCPUDPIdent==0x01)
	{
	   s_cdzProtocolParam.iProtoType = PROTOCOLTP_UDP;
	}
	else
	{
	   s_cdzProtocolParam.iProtoType = PROTOCOLTP_TCP;
	}

	result = pthread_create(&hcdzProtocol, PTHREAD_CREATE_JOINABLE, (void *)cdzProtocolProcess ,NULL);
	if(result)
	{
		perror("pthread_create: MainControl.\n");
		exit(EXIT_FAILURE);
	}
	result = pthread_create(&RcvProtocol, PTHREAD_CREATE_JOINABLE, (void *)RcvProtocolProcess ,NULL);
	if(result)
	{
		exit(EXIT_FAILURE);
	}

	
	return SD_SUCCESS;

}

ST_RET cdzProtocolExit(void)
{
	s_ProtocolExitFlag = true;
	sleep(WAIT_THREAD_EXIT_TIME);

	CloseSocket();

	return SD_SUCCESS;
}

