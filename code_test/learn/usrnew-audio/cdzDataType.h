
#ifndef __CDZPROTOCOLDATA__
#define __CDZPROTOCOLDATA__

#include <time.h>
#include "DataType.h"
#include "convertion.h"

#define INVALID_HANDLE_VALUE    -1

#define CDZ_MEA_POINT_MAX		64		
#define CDZ_MEA_POINT_NUM		4		

#define CDZ_PROGRAM_LOG_MAX		256		


#define CDZ_TERM_MEA_POINT		0		
#define CDZ_AC_METER_MEA_POINT	1		
#define CDZ_DC_METER_MEA_POINT	2		
#define CDZ_CHARGE_MEA_POINT	3		

#define	CDZ_OPT_RO				0x01	
#define CDZ_OPT_WO				0x02	
#define CDZ_OPT_RW				(CDZ_OPT_RO|CDZ_OPT_WO)	


#define CDZ_AUTH_NO				0x00	
#define CDZ_AUTH_LOW			0x01	
#define CDZ_AUTH_HIGH			0x02	
#define CDZ_AUTH_HIGHEST		0x04	
		

typedef struct __CDZ_DATA_ITEM__
{
	ST_UINT16 IdentCode;				
	ST_UCHAR *pData;					
	ST_UCHAR Atrrib;					
	ST_UCHAR AuthLevel;					
	ST_UINT16 Lenth;					
	ST_UINT16 DataItemCount;			
}CDZ_DATA_ITEM;


typedef struct __CDZ_TERM_PARA__
{
	ST_UCHAR StationCommAdd[9];			
	ST_UCHAR ResStationCommAdd1[9];		
	ST_UCHAR ResStationCommAdd2[9];		
	ST_UCHAR DefaultGateWayAdd[8];		
	ST_UCHAR APN[16];					
	ST_UCHAR TermDistCode[2];			
	ST_UCHAR TermAdd[2];				
	ST_UCHAR HeartBeatInter;			
	ST_UCHAR EnterUserName[32];			
	ST_UCHAR EnterPassWord[32];			
	ST_UCHAR TCPUDPIdentCode;			

	ST_UCHAR CommPass[3];				
	ST_UCHAR SetPass[3];				
	ST_UCHAR AdminPass[3];				
	ST_UCHAR TermReset;					

	ST_UCHAR TermTime[6];				
	ST_UCHAR TimeRule[4];				
	ST_UCHAR AlarmMask[16];				
	ST_UCHAR IOverTime;					
	ST_UCHAR IOverReturnTime;			
	ST_UCHAR DiConfig;					
    ST_UCHAR StartParam[3];				

	ST_UCHAR PhaseFaultTime;			
	ST_UCHAR PhaseFaultReturnTime;		
    

	ST_UCHAR CurrentTaskNum;			
	ST_UCHAR TaskNo[254];				
	ST_UCHAR TaskStatus[32];			

	ST_UCHAR TermSoftVer[8];			
	ST_UCHAR TermHardWareVer[2];		

	ST_UCHAR SlaveTermAddr1[16];		
	ST_UCHAR SlaveTermAddr2[16];		
	ST_UCHAR SlaveTermAddr3[16];		
	ST_UCHAR SlaveTermAddr4[16];		
	
	ST_UCHAR ExchangeType;				
	ST_UCHAR CityCode[3];				
	ST_UCHAR ParkFee[3];				
	ST_UCHAR KcParkFeeFlag;				
	ST_UCHAR HostWaitTime;				
	ST_UCHAR SlaveWaitTime;				
	ST_UCHAR SlaveNo[5];				

	ST_UCHAR ChargeManCardNo1[8];		
	ST_UCHAR ChargeManCardNo2[8];		
	ST_UCHAR ChargeManCardNo3[8];		
	ST_UCHAR ChargeManCardNo4[8];		
	ST_UCHAR ChargeManCardNo5[8];		
	ST_UCHAR ChargeManCardNo6[8];		
	ST_UCHAR ChargeManCardNo7[8];		
	ST_UCHAR ChargeManCardNo8[8];		
	ST_UCHAR ChargeManCardNo9[8];		
	ST_UCHAR ChargeManCardNo10[8];		
	
	ST_UCHAR FaultRepairCard1[8];		
	ST_UCHAR FaultRepairCard2[8];		
	ST_UCHAR FaultRepairCard3[8];		
	ST_UCHAR FaultRepairCard4[8];		
	ST_UCHAR FaultRepairCard5[8];		
	ST_UCHAR FaultRepairCard6[8];		
	ST_UCHAR FaultRepairCard7[8];		
	ST_UCHAR FaultRepairCard8[8];		
	ST_UCHAR FaultRepairCard9[8];		
	ST_UCHAR FaultRepairCard10[8];		

	ST_UCHAR TermUseFlag;				
	ST_UCHAR LowMoneyAnn[3];			

	ST_UCHAR CellFlag[32];				
	ST_UCHAR CellType[32];				
	ST_UCHAR CellParam[32];				

	ST_UCHAR CurChargeFee1[3];			
	ST_UCHAR CurChargeFee2[3];			
	ST_UCHAR CurChargeFee3[3];			
	ST_UCHAR CurChargeFee4[3];			
	ST_UCHAR CurChargeFee5[3];			
	ST_UCHAR CurChargeFee6[3];			
	ST_UCHAR CurChargeFee7[3];			
	ST_UCHAR CurChargeFee8[3];			

	ST_UCHAR ResChargeFee1[3];			
	ST_UCHAR ResChargeFee2[3];			
	ST_UCHAR ResChargeFee3[3];			
	ST_UCHAR ResChargeFee4[3];			
	ST_UCHAR ResChargeFee5[3];			
	ST_UCHAR ResChargeFee6[3];			
	ST_UCHAR ResChargeFee7[3];			
	ST_UCHAR ResChargeFee8[3];			

	ST_UCHAR ResChargeFeeSwitchTime[5];	
	ST_UCHAR GasPrice[3];

}CDZ_TERM_PARA;


