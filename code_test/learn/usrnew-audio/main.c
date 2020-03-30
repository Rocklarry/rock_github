/*********************************************************************************
  
文 件 名：	main.c
版    本：	1.0
概    述：	主控制流程
作    者：	yansudan
日    期：	2011.01.15
修改记录：

*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h> 
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/ioctl.h>
#include "public.h"
#include "Meter.h"
#include "Card.h"
#include "mycom.h"
#include "DataType.h"
#include "convertion.h"
#include "sql.h"
#include "timer.h"
#include "cdzChargeRecord.h"
#include "cdzProtocol.h"
#include "audio_play.h"
#include "AlarmRecord.h"
#include "KuoZhanBoard.h"
#include "display.h"
#include "cdzDataType.h"

ST_INT16 DefalutPhaseFlag = 0; //告警标志，bit0-bit11分别为：bit0:A相欠压，bit1:B相欠压，bit2:C相欠压，bit3:A相过压，bit4:B相过压，bit5:C相过压，
									  //bit6:A相过流，bit7:B相过流，bit8:C相过流，bit9:A相断相，bit10:B相断相，bit11:C相断相
ST_BOOL s_ProtolLandFlag = false;           /* 规约登陆标志 */ 

ST_BOOL ProtocolExitFlag_Gprs = false;          /* 规约线程退出标志 */   

union PriAlarmTP PriAlarm;

ChargeMsgTP ChargeMsg;       //保存充电信息
EventFlagTP EventFlag;        //充电过程中的一些标记量

DisplayMsgTP DisplayMsg;      //界面显示内容
ChargeRecordTP ChargeRecord;  //充电记录
 
HouTaiDataTP HoutaiData;      //读到的后台参数

DisplayInfoTP DisplayInfo; //显示信息
SetChargeParaTP SetChargePara;//设置充电信息参数OUT1_QiangLed
extern ST_INT32 CurrentKeepLiveCount;


static char Opendoorflag = 0;      //打开门的标记，门打开置1

/********************************************************************************* 
 函数名称： GetHoutaiMsg
 功能描述：	读取数据库中测量点的参数设置
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:读取成功，SD_FAILURE;读取失败
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_INT32 GetHoutaiMsg(void)
{
	ST_INT16 ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8034,&HoutaiData.OverCur_VolTime);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8035,&HoutaiData.OverCur_VolResuTime);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8035,&HoutaiData.OverCur_VolResuTime);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8065,&HoutaiData.PhaseFailTime);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8066,&HoutaiData.PhaseFail_ResuTime);
	if (ContentLen <= 0 ) return SD_FAILURE;	
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8821,HoutaiData.AreaCode);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8822,HoutaiData.StopCarFeePrice);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8823,&HoutaiData.IsDeductStopCarFee);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x882C,HoutaiData.ZhongduanNum);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8830,HoutaiData.ManageCardnum1);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8831,HoutaiData.ManageCardnum2);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8832,HoutaiData.ManageCardnum3);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8833,HoutaiData.ManageCardnum4);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8834,HoutaiData.ManageCardnum5);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8835,HoutaiData.ManageCardnum6);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8836,HoutaiData.ManageCardnum7);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8837,HoutaiData.ManageCardnum8);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8838,HoutaiData.ManageCardnum9);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8839,HoutaiData.ManageCardnum10);
	if (ContentLen <= 0 ) return SD_FAILURE;
		
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8880,HoutaiData.PrepairCardnum1);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8881,HoutaiData.PrepairCardnum2);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8882,HoutaiData.PrepairCardnum3);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8883,HoutaiData.PrepairCardnum4);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8884,HoutaiData.PrepairCardnum5);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8885,HoutaiData.PrepairCardnum6);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8886,HoutaiData.PrepairCardnum7);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8887,HoutaiData.PrepairCardnum8);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8888,HoutaiData.PrepairCardnum9);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8889,HoutaiData.PrepairCardnum10);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8841,HoutaiData.ChargeMoneyLimit);
	if (ContentLen <= 0 ) return SD_FAILURE;
		
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0xC421,HoutaiData.Rate1);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0xC422,HoutaiData.Rate2);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0xC423,HoutaiData.Rate3);
	if (ContentLen <= 0 ) return SD_FAILURE;
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0xC424,HoutaiData.Rate4);
	if (ContentLen <= 0 ) return SD_FAILURE;
		
	ContentLen = 0;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8925,HoutaiData.OverCurProportion);
	if (ContentLen <= 0 ) return SD_FAILURE;
	
	ContentLen = 0;	
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8940,HoutaiData.OverVolProportion);
	if (ContentLen <= 0 ) return SD_FAILURE;		
		
	ContentLen = 0;	
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8942,HoutaiData.LowVolProportion);
	if (ContentLen <= 0 ) return SD_FAILURE;	
	
	ContentLen = 0;	
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8963,HoutaiData.ChargeCurLimit);
	if (ContentLen <= 0 ) return SD_FAILURE;

	ST_INT32 temp;	
	temp = CDZ_BCDTOHEX(HoutaiData.Rate1[0])+CDZ_BCDTOHEX(HoutaiData.Rate1[1])*100+CDZ_BCDTOHEX(HoutaiData.Rate1[2])*10000;
	HoutaiData.fRate1 = (float)(temp/100);
	temp = CDZ_BCDTOHEX(HoutaiData.Rate2[0])+CDZ_BCDTOHEX(HoutaiData.Rate2[1])*100+CDZ_BCDTOHEX(HoutaiData.Rate2[2])*10000;
	HoutaiData.fRate2 = (float)(temp/100);
	temp = CDZ_BCDTOHEX(HoutaiData.Rate3[0])+CDZ_BCDTOHEX(HoutaiData.Rate3[1])*100+CDZ_BCDTOHEX(HoutaiData.Rate3[2])*10000;
	HoutaiData.fRate3 = (float)(temp/100);
	temp = CDZ_BCDTOHEX(HoutaiData.Rate4[0])+CDZ_BCDTOHEX(HoutaiData.Rate4[1])*100+CDZ_BCDTOHEX(HoutaiData.Rate4[2])*10000;
	HoutaiData.fRate4 = (float)(temp/100);
	
	temp = CDZ_BCDTOHEX(HoutaiData.OverCurProportion[0])+CDZ_BCDTOHEX(HoutaiData.OverCurProportion[1])*100;
	HoutaiData.fOverCurLimit = 16*(float)temp/100;
	
	temp = CDZ_BCDTOHEX(HoutaiData.OverVolProportion[0])+CDZ_BCDTOHEX(HoutaiData.OverVolProportion[1])*100;
	
	HoutaiData.fOverVolLimit = 220*(float)temp/100;	
	printf("HoutaiData.fOverVolLimit is %7.2f\n",HoutaiData.fOverVolLimit);
	
	printf("HoutaiData.LowVolProportion[1] is %2X\n",HoutaiData.LowVolProportion[1]);
	temp = CDZ_BCDTOHEX(HoutaiData.LowVolProportion[0])+CDZ_BCDTOHEX(HoutaiData.LowVolProportion[1])*100;


	printf("temp is %2d\n",temp);
	HoutaiData.fLowVolLimit = 220*(float)temp/100;	
	
	temp = CDZ_BCDTOHEX(HoutaiData.ChargeCurLimit[0])+CDZ_BCDTOHEX(HoutaiData.ChargeCurLimit[1])*100;
	HoutaiData.fChargeCurLimit =16*(float)temp/100;
	
	temp = CDZ_BCDTOHEX(HoutaiData.ChargeMoneyLimit[0])+CDZ_BCDTOHEX(HoutaiData.ChargeMoneyLimit[1])*100+CDZ_BCDTOHEX(HoutaiData.ChargeMoneyLimit[2])*10000;
	HoutaiData.fChargeMoneyLimit =(float)temp/100;
//	HoutaiData.fChargeMoneyLimit = 604;			
	printf("fChargeCurLimit is %5.2f\n",HoutaiData.fChargeCurLimit);
//	HoutaiData.fOverCurLimit = 0.6;
	HoutaiData.fLowVolLimit = 240;
//	HoutaiData.fOverVolLimit = 200;
	HoutaiData.fChargeCurLimit = 0;	
	//HoutaiData.fRate1 = 10.00;
	
	
	printf("HoutaiData.fLowVolLimit is %7.2f\n",HoutaiData.fLowVolLimit);	
	return 0;
}




/*********************************************************************************************
		                            函数声明 
*********************************************************************************************/
void PaleIdleDealAllCard(void);
void MainControl(void);
void PaleIdleDealChargeCard(void);
void PaleChargingDealAllCard(void);
void PaleExceptDealAllCard(void);
void sendText(void);
void RcvText(void);
void InitData(void);
void CheckQiang(void);

void PaleIdleDealTestCard(void);
void PaleChargingDealTestCard(void);
void PaleIdleDealManageCard(void);
void PaleChargingDealManageCard(void);
void PaleIdleDealRepairCard(void);
void PaleChargingDealRepairCard(void);
void PaleIdleDealKeyModifyCard(void);
void PaleChargingDealKeyModifyCard(void);
void DisplayControl(void);
ST_INT32 GetCardMsg(void);
void ChargeLedOn(void);
void InitGPIO(void);
void ChargeLedOff(void);
void PaleExcepLedOn(void);
void PaleExcepLedOff(void);
void MeterExcep_Audio(void);

void ControlGPIO(ST_CHAR GPIO_ID,ST_CHAR GPIO_value);

ST_CHAR CheckFangleiqiBad(void);
ST_CHAR CheckPrepairDoorOpen(void);

void ScreenLedOn(void);
void ScreenLedOff(void);
void OutPWM(void);

void OpenChongDianCangLedOff(void);
void OpenQiangLedOff(void);
void OpenDengXiangLedOff(void);

void OpenChongDianCangLedOn(void);
void OpenQiangLedOn(void);
void OpenDengXiangLedOn(void);

ST_CHAR CheckChaTouConnect(void);

ST_CHAR  CheckTuRanDuanDian(void);
ST_UCHAR MeterExcep_Disp_EndCharge(void);
//ST_UCHAR MeterExcep_Disp_Idle(void);
void PageThank_Return(void);
void StartCharge(void);
ST_CHAR CheckJiTingBution(void);
void ControlRGYLed(void);
void IdleLedOn(void);
void ChargeFullLedOn(void);

/**********dump one msg ***/
void dump_msg(ChargeRecordTP *msg)
{
    dump_buff("SendHoutaiFlag", &msg->SendHoutaiFlag, sizeof(msg->SendHoutaiFlag));
    dump_buff("sExchangeType", &msg->sExchangeType, sizeof(msg->sExchangeType));
    dump_buff("sExchangeSeq", &msg->sExchangeSeq[0], sizeof(msg->sExchangeSeq));
    dump_buff("sAreaCode", &msg->sAreaCode[0], sizeof(msg->sAreaCode));
    dump_buff("sStartCardNo", &msg->sStartCardNo[0], sizeof(msg->sStartCardNo));
    dump_buff("sEndCardNo", &msg->sEndCardNo[0], sizeof(msg->sEndCardNo));
    dump_buff("sStartCardType", &msg->sStartCardType[0], sizeof(msg->sStartCardType));
    dump_buff("sEndCardType", &msg->sEndCardType[0], sizeof(msg->sEndCardType));
    dump_buff("sStartEnerge", &msg->sStartEnerge[0], sizeof(msg->sStartEnerge));
    dump_buff("sStartEnerge", &msg->sStartEnerge[0], sizeof(msg->sStartEnerge));
    dump_buff("sStartEnerge1", &msg->sStartEnerge1[0], sizeof(msg->sStartEnerge1));
    dump_buff("sStartEnerge2", &msg->sStartEnerge2[0], sizeof(msg->sStartEnerge2));
    dump_buff("sStartEnerge3", &msg->sStartEnerge3[0], sizeof(msg->sStartEnerge3));
    dump_buff("sStartEnerge4", &msg->sStartEnerge4[0], sizeof(msg->sStartEnerge4));
    dump_buff("sEndEnerge", &msg->sEndEnerge[0], sizeof(msg->sEndEnerge));
    dump_buff("sEndEnerge1", &msg->sEndEnerge1[0], sizeof(msg->sEndEnerge1));
    dump_buff("sEndEnerge2", &msg->sEndEnerge2[0], sizeof(msg->sEndEnerge2));
    dump_buff("sEndEnerge3", &msg->sEndEnerge3[0], sizeof(msg->sEndEnerge3));
    dump_buff("sEndEnerge4", &msg->sEndEnerge4[0], sizeof(msg->sEndEnerge4));
    dump_buff("sPrice1", &msg->sPrice1[0], sizeof(msg->sPrice1));
    dump_buff("sPrice2", &msg->sPrice2[0], sizeof(msg->sPrice2));
    dump_buff("sPrice3", &msg->sPrice3[0], sizeof(msg->sPrice3));
    dump_buff("sPrice4", &msg->sPrice4[0], sizeof(msg->sPrice4));
    dump_buff("sParkFeePrice", &msg->sParkFeePrice[0], sizeof(msg->sParkFeePrice));
    dump_buff("sStartTime", &msg->sStartTime[0], sizeof(msg->sStartTime));
    dump_buff("sEndTime", &msg->sEndTime[0], sizeof(msg->sEndTime));
    dump_buff("sParkFee", &msg->sParkFee[0], sizeof(msg->sParkFee));
    dump_buff("sGasPrice", &msg->sGasPrice[0], sizeof(msg->sGasPrice));
    dump_buff("sGasFee", &msg->sGasFee[0], sizeof(msg->sGasFee));
    dump_buff("sMoneyBefore", &msg->sMoneyBefore[0], sizeof(msg->sMoneyBefore));
    dump_buff("sMoneyAfter", &msg->sMoneyAfter[0], sizeof(msg->sMoneyAfter));
    dump_buff("sCardCounter", &msg->sCardCounter[0], sizeof(msg->sCardCounter));
    dump_buff("sTermNo", &msg->sTermNo[0], sizeof(msg->sTermNo));
    dump_buff("sCardVer", &msg->sCardVer[0], sizeof(msg->sCardVer));
    dump_buff("sPosNo", &msg->sPosNo[0], sizeof(msg->sPosNo));
    dump_buff("sCardStatus", &msg->sCardStatus, sizeof(msg->sCardStatus));
    dump_buff("sTimeLong", &msg->sTimeLong[0], sizeof(msg->sTimeLong));
}


/********************************************************************************* 
 函数名称： main
 功能描述：	应用程序入口函数，主要实现初始化和创建线程
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT32 main(void)
{

	int result;
	//ST_CHAR kahao[] = {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18},bcc;
//	ST_CHAR money[] = {0x31,0x32,0x33,0x34,0x35,0x2E,0x36,0x37};
	//PlaySound_Money(CURRENTPAID,money);
 //   char stmp[256] = { 0 };
#if 0
    ChargeRecordTP msg;

    bcd2str(stmp, kahao, 0, sizeof(kahao));

    init_db("mydata.db", "message");


    insert_db(&ChargeRecord4, "message");
    insert_db(&ChargeRecord5, "message");
	result = search_db("开始卡号", stmp, 1, &msg, "message");
    printf("search result %d\n",result);
	dump_msg(&msg);
    rm_db();
#endif
#if 0
    
    InitGPIO();
    while(1)
    {
	//PlaySound(NOMOVETHECAR,NULL);
	OpenDoor();
	sleep(2);
	CloseDoor();
	sleep(1);
	printf("Led off\n");
    }
#endif

#if 1
	pthread_t Mainthread;

	init_db("mydata.db", "message");
	init_alarm_db("alarm.db");
	create_alarm_table("alarm");
	create_table("message");
	InitGPIO();
	cdzLoadParam();
	InitCom();
	CheckQiang();
	CardInit();
	InitData();
	InitExpandBoard();
	InitTimer();	
//	cdzProtocolInit();
//	cdzGprsInit(); 
	GetHoutaiMsg();
	InitDisplay();
	InitSpi();
	InitData();
	OutPWM();
	
	while(0) 
	{
		result = GetCardMsg();
		printf("result:%d\n",result);
		sleep(1);
		rm_db();
	}	
    #if 1
	result = pthread_create(&Mainthread, PTHREAD_CREATE_JOINABLE, (void *)MainControl ,NULL);
	if(result)
	{
		perror("pthread_create: MainControl.\n");
		exit(EXIT_FAILURE);
	}	
	
	#endif
	while(1);
#endif
}




/********************************************************************************* 
 函数名称： InitData
 功能描述：	初始化全局变量
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void InitData(void)
{
	ChargeMsg.PaleStatus = PALEIDLE;
	ChargeRecord.SendHoutaiFlag = 0;
	EventFlag.QiangConGood = 0;	
	DisplayInfo.CardBalance = 0.00;
	DisplayInfo.MaxChargeEle = 0.00;
	ChargeMsg.f_ChargeRate = HoutaiData.fRate3;
	memset(ChargeRecord.sStartEnerge,0,4);
	memset(ChargeRecord.sStartEnerge1,0,4);
	memset(ChargeRecord.sStartEnerge2,0,4);
	memset(ChargeRecord.sStartEnerge3,0,4);
	memset(ChargeRecord.sStartEnerge4,0,4);
	memset(ChargeRecord.sEndEnerge,0,4);
	memset(ChargeRecord.sEndEnerge1,0,4);
	memset(ChargeRecord.sEndEnerge2,0,4);
	memset(ChargeRecord.sEndEnerge3,0,4);
	memset(ChargeRecord.sEndEnerge4,0,4);
	memset(ChargeRecord.sStartTime,0,6);
	memset(ChargeRecord.sEndTime,0,6);

	memset(ChargeRecord.sCardCounter,0,sizeof(ChargeRecord.sCardCounter));
	memset(ChargeRecord.sCardVer,0,sizeof(ChargeRecord.sCardVer));
	memset(ChargeRecord.sPosNo,0,sizeof(ChargeRecord.sPosNo));	
	memset(DisplayInfo.ChargedTime,0,sizeof(DisplayInfo.ChargedTime));
	memset(DisplayInfo.ChargingTime,0,sizeof(DisplayInfo.ChargingTime));

	//DisplayInfo.CurrentPage = DISP_IDLE;
	SetChargePara.SetStartChargeMode = 0;

	

}

/*********************************************************************************
 函数名称： Tanka
 功能描述：     发送弹卡命令，成功后继续寻卡
 输    入： 无
 输        出： RES_TRUE：发送成功，RES_FAULSE：发送失败
 返 回 值： 无
 作    者：     yansudan
 日    期：     2011.2.6
 修改记录：
*********************************************************************************/

void Tanka(void)
{
	EventFlag.IsExistCardFlag = 0;		
	CardMsg.Sendcmd = FIND_CARD;
	//DisplayInfo.CurrentPage = DISP_BRUSHCARD;

}



/********************************************************************************* 
 函数名称： CheckSignal
 功能描述：	检测输入信号
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void CheckSignal(void)
{
	CheckTuRanDuanDian();	
	CheckFangleiqiBad();
	CheckPrepairDoorOpen();
}



void ControlAllLed(void)
{
	if((MeterMsg.MeterCurrentTime[2]>=6)&&(MeterMsg.MeterCurrentTime[2]<19))
	{			
		EventFlag.ChongdianCangLedOnFg = 0;
		OpenDengXiangLedOff();
	}
	else
	{
		EventFlag.ChongdianCangLedOnFg = 1;
		OpenDengXiangLedOn();		
	}
}


void CheckScreenStatus(void)
{
	if( 1 == EventFlag.ScreenExcepFlag)
	{
		if( DisplayInfo.CurrentPage!=DISP_BRUSHCARD_EXIT|| PALECHARGING == ChargeMsg.PaleStatus  )
		{
			ChargeMsg.PaleStatus = PALEEXCEPTION;
		}		
	}

}


/********************************************************************************* 
 函数名称： MainControl
 功能描述：	主线程回调函数，实现在桩的三种状态下检测到所有卡的处理
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.9.5
 修改记录：
*********************************************************************************/

