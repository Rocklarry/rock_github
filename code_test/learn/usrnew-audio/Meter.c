#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <pthread.h>
#include <unistd.h>
#include <math.h>
#include "mycom.h"
#include "Meter.h"
#include "public.h"
#include "convertion.h"
#include "DataType.h"
#include "AlarmRecord.h"
#include "cdzDataType.h"


MeterTP MeterMsg;



static ST_CHAR ProtocolType = 0;    //协议类型，0:1997协议，1:2007协议
void EXBOARDCOMSend(void);
void EXBOARDCOMRcv(void);

/********************************************************************************* 
 函数名称： GetProtocolType
 功能描述：	读取后台表测量点参数，获得表所使用的通信协议
 输    入：	无
 输	   出： 无
 返 回 值： SD_FAILURE：获得协议失败，SD_SUCCESS:获得协议成功
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_INT32 GetProtocolType(void)
{
	ST_INT16 ContentLen = 0;
	ST_UCHAR MeterProtoTp;
	ContentLen = cdzReadDataItemContentMeaPoint(0x01, 0x8903,&MeterProtoTp);
	if (ContentLen <= 0 ) return SD_FAILURE;
	printf("the MeterProtoTp is %2x\n",MeterProtoTp);
	if(MeterProtoTp==0x11)
	{
		ProtocolType = 1;
		printf("the protocol is 2007\n");
	}
	MeterMsg.MeterCmd = GETADDR;
	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： InitMeter
 功能描述：	初始化电表函数，创建表接收和发送线程
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void InitMeter(void)
{
	//ST_INT32 result;
	//pthread_t MeterSendthread, MeterRcvthread;
	#if 0
	GetProtocolType();
	result = pthread_create(&MeterSendthread, PTHREAD_CREATE_JOINABLE, (void *)EXBOARDCOMSend ,NULL);
	if(result)
	{
		perror("pthread_create: EXBOARDCOMSend.\n");
		exit(EXIT_FAILURE);
	}

	result = pthread_create(&MeterRcvthread, PTHREAD_CREATE_JOINABLE, (void *)EXBOARDCOMRcv ,NULL);
	if(result)
	{
		perror("pthread_create: EXBOARDCOMRcv.\n");
		exit(EXIT_FAILURE);
	}
	#endif
}

/********************************************************************************* 
 函数名称： PackAlarmRecord
 功能描述：	打包报警参数内容，产生一条告警记录
 输    入：	AlarmCode：告警编码
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PackAlarmRecord(ST_INT32 AlarmCode)
{
	unsigned char AlarmPara[15];  //告警记录上报参数
	ST_UINT16 temp,i;
	AlarmPara[0] = CDZ_HEXTOBCD(((ST_UINT16)(MeterMsg.fVol_A*10))%100);
	AlarmPara[1] = CDZ_HEXTOBCD((((ST_UINT16)(MeterMsg.fVol_A*10)))/100);
	
	AlarmPara[2] = CDZ_HEXTOBCD(((ST_UINT16)(MeterMsg.fVol_B*10))%100);
	AlarmPara[3] = CDZ_HEXTOBCD(((ST_UINT16)(MeterMsg.fVol_B*10))/100);
	
	AlarmPara[4] = CDZ_HEXTOBCD(((ST_UINT16)(MeterMsg.fVol_C*10))%100);
	AlarmPara[5] = CDZ_HEXTOBCD(((ST_UINT16)(MeterMsg.fVol_C*10))/100);
	
	temp = (ST_UINT16)(MeterMsg.fCur_A*1000);
	for(i = 0;i<3;i++)
	{
		AlarmPara[i+6] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
        temp = (ST_UINT16)(MeterMsg.fCur_B*1000);
        for(i = 0;i<3;i++)
        {
                AlarmPara[i+9] = CDZ_HEXTOBCD(temp%100);
                temp = temp/100;
        }	
        temp = (ST_UINT16)(MeterMsg.fCur_C*1000);
        for(i = 0;i<3;i++)
        {
                AlarmPara[i+12] = CDZ_HEXTOBCD(temp%100);
                temp = temp/100;
        }	
	printf("fVol_A is %4.1f\n",MeterMsg.fVol_A);
	printf("fVol_B is %4.1f\n",MeterMsg.fVol_B);
	printf("fVol_C is %4.1f\n",MeterMsg.fVol_C);
	printf("fCur_A is %6.3f\n",MeterMsg.fCur_A);
	printf("fCur_B is %6.3f\n",MeterMsg.fCur_B);
	printf("fCur_C is %6.3f\n",MeterMsg.fCur_C);
	int j;
	for(j=0;j<15;j++)
	{
		printf("Para is %2x\n",AlarmPara[j]);
	}
	InitNewAlarmRecord(0x01,AlarmCode,AlarmPara,15);
	int temp2 = DefalutPhaseFlag&0xFFF;
	printf("temp2 is %3x\n",temp2);

	if((DefalutPhaseFlag&0x7FFF)!=0)
	{

		printf("DefalutPhaseFlagg is %2X\n",DefalutPhaseFlag);
		if((EventFlag.MeterExcepFlag&0x02)==0)
		{
			printf("MeterExcepFlag yi huo qian is %2x\n",EventFlag.MeterExcepFlag);
			EventFlag.MeterExcepFlag^=0x02;
			if( DISPIDLE == DisplayInfo.CurrentPage|| ChargeMsg.PaleStatus == PALECHARGING )
			{
				ChargeMsg.PaleStatus = PALEEXCEPTION;
			}
		}
		if(((EventFlag.MeterExcepFlag&0x02)!=0)&&(ChargeMsg.PaleStatus==PALECHARGING))
		{
			ChargeMsg.PaleStatus = PALEEXCEPTION;
		}
	}
	else
	{
		 printf("DefalutPhaseFlag is %2X\n",DefalutPhaseFlag);
		if((EventFlag.MeterExcepFlag&0x02)==0x02)
		{
			EventFlag.MeterExcepFlag^=0x02;
		}
	}	
}



/********************************************************************************* 
 函数名称： SetSysTime
 功能描述：	设置系统时间
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.5.30
 修改记录：
*********************************************************************************/

void SetSysTime(void)
{
	char timecmd[50],savecmd[50]="hwclock --systohc";
	sprintf(timecmd,"date -s '%4d-%2d-%2d %2d:%2d:%2d'",MeterMsg.MeterCurrentTime[6]+2000,MeterMsg.MeterCurrentTime[5],MeterMsg.MeterCurrentTime[4],
												MeterMsg.MeterCurrentTime[2],MeterMsg.MeterCurrentTime[1],MeterMsg.MeterCurrentTime[0]);
	system(timecmd);
	system(savecmd);						
}