typedef struct __CDZ_MEA_POINT_PARA__
{
	ST_UCHAR MeaPointStatus;			
	ST_UCHAR MeaPointAttrib;			
	ST_UCHAR MeaPointAddr[6];			
	ST_UCHAR MeaPointProtocol;			
	ST_UCHAR MeaPointNo;				
	ST_UCHAR MeaPointBaud;				
	
	ST_UCHAR TermVoltage[2];			
	ST_UCHAR RateCurrent[2];			

	ST_UCHAR RateCurrentGFH[2];			
	ST_UCHAR RateCurrentGFHRetrun[2];	

	ST_UCHAR Parity;					
	ST_UCHAR Databit;					
	ST_UCHAR Stopbit;					
	ST_UCHAR Password[8]; 	            
	ST_UCHAR User[8];					

	ST_UCHAR RateGY[2]; 	            
	ST_UCHAR RateGYReturn[2];			
	ST_UCHAR RateQY[2]; 	            
	ST_UCHAR RateQYReturn[2];			

	ST_UCHAR RateChargeCurrentMix[2];	
	ST_UCHAR OutputMaxCurrent[2];		
}CDZ_MEA_POINT_PARA;



typedef struct __CDZ_MEA_POINT_DATA__
{
	ST_UCHAR TotalP[4]; 	           
	ST_UCHAR TotalP1[4]; 	           
	ST_UCHAR TotalP2[4]; 	           
	ST_UCHAR TotalP3[4]; 	           
	ST_UCHAR TotalP4[4]; 	           

	ST_UCHAR VoltageA[4]; 	           
	ST_UCHAR VoltageB[4]; 	           
	ST_UCHAR VoltageC[4]; 	           

	ST_UCHAR CurrentA[4]; 	           
	ST_UCHAR CurrentB[4]; 	           
	ST_UCHAR CurrentC[4]; 	           

	ST_UCHAR Date[4];					
	ST_UCHAR Time[3];					

	ST_UCHAR OutPutVoltage[2];			
	ST_UCHAR OutPutCurrent[3];			
	ST_UCHAR TermFaultStatus[4];		
	ST_UCHAR ACSwitchStatus;			
	ST_UCHAR DCSwitchStatus;			

	ST_UCHAR CurExchangeSeq[5];			
	ST_UCHAR CurStartCardNo[8];			
	ST_UCHAR CurExchangeEnerge[4];		
	ST_UCHAR CurExchangeEnergeFee1[4];	
	ST_UCHAR CurExchangeEnergeFee2[4];	
	ST_UCHAR CurExchangeEnergeFee3[4];	
	ST_UCHAR CurExchangeEnergeFee4[4];	
	ST_UCHAR CurExChangeStartTime[6];	
	ST_UCHAR CurExChangeEndTime[6];		
	ST_UCHAR CurParkFee[3];				
	ST_UCHAR CurBeforeExChangeMoney[4];
	ST_UCHAR CurAfterExChangeMoney[4];	
	ST_UCHAR CurCardVerNo[5];			
	ST_UCHAR CurPosNo[5];				
	ST_UCHAR CurCardStatus;				

	ST_UCHAR CurChargeStatus[2];		
	ST_UCHAR CurStartType[2];			

	ST_UCHAR LastExchangeSeq[5];		
	ST_UCHAR LastStartCardNo[8];		
	ST_UCHAR LastExchangeEnerge[4];		
	ST_UCHAR LastExchangeEnergeFee1[4];
	ST_UCHAR LastExchangeEnergeFee2[4];
	ST_UCHAR LastExchangeEnergeFee3[4];
	ST_UCHAR LastExchangeEnergeFee4[4];
	ST_UCHAR LastExChangeStartTime[6];	
	ST_UCHAR LastExChangeEndTime[6];	
	ST_UCHAR LastParkFee[3];			
	ST_UCHAR LastBeforeExChangeMoney[4];
	ST_UCHAR LastAfterExChangeMoney[4];
	ST_UCHAR LastCardVerNo[5];			
	ST_UCHAR LastPosNo[5];				
	ST_UCHAR LastCardStatus;			

	ST_UCHAR LastEndCardNo[8];			
	ST_UCHAR LastStartCardType[2];		
	ST_UCHAR LastEndCardType[2];		

	ST_UCHAR CellGroupVoltage[3];		
	ST_UCHAR CellGroupChargeCurrent[3];
	ST_UCHAR CellGroupChargePower[3];	
	ST_UCHAR CellGroupChargeTime[4];	
	ST_UCHAR CellGroupChargeEnerge[4];	
	ST_UCHAR CellVoltage[3];			
	ST_UCHAR CellAh[4];					
	ST_UCHAR CellGroupTemp[3];			
	ST_UCHAR ChargeOutputVoltage[3];	
	ST_UCHAR ChargeOutputCurrent[3];	
	ST_UCHAR ChargeTemp[3];				

	ST_UCHAR CellGroupStatus;			
	ST_UCHAR CellGroupFaultCode;		
	ST_UCHAR ChargeStatus; 	           
	ST_UCHAR ChargeFaultCode;			
	ST_UCHAR ChargeACSwitchStatus;		
	ST_UCHAR ChargeDCSwitchStatus;		
	ST_UCHAR ChargeDCSwitchTrip;		
	ST_UCHAR WatchFault;				
	ST_UCHAR WatchCommFault;			
	ST_UCHAR ChargeOnOff;				
	ST_UCHAR ChargeStop;				
}CDZ_MEA_POINT_DATA;