void MainControl(void)
{
	while(1)
	{
		printf("ChargeMsg.PaleStatus is %d EventFlag.MeterExcepFlag is %d\n",ChargeMsg.PaleStatus,EventFlag.MeterExcepFlag);
		//printf("EventFlag.CutdownEleFlag is %d\n",EventFlag.CutdownEleFlag);
		CheckDoorIsOpen();
		CheckChaTouConnect();
		CheckTuRanDuanDian();
		CheckJiTingBution();
		//ControlAllLed();	
		CheckPrepairDoorOpen();
		CheckScreenStatus();
		ControlRGYLed();
		
		switch(ChargeMsg.PaleStatus)
		{
			case PALEIDLE:
			{
				PaleIdleDealAllCard();
			}
			break;
			case PALECHARGING:
			{
				//printf("come to pale charging\n");
				PaleChargingDealAllCard();
			}
			break;		
			case PALEEXCEPTION:
			{
			//	printf("come to pale PaleExceptDealAllCardPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP\n");
				PaleExceptDealAllCard();
			}
			break;		

			default:
			    break;
		}

		usleep(50*1000);
	}
	
	
}

/********************************************************************************* 
 函数名称： PreChargeWait
 功能描述： 选择预充电后，刷卡完成后，进入充电等待状态
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.8.25
 修改记录：
*********************************************************************************/

void PreChargeWait(void)
{
	#if 0
	struct tm *stShowNowTime;
	time_t now;			
	now = time(0);
	stShowNowTime = localtime(&now);
	DisplayInfo.SysTime[0] = stShowNowTime->tm_sec;
	DisplayInfo.SysTime[1] = stShowNowTime->tm_min;
	DisplayInfo.SysTime[2] = stShowNowTime->tm_hour;
	DisplayInfo.SysTime[3] = stShowNowTime->tm_mday;
	DisplayInfo.SysTime[4] = stShowNowTime->tm_mon+1;
	DisplayInfo.SysTime[5] = stShowNowTime->tm_year%100;
	printf("DisplayInfo.SysTime[0] is %d\n",DisplayInfo.SysTime[0]);
	#endif
	
	if((DisplayInfo.SysTime[2]==SetChargePara.PreChargeTime[1])&&(DisplayInfo.SysTime[1]==SetChargePara.PreChargeTime[0]))
	{
		StartCharge();
		PlaySound(BEGINECHARGE,NULL);		
	//	Tanka();						
	}
}

/********************************************************************************* 
 函数名称： PaleIdleDealAllCard
 功能描述：	充电桩在空闲状态下检测到所有卡的处理
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6 
 修改记录：
*********************************************************************************/

void PaleIdleDealAllCard(void)
{
//	static char OpenRepairDoorflag = 0;
//	printf("EventFlag.IsExistCardFlag is %d\n",EventFlag.IsExistCardFlag);
//	printf("EventFlag.ChargeCompleteFlag is %d\n",EventFlag.ChargeCompleteFlag);
	 
	if(EventFlag.IsExistCardFlag==1)
	{
		
		if(CardMsg.IsSystemCard)
		{
			switch(CardMsg.CardType)
			{
				case CHARGECARD:
				{
					
					PaleIdleDealChargeCard();
				}
				break;
				case TESTCARD:
				{					
					printf("This is a test card\n");
					//PaleIdleDealTestCard();
				}
				break;
				case MANAGECARD:
				{

					printf("This is a manage card\n");
					//PaleIdleDealManageCard();
				}
				break;
				case REPAIRCARD:
				{
					printf("This is a repair card\n");
				//	PaleIdleDealRepairCard();
				}
				break;
				case MODIFYKEYCARD:
				{
					//PlaySound(AUDIO_MODIFYKEYCARD,NULL);
					//sleep(3);
				//printf("This is a KeyModify card\n");
				//	PaleIdleDealKeyModifyCard();
				}
				break;
				case SETPARACARD:
				{
					
				}
				break;
				default:
				{
					
				//	Tanka();

				}
				break;
			}
		}
	}
	
	else if(DisplayInfo.CurrentPage == DISP_WAIT_CHARGE && EventFlag.StartChargeFlag==3)
	{
		
		PreChargeWait();
	}

	
}

/********************************************************************************* 
 函数名称： GetCardMsg
 功能描述：	在充电记录中获得已知卡号的充电记录
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_INT32  GetCardMsg(void)
{
	int FindResult = 1 ;
#if 1
	ChargeRecordTP *p; //保存从数据库中读出来的记录
	p = &ChargeMsg.LastRecord;

	char temp[50] = { 0 };
	
	bcd2str(temp, (ST_CHAR *)CardMsg.CardSeria, 0,CARDSERIALEN);
	FindResult = search_db("开始卡号", temp,1, p ,"message");
	printf("FindResult is %d ChargeMsg.LastRecord.sCardStatus is %d\n",FindResult,ChargeMsg.LastRecord.sCardStatus);
	if(FindResult)
	{
		if(ChargeMsg.LastRecord.sCardStatus==CARDLOCK)
		{
			FindResult = 1;
		}
		else FindResult = 0;
	}
#endif
	return FindResult;
}


/********************************************************************************* 
 函数名称： PayMoney
 功能描述：	发送结束充电命令给读卡器，实现扣钱和对卡的解锁
 输    入：	Money：需要扣除费用
 输	   出： RES_TRUE：扣钱成功，RES_FAULSE：扣钱失败
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_INT32 PayMoney(ST_FLOAT Money) 
{
    printf("Paid money is :%f\n",Money);
	FloatToHex4(Money+0.005,CardMsg.ChargeMoney);	
	printf("Charge Money:%d",CardMsg.ChargeMoney[0]);

	printf("Charge Money:%d",CardMsg.ChargeMoney[1]);
	printf("Charge Money:%d",CardMsg.ChargeMoney[2]);
	printf("Charge Money:%d",CardMsg.ChargeMoney[3]);
	return TxToCardEndChargeCmd();
						
}


typedef enum gpio_enum {
    GPIO_IN0,
    GPIO_IN1,
    GPIO_IN2,
    GPIO_IN3,
    GPIO_IN4,
    GPIO_IN5,
    GPIO_IN6,
    GPIO_IN7,
    GPIO_IN8,
    GPIO_IN9,
    GPIO_IN10,
    GPIO_IN11,
    GPIO_OUT0,
    GPIO_OUT1,
    GPIO_OUT2,
    GPIO_OUT3,
    GPIO_OUT4,
    GPIO_OUT5,
    GPIO_OUT6,
    GPIO_OUT7,
    GPIO_OUT8,
    GPIO_OUT9,
    GPIO_OUT10,
    GPIO_OUT11,
    GPIO_OUT12,
    GPIO_OUT13,
    GPIO_OUT14,
    GPIO_INVALID
} GPIO_ENUM;

static struct gpio_struct {
    GPIO_ENUM name;
    int id;
} gpio[] = {
    { GPIO_IN0, 145 },
    { GPIO_IN1, 111 },
    { GPIO_IN2, 109 },
    { GPIO_IN3, 108 },
    { GPIO_IN4, 107 },
    { GPIO_IN5, 106 },
    { GPIO_IN6, 103 },
    { GPIO_IN7, 102 },
    { GPIO_IN8, 117 },
    { GPIO_IN9, 118 },
    { GPIO_IN10, 149 },
    { GPIO_IN11, 150 },
    { GPIO_OUT0, 58 },
    { GPIO_OUT1, 41 },
    { GPIO_OUT2, 40 },
    { GPIO_OUT3, 39 },
    { GPIO_OUT4, 38 },
    { GPIO_OUT5, 191 },
    { GPIO_OUT6, 156 },
    { GPIO_OUT7, 158 },
    { GPIO_OUT8, 60 },
    { GPIO_OUT9, 63 },
    { GPIO_OUT10,56 },

};


/********************************************************************************* 
 函数名称： InitGPIO
 功能描述：	初始化IO口
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void InitGPIO(void)
{

//	ST_CHAR value[5];
	char cmd[256];
	int i;

	printf("initiate gpio\n");

	for(i = 0; i < ARRAY_SIZE(gpio); i++) 
	{
	    sprintf(cmd, "echo %d > %s", gpio[i].id, CREATE_IO);
	    system(cmd);

	    if (i < GPIO_OUT0) 
	    {
			sprintf(cmd, "echo \"in\" > /sys/class/gpio/gpio%d/direction", gpio[i].id);
	    } 
	    else
	    {
			sprintf(cmd, "echo \"out\" > /sys/class/gpio/gpio%d/direction", gpio[i].id);
	    }
	 //   puts(cmd);
	    system(cmd);
	}
	#if 1
	ControlGPIO(OUT0_58_DianCiSuo,RELAY_OFF);
	ControlGPIO(OUT1_41_ChongDianQiang,RELAY_OFF); 
	ControlGPIO(OUT1_QiangLed,RELAY_OFF);
	ControlGPIO(OUT6_ChargeLed,RELAY_OFF);
	ControlGPIO(OUT4_JieChuQi,RELAY_OFF);

	ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_OFF);
	ControlGPIO(OUT5_DengXiangLed,RELAY_ON);
	ControlGPIO(OUT7_IdleLed,RELAY_ON);
	
	
	//ControlGPIO(OUT_56_ScreenLed,0);
	#endif
	#if 0
	ControlGPIO(OUT0_58_DianCiSuo,0);
	ControlGPIO(OUT1_41_ChongDianQiang,0); 
	ControlGPIO(OUT2_40_LogoLed,0);
	ControlGPIO(OUT3_39_DengXiang,1);
	ControlGPIO(OUT5_191_JieChuQiOff,0);
	ControlGPIO(OUT4_38_JieChuQiOn,0);
	ControlGPIO(OUT8_62_ChargeLed,0);
	ControlGPIO(OUT9_63_ExceptionLed,0);
	ControlGPIO(OUT6_97,0);
	ControlGPIO(OUT7_96,0);
	
	ControlGPIO(OUT_56_ScreenLed,0);
	#endif
}

void ControlRGYLed(void)
{
	if( PALEIDLE == ChargeMsg.PaleStatus )
	{
		if(1 == EventFlag.ChargeFullFlag|| 1 == EventFlag.ChargeCompleteFlag)
		{
			ChargeFullLedOn();
		}
		else if(EventFlag.MeterExcepFlag!=0||EventFlag.CardExcepFlag==1||EventFlag.RepairDoorOpenFg==1||EventFlag.ScreenExcepFlag == 1
				|| EventFlag.CutdownEleFlag==1)
		{
			PaleExcepLedOn();
		}
		else 
		{
			IdleLedOn();
		}
	}
	else if( PALECHARGING == ChargeMsg.PaleStatus )
	{
		ChargeLedOn();
	}
	else if( PALEEXCEPTION == ChargeMsg.PaleStatus )
	{		
		PaleExcepLedOn();
	}
		
}

/********************************************************************************* 
 函数名称： ChargeLedOn
 功能描述：	点亮充电指示灯
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ChargeLedOn(void)
{
	ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_OFF);
	ControlGPIO(OUT7_IdleLed,RELAY_OFF);
	
	ControlGPIO(OUT6_ChargeLed,RELAY_ON);
	
}


/********************************************************************************* 
 函数名称： ChargeLedOff
 功能描述：	灭充电指示灯
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ChargeLedOff(void)
{
//	ControlGPIO(OUT6_ChargeLed,RELAY_OFF);
}


/********************************************************************************* 
 函数名称： ScreenLedOff
 功能描述：	灭充电指示灯
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ScreenLedOff(void)
{
	//ControlGPIO(OUT6_ChargeLed,0);
}

/********************************************************************************* 
 函数名称： ScreenLedOn
 功能描述：	灭充电指示灯
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ScreenLedOn(void)
{
	//ControlGPIO(OUT_56_ScreenLed,1);
}




/********************************************************************************* 
 函数名称： GetIOSignal
 功能描述：	读取GPIO信号
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR GetIOSignal(ST_CHAR GPIO_ID)
{
	ST_INT32 fd;
	ST_CHAR value[5],len;
	char path[256];

	sprintf(path, "/sys/class/gpio/gpio%d/value", GPIO_ID);
	
	fd = open(path,O_RDONLY);
	len= read(fd,value,1);

	if(len>0)
	{
		if(value[0]==0x30)
		{
			close(fd);
			return 0;
		}
		
		if(value[0]==0x31)
		{
			close(fd);
			return 1;
		}
			
	}
	return 0;
}



/********************************************************************************* 
 函数名称： ControlGPIO
 功能描述：	IO口的控制
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ControlGPIO(ST_CHAR GPIO_ID,ST_CHAR GPIO_value)
{

	ST_INT32 fd;
	ST_CHAR value[5],len;
	char path[256];

	sprintf(path, "/sys/class/gpio/gpio%d/value", GPIO_ID);
	
	fd = open(path, O_RDWR);
	len = sprintf(value, "%d", GPIO_value);
	write(fd, value, len);
	
	close(fd);

}

/********************************************************************************* 
 函数名称： OutPWM
 功能描述：	输出脉宽为%50，1000HZ的波形
 输    入：	无
 输	   出： 无
 返 回 值： 
 作    者：	yansudan
 日    期：	2011.4.18
 修改记录：
*********************************************************************************/

void OutPWM(void)
{
	ST_INT32 fd;
	ST_CHAR value[5],len;
	char path[256];
	sprintf(path, "/sys/class/leds/pwm3/brightness%d/value", OUT_PWM);
	fd = open(path, O_RDWR);
	len = sprintf(value, "%d", PW_SACLE);
	write(fd, value, len);

	close(fd);	
}


/********************************************************************************* 
 函数名称： CheckDoorIsClosed
 功能描述：	检测插座门是否关好
 输    入：	无
 输	   出： 无
 返 回 值：  返回1：关好，返回0：未关好
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR CheckDoorIsClosed(void)
{
	#if 1
	ST_CHAR ret;
	ret = GetIOSignal(IN2_109_ChaZuoMen);
	if(ret==0)
	{
//	ControlGPIO(OUT1_41_ChongDianQiang,RELAY_OFF);
		OpenChongDianCangLedOff();
	//StopCompose();
	printf("Door input 0\n");
	return 1;
	}
	else	
	{
		OpenChongDianCangLedOn();
		printf("Door input 1\n");
		return 0;
	}
	#endif
	#if 0
	return 1;
	#endif
}


/********************************************************************************* 
 函数名称： CheckTuRanDuanDian
 功能描述： 实时检测是否突然断电，若突然断电则EventFlag.CutdownEleFlag置1，否则0
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/



ST_CHAR  CheckTuRanDuanDian(void)
{
	#if 1
	ST_CHAR ret;
	ret = GetIOSignal(IN4_107_DuanDian);
	
//	printf("tu ran du dian ret is %d\n",ret);
	if(ret==0)
	{
		if( DISP_IDLE == DisplayInfo.CurrentPage||DISP_EXCEP_IDLE == DisplayInfo.CurrentPage|| ChargeMsg.PaleStatus == PALECHARGING||
			DISP_EXCEP_IDLE_A == DisplayInfo.CurrentPage ||DISP_EXCEP_IDLE_B == DisplayInfo.CurrentPage||DISP_NOPAPLE_IDLE == DisplayInfo.CurrentPage)
		{
			ChargeMsg.PaleStatus = PALEEXCEPTION;
		}
		EventFlag.CutdownEleFlag = 1;
		//OpenChongDianCangLedOff();
		OpenDengXiangLedOff();	
		printf("EventFlag.CutdownEleFlag is %d ChargeMsg.PalesStatus is %d DisplayInfo.CurrentPage is %d\n",EventFlag.CutdownEleFlag,ChargeMsg.PaleStatus,DisplayInfo.CurrentPage);	
		return 1;
	}
	else 
	{
		if(EventFlag.CutdownEleFlag==1)
		{
			if(GetIOSignal(IN3_108_JieChuQi))
			{
				EventFlag.CutdownEleFlag = 0;
				printf("CheckTuRanDuanDian is set 0\n");
				//OpenChongDianCangLedOn();
				OpenDengXiangLedOn();
			}

		}
	//	printf("lai dian\n");
		return 0;
	}
	#endif
	#if 0
	return 1;
	#endif
}


/********************************************************************************* 
 函数名称： CheckJiTingBution
 功能描述： 实时检测急停按钮是否按下，若突若按下则EventFlag.CutdownEleFlag置1，否则0
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR CheckJiTingBution(void)
{
	ST_CHAR ret;

	ret = GetIOSignal(IN3_108_JieChuQi);
 
	//printf("ji ting ret is %d\n",ret);
	if(ret==0)
	{
		EventFlag.CutdownEleFlag = 1;
		if( DISP_IDLE == DisplayInfo.CurrentPage||DISP_EXCEP_IDLE == DisplayInfo.CurrentPage|| ChargeMsg.PaleStatus == PALECHARGING||
			DISP_EXCEP_IDLE_A == DisplayInfo.CurrentPage ||DISP_EXCEP_IDLE_B == DisplayInfo.CurrentPage||DISP_NOPAPLE_IDLE == DisplayInfo.CurrentPage)
		{
			ChargeMsg.PaleStatus = PALEEXCEPTION;
		}		
	//	OpenChongDianCangLedOff();
	//	OpenDengXiangLedOff();		

		return 1;
	}
	else 
	{
		if(EventFlag.CutdownEleFlag==1)
		{
			if(GetIOSignal(IN4_107_DuanDian))
			EventFlag.CutdownEleFlag = 0;
//			PlaySound(AUDIO_ZHONGDUAN_LAIDIAN,NULL);
		}

		return 0;
	}
	
}



/********************************************************************************* 
 函数名称： CheckChaTouConnect
 功能描述：	检测充电插头是否连接
 输    入：	无
 输	   出： 无
 返 回 值：  返回1：关好，返回0：未关好
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR CheckChaTouConnect(void)
{
	#if 1
//	static int i;
	ST_CHAR ret_cp0,ret_cp1,ret_cc0,ret_cc1;
	ret_cp0 = GetIOSignal(PW_CHECK_CP0);
	ret_cp1 = GetIOSignal(PW_CHECK_CP1);
	ret_cc0 = GetIOSignal(PW_CHECK_CC0);
	ret_cc1 = GetIOSignal(PW_CHECK_CC1);
	//printf("ret_cp0 is %d\n",ret_cp0);
	//printf("ret_cp1 is %d\n",ret_cp1);
	//printf("ret_cc0 is %d\n",ret_cc0);
	//printf("ret_cc1 is %d\n",ret_cc1);
//	printf("EventFlag.QiangConGood is %d\n",EventFlag.QiangConGood);
	 
#if 0
	    printf("ret_cp0 is %d\n",ret_cp0);
        printf("ret_cp1 is %d\n",ret_cp1);
     //   printf("ret_cc0 is %d\n",ret_cc0);
       // printf("ret_cc1 is %d\n",ret_cc1);
#endif
#if 0	
	ret_IN0_145_FangLeiQi = GetIOSignal(ret_IN0_145_FangLeiQi);
	ret_IN1_111_JianXiuMen = GetIOSignal(ret_IN1_111_JianXiuMen);
	ret_IN2_109_ChaZuoMen = GetIOSignal(ret_IN2_109_ChaZuoMen);
	ret_IN3_108_JieChuQi = GetIOSignal(ret_IN3_108_JieChuQi);
	ret_IN4_107 = GetIOSignal(ret_IN4_107);
	ret_IN5_106 = GetIOSignal(ret_IN5_106);
	ret_IN2_109 = GetIOSignal(ret_IN2_109);
	ret_IN7_102 = GetIOSignal(ret_IN7_102);
	
	printf("ret_cp0 is %d\n",ret_cp0);
	printf("ret_cp1 is %d\n",ret_cp1);
	printf("ret_cc0 is %d\n",ret_cc0);
	printf("ret_cc1 is %d\n",ret_cc1);
	
	printf("ret_IN0_145_FangLeiQi is %d\n",ret_IN0_145_FangLeiQi);
	printf("ret_IN1_111_JianXiuMen is %d\n",ret_IN1_111_JianXiuMen);
	printf("ret_IN2_109_ChaZuoMen is %d\n",ret_IN2_109_ChaZuoMen);
	printf("ret_IN3_108_JieChuQi is %d\n",ret_IN3_108_JieChuQi);
	printf("ret_IN4_107 is %d\n",ret_IN4_107);
	printf("ret_IN5_106 is %d\n",ret_IN5_106);
	printf("ret_IN2_109 is %d\n",ret_IN2_109);
	printf("ret_IN7_102 is %d\n",ret_IN7_102);
	i++;
	printf ("i is %d\n",i);
	
#endif
//	if(ret_cp0==1&&ret_cp1==1&&ret_cc0==1)
	if(ret_cp0==1)
	{
		EventFlag.QiangConGood = 1;
		OpenQiangLedOn();
		return 1;
	}
//	EventFlag.QiangConGood = 0;
	
	if((ret_cp0==0&&ret_cp1==0)||(ret_cc0==0&&ret_cc1==0))
	{
		EventFlag.QiangConGood = 0;
		OpenQiangLedOff();
		if(1 == EventFlag.ChargeCompleteFlag||1 == EventFlag.ChargeFullFlag)
		{
			EventFlag.ChargeCompleteFlag = 0;
			EventFlag.ChargeFullFlag = 0;
		}
		if( ChargeMsg.PaleStatus == PALECHARGING )
		{			
			if(EventFlag.IsExistCardFlag==0)
			ChargeMsg.PaleStatus = PALEEXCEPTION;
			return 0;
		}
	}
	#endif
	#if 0
	return 1;
	#endif
return 0;
}


void OpenChongDianCangLedOn(void)
{
	ControlGPIO(OUT1_41_ChongDianQiang,RELAY_ON); 
}
void OpenChongDianCangLedOff(void)
{
	ControlGPIO(OUT1_41_ChongDianQiang,RELAY_OFF); 
}


void OpenQiangLedOn(void)
{
	ControlGPIO(OUT1_QiangLed,RELAY_ON);
}
void OpenQiangLedOff(void)
{
	ControlGPIO(OUT1_QiangLed,RELAY_OFF); 
}


void OpenDengXiangLedOn(void)
{
	ControlGPIO(OUT5_DengXiangLed,RELAY_ON); 
}
void OpenDengXiangLedOff(void)
{
	ControlGPIO(OUT5_DengXiangLed,RELAY_OFF); 
}





/********************************************************************************* 
 函数名称： CheckDoorIsOpen
 功能描述：	检测插座门是打开
 输    入：	无
 输	   出： 无
 返 回 值： 返回1：打开，返回0：未打开
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR CheckDoorIsOpen(void)
{
	#if 1
	ST_CHAR ret;
	ret = GetIOSignal(IN2_109_ChaZuoMen);
	if(ret==1)
	{
		//if(EventFlag.ChongdianCangLedOnFg==1)
		{	
			//ControlGPIO(OUT1_41_ChongDianQiang,RELAY_ON);
			OpenChongDianCangLedOn();
		}
	
		//StopCompose();
	//	printf("Door input 1\n");
		return 1;
	}
	else 
	{
		printf("Door input 0\n");
		OpenChongDianCangLedOff();
		return 0;
	}
	#endif 
	#if 0
	return 1;
	#endif 

}


/********************************************************************************* 
 函数名称： CheckPrepairDoorOpen
 功能描述：	检测检修门是否打开
 输    入：	无
 输	   出： 无
 返 回 值：返回1：打开，返回0：关好
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR CheckPrepairDoorOpen(void)
{
	#if 1
	ST_CHAR ret;
	ret = GetIOSignal(IN1_111_JianXiuMen);
	if(ret==1)
	{
	//	printf("jian xiu men da kai DisplayInfo.CurrentPage is %d\n",DisplayInfo.CurrentPage);		
		//if( DISP_IDLE == DisplayInfo.CurrentPage|| ChargeMsg.PaleStatus == PALECHARGING )
		if( DISP_IDLE == DisplayInfo.CurrentPage||DISP_EXCEP_IDLE == DisplayInfo.CurrentPage|| ChargeMsg.PaleStatus == PALECHARGING||
			DISP_EXCEP_IDLE_A == DisplayInfo.CurrentPage ||DISP_EXCEP_IDLE_B == DisplayInfo.CurrentPage||DISP_NOPAPLE_IDLE == DisplayInfo.CurrentPage)
		{
			EventFlag.RepairDoorOpenFg = 1;
			ChargeMsg.PaleStatus = PALEEXCEPTION;
		//	printf("CheckPrepairDoorOpen ChargeMsg.PaleStatus is %d\n",ChargeMsg.PaleStatus);
		}
		return 1;
	}
	EventFlag.RepairDoorOpenFg = 0;
//	printf("EventFlag.RepairDoorOpenFg is %d\n",EventFlag.RepairDoorOpenFg);
	#endif
	#if 0
	return 1;
	#endif
	//PaleExcepLedOff();
	return 0;
}



/********************************************************************************* 
 函数名称： CheckFangleiqiBad
 功能描述：	检测防雷器是否故障
 输    入：	无
 输	   出： 无
 返 回 值：返回1：故障，返回0：未故障
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_CHAR CheckFangleiqiBad(void)
{
	ST_CHAR ret;
	ret = GetIOSignal(IN0_145_FangLeiQi);
	if(ret==1)
	{
		printf("Fangleiqi is bad\n");
		return 1;
	}
	return 0;
}

/********************************************************************************* 
 函数名称： OpenDoor
 功能描述：	打开插座门
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void OpenDoor(void)
{
	ControlGPIO(OUT0_58_DianCiSuo,RELAY_ON);

	printf("*******************************open door*************************************\n");
}


/********************************************************************************* 
 函数名称： CloseDoor
 功能描述：	关闭插座门
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void CloseDoor(void)
{
	ControlGPIO(OUT0_58_DianCiSuo,RELAY_OFF);
	printf("*******************************close door*************************************\n");
}



/********************************************************************************* 
 函数名称： JieChuQiOn
 功能描述：	开启交流接触器
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void JieChuQiOn(void)
{
	 
	ControlGPIO(OUT4_JieChuQi,RELAY_ON);
	//ControlGPIO(OUT5_191_JieChuQiOff,1);
	//ControlGPIO(OUT4_38_JieChuQiOn,1);
	
}

/********************************************************************************* 
 函数名称： JieChuQiOff
 功能描述：	断开交流接触器
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void JieChuQiOff(void)
{
	ControlGPIO(OUT4_JieChuQi,RELAY_OFF);
	//ControlGPIO(OUT5_191_JieChuQiOff,0);
	//ControlGPIO(OUT4_38_JieChuQiOn,0);
}

/********************************************************************************* 
 函数名称： IdleLedOn
 功能描述：	点亮待机状态灯
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.8.6
 修改记录：
*********************************************************************************/