/********************************************************************************* 
 函数名称： ACVoltChk
 功能描述：	检测交流电压,是否过压,欠压.//1秒检测
 输    入：	fACVolt:交流电压，VolPhase：电压相位
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
                                
 unsigned char  DealOver_Low_Vol(const float fACVolt,ST_INT32 VolPhase)
{
    static unsigned char    ubACLowCnt_A=0;
    static unsigned char    ubACLowCnt_B=0;
    static unsigned char    ubACLowCnt_C=0;        
    static unsigned char    ubACHighCnt_A=0;
    static unsigned char    ubACHighCnt_B=0;
    static unsigned char    ubACHighCnt_C=0;        
    static unsigned char    ubACBackCnt_A=0;
    static unsigned char    ubACBackCnt_B=0;
    static unsigned char    ubACBackCnt_C=0;        
    static unsigned char    ubACVoltStatus_A=0;
    static unsigned char    ubACVoltStatus_B=0;
    static unsigned char    ubACVoltStatus_C=0;
    unsigned char ReturnStatus = 0;
    
    switch(VolPhase)
    {
    	case 1:
    		{
    			 if( fACVolt<HoutaiData.fLowVolLimit)//判断交流欠压点
				{
					ubACHighCnt_A=0;
					ubACBackCnt_A=0;
					ubACLowCnt_A++;
					if(ubACLowCnt_A>1)//10S低于交流欠压告警
					{
				//		EventFlag.MeterExcepFlag^=0x02;
						ubACLowCnt_A=0;
						ubACVoltStatus_A = ACVoltLow;
				
						if((DefalutPhaseFlag&0x01)==0) //A相欠压告警已上报
						{
							DefalutPhaseFlag^=0x01;
							PackAlarmRecord(VOL_ALOW);
							printf("qian ya A test!!!!!!!!!!!!!!!!!!!!!1\n");
						//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						}
						#if 0
						else //测试过压恢复
						{
							HoutaiData.fLowVolLimit =200;
							printf("guo ya huifu A test!!!!!!!!!!!!!!!!!!!!!1\n");
						}	
						#endif		
						
					}	
				}
				else if((ubACVoltStatus_A==ACVoltLow&&fACVolt>HoutaiData.fLowVolLimit) || (ubACVoltStatus_A==ACVoltHigh && fACVolt<HoutaiData.fOverVolLimit)) //A相电压恢复
				{
				  	ubACLowCnt_A=0;
					ubACHighCnt_A=0;
					ubACBackCnt_A++;
					printf("phase volA hf\n");
					printf("phase ubACBackCnt_A %d\n",ubACBackCnt_A);
					 printf("phase DefalutPhaseFlag %d\n",DefalutPhaseFlag);
					if(ubACBackCnt_A>1)//交流告警恢复正常
					{
						ubACBackCnt_A=0;
						ubACVoltStatus_A=ACVoltNormal;
				//		EventFlag.MeterExcepFlag^=0x02;
						
						if((DefalutPhaseFlag&0x01)==0x01) //A相欠压告警已上报
						{
							DefalutPhaseFlag^=0x01;
							PackAlarmRecord(VOL_ALOWHF);
						}
						int temp = DefalutPhaseFlag&0x08;
						printf("DefalutPhaseFlag temp is %d\n",temp);
						if((DefalutPhaseFlag&0x08)==0x08) //A相欠压告警已上报
						{
							 printf("phase VolA DefalutPhaseFlag qian %d\n",DefalutPhaseFlag);

							DefalutPhaseFlag^=0x08;
							 printf("phase VolA DefalutPhaseFlag hou %d\n",DefalutPhaseFlag);

							PackAlarmRecord(VOL_AEXCESSHF);
						}
			
					printf("guo ya huifu test A!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					}
				}
				else if(fACVolt<=HoutaiData.fOverVolLimit)
				{
				  	ubACLowCnt_A=0;
				  	ubACHighCnt_A=0;
					ubACBackCnt_A=0;
					ubACVoltStatus_A=ACVoltNormal;//交流电压正常
					
				//	EventFlag.MeterExcepFlag = 0;
			
				}					
				else if(fACVolt>HoutaiData.fOverVolLimit)
				{
				  	ubACLowCnt_A=0;
					ubACBackCnt_A=0;
				    ubACHighCnt_A++;
				    printf("A parse Over ubACHighCnt_A is %d\n",ubACHighCnt_A);
					if(ubACHighCnt_A>1)//持续10S后交流过压警告
					{	
						ubACVoltStatus_A=ACVoltHigh;
						ubACHighCnt_A=0;
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						
						if((DefalutPhaseFlag&0x08)==0) 
						{
							DefalutPhaseFlag^=0x08;
							PackAlarmRecord(VOL_AEXCESS);
						}	
						#if 0
						else //测试过压恢复
						{
							HoutaiData.fOverVolLimit = 250;
							printf("guo ya huifu  test!!!!!!!!!!!!!!!!!!!!!1\n");
						}	
						#endif								
					}
				}
				ReturnStatus = ubACVoltStatus_A;
    		}
    	break;
    	case 2:
    		{
    			if( fACVolt<HoutaiData.fLowVolLimit)//判断交流欠压点
				{
					ubACHighCnt_B=0;
					ubACBackCnt_B=0;
					ubACLowCnt_B++;
					if(ubACLowCnt_B>2)//10S低于交流欠压告警
					{
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						ubACLowCnt_B=0;
						ubACVoltStatus_B = ACVoltLow;
				
						if((DefalutPhaseFlag&0x02)==0) //B相欠压告警已上报
						{
							DefalutPhaseFlag^=0x02;
							PackAlarmRecord(VOL_BLOW);
						}
					}	
				}
				else if((ubACVoltStatus_B==ACVoltLow&&fACVolt>HoutaiData.fLowVolLimit) || (ubACVoltStatus_B==ACVoltHigh && fACVolt<HoutaiData.fOverVolLimit)) //A相电压恢复
				{
					 printf("phase volB hf\n");
				  	ubACLowCnt_B=0;
					ubACHighCnt_B=0;
					ubACBackCnt_B++;
					if(ubACBackCnt_B>1)//交流告警恢复正常
					{
						ubACBackCnt_B=0;
						ubACVoltStatus_B=ACVoltNormal;
				//		EventFlag.MeterExcepFlag^=0x02;

						if((DefalutPhaseFlag&0x02)==0x02) //A相欠压告警已上报
						{
							DefalutPhaseFlag^=0x02;
							PackAlarmRecord(VOL_BLOWHF);
						}
						if((DefalutPhaseFlag&0x10)==0x10) //A相欠压告警已上报
						{
							 printf("phase VolB DefalutPhaseFlag qian %d\n",DefalutPhaseFlag);

							DefalutPhaseFlag^=0x10;
							 printf("phase VolB DefalutPhaseFlag hou %d\n",DefalutPhaseFlag);

							PackAlarmRecord(VOL_BEXCESSHF);
						}
			
					}
					printf("guo ya huifu test B!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				}
				else if(fACVolt<=HoutaiData.fOverVolLimit)
				{
				  	ubACLowCnt_B=0;
				  	ubACHighCnt_B=0;
					ubACBackCnt_B=0;
					ubACVoltStatus_B=ACVoltNormal;//交流电压正常
				//	EventFlag.MeterExcepFlag = 0;
			
				}
					
				else if(fACVolt>HoutaiData.fOverVolLimit)
				{
				  	ubACLowCnt_B=0;
					ubACBackCnt_B=0;
				    ubACHighCnt_B++;
					if(ubACHighCnt_B>1)//持续10S后交流过压警告
					{	
						ubACVoltStatus_B=ACVoltHigh;
						ubACHighCnt_B=0;
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						if((DefalutPhaseFlag&0x10)==0) 
						{
							DefalutPhaseFlag^=0x10;
							PackAlarmRecord(VOL_BEXCESS);
						}										
					}
				} 
				ReturnStatus = ubACVoltStatus_B;   			
    		}
    	break;
    	case 3:
    		{
    			if( fACVolt<HoutaiData.fLowVolLimit)//判断交流欠压点
				{
					ubACHighCnt_C=0;
					ubACBackCnt_C=0;
					ubACLowCnt_C++;
					if(ubACLowCnt_C>1)//10S低于交流欠压告警
					{
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						ubACLowCnt_C=0;
						ubACVoltStatus_C = ACVoltLow;
				
						if((DefalutPhaseFlag&0x04)==0) //A相欠压告警已上报
						{
							DefalutPhaseFlag^=0x04;
							PackAlarmRecord(VOL_CLOW);
						}
					}	
				}
				else if((ubACVoltStatus_C==ACVoltLow&&fACVolt>HoutaiData.fLowVolLimit) || (ubACVoltStatus_C==ACVoltHigh && fACVolt<HoutaiData.fOverVolLimit)) //A相电压恢复
				{ printf("phase volC hf\n");
				  	ubACLowCnt_C=0;
					ubACHighCnt_C=0;
					ubACBackCnt_C++;
					if(ubACBackCnt_C>1)//交流告警恢复正常
					{
						ubACBackCnt_C=0;
						ubACVoltStatus_C=ACVoltNormal;
				//		EventFlag.MeterExcepFlag^=0x02;

						if((DefalutPhaseFlag&0x04)==0x04) //A相欠压告警已上报
						{
							DefalutPhaseFlag^=0x04;
							PackAlarmRecord(VOL_CLOWHF);
						}
						if((DefalutPhaseFlag&0x20)==0x20) //A相欠压告警已上报
						{
							printf("phase VolC DefalutPhaseFlag qian %d\n",DefalutPhaseFlag);
							DefalutPhaseFlag^=0x20;
							 printf("phase VolC DefalutPhaseFlag hou %d\n",DefalutPhaseFlag);

							PackAlarmRecord(VOL_CEXCESSHF);
						}
			
					}
					printf("guo ya huifu test C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				}
				else if(fACVolt<=HoutaiData.fOverVolLimit)
				{
				  	ubACLowCnt_C=0;
				  	ubACHighCnt_C=0;
					ubACBackCnt_C=0;
					ubACVoltStatus_C=ACVoltNormal;//交流电压正常
				//	EventFlag.MeterExcepFlag = 0;
			
				}
					
				else if(fACVolt>HoutaiData.fOverVolLimit)
				{
				  	ubACLowCnt_C=0;
					ubACBackCnt_C=0;
				    ubACHighCnt_C++;
					if(ubACHighCnt_C>1)//持续10S后交流过压警告
					{	
						ubACVoltStatus_C=ACVoltHigh;
						ubACHighCnt_C=0;
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						if((DefalutPhaseFlag&0x20)==0) 
						{
							DefalutPhaseFlag^=0x20;
							PackAlarmRecord(VOL_CEXCESS);
						}										
					}
				}  
				ReturnStatus = ubACVoltStatus_C;  			
    		}
    	break;
    	default: break;
    }
    return ReturnStatus;
}

/********************************************************************************* 
 函数名称： DealOverCurrent
 功能描述：	检测交流电流,是否过过流,欠流.//1秒执行
 输    入：	ffDcCur：当前电流，VolPhase：相位
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void DealOverCurrent(const float fDcCur,ST_INT32 VolPhase)
{
	static unsigned char  	ubDcCurHighCnt_A=0;
	static unsigned char  	ubDcCurHighCnt_B=0;
	static unsigned char  	ubDcCurHighCnt_C=0;		
	static unsigned char	ubDcCurBackCnt_A=0;
	static unsigned char	ubDcCurBackCnt_B=0;
	static unsigned char	ubDcCurBackCnt_C=0;		
	static unsigned char ucStatus_A;
	static unsigned char ucStatus_B;
	static unsigned char ucStatus_C;	
	switch(VolPhase)
	{
		case 1:
			{
				if((!ucStatus_A)&&(fDcCur<=HoutaiData.fOverCurLimit))
				{
				  	ubDcCurHighCnt_A=0;
					ubDcCurBackCnt_A=0;
					ucStatus_A=0;//电流正常
				}
				else if(ucStatus_A&&(fDcCur<=HoutaiData.fOverCurLimit))		//A相过流恢复
				{
				    ubDcCurHighCnt_A=0;
					if(ubDcCurBackCnt_A++>10)
					{
					    ucStatus_A=0;//电流恢复正常
						ubDcCurBackCnt_A=0;
				//		EventFlag.MeterExcepFlag^=0x02;

						if((DefalutPhaseFlag&0x40)==0x40) 
						{
							DefalutPhaseFlag^=0x40;
							PackAlarmRecord(CUR_AEXCESSHF);
						}				
			
					}
				}	
				else if(fDcCur>HoutaiData.fOverCurLimit)   //C相过流
				{
					ubDcCurBackCnt_A=0;
				    ubDcCurHighCnt_A++;
					printf("over current cout is %d\n",ubDcCurHighCnt_A);
					if(ubDcCurHighCnt_A>10)//持续10S后过流警告
					{
						ucStatus_A=1;
						ubDcCurHighCnt_A=0;
						printf("EventFlag.MeterExcepFlag is %2x\n",EventFlag.MeterExcepFlag);
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						printf("EventFlag.MeterExcepFlag is %2x\n",EventFlag.MeterExcepFlag);
						if((DefalutPhaseFlag & 0x40)==0) 
						{
							DefalutPhaseFlag^=0x40;
							PackAlarmRecord(CUR_AEXCESS);
							printf("over current alarm\n");
							printf("DefalutPhaseFlag is %2x\n",DefalutPhaseFlag);
						}												
					}
				}											
			}
		break;
		case 2:
			{
				if((!ucStatus_B)&&(fDcCur<=HoutaiData.fOverCurLimit))
				{
				  	ubDcCurHighCnt_B=0;
					ubDcCurBackCnt_B=0;
					ucStatus_B=0;//电流正常
				}
				else if(ucStatus_B&&(fDcCur<=HoutaiData.fOverCurLimit))						//A相过流恢复
				{
				    ubDcCurHighCnt_B=0;
					if(ubDcCurBackCnt_B++>10)
					{
					    ucStatus_B=0;//电流恢复正常
						ubDcCurBackCnt_B=0;
				//		EventFlag.MeterExcepFlag^=0x02;

						if((DefalutPhaseFlag&0x80)==0x80) 
						{
							DefalutPhaseFlag^=0x80;
							PackAlarmRecord(CUR_BEXCESSHF);
						}				
			
					}
				}	
				else if(fDcCur>HoutaiData.fOverCurLimit)   //C相过流
				{
					ubDcCurBackCnt_B=0;
				    ubDcCurHighCnt_B++;
					if(ubDcCurHighCnt_B>10)//持续10S后过流警告
					{
						ucStatus_B=1;
						ubDcCurHighCnt_B=0;
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						if((DefalutPhaseFlag&0x80)==0) 
						{
							DefalutPhaseFlag=0x80;
							PackAlarmRecord(CUR_BEXCESS);
						}												
					}
				}				
			}
		break;
		case 3:
			{
				if((!ucStatus_C)&&(fDcCur<=HoutaiData.fOverCurLimit))
				{
				  	ubDcCurHighCnt_C=0;
					ubDcCurBackCnt_C=0;
					ucStatus_C=0;//电流正常
				}
				else if(ucStatus_C&&(fDcCur<=HoutaiData.fOverCurLimit))						//A相过流恢复
				{
				    ubDcCurHighCnt_C=0;
					if(ubDcCurBackCnt_C++>10)
					{
					    ucStatus_C=0;//电流恢复正常
						ubDcCurBackCnt_C=0;
				//		EventFlag.MeterExcepFlag^=0x02;

						if((DefalutPhaseFlag&0x100)==0x100) 
						{
							DefalutPhaseFlag^=0x100;
							PackAlarmRecord(CUR_CEXCESSHF);
						}				
			
					}
				}	
				else if(fDcCur>HoutaiData.fOverCurLimit)   //C相过流
				{
					ubDcCurBackCnt_C=0;
				    ubDcCurHighCnt_C++;
					if(ubDcCurHighCnt_C>10)//持续10S后过流警告
					{
						ucStatus_C=1;
						ubDcCurHighCnt_C=0;
				//		EventFlag.MeterExcepFlag^=0x02;
					//	ChargeMsg.PaleStatus = PALEEXCEPTION;
						if((DefalutPhaseFlag&0x100)==0) 
						{
							DefalutPhaseFlag^=0x100;
							PackAlarmRecord(CUR_CEXCESS);
						}												
					}
				}				
			}
		break;
		default:break;				
		
	}
	
}

/********************************************************************************* 
 函数名称： GetAdressCommand
 功能描述：	打包获取表地址命令帧
 输    入：	无
 输	   出： 打包好的buffer
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_UCHAR* GetAdressCommand()
{
	ST_UCHAR bCommandLen=0;
	MeterMsg.bAddrCmd[bCommandLen++] = 0x68;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xAA;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xAA;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xAA;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xAA;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xAA;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xAA;
	MeterMsg.bAddrCmd[bCommandLen++] = 0x68;
	MeterMsg.bAddrCmd[bCommandLen++] = 0x13;
	MeterMsg.bAddrCmd[bCommandLen++] = 0x00;
	MeterMsg.bAddrCmd[bCommandLen++] = 0xDF;
	MeterMsg.bAddrCmd[bCommandLen++] = 0x16;
	return MeterMsg.bAddrCmd;
}

/********************************************************************************* 
 函数名称： PackSendMeterData
 功能描述：	打包表发送命令的数据帧
 输    入：	DataId：数据标识
 输	   出： 发送数据长度
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_UCHAR PackSendMeterData(ST_UCHAR *DataId)
{
	Uart[EXBOARDCOM].sendbuf[0] =METER_STX;
	memcpy(&Uart[EXBOARDCOM].sendbuf[1], MeterMsg.bAddress, 6);
	Uart[EXBOARDCOM].sendbuf[7] = METER_STX;
	if(ProtocolType==1)
	{
		Uart[EXBOARDCOM].sendbuf[8] = 0x11;
		Uart[EXBOARDCOM].sendbuf[9] = 0x04;
		memcpy(&Uart[EXBOARDCOM].sendbuf[10],DataId,4);
		Uart[EXBOARDCOM].sendbuf[14]  = CheckSum(Uart[EXBOARDCOM].sendbuf,14);
		Uart[EXBOARDCOM].sendbuf[15] = METER_ETX;
		return 16;
	}
	else
	{
		Uart[EXBOARDCOM].sendbuf[8] = 0x01;
		Uart[EXBOARDCOM].sendbuf[9] = 0x02;
		memcpy(&Uart[EXBOARDCOM].sendbuf[10],DataId,2);	
		Uart[EXBOARDCOM].sendbuf[12] = CheckSum(Uart[EXBOARDCOM].sendbuf, 12);
		Uart[EXBOARDCOM].sendbuf[13] = METER_ETX;
		return 14;
	}



}

/********************************************************************************* 
 函数名称： MeterComSend
 功能描述：	发送线程回调函数，发送度电压、电流、电量等命令
 输    入：	DataId：数据标识
 输	   出： 发送数据长度
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void MeterComSend(void)
{
	ST_UCHAR DataID[4],SendDataLen;
	ST_UCHAR i;
	
	//EventFlag.MeterExcepFlag = 0;

	memset(Uart[EXBOARDCOM].sendbuf,0,sizeof(Uart[EXBOARDCOM].sendbuf));
	if(MeterMsg.SendCount<3)
	{
		switch(MeterMsg.MeterCmd)
		{
			case GETADDR:
			{
				write(ExBoardfd, GetAdressCommand(), 12);
				printf("SendAddrcmd\n");

				MeterMsg.Rcvfinishflag = 0;	
				usleep(300*1000);

			}
			break;
			case READ_RATE_TO_TIME:
			{
				DataID[0]=0x34;DataID[1]=0x33;DataID[2] = 0x34;DataID[3] = 0x37;
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				printf("READ_RATE_TO_TIME:");
				for(i = 0;i<SendDataLen;i++)
				{printf("%02X",Uart[EXBOARDCOM].sendbuf[i]);}
				printf("\n");
				 MeterMsg.Rcvfinishflag = 0;
				usleep(500*1000);

			}
			break;
			case READCUR_A:
			{
				if(ProtocolType==1)
				{ 
					DataID[0]=0x33;DataID[1]=0x34;DataID[2] = 0x35;DataID[3] = 0x35;
				}
				else
				{
                	DataID[0]=0x54;DataID[1]=0xE9;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				printf("sendReadCur_A:");
				for(i = 0;i<SendDataLen;i++)
				{printf("%02X",Uart[EXBOARDCOM].sendbuf[i]);}
				printf("\n");
				 MeterMsg.Rcvfinishflag = 0;
				usleep(300*1000);

			}
			break;
			case READCUR_B:
			{
				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x35;DataID[2] = 0x35;DataID[3] = 0x35;
				}
				else
				{
                	DataID[0]=0x55;DataID[1]=0xE9;
            	}
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				printf("sendReadCur_B:");
                 MeterMsg.Rcvfinishflag = 0;	
                 usleep(300*1000);				
			}
			break;
			case READCUR_C:
			{	 
				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x36;DataID[2] = 0x35;DataID[3] = 0x35;
				}
				else
				{
                	DataID[0]=0x56;DataID[1]=0xE9;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				printf("sendReadCur_C:");
				MeterMsg.Rcvfinishflag = 0;
				usleep(300*1000);

			}
			break;
			case READVOL_A:
			{

				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x34;DataID[2] = 0x34;DataID[3] = 0x35;
				}
				else
				{
                	DataID[0]=0x44;DataID[1]=0xE9;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				printf("sendReadREADVOL_A:");
				MeterMsg.Rcvfinishflag = 0;
				usleep(300*1000);
			
			}
			break;
			case READVOL_B:
			{

				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x35;DataID[2] = 0x34;DataID[3] = 0x35;
				}
				else
				{
                	DataID[0]=0x45;DataID[1]=0xE9;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;	
				usleep(300*1000);			
			
			}
			break;
			case READVOL_C:
			{

				if(ProtocolType==1)
				{				
					DataID[0]=0x33;DataID[1]=0x36;DataID[2] = 0x34;DataID[3] = 0x35;
				}
				else
				{
              		DataID[0]=0x46;DataID[1]=0xE9;
                }
				SendDataLen = PackSendMeterData(DataID);
				printf("READVOL_C::");
                for(i = 0;i<SendDataLen;i++)
                {
                	printf("%02X",Uart[EXBOARDCOM].sendbuf[i]);
                	
                }
                printf("\n");

				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;	
				usleep(300*1000);				

			}
			break;
			case READTOTAL_POWER:
			{
				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x33;DataID[2] = 0x34;DataID[3] = 0x33;
				}
				else
				{
		               		DataID[0]=0x43;DataID[1]=0xC3;
        		        }
				SendDataLen = PackSendMeterData(DataID);
				printf("READTOTAL_POWER:");
                for(i = 0;i<SendDataLen;i++)
                {printf("%02X",Uart[EXBOARDCOM].sendbuf[i]);}
                printf("\n");

				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;
				usleep(300*1000);					

			}
			break;
			case READ_POWER1:
			{

				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x34;DataID[2] = 0x34;DataID[3] = 0x33;
				}
				else
				{
                	DataID[0]=0x44;DataID[1]=0xC3;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;	
				usleep(300*1000);		

			}
			break;
			case READ_POWER2:
			{

				if(ProtocolType==1)
				{ 
					DataID[0]=0x33;DataID[1]=0x35;DataID[2] = 0x34;DataID[3] = 0x33;
				}
				else
				{
                	DataID[0]=0x45;DataID[1]=0xC3;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;	
				usleep(300*1000);			

			}
			break;
			case READ_POWER3:
			{

				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x36;DataID[2] = 0x34;DataID[3] = 0x33;
				}
				else
				{
                	DataID[0]=0x46;DataID[1]=0xC3;
                }
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;			
				usleep(300*1000);	

			}
			break;
			case READ_POWER4:
			{

				if(ProtocolType==1)
				{
					DataID[0]=0x33;DataID[1]=0x37;DataID[2] = 0x34;DataID[3] = 0x33;
				}
				else
				{
               				DataID[0]=0x47;DataID[1]=0xC3;
                		}
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;		
				usleep(300*1000);		

			}
			break;
		#if 1
			if(ProtocolType==1)
			{
				case READ_STATUS_A:
				{
					DataID[0]=0x37;DataID[1]=0x38;DataID[2] = 0x33;DataID[3] = 0x37;
					SendDataLen = PackSendMeterData(DataID);
					write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
					MeterMsg.Rcvfinishflag = 0;	
					usleep(300*1000);		
				}
				break;
				case READ_STATUS_B:
				{
					DataID[0]=0x38;DataID[1]=0x38;DataID[2] = 0x33;DataID[3] = 0x37;
					SendDataLen = PackSendMeterData(DataID);
					write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
					MeterMsg.Rcvfinishflag = 0;	
					usleep(300*1000);			
				}
				break;
				case READ_STATUS_C:
				{
					DataID[0]=0x39;DataID[1]=0x38;DataID[2] = 0x33;DataID[3] = 0x37;
					SendDataLen = PackSendMeterData(DataID);
					write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
					MeterMsg.Rcvfinishflag = 0;	
					usleep(300*1000);		
				}
				break;
							
			}
			else
			{
			 	case READ_STATUS_97:
			 	{
                	DataID[0]=0x54;DataID[1]=0xF3;
					SendDataLen = PackSendMeterData(DataID);
					write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
					MeterMsg.Rcvfinishflag = 0;	
					usleep(300*1000);
			 		
			 	}
			 	break;
				
			}
		#endif
			case READ_CURENTTIME_SMH:
			{
				DataID[0]=0x35;DataID[1]=0x34;DataID[2] = 0x33;DataID[3] = 0x37;
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;
				usleep(300*1000);
				
			}	
			break;						
			case READ_CURENTTIME_WDMY:
			{
				DataID[0]=0x34;DataID[1]=0x34;DataID[2] = 0x33;DataID[3] = 0x37;
				SendDataLen = PackSendMeterData(DataID);
				write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, SendDataLen);
				MeterMsg.Rcvfinishflag = 0;		
				usleep(300*1000);				
				
			}	
			break;	
							
			default:break;
		}
		MeterMsg.SendCount++;
	}
	else
	{
		MeterMsg.SendCount = 0;
		if((EventFlag.MeterExcepFlag&0x01)==0)
		{
			EventFlag.MeterExcepFlag^=0x01;
			if( DISPIDLE == DisplayInfo.CurrentPage|| ChargeMsg.PaleStatus == PALECHARGING )
			{
				ChargeMsg.PaleStatus = PALEEXCEPTION;
			}
			InitNewAlarmRecord(0x00,DBTXGZ,NULL,0); //上传电表通信故障		
		}		

	}

	
}

/********************************************************************************* 
 函数名称： AnasysMeterAddr
 功能描述：	解析读取表地址的返回帧
 输    入：	无
 输	   出： 1：读取表地址成功，0：读取表地址失败
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_UCHAR AnasysMeterAddr(void)
{
	ST_UCHAR i=0;
	while(Uart[EXBOARDCOM].revbuf[i]==0xfe)
	{
		i++;
	}
	memcpy(MeterMsg.Rcvbuffer,&Uart[EXBOARDCOM].revbuf[i],Uart[EXBOARDCOM].rxnum-i);
	if(Uart[EXBOARDCOM].rxnum-i== 0x12 && 
	   *MeterMsg.Rcvbuffer == 0x68 &&
	   *(MeterMsg.Rcvbuffer + 7) == 0x68 &&
	   *(MeterMsg.Rcvbuffer + 8) == 0x93 &&
	   *(MeterMsg.Rcvbuffer + 9) == 0x06 &&
	   *(MeterMsg.Rcvbuffer + 16) == CheckSum(MeterMsg.Rcvbuffer,16) &&
	   *(MeterMsg.Rcvbuffer + 17) == 0x16)
	{
		printf("address OK\n");
		MeterMsg.SendCount = 0;
		memcpy(MeterMsg.bAddress, MeterMsg.Rcvbuffer + 10, 6);
		for(i = 0;i<6;i++)
		{
			MeterMsg.bAddress[i]-=0x33;
		}
		return 1;
	}
	printf("address Failed\n");
	
	return 0;
}


/********************************************************************************* 
 函数名称： AnasysMeterCmd
 功能描述：	解析接收数据帧
 输    入：	无
 输	   出： 1：接收数据正确，0：接收数据失败
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_UCHAR AnasysMeterCmd(void)
{
	ST_UCHAR i=0;
	while(Uart[EXBOARDCOM].revbuf[i]==0xfe)
	{
		i++;
	}
	memcpy(MeterMsg.Rcvbuffer,&Uart[EXBOARDCOM].revbuf[i],Uart[EXBOARDCOM].rxnum-i);
	if(ProtocolType==1)
	{	
		if(Uart[EXBOARDCOM].rxnum-i == *(MeterMsg.Rcvbuffer + 9) + 12 && 
		*MeterMsg.Rcvbuffer == 0x68 &&
		*(MeterMsg.Rcvbuffer + 1) == *MeterMsg.bAddress &&
		*(MeterMsg.Rcvbuffer + 2) == *(MeterMsg.bAddress + 1) &&
		*(MeterMsg.Rcvbuffer + 3) == *(MeterMsg.bAddress + 2) &&
		*(MeterMsg.Rcvbuffer+ 4) == *(MeterMsg.bAddress + 3) &&
		*(MeterMsg.Rcvbuffer + 5) == *(MeterMsg.bAddress + 4) &&
		*(MeterMsg.Rcvbuffer+ 6) == *(MeterMsg.bAddress + 5) &&
		*(MeterMsg.Rcvbuffer + 7) == 0x68 &&
		*(MeterMsg.Rcvbuffer + 8) == 0x91 &&	   
		*(MeterMsg.Rcvbuffer + Uart[EXBOARDCOM].rxnum-i- 2) == CheckSum(MeterMsg.Rcvbuffer,Uart[EXBOARDCOM].rxnum-i- 2) &&
		*(MeterMsg.Rcvbuffer + Uart[EXBOARDCOM].rxnum-i- 1) == 0x16)
		{
			if(*(MeterMsg.Rcvbuffer+10) == 0x33 && *(MeterMsg.Rcvbuffer+11) == 0x34 && *(MeterMsg.Rcvbuffer+12) == 0x35 && *(MeterMsg.Rcvbuffer+13)==0x35)
			{
				MeterMsg.MeterCmd = READCUR_A;
			}
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x35&&*(MeterMsg.Rcvbuffer+12)==0x35&&*(MeterMsg.Rcvbuffer+13)==0x35)
			{
				MeterMsg.MeterCmd = READCUR_B;
			}			
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x36&&*(MeterMsg.Rcvbuffer+12)==0x35&&*(MeterMsg.Rcvbuffer+13)==0x35)
			{
				MeterMsg.MeterCmd = READCUR_C;
			}	
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x34&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x35)
			{
				MeterMsg.MeterCmd = READVOL_A;
			}
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x35&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x35)
			{
				MeterMsg.MeterCmd = READVOL_B;
			}	
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x36&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x35)
			{
				MeterMsg.MeterCmd = READVOL_C;
			}	
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x33&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x33)
			{
				MeterMsg.MeterCmd = READTOTAL_POWER;
			}
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x34&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x33)
			{
				MeterMsg.MeterCmd = READ_POWER1;
			}
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x35&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x33)
			{
				MeterMsg.MeterCmd = READ_POWER2;
			}		
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x36&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x33)
			{
				MeterMsg.MeterCmd = READ_POWER3;
			}	
			if(*(MeterMsg.Rcvbuffer+10)==0x33&&*(MeterMsg.Rcvbuffer+11)==0x37&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x33)
			{
				MeterMsg.MeterCmd = READ_POWER4;
			}	
			if(*(MeterMsg.Rcvbuffer+10)==0x37&&*(MeterMsg.Rcvbuffer+11)==0x38&&*(MeterMsg.Rcvbuffer+12)==0x33&&*(MeterMsg.Rcvbuffer+13)==0x37)
			{
				MeterMsg.MeterCmd = READ_STATUS_A;
			}		
			if(*(MeterMsg.Rcvbuffer+10)==0x38&&*(MeterMsg.Rcvbuffer+11)==0x38&&*(MeterMsg.Rcvbuffer+12)==0x33&&*(MeterMsg.Rcvbuffer+13)==0x37)
			{
				MeterMsg.MeterCmd = READ_STATUS_B;
			}		
			if(*(MeterMsg.Rcvbuffer+10)==0x39&&*(MeterMsg.Rcvbuffer+11)==0x38&&*(MeterMsg.Rcvbuffer+12)==0x33&&*(MeterMsg.Rcvbuffer+13)==0x37)
			{
				MeterMsg.MeterCmd = READ_STATUS_C;
			}	
			if(*(MeterMsg.Rcvbuffer+10)==0x35&&*(MeterMsg.Rcvbuffer+11)==0x34&&*(MeterMsg.Rcvbuffer+12)==0x33&&*(MeterMsg.Rcvbuffer+13)==0x37)
			{
				MeterMsg.MeterCmd = READ_CURENTTIME_SMH;
			}		
			if(*(MeterMsg.Rcvbuffer+10)==0x34&&*(MeterMsg.Rcvbuffer+11)==0x33&&*(MeterMsg.Rcvbuffer+12)==0x34&&*(MeterMsg.Rcvbuffer+13)==0x37)
			{
				MeterMsg.MeterCmd = READ_RATE_TO_TIME;
			}																						
			MeterMsg.SendCount = 0;
			printf("CmdRev OK\n");
			return 1;
		}
	}
	else
	{
       if(Uart[EXBOARDCOM].rxnum-i == *(MeterMsg.Rcvbuffer + 9) + 12 &&
        *MeterMsg.Rcvbuffer == 0x68 &&
        *(MeterMsg.Rcvbuffer + 1) == *MeterMsg.bAddress &&
        *(MeterMsg.Rcvbuffer + 2) == *(MeterMsg.bAddress + 1) &&
        *(MeterMsg.Rcvbuffer + 3) == *(MeterMsg.bAddress + 2) &&
        *(MeterMsg.Rcvbuffer+ 4) == *(MeterMsg.bAddress + 3) &&
        *(MeterMsg.Rcvbuffer + 5) == *(MeterMsg.bAddress + 4) &&
        *(MeterMsg.Rcvbuffer+ 6) == *(MeterMsg.bAddress + 5) &&
        *(MeterMsg.Rcvbuffer + 7) == 0x68 &&
        *(MeterMsg.Rcvbuffer + 8) == 0x81 &&
        *(MeterMsg.Rcvbuffer + Uart[EXBOARDCOM].rxnum-i- 2) == CheckSum(MeterMsg.Rcvbuffer,Uart[EXBOARDCOM].rxnum-i- 2) &&
        *(MeterMsg.Rcvbuffer + Uart[EXBOARDCOM].rxnum-i- 1) == 0x16)
        {
        	
        	if(*(MeterMsg.Rcvbuffer+10)==0x54&&*(MeterMsg.Rcvbuffer+11)==0xE9)
        	{
        		MeterMsg.MeterCmd = READCUR_A;
        	}
        	if(*(MeterMsg.Rcvbuffer+10)==0x55&&*(MeterMsg.Rcvbuffer+11)==0xE9)
        	{
        		MeterMsg.MeterCmd = READCUR_B;
        	}  
        	if(*(MeterMsg.Rcvbuffer+10)==0x56&&*(MeterMsg.Rcvbuffer+11)==0xE9)
        	{
        		MeterMsg.MeterCmd = READCUR_C;
        	}   
        	if(*(MeterMsg.Rcvbuffer+10)==0x44&&*(MeterMsg.Rcvbuffer+11)==0xE9)
        	{
        		MeterMsg.MeterCmd = READVOL_A;
        	}  
        	if(*(MeterMsg.Rcvbuffer+10)==0x45&&*(MeterMsg.Rcvbuffer+11)==0xE9)
        	{
        		MeterMsg.MeterCmd = READVOL_B;
        	}
        	if(*(MeterMsg.Rcvbuffer+10)==0x46&&*(MeterMsg.Rcvbuffer+11)==0xE9)
        	{
        		MeterMsg.MeterCmd = READVOL_C;
        	}     
        	if(*(MeterMsg.Rcvbuffer+10)==0x43&&*(MeterMsg.Rcvbuffer+11)==0xC3)
        	{
        		MeterMsg.MeterCmd = READTOTAL_POWER;
        	}   
        	if(*(MeterMsg.Rcvbuffer+10)==0x44&&*(MeterMsg.Rcvbuffer+11)==0xC3)
        	{
        		MeterMsg.MeterCmd = READ_POWER1;
        	} 
        	if(*(MeterMsg.Rcvbuffer+10)==0x45&&*(MeterMsg.Rcvbuffer+11)==0xC3)
        	{
        		MeterMsg.MeterCmd = READ_POWER2;
        	} 
        	if(*(MeterMsg.Rcvbuffer+10)==0x46&&*(MeterMsg.Rcvbuffer+11)==0xC3)
        	{
        		MeterMsg.MeterCmd = READ_POWER3;
        	} 
        	if(*(MeterMsg.Rcvbuffer+10)==0x47&&*(MeterMsg.Rcvbuffer+11)==0xC3)
        	{
        		MeterMsg.MeterCmd = READ_POWER4;
        	}      
        	if(*(MeterMsg.Rcvbuffer+10)==0x54&&*(MeterMsg.Rcvbuffer+11)==0xF3)
        	{
        		MeterMsg.MeterCmd = READ_STATUS_97;
        	}        	   	        	        	        	     	   	        	      	    	
			MeterMsg.SendCount = 0;
            printf("CmdRev OK\n");
            return 1;
        }
	}
	printf("CmdRev Failed\n");
	return 0;
}





/********************************************************************************* 
 函数名称： MeterComRcv
 功能描述：	表接收线程处理函数
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void MeterComRcv(void)
{
	ST_CHAR bTemp[5];
	//ST_UCHAR AlarmRes;
	ST_UCHAR i;

	if(MeterMsg.MeterCmd!=GETADDR)
	{
		
		if(AnasysMeterCmd())
		{
			if((EventFlag.MeterExcepFlag&0x01)==1)
			{
				EventFlag.MeterExcepFlag^=0x01;
				InitNewAlarmRecord(0x00,0x01B8,NULL,0); 
			}
			MeterMsg.SendCount = 0; 
		}
		else
		{
			return;
		}
	}					
	switch(MeterMsg.MeterCmd)
	{
		case GETADDR:
		{
			printf("Addr:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");
			
			if(AnasysMeterAddr())
			{
				if((EventFlag.MeterExcepFlag&0x01)==1)
				{
					EventFlag.MeterExcepFlag^=0x01;
					InitNewAlarmRecord(0x00,0x01B8,NULL,0); 
				}
				MeterMsg.SendCount = 0; 
				//MeterMsg.MeterCmd = READ_RATE_TO_TIME;
				MeterMsg.MeterCmd = READTOTAL_POWER;
				printf("MeterMsg.MeterCmd is %d",MeterMsg.MeterCmd);
				MeterMsg.Rcvfinishflag = 1;	
			}			
			
		}
		break;
		case READ_RATE_TO_TIME:
		{
			
			printf("Rate to time：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("EventFlag.MeterExcepFlag is %d\n",EventFlag.MeterExcepFlag);
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{		
			#if 1	
				for(i=0;i<14*3;i=i+3)
				{
					int j = 0;
					*bTemp = *(MeterMsg.Rcvbuffer + 14+i) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15+i) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16+i) - 0x33;
					MeterMsg.RateToTime[j].RateIndex = CDZ_BCDTOHEX(*bTemp);
					MeterMsg.RateToTime[j].RateToTime_Min = CDZ_BCDTOHEX(*(bTemp+1));
					MeterMsg.RateToTime[j].RateToTime_Hour = CDZ_BCDTOHEX(*(bTemp+2));
					j++;
					//printf("MeterMsg.MeterCurrentTime is %d:%d,rate is %d\n",MeterMsg.MeterCurrentTime[2],MeterMsg.MeterCurrentTime[1],MeterMsg.RateToTime[0]);
				}																 
			#endif
				MeterMsg.MeterCmd = READCUR_A;
				printf("MeterMsg.MeterCmd is %d\n",MeterMsg.MeterCmd);
			}
			MeterMsg.Rcvfinishflag = 1;	
			
		}
		break;
		case READCUR_A:
		{
			printf("Cur_A:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");
			printf("EventFlag.MeterExcepFlag is %d\n",EventFlag.MeterExcepFlag);
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					if((*(MeterMsg.Rcvbuffer + 16)&0x80)==0x80)
					{
						*(MeterMsg.Rcvbuffer + 16) = *(MeterMsg.Rcvbuffer + 16)^0x80;
					}
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 16) - 0x33;
					*(bTemp + 3) = 0x00;

				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 13) - 0x33;
					*(bTemp + 2) = 0x00;
					*(bTemp + 3) = 0x00;
				}

				MeterMsg.fCur_A = ((float)BCDToInt(bTemp))/1000;
				printf("fCur_A:%6.3f\n",fabs(MeterMsg.fCur_A));

				MeterMsg.MeterCmd = READCUR_B;
				DealOverCurrent(fabs(MeterMsg.fCur_A),1);
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0xB621, (MeterMsg.Rcvbuffer + 14));

			}					
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READCUR_B:
		{
			printf("Cur_B:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					if((*(MeterMsg.Rcvbuffer + 16)&0x80)==0x80)
					{
						*(MeterMsg.Rcvbuffer + 16) = *(MeterMsg.Rcvbuffer + 16)^0x80;
					}
					 *(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 16) - 0x33;

					*(bTemp + 3) = 0x00;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 13) - 0x33;
					*(bTemp + 2) = 0x00;
					*(bTemp + 3) = 0x00;
				}
				MeterMsg.fCur_B = ((float)BCDToInt(bTemp))/1000;
				printf("fCur_B:%6.3f\n",MeterMsg.fCur_B);
				DealOverCurrent(fabs(MeterMsg.fCur_B),2);
				MeterMsg.MeterCmd = READCUR_C;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0xB622, (MeterMsg.Rcvbuffer + 14));

			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READCUR_C:
		{
			printf("Cur_C:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
						
					if((*(MeterMsg.Rcvbuffer + 16)&0x80)==0x80)
					{
						printf("MeterMsg.Rcvbuffer + 16 qian is %2x\n",*(MeterMsg.Rcvbuffer + 16));
						*(MeterMsg.Rcvbuffer + 16) = *(MeterMsg.Rcvbuffer + 16)^0x80;
						 printf("MeterMsg.Rcvbuffer + 16 hou is %2x\n",*(MeterMsg.Rcvbuffer + 16));

					}
					 *(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 16) - 0x33;

					*(bTemp + 3) = 0x00;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer+ 13) - 0x33;
					*(bTemp + 2) = 0x00;
					*(bTemp + 3) = 0x00;
				}
				MeterMsg.fCur_C = ((float)BCDToInt(bTemp))/1000.0;
				printf("fCur_C:%6.3f\n",MeterMsg.fCur_C);
				DealOverCurrent(fabs(MeterMsg.fCur_C),3);
				MeterMsg.MeterCmd = READVOL_A;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0xB623, (MeterMsg.Rcvbuffer + 14));

			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READVOL_A:
		{
			printf("Vol_A:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer+ 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = 0x00;;
					*(bTemp + 3) = 0x00;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 13) - 0x33;
					*(bTemp + 2) = 0x00;
					*(bTemp + 3) = 0x00;
				}
				MeterMsg.fVol_A = ((float)BCDToInt(bTemp))/10.0;
				printf("fVol_A:%4.1f\n",MeterMsg.fVol_A);
				DealOver_Low_Vol(MeterMsg.fVol_A,1);
				MeterMsg.MeterCmd = READVOL_B;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0xB611, (MeterMsg.Rcvbuffer + 14));
				if(MeterMsg.fVol_A==0&&MeterMsg.fVol_B>METER_DUANLU_LIMIT&&MeterMsg.fVol_C>METER_DUANLU_LIMIT)
				{
					if((DefalutPhaseFlag&0x1000)==0) //0:上次A相短路
					{
						DefalutPhaseFlag^=0x1000; 
						PackAlarmRecord(PHASE_A_DUANLU);  
													
					}																
				}
				else if((DefalutPhaseFlag&0x1000)==1) //1:上次A相缺相，已上报
				{ 
					DefalutPhaseFlag^=0x1000; 
					PackAlarmRecord(PHASE_A_DUANLU_HF);							
				}
			}	
		
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READVOL_B:
		{
			printf("Vol_B:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = 0x00;;
					*(bTemp + 3) = 0x00;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer+ 13) - 0x33;
					*(bTemp + 2) = 0x00;
					*(bTemp + 3) = 0x00;
				}
				MeterMsg.fVol_B = ((float)BCDToInt(bTemp))/10.0;
				printf("fVol_B:%4.1f\n",MeterMsg.fVol_B);
				DealOver_Low_Vol(MeterMsg.fVol_B,2);
				MeterMsg.MeterCmd = READVOL_C;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0xB612, (MeterMsg.Rcvbuffer + 14));
				if(MeterMsg.fVol_B==0&&MeterMsg.fVol_A>METER_DUANLU_LIMIT&&MeterMsg.fVol_C>METER_DUANLU_LIMIT)
				{
					if((DefalutPhaseFlag&0x2000)==0) //0:上次B相短路
					{
						DefalutPhaseFlag^=0x2000; 
						PackAlarmRecord(PHASE_B_DUANLU);  
													
					}																
				}
				else if((DefalutPhaseFlag&0x2000)==1) //1:上次B相缺相，已上报
				{ 
					DefalutPhaseFlag^=0x2000; 
					PackAlarmRecord(PHASE_B_DUANLU_HF);							
				}						

			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READVOL_C:
		{
			printf("Vol_C:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = 0x00;;
					*(bTemp + 3) = 0x00;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 13) - 0x33;
					*(bTemp + 2) = 0x00;
					*(bTemp + 3) = 0x00;
				}
				MeterMsg.fVol_C = ((float)BCDToInt(bTemp))/10.0;
				printf("fVol_C:%4.1f\n",MeterMsg.fVol_C);
				DealOver_Low_Vol(MeterMsg.fVol_C,3);
				//MeterMsg.MeterCmd = READTOTAL_POWER;
				MeterMsg.MeterCmd = READ_STATUS_A;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0xB613, (MeterMsg.Rcvbuffer + 14));
				if(MeterMsg.fVol_C==0&&MeterMsg.fVol_A>METER_DUANLU_LIMIT&&MeterMsg.fVol_B>METER_DUANLU_LIMIT)
				{
					if((DefalutPhaseFlag&0x4000)==0) //0:上次C相短路
					{
						DefalutPhaseFlag^=0x4000; 
						PackAlarmRecord(PHASE_C_DUANLU);  
													
					}																
				}
				else if((DefalutPhaseFlag&0x4000)==1) //1:上次C相缺相，已上报
				{ 
					DefalutPhaseFlag^=0x4000; 
					PackAlarmRecord(PHASE_C_DUANLU_HF);							
				}
			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READTOTAL_POWER:
		{
			printf("Total Power:");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 16) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 17) - 0x33;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer+ 13) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer+ 15) - 0x33;
				}
				MeterMsg.fPositiveTotalPower = ((float)BCDToInt(bTemp))/100.00;
				printf("MeterMsg.fPositiveTotalPower is %9.2f\n",MeterMsg.fPositiveTotalPower);
				//MeterMsg.MeterCmd = READ_POWER1;
				MeterMsg.MeterCmd = READCUR_A;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0x9010, (MeterMsg.Rcvbuffer + 14));

			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READ_POWER1:
		{
			printf("Power1：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 17) - 0x33;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer+ 13) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
				}
				MeterMsg.fPositivePower1 = ((float)BCDToInt(bTemp))/100.00;
				 printf("MeterMsg.fPositivePower1 is %9.2f\n",MeterMsg.fPositivePower1);

				MeterMsg.MeterCmd = READ_POWER2;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0x9011, (MeterMsg.Rcvbuffer + 14));

			}
	
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READ_POWER2:
		{
			printf("Power2：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");				
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer+ 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 17) - 0x33;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 13) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 14) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer+ 15) - 0x33;
				}
				MeterMsg.fPositivePower2 = ((float)BCDToInt(bTemp))/100.00;
				 printf("MeterMsg.fPositivePower2 is %9.2f\n",MeterMsg.fPositivePower2);
				MeterMsg.MeterCmd = READ_POWER3;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0x9012, (MeterMsg.Rcvbuffer + 14));
	
			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READ_POWER3:
		{
			printf("Power3：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");				
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 17) - 0x33;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer+ 13) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 14) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer+ 15) - 0x33;
				}
				MeterMsg.fPositivePower3 = ((float)BCDToInt(bTemp))/100.00;
				 printf("MeterMsg.fPositivePower3 is %9.2f\n",MeterMsg.fPositivePower3);	
				MeterMsg.MeterCmd = READ_POWER4;
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0x9013, (MeterMsg.Rcvbuffer + 14));

			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
		case READ_POWER4:
		{
			printf("Power4：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");					
			
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{
				if(ProtocolType==1)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 17) - 0x33;
				}
				else
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 12) - 0x33;
					*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 13) - 0x33;
					*(bTemp + 2) = *(MeterMsg.Rcvbuffer+ 14) - 0x33;
					*(bTemp + 3) = *(MeterMsg.Rcvbuffer+ 15) - 0x33;
				}
				MeterMsg.fPositivePower4 = ((float)BCDToInt(bTemp))/100.00;
				 printf("MeterMsg.fPositivePower4 is %9.2f\n",MeterMsg.fPositivePower4);
				if(ProtocolType==1)
				{
					MeterMsg.MeterCmd = READ_STATUS_A;//READ_STATUS_A;
				}
				else
				{   
					MeterMsg.MeterCmd = READ_STATUS_97;
				}
				cdzWriteDataItemContentNoAuth(CDZ_AC_METER_MEA_POINT, 0x9014, (MeterMsg.Rcvbuffer + 14));

			}
			MeterMsg.Rcvfinishflag = 1;	
		}
		break;
	#if 1
		if(ProtocolType==1)
		{
			case READ_STATUS_A:
			{
				printf("Status_A:");
				for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
				{
					printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
				}
				printf("\n");
				if((EventFlag.MeterExcepFlag&0x01)==0)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
				
					if(*bTemp&0x80)                         //A相缺相
					{
						if((DefalutPhaseFlag&0x0200)==0) //0:上次A相断相，且已上报 
						{
							DefalutPhaseFlag^=0x0200;	
							PackAlarmRecord(AXDX);
													
						}										
					}
					else                             //A相不缺相
					{
						if((DefalutPhaseFlag&0x0200)==1) //1:上次A相缺相，已上报
						{ 
							DefalutPhaseFlag^=0x0200;
							PackAlarmRecord(AXDXHF);
							
						}
					}
					MeterMsg.MeterCmd = READ_STATUS_B;	
				}
				MeterMsg.Rcvfinishflag = 1;	
			}
			break;
			case READ_STATUS_B:
			{
				printf("Status_B：");
				for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
				{
					printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
				}
				printf("\n");
				if((EventFlag.MeterExcepFlag&0x01)==0)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
				
					if(*bTemp&0x80)                         //B相缺相
					{
						if((DefalutPhaseFlag&0x0400)==0) //0:上次B相断相，且已上报 
						{
							DefalutPhaseFlag^=0x0400;
							PackAlarmRecord(BXDX);
														
						}										
					}
					else                             //B相不缺相
					{
						if((DefalutPhaseFlag&0x0400)==1) //1:上次B相缺相，已上报
						{ 
							DefalutPhaseFlag^=0x0400;
							PackAlarmRecord(BXDXHF);
							
						}
					}
					MeterMsg.MeterCmd = READ_STATUS_C;		
				}
				MeterMsg.Rcvfinishflag = 1;	
			}
			break;
			case READ_STATUS_C:
			{
				printf("Status_C：");
				for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
				{
					printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
				}
				printf("\n");
				if((EventFlag.MeterExcepFlag&0x01)==0)
				{
					*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
				
					if(*bTemp&0x80)                         //C相缺相
					{
						if((DefalutPhaseFlag&0x0800)==0) //0:上次C相断相，且已上报 
						{
							DefalutPhaseFlag^=0x0800;
							PackAlarmRecord(CXDX);
														
						}										
					}
					else                             //C相不缺相
					{
						if((DefalutPhaseFlag&0x0800)==1) //1:上次C相缺相，已上报
						{ 
							DefalutPhaseFlag^=0x0800;
							PackAlarmRecord(CXDXHF);
							
						}
					}
					MeterMsg.MeterCmd = READ_CURENTTIME_SMH;	
				}
				MeterMsg.Rcvfinishflag = 1;		
			}
			break;
		}
		else
		{
			case READ_STATUS_97:
			{
				if(AnasysMeterCmd())
				{
					int j = 0;
					for (j = 0;j < 3;j++)
					{
						if((*(MeterMsg.Rcvbuffer + 12) - 0x33) & 1 << j)
						{
							switch (j)
							{
								case 0:
								if((DefalutPhaseFlag&0x0200)==0) //0:A  
								{
									DefalutPhaseFlag^=0x0200;
									PackAlarmRecord(AXDX);
																
								}
									break;
								case 1:
								if((DefalutPhaseFlag&0x0400)==0) //0:
								{
									DefalutPhaseFlag^=0x0400;
									PackAlarmRecord(BXDX);
																
								}
									break;
								case 2:
								if((DefalutPhaseFlag&0x0800)==0) //0:
								{
									DefalutPhaseFlag^=0x0800;
									PackAlarmRecord(CXDX);
																
								}
									break;
								default:
									break;
							}
							
						}
						else
						{
							switch (j)
							{
							case 0:
							if((DefalutPhaseFlag&0x0200)==1) //1:
							{ 
								DefalutPhaseFlag^=0x0200;
								PackAlarmRecord(AXDXHF);
								
							}
							break;
							case 1:
							if((DefalutPhaseFlag&0x0400)==0) //0:
							{
								DefalutPhaseFlag^=0x0400;
								PackAlarmRecord(BXDX);
															
							}
							break;
							case 2:
							if((DefalutPhaseFlag&0x0800)==1) //1:
							{ 
								DefalutPhaseFlag^=0x0800;
								PackAlarmRecord(CXDXHF);
								
							}
							break;
							default:
								break;
							}
							
						}
					}
					MeterMsg.MeterCmd = READCUR_A;	
					MeterMsg.Rcvfinishflag = 1;	
				}
				break;
			}	
		}
	#endif
		case READ_CURENTTIME_SMH:
		{
			
			printf("Current time hour min：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");		
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{			

				*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
				*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
				*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16) - 0x33;

				MeterMsg.MeterCurrentTime[0] = CDZ_BCDTOHEX(*bTemp);
				MeterMsg.MeterCurrentTime[1] = CDZ_BCDTOHEX(*(bTemp+1));
				MeterMsg.MeterCurrentTime[2] = CDZ_BCDTOHEX(*(bTemp+2));						
				 printf("MeterMsg.MeterCurrentTime is %d:%d\n",MeterMsg.MeterCurrentTime[2],MeterMsg.MeterCurrentTime[1]);
				MeterMsg.MeterCmd = READ_CURENTTIME_WDMY;
			}
			for(i = 0;i<14;i++)
			{
				if((MeterMsg.MeterCurrentTime[2]>=MeterMsg.RateToTime[i].RateToTime_Hour)&&
					(MeterMsg.MeterCurrentTime[2]<=MeterMsg.RateToTime[i+1].RateToTime_Hour)&&
					(MeterMsg.RateToTime[i+1].RateToTime_Hour!=0))
				{
					if(MeterMsg.MeterCurrentTime[2]==MeterMsg.RateToTime[i].RateToTime_Hour&&
						MeterMsg.MeterCurrentTime[1]>=MeterMsg.RateToTime[i].RateToTime_Min)
					{
						if(MeterMsg.RateToTime[i].RateIndex==2)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate2;		
						if(MeterMsg.RateToTime[i].RateIndex==3)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate3;	
						if(MeterMsg.RateToTime[i].RateIndex==4)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate4;										
					}
					else if(MeterMsg.MeterCurrentTime[2]==MeterMsg.RateToTime[i+1].RateToTime_Hour&&
						MeterMsg.MeterCurrentTime[1]<MeterMsg.RateToTime[i+1].RateToTime_Min)
					{
						if(MeterMsg.RateToTime[i].RateIndex==2)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate2;		
						if(MeterMsg.RateToTime[i].RateIndex==3)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate3;	
						if(MeterMsg.RateToTime[i].RateIndex==4)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate4;										
					}
					else
					{
						if(MeterMsg.RateToTime[i].RateIndex==2)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate2;		
						if(MeterMsg.RateToTime[i].RateIndex==3)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate3;	
						if(MeterMsg.RateToTime[i].RateIndex==4)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate4;		
					}

				}
				if((MeterMsg.MeterCurrentTime[2]>=MeterMsg.RateToTime[i].RateToTime_Hour)&&
					(MeterMsg.RateToTime[i+1].RateToTime_Hour==0))
				{
						if(MeterMsg.MeterCurrentTime[2]==MeterMsg.RateToTime[i].RateToTime_Hour&&
						MeterMsg.MeterCurrentTime[1]>=MeterMsg.RateToTime[i].RateToTime_Min)
					{
						if(MeterMsg.RateToTime[i].RateIndex==2)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate2;		
						if(MeterMsg.RateToTime[i].RateIndex==3)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate3;	
						if(MeterMsg.RateToTime[i].RateIndex==4)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate4;										
					}
					else if(MeterMsg.MeterCurrentTime[2]==MeterMsg.RateToTime[i+1].RateToTime_Hour&&
						MeterMsg.MeterCurrentTime[1]<MeterMsg.RateToTime[i+1].RateToTime_Min)
					{
						if(MeterMsg.RateToTime[i].RateIndex==2)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate2;		
						if(MeterMsg.RateToTime[i].RateIndex==3)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate3;	
						if(MeterMsg.RateToTime[i].RateIndex==4)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate4;										
					}
					else
					{
						if(MeterMsg.RateToTime[i].RateIndex==2)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate2;		
						if(MeterMsg.RateToTime[i].RateIndex==3)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate3;	
						if(MeterMsg.RateToTime[i].RateIndex==4)		
							ChargeMsg.f_ChargeRate = HoutaiData.fRate4;		
					}
				}
				
					
			}

			MeterMsg.Rcvfinishflag = 1;	
			
		}
		break;
		case READ_CURENTTIME_WDMY:
		{
			printf("Current time week day month year：");
			for(i = 0;i<Uart[EXBOARDCOM].rxnum;i++)
			{
				printf("%02X",Uart[EXBOARDCOM].revbuf[i]);
			}
			printf("\n");		
			if((EventFlag.MeterExcepFlag&0x01)==0)
			{			

				*bTemp = *(MeterMsg.Rcvbuffer + 14) - 0x33;
				*(bTemp + 1) = *(MeterMsg.Rcvbuffer + 15) - 0x33;
				*(bTemp + 2) = *(MeterMsg.Rcvbuffer + 16) - 0x33;
				*(bTemp + 3) = *(MeterMsg.Rcvbuffer + 17) - 0x33;

				MeterMsg.MeterCurrentTime[3] = CDZ_BCDTOHEX(*bTemp);
				MeterMsg.MeterCurrentTime[4] = CDZ_BCDTOHEX(*(bTemp+1));
				MeterMsg.MeterCurrentTime[5] = CDZ_BCDTOHEX(*(bTemp+2));		
				MeterMsg.MeterCurrentTime[6] = CDZ_BCDTOHEX(*(bTemp+3));				
				 printf("MeterMsg.MeterCurrentTime is %4d-%2d-%2d\n",MeterMsg.MeterCurrentTime[6],MeterMsg.MeterCurrentTime[5],MeterMsg.MeterCurrentTime[4]);
			//	MeterMsg.MeterCmd = READCUR_A;
				MeterMsg.MeterCmd = READTOTAL_POWER;
			}	
			MeterMsg.Rcvfinishflag = 1;				
			static char SetSysTimeFlag = 0;
			if(SetSysTimeFlag==0)
			{
				SetSysTimeFlag = 1;
				SetSysTime();
			}
		}
		break;
					
		default:break;
	}
	
	

}


