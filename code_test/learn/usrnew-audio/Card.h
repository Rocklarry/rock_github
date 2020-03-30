#ifndef  _CARD_H_
#define _CARD_H_

#include "DataType.h"

#define CARDSERIALEN      		8

#define CHARGECARD          	0x01             //充电卡
#define TESTINGCARD          	0x11                //测试卡
#define MANAGECARD         		0x51              // 管理卡
#define REPAIRCARD            	0x12              //故障检修卡
#define MODIFYKEYCARD       	0x02              //修改密钥卡
#define SETPARACARD          	0x41                 //出场设置卡

#define  CARDLOCK              	1
#define  CARDUNLOCK          	0
#define  CARDREPAY           	2



#define NOSYSTEM_CARD      		0
#define SYSTEM_CARD          	1


#define FIND_CARD                         1      //寻卡
#define PRETREATMENT_CARD                 2      //预充电
#define STARTCHARGE_CARD                  3     //开始充电
#define REPAY_CARD                        4    //补充交易
#define ENDCHARGE_CARD                    5    //结束充电

#define RANDOMNUMBER_CARD                 6
#define CALCULATEMAC2_CARD                7

#define   OUT_CARD                        8
#define REDIMESAMKEY_CARD                 9
#define REDIMESAMPARA_CARD                10                                       


/**********************************************************************************************************
 帧头帧尾
*********************************************************************************************************/
#define STX								0x02					/*	帧头								*/
#define ETX								0x03					/*	帧尾								*/
/*********************************************************************************************************
  命令字
*********************************************************************************************************/
#define CTRLCARDESAM					0x51					/*	对ESAM和卡操作的命令				*/
#define CTRLOUTCARD						0x32					/*	对读卡器tanka操作的命令				*/
/*********************************************************************************************************
  命令参数
*********************************************************************************************************/
#define FINDCARD						0x37					/*	寻卡								*/
#define PRETREATMENT					0x36					/*	预处理								*/
#define REPAY							0x35					/*	补充交易							*/
#define ENDCHARGE						0x34					/*	结束加电							*/
#define CALCULATEMAC2					0x33					/*	计算MAC2							*/
#define RANDOMNUMBER					0x32					/*	取随机数							*/
#define STARTCHARGE						0x30					/*	开始加电							*/
#define OUTCARD							0x40					/*	弹卡								*/
#define REDIMESAMKEY					0x51					/*	控制修改ESAM密匙					*/
#define REDIMESAMPARA					0x52				    /*	控制就该ESAM出厂参数				*/
/*********************************************************************************************************
  加电返回状态字
*********************************************************************************************************/
#define READCARDSUCCESS					0x59					/*	成功								*/
#define CTRLCARDFIAL					0x4E					/*	加电失败结束加电失败预处理过程失败  */
#define PSAMRESETFIAL					0x47					/*	PSAM复位失败						*/
#define CARDRESETFIAL					0x48					/*	用户卡复位失败						*/
#define CARDFASE						0x41					/*	卡片失效/过期						*/
#define INVALID							0x43					/*	需要补充交易								*/
#define SBCHARGING						0x44					/*	其他用户正在充电					*/
#define NOCARD							0x57					/*	卡机内无卡							*/
/*********************************************************************************************************
  读卡器响应寻卡命令返回的特殊状态字S1、S2
*********************************************************************************************************/
#define S1CARDREADY						0x30			 		/*	卡机内有卡							*/	
#define S1NOCARD						0x31					/*	卡机内无卡							*/
#define S2SYSCARD						0x30					/*	系统卡								*/
#define S2NOSYSCARD						0x31					/*	非系统卡							*/
/*********************************************************************************************************
  读卡器响应预处理返回的卡类型,卡状态
*********************************************************************************************************/
#define CUSTOMCARD						0x01			 		/*	用户卡								*/
#define KEYCARD							0x02					/*	密匙卡								*/
#define TESTCARD						       0x11					/*	功能测试卡							*/
#define SETCARD							0x41					/*	设置卡								*/
#define ADMINCARD						0x51					/*	员工卡								*/
#define RECHARGEABLECARD				0x61					/*	充值卡								*/
#define CANCHARGE						0x30					/*	可以充电							*/
#define CANENDCHARGE					0x31					/*	可以结束充电						*/
#define CANREPAY						0x32					/*	可以补充交易						*/
/*********************************************************************************************************
  弹卡返回状态
*********************************************************************************************************/
#define PSTATEOUTCARDSUCCESS			0x30					/*	弹卡成功							*/
#define PSTATEOUTCARDFIAL				0x31					/*	弹卡失败							*/
/*********************************************************************************************************
  补充交易返回状态
*********************************************************************************************************/
#define REPAYSUCCESS			        0x59					/*	补充交易成功							*/
#define REPAYFIAL				        0x4e					/*	补充交易失败							*/


/*********************************************************************************************************
  解析结果
*********************************************************************************************************/	