void IdleLedOn(void)
{
	ControlGPIO(OUT6_ChargeLed,RELAY_OFF);
	ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_OFF);
	ControlGPIO(OUT7_IdleLed,RELAY_ON);	
}

/********************************************************************************* 
 函数名称： IdleLedOff
 功能描述：	点灭待机状态灯
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.8.6
 修改记录：
*********************************************************************************/

void IdleLedOff(void)
{
	//ControlGPIO(OUT7_IdleLed,RELAY_ON);	
}


/********************************************************************************* 
 函数名称： PaleExcepLedOn
 功能描述：	点亮充满指示灯
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ChargeFullLedOn(void)
{
	ControlGPIO(OUT7_IdleLed,RELAY_OFF);
	ControlGPIO(OUT6_ChargeLed,RELAY_OFF);
	ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_ON);
}

/********************************************************************************* 
 函数名称： PaleExcepLedOn
 功能描述：	点灭充满指示灯
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void ChargeFullLedOff(void)
{
	//ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_OFF);
}


/********************************************************************************* 
 函数名称： PaleExcepLedOn
 功能描述：	点亮故障指示灯
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PaleExcepLedOn(void)
{

	static struct timeval start,end;
	static ST_LONG timeuse = 0;
	static ST_CHAR TimerFlag = 0,YellowledOnflag = 0;

	#if 1
	if(1 == EventFlag.ChargeFullFlag||1 == EventFlag.ChargeCompleteFlag)
	{
		ChargeFullLedOn();
	}
	else
	{
		ControlGPIO(OUT7_IdleLed,RELAY_OFF);
		ControlGPIO(OUT6_ChargeLed,RELAY_OFF);
		
		if(0 == TimerFlag)
		{
			gettimeofday (&start ,NULL);
	
			TimerFlag = 1;
		}

		if(0 == YellowledOnflag)
		{
			ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_ON);
			
			gettimeofday (&end ,NULL);
			timeuse = ((1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000);
		//	printf("timeuse start is %ld",timeuse);
			//printf("end.tv_sec is %ld start.tv_sec is %ld end.tv_usec is %ld start.tv_usec is %ld",end.tv_sec,start.tv_sec,end.tv_usec,start.tv_usec);
			if(timeuse>500)
			{
			//	printf("timeuse start is %ld",timeuse);
				ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_OFF);	
				timeuse = 0;
				YellowledOnflag = 1;
				start = end;
			}
		}
		if(1 == YellowledOnflag)
		{
			gettimeofday (&end ,NULL);
			timeuse = ((1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000);
			if(timeuse>500)
			{
			//	printf("timeuse end is %ld",timeuse);
				timeuse = 0;
				YellowledOnflag = 0;
				TimerFlag = 0;
			}
		}

	}
	#endif
	//ControlGPIO(OUT9_63_ExceptionLed,1);

}

void PaleExcepLedOff(void)
{
	#if 0
	if(1 == EventFlag.ChargeFullFlag||1 == EventFlag.ChargeCompleteFlag)
	{
		ChargeFullLedOn();	
	}
	else
	{
		ControlGPIO(OUT8_Excep_ChargeFull_led,RELAY_OFF);
	}
	#endif
	//ControlGPIO(OUT9_63_ExceptionLed,0);
}


/********************************************************************************* 
 函数名称： CheckQiang
 功能描述：	检测充电枪是否插好，如果插好，函数中的两个参数置1
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void CheckQiang(void)
{
	//EventFlag.IsChargeQiang = 0;
	EventFlag.QiangConGood = 1;
}

/********************************************************************************* 
 函数名称： PackCardMsg
 功能描述：打包第一次刷卡后卡信息和充电单价界面
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PackCardMsg(void)
{
	sprintf((char*)DisplayMsg.Displaybuf,"%2X%2X%2X%2X%2X%2X%2X%2X",CardMsg.CardSeria[0],CardMsg.CardSeria[1],
	CardMsg.CardSeria[2],CardMsg.CardSeria[3],CardMsg.CardSeria[4],
	CardMsg.CardSeria[5],CardMsg.CardSeria[6],CardMsg.CardSeria[7]);
	
	sprintf((char*)&DisplayMsg.Displaybuf[16],"%7.2f",CardMsg.f_CardMoney);
	sprintf((char*)&DisplayMsg.Displaybuf[23],"%7.4f",HoutaiData.fRate2);
	sprintf((char*)&DisplayMsg.Displaybuf[30],"%7.4f",HoutaiData.fRate3);
	sprintf((char*)&DisplayMsg.Displaybuf[37],"%7.4f",HoutaiData.fRate4);
	sprintf((char*)&DisplayMsg.Displaybuf[44],"%7.4f",ChargeMsg.f_ChargeRate);
	
}

/********************************************************************************* 
 函数名称： PackChargeData
 功能描述：打包充电中界面的显示数据
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void PackChargeData_F(void)
{
	sprintf((char*)DisplayMsg.Displaybuf,"%9.2f",ChargeMsg.f_TotalChargeEle);	
	sprintf((char*)&DisplayMsg.Displaybuf[9],"%9.2f",ChargeMsg.f_ChargeEle2);
	sprintf((char*)&DisplayMsg.Displaybuf[18],"%7.4f",HoutaiData.fRate2);
	sprintf((char*)&DisplayMsg.Displaybuf[25],"%7.2f",ChargeMsg.f_CurrentChargeMoney);
	sprintf((char*)&DisplayMsg.Displaybuf[32],"%7.2f",ChargeMsg.f_CurrentBalance);
	sprintf((char*)&DisplayMsg.Displaybuf[39],"%4d",ChargeMsg.ChargeTmLong[2]);	
	sprintf((char*)&DisplayMsg.Displaybuf[43],"%2d",ChargeMsg.ChargeTmLong[1]);	
	sprintf((char*)&DisplayMsg.Displaybuf[45],"%2d",ChargeMsg.ChargeTmLong[0]);	
}
void PackChargeData_P(void)
{
	sprintf((char*)DisplayMsg.Displaybuf,"%9.2f",ChargeMsg.f_TotalChargeEle);
	
	sprintf((char*)&DisplayMsg.Displaybuf[9],"%9.2f",ChargeMsg.f_ChargeEle3);
	sprintf((char*)&DisplayMsg.Displaybuf[18],"%7.4f",HoutaiData.fRate3);
	sprintf((char*)&DisplayMsg.Displaybuf[25],"%7.2f",ChargeMsg.f_CurrentChargeMoney);
	sprintf((char*)&DisplayMsg.Displaybuf[32],"%7.2f",ChargeMsg.f_CurrentBalance);
	sprintf((char*)&DisplayMsg.Displaybuf[39],"%4d",ChargeMsg.ChargeTmLong[2]);	
	sprintf((char*)&DisplayMsg.Displaybuf[43],"%2d",ChargeMsg.ChargeTmLong[1]);	
	sprintf((char*)&DisplayMsg.Displaybuf[45],"%2d",ChargeMsg.ChargeTmLong[0]);	
}
void PackChargeData_G(void)
{
	sprintf((char*)DisplayMsg.Displaybuf,"%9.2f",ChargeMsg.f_TotalChargeEle);

	sprintf((char*)&DisplayMsg.Displaybuf[9],"%9.2f",ChargeMsg.f_ChargeEle4);
	sprintf((char*)&DisplayMsg.Displaybuf[18],"%7.4f",HoutaiData.fRate4);
	sprintf((char*)&DisplayMsg.Displaybuf[25],"%7.2f",ChargeMsg.f_CurrentChargeMoney);
	sprintf((char*)&DisplayMsg.Displaybuf[32],"%7.2f",ChargeMsg.f_CurrentBalance);
	sprintf((char*)&DisplayMsg.Displaybuf[39],"%4d",ChargeMsg.ChargeTmLong[2]);	
	sprintf((char*)&DisplayMsg.Displaybuf[43],"%2d",ChargeMsg.ChargeTmLong[1]);	
	sprintf((char*)&DisplayMsg.Displaybuf[45],"%2d",ChargeMsg.ChargeTmLong[0]);	
}



/********************************************************************************* 
 函数名称： PackTestSoftwareMsg
 功能描述：打包发送给充电测试软件充电信息
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PackTestSoftwareMsg(void)
{
	int i =0;
		//add by 11-03-26 15:02 汉光添加测试软件信息
	if(DisplayMsg.CurrentPage ==DISPCHARGING)
	{
		ChargeMsg.SendTestSoftwareMsg[0] = 0x68;
		ChargeMsg.SendTestSoftwareMsg[1] = 48;
		memcpy(&ChargeMsg.SendTestSoftwareMsg[2],DisplayMsg.Displaybuf,47);
		ChargeMsg.SendTestSoftwareMsg[49] = ChargeMsg.PaleStatus+0x30;
		for(i = 0;i<50;i++);
		{
			if(ChargeMsg.SendTestSoftwareMsg[i]==0x00||ChargeMsg.SendTestSoftwareMsg[i]==0x20)
{
	ChargeMsg.SendTestSoftwareMsg[i] = 0x30;
}
		}
		ChargeMsg.SendTestSoftwareMsg[50] = GetBCC(ChargeMsg.SendTestSoftwareMsg,50);
		ChargeMsg.SendTestSoftwareMsg[51] = 0x16;
	}
	else
	{
		ChargeMsg.SendTestSoftwareMsg[0] = 0x68;
		ChargeMsg.SendTestSoftwareMsg[1] = 48;
		memset(&ChargeMsg.SendTestSoftwareMsg[2],0x30,47);
		printf("Chargepalestatus %d\n",ChargeMsg.PaleStatus);
		if(ChargeMsg.PaleStatus==0||ChargeMsg.PaleStatus==1)
		{
		ChargeMsg.SendTestSoftwareMsg[49] = 0x30;
		}
		if(ChargeMsg.PaleStatus==2)
		{
			ChargeMsg.SendTestSoftwareMsg[49] = 0x32;
		}
                ChargeMsg.SendTestSoftwareMsg[50] = GetBCC(ChargeMsg.SendTestSoftwareMsg,50);
		ChargeMsg.SendTestSoftwareMsg[51] = 0x16;
	}
	write(Screenfd,ChargeMsg.SendTestSoftwareMsg,52);
}


/********************************************************************************* 
 函数名称： PackCurrentBanlanceData
 功能描述：打包显示当前金额和本次消费金额界面的显示数据
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void PackCurrentBanlanceData(void)
{
	sprintf((char*)DisplayMsg.Displaybuf,"%7.2f",ChargeMsg.f_CurrentBalance);
	sprintf((char*)&DisplayMsg.Displaybuf[7],"%7.2f",ChargeMsg.f_CurrentChargeMoney);	
}


/********************************************************************************* 
 函数名称： PackTestCardData
 功能描述：打包测试卡界面显示信息
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void PackTestCardData(void)
{
	printf("test card data packed\n");
	printf("fPositiveTotalPower is %9.2f \n",MeterMsg.fPositiveTotalPower);
	printf("MeterMsg.fPositivePower2 is %9.2f \n",MeterMsg.fPositivePower2);
	printf("MeterMsg.fPositivePower3 is %9.2f \n",MeterMsg.fPositivePower3);
	printf("MeterMsg.fPositivePower4 is %9.2f \n",MeterMsg.fPositivePower4);
	sprintf((char*)DisplayMsg.Displaybuf,"%9.2f",MeterMsg.fPositiveTotalPower);	
	sprintf((char*)&DisplayMsg.Displaybuf[9],"%9.2f",MeterMsg.fPositivePower2);
	sprintf((char*)&DisplayMsg.Displaybuf[18],"%9.2f",MeterMsg.fPositivePower3);
	sprintf((char*)&DisplayMsg.Displaybuf[27],"%9.2f",MeterMsg.fPositivePower4);
	sprintf((char*)&DisplayMsg.Displaybuf[36],"%7.4f",HoutaiData.fRate2);
	sprintf((char*)&DisplayMsg.Displaybuf[43],"%7.4f",HoutaiData.fRate3);
	sprintf((char*)&DisplayMsg.Displaybuf[50],"%7.4f",HoutaiData.fRate4);
}

/********************************************************************************* 
 函数名称： PackLastChargeData
 功能描述：打包上次充电信息界面显示信息
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void PackLastChargeData_F(void)
{
	ST_INT32 temp;
	temp = BCDToInt(ChargeMsg.LastRecord.sEndEnerge)-BCDToInt(ChargeMsg.LastRecord.sStartEnerge);

	DisplayInfo.LastChargeEle = (float)(temp/100.00);

	printf("DisplayInfo.LastChargeEle is %7.2f\n",DisplayInfo.LastChargeEle);
	

	
	temp = BCDToInt(ChargeMsg.LastRecord.sMoneyBefore)-BCDToInt(ChargeMsg.LastRecord.sMoneyAfter);
	DisplayInfo.LastChargeMoney = (float)(temp/100.00);
	printf("DisplayInfo.LastChargeMoney is %7.2f\n",DisplayInfo.LastChargeMoney);
	printf("DisplayInfo.LastChargeEndTime is:");
	int i;
	for(i = 0; i<6;i++)
	{
		temp = CDZ_BCDTOHEX(ChargeMsg.LastRecord.sEndTime[i]);
		DisplayInfo.LastChargeEndTime[i] = temp;	
		printf("%d ",DisplayInfo.LastChargeEndTime[i]);	
	}
	DisplayInfo.LastChargeEndTime[5]+=2000;
	printf("\n");
	

}

void PackLastChargeData_P(void)
{
	ST_INT32 temp;
	temp = BCDToInt(ChargeMsg.LastRecord.sEndEnerge)-BCDToInt(ChargeMsg.LastRecord.sStartEnerge);
	printf("ChargeMsg.LastRecord.sEndEnerge is % d\n",BCDToInt(ChargeMsg.LastRecord.sEndEnerge));
	sprintf((char*)DisplayMsg.Displaybuf,"%9.2f",(float)temp/100);
	temp = BCDToInt(ChargeMsg.LastRecord.sEndEnerge3)-BCDToInt(ChargeMsg.LastRecord.sStartEnerge3);
	
	sprintf((char*)&DisplayMsg.Displaybuf[9],"%9.2f",(float)temp/100);
	
	sprintf((char*)&DisplayMsg.Displaybuf[18],"%7.4f",HoutaiData.fRate3);
	
	temp = BCDToInt(ChargeMsg.LastRecord.sMoneyBefore)-BCDToInt(ChargeMsg.LastRecord.sMoneyAfter);
	
	sprintf((char*)&DisplayMsg.Displaybuf[25],"%7.2f",(float)temp/100);
	temp = BCDToInt(ChargeMsg.LastRecord.sMoneyAfter);
	sprintf((char*)&DisplayMsg.Displaybuf[32],"%7.2f",(float)temp/100);

	
	temp =  CDZ_BCDTOHEX(ChargeMsg.LastRecord.sTimeLong[2]);
	sprintf((char*)&DisplayMsg.Displaybuf[39],"%4d",temp);
	
	temp =  CDZ_BCDTOHEX(ChargeMsg.LastRecord.sTimeLong[1]);
	sprintf((char*)&DisplayMsg.Displaybuf[43],"%2d",temp);
	
	temp =  CDZ_BCDTOHEX(ChargeMsg.LastRecord.sTimeLong[0]);
	sprintf((char*)&DisplayMsg.Displaybuf[45],"%2d",temp);		
}

void PackLastChargeData_G(void)
{
	ST_INT32 temp;
	temp = BCDToInt(ChargeMsg.LastRecord.sEndEnerge)-BCDToInt(ChargeMsg.LastRecord.sStartEnerge);
	printf("ChargeMsg.LastRecord.sEndEnerge is % d\n",BCDToInt(ChargeMsg.LastRecord.sEndEnerge));
	sprintf((char*)DisplayMsg.Displaybuf,"%9.2f",(float)temp/100);
	temp = BCDToInt(ChargeMsg.LastRecord.sEndEnerge4)-BCDToInt(ChargeMsg.LastRecord.sStartEnerge4);
	
	sprintf((char*)&DisplayMsg.Displaybuf[9],"%9.2f",(float)temp/100);
	
	sprintf((char*)&DisplayMsg.Displaybuf[18],"%7.4f",HoutaiData.fRate4);
	
	temp = BCDToInt(ChargeMsg.LastRecord.sMoneyBefore)-BCDToInt(ChargeMsg.LastRecord.sMoneyAfter);
	
	sprintf((char*)&DisplayMsg.Displaybuf[25],"%7.2f",(float)temp/100);
	temp = BCDToInt(ChargeMsg.LastRecord.sMoneyAfter);
	sprintf((char*)&DisplayMsg.Displaybuf[32],"%7.2f",(float)temp/100);

	
	temp =  CDZ_BCDTOHEX(ChargeMsg.LastRecord.sTimeLong[2]);
	sprintf((char*)&DisplayMsg.Displaybuf[39],"%4d",temp);
	
	temp =  CDZ_BCDTOHEX(ChargeMsg.LastRecord.sTimeLong[1]);
	sprintf((char*)&DisplayMsg.Displaybuf[43],"%2d",temp);
	
	temp =  CDZ_BCDTOHEX(ChargeMsg.LastRecord.sTimeLong[0]);
	sprintf((char*)&DisplayMsg.Displaybuf[45],"%2d",temp);	
}



/********************************************************************************* 
 函数名称： PackPrintData
 功能描述： 打包要打印的数据内容，多字节的先传低字节
 输    入：	PrintData：打印内容buffer
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.8.5
 修改记录：
*********************************************************************************/


