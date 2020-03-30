#ifndef _PUBLIC_H_
#define _PUBLIC_H_
#include "DataType.h"



typedef struct
{
	ST_CHAR PreChargeTime[2];   //通过界面设定的预充时间。PreChargeTime[0]：分；PreChargeTime[1]时
	ST_FLOAT SetChargeEle;     //按电量充电时，设定的充电电量
	ST_FLOAT SetChargeMoney;   //按金额充电时，设定的充电金额
	ST_CHAR SetChargeTime[3];  //按时间充电时，设定的充电时间,0:秒；1:分，2：时
	ST_CHAR SetChargeMode;     //用户选择的充电模式，1：按电量充电；2：按金额充电；3：按时间充电；4：自动充满，5：查询余额。
	ST_CHAR SetStartChargeMode;//用户选择的启动充电模式。1：预约充电；0：立即充电

}SetChargeParaTP;

typedef struct
{
	ST_FLOAT CardBalance;//充电前卡中的余额，精确到分，小数点后两位。
	ST_FLOAT MaxChargeEle;//当前余额最多可充电量度数，同样精确到小数点后两位。
	ST_CHAR  CardNo[8];//当前充电卡卡号
	ST_FLOAT ChargedEle;//已充电量度数
	ST_FLOAT ChargingEle;//未充电量度数
	ST_FLOAT ChargedMoney;//已充金额
	ST_FLOAT ChargingMoney;//未充金额
	ST_CHAR  ChargedTime[3];//已充时间，0-2分别为：秒、分、时
	ST_CHAR  ChargingTime[3];//未充时间，0-2分别为：秒、分、时
	ST_UCHAR CurrentPage;//当前的界面号，每次界面跳转时，都需给该变量赋值。
	ST_UCHAR LastPage;//记录上一个界面号。 
	ST_FLOAT LastChargeEle;//上次充电电量
	ST_FLOAT LastChargeMoney;//上次充电金额
	ST_CHAR  LastChargeEndTime[6];//上次充电结束时间,0-5分别为秒、分、时、日、月、年
	ST_CHAR  FindLastChargeRec;  //充电卡状态标志，0：卡为正常解锁状态；1：卡为锁定状态，但在桩中找到充电记录，对应界面45；2：卡为锁定状态，但在桩中未找到充电记录。对应界面47
	
	ST_CHAR SysTime[6];//存放主板系统时间，0-5分别为秒、分、时、日、月、年	 

}DisplayInfoTP;

typedef struct 
{
	ST_CHAR IsExistCardFlag; 
	ST_CHAR CardExcepFlag;
	ST_CHAR MeterExcepFlag;  
	ST_CHAR IsPatCardFlag;		// pat card 1, not pat card 0 刷卡标志： 刷卡，1； 未刷卡，0.
	//ST_CHAR IsChargeQiang;  
	ST_CHAR QiangConGood;//插头连接好的标志，0：未连接好；1：连接好
	ST_CHAR StartChargeFlag;  //开始充电标记0:未充电状态，1：启动充电， 3:刷卡后，进入预约充电等待界面
	ST_CHAR CutdownEleFlag;  
	ST_INT32 SoundTipFlag;
	ST_CHAR ChargeFullFlag;
	ST_CHAR ChargeFullNomoneyNoPulgFg;//1：充电过程中，余额不足标志；0：充电过程中余额充足
	ST_CHAR ChargeExcepEndFlag;
	ST_CHAR ChongdianCangLedOnFg;
	ST_CHAR Repair_Or_Manage_CardEndChargeFg;
	ST_CHAR RepairDoorOpenFg;
	ST_CHAR  NoSameCardFlag; //1：显示非同一张卡，	
	ST_CHAR OpenDoorFlag;   //1：通知主控开门；0:默认状态
	ST_CHAR CloseDoorFlag   //1:通知主控关门：0：默认状态
	ST_CHAR ChargeCompleteFlag; //充电结束标志 1：充电完成。0：未完成
	
}EventFlagTP;

/*********************************************************************************************************************************

flag使用说明：

IsPatCardFlag：






***********************************************************************************************************************************/







#endif