#define OPERATERFIAL					0x0000					/*	读卡操作失败						*/
#define FINDCARDSUCCESS					0x1011					/*	寻卡成功							*/
#define FINDNOCARD						0x1000					/*	未发现任何卡						*/
#define FINDNOSYSCARD					0x1010					/*	有卡，但不是系统卡					*/
#define PRETREATSUCCESS					0x2111					/*	预处理成功							*/
#define PRETREATFIAL					0x2000					/*	预处理失败							*/
#define VERYBUSY						0x2001					/*	系统忙，有人在充电					*/
#define CUSCANCHARGE					0x2010					/*	用户卡且可以充电					*/
#define CUSCANENDCHARGE					0x2011					/*	用户卡且可以结束充电				*/
#define CUSCANREPAY						0x2012					/*	用户卡且可以补充交易				*/
#define FINDNEWKEY						0x2020					/*	发现密钥修改卡						*/
#define FINDTESTCARD					0x2030					/*	发现功能测试卡						*/
#define FINDSETCARD						0x2040					/*	发现设置卡							*/
#define FINDADMINKCARD					0x2050					/*	发现管理员卡						*/
#define FINDRECHARGECARD				0x2060					/*	发现充值卡							*/
#define STARTCHARGESUCCESS				0x3111					/*	加电成功							*/
#define STARTCHARGEFIAL					0x3000					/*	加电失败							*/
#define ENDCHARGESUCCESS				0x4111					/*	结束加电成功						*/
#define ENDCHARGEFIAL					0x4000					/*	结束加电失败						*/
#define OUTCARDSUCCESS					0x5111					/*	弹卡成功							*/
#define OUTCARDFIAL						0x5000					/*	弹卡失败							*/
#define UPDATAKEYSUCCESS				0x6111					/*	更新成功							*/
#define UPDATAKEYFIAL					0x6010					/*	更新失败							*/
#define INITCARDSUCCESS					0x7111					/*	初始化卡成功						*/
#define INITCARDFIAL					0x7010					/*	初始化卡失败						*/
#define OPERATERIVALD					0x8000					/*	不操作		*/
#define UNLOCKFAILED	                0x9000					/*  解锁卡失败*/
#define LOCKCARD	                    0xA000					/*  卡已锁，须到服务中心解锁卡*/

#define PSAMRESETFAIL                   0xB000                  //PSAM复位失败
#define CARDRESETFAIL                   0xC000                  //用户卡、密钥卡、设置卡复位失败
 

#define RES_TRUE                        1
#define RES_FAULSE                    0

typedef struct 
{

	ST_UCHAR CardSeria[CARDSERIALEN];           //卡号	
	ST_UCHAR CardMoney[4];                     //卡中金额
	ST_FLOAT f_CardMoney; 
	ST_UCHAR CardType;                        //卡类型
	ST_UCHAR IsSystemCard;                   // 是否是系统卡
	ST_UCHAR CardStatus;                     //卡状态
	ST_UCHAR SendCout;                         //记录发送次数	      
	ST_UCHAR Sendcmd;
	ST_UCHAR ChargeMoney[4];
	ST_UCHAR ChargeEle[4];
	
	ST_INT32 Responsedata;
	ST_UCHAR bBusyNow;											// 是否充电中

	ST_INT32 iDealCounts;										// 购电次数
	
	ST_UCHAR byPiceChangeTime[5];								// 备用费率电价单价切换时间
	ST_UCHAR byPresentDealPice1[3];								// 当前交易费率1电量单价
	ST_UCHAR byPresentDealPice2[3];								// 当前交易费率2电量单价
	ST_UCHAR byPresentDealPice3[3];								// 当前交易费率3电量单价
	ST_UCHAR byPresentDealPice4[3];								// 当前交易费率4电量单价
	ST_UCHAR byPresentDealPice5[3];								// 当前交易费率5电量单价
	ST_UCHAR byPresentDealPice6[3];								// 当前交易费率6电量单价
	ST_UCHAR byPresentDealPice7[3];								// 当前交易费率7电量单价
	ST_UCHAR byPresentDealPice8[3];								// 当前交易费率8电量单价

	ST_UCHAR byReserveDealPice1[3];								// 备用交易费率1电量单价
	ST_UCHAR byReserveDealPice2[3];								// 备用交易费率2电量单价
	ST_UCHAR byReserveDealPice3[3];								// 备用交易费率3电量单价
	ST_UCHAR byReserveDealPice4[3];								// 备用交易费率4电量单价
	ST_UCHAR byReserveDealPice5[3];								// 备用交易费率5电量单价
	ST_UCHAR byReserveDealPice6[3];								// 备用交易费率6电量单价
	ST_UCHAR byReserveDealPice7[3];								// 备用交易费率7电量单价
	ST_UCHAR byReserveDealPice8[3];								// 备用交易费率8电量单价

}CardTP;

typedef struct
{
	ST_UCHAR CardSeria[CARDSERIALEN];
	ST_UCHAR RandomNumber[4];
	ST_UCHAR OnLineDealSeq[2];
	ST_UCHAR Left[4];
	ST_UCHAR ChargeMoney[4];
	ST_UCHAR DealFlag;
	ST_UCHAR PaleNumber[6];
	ST_UCHAR EndChargeTime[7];
	ST_UCHAR TuojiDealSeq[2];
}RepayCardMsgTP;



extern CardTP CardMsg;
void CardInit(void);

ST_INT32 TxToCardEndChargeCmd(void);
ST_UCHAR TxToCardStartChargeCmd(void);
ST_UCHAR TxToCardModifyKeyCmd(void);
ST_UCHAR OutCard(void);
#endif