ST_CHAR PrintDataLen = 1; //打印数据内容长度，默认值为1:表示查询打印机状态，当需要打印数据时，该变量为打印数据长度
ST_UCHAR PrintData[100]; //打印内容buffer

ST_UCHAR* PackPrintData(ST_UCHAR *PrintData)
{
//	ST_INT16 i;
	struct tm *stShowNowTime;
	time_t now;
	time(&now);
	stShowNowTime = localtime(&now);	
	
//	sprintf((char*)PrintData,"%X%X%X%X%X%X%X%X",ChargeMsg.ChargeCardSeria[0],ChargeMsg.ChargeCardSeria[1],
//	ChargeMsg.ChargeCardSeria[2],ChargeMsg.ChargeCardSeria[3],ChargeMsg.ChargeCardSeria[4],
//	ChargeMsg.ChargeCardSeria[5],ChargeMsg.ChargeCardSeria[6],ChargeMsg.ChargeCardSeria[7]);
	
	memcpy(PrintData,ChargeMsg.ChargeCardSeria,8);
	memcpy(&PrintData[8],ChargeRecord.sTermNo,5);
	PrintData[13] = (ST_UCHAR)(ChargeMsg.f_ChargeRate*100);
	
	PrintData[14] = (ST_UCHAR)((ST_INT16)(ChargeMsg.f_TotalChargeEle*100) & 0x00FF);
	PrintData[15] = ((ST_INT16)(ChargeMsg.f_TotalChargeEle*100) & 0xFF00)>>8;
	
	PrintData[16] = (ST_UCHAR)((ST_INT16)(ChargeMsg.f_CurrentChargeMoney*100) & 0x00FF);
	PrintData[17] = ((ST_INT16)(ChargeMsg.f_CurrentChargeMoney*100) & 0xFF00)>>8;
	
	memcpy(&PrintData[18],ChargeMsg.ChargeTmLong,3);
	printf("ChargeMsg.f_TotalChargeEle is %9.2f PrintData[14] is %d\n",ChargeMsg.f_TotalChargeEle,PrintData[14]);
	
	
	
	#if 0
//	sprintf((char*)&PrintData[8],"%d%d%d%d%d",ChargeRecord.sTermNo[0],ChargeRecord.sTermNo[1],ChargeRecord.sTermNo[2],ChargeRecord.sTermNo[3],ChargeRecord.sTermNo[4]);
	//sprintf((char*)&PrintData[13],"%4d",ChargeMsg.InerSeq);
	//sprintf((char*)&PrintData[17],"%7.4f",ChargeMsg.f_ChargeRate);
	//sprintf((char*)&PrintData[24],"%9.2f",ChargeMsg.f_TotalChargeEle);
	sprintf((char*)&PrintData[33],"%7.2f",ChargeMsg.f_CurrentChargeMoney);
	sprintf((char*)&PrintData[40],"%4d",ChargeMsg.ChargeTmLong[2]);	
	sprintf((char*)&PrintData[44],"%2d",ChargeMsg.ChargeTmLong[1]);	
	sprintf((char*)&PrintData[46],"%2d",ChargeMsg.ChargeTmLong[0]);	
	
	sprintf((char*)&PrintData[48],"%4d",stShowNowTime->tm_year+1900);
	sprintf((char*)&PrintData[52],"%2d",stShowNowTime->tm_mon+1);
	sprintf((char*)&PrintData[54],"%2d",stShowNowTime->tm_mday);
	sprintf((char*)&PrintData[56],"%2d",stShowNowTime->tm_hour);
	sprintf((char*)&PrintData[58],"%2d",stShowNowTime->tm_min);
	sprintf((char*)&PrintData[60],"%2d",stShowNowTime->tm_sec);
	PrintDataLen = 62;
	printf("stShowNowTime->tm_year is %d\n",stShowNowTime->tm_year);
	#endif
	PrintDataLen = 21;
	return PrintData;	
}

#if 0
ST_UCHAR* PackPrintData(ST_UCHAR *PrintData)
{
	ST_INT16 i;
	struct tm *stShowNowTime;
	time_t now;
	time(&now);
	stShowNowTime = localtime(&now);	
	
	sprintf((char*)PrintData,"%X%X%X%X%X%X%X%X",ChargeMsg.ChargeCardSeria[0],ChargeMsg.ChargeCardSeria[1],
	ChargeMsg.ChargeCardSeria[2],ChargeMsg.ChargeCardSeria[3],ChargeMsg.ChargeCardSeria[4],
	ChargeMsg.ChargeCardSeria[5],ChargeMsg.ChargeCardSeria[6],ChargeMsg.ChargeCardSeria[7]);
	
	sprintf((char*)&PrintData[8],"%d%d%d%d%d",ChargeRecord.sTermNo[0],ChargeRecord.sTermNo[1],ChargeRecord.sTermNo[2],ChargeRecord.sTermNo[3],ChargeRecord.sTermNo[4]);
	sprintf((char*)&PrintData[13],"%4d",ChargeMsg.InerSeq);
	sprintf((char*)&PrintData[17],"%7.4f",ChargeMsg.f_ChargeRate);
	sprintf((char*)&PrintData[24],"%9.2f",ChargeMsg.f_TotalChargeEle);
	sprintf((char*)&PrintData[33],"%7.2f",ChargeMsg.f_CurrentChargeMoney);
	sprintf((char*)&PrintData[40],"%4d",ChargeMsg.ChargeTmLong[2]);	
	sprintf((char*)&PrintData[44],"%2d",ChargeMsg.ChargeTmLong[1]);	
	sprintf((char*)&PrintData[46],"%2d",ChargeMsg.ChargeTmLong[0]);	
	
	sprintf((char*)&PrintData[48],"%4d",stShowNowTime->tm_year+1900);
	sprintf((char*)&PrintData[52],"%2d",stShowNowTime->tm_mon+1);
	sprintf((char*)&PrintData[54],"%2d",stShowNowTime->tm_mday);
	sprintf((char*)&PrintData[56],"%2d",stShowNowTime->tm_hour);
	sprintf((char*)&PrintData[58],"%2d",stShowNowTime->tm_min);
	sprintf((char*)&PrintData[60],"%2d",stShowNowTime->tm_sec);
	PrintDataLen = 62;
	printf("stShowNowTime->tm_year is %d\n",stShowNowTime->tm_year);
	return PrintData;	
}
#endif

void Print(void)
{
	PrintDataLen = 21;
}


