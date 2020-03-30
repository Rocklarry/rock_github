#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Card.h"
#include "mycom.h"
#include "DataType.h"
#include "public.h"
#include "convertion.h"
#include "audio_play.h"


CardTP CardMsg;     //存放卡信息

RepayCardMsgTP RepayCardMsg;

/*********************************************************************************************
		                             函数声明 
*********************************************************************************************/
void CardComSend(void);
void CardComRcv(void);


/********************************************************************************* 
 函数名称： CardInit
 功能描述：	卡初始化，创建卡接收线程和发发送线程
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void CardInit(void)
{
	int result;
	pthread_t CardSendthread,CardRcvthread;
	
	result = pthread_create(&CardSendthread, PTHREAD_CREATE_JOINABLE, (void *)CardComSend ,NULL);	
	if(result)
	{
		perror("pthread_create: CardComSend.\n");
		exit(EXIT_FAILURE);
	}
	
	result = pthread_create(&CardRcvthread, PTHREAD_CREATE_JOINABLE, (void *)CardComRcv ,NULL);
	if(result)
	{
		perror("pthread_create: CardComRcv.\n");
		exit(EXIT_FAILURE);
	}
}


/********************************************************************************* 
 函数名称： GetFindCardCommand
 功能描述：	发送寻卡命令
 输    入：	byFindCardCommand：寻卡命令数据buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void  GetFindCardCommand(ST_UCHAR *byFindCardCommand)
{
	
	ST_UCHAR str[20],bcc;
	ST_INT32 iCommandLen = 0;
	byFindCardCommand[iCommandLen++] = 0x02;
	byFindCardCommand[iCommandLen++] = 0x00;
	byFindCardCommand[iCommandLen++] = 0x02;
	byFindCardCommand[iCommandLen++] = 0x51;
	byFindCardCommand[iCommandLen++] = 0x37;
	byFindCardCommand[iCommandLen++] = 0x03;
	bcc =GetBCC(byFindCardCommand,iCommandLen);
	byFindCardCommand[iCommandLen++] = bcc;
	write(Cardfd, byFindCardCommand, 7);
	sprintf((ST_CHAR *)str,"%X%X%X%X%X%X%X",byFindCardCommand[0],byFindCardCommand[1],byFindCardCommand[2],byFindCardCommand[3],
	byFindCardCommand[4],byFindCardCommand[5],byFindCardCommand[6]);
	printf("sendfindcard:%s\n",str);

}

/********************************************************************************* 
 函数名称： GetPretreatmentCommand
 功能描述：	发送预处理命令
 输    入：	GetPretreatmentCommand：预处理命令buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void  GetPretreatmentCommand(ST_UCHAR *byPretreatmentCommand)
{
	ST_UCHAR bcc;
	ST_INT32 iCommandLen = 0;
	struct tm *stShowNowTime;
	time_t now;

	time(&now);
	stShowNowTime = localtime(&now);

	byPretreatmentCommand[iCommandLen++] = 0x02;
	byPretreatmentCommand[iCommandLen++] = 0x00;
	byPretreatmentCommand[iCommandLen++] = 0x09;
	byPretreatmentCommand[iCommandLen++] = 0x51;
	byPretreatmentCommand[iCommandLen++] = PRETREATMENT;
	byPretreatmentCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_year/100);
	byPretreatmentCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_year%100);
	byPretreatmentCommand[iCommandLen++] =0;//(ST_UCHAR)(stShowNowTime->tm_mon+1);
	byPretreatmentCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_mday);
	byPretreatmentCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_hour);
	byPretreatmentCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_min);
	byPretreatmentCommand[iCommandLen++] = 0;//(ST_UCHAR)(stShowNowTime->tm_sec);
	byPretreatmentCommand[iCommandLen++] = ETX;
	bcc = GetBCC(byPretreatmentCommand,iCommandLen);
	byPretreatmentCommand[iCommandLen++] = bcc;
	write(Cardfd, byPretreatmentCommand, iCommandLen);
	int i;
	printf("send yuchilicmd\n");
        for(i=0;i<iCommandLen;i++)
	{
	printf("%2X",byPretreatmentCommand[i]);
	}
	printf("\n");
}


/********************************************************************************* 
 函数名称： GetStartChargeCommand
 功能描述：	发送开始充电命令
 输    入：	byStartChargeCommand： 开始充电命令buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void  GetStartChargeCommand(ST_UCHAR *byStartChargeCommand)
{
	ST_INT32 iCommandLen = 0;
	struct tm *stShowNowTime;
	time_t now;
	ST_UCHAR bcc,i;

	now = time(0);
	stShowNowTime = localtime(&now);
	
	byStartChargeCommand[iCommandLen++] = 0x02;
	byStartChargeCommand[iCommandLen++] = 0x00;
	byStartChargeCommand[iCommandLen++] = 0x09;
	byStartChargeCommand[iCommandLen++] = 0x51;//CTRLCARDESAM;
	byStartChargeCommand[iCommandLen++] = 0x30;//STARTCHARGE;
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_year/100);
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_year%100);
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_mon+1);
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_mday);
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_hour);
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_min);
	byStartChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_sec);
	byStartChargeCommand[iCommandLen++] = 0x03;
	bcc = GetBCC(byStartChargeCommand,iCommandLen);
	byStartChargeCommand[iCommandLen++] = bcc;
	write(Cardfd, byStartChargeCommand, iCommandLen);
	for(i=0;i<iCommandLen;i++)
{
printf("%2X\n",byStartChargeCommand[i]);
}
//printf("\n");
//	write(Cardfd, byStartChargeCommand, iCommandLen);
}


/********************************************************************************* 
 函数名称： GetEndChargeCommand
 功能描述：	发送结束充电命令
 输    入：	byEndChargeCommand： 结束充电命令buffer，byShouldPay：要扣金额，byChargeEle：充电量
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void  GetEndChargeCommand(ST_UCHAR *byEndChargeCommand,ST_UCHAR *byShouldPay,ST_UCHAR *byChargeEle)
{
	ST_INT32 iCommandLen = 0;
	struct tm *stShowNowTime;
	time_t now;
	ST_UCHAR bcc,i;
	now = time(0);
	stShowNowTime = localtime(&now);
//	printf("paymoney:%2X%2X%2X%2x\n",*(byShouldPay+3),*(byShouldPay + 2),(byShouldPay + 1),byShouldPay);
	byEndChargeCommand[iCommandLen++] = STX;
	byEndChargeCommand[iCommandLen++] = 0x00;
	byEndChargeCommand[iCommandLen++] = 0x11;
	byEndChargeCommand[iCommandLen++] = CTRLCARDESAM;
	byEndChargeCommand[iCommandLen++] = ENDCHARGE;
	byEndChargeCommand[iCommandLen++] = *(byShouldPay+3);
	byEndChargeCommand[iCommandLen++] = *(byShouldPay + 2);
	byEndChargeCommand[iCommandLen++] = *(byShouldPay + 1);
	byEndChargeCommand[iCommandLen++] = *byShouldPay;
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_year/100);
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_year%100);
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_mon+1);
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_mday);
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_hour);
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_min);
	byEndChargeCommand[iCommandLen++] =0;// (ST_UCHAR)(stShowNowTime->tm_sec);
	byEndChargeCommand[iCommandLen++] = *(byChargeEle+3);
	byEndChargeCommand[iCommandLen++] = *(byChargeEle+2);
	byEndChargeCommand[iCommandLen++] = *(byChargeEle+1);
	byEndChargeCommand[iCommandLen++] = *byChargeEle;
	
	byEndChargeCommand[iCommandLen++] = ETX;
	bcc = GetBCC(byEndChargeCommand,iCommandLen);
	printf("bcc:%d",bcc);
	byEndChargeCommand[iCommandLen++] = bcc;
	for(i=0;i<iCommandLen;i++)
	{
		printf("%2X",byEndChargeCommand[i]);
	}
	printf("\n");
	write(Cardfd, byEndChargeCommand, iCommandLen);
}

/********************************************************************************* 
 函数名称： GetRepayCommand
 功能描述：	获取随机数命令
 输    入：	byRandomNumberCommand： 获取随机数的buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void GetRepayCommand(ST_UCHAR *byRepayCommand)
{
	ST_INT32 iCommandLen = 0;
	ST_UCHAR bcc;
	byRepayCommand[iCommandLen++] = STX;
	byRepayCommand[iCommandLen++] = 0x00;
	byRepayCommand[iCommandLen++] = 0x02;
	byRepayCommand[iCommandLen++] = 0x51;
	byRepayCommand[iCommandLen++] = 0x35;
	byRepayCommand[iCommandLen++] = ETX;
	bcc = GetBCC(byRepayCommand,iCommandLen);
	byRepayCommand[iCommandLen++] = bcc;
	write(Cardfd, byRepayCommand, iCommandLen);
}



/********************************************************************************* 
 函数名称： GetRandomNumberCommand
 功能描述：	获取随机数命令
 输    入：	byRandomNumberCommand： 获取随机数的buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void GetRandomNumberCommand(ST_UCHAR *byRandomNumberCommand)
{
	ST_UCHAR iCommandLen = 0;
	byRandomNumberCommand[iCommandLen] = STX;
	iCommandLen++;
	byRandomNumberCommand[iCommandLen] = 0x00;
	iCommandLen++;
	byRandomNumberCommand[iCommandLen] = 0x02;
	iCommandLen++;
	byRandomNumberCommand[iCommandLen] = CTRLCARDESAM;
	iCommandLen++;
	byRandomNumberCommand[iCommandLen] = RANDOMNUMBER;
	iCommandLen++;
	byRandomNumberCommand[iCommandLen] = ETX;
	iCommandLen++;
	byRandomNumberCommand[iCommandLen] = GetBCC(byRandomNumberCommand,iCommandLen - 1);
	iCommandLen++;
	write(Cardfd, byRandomNumberCommand, iCommandLen);
}


/********************************************************************************* 
 函数名称： GetCalculateMAC2Command
 功能描述：	计算mac命令
 输    入：	byRandomNumberCommand： 获取随机数的buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void  GetCalculateMAC2Command(ST_UCHAR *byCalculateMAC2Command,ST_UCHAR *byData,
										ST_INT32 iDataLen,ST_UCHAR *byAddr)
{
	ST_INT32 iCommandLen = 0;
	byCalculateMAC2Command[iCommandLen++] = STX;
	byCalculateMAC2Command[iCommandLen++] = 0x00;
	byCalculateMAC2Command[iCommandLen++] = 0x04;
	byCalculateMAC2Command[iCommandLen++] = CTRLCARDESAM;
	byCalculateMAC2Command[iCommandLen++] = CALCULATEMAC2;

	memcpy((byCalculateMAC2Command + iCommandLen),byData,iDataLen);
	iCommandLen += iDataLen;
	memcpy((byCalculateMAC2Command + iCommandLen),byAddr,4);
	iCommandLen += iDataLen;

	byCalculateMAC2Command[iCommandLen++] = ETX;
	byCalculateMAC2Command[iCommandLen++] = GetBCC(byCalculateMAC2Command,iCommandLen - 1);
	write(Cardfd, byCalculateMAC2Command, iCommandLen);
}

/********************************************************************************* 
 函数名称： GetOutCardCommand
 功能描述：	弹卡命令
 输    入：	byOutCardCommand： 弹卡命令的buffer
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void  GetOutCardCommand(ST_UCHAR *byOutCardCommand)
{
	ST_UCHAR bcc,i;
	ST_INT32 iCommandLen = 0;
	byOutCardCommand[iCommandLen++] = STX;
	byOutCardCommand[iCommandLen++] = 0x00;
	byOutCardCommand[iCommandLen++] = 0x02;
	byOutCardCommand[iCommandLen++] = CTRLOUTCARD;
	byOutCardCommand[iCommandLen++] = OUTCARD;
	byOutCardCommand[iCommandLen++] = ETX;
	bcc = GetBCC(byOutCardCommand,iCommandLen);
	byOutCardCommand[iCommandLen++] = bcc;
for(i=0;i<iCommandLen;i++)
{
	printf("%2X",byOutCardCommand[i]);
}
printf("\n");
	write(Cardfd, byOutCardCommand, iCommandLen);
}

void  GetRedimESAMKeyCommand(ST_UCHAR *byRedimESAMKeyCommand,ST_UCHAR byContrl)
{
	ST_INT32 iCommandLen = 0;
	ST_UCHAR bcc;
	byRedimESAMKeyCommand[iCommandLen++] = STX;
	byRedimESAMKeyCommand[iCommandLen++] = 0x00;
	byRedimESAMKeyCommand[iCommandLen++] = 0x03;
	byRedimESAMKeyCommand[iCommandLen++] = 0x51;
	byRedimESAMKeyCommand[iCommandLen++] = 0x51;
	byRedimESAMKeyCommand[iCommandLen++] = byContrl;
	byRedimESAMKeyCommand[iCommandLen++] = ETX;
	bcc = GetBCC(byRedimESAMKeyCommand,iCommandLen);
	byRedimESAMKeyCommand[iCommandLen++] = bcc;
	write(Cardfd, byRedimESAMKeyCommand, iCommandLen);
int i;
printf("send xiugaikey\n");
for(i=0;i<iCommandLen;i++)
{
        printf("%2X",byRedimESAMKeyCommand[i]);
}
printf("\n");

}

void  GetRedimESAMParaCommand(ST_UCHAR *byRedimESAMParaCommand,ST_UCHAR byContrl)
{
	ST_INT32 iCommandLen = 0;
	byRedimESAMParaCommand[iCommandLen++] = STX;
	byRedimESAMParaCommand[iCommandLen++] = 0x00;
	byRedimESAMParaCommand[iCommandLen++] = 0x03;
	byRedimESAMParaCommand[iCommandLen++] = CTRLCARDESAM;
	byRedimESAMParaCommand[iCommandLen++] = REDIMESAMPARA;
	byRedimESAMParaCommand[iCommandLen++] = byContrl;
	byRedimESAMParaCommand[iCommandLen++] = ETX;
	byRedimESAMParaCommand[iCommandLen++] = GetBCC(byRedimESAMParaCommand,iCommandLen - 1);
	write(Cardfd, byRedimESAMParaCommand, iCommandLen);
}

/********************************************************************************* 
 函数名称： FindCardResProcess
 功能描述：	解析寻卡命令
 输    入：	byFrameData： 接收buffer中的数据部分
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32  FindCardResProcess(ST_UCHAR *byFrameData)
{
	if (*(byFrameData) == READCARDSUCCESS)
	{
		if (*(byFrameData + 1) == S1CARDREADY)
		{
			
			if (*(byFrameData + 2) == S2SYSCARD)
			{
				CardMsg.IsSystemCard = SYSTEM_CARD;
				CardMsg.Sendcmd = PRETREATMENT_CARD;
				printf("FindCardSucess\n");
				return FINDCARDSUCCESS;
			} 
			else if(*(byFrameData + 2) == S2NOSYSCARD)
			{
				CardMsg.IsSystemCard = NOSYSTEM_CARD;
				return FINDNOSYSCARD;
			}
			else
			{
				return OPERATERFIAL;
			}

			
		} 
		else if(*(byFrameData + 1) == S1NOCARD)
		{
			return FINDNOCARD;
		}
		else
		{
			return OPERATERFIAL;
		}
		
	} 
	else
	{
		return OPERATERFIAL;
	}
}


/********************************************************************************* 
 函数名称： PretreatmentResProcess
 功能描述：	解析预处理命令
 输    入：	byFrameData： 接收buffer中的数据部分
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32  PretreatmentResProcess(ST_UCHAR *byFrameData)
{
	if (*(byFrameData) == READCARDSUCCESS)			    
	{
		printf("card status:%x\n",*(byFrameData + 2));
		CardMsg.Sendcmd = 0;
		memcpy(CardMsg.CardSeria, byFrameData + 3, 8);							
		CardMsg.CardType = *(byFrameData + 1);
		switch (*(byFrameData + 1))		
		{
			case CUSTOMCARD:					
			{
				memcpy(CardMsg.CardMoney,(byFrameData + 11),4);						
				Hex4ToFloat((ST_CHAR *)CardMsg.CardMoney,&CardMsg.f_CardMoney,1);
				printf("**************************CardMsg.f_CardMoney is %d*************************\n",CardMsg.f_CardMoney);
				CardMsg.iDealCounts = ((ST_INT32)(*(byFrameData + 15)) << 8) + 
					(ST_INT32)(*(byFrameData + 16));	
				if (*(byFrameData + 2) == CANCHARGE)	
				{
					EventFlag.IsExistCardFlag = 1;
					CardMsg.CardStatus = CARDUNLOCK;
					
					printf("YuchuliSucess cardunlock\n");
					return CUSCANCHARGE;
				} 
				else if(*(byFrameData + 2) == CANENDCHARGE)	
				{
					EventFlag.IsExistCardFlag = 1;
					CardMsg.CardStatus = CARDLOCK;
					printf("YuchuliSucess cardlock\n");
					return CUSCANENDCHARGE;
				}
				else if(*(byFrameData + 2) == CANREPAY)			
				{
					CardMsg.CardStatus = CARDREPAY;
					printf("Yuchulifailed \n");
					CardMsg.Sendcmd = REPAY_CARD;
					return CUSCANREPAY;
				}
				else
				{
					printf("Yuchulifailed\n");
					CardMsg.Sendcmd = FIND_CARD;
					return PRETREATFIAL;
				}				
			}
			case KEYCARD:	
				printf("FINDNEWKEY\n");					
				return FINDNEWKEY;

			case TESTCARD:		
				printf("FINDTESTCARD\n");						
				return FINDTESTCARD;

			case SETCARD:
				printf("FINDTESTCARD\n");					
				return FINDSETCARD;

			case ADMINCARD:	
				printf("FINDTESTCARD\n");					
				return FINDADMINKCARD;

			case RECHARGEABLECARD:	
				printf("FINDTESTCARD\n");											
				return FINDRECHARGECARD;

			default:
				return PRETREATFIAL;

		}
	} 
	else if(*(byFrameData) == CTRLCARDFIAL)			
	{
		CardMsg.Sendcmd = FIND_CARD;
		return PRETREATFIAL;
	}
	else if(*(byFrameData) == SBCHARGING)	
	{
		CardMsg.Sendcmd = FIND_CARD;
		return VERYBUSY;
	}
	else if(*(byFrameData) == NOCARD)			
	{
		CardMsg.Sendcmd = FIND_CARD;
		return FINDNOCARD;
	}
	else if(*(byFrameData) == PSAMRESETFIAL)
	{
		CardMsg.Sendcmd = FIND_CARD;
		return PSAMRESETFAIL;
	}
	else if(*(byFrameData) == CARDRESETFIAL)
	{
		CardMsg.Sendcmd = FIND_CARD;
		return CARDRESETFAIL;
	}	
	return OPERATERFIAL;
}

static ST_CHAR RcvRepaycmdFlag;

ST_INT32  RepayResProcess(ST_UCHAR *byFrameData)
{
	RcvRepaycmdFlag = 1;
	if(*(byFrameData)==REPAYSUCCESS)
	{
		CardMsg.Sendcmd = FIND_CARD;
		return PRETREATSUCCESS;
	}
	else if(*(byFrameData)==REPAYFIAL)
	{
		//CardMsg.Sendcmd = FIND_CARD;
		return PRETREATFIAL;
	}
	else
	{
		CardMsg.Sendcmd = FIND_CARD;
		return FINDNOCARD;
	}	
	
	return FINDNOCARD;
}


/********************************************************************************* 
 函数名称： StartChargeResProcess
 功能描述：	解析开始充电命令
 输    入：	byFrameData： 接收buffer中的数据部分
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32  StartChargeResProcess(ST_UCHAR *byFrameData)
{
	switch(*byFrameData)
	{
		case READCARDSUCCESS:									
		{printf("rcvstartchargesucess\n");
			memcpy(CardMsg.CardMoney,(byFrameData + 11),4);		
			CardMsg.Responsedata = STARTCHARGESUCCESS;			
			return STARTCHARGESUCCESS; 			
		}
		case CTRLCARDFIAL:							
			return STARTCHARGEFIAL;

		case NOCARD:									
			return FINDNOCARD;

		default:
			return OPERATERFIAL;

	}	
}

/********************************************************************************* 
 函数名称： EndChargeResProcess
 功能描述：	解析开始充电命令
 输    入：	byFrameData： 接收buffer中的数据部分
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32  EndChargeResProcess(ST_UCHAR *byFrameData)
{
	switch(*byFrameData)
	{
		case READCARDSUCCESS:															
		{
			printf("endchargesucess\n");
			memcpy(CardMsg.CardMoney,(byFrameData + 11),4);						
			return ENDCHARGESUCCESS; 			
		}
		case CTRLCARDFIAL:	
			printf("endchargefailed\n");								
			return ENDCHARGEFIAL;

		case NOCARD:								
			return FINDNOCARD;
		case INVALID:
			return UNLOCKFAILED;

		default:
			return OPERATERFIAL;

	}
}
ST_INT32  RandomNumberResProcess(ST_UCHAR *byFrameData)	
{
	return 0;
}
ST_INT32  CalculateMac2ResProcess(ST_UCHAR *byFrameData)
{
	return 0;
}

/********************************************************************************* 
 函数名称： OutCardResProcess
 功能描述：	解析弹卡充电命令
 输    入：	byFrameData： 接收buffer中的数据部分
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32  OutCardResProcess(ST_UCHAR *byFrameData)
{
	switch(*byFrameData)
	{
		case PSTATEOUTCARDSUCCESS:			
			return OUTCARDSUCCESS;

		case PSTATEOUTCARDFIAL:				
			return OUTCARDFIAL;

		default:		
			return OPERATERFIAL;

	}
}
ST_INT32  RedimESAMKeyResProcess(ST_UCHAR *byFrameData)
{
	switch(*byFrameData)
	{
		case READCARDSUCCESS:			
			return UPDATAKEYSUCCESS;

		case CTRLCARDFIAL:						
			return UPDATAKEYFIAL;

		case INVALID:						
			return OPERATERIVALD;

		case NOCARD:	
			return FINDNOCARD;

		default:						
			return OPERATERFIAL;

	}
}
ST_INT32  RedimESAMParaResProcess(ST_UCHAR *byFrameData)
{
	switch(*byFrameData)
	{
		case READCARDSUCCESS:				
			memcpy(CardMsg.byPiceChangeTime,(byFrameData + 7),5);								
			memcpy(CardMsg.byPresentDealPice1,(byFrameData + 12),3);					
			memcpy(CardMsg.byPresentDealPice2,(byFrameData + 15),3);					
			memcpy(CardMsg.byPresentDealPice3,(byFrameData + 18),3);
			memcpy(CardMsg.byPresentDealPice4,(byFrameData + 21),3);					
			memcpy(CardMsg.byPresentDealPice5,(byFrameData + 24),3);				
			memcpy(CardMsg.byPresentDealPice6,(byFrameData + 27),3);
			memcpy(CardMsg.byPresentDealPice7,(byFrameData + 30),3);	
			memcpy(CardMsg.byPresentDealPice8,(byFrameData + 33),3);
			
			memcpy(CardMsg.byReserveDealPice1,(byFrameData + 36),3);
			memcpy(CardMsg.byReserveDealPice2,(byFrameData + 39),3);
			memcpy(CardMsg.byReserveDealPice3,(byFrameData + 42),3);
			memcpy(CardMsg.byReserveDealPice4,(byFrameData + 45),3);	
			memcpy(CardMsg.byReserveDealPice5,(byFrameData + 48),3);	
			memcpy(CardMsg.byReserveDealPice6,(byFrameData + 51),3);
			memcpy(CardMsg.byReserveDealPice7,(byFrameData + 54),3);
			memcpy(CardMsg.byReserveDealPice8,(byFrameData + 57),3);
			return INITCARDSUCCESS;

		case CTRLCARDFIAL:						
			return INITCARDFIAL;

		case INVALID:											
			return OPERATERIVALD;

		case NOCARD:						
			return FINDNOCARD;

		default:							
			return OPERATERFIAL;

	}
}

/********************************************************************************* 
 函数名称： OutCardResProcess
 功能描述：	解析接收数据帧
 输    入：	byResCardData： 接收buffer，iDataLen：接收到的长度
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_INT32  AnalysisResCardData(ST_UCHAR *byResCardData,ST_INT32 iDataLen)
{
	//ST_UCHAR bcc;
//	memcpy(Uart[CARDCOM].revbuf, byResCardData, iDataLen);
	//Uart[CARDCOM].rxnum = iDataLen;
	if (*Uart[CARDCOM].revbuf == STX &&
		(*(Uart[CARDCOM].revbuf + 1) << 8) + *(Uart[CARDCOM].revbuf + 2) + 5 == Uart[CARDCOM].rxnum &&	
		*(Uart[CARDCOM].revbuf + (Uart[CARDCOM].rxnum - 2)) == ETX&&						
		*(Uart[CARDCOM].revbuf + (Uart[CARDCOM].rxnum - 1)) ==GetBCC(Uart[CARDCOM].revbuf,Uart[CARDCOM].rxnum-1))
	{
    		printf("kajiexihanshu\n");
		CardMsg.SendCout = 0;
		if(EventFlag.CardExcepFlag==1) 
		{
			printf("EventFlag.CardExcepFlag is 0\n");
			EventFlag.CardExcepFlag = 0;
		}
		if (*(Uart[CARDCOM].revbuf + 3) == CTRLCARDESAM)																
		{
			switch(*(Uart[CARDCOM].revbuf + 4))															
			{
				case FINDCARD:
					return FindCardResProcess(Uart[CARDCOM].revbuf + 5);													


				case PRETREATMENT:
					return PretreatmentResProcess(Uart[CARDCOM].revbuf + 5);												


				case REPAY:
					return RepayResProcess(Uart[CARDCOM].revbuf + 5);														


				case ENDCHARGE:
					return EndChargeResProcess(Uart[CARDCOM].revbuf + 5);										

				case CALCULATEMAC2:
					return CalculateMac2ResProcess(Uart[CARDCOM].revbuf + 5);										

				case RANDOMNUMBER:
					return RandomNumberResProcess(Uart[CARDCOM].revbuf + 5);						


				case STARTCHARGE:
					return StartChargeResProcess(Uart[CARDCOM].revbuf + 5);	


				case REDIMESAMKEY:
					return RedimESAMKeyResProcess(Uart[CARDCOM].revbuf + 5);				


				case REDIMESAMPARA:
					return RedimESAMParaResProcess(Uart[CARDCOM].revbuf + 5);	


				default:
					return OPERATERFIAL;

			}
		}
		else if (*(Uart[CARDCOM].revbuf + 3) == CTRLOUTCARD	&&														
				 *(Uart[CARDCOM].revbuf + 4) == OUTCARD)	
		{	
			return OutCardResProcess(Uart[CARDCOM].revbuf + 5);
		}
		else
		{
			return OPERATERFIAL;
		}
	}
	else
	{
		return OPERATERFIAL;
	}
}

  /********************************************************************************* 
 函数名称： CardComSend
 功能描述：	卡发送寻卡和预处理命令函数
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/  

void CardComSend(void)
{
	CardMsg.Sendcmd = FIND_CARD;
	CardMsg.Responsedata = 0;
//	static ST_CHAR SendPrementflag = 0;
	while(1)
	{		
		switch(CardMsg.Sendcmd)
		{
			case FIND_CARD:
			{
                if(CardMsg.Responsedata==FINDNOSYSCARD)
    	        {
            	//        PlaySound(USENANWANGCARD,NULL);
                   // 	sleep(4);
                        Tanka();
            	}

				else if(CardMsg.SendCout<10)
				{
					GetFindCardCommand(Uart[CARDCOM].sendbuf);
					CardMsg.SendCout++;
					printf("sendcout is %d\n",CardMsg.SendCout);				
				}
				else
				{
					CardMsg.SendCout = 0;
					EventFlag.CardExcepFlag = 1;
					
					ChargeMsg.PaleStatus = PALEEXCEPTION;
					printf("EventFlag.CardExcepFlag is 1\n");
				}
				
			}							
			break;
			case PRETREATMENT_CARD:
			{
				if(CardMsg.SendCout<10)
				{

					GetPretreatmentCommand(Uart[CARDCOM].sendbuf);	
					printf("send yuchili\n");			
					CardMsg.SendCout++;	
					CardMsg.Sendcmd = 0;
					//usleep(1000*200);

				}
				else
				{
					CardMsg.SendCout = 0;
					EventFlag.CardExcepFlag = 1;
					ChargeMsg.PaleStatus = PALEEXCEPTION;
				}								
				
			}
			break;
			case REPAY_CARD:
			{
				GetRepayCommand(Uart[CARDCOM].sendbuf);
				CardMsg.Sendcmd = 0;
			}
			break;
			default:break;
		}					

		usleep(1000*100);
	}
	
}

  /********************************************************************************* 
 函数名称： TxToCardStartChargeCmd
 功能描述：	发送开始充电命令
 输    入： 无
 输	   出： RES_TRUE：发送成功，RES_FAULSE：发送失败
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/ 
ST_UCHAR TxToCardStartChargeCmd(void)
{

	GetStartChargeCommand(Uart[CARDCOM].sendbuf);	
	usleep(500*1000);		
	if(CardMsg.Responsedata==STARTCHARGESUCCESS)
	{
		CardMsg.SendCout = 0;
		printf("startcharge card\n");
		return RES_TRUE;
	}
	return RES_FAULSE;
}

/********************************************************************************* 
 函数名称： TxToCardEndChargeCmd
 功能描述：	发送结束充电命令
 输    入： 无
 输	   出： RES_TRUE：发送成功，RES_FAULSE：发送失败
 返 回 值： RES_TRUE:结束加电命令成功；RES_FAULSE：结束加电命令失败，LOCKCARD：卡已锁
 作    者：	yansudan
 日    期：	2011.8.1
 修改记录：
*********************************************************************************/ 
ST_INT32 TxToCardEndChargeCmd(void)
{
	GetEndChargeCommand(Uart[CARDCOM].sendbuf,CardMsg.ChargeMoney,CardMsg.ChargeEle);
	usleep(500*1000);	
	if(CardMsg.Responsedata == ENDCHARGESUCCESS)
	{
		CardMsg.SendCout = 0;
		return RES_TRUE;
	}		
	else if(CardMsg.Responsedata==UNLOCKFAILED)
	{
		GetFindCardCommand(Uart[CARDCOM].sendbuf);
		usleep(200*1000);
		if(CardMsg.Responsedata==FINDCARDSUCCESS)
		{
			GetPretreatmentCommand(Uart[CARDCOM].sendbuf);	
		}
		usleep(100*1000);
		if(CardMsg.Responsedata==CUSCANCHARGE)
		return RES_TRUE;
		
		else if(CardMsg.Responsedata==CUSCANENDCHARGE)
		{
			GetEndChargeCommand(Uart[CARDCOM].sendbuf,CardMsg.ChargeMoney,CardMsg.ChargeEle);
			usleep(300*1000);
			if(CardMsg.Responsedata==ENDCHARGESUCCESS)
			{
				return RES_TRUE;
			}			
		}
		else if(CardMsg.Responsedata==CUSCANREPAY)
		{
			GetRepayCommand(Uart[CARDCOM].sendbuf);
			usleep(300*1000);
			if(CardMsg.Responsedata==REPAYSUCCESS)
			return RES_TRUE;
			if(CardMsg.Responsedata==REPAYFIAL)
			{
				memcpy(RepayCardMsg.CardSeria,&Uart[CARDCOM].revbuf[6],8);
				memcpy(RepayCardMsg.RandomNumber,&Uart[CARDCOM].revbuf[14],4);
				memcpy(RepayCardMsg.OnLineDealSeq,&Uart[CARDCOM].revbuf[18],2);
				memcpy(RepayCardMsg.Left,&Uart[CARDCOM].revbuf[20],4);
				memcpy(RepayCardMsg.ChargeMoney,&Uart[CARDCOM].revbuf[24],4);
				RepayCardMsg.DealFlag = Uart[CARDCOM].revbuf[28];
				memcpy(RepayCardMsg.PaleNumber,&Uart[CARDCOM].revbuf[29],6);
				memcpy(RepayCardMsg.EndChargeTime,&Uart[CARDCOM].revbuf[35],7);
				memcpy(RepayCardMsg.TuojiDealSeq,&Uart[CARDCOM].revbuf[42],2);
			}
			return LOCKCARD;
			
		}
	}
	
	return RES_FAULSE;
		
}

 /********************************************************************************* 
 函数名称： OutCard
 功能描述：	发送弹卡命令
 输    入： 无
 输	   出： RES_TRUE：发送成功，RES_FAULSE：发送失败
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/ 
ST_UCHAR OutCard(void)
{
	while(CardMsg.SendCout<3)
	{
		GetOutCardCommand(Uart[CARDCOM].sendbuf);
		
		usleep(1000*1000);	
		if(CardMsg.Responsedata==OUTCARDSUCCESS)
		{
			CardMsg.SendCout = 0;
			return RES_TRUE;
		}

		else
		{
			CardMsg.SendCout++;
		}
	}
	if(CardMsg.SendCout>=3)
	{
		CardMsg.SendCout = 0;
		EventFlag.CardExcepFlag = 1;
		ChargeMsg.PaleStatus = PALEEXCEPTION;
		return RES_FAULSE;
	}	
	return RES_FAULSE;
}

 /********************************************************************************* 
 函数名称： TxToCardModifyKeyCmd
 功能描述：	发送修改密钥命令
 输    入： 无
 输	   出： RES_TRUE：发送成功，RES_FAULSE：发送失败
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/ 

ST_UCHAR TxToCardModifyKeyCmd(void)
{
	GetRedimESAMKeyCommand(Uart[CARDCOM].sendbuf,0x01);	
	usleep(200*1000);	
	if(CardMsg.Responsedata==UPDATAKEYSUCCESS)
	{
		CardMsg.SendCout = 0;
		return RES_TRUE;
	}

	return RES_FAULSE;

}



 /********************************************************************************* 
 函数名称： CardComRcv
 功能描述：	卡接收线程回调函数
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/ 
void CardComRcv(void)
{
	ST_UCHAR str[20],i=0;
	while(1)
	{			
		Uart[CARDCOM].rxnum = (ST_INT32)ReadCom(Cardfd, Uart[CARDCOM].revbuf, sizeof(Uart[CARDCOM].revbuf),10*1000);

		if(Uart[CARDCOM].rxnum > 0)
		{
			CardMsg.Responsedata = AnalysisResCardData(Uart[CARDCOM].revbuf,Uart[CARDCOM].rxnum);
			

			printf("CardMsg.Responsedata is %2X rxnum is %d\n", CardMsg.Responsedata,Uart[CARDCOM].rxnum);
			
		//	printf("*********************CardMsg.Sendcmd is %d*************************\n",CardMsg.Sendcmd);
			
			if( RcvRepaycmdFlag == 1 && CardMsg.Responsedata == PRETREATFIAL )
			{
				RcvRepaycmdFlag = 0;
				EventFlag.IsExistCardFlag = 1;
				DisplayInfo.FindLastChargeRec = 2;     //提示卡已锁				
			}			
			memcpy(str,Uart[CARDCOM].revbuf,Uart[CARDCOM].rxnum);
			for(i=0;i<Uart[CARDCOM].rxnum;i++)
			{
				printf("%2X",str[i]);
			}
			printf("\n");
			
			Uart[CARDCOM].rxnum = 0;
			memset(Uart[CARDCOM].revbuf,0,sizeof(Uart[CARDCOM].revbuf));
		}

		usleep(10*1000);
	}
}