typedef struct __CDZ_PROGRAM_DATA__
{
	ST_UINT16 IdentCode;				
	ST_UINT16 DataLen;					
	ST_UCHAR *pOldDataContent;			
	ST_UCHAR *pNewDataContent;			
}CDZ_PROGRAM_DATA;


typedef struct __CDZ_PROGRAM_LOG__
{
	ST_UINT16 sMeaPointNo;				
	struct tm sProgramTime;				
	CDZ_PROGRAM_DATA pcdzProgramData;	
}CDZ_PROGRAM_LOG;



ST_UINT16 cdzGetDataItemLenth(const ST_UCHAR *cdzMeaPointFlag, ST_UINT16 cdzIdentCode);


ST_UINT16 cdzGetDataItemLenthMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode);

ST_INT16 cdzReadDataItemContent(const ST_UCHAR *cdzMeaPointFlag, ST_UINT16 cdzIdentCode,
							   ST_UCHAR *sDataItemContent);

ST_INT16 cdzReadDataItemContentMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   ST_UCHAR *sDataItemContent);

ST_RET cdzWriteDataItemContentAuth(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   const ST_UCHAR *sDataItemContent, ST_UCHAR sAuthLevel, 
							   const ST_UCHAR *sPassword);


ST_RET cdzWriteDataItemContentNoAuth(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   const ST_UCHAR *sDataItemContent);

ST_RET cdzLoadParam(void);

void cdzReverseBytes(const ST_UCHAR *sSrc, ST_UCHAR *sDst, ST_UINT16 sLen);
ST_INT16 cdzReadDataItemContentMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   ST_UCHAR *sDataItemContent);

ST_INT16 cdzReadDataItemContentMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   ST_UCHAR *sDataItemContent);

#endif