/********************************************************************************* 
 函数名称： StartCharge
 功能描述： 开始充电
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void StartCharge(void)
{
	struct tm *stShowNowTime;
	time_t now;
	time(&now);
	stShowNowTime = localtime(&now);
	CardMsg.CardStatus = CARDLOCK;
	IdleLedOff();
//	ChargeLedOn();
	JieChuQiOn();
	
	memset(ChargeMsg.ChargeTmLong,0,3);
	memcpy(ChargeMsg.ChargeCardSeria,CardMsg.CardSeria,CARDSERIALEN);
	ChargeMsg.ChargeCardType = CardMsg.CardType;
	
	ChargeMsg.f_StartChargeEle =  MeterMsg.fPositiveTotalPower; 
	ChargeMsg.f_StartChargeEle1 = MeterMsg.fPositivePower1;
	ChargeMsg.f_StartChargeEle2 = MeterMsg.fPositivePower2;
	ChargeMsg.f_StartChargeEle3 = MeterMsg.fPositivePower3;
	ChargeMsg.f_StartChargeEle4 = MeterMsg.fPositivePower4;
	
	memcpy(ChargeMsg.StartChargeMoney,CardMsg.CardMoney,4);
	ChargeMsg.f_TotalChargeEle = 000000.00;          //总充电量
	ChargeMsg.f_ChargeEle1 = 000000.00;
	ChargeMsg.f_ChargeEle2 = 000000.00;
	ChargeMsg.f_ChargeEle3 = 000000.00;
	ChargeMsg.f_ChargeEle4 = 000000.00;
	ChargeMsg.f_StartChargeMoney = CardMsg.f_CardMoney; //开始充电金额
	ChargeMsg.f_CurrentBalance = CardMsg.f_CardMoney;	//当前余额
	ChargeMsg.f_EndChargeEle = 0.00;	              //结束充电量
	ChargeMsg.f_CurrentChargeMoney = 0000.00;
	ChargeMsg.f_EndChargeMoney = 0.00;
	
	DisplayInfo.ChargedEle = 0.00;
	DisplayInfo.ChargedMoney = 0.00;
	DisplayInfo.ChargingEle = 0.00;
	DisplayInfo.ChargingMoney = 0.00;
	DisplayInfo.LastChargeEle = 0.00;
	DisplayInfo.LastChargeMoney = 0.00;
	memset(DisplayInfo.LastChargeEndTime,0,sizeof(DisplayInfo.LastChargeEndTime));
	memset(DisplayInfo.ChargedTime,0,sizeof(DisplayInfo.ChargedTime));
	memset(DisplayInfo.ChargingTime,0,sizeof(DisplayInfo.ChargingTime));
	//DisplayInfo.ChargedTime = 0.00;
	//DisplayInfo.ChargingTime = 0.00;
	
	ChargeMsg.StartChargeTime.Year = (ST_UCHAR)(stShowNowTime->tm_year % 100);
	ChargeMsg.StartChargeTime.Month = (ST_UCHAR)(stShowNowTime->tm_mon+1);
	ChargeMsg.StartChargeTime.Day = (ST_UCHAR)(stShowNowTime->tm_mday);
	ChargeMsg.StartChargeTime.Hour = (ST_UCHAR)(stShowNowTime->tm_hour);
	ChargeMsg.StartChargeTime.Minite = (ST_UCHAR)(stShowNowTime->tm_min);
	ChargeMsg.StartChargeTime.Second = (ST_UCHAR)(stShowNowTime->tm_sec);
	ChargeMsg.PaleStatus = 	PALECHARGING;
	ChargeRecord.sExchangeType = 0; //交易类型为开始充电上传


	EventFlag.StartChargeFlag = 1;
	ST_UCHAR PaleStatusAlarm[2] = {0x00,0x00};

	InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 
	sleep(1);
	InitNewChargeRecord();
	printf("########################################startcharge InitNewCharge################################\n");
	//InitNewChargeRecord_Gprs();
	printf("ChargeMsg.palestatus is %d\n",ChargeMsg.PaleStatus);
}

/********************************************************************************* 
 函数名称： YuChongDian
 功能描述： 预充电，检测电流有无输出，有电流输出则插头已插好，启动充电；否则继续提示插头未接好，三次预充电后，
 			仍未连接好插头则返回主界面
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

//static char Opendoorflag = 0;      //打开门的标记，门打开置1


void YuChongDian(void)
{
	JieChuQiOn();
	sleep(8);
	printf("fcurA is %6.3f\n",MeterMsg.fCur_A);
	printf("fcurB is %6.3f\n",MeterMsg.fCur_B);
	printf("fcurC is %6.3f\n",MeterMsg.fCur_C);
	
	if((MeterMsg.fCur_A>0.5&&MeterMsg.fCur_A<16.000)||(MeterMsg.fCur_B>0.5&&MeterMsg.fCur_B<16.000)||(MeterMsg.fCur_C>0.5&&MeterMsg.fCur_C<16.000))
	{	
		if(ChargeMsg.PaleStatus != PALEEXCEPTION)
		{	
		if(CardMsg.CardType==CHARGECARD)
		{	
		//	int temp = 0;
			if(TxToCardStartChargeCmd())
			{
				DisplayMsg.CurrentPage = DISPNOTMOVE_CAR;
				PlaySound(NOMOVETHECAR,NULL);
				sleep(4);		
				CardMsg.CardStatus = CARDLOCK;			
				StartCharge();
				PlaySound(BEGINECHARGE,NULL);
				ChargeMsg.YuChongDianCount = 0;
				Tanka();
				printf("Charging!\n");
					
			}
		
			else
			{
				JieChuQiOff();
				ChargeMsg.YuChongDianCount = 0;
				DisplayMsg.CurrentPage = STARTCHARGEFAILED;
				sleep(3);
				DisplayMsg.CurrentPage = DISPIDLE;

				Tanka();

			}
		
		}
		if(CardMsg.CardType==MANAGECARD)
		{
			DisplayMsg.CurrentPage = DISPNOTMOVE_CAR;
			PlaySound(NOMOVETHECAR,NULL);
			sleep(4);					
			StartCharge();
			PlaySound(BEGINECHARGE,NULL);
			EventFlag.SoundTipFlag = 0;
			ChargeMsg.YuChongDianCount = 0;

			Tanka();
		}
		}
				
	}
	else
	{
		if(ChargeMsg.YuChongDianCount<2)
		{
			JieChuQiOff();
			OpenDoor();
			Opendoorflag = 0;
			ChargeMsg.YuChongDianCount++;
			if(CardMsg.CardType==CHARGECARD)
			{	
				PackCardMsg();                       //打包要显示的卡和桩信息 
				DisplayMsg.CurrentPage = DISPCLODEDOOR;	
				PlaySound(PUSHPLUG,NULL);
			}
		}
		else
		{
			ChargeMsg.YuChongDianCount = 0;
			DisplayMsg.CurrentPage = DISPIDLE;

			Tanka();
		}
	}

}

/********************************************************************************* 
 函数名称： BrashCardStartCharge
 功能描述： 桩在空闲状态下检测卡后，正常启动充电，或等待充电
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/

void BrashCardStartCharge(void)
{
	static ST_CHAR SendStartChargefg = 0;
	int i = 0;
		
	DisplayInfo.CardBalance = CardMsg.f_CardMoney;
	DisplayInfo.MaxChargeEle = (DisplayInfo.CardBalance-DEPOSIT)*ChargeMsg.f_ChargeRate;
	memcpy(DisplayInfo.CardNo,CardMsg.CardSeria,8);
	for(i =0;i<8;i++) DisplayInfo.CardNo[i]+=0x30;
	memcpy(ChargeMsg.ChargeCardSeria,CardMsg.CardSeria,CARDSERIALEN);
	EventFlag.IsPatCardFlag = 1;
	
	if(DisplayInfo.CardBalance > DEPOSIT)
	{
		if(SetChargePara.SetChargeMode == 1)
		{
			if((DisplayInfo.CardBalance - DEPOSIT)>SetChargePara.SetChargeMoney)
			{
				SendStartChargefg = 1;
			}
			
		}
		else if(SetChargePara.SetChargeMode == 2 )
		{
			if((DisplayInfo.CardBalance - DEPOSIT)>SetChargePara.SetChargeEle)
			{
				SendStartChargefg = 1;
			}
			
		}
			
		else
		{
			SendStartChargefg = 1;
		}
	}
	
	if(SendStartChargefg == 1)
	{

		printf("charge mode is 0\n");
		if(TxToCardStartChargeCmd())  //向读卡器发送开始加电命令正确 
		{
			CardMsg.CardStatus = CARDLOCK;
			EventFlag.SendStartChargeCmdFg = 1;
			if(SetChargePara.SetStartChargeMode==0)
			{
				StartCharge();
//				PlaySound(BEGINECHARGE,NULL);
				SendStartChargefg = 0;
				EventFlag.SendStartChargeCmdFg = 0;
				Tanka();
				
			}
			else
			{
				SendStartChargefg = 0;
				Tanka();
				EventFlag.StartChargeFlag = 3;  //进入预充电等待界面
				InitNewChargeRecord();
				printf("########################################BrashCardStartCharge InitNewChargeRecord################################\n");
				EventFlag.SendStartChargeCmdFg = 0;
				sleep(3);
				return;

						
			}
		}
                           
		else          //如果开始加电不成功，则显示的界面
		{										
			Tanka();
			EventFlag.SendStartChargeCmdFg = 0;
			printf("jia dian failed\n");
			return;
		}
		
	}
		
}



/********************************************************************************* 
 函数名称： PayLastChargeMoney
 功能描述： 桩在空闲状态下检测卡后，充电卡上次充电未付费，则扣除上次费用
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/

void PayLastChargeMoney(void)
{
	ST_FLOAT LastChargeMoney_bf = 0.00,LastChargeMoney_af = 0.00,temp;
	ST_INT32 PaymoneyRes = -1;
	static ST_CHAR SendPayCount;
	
	printf("**********************************paid last charge fee******************************\n");
	if(GetCardMsg())               //在本桩记录中得到卡信息，扣钱，解锁卡，更新读到的卡余额
	{
		dump_msg(&ChargeMsg.LastRecord);		
		PackLastChargeData_F();
		DisplayInfo.CardBalance = CardMsg.f_CardMoney - DisplayInfo.LastChargeMoney;
		DisplayInfo.MaxChargeEle = (DisplayInfo.CardBalance-DEPOSIT)*ChargeMsg.f_ChargeRate;
		printf("DisplayInfo.CardBalance is %7.2f\n",DisplayInfo.CardBalance);
		
		if(PaymoneyRes == -1)
		{
			PaymoneyRes = PayMoney(temp);			
		}		
		if(PaymoneyRes != -1)
		{
			if(PaymoneyRes == RES_TRUE)
			{
				ChargeMsg.LastRecord.sCardStatus = 	CARDUNLOCK;
				
				DisplayInfo.FindLastChargeRec = 1;            //进入扣除上次充电费用界面
				EventFlag.IsPatCardFlag = 1;

				if(insert_db(&ChargeMsg.LastRecord,"message")==RESULT_SQL_FAIL)
				{
					printf("insert error!\n");
				}
																														
				CardMsg.CardStatus = CARDUNLOCK;
				EventFlag.IsExistCardFlag = 0;          
				PaymoneyRes = -1;
				CardMsg.Sendcmd = FIND_CARD;	
			}
			else if(PaymoneyRes == RES_FAULSE||PaymoneyRes == LOCKCARD) //界面提示请再次刷卡付费08-11
			{
				if(SendPayCount<4)
				{
					CardMsg.Sendcmd = FIND_CARD;
					PaymoneyRes = -1;
					EventFlag.IsExistCardFlag = 0;
					SendPayCount++;
									
				}
				else                     //提示卡已锁，请到服务中心处理
				{
					Tanka();
					PaymoneyRes = -1;
					SendPayCount = 0;
					DisplayInfo.FindLastChargeRec = 2;
					EventFlag.IsPatCardFlag = 1;
					ChargeMsg.PaleStatus = PALEIDLE;
				}
			}						
		}							
	}
	else      //在本桩的充电记录中未找到该卡的消费记录，则提示用户该卡已锁，弹卡
	{
		printf("**********************************card lock******************************\n");
		DisplayInfo.FindLastChargeRec = 2;
		EventFlag.IsPatCardFlag = 1;
//		PlaySound(CARDLOCKED,NULL);
		Tanka();
		return;
	}	
	
}

void PayInEndChargeStatus(void)
{
	static ST_INT32 PaymoneyRes = -1;
	static ST_CHAR SendPayCount;

	if(CardMsg.CardStatus==CARDUNLOCK)
	{
		PaymoneyRes = RES_TRUE;
	}
	else if(CardMsg.CardStatus==CARDLOCK)
	{
		if(PaymoneyRes == -1)
		{
			PaymoneyRes = PayMoney(ChargeMsg.f_CurrentChargeMoney);			
		}
	}	
	if(PaymoneyRes != -1)
	{
		if(PaymoneyRes == RES_TRUE)
		{
			if(EventFlag.IsPatCardFlag == 0 )
			{
				EventFlag.IsPatCardFlag = 1;
				CardMsg.CardStatus = CARDUNLOCK;
				EventFlag.ChargeFullFlag = 0;
				EventFlag.ChargeExcepEndFlag = 0;
				EventFlag.ChargeCompleteFlag = 0;
				ChargeFullLedOff();
				if( PALEIDLE == ChargeMsg.PaleStatus)
				{
					//IdleLedOn();
				}			
				ChargeMsg.PaleStatus = PALEIDLE;  //这里桩故障或者正常，都视为正常。
				PaymoneyRes = -1;
				InitNewChargeRecord();
				printf("########################################PayInEndChargeStatus InitNewChargeRecord################################\n");
									
				ST_CHAR CurrentPaid[7],CurrentBalance[7];
				sprintf(CurrentPaid,"%7.2f",ChargeMsg.f_CurrentChargeMoney);
				sprintf(CurrentBalance,"%7.2f",ChargeMsg.f_CurrentBalance);
				  			
			}	

			Tanka();
		}
		else if(PaymoneyRes == RES_FAULSE||PaymoneyRes == LOCKCARD) //界面提示请再次刷卡付费08-11
		{
			if(SendPayCount<4)
			{
				CardMsg.Sendcmd = FIND_CARD;
				PaymoneyRes = -1;
				EventFlag.IsExistCardFlag = 0;
				SendPayCount++;
				
								
			}
			else                     //界面提示本次付费失败，充电卡未解锁，请到服务中心处理
			{
				Tanka();
				PaymoneyRes = -1;
				//EventFlag.IsPatCardFlag = 0;//这个应在显示模块中清零。
				SendPayCount = 0;
				EventFlag.ChargeExcepEndFlag = 0;
				EventFlag.ChargeCompleteFlag = 0;
				EventFlag.ChargeFullFlag = 0; 
				ChargeMsg.PaleStatus = PALEIDLE;
			}

		}			
	}
		

}

/********************************************************************************* 
 函数名称： CheckBalance
 功能描述： 用户选择查询余额处理
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/
void CheckBalance(void)
{
	DisplayInfo.CardBalance = CardMsg.f_CardMoney;
	DisplayInfo.MaxChargeEle = (DisplayInfo.CardBalance-DEPOSIT)*ChargeMsg.f_ChargeRate;
	EventFlag.IsPatCardFlag = 1; //显示当前余额界面
	Tanka();
}


/********************************************************************************* 
 函数名称： PreChargeWaitFindCard
 功能描述： 充电桩在充电等待界面检测到充电卡。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/

void PreChargeWaitFindCard(void)
{
	//ST_FLOAT LastChargeMoney_bf = 0.00,LastChargeMoney_af = 0.00;
	static ST_UCHAR SendPayCount = 0;

	static ST_INT32 PaymoneyRes=-1;
	
	if(CardMsg.CardStatus == CARDUNLOCK)
	{
		PaymoneyRes = RES_TRUE;
	}
	else if(CardMsg.CardStatus == CARDLOCK)
	{

		if(PaymoneyRes==-1)
		{
			PaymoneyRes = PayMoney(0.00);	
			SendPayCount++;
			printf("PaymoneyRes is %d SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n",PaymoneyRes);
			
		}
	}				
	if(PaymoneyRes!=-1)
	{
		if(PaymoneyRes == RES_TRUE)
		{						
			CardMsg.CardStatus = CARDUNLOCK;
			if(EventFlag.IsPatCardFlag == 0 )
			{
				EventFlag.IsPatCardFlag = 1;																	
			}
			InitNewChargeRecord();
			printf("############################################PreChargeWaitFindCard chargerecord#######################3\n");
			PaymoneyRes = -1;
			Tanka();	
		}
		else if(PaymoneyRes == RES_FAULSE||PaymoneyRes == LOCKCARD) //界面提示请再次刷卡付费08-11
		{
			if(SendPayCount<4)
			{
				CardMsg.Sendcmd = FIND_CARD;
				PaymoneyRes = -1;
				EventFlag.IsExistCardFlag = 0;
								
			}
			else                     //界面提示本次付费失败，充电卡未解锁，请到服务中心处理
			{
				Tanka();
				PaymoneyRes = -1;
				SendPayCount = 0;
			}

		}

	}

}

/********************************************************************************* 
 函数名称： PaleIdleDealChargeCard
 功能描述： 桩在空闲状态下检测到充电卡的处理过程
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/

void PaleIdleDealChargeCard(void)
{
//	ST_INT32 i;
	
	switch(DisplayInfo.CurrentPage)
	{

		case DISP_BRUSHCARD:
		case DISP_BRUSHCARD_EXIT:
		{
			if(CardMsg.CardStatus==CARDUNLOCK)
			{
				if(SetChargePara.SetChargeMode==5)
				{
					CheckBalance();
				}
				else
				{
					printf("come to brashcard\n");
					BrashCardStartCharge();
				}
			}	
			else if( CardMsg.CardStatus==CARDREPAY && DisplayInfo.FindLastChargeRec==2)		
			{
				printf("********************************** card locked11*****************************\n");
				DisplayInfo.CardBalance = CardMsg.f_CardMoney;
				EventFlag.IsPatCardFlag = 1;
				Tanka();				
			}	
			else if(EventFlag.StartChargeFlag==0)
			{
				PayLastChargeMoney();
			}				
		}
		break;
		case DISP_WAIT_CHARGE:
		{
			if(!memcmp((ST_CHAR *)CardMsg.CardSeria,(ST_CHAR *)ChargeMsg.ChargeCardSeria,sizeof(CardMsg.CardSeria)))     //同一张充电卡			
			{
				PreChargeWaitFindCard();
											
			}
			else
			{
				EventFlag.NoSameCardFlag = 1; //显示非同一张卡
				printf("DISP_WAIT_CHARGE,EventFlag.IsExistCardFlag is %d\n",EventFlag.IsExistCardFlag);

//				PlaySound(NOTSAMECARD,NULL);		
				sleep(3);
				Tanka();	
			}									
		}
		break;
		case DISP_NOMONEY_CHARING_ELE:
		case DISP_NOMONEY_CHARGING_MON: 
		case DISP_NOMONEY_CHARGING_TIME:  
		case DISP_NOMONEY_CHARGING_ZIDONG:
		case DISP_FINISHCHARGE_ELE:
		case DISP_FINISHCHARGE_MON:
		case DISP_FINISHCHARGE_TIME:
		case DISP_FINISH_ZIDONG:
		case DISP_CHARGE_FULL_ELE:  
		case DISP_CHARGE_FULL_MON:  
		case DISP_CHARGE_FULL_TIME: 

		{
			if(!memcmp((ST_CHAR *)CardMsg.CardSeria,(ST_CHAR *)ChargeMsg.ChargeCardSeria,sizeof(CardMsg.CardSeria)))     //同一张充电卡			
			{
				PayInEndChargeStatus();
							
			}
			else
			{
				EventFlag.NoSameCardFlag = 1; //显示非同一张卡

//				PlaySound(NOTSAMECARD,NULL);		
				//sleep(3);
				Tanka();	
			}
				
		}
		break;

		default: Tanka();
		break;				
	}
	return;
}

/********************************************************************************* 
 函数名称： SaveChargeRecData
 功能描述： 打包充电记录数据
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void SaveChargeRecData(ChargeRecordTP *ChargeRecord)
{ 
	ST_UCHAR i;
	ST_UINT32 temp;
	ChargeRecord->SendHoutaiFlag = 0x00;
	ChargeMsg.InerSeq++;
	temp = ChargeMsg.InerSeq++;
	for(i=0;i<5;i++)
	{
		ChargeRecord->sExchangeSeq[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	memcpy(ChargeRecord->sAreaCode,HoutaiData.AreaCode,3);
	memcpy(ChargeRecord->sStartCardNo,ChargeMsg.ChargeCardSeria,CARDSERIALEN);
	memcpy(ChargeRecord->sEndCardNo,CardMsg.CardSeria,CARDSERIALEN);

	ChargeRecord->sStartCardType[0] = ChargeMsg.ChargeCardType;
	ChargeRecord->sStartCardType[1] =  0;
	ChargeRecord->sEndCardType[0] = CardMsg.CardType;
	ChargeRecord->sEndCardType[1] =  0;
	temp = ChargeMsg.f_StartChargeEle*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sStartEnerge[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp = ChargeMsg.f_StartChargeEle1*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sStartEnerge1[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp = ChargeMsg.f_StartChargeEle2*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sStartEnerge2[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp = ChargeMsg.f_StartChargeEle3*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sStartEnerge3[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp = ChargeMsg.f_StartChargeEle4*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sStartEnerge4[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	
	
	temp = ChargeMsg.f_EndChargeEle*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sEndEnerge[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp =  ChargeMsg.f_EndChargeEle1*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sEndEnerge1[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp =  ChargeMsg.f_EndChargeEle2*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sEndEnerge2[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp =  ChargeMsg.f_EndChargeEle3*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sEndEnerge3[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp =  ChargeMsg.f_EndChargeEle4*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sEndEnerge4[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	for(i=0;i<3;i++)
	{
		ChargeRecord->sPrice1[i]= HoutaiData.Rate1[i];
		ChargeRecord->sPrice2[i]= HoutaiData.Rate2[i];
		ChargeRecord->sPrice3[i]= HoutaiData.Rate3[i];
		ChargeRecord->sPrice4[i]= HoutaiData.Rate4[i];
		ChargeRecord->sParkFeePrice[i] = HoutaiData.StopCarFeePrice[i];
	}
	ChargeRecord->sStartTime[5] = CDZ_HEXTOBCD(ChargeMsg.StartChargeTime.Year);
	ChargeRecord->sStartTime[4] = CDZ_HEXTOBCD(ChargeMsg.StartChargeTime.Month);
	ChargeRecord->sStartTime[3] = CDZ_HEXTOBCD(ChargeMsg.StartChargeTime.Day);
	ChargeRecord->sStartTime[2] = CDZ_HEXTOBCD(ChargeMsg.StartChargeTime.Hour);
	ChargeRecord->sStartTime[1] = CDZ_HEXTOBCD(ChargeMsg.StartChargeTime.Minite);
	ChargeRecord->sStartTime[0] = CDZ_HEXTOBCD(ChargeMsg.StartChargeTime.Second);
	
	ChargeRecord->sEndTime[5] = CDZ_HEXTOBCD(ChargeMsg.EndChargeTime.Year);
	ChargeRecord->sEndTime[4] = CDZ_HEXTOBCD(ChargeMsg.EndChargeTime.Month);
	ChargeRecord->sEndTime[3] = CDZ_HEXTOBCD(ChargeMsg.EndChargeTime.Day);
	ChargeRecord->sEndTime[2] = CDZ_HEXTOBCD(ChargeMsg.EndChargeTime.Hour);
	ChargeRecord->sEndTime[1] = CDZ_HEXTOBCD(ChargeMsg.EndChargeTime.Minite);
	ChargeRecord->sEndTime[0] = CDZ_HEXTOBCD(ChargeMsg.EndChargeTime.Second);
	memset(ChargeRecord->sParkFee,0,3);
	memset(ChargeRecord->sGasPrice,0,3);
	memset(ChargeRecord->sGasFee,0,4);
	
	temp = ChargeMsg.f_StartChargeMoney*100;
	for(i=0;i<4;i++)
	{
		ChargeRecord->sMoneyBefore[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp =ChargeMsg.f_CurrentBalance*100;
	for(i = 0;i<4;i++)
	{
		ChargeRecord->sMoneyAfter[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}
	temp = CardMsg.iDealCounts;
	for(i=0;i<5;i++)
	{
		ChargeRecord->sCardCounter[i] = CDZ_HEXTOBCD(temp%100);
		temp = temp/100;
	}

	memcpy(ChargeRecord->sTermNo,HoutaiData.ZhongduanNum,5);	
	memset(ChargeRecord->sCardVer,0,5);
	memset(ChargeRecord->sPosNo,0,5);
	ChargeRecord->sCardStatus = CDZ_HEXTOBCD(CardMsg.CardStatus);
	ChargeRecord->sTimeLong[0] = CDZ_HEXTOBCD(ChargeMsg.ChargeTmLong[0]);	
	ChargeRecord->sTimeLong[1] = CDZ_HEXTOBCD(ChargeMsg.ChargeTmLong[1]);
	ChargeRecord->sTimeLong[2] = CDZ_HEXTOBCD(ChargeMsg.ChargeTmLong[2]);		
	
	if (ChargeMsg.InerSeq > 200000)
	{
		ChargeMsg.InerSeq = 0;
	}

	if(insert_db(ChargeRecord,"message")==RESULT_SQL_FAIL)
	{
		printf("insert error!\n");
	}

}


/********************************************************************************* 
 函数名称： EndCharge
 功能描述： 结束充电，计算充电费用
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void EndCharge(void)
{
	struct tm *stShowNowTime;
	time_t now;
	time(&now);
	stShowNowTime = localtime(&now);	
	JieChuQiOff();
	ChargeLedOff();
	EventFlag.StartChargeFlag = 0;	
	ChargeMsg.f_EndChargeEle = MeterMsg.fPositiveTotalPower;
	ChargeMsg.f_EndChargeEle1 = MeterMsg.fPositivePower1;
	ChargeMsg.f_EndChargeEle2= MeterMsg.fPositivePower2;
	ChargeMsg.f_EndChargeEle3= MeterMsg.fPositivePower3;
	ChargeMsg.f_EndChargeEle4 = MeterMsg.fPositivePower4;
	ChargeMsg.f_TotalChargeEle = ChargeMsg.f_EndChargeEle-ChargeMsg.f_StartChargeEle;
	ChargeMsg.f_ChargeEle1 = ChargeMsg.f_EndChargeEle1-ChargeMsg.f_StartChargeEle1;
	ChargeMsg.f_ChargeEle2 = ChargeMsg.f_EndChargeEle2-ChargeMsg.f_StartChargeEle2;
	ChargeMsg.f_ChargeEle3 = ChargeMsg.f_EndChargeEle3-ChargeMsg.f_StartChargeEle3;
	ChargeMsg.f_ChargeEle4 = ChargeMsg.f_EndChargeEle4-ChargeMsg.f_StartChargeEle4;
	FloatToHex4(ChargeMsg.f_TotalChargeEle,CardMsg.ChargeEle);
	if(!memcmp((ST_CHAR *)CardMsg.CardMoney,(ST_CHAR *)ChargeMsg.StartChargeMoney,sizeof(CardMsg.CardMoney)))
	{
		ChargeMsg.f_CurrentBalance = ChargeMsg.f_StartChargeMoney-ChargeMsg.f_CurrentChargeMoney;
			
	}
	else
	{
		memcpy(ChargeMsg.StartChargeMoney,CardMsg.CardMoney,4);
		Hex4ToFloat(ChargeMsg.StartChargeMoney,&ChargeMsg.f_StartChargeMoney,1);
		ChargeMsg.f_CurrentBalance = ChargeMsg.f_StartChargeMoney-ChargeMsg.f_CurrentChargeMoney;	
	}	
	ChargeMsg.EndChargeTime.Year = (ST_UCHAR)(stShowNowTime->tm_year % 100);
	ChargeMsg.EndChargeTime.Month = (ST_UCHAR)(stShowNowTime->tm_mon+1);
	ChargeMsg.EndChargeTime.Day = (ST_UCHAR)(stShowNowTime->tm_mday);
	ChargeMsg.EndChargeTime.Hour = (ST_UCHAR)(stShowNowTime->tm_hour);
	ChargeMsg.EndChargeTime.Minite = (ST_UCHAR)(stShowNowTime->tm_min);
	ChargeMsg.EndChargeTime.Second = (ST_UCHAR)(stShowNowTime->tm_sec);
	
	if(EventFlag.CutdownEleFlag==1)
	{
		ChargeRecord.sExchangeType = 2; //交易类型为突然断电结束充电上传
	}
	else
	{
		ChargeRecord.sExchangeType = 1; //交易类型为结束充电上传		
	}

	if(ChargeMsg.PaleStatus==PALEEXCEPTION)  //充电桩故障结束充电
	{
						
		ST_UCHAR PaleStatusAlarm[2] = {0x02,0x00};
		InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 
		InitNewChargeRecord();
		printf("########################################endcharge InitNewCharge################################\n");
		if(1 == EventFlag.CardExcepFlag)
		{
			EventFlag.ChargeExcepEndFlag|=0x01;
		}
		if(EventFlag.MeterExcepFlag!=0)
		{
			EventFlag.ChargeExcepEndFlag|=0x02;
			
		}
		if(1 == EventFlag.CutdownEleFlag)
		{
			EventFlag.ChargeExcepEndFlag|=0x04;
		}
		if(1 == EventFlag.RepairDoorOpenFg)
		{
			EventFlag.ChargeExcepEndFlag|=0x08;
		}
		if(1 == EventFlag.ScreenExcepFlag)
		{
			EventFlag.ChargeExcepEndFlag|=0x10;
		}
		if(0 == EventFlag.QiangConGood)
		{
			EventFlag.ChargeExcepEndFlag|=0x20;
		}
		//sleep(1);		
	}

}

/********************************************************************************* 
 函数名称： SameChargeCardEndCharge
 功能描述： 充电桩正在充电过程中检测到正在充电的卡处理。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/

void SameChargeCardEndCharge(void)
{
//	ST_FLOAT LastChargeMoney_bf = 0.00,LastChargeMoney_af = 0.00,temp = 0.00;
	static ST_UCHAR SendPayCount = 0;

	static ST_INT32 PaymoneyRes=-1;
	
	if(CardMsg.CardStatus == CARDUNLOCK)
	{
		PaymoneyRes = RES_TRUE;
	}
	else if(CardMsg.CardStatus == CARDLOCK)
	{

		if(PaymoneyRes==-1)
		{
			PaymoneyRes = PayMoney(ChargeMsg.f_CurrentChargeMoney);	
			SendPayCount++;
			printf("PaymoneyRes is %d SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n",PaymoneyRes);
			
		}
	}				
	if(PaymoneyRes!=-1)
	{
		if(PaymoneyRes == RES_TRUE)
		{
//			PlaySound(ENDCHARGING,NULL);
//			sleep(1);	
			CardMsg.CardStatus = CARDUNLOCK;
			EndCharge();
			//IdleLedOn();
			printf("#######end charge\n");
			
			if(EventFlag.IsPatCardFlag == 0 )
			{
				EventFlag.IsPatCardFlag = 1;
													
				ST_CHAR CurrentPaid[7],CurrentBalance[7];
				sprintf(CurrentPaid,"%7.2f",ChargeMsg.f_CurrentChargeMoney);
				sprintf(CurrentBalance,"%7.2f",ChargeMsg.f_CurrentBalance);
				printf("###############W_FinishFullEle main EventFlag.IsPatCardFlag %d \n",EventFlag.IsPatCardFlag);
				
				//PlaySound_Money(CURRENTPAID,CurrentPaid);
				//PlaySound_Money(CURRENTBANLANCE,CurrentBalance);	
			}
			PaymoneyRes = -1;

			ChargeMsg.PaleStatus = PALEIDLE;
			Tanka();	
		}
		else if(PaymoneyRes == RES_FAULSE||PaymoneyRes == LOCKCARD) //界面提示请再次刷卡付费08-11
		{
			if(SendPayCount<4)
			{
				CardMsg.Sendcmd = FIND_CARD;
				PaymoneyRes = -1;
				EventFlag.IsExistCardFlag = 0;
								
			}
			else                     //界面提示本次付费失败，充电卡未解锁，请到服务中心处理
			{
//				PlaySound(ENDCHARGING,NULL);
				//sleep(1);	
				EndCharge();
				//IdleLedOn();

				printf("###########endcharge ok\n");
				Tanka();
				PaymoneyRes = -1;
				//EventFlag.IsPatCardFlag = 0;//这个应在显示模块中清零。
				SendPayCount = 0;
				ChargeMsg.PaleStatus = PALEIDLE;
			}

		}

	}

}

/********************************************************************************* 
 函数名称： PaleChargingDealChargeCard
 功能描述： 充电桩在正在充电过程中检测到充电卡的处理过程。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.20
 修改记录：
*********************************************************************************/

void PaleChargingDealChargeCard(void)
{	
	if(!memcmp((ST_CHAR *)CardMsg.CardSeria,(ST_CHAR *)ChargeMsg.ChargeCardSeria,sizeof(CardMsg.CardSeria)))     //同一张充电卡
	{	
		//printf("come to same card end charge\n");	
		
		SameChargeCardEndCharge();
	}
	else
	{
		EventFlag.NoSameCardFlag = 1;         //显示不是同一张卡
//		PlaySound(NOTSAMECARD,NULL);		
		Tanka();		
	}	
	
}


/********************************************************************************* 
 函数名称： Charging
 功能描述： 正在充电，计算充电费用，充电过程中检测到余额不足，充满，突然断电的情况均结束充电。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void Charging(void)
{
	//ST_CHAR ChargeFullCondition;
	printf("EventFlag.ChargeCompleteFlag is %d\n",EventFlag.ChargeCompleteFlag);
	printf("SetChargeMode is %d SetChargeEle is %8.2f ChargedEle is %8.2f\n",SetChargePara.SetChargeMode,SetChargePara.SetChargeEle,DisplayInfo.ChargedEle);
	#if 1
	if(MeterMsg.fCur_A<HoutaiData.fChargeCurLimit&&MeterMsg.fCur_B<HoutaiData.fChargeCurLimit&&MeterMsg.fCur_C<HoutaiData.fChargeCurLimit)//电流阀值
	{
		EventFlag.ChargeFullCountFlag = 1;
	//	ChargeFullLedOn();
		
		if(ChargeMsg.ChargeFullCount>=30)
		{
			ChargeMsg.ChargeFullCount = 0;
			EventFlag.ChargeFullCountFlag = 0;
     
		//	PlaySound(AUDIO_CHARGECOMPLETE,NULL);		      	
			EndCharge();
			ChargeMsg.PaleStatus = PALEIDLE;
			ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
			InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息  
			EventFlag.ChargeFullFlag = 1;
		}
	}
	else if((SetChargePara.SetChargeMode==1&&fabs(SetChargePara.SetChargeEle-DisplayInfo.ChargedEle)<0.005)
			||(SetChargePara.SetChargeMode==2&&fabs(SetChargePara.SetChargeMoney-DisplayInfo.ChargedMoney)<0.005)		
			||(SetChargePara.SetChargeMode==3&&!(memcmp(SetChargePara.SetChargeTime,ChargeMsg.ChargeTmLong,3)))
			)
	{
		//PlaySound(AUDIO_CHARGECOMPLETE,NULL);
		printf("****************************************charge complete*****************************\n");
		//ChargeFullLedOn();
		EndCharge();		
		ChargeMsg.PaleStatus = PALEIDLE;
		ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
		InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息  
		EventFlag.ChargeCompleteFlag = 1;
		
	}
	else
	{
		
		ChargeMsg.ChargeFullCount = 0;
		ChargeMsg.f_CurrentChargeEle = MeterMsg.fPositiveTotalPower;
	
		ChargeMsg.f_TotalChargeEle = ChargeMsg.f_CurrentChargeEle-ChargeMsg.f_StartChargeEle;
		ChargeMsg.f_ChargeEle2 = MeterMsg.fPositivePower2-ChargeMsg.f_StartChargeEle2;
		ChargeMsg.f_ChargeEle3 = MeterMsg.fPositivePower3-ChargeMsg.f_StartChargeEle3;
		ChargeMsg.f_ChargeEle4 = MeterMsg.fPositivePower4-ChargeMsg.f_StartChargeEle4;
	
		ChargeMsg.f_CurrentChargeMoney = ChargeMsg.f_TotalChargeEle*ChargeMsg.f_ChargeRate;	
	
		ChargeMsg.f_CurrentBalance = ChargeMsg.f_StartChargeMoney-ChargeMsg.f_CurrentChargeMoney; 
		ChargeMsg.f_EndChargeMoney = ChargeMsg.f_CurrentBalance;//结束金额
		
		DisplayInfo.ChargedEle = ChargeMsg.f_TotalChargeEle;
		DisplayInfo.ChargedMoney = ChargeMsg.f_CurrentChargeMoney;
		DisplayInfo.CardBalance = ChargeMsg.f_CurrentBalance;
		
		if( SetChargePara.SetChargeMode == 1 )
		{ 
			DisplayInfo.ChargingEle = SetChargePara.SetChargeEle - DisplayInfo.ChargedEle;
		}
		else if( SetChargePara.SetChargeMode == 2 )
		{
			DisplayInfo.ChargingMoney = SetChargePara.SetChargeMoney - DisplayInfo.ChargedMoney;
						
		}				
	}
	
	if(ChargeMsg.ChargeCardType==CHARGECARD)
    {
        if(ChargeMsg.f_CurrentBalance<DEPOSIT)        //余额不足
        {
          //  PlaySound(NOMONEYSTOPCHARGE,NULL);
			//IdleLedOn();
            EndCharge();
            ChargeMsg.PaleStatus = PALEIDLE;
      		ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
    		InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息
            EventFlag.ChargeNomoneyNoPulgFg = 1;        
        }
    }
    #endif

/*
	if(EventFlag.IsQiangConGood==0)
	{       
  			PlaySound(AUDIO_QIANUNCONNECT_CHARGING,NULL);
 			EndCharge();
      		zz;
  			ST_CHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
			InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息
      		EventFlag.ChargeNomoneyNoPulgFg = 3;           //充电插头已断开连接
	}
*/
	
}


/********************************************************************************* 
 函数名称： PaleChargingDealAllCard
 功能描述： 充电桩在正在充电过程中检测到所有卡的处理过程。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


void PaleChargingDealAllCard(void)
{
	if(EventFlag.IsExistCardFlag==1)
	{
		if(CardMsg.IsSystemCard)
		{
			switch(CardMsg.CardType)
			{
				case CHARGECARD:
				{
					//printf("come to deal chargecard\n");
					PaleChargingDealChargeCard();
				}
				break;
				case TESTCARD:
				{
					printf("find test card in charging\n");
					PaleChargingDealTestCard();
				}
				break;
				case MANAGECARD:
				{
					printf("find manage card in charging\n");
					PaleChargingDealManageCard();
				}
				break;
				case REPAIRCARD:
				{
					printf("find repair card in charging\n");
					PaleChargingDealRepairCard();
				}
				break;
				case MODIFYKEYCARD:
				{
					printf("find keymodify card in charging\n");
					PaleChargingDealKeyModifyCard();
				}
				break;
				case SETPARACARD:
				{
					
				}
				break;
				default:break;
			}
		}

	}
	else
	{
			
		Charging();
	}	
}







void PageThank_Return(void)
{
	#if 0
	if(ChargeMsg.PaleStatus==PALEIDLE)
	{
		DisplayMsg.CurrentPage = DISPIDLE;
	}
	else if(EventFlag.CutdownEleFlag==1)
	{
		DisplayMsg.CurrentPage = DISPDUANDIAN_IDLE;
		PlaySound(AUDIO_DUANDIAN_IDLE,NULL);
	}
	else if(EventFlag.RepairDoorOpenFg==1)
	{
		DisplayMsg.CurrentPage = REPAIRDOOROEPN_IDLE;
		PlaySound(REPAIRDOOROPENIDLE,NULL);
	}
	else if(EventFlag.MeterExcepFlag==1)
	{
		DisplayMsg.CurrentPage = METER_EXCEPTION_IDLE;
		PlaySound(METERCOMEXCEP,NULL);
	}
	else if(EventFlag.MeterExcepFlag==2)
	{
	    DisplayMsg.CurrentPage = MeterExcep_Disp_Idle();
	    printf(" MeterExcep_Audio 5\n");
		MeterExcep_Audio();
	}	
	else if(EventFlag.CardExcepFlag==1)
	{
		DisplayMsg.CurrentPage = CARD_EXCEPTION_IDLE;
		PlaySound(CARDCOMEXCEP,NULL);
	}
	#endif
		
			
}





/********************************************************************************* 
 函数名称： PaleExcepFindCard
 功能描述： 桩在异常状态下检测到卡的处理，卡解锁弹卡，卡未解锁且是当前充电卡，扣钱，卡未解锁且不是当前充电卡，查找记录，找到扣钱，没找到弹卡
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PaleExcepFindCard(void)
{
	if(EventFlag.IsExistCardFlag==1)
	{
		if(CardMsg.IsSystemCard)
		{
			printf("come to PaleExceptDealAllCard EventFlag.IsExistCardFlag is 1");
			if(EventFlag.ChargeExcepEndFlag!=0||EventFlag.ChargeNomoneyNoPulgFg==1||EventFlag.ChargeCompleteFlag==1||EventFlag.ChargeFullFlag == 1)
			{
				if(!memcmp((ST_CHAR *)CardMsg.CardSeria,(ST_CHAR *)ChargeMsg.ChargeCardSeria,sizeof(CardMsg.CardSeria)))
				{
					PayInEndChargeStatus();
				}
				else
				{
					EventFlag.NoSameCardFlag = 1; //显示非同一张卡
			//		PlaySound(NOTSAMECARD,NULL);		
			//		sleep(3);
					Tanka();
				}
			}		
			else
			{
				
		//		PlaySound(AUDIO_EXCEP_IDLE,NULL);
		//		sleep(3);
				Tanka();
			}		
			
		}
		
	}

}


void MeterExcep_Audio_Idle(void)
{
	//ST_UCHAR DisplayPage;
	if((DefalutPhaseFlag&0x0001)==1)
	{
		//DisplayPage = DISPMETER_L_VOL_A_IDLE;
		PlaySound(AUDIO_METER_L_VOL_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0002)==0x0002)
	{
		//DisplayPage = DISPMETER_L_VOL_B_IDLE;
		PlaySound(AUDIO_METER_L_VOL_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0004)==0x0004)
	{
		//DisplayPage = DISPMETER_L_VOL_C_IDLE;
		PlaySound(AUDIO_METER_L_VOL_C_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0008)==0x0008)
	{
		//DisplayPage = DISPMETER_H_VOL_A_IDLE;
		PlaySound(AUDIO_METER_H_VOL_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0010)==0x0010)
	{
		//DisplayPage = DISPMETER_H_VOL_B_IDLE;
		PlaySound(AUDIO_METER_H_VOL_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0020)==0x0020)
	{
		//DisplayPage = DISPMETER_H_VOL_C_IDLE;
		PlaySound(AUDIO_METER_H_VOL_C_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0040)==0x0040)
	{
		//DisplayPage = DISPMETER_H_CUR_A_IDLE;
		PlaySound(AUDIO_METER_H_CUR_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0080)==0x0080)
	{
		//DisplayPage = DISPMETER_H_CUR_B_IDLE;
		PlaySound(AUDIO_METER_H_CUR_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0100)==0x0100)
	{
		//DisplayPage = DISPMETER_H_CUR_C_IDLE;
		PlaySound(AUDIO_METER_H_CUR_C_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0200)==0x0200)
	{
		//DisplayPage = DISPMETER_DUANXIANG_A_IDLE;
		PlaySound(AUDIO_METER_DUANXIANG_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0400)==0x0400)
	{
		//DisplayPage = DISPMETER_DUANXIANG_B_IDLE;
		PlaySound(AUDIO_METER_DUANXIANG_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0800)==0x0800)
	{
		//DisplayPage = DISPMETER_DUANXIANG_C_IDLE;
		PlaySound(AUDIO_METER_DUANXIANG_C_IDLE,NULL);
	}	
	else if((DefalutPhaseFlag&0x1000)==0x1000)
	{
		//DisplayPage = DISPMETER_DUANLU_A_IDLE;
		PlaySound(AUDIO_METER_DUANLU_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x2000)==0x2000)
	{
		//DisplayPage = DISPMETER_DUANLU_B_IDLE;
		PlaySound(AUDIO_METER_DUANLU_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x4000)==0x4000)
	{
		//DisplayPage = DISPMETER_DUANLU_C_IDLE;
		PlaySound(AUDIO_METER_DUANLU_C_IDLE,NULL);
	}	
//	return DisplayPage;
}


ST_UCHAR MeterExcep_Disp_EndCharge(void)
{	
	if((DefalutPhaseFlag&0x0001)==1)
	{
		DisplayMsg.CurrentPage = DISPMETER_L_VOL_A_CHARGING_F;
		//PlaySound(AUDIO_METER_L_VOL_A_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0002)==0x0002)
	{
		DisplayMsg.CurrentPage = DISPMETER_L_VOL_B_CHARGING_F;
		//PlaySound(AUDIO_METER_L_VOL_B_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0004)==0x0004)
	{
		DisplayMsg.CurrentPage = DISPMETER_L_VOL_C_CHARGING_F;
		//PlaySound(AUDIO_METER_L_VOL_C_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0008)==0x0008)
	{
		DisplayMsg.CurrentPage = DISPMETER_H_VOL_A_CHARGING_F;
		//PlaySound(AUDIO_METER_H_VOL_A_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0010)==0x0010)
	{
		DisplayMsg.CurrentPage = DISPMETER_H_VOL_B_CHARGING_F;
		//PlaySound(AUDIO_METER_H_VOL_B_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0020)==0x0020)
	{
		DisplayMsg.CurrentPage = DISPMETER_H_VOL_C_CHARGING_F;
		//PlaySound(AUDIO_METER_H_VOL_C_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0040)==0x0040)
	{
		DisplayMsg.CurrentPage = DISPMETER_H_CUR_A_CHARGING_F;
		//PlaySound(AUDIO_METER_H_CUR_A_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0080)==0x0080)
	{
		DisplayMsg.CurrentPage = DISPMETER_H_CUR_B_CHARGING_F;
		//PlaySound(AUDIO_METER_H_CUR_B_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0100)==0x0100)
	{
		DisplayMsg.CurrentPage = DISPMETER_H_CUR_C_CHARGING_F;
		//PlaySound(AUDIO_METER_H_CUR_C_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0200)==0x0200)
	{
		DisplayMsg.CurrentPage = DISPMETER_DUANXIANG_A_CHARGING_F;
		//PlaySound(AUDIO_METER_DUANXIANG_A_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0400)==0x0400)
	{
		DisplayMsg.CurrentPage = DISPMETER_DUANXIANG_B_CHARGING_F;
		//PlaySound(AUDIO_METER_DUANXIANG_B_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x0800)==0x800)
	{
		DisplayMsg.CurrentPage = DISPMETER_DUANXIANG_C_CHARGING_F;
		//PlaySound(AUDIO_METER_DUANXIANG_C_CHARGING,NULL);
	}	
	else if((DefalutPhaseFlag&0x1000)==0x1000)
	{
		DisplayMsg.CurrentPage = DISPMETER_DUANLU_A_CHARGING_F;
		//PlaySound(AUDIO_METER_DUANLU_A_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x2000)==0x2000)
	{
		DisplayMsg.CurrentPage = DISPMETER_DUANLU_B_CHARGING_F;
		//PlaySound(AUDIO_METER_DUANLU_B_CHARGING,NULL);
	}
	else if((DefalutPhaseFlag&0x4000)==0x4000)
	{
		DisplayMsg.CurrentPage = DISPMETER_DUANLU_C_CHARGING_F;
		//PlaySound(AUDIO_METER_DUANLU_C_CHARGING,NULL);
	}

	return DisplayMsg.CurrentPage;
}



void MeterExcep_Audio(void)
{	
	if((DefalutPhaseFlag&0x0001)==0x0001)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_L_VOL_A_CHARGING_F)
		PlaySound(AUDIO_METER_L_VOL_A_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_L_VOL_A_IDLE)
		PlaySound(AUDIO_METER_L_VOL_A_IDLE,NULL);			
	}
	else if((DefalutPhaseFlag&0x0002)==0x0002)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_L_VOL_B_CHARGING_F)
		PlaySound(AUDIO_METER_L_VOL_B_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_L_VOL_B_IDLE)
		PlaySound(AUDIO_METER_L_VOL_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0004)==0x0004)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_L_VOL_C_CHARGING_F)
		PlaySound(AUDIO_METER_L_VOL_C_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_L_VOL_C_IDLE)
		PlaySound(AUDIO_METER_L_VOL_C_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0008)==0x0008)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_H_VOL_A_CHARGING_F)
		PlaySound(AUDIO_METER_H_VOL_A_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_H_VOL_A_IDLE)
		PlaySound(AUDIO_METER_H_VOL_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0010)==0x0010)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_H_VOL_B_CHARGING_F)
		PlaySound(AUDIO_METER_H_VOL_B_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_H_VOL_B_IDLE)
		PlaySound(AUDIO_METER_H_VOL_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0020)==0x0020)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_H_VOL_C_CHARGING_F)
		PlaySound(AUDIO_METER_H_VOL_C_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_H_VOL_C_IDLE)
		PlaySound(AUDIO_METER_H_VOL_C_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0040)==0x0040)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_H_CUR_A_CHARGING_F)
		PlaySound(AUDIO_METER_H_CUR_A_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_H_CUR_A_IDLE)
		PlaySound(AUDIO_METER_H_CUR_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0080)==0x0080)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_H_CUR_B_CHARGING_F)
		PlaySound(AUDIO_METER_H_CUR_B_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_H_CUR_B_IDLE)
		PlaySound(AUDIO_METER_H_CUR_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0100)==0x0100)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_H_CUR_C_CHARGING_F)
		PlaySound(AUDIO_METER_H_CUR_C_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_H_CUR_C_IDLE)
		PlaySound(AUDIO_METER_H_CUR_C_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0200)==0x0200)
	{
		printf("DisplayMsg.CurrentPage is %d\n",DisplayMsg.CurrentPage);
		if(DisplayMsg.CurrentPage == DISPMETER_DUANXIANG_A_CHARGING_F)
		PlaySound(AUDIO_METER_DUANXIANG_A_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_DUANXIANG_A_IDLE)
		PlaySound(AUDIO_METER_DUANXIANG_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0400)==0x0400)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_DUANXIANG_B_CHARGING_F)
		PlaySound(AUDIO_METER_DUANXIANG_B_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_DUANXIANG_B_IDLE)
		PlaySound(AUDIO_METER_DUANXIANG_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x0800)==0x0800)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_DUANXIANG_C_CHARGING_F)
		PlaySound(AUDIO_METER_DUANXIANG_C_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_DUANXIANG_C_IDLE)
		PlaySound(AUDIO_METER_DUANXIANG_C_IDLE,NULL);
	}	
	else if((DefalutPhaseFlag&0x1000)==0x1000)
	{
		if(DisplayMsg.CurrentPage ==DISPMETER_DUANLU_A_CHARGING_F)
		PlaySound(AUDIO_METER_DUANLU_A_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_DUANLU_A_IDLE)
		PlaySound(AUDIO_METER_DUANLU_A_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x2000)==0x2000)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_DUANLU_B_CHARGING_F)
		PlaySound(AUDIO_METER_DUANLU_B_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_DUANLU_B_IDLE)
		PlaySound(AUDIO_METER_DUANLU_B_IDLE,NULL);
	}
	else if((DefalutPhaseFlag&0x4000)==0x4000)
	{
		if(DisplayMsg.CurrentPage == DISPMETER_DUANLU_C_CHARGING_F)
		PlaySound(AUDIO_METER_DUANLU_C_CHARGING,NULL);
		
		if(DisplayMsg.CurrentPage == DISPMETER_DUANLU_C_IDLE)
		PlaySound(AUDIO_METER_DUANLU_C_IDLE,NULL);
	}

}

/********************************************************************************* 
 函数名称： DealScreenExcep
 功能描述： 显示屏故障处理
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.9.6
 修改记录：
*********************************************************************************/
void DealScreenExcep(void)
{
	static ST_CHAR Status_last = 0,Status_current = 0;
	ST_CHAR ScreenExcepFlag_HF = 0;
//	printf("EventFlag.ScreenExcepFlag is %d ScreenExcepFlag_HF is %d Status_last is %d \n",EventFlag.ScreenExcepFlag,ScreenExcepFlag_HF,Status_last);
	Status_current = EventFlag.ScreenExcepFlag;
	if(Status_current != Status_last )
	{
		if(1 ==Status_last && 0 ==Status_current)
		{
			ScreenExcepFlag_HF = 1;
			printf(" Status_current is %d Status_last is %d \n",Status_current,Status_last);
		}
		Status_last = Status_current;		
	}
	
	if(EventFlag.ScreenExcepFlag == 1)   
	{
		//PaleExcepLedOn();
		if(DisplayInfo.CurrentPage==DISP_CHARGING_ELE||DisplayInfo.CurrentPage==DISP_CHARGING_MONEY||DisplayInfo.CurrentPage==DISP_CHARGING_TIME||DisplayInfo.CurrentPage==DISP_CHARGING_ZIDONG)
		{		
			EndCharge();
		}		
	}
	else
	{
		if(EventFlag.ChargeExcepEndFlag&0x10)
		{
		//	sleep(2);
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.RepairDoorOpenFg==0&&EventFlag.CutdownEleFlag==0)
			{
				if(EventFlag.QiangConGood==1) 
				{
					JieChuQiOn();
					//PaleExcepLedOff();
					//ChargeLedOn();
					EventFlag.StartChargeFlag = 1;
					EventFlag.ChargeExcepEndFlag^= 0x10;
							
					ChargeMsg.PaleStatus = PALECHARGING;	
				//	InitNewAlarmRecord(0x00,ZDLD,NULL,0);//突然断电后来电时上报给主站		
					printf("DealScreenExcep alarm hf charging\n");
				}
				else
				{
						EventFlag.ChargeExcepEndFlag = 0;
						ScreenExcepFlag_HF = 0;					
						ChargeMsg.PaleStatus = PALEIDLE;
						printf(" ************************plug out DealScreenExcep alarm hf Idle\n");				
				}

			}
			
		}
		else if(1 == ScreenExcepFlag_HF)
		{
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.RepairDoorOpenFg==0&&EventFlag.CutdownEleFlag==0)	  //在待机状态或者充满、余额不足、充电完成状态下，充电桩断电来电恢复
			{	
				if(EventFlag.ChargeFullFlag==1||EventFlag.ChargeNomoneyNoPulgFg==1||EventFlag.ChargeCompleteFlag==1)  
				{
					if(EventFlag.ChargeNomoneyNoPulgFg==1)
					{
						//IdleLedOn();
					}	
					
				}
				else 
				{				
					//IdleLedOn();
				}
				ScreenExcepFlag_HF = 0;
				ChargeMsg.PaleStatus = PALEIDLE;
				
				ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
				InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息
				printf("DealScreenExcep alarm\n");				 	
			}
		}
	}		
}


/********************************************************************************* 
 函数名称： DealTuRanDuanDianLaiDian
 功能描述： 桩突然断电后来电的处理，来电后检测插头连接，连接好的情况下启动充电，
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
		
void DealTuRanDuanDianLaiDian(void)
{
//	static ST_UCHAR yuchongdiancount = 0;
//	static ST_CHAR yuchongdianfinish = 0;
//	static ST_CHAR LastDuanDianStatus = 0;
	static ST_CHAR Status_last,Status_current;
	ST_CHAR CutdownEleFlag_HF;
	Status_current = EventFlag.CutdownEleFlag;
	
	if(Status_current != Status_last )
	{
		if(1 ==Status_last && 0 ==Status_current)
		{
			CutdownEleFlag_HF = 1;
		}
		Status_last = Status_current;		
	}
	printf("EventFlag.CutdownEleFlag is %d\n",EventFlag.CutdownEleFlag);
	
	if((EventFlag.ChargeExcepEndFlag&0x04) && EventFlag.QiangConGood==0)
	{		
		EventFlag.ChargeExcepEndFlag^= 0x04;		
		ChargeMsg.PaleStatus = PALEIDLE;
		printf(" ************************plug out DealTuRanDuanDianLaiDian alarm hf Idle\n");		
	}

	if(EventFlag.CutdownEleFlag==1)   //突然断电状态
	{

		//PaleExcepLedOn();
		if(DisplayInfo.CurrentPage==DISP_CHARGING_ELE||DisplayInfo.CurrentPage==DISP_CHARGING_MONEY||DisplayInfo.CurrentPage==DISP_CHARGING_TIME||DisplayInfo.CurrentPage==DISP_CHARGING_ZIDONG)
		{
			printf("######################################DealTuRanDuanDianLaiDian DisplayInfo.CurrentPage is %d\n",DisplayInfo.CurrentPage);
			EndCharge();
			printf("#########EventFlag.CutdownEleFlag\n");

		}				
	}
	else
	{
		if(EventFlag.ChargeExcepEndFlag&0x04)
		{
		//	sleep(2);
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.RepairDoorOpenFg==0&&EventFlag.ScreenExcepFlag == 0)
			{
				if(EventFlag.QiangConGood==1) 
				{
					JieChuQiOn();
					//PaleExcepLedOff();
					//ChargeLedOn();
					EventFlag.StartChargeFlag = 1;
					EventFlag.ChargeExcepEndFlag^= 0x04;		
					ChargeMsg.PaleStatus = PALECHARGING;	
					InitNewAlarmRecord(0x00,ZDLD,NULL,0);//突然断电后来电时上报给主站		
					printf("DealTuRanDuanDianLaiDian alarm hf charging\n");
				}
				else
				{
						EventFlag.ChargeExcepEndFlag = 0;
						CutdownEleFlag_HF = 0;					
						ChargeMsg.PaleStatus = PALEIDLE;
						printf(" ************************plug out DealTuRanDuanDianLaiDian alarm hf Idle\n");				
				}

			}
		}
		else if( 1 == CutdownEleFlag_HF )
		{
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.RepairDoorOpenFg==0&&EventFlag.ScreenExcepFlag == 0)	  //在待机状态或者充满、余额不足、充电完成状态下，充电桩断电来电恢复
			{	
				if(EventFlag.ChargeFullFlag==1||EventFlag.ChargeNomoneyNoPulgFg==1||EventFlag.ChargeCompleteFlag==1)  
				{
					if(EventFlag.ChargeNomoneyNoPulgFg==1)
					{
						//IdleLedOn();
					}	
					
				}
				else 
				{				
					//IdleLedOn();
				}
				CutdownEleFlag_HF = 0;
				ChargeMsg.PaleStatus = PALEIDLE;	
				ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
				InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息
				printf("DealTuRanDuanDianLaiDian alarm\n");				 	
			}
		}
	}	
	
}

/********************************************************************************* 
 函数名称： CommunitHF
 功能描述： 桩异常自恢复的处理,通信自恢复时，桩在插头连接好的情况下启动充电，
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/




/********************************************************************************* 
 函数名称： CommunicationExcOrMeterExc
 功能描述： 当通信异常，或者过压过流欠压等异常时，停止充电
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void CommunicationExcOrMeterExc(void)
{
	static ST_CHAR Status_last,Status_current,CountHF = 0;
	ST_CHAR CardMeterComExcepFg_HF,CardMeterComExcepFg;
	#if 0
	if((DefalutPhaseFlag&0x0008)==0x0008)   //测试过压恢复
	{
		printf("****************************************************DefalutPhaseFlag is %x\n",DefalutPhaseFlag);
		sleep(1);
		CountHF++;
		if(CountHF==10)
		{
			HoutaiData.fOverVolLimit = 250;
			printf("****************************************************HoutaiData.fOverVolLimit huifu\n");
			CountHF = 0;
		}
	}
	else CountHF = 0;
		
	#endif  
	#if 1
	if((DefalutPhaseFlag&0x0001)==0x0001)  //测试欠压恢复
	{
		printf("****************************************************DefalutPhaseFlag is %x\n",DefalutPhaseFlag);
		sleep(1);
		CountHF++;
		if(CountHF==10)
		{
			HoutaiData.fLowVolLimit = 190;
			printf("****************************************************HoutaiData.fOverVolLimit huifu\n");
			CountHF = 0;
		}
	}
	else CountHF = 0;
		
	#endif
	
	if(EventFlag.MeterExcepFlag!=0||EventFlag.CardExcepFlag==1)
	{
		CardMeterComExcepFg = 1;
		
	}
	else
	{
		CardMeterComExcepFg = 0;
	}
	
	Status_current = CardMeterComExcepFg;
	if(Status_current != Status_last )
	{
		if(1 ==Status_last && 0 ==Status_current)
		{
			CardMeterComExcepFg_HF = 1;
		}
		Status_last = Status_current;		
	}
	
	if(EventFlag.MeterExcepFlag!=0||EventFlag.CardExcepFlag==1)  //通信异常或过压过流欠压
	{	
		//PaleExcepLedOn();	
		if(DisplayInfo.CurrentPage==DISP_CHARGING_ELE||DisplayInfo.CurrentPage==DISP_CHARGING_MONEY||DisplayInfo.CurrentPage==DISP_CHARGING_TIME||DisplayInfo.CurrentPage==DISP_CHARGING_ZIDONG)
		{	
			sleep(1);		
			if(EventFlag.MeterExcepFlag==1)
			{
				
				EndCharge();
				printf("#########EventFlag.MeterExcepFlag\n");
			//	PlaySound(AUDIO_EXCEP_CHARGING,NULL);
				//ST_CHAR PaleStatusAlarm[2] = {0x02,0x00};
				//InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 
			}
			else if(EventFlag.CardExcepFlag==1)
			{				
				EndCharge();	
				printf("#########EventFlag.MeterExcepFlag\n");			
				//ST_CHAR PaleStatusAlarm[2] = {0x02,0x00};
				//InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 

			//	PlaySound(AUDIO_CARDCOM_EXCEP_CHARGING,NULL);
			//	sleep(3);
			}
			else 
			{
			
				EndCharge();
				printf("#########EventFlag.MeterExcepFlag2\n");	
			//	PlaySound(AUDIO_EXCEP_CHARGING,NULL);

			//	ST_CHAR PaleStatusAlarm[2] = {0x02,0x00};
			//	InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 				
			}
		}

		if(DisplayInfo.CurrentPage==DISP_IDLE) //异常结束时，待机界面显示故障信息
		{

		//	PlaySound(AUDIO_EXCEP_IDLE,NULL);
		}	

	}
	else 
	{
		if(EventFlag.ChargeExcepEndFlag&0x03)
		{
			if(EventFlag.CutdownEleFlag==0&&EventFlag.RepairDoorOpenFg==0&&EventFlag.ScreenExcepFlag == 0 )
			{
				PaleExcepLedOff();
					
				if(EventFlag.QiangConGood==1)
				{
					JieChuQiOn();	
					//ChargeLedOn();			
					EventFlag.StartChargeFlag = 1;
					EventFlag.ChargeExcepEndFlag&= 0xFC;
					ChargeMsg.PaleStatus = PALECHARGING;	
					ST_UCHAR PaleStatusAlarm[2] = {0x00,0x00};         //充电桩正在充电
					InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 	
					printf("CommunicationExcOrMeterExc alarm\n");
				}
				else
				{
						EventFlag.ChargeExcepEndFlag = 0;
						CardMeterComExcepFg_HF = 0;					
						ChargeMsg.PaleStatus = PALEIDLE;
						
						printf(" ************************plug out CommunicationExcOrMeterExc alarm hf Idle EventFlag.ChargeExcepEndFlag is %d\n",EventFlag.ChargeExcepEndFlag);				
				}					
			}				
		}
		else if( 1 == CardMeterComExcepFg_HF )		
		{
			if(EventFlag.CutdownEleFlag==0&&EventFlag.RepairDoorOpenFg==0&& EventFlag.ScreenExcepFlag == 0)
			{
				if(EventFlag.ChargeFullFlag==1||EventFlag.ChargeNomoneyNoPulgFg==1||EventFlag.ChargeCompleteFlag==1)  
				{
					if(EventFlag.ChargeNomoneyNoPulgFg==1)
					{
						//IdleLedOn();
					}
				}
				else
				{
					//IdleLedOn();
				}
				CardMeterComExcepFg_HF = 0;
				ChargeMsg.PaleStatus = PALEIDLE;	
				ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
				InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 	
				printf("CommunitHF alarm\n");
				
			}
		}
	}
		
}

/********************************************************************************* 
 函数名称： DealRepairDoorOpen
 功能描述： 维修门打开，充电桩处于故障状态，该函数实现当维修门打开时的处理
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：2011-5-25
*********************************************************************************/

void DealRepairDoorOpen(void)
{
	//static ST_UCHAR yuchongdiancount = 0;
//	static ST_CHAR yuchongdianfinish = 0;	
	static ST_CHAR Status_last,Status_current;
	ST_CHAR RepairDoorOpenFg_HF;
	Status_current = EventFlag.RepairDoorOpenFg;
	if(Status_current != Status_last )
	{
		if(1 ==Status_last && 0 ==Status_current)
		{
			RepairDoorOpenFg_HF = 1;
		}
		Status_last = Status_current;		
	}
		
	printf("come in DealRepairDoorOpen\n");
	if(EventFlag.RepairDoorOpenFg==1)  //维修门打开
	{	
		
		printf("DealRepairDoorOpen yellow led on\n");
		//PaleExcepLedOn();
	
		if(DisplayInfo.CurrentPage==DISP_CHARGING_ELE||DisplayInfo.CurrentPage==DISP_CHARGING_MONEY||DisplayInfo.CurrentPage==DISP_CHARGING_TIME||DisplayInfo.CurrentPage==DISP_CHARGING_ZIDONG)
		{
			
			EndCharge();
			printf("############repair open\n");
	//		PlaySound(AUDIO_ZANTING_CHARGING,NULL);
		//	ST_CHAR PaleStatusAlarm[2] = {0x02,0x00};
		//	InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 					
		}

		if(DisplayMsg.CurrentPage==DISP_IDLE) //在空闲状态下
		{
		//	PlaySound(REPAIRDOOROPENIDLE,NULL);								
		}	

	}
	else
	{			
//		printf("DealRepairDoorOpen EventFlag.CutdownEleFlag is %d",EventFlag.CutdownEleFlag);		
		if(EventFlag.ChargeExcepEndFlag&0x08)
		{
			//sleep(2);
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.CutdownEleFlag==0&&EventFlag.ScreenExcepFlag == 0)
			{
				if(EventFlag.QiangConGood==1) 
				{
					JieChuQiOn();
					//ChargeLedOn();
					PaleExcepLedOff();
					EventFlag.StartChargeFlag = 1;
					EventFlag.ChargeExcepEndFlag^= 0x08;
					ChargeMsg.PaleStatus = PALECHARGING;
					printf("**********************************DealRepairDoorOpen resume********************************\n" );	
				}
				else
				{
					EventFlag.ChargeExcepEndFlag = 0;
					RepairDoorOpenFg_HF = 0;					
					ChargeMsg.PaleStatus = PALEIDLE;
					printf(" ************************plug out CommunicationExcOrMeterExc alarm hf Idle b\n");				
				}
							
			}
        
		}
		else if( 1 == RepairDoorOpenFg_HF )
		{
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.CutdownEleFlag==0&&EventFlag.ScreenExcepFlag == 0)
			{
				if(EventFlag.ChargeFullFlag==1||EventFlag.ChargeNomoneyNoPulgFg==1||EventFlag.ChargeCompleteFlag==1) 
				{
					if(EventFlag.ChargeNomoneyNoPulgFg==1)
					{
						//IdleLedOn();
					}
				}
				else
				{
					//IdleLedOn();
				}	
				RepairDoorOpenFg_HF = 0;		
				ChargeMsg.PaleStatus = PALEIDLE;	
			//	printf(" ************************plug out CommunicationExcOrMeterExc alarm hf Idle\n");	
				ST_UCHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪
				InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息
				printf("DealRepairDoorOpen alarm\n");
			}
		}
		
	}	
		
}


/********************************************************************************* 
 函数名称： DealPulgOutInCharging
 功能描述： 充电过程中，插头被拔出，则停止充电，当插头插好后，继续充电
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.7.25
 修改记录：
*********************************************************************************/
		
void DealPulgOutInCharging(void)
{
//	static ST_CHAR LastDuanDianStatus = 0;
	
	static ST_CHAR Status_last,Status_current;
	ST_CHAR QiangConGood_HF;
	Status_current = EventFlag.QiangConGood;
	if(Status_current != Status_last )
	{
		if(0 ==Status_last && 1 ==Status_current)
		{
			QiangConGood_HF = 1;
		}
		Status_last = Status_current;		
	}	

	if( EventFlag.QiangConGood==0 )
	{
		if( DISP_IDLE == DisplayInfo.CurrentPage|| DISP_NOPAPLE_IDLE == DisplayInfo.CurrentPage) 
		{
			EventFlag.ChargeExcepEndFlag = 0;
			ChargeMsg.PaleStatus = PALEIDLE;
			printf(" ************************plug out DealPulgOutInCharging alarm  Idle\n");	
			QiangConGood_HF = 0;
		}

		//PaleExcepLedOn();
		
		if(DisplayInfo.CurrentPage==DISP_CHARGING_ELE||DisplayInfo.CurrentPage==DISP_CHARGING_MONEY||DisplayInfo.CurrentPage==DISP_CHARGING_TIME||DisplayInfo.CurrentPage==DISP_CHARGING_ZIDONG)
		{
			EndCharge();
			printf("###############pulg out \n");
	//		PlaySound(AUDIO_DUANDIAN_CHARGING,NULL);
		//	ST_CHAR PaleStatusAlarm[2] = {0x02,0x00};
		//	InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息
		}
		if(DisplayInfo.CurrentPage==DISP_IDLE) 
		{
	//		PlaySound(AUDIO_EXCEP_IDLE,NULL);			
		}		
	}
	else
	{		
		if((EventFlag.ChargeExcepEndFlag&0x20)&& 1 == QiangConGood_HF)
		{
		//	sleep(2);
			if(EventFlag.MeterExcepFlag==0&&EventFlag.CardExcepFlag==0&&EventFlag.RepairDoorOpenFg==0&&EventFlag.CutdownEleFlag==0&&EventFlag.ScreenExcepFlag == 0)
			{
				if(EventFlag.QiangConGood==1) 
				{
					JieChuQiOn();
					PaleExcepLedOff();
					//ChargeLedOn();
					EventFlag.StartChargeFlag = 1;
					EventFlag.ChargeExcepEndFlag^= 0x20;	
					QiangConGood_HF = 0;	
					ChargeMsg.PaleStatus = PALECHARGING;	
					//InitNewAlarmRecord(0x00,ZDLD,NULL,0); //突然断电后来电时上报给主站	
					printf("DealPulgOutInCharging alarm\n");	
				}				
			}
		}		
	}
}


/********************************************************************************* 
 函数名称： DealChargeFull_Nomoney
 功能描述： 充满电或者充电过程中余额不足结束充电后，桩出现异常的处理
 输    入：	
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：2011-4-8
*********************************************************************************/
void DealChargeFull_Nomoney(void)
{
	#if 0
	ST_UCHAR DisplayPageTemp = 0;
    DisplayPageTemp = MeterExcep_Disp_Idle();

	if(EventFlag.ChargeNomoneyNoPulgFg == 1&&ChargeMsg.ChargeFullNomoneyNoPulgCount>5*60)
	{
		if(EventFlag.MeterExcepFlag==1)
		{
	 		DisplayMsg.CurrentPage =  METER_EXCEPTION_IDLE;
		}
		else if(EventFlag.CardExcepFlag==1)
		{
			DisplayMsg.CurrentPage = CARD_EXCEPTION_IDLE;
		}
		else if(EventFlag.CutdownEleFlag==1)
		{
			DisplayMsg.CurrentPage = DISPDUANDIAN_IDLE;
		}
		else if(EventFlag.RepairDoorOpenFg==1)
		{
			DisplayMsg.CurrentPage = REPAIRDOOROPENIDLE;
		}
		else  DisplayMsg.CurrentPage = MeterExcep_Disp_Idle();
			
		EventFlag.ChargeNomoneyNoPulgFg = 0;
		ChargeMsg.ChargeFullNomoneyNoPulgCount = 0;
	}
	#endif
}







/********************************************************************************* 
 函数名称： PaleExceptDealAllCard
 功能描述： 桩在异常情况下检测到所有卡的处理,首先弹卡，突然断电后，当再次来电时检测到插头则继续充电，
		 	 当通信异常结束充电，当正常是也需继续充电，
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：2011-4-8
*********************************************************************************/


void PaleExceptDealAllCard(void)
{
	//printf("come to PaleExceptDealAllCard EventFlag.IsExistCardFlag is 1");
	if(EventFlag.IsExistCardFlag==1)
	{
		printf("come to PaleExceptDealAllCard EventFlag.IsExistCardFlag is 1");
		PaleExcepFindCard();
	}
	else
	{	
		DealPulgOutInCharging();	
	//	printf("come to PaleExceptDealAllCard\n");
		DealRepairDoorOpen();
	
		CommunicationExcOrMeterExc();
		DealTuRanDuanDianLaiDian();
		//CommunitHF();
		DealScreenExcep();
	}
}


/********************************************************************************* 
 函数名称： PaleIdleDealTestCard
 功能描述： 桩在空闲状态下检测到功能测试卡处理。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


void PaleIdleDealTestCard(void)
{
	static ST_UCHAR OpenDoorflag = 0,Starttestflag=0;
	if(Starttestflag==0)
	{
		PlaySound(AUDIO_FUCTIONCARD,NULL);
		sleep(3);		
		DisplayMsg.CurrentPage = FUNCTIONTEST;		
		PlaySound(AUDIO_START_TO_TEST,NULL);		
		sleep(3);
		OpenDoor();
		printf("start come to test open door\n");
		DisplayMsg.CurrentPage = TESTOPENDOOR;
		PlaySound(AUDIO_OPENDOOR,NULL);
		sleep(3);		
		Starttestflag = 1;
	}
	if(CheckDoorIsOpen()&&OpenDoorflag==0)
	{
		PlaySound(BEGINECHARGE,NULL);
		OpenDoorflag = 1;		
		JieChuQiOn();
		//ChargeLedOn();	
		PackTestCardData();
		DisplayMsg.CurrentPage = TESTCHARGING;
		sleep(60); //实际为3秒
		JieChuQiOff();
		ChargeLedOff();
		sleep(3);
		DisplayMsg.CurrentPage = TESTCLOSEDOOR;
		PlaySound(CLOSEDOOR,NULL);
		sleep(3); //便于测试设为3，正常为60秒		
	}
	if(OpenDoorflag==1)
	{
		if(CheckDoorIsClosed())
		{
			DisplayMsg.CurrentPage = TESTCOMPELETE;
			PlaySound(AUDIO_TEST_COMPLETED,NULL);
			sleep(3);//便于测试设为3.正常为60
			DisplayMsg.CurrentPage = DISPIDLE;
			OpenDoorflag = 0;
			Starttestflag = 0;
			Tanka();
			
		}
	}
				
}

/********************************************************************************* 
 函数名称： PaleChargingDealTestCard
 功能描述： 桩在正在充电过程中检测到功能测试卡。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PaleChargingDealTestCard(void)
{
	DisplayMsg.LastPage = DisplayMsg.CurrentPage;
	DisplayMsg.CurrentPage = TESTCARDINCHARGE;
	PlaySound(AUDIO_FUCTIONCARD_PALEIDE_USE,NULL);
	sleep(4);
	DisplayMsg.CurrentPage = DisplayMsg.LastPage;
	Tanka();

}

/********************************************************************************* 
 函数名称： CheckManageCardNo
 功能描述： 检测是否是桩内管理卡的卡号。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_UCHAR CheckManageCardNo(void)
{
	return 1;
}

/********************************************************************************* 
 函数名称： CheckRepairCardNo
 功能描述： 检测是否是故障检修卡。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_UCHAR CheckRepairCardNo(void)
{
	return 1;
}


/********************************************************************************* 
 函数名称： PaleIdleDealManageCard
 功能描述： 桩在空闲状态下检测到管理卡的处理过程
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


void PaleIdleDealManageCard(void)
{
	static ST_INT32 DoorCount = 0;
	static ST_CHAR Opendoorflag = 0;
	
	if(CheckManageCardNo())
	{		
		if(!(EventFlag.SoundTipFlag &(1<<(PUSHPLUG-1))))			
		{
			PlaySound(AUDIO_MANAGECARD,NULL);
			sleep(3);			
			PackCardMsg();
			DisplayMsg.CurrentPage = DISPCLODEDOOR;
			OpenDoor();
			PlaySound(PUSHPLUG,NULL);
			EventFlag.SoundTipFlag^=(1<<(PUSHPLUG-1));
		}
		 
		usleep(500*1000);
		if(CheckDoorIsOpen()&&Opendoorflag==0)
		{
			if(CheckChaTouConnect()==1)         //检测到本次的充电插头为充电枪
			{
				PlaySound(CLOSEDOOR,NULL);
				DisplayMsg.CurrentPage = DISPONLYCLODEDOOR;        //显示关好插座门

			}
			else
			{
				PlaySound(PUSHPLUGCLOSEDOOR,NULL);
			}
			printf("Door is Open!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
			DoorCount = 0;	
			Opendoorflag = 1;					
		}
		else if(Opendoorflag==0) 
		{
			printf("Not Open the Door!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			DoorCount++;
			if(DoorCount>60)  //1分钟没有开门则弹卡，返回主界面
			{
				DoorCount = 0;
				DisplayMsg.CurrentPage = DISPIDLE;
				//EventFlag.SoundTipFlag = 0;
				Tanka();

			}
			else
			{
				if(IsPlayFinish())
				{
					PlaySound(PUSHPLUG,NULL);
				}
			}
		}
		if(Opendoorflag==1)
		{
			if(CheckDoorIsClosed())                //检测门是否关好
			{
				printf("Door is Close!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				StopCompose();
				CloseDoor();
				DoorCount = 0;
				if(CheckChaTouConnect()==1)            //检测到枪插好
				{
					DisplayMsg.CurrentPage = DISPNOTMOVE_CAR;
					PlaySound(NOMOVETHECAR,NULL);
					sleep(4);					
					StartCharge();
					PlaySound(BEGINECHARGE,NULL);
					Opendoorflag = 0;
					//EventFlag.SoundTipFlag = 0;		
					Tanka();

					printf("start charge qiang cha hao \n");
				}                                				
				else
				{
					printf("Kai shi yu chong dian !!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
					PackCardMsg();                       //打包要显示的卡和桩信息 
					DisplayMsg.CurrentPage = CHARTOUCONNECTING;
					YuChongDian();      //枪未插好，启动预充电
					
				}
			}
			else
			{
				printf("Not Close the door !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				DoorCount++;
				if(DoorCount>60*5)  //5分钟未关门则弹卡，返回主界面
				{
					DoorCount = 0;							
					DisplayMsg.CurrentPage = DISPIDLE;								
					//EventFlag.SoundTipFlag = 0;	
					Tanka();

					return;
				}
				else
				{
                              
    				if(IsPlayFinish())
					{
						if(CheckChaTouConnect())         //检测到本次的充电插头为充电枪
						{
							DisplayMsg.CurrentPage = DISPONLYCLODEDOOR;        //显示关好插座门
							PlaySound(CLOSEDOOR,NULL);

						}
						else
						{
							
							PlaySound(PUSHPLUGCLOSEDOOR,NULL);
						}
					}
				
				}
			}		
		}		
	}
	else
	{
		DisplayMsg.CurrentPage = MANAGECARDUNUSE;
		Tanka();
		
		sleep(3);
		DisplayMsg.CurrentPage = DISPIDLE;
	}	
}


/********************************************************************************* 
 函数名称： PaleChargingDealManageCard
 功能描述： 桩在充电状态下检测到管理卡的处理过程。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


void PaleChargingDealManageCard(void)
{
	#if 0
	static ST_UCHAR DoorOpenflag = 0;
	static ST_INT32 WaitCloseDoorTimeCount = 0;
	static ST_UCHAR Checkfinishflag = 0,EndChargeFlag = 0;
	if(CheckManageCardNo())
	{
		if(EndChargeFlag==0)
		{
			DisplayMsg.CurrentPage = DISPENDCHARGE;	
			PlaySound(ENDCHARGING,NULL);
			EndCharge();
			OpenDoor();
			PlaySound(OPENDOORPULLPLUG,NULL);
			EndChargeFlag = 1;
			EventFlag.Repair_Or_Manage_CardEndChargeFg = 1;
		}
	}		
	else
	{
		//DisplayMsg.LastPage = DisplayMsg.CurrentPage;
		//DisplayMsg.CurrentPage = MANAGECARDUNUSE;
		PlaySound(AUDIO_MANAGECARD_NOUSE,NULL);
		sleep(3);
		Tanka();
		
		sleep(3);
		Checkfinishflag = 0;
		//DisplayMsg.CurrentPage = DisplayMsg.LastPage;
		return;
	}
	if(CheckDoorIsOpen()&&DoorOpenflag==0)
	{
		CheckChaTouConnect();
		if(EventFlag.IsChargeQiang==1&&EventFlag.QiangConGood==0)         //检测到本次的充电插头为充电枪
		{
			//if(EventFlag.IsQiangConGood==0)       //检测到枪已经拔掉
			{
				DisplayMsg.CurrentPage = DISPENDCLOSEDOOR;        //显示关好插座门
				PlaySound(CLOSEDOOR,NULL);
				//EventFlag.IsChargeQiang = 0;
			}
		}
		else
		{
			PlaySound(PULLPLUGCLOSEDOOR,NULL);
		}
		DoorOpenflag = 1;
		
	}
	else if(DoorOpenflag==0) 
	{
		if(IsPlayFinish())
		{
			PlaySound(OPENDOORPULLPLUG,NULL);
		}
	}
	if(DoorOpenflag==1)
	{
		usleep(500*1000);         //每次延时500毫秒
		
		if(CheckDoorIsClosed()) //检测到插座门已关好
		{
			StopCompose();
			CloseDoor();
			EventFlag.IsChargeQiang = 0;
			ChargeMsg.PaleStatus = PALEIDLE;
			DisplayMsg.CurrentPage = DISPTHANK;
			PlaySound(THANKBYE,NULL); 
			Checkfinishflag = 0;			
			DoorOpenflag = 0;
			WaitCloseDoorTimeCount = 0;
			ST_CHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪  
			InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 
			//EventFlag.SoundTipFlag = 0;	
			EndChargeFlag = 0;
			
			sleep(1);
			Tanka();

			sleep(1);
			PageThank_Return();
			return;					
		}	
		else
		{	                          
			if(IsPlayFinish())
			{
				CheckChaTouConnect();
				if(EventFlag.IsChargeQiang==1&&EventFlag.QiangConGood==0)         //检测到本次的充电插头为充电枪
				{
					//if(EventFlag.IsQiangConGood==0)       //检测到枪已经拔掉
					{						
						DisplayMsg.CurrentPage = DISPENDCLOSEDOOR;        //显示关好插座门
						PlaySound(CLOSEDOOR,NULL);
					}
				}
				else
				{
					
					PlaySound(PULLPLUGCLOSEDOOR,NULL);
				}
			}
			WaitCloseDoorTimeCount++;
			if(WaitCloseDoorTimeCount>=600)  //5分钟后仍然没有关门，则上报插座门关门故障给后台
			{
				InitNewAlarmRecord(0x00,CZMGMGZ,NULL,0);
			}
		}		

	}
	#endif
}


/********************************************************************************* 
 函数名称： PaleIdleDealRepairCard
 功能描述： 桩在空闲状态下检测到故障检修卡的处理过程。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PaleIdleDealRepairCard(void)
{
	if(CheckRepairCardNo())
	{
		printf("This is repair card\n");
		DisplayMsg.CurrentPage = FINDREPAIRCARD;
		PlaySound(AUDIO_REPAIRCARD,NULL);
		sleep(3);

		Tanka();
		DisplayMsg.CurrentPage = DISPIDLE;
	}
	else
	{
		DisplayMsg.CurrentPage = REPAIRCARDUNUSE;
		sleep(3);
		Tanka();

		DisplayMsg.CurrentPage = DISPIDLE;
	}
}


/********************************************************************************* 
 函数名称： PaleChargingDealRepairCard
 功能描述： 桩在充电状态下检测到故障检修卡的处理过程
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


void PaleChargingDealRepairCard(void)
{
	#if 0
	static ST_INT32 WaitCloseDoorTimeCount = 0;
	static ST_UCHAR DoorOpenflag = 0;
	static ST_UCHAR Checkfinishflag = 0;
	if(CheckRepairCardNo())
	{
		if(Checkfinishflag==0)
		{
			printf("This is repair card.endcharging\n");
			PlaySound(ENDCHARGING,NULL);
			EndCharge();
			DisplayMsg.CurrentPage = DISPENDCHARGE;
			OpenDoor();
			PlaySound(OPENDOORPULLPLUG,NULL);
			Checkfinishflag = 1;
			EventFlag.Repair_Or_Manage_CardEndChargeFg = 1;
		}
	}
	else
	{
		DisplayMsg.LastPage = DisplayMsg.CurrentPage;
		DisplayMsg.CurrentPage = REPAIRCARDUNUSE;
		Tanka();
		
		sleep(3);
		Checkfinishflag = 0;
		DisplayMsg.CurrentPage = DisplayMsg.LastPage;
	}

	if(CheckDoorIsOpen()&&DoorOpenflag==0)
	{
		CheckChaTouConnect();
		if(EventFlag.IsChargeQiang==1&&EventFlag.QiangConGood==0)         //检测到本次的充电插头为充电枪
		{
			//if(EventFlag.IsQiangConGood==0)       //检测到枪已经拔掉
			{
				DisplayMsg.CurrentPage = DISPENDCLOSEDOOR;        //显示关好插座门
				PlaySound(CLOSEDOOR,NULL);
				//EventFlag.IsChargeQiang = 0;
			}
		}
		else
		{
			PlaySound(PULLPLUGCLOSEDOOR,NULL);
		}
		DoorOpenflag = 1;
		
	}
	else if(DoorOpenflag==0) 
	{
		if(IsPlayFinish())
		{
			PlaySound(OPENDOORPULLPLUG,NULL);
		}
	}
	if(DoorOpenflag==1)
	{
		usleep(500*1000);         //每次延时500毫秒
		
		if(CheckDoorIsClosed()) //检测到插座门已关好
		{
			StopCompose();
			CloseDoor();
			EventFlag.IsChargeQiang = 0;
			ChargeMsg.PaleStatus = PALEIDLE;
			DisplayMsg.CurrentPage = DISPTHANK; 
			PlaySound(THANKBYE,NULL);
			Checkfinishflag = 0;			
			DoorOpenflag = 0;
			WaitCloseDoorTimeCount = 0;
			ST_CHAR PaleStatusAlarm[2] = {0x01,0x00};         //充电桩就绪  
			InitNewAlarmRecord(0x00,KSCD,PaleStatusAlarm,2);  //充电桩状态改变时，上报告警信息 
			//EventFlag.SoundTipFlag = 0;
			
			sleep(1);
			Tanka();

			sleep(1);
			PageThank_Return();
			return;					
		}	
		else
		{	                          
			if(IsPlayFinish())
			{
				CheckChaTouConnect();
				if(EventFlag.IsChargeQiang==1&&EventFlag.QiangConGood==0)         //检测到本次的充电插头为充电枪
				{
					//if(EventFlag.IsQiangConGood==0)       //检测到枪已经拔掉
					{						
						DisplayMsg.CurrentPage = DISPENDCLOSEDOOR;        //显示关好插座门
						PlaySound(CLOSEDOOR,NULL);
					}
				}
				else
				{
					
					PlaySound(PULLPLUGCLOSEDOOR,NULL);
				}
			}
			WaitCloseDoorTimeCount++;
			if(WaitCloseDoorTimeCount>=600)  //5分钟后仍然没有关门，则上报插座门关门故障给后台
			{
				InitNewAlarmRecord(0x00,CZMGMGZ,NULL,0);
			}
		}		

	}
	#endif
}



/********************************************************************************* 
 函数名称： PaleIdleDealKeyModifyCard
 功能描述： 桩在空闲状态下检测到修改密钥卡。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PaleIdleDealKeyModifyCard(void)
{
	printf("This is Modify Key card\n");
	if(TxToCardModifyKeyCmd()==RES_TRUE)
	{
		DisplayMsg.CurrentPage = MODIFYKEYSUCESS;
		PlaySound(AUDIO_MODIFYKEYSUCESS,NULL);
		sleep(3);				
		DisplayMsg.CurrentPage = DISPIDLE;
		Tanka();

	}	
	else
	{
		
		DisplayMsg.CurrentPage = MODIFYKEYFAIL;
		PlaySound(AUDIO_MODIFYKEYFAIED,NULL);		
		sleep(3);
		DisplayMsg.CurrentPage = DISPIDLE;
		Tanka();

	}
	
}


/********************************************************************************* 
 函数名称： PaleChargingDealKeyModifyCard
 功能描述： 桩在充电状态下检测到修改密钥卡。
 输    入：	无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void PaleChargingDealKeyModifyCard(void)
{
	printf("This is Modify Key card,in charging\n");
	DisplayMsg.LastPage = DisplayMsg.CurrentPage;
	DisplayMsg.CurrentPage = MODIFYCARDINCHARGE;
	PlaySound(AUDIO_KEYCARD_PALEIDLE_USE,NULL);	
	sleep(4);
	DisplayMsg.CurrentPage = DisplayMsg.LastPage;
	Tanka();
	
}










