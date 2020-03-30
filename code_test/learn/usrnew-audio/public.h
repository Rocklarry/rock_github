#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include "DataType.h"

#define DEPOSIT         0                  //卡最低保底金额

#define RELAY_OFF       1
#define RELAY_ON        0

#define DEBUG_DB

#if defined(DEBUG_DB)
#define dbg(fmt, args...) printf(fmt, ##args)
#else
#define dbg(fmt, args...) do {} while(0)
#endif



typedef struct
{
	ST_CHAR PreChargeTime[2];   //通过界面设定的预充时间。PreChargeTime[0]：分；PreChargeTime[1]时1111
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
	ST_INT16 LastChargeEndTime[6];//上次充电结束时间,0-5分别为秒、分、时、日、月、年
	ST_CHAR  FindLastChargeRec;  //充电卡状态标志，0：卡为正常解锁状态；1：卡为锁定状态，但在桩中找到充电记录，对应界面45；2：卡为锁定状态，但在桩中未找到充电记录。对应界面47
	
	ST_INT16 SysTime[6];//存放主板系统时间，0-5分别为秒、分、时、日、月、年	 
	ST_CHAR NodeNo[8];					// Charge Node No
	ST_FLOAT Rated_Voltage;				// Rated Voltage
	ST_FLOAT Rated_Current;				// Rated Current
}DisplayInfoTP;



#define LED_ON_TIME_FROM                 6
#define LED_ON_TIME_TO                   19


#if 1

#define PW_CHECK_CP0                     149
#define PW_CHECK_CP1                     150  
#define PW_CHECK_CC0                     117
#define PW_CHECK_CC1                     118 
#define PW_SACLE                         178
#define CREATE_IO   "/sys/class/gpio/export"

#define IN0_145_FangLeiQi                 107 // PC11    145 
#define IN1_111_JianXiuMen                106 // PC10    111 
#define IN2_109_ChaZuoMen                 103 // PC7     109 
#define IN3_108_JieChuQi                  102 // PC6     108 
#define IN4_107_DuanDian                  145 // PD17    107 
#define IN5_106                           111 // PC15    106 
#define IN6_109                           109 // PC13    103 
#define IN7_102_JiTingBution              108 // PC12    102 



#define OUT5_DengXiangLed                  58 //k2  PA26    灯箱

#define OUT1_QiangLed                       41 //k1 PA9     双色灯

#define OUT6_ChargeLed                      40  //k6 PA8      红


#define OUT1_41_ChongDianQiang              191  // k4 PE31      

#define OUT7_IdleLed                        38  //k7 PA6       绿


#define OUT4_JieChuQi                       39  //k3 PA7      接触器

#define OUT4_38_JieChuQiOn                  38  //k7 PA6
#define OUT5_191_JieChuQiOff                191 //k4 PE31


#define OUT8_Excep_ChargeFull_led           156  //97  k8 //PD28    黄   


#define OUT0_58_DianCiSuo                   158    //96  k5//PD30      灯箱 电插锁

#define OUT_PWM                             57



#define OUT8_62_ChargeLed                   60
#define OUT9_63_ExceptionLed                63
#define OUT_56_ScreenLed                    56 //PA24

#endif

#if 0

#define PW_CHECK_CP0                     149
#define PW_CHECK_CP1                     150  
#define PW_CHECK_CC0                     117
#define PW_CHECK_CC1                     118 
#define PW_SACLE                        178
#define CREATE_IO   "/sys/class/gpio/export"

#define IN0_145_FangLeiQi                 107 // PC11    145 
#define IN1_111_JianXiuMen                106 // PC10    111 
#define IN2_109_ChaZuoMen                 103 // PC7     109 
#define IN3_108_JieChuQi                  102 // PC6     108 
#define IN4_107_DuanDian                  145 // PD17    107 
#define IN5_106                           111 // PC15    106 
#define IN2_109                           109 // PC13    103 
#define IN7_102                           108 // PC12    102 

#define OUT0_58_DianCiSuo                   58 //k2  PA26
#define OUT2_40_LogoLed                     41  //k1 PA9

#define OUT3_39_DengXiang                   40  //k6 PA8

#define OUT1_41_ChongDianQiang              39  //k3 PA7

#define OUT4_38_JieChuQiOn                  38  //k7 PA6
#define OUT5_191_JieChuQiOff                191 //k4 PE31
#define OUT6_97                             97 //   PC1               
#define OUT7_96                             96  //PC0
#define OUT_PWM                             57

#define OUT8_62_ChargeLed                   60
#define OUT9_63_ExceptionLed                63
#define OUT_56_ScreenLed                    56 //PA24

#endif



#define PALEIDLE                            0
#define PALECHARGING                        1
#define PALEEXCEPTION                       2 
#define PALE_NO_USE                         4

#define  RATEDVOL                           220		                                
#define  RATEDCUR                           16		                  
#define  CHARGEMONEYLIMIT 	                15		                      

#define VOL_HI_LIMIT                        253
#define VOL_LOW_LIMIT                       200
#define CUR_HI_LIMIT                        20

#define ACVoltNormal                        1
#define ACVoltLow                           2 
#define ACVoltHigh                          3




#define  PUSHPLUGTIMELIMIT                  60           
#define  CHARGEFULLTIMELIMIT                60           
#define  CHARGEFULLCURLIMIT	                0       


//以下是显示界面的页面号定义

#define DISP_IDLE                             10	           //待机画面，可选择立即充电和预约充电               

#define DISP_CHARGEMODESEL	                  11               //充电模式选择界面           
#define DISP_BRUSHCARD                        12	           //提示刷卡界面          
                  
#define DISP_QUERYBANLANCE                    13               //查询余额界面
#define DISP_CHARGE_ELE_SEL                   14               //按电量充电界面

#define DISP_CHARGE_MON_SEL                   15               //按金额充电界面

#define DISP_CHARGE_TIME_SEL                  16               //按时间充电界面

            
#define DISP_CONNECT_PLUG                     17               //提示连接插头界面                     
#define DISP_BALANCE_NOT_ENOUGH               18               //提示输入参数过大，余额不足界面            
#define DISP_INPUTERR_BACK                    19			   //输入错误超过三次，提示拔出充电插头
#define DISP_CHARGING_ELE                  	  20               //开始充电界面，按电量充电     
#define DISP_ENDCHARGE                        21               //结束充电界面                 

#define DISP_PULLOUT_PLUG                     22               //提示拔出插头界面
#define DISP_PRINT_EXCEP                      23               //打印机故障界面
#define DISP_PRINT_NOPAPER                    24               //打印机没纸界面
        
#define DISP_THANK                            25               //谢谢使用界面
 
#define DISP_NOT_SAME_CARD                    26               //非同一张卡
#define DISP_EXCEPENDCHARGE_ELE               27               //充电过程中故障，按电量充电
#define DISP_EXCEPENDCHARGE_MON               28               //充电过程中故障，按金额充电
#define DISP_EXCEPENDCHARGE_TIME              29               //充电过程中故障，按时间充电
#define DISP_EXCEPENDCHARGE_FULL              30               //充电过程中故障，自动充满
#define DISP_BRUSHCARD_EXIT                   31               //有退出按钮的请刷卡界面

         

#define DISP_FINISHCHARGE_ELE                 33               //已充满，按电量充电
#define DISP_PRECHARGE_INPUT                  35			   //预充电时间输入界面
#define DISP_PALEINFO                         36               //查询桩信息界面  
#define DISP_CHARGE_ELE_INPUT                 37               //按输入电量充电界面
#define DISP_CHARGE_MON_INPUT                 38               //按金额充电输入界面
#define DISP_CHARGE_TIME_INPUT                39               //按时间充电输入界面
#define DISP_CHARGE_FULL_ELE                  40               //充满_按电量
#define DISP_CHARGE_FULL_MON                  41              //充满_按金额
#define DISP_CHARGE_FULL_TIME                 42              //充满_按时间
                                                               

#define DISP_PAY_LAST_FEE                     45               //刷卡后支付上次充电费用
#define DISP_WAIT_CHARGE                      46               //设定好预充电时间后，等待充电界面

#define DISP_CARDLOCK                         47              //充电刷卡时卡片已锁
                                                               
#define DISP_CHARGING_MONEY                   48              //开始充电界面，按金额充电
#define DISP_CHARGING_TIME                    49              //开始充电界面，按时间充电
#define DISP_CHARGING_ZIDONG                  58              //开始充电界面，自动充满 

#define DISP_OPENDOOR_BF                      59              //打开充电仓门，充电前

#define DISP_FINISHCHARGE_MON                 66              //充电完成，按金额充电

#define DISP_FINISHCHARGE_TIME                67              //充电完成，按时间充电
#define DISP_FINISH_ZIDONG                    68              //已充满，自动充满模式

#define DISP_CLOSEDOOR_BF                     80             //关闭插座门，充电前
#define DISP_OPENDOOR_AF                      81              //打开充电仓门，充电后
#define DISP_CLOSEDOOR_AF                     82             //关闭插座门，充电后
#define DISP_NOMONEY                          83             //余额不足，请及时充值
#define DISP_INPUT_AGAIN                      84             //余额不足，请重新输入
#define DISP_INPUT_ERROR                      85             //输入错误次数过多

#define DISP_NOMONEY_CHARING_ELE              86             //充电过程中余额不足，按电量充电
#define DISP_NOMONEY_CHARGING_MON             87             //充电过程中余额不足，按金额充电
#define DISP_NOMONEY_CHARGING_TIME            88            //充电过程中余额不足，按时间充电
#define DISP_NOMONEY_CHARGING_ZIDONG          89             //充电过程中余额不足，自动充满

#define DISP_NOPAPLE_IDLE                     90             //首页无纸提示

#define DISP_EXCEP_IDLE                       200              //充电桩故障，暂停服务
#define DISP_EXCEP_IDLE_A                     202              //充电桩故障，暂停服务
#define DISP_EXCEP_IDLE_B                     203              //充电桩故障，暂停服务





#define DISPIDLE                              1	                          
#define DISPCLODEDOOR                         2				
#define DISPCHARGING	                      3	                           
#define DISPENDCHARGE                         4	                        
#define DISPENDCLOSEDOOR                      5                                  
#define DISPEXCEPENDCHARGE                    6  
#define DISPNOCHARGECARD                      7               
#define DISPTHANK                             8                                    
#define DISPEXCEPIDLE                         9                           
#define DISPCHARGEFULL                        10				
#define DISPBALANCENOTENOUGH                  11                        
#define DISPNOMONEY                           12                               

#define DISPPAYLASTFEE                        13
#define DISPCARDLOCK                          14          
#define DISPNOSYSTEMCARD                      15          
        
#define DISPCURENTBANLANCE                    17   



#define FUNCTIONTEST                          18                     
#define TESTOPENDOOR                          19                  
#define TESTCONCHATOU                         20                  
#define TESTCHARGING                          21                 
#define TESTCLOSEDOOR                         22                
#define TESTCOMPELETE                         23              
#define TESTENDCHARGE                         24           
#define TESTCARDINCHARGE                      25            



#define MANAGECARDUNUSE                       26        


#define FINDREPAIRCARD                        27       
#define REPAIRCARDUNUSE                       28      



#define MODIFYKEYSUCESS                       29
#define MODIFYKEYFAIL                         30
#define MODIFYCARDINCHARGE                    31
#define CHARTOUCONNECTING                     32
#define EXCEPTENDCHARGECHECKCHAROU            33    //异常结束充电后，当异常恢复时，检测插头连接
#define STARTCHARGEFAILED                     34    //发送开始充电命令失败后，不能启动充电，界面提示
#define ENDCHARGEREPAYFAILEDTIP               35    //补充交易失败后，卡已锁，提示到服务中心解锁
#define ENDCHARGEFAILETIP                     36    //当发送结束加电命令失败后，提示可以下次充电结算本次费用
#define FINDCARDREPAYFAILED                   37    //当补充交易失败，卡已锁定后，再次检测到卡时提示用户解锁卡
#define PAIDLASTMONEYFAILED                   38    // 当扣除上次充电费用失败，但卡并为锁的情况下，提示再次插入卡解锁

#define METER_EXCEPTION_CHARGING              39    //电表通信故障.充电已结束，请插卡付费
#define CARD_EXCEPTION_CHARGING               40    //读卡器通信故障, 充电已结束，禁止插卡操作
#define REPAIRDOOROEPN_CHARGING               41    // 维修门打开, 充电已结束，请插卡付费
#define REPAIRDOOROEPN_IDLE                   42    // 桩在空闲状态下，维修门打开，提示禁止充电
#define METER_EXCEPTION_IDLE                  43    // 桩在空闲状态下，表通信故障，提示暂停服务
#define CARD_EXCEPTION_IDLE                   44    // 桩在空闲状态下，卡通信故障，提示暂停服务

#define DISPDUANDIAN_CHARGING_F               45    //桩在充电过程中，突然断电提示,峰时段充电
#define DISPDUANDIAN_CHARGING_P               46    //桩在充电过程中，突然断电提示，平时段充电
#define DISPDUANDIAN_CHARGING_G               47    //桩在充电过程中，突然断电提示,谷时段充电

#define METER_EXCEPTION_CHARGING_P            48    //电表通信故障.充电已结束，请插卡付费,平时段
#define METER_EXCEPTION_CHARGING_G            49    //电表通信故障.充电已结束，请插卡付费,谷时段

#define CARD_EXCEPTION_CHARGING_P             50    //读卡器通信故障, 充电已结束，禁止插卡操作，平时段
#define CARD_EXCEPTION_CHARGING_G             51    //读卡器通信故障, 充电已结束，禁止插卡操作，谷时段

#define REPAIRDOOROEPN_CHARGING_P             52    // 维修门打开, 充电已结束，请插卡付费,平时段
#define REPAIRDOOROEPN_CHARGING_G             53    // 维修门打开, 充电已结束，请插卡付费，谷时段

#define ENDCHARGEREPAYFAILEDTIP_P             54    //补充交易失败后，卡已锁，提示到服务中心解锁,平时段
#define ENDCHARGEREPAYFAILEDTIP_G             55    //补充交易失败后，卡已锁，提示到服务中心解锁，谷时段

#define PAIDLASTMONEYFAILED_P                 56    // 当扣除上次充电费用失败，但卡并为锁的情况下，提示再次插入卡解锁
#define PAIDLASTMONEYFAILED_G                 57    // 当扣除上次充电费用失败，但卡并为锁的情况下，提示再次插入卡解锁

#define DISPCHARGING_P                        58    //正在充电，平时段
#define DISPCHARGING_G                        59    //正在充电，谷时段

#define DISPENDCHARGE_P                       60    //充电终止，平时段	
#define DISPENDCHARGE_G                       61    //充电终止，谷时段

#define DISPENDCLOSEDOOR_P                    62    //充电终止，关闭插座门
#define DISPENDCLOSEDOOR_G                    63    //充电终止，关闭插座门

#define DISPEXCEPENDCHARGE_P                  64    //充电桩异常，终止充电，平时段
#define DISPEXCEPENDCHARGE_G                  65    //充电桩异常，终止充电，谷时段

#define DISPCHARGEFULL_P                      66    //已充满，平时段
#define DISPCHARGEFULL_G                      67    //已充满，谷时段

#define DISPBALANCENOTENOUGH_P                68    //余额不足，终止充电，平时段   
#define DISPBALANCENOTENOUGH_G                69    //余额不足，终止充电，谷时段  

#define DISPPAYLASTFEE_P                      70    //扣除上次费用，平时段
#define DISPPAYLASTFEE_G                      71    //扣除上次费用，谷时段

#define DISPONLYCLODEDOOR                     72    //检测到插好插头后，提示关闭插座门	

#define DISPMETER_H_VOL_A_CHARGING_F          73    //充电过程中，电表A相过压，峰时段
#define DISPMETER_H_VOL_A_CHARGING_P          74    //充电过程中，电表A相过压，平时段
#define DISPMETER_H_VOL_A_CHARGING_G          75    //充电过程中，电表A相过压，谷时段

#define DISPMETER_H_VOL_B_CHARGING_F          76    //充电过程中，电表B相过压，峰时段
#define DISPMETER_H_VOL_B_CHARGING_P          77    //充电过程中，电表B相过压，平时段
#define DISPMETER_H_VOL_B_CHARGING_G          78    //充电过程中，电表B相过压，谷时段

#define DISPMETER_H_VOL_C_CHARGING_F          79    //充电过程中，电表C相过压，峰时段
#define DISPMETER_H_VOL_C_CHARGING_P          80    //充电过程中，电表C相过压，平时段
#define DISPMETER_H_VOL_C_CHARGING_G          81    //充电过程中，电表C相过压，谷时段

#define DISPMETER_H_CUR_A_CHARGING_F          82    //充电过程中，电表A相过流，峰时段
#define DISPMETER_H_CUR_A_CHARGING_P          83    //充电过程中，电表A相过流，平时段
#define DISPMETER_H_CUR_A_CHARGING_G          84    //充电过程中，电表A相过流，谷时段
                                                                           
#define DISPMETER_H_CUR_B_CHARGING_F          85    //充电过程中，电表B相过流，峰时段
#define DISPMETER_H_CUR_B_CHARGING_P          86    //充电过程中，电表B相过流，平时段
#define DISPMETER_H_CUR_B_CHARGING_G          87    //充电过程中，电表B相过流，谷时段
                                                                           
#define DISPMETER_H_CUR_C_CHARGING_F          88    //充电过程中，电表C相过流，峰时段
#define DISPMETER_H_CUR_C_CHARGING_P          89    //充电过程中，电表C相过流，平时段
#define DISPMETER_H_CUR_C_CHARGING_G          90    //充电过程中，电表C相过流，谷时段

#define DISPMETER_DUANXIANG_A_CHARGING_F      91    //充电过程中，电表A相断相，峰时段
#define DISPMETER_DUANXIANG_A_CHARGING_P      92    //充电过程中，电表A相断相，平时段
#define DISPMETER_DUANXIANG_A_CHARGING_G      93    //充电过程中，电表A相断相，谷时段

#define DISPMETER_DUANXIANG_B_CHARGING_F      94    //充电过程中，电表B相断相，峰时段
#define DISPMETER_DUANXIANG_B_CHARGING_P      95    //充电过程中，电表B相断相，平时段
#define DISPMETER_DUANXIANG_B_CHARGING_G      96    //充电过程中，电表B相断相，谷时段

#define DISPMETER_DUANXIANG_C_CHARGING_F      97    //充电过程中，电表C相断相，峰时段
#define DISPMETER_DUANXIANG_C_CHARGING_P      98    //充电过程中，电表C相断相，平时段
#define DISPMETER_DUANXIANG_C_CHARGING_G      99    //充电过程中，电表C相断相，谷时段

#define DISPMETER_DUANLU_A_CHARGING_F         100    //充电过程中，电表A相短路，峰时段
#define DISPMETER_DUANLU_A_CHARGING_P         101    //充电过程中，电表A相短路，平时段
#define DISPMETER_DUANLU_A_CHARGING_G         102    //充电过程中，电表A相短路，谷时段

#define DISPMETER_DUANLU_B_CHARGING_F         103    //充电过程中，电表B相短路，峰时段
#define DISPMETER_DUANLU_B_CHARGING_P         104    //充电过程中，电表B相短路，平时段
#define DISPMETER_DUANLU_B_CHARGING_G         105    //充电过程中，电表B相短路，谷时段

#define DISPMETER_DUANLU_C_CHARGING_F         106    //充电过程中，电表C相短路，峰时段
#define DISPMETER_DUANLU_C_CHARGING_P         107    //充电过程中，电表C相短路，平时段
#define DISPMETER_DUANLU_C_CHARGING_G         108    //充电过程中，电表C相短路，谷时段

#define DISPMETER_L_VOL_A_CHARGING_F          109    //充电过程中，电表A相欠压，峰时段
#define DISPMETER_L_VOL_A_CHARGING_P          110    //充电过程中，电表A相欠压，平时段
#define DISPMETER_L_VOL_A_CHARGING_G          111    //充电过程中，电表A相欠压，谷时段

#define DISPMETER_L_VOL_B_CHARGING_F          112    //充电过程中，电表B相欠压，峰时段
#define DISPMETER_L_VOL_B_CHARGING_P          113    //充电过程中，电表B相欠压，平时段
#define DISPMETER_L_VOL_B_CHARGING_G          114    //充电过程中，电表B相欠压，谷时段

#define DISPMETER_L_VOL_C_CHARGING_F          115    //充电过程中，电表C相欠压，峰时段
#define DISPMETER_L_VOL_C_CHARGING_P          116    //充电过程中，电表C相欠压，平时段
#define DISPMETER_L_VOL_C_CHARGING_G          117    //充电过程中，电表C相欠压，谷时段

#define DISPDUANDIAN_IDLE                     118    //空闲状态，终端断电

#define DISPMETER_H_VOL_A_IDLE                119    //充电过程中，电表A相过压，空闲状态
#define DISPMETER_H_VOL_B_IDLE                120    //充电过程中，电表B相过压，空闲状态
#define DISPMETER_H_VOL_C_IDLE                121    //充电过程中，电表C相过压，空闲状态

#define DISPMETER_H_CUR_A_IDLE                122    //充电过程中，电表A相过流，空闲状态
#define DISPMETER_H_CUR_B_IDLE                123    //充电过程中，电表B相过流，空闲状态
#define DISPMETER_H_CUR_C_IDLE                124    //充电过程中，电表C相过流，空闲状态

#define DISPMETER_L_VOL_A_IDLE                125    //充电过程中，电表A相欠压，空闲状态
#define DISPMETER_L_VOL_B_IDLE                126    //充电过程中，电表B相欠压，空闲状态
#define DISPMETER_L_VOL_C_IDLE                127    //充电过程中，电表C相欠压，空闲状态

#define DISPMETER_DUANLU_A_IDLE               128    //充电过程中，电表A相短路，空闲状态
#define DISPMETER_DUANLU_B_IDLE               129    //充电过程中，电表B相短路，空闲状态
#define DISPMETER_DUANLU_C_IDLE               130    //充电过程中，电表C相短路，空闲状态

#define DISPMETER_DUANXIANG_A_IDLE            131    //充电过程中，电表A相断相，空闲状态
#define DISPMETER_DUANXIANG_B_IDLE            132    //充电过程中，电表B相断相，空闲状态
#define DISPMETER_DUANXIANG_C_IDLE            133    //充电过程中，电表C相断相，空闲状态

#define DISPENDCHARGE_NOSAME_CHARGECARD       134    //结束充电后，使用不同的充电卡付费提示界面


#define DISPZAITING_CHARGING                  135    //充电桩暂停充电，可插卡付费
#define DISPNOTMOVE_CAR                       136    //充电即将开始，请勿移动车辆

#define DISPBATRRRY_VOL_LOW                   137    //备用电池低，暂停服务

#define DISPCHIANG_UNCONNECT_CHARGING_F       138   //充电过程中，充电枪断开连接，峰时段
#define DISPCHIANG_UNCONNECT_CHARGING_P       139   //充电过程中，充电枪断开连接，平时段
#define DISPCHIANG_UNCONNECT_CHARGING_G       140   //充电过程中，充电枪断开连接，谷时段

//add 07-23






#define CARDSERIALEN                      8

//以下是语音播放序号定义

#define AUDIO_PULLPUG                    20          //请拔掉充电枪






#define WELCOMEUSE                        1        //“欢迎使用中国南方电网公司为您提供的电动汽车充电服务”,“您当前的余额是XXXX.XX元” 
#define BANLANCENOTENOUGH                 2        //“您的余额不足，请尽快充值”                                                       
#define PUSHPLUG                          3        //“请打开充电桩插座门，插入电源插头”                                               
#define CLOSEDOOR                         4        //“请关闭插座门”                                                                   
#define NOMOVETHECAR                      5        //“充电即将开始。请勿移动车辆”                                                     
#define BEGINECHARGE                      6        //“开始进行充电”                                                                   
#define ENDCHARGING                       7        //“充电结束”                                                                       
#define CURRENTPAID                       8        //“本次消费金额XXXX.XX元”                                                          
#define CURRENTBANLANCE                   9        //“您当前的余额是XXXX.XX元”                                                        
#define TAKEOUTCARD                       10       //“请取卡”                                                                         
#define THANKBYE                          11       //“谢谢使用，再见”                                                                 
#define NOTSAMECARD                       12       //“卡号不正确，请使用同一张卡”                                                     
#define USENANWANGCARD                    13       //“请使用南方电网公司汽车充电卡”                                                   
#define CHARGEFULL                        14       //“电已充满，请插卡付费”                                                           
#define CHARGEEXCEPTION                   15       //“充电故障，已终止充电”   
#define CARDLOCKED                        16       //该卡已锁，请解锁卡后使用  
#define PUSHPLUGCLOSEDOOR                 17       //请插好充电插头，关好插座门
#define OPENDOORPULLPLUG                  18       //请打开插座门，取出充电插头
#define PULLPLUGCLOSEDOOR                 19       //请拔掉充电插头，关好插座门
#define NOMONEYSTOPCHARGE                 20       //余额不足，已终止充电      
#define PAIDLASTFEE                       21       //您上次充电费用已扣除   

#define METEREXCEPCHARGING                22       //电表通信故障，充电结束，请插卡付费
#define CARDCOMEXCEPCHARGING              23       //读卡器通信故障，充电结束，禁止插卡操作
#define REPAIRDOOROPENCHARGING            24       //维修门打开，充电结束，请插卡付费
#define REPAIRDOOROPENIDLE                25      //维修门打开，禁止充电
#define REPAIRDOOROPEN_FINDCARD           26      //维修门打开，请关闭维修门再充电

#define METERCOMEXCEP                     27      //电表通信异常，禁止充电
#define CARDCOMEXCEP                      28      //读卡器通信异常，禁止插卡操作
#define PALEEXCEP_HL_VOL                  29      //充电桩异常，暂停服务

#define AUDIO_DUANDIAN_CHARGING           30      //充电桩已断电，充电结束，请插卡付费
#define AUDIO_METER_H_VOL_A_CHARGING      31      //电表A相过压，充电结束，请插卡付费
#define AUDIO_METER_H_VOL_B_CHARGING      32      //电表B相过压，充电结束，请插卡付费
#define AUDIO_METER_H_VOL_C_CHARGING      33      //电表C相过压，充电结束，请插卡付费
#define AUDIO_METER_L_VOL_A_CHARGING      34      //电表A相欠压，充电结束，请插卡付费
#define AUDIO_METER_L_VOL_B_CHARGING      35      //电表B相欠压，充电结束，请插卡付费
#define AUDIO_METER_L_VOL_C_CHARGING      36      //电表C相欠压，充电结束，请插卡付费
#define AUDIO_METER_H_CUR_A_CHARGING      37      //电表A相过流，充电结束，请插卡付费        
#define AUDIO_METER_H_CUR_B_CHARGING      38      //电表B相过流，充电结束，请插卡付费 
#define AUDIO_METER_H_CUR_C_CHARGING      39      //电表C相过流，充电结束，请插卡付费                                              
#define AUDIO_METER_DUANXIANG_A_CHARGING  40      //电表A相断相，充电结束，请插卡付费
#define AUDIO_METER_DUANXIANG_B_CHARGING  41      //电表B相断相，充电结束，请插卡付费
#define AUDIO_METER_DUANXIANG_C_CHARGING  42      //电表C相断相，充电结束，请插卡付费
#define AUDIO_METER_DUANLU_A_CHARGING     43      //电表A相短路，充电结束，请插卡付费        
#define AUDIO_METER_DUANLU_B_CHARGING     44      //电表B相短路，充电结束，请插卡付费 
#define AUDIO_METER_DUANLU_C_CHARGING     45      //电表C相短路，充电结束，请插卡付费 
                                          

#define AUDIO_DUANDIAN_IDLE               46      //充电桩停电，禁止充电
#define AUDIO_METER_H_VOL_A_IDLE          47       //电表A相过压，禁止充电
#define AUDIO_METER_H_VOL_B_IDLE          48       //电表B相过压，禁止充电
#define AUDIO_METER_H_VOL_C_IDLE          49       //电表C相过压，禁止充电
                          				
#define AUDIO_METER_L_VOL_A_IDLE          50       //电表A相欠压，禁止充电
#define AUDIO_METER_L_VOL_B_IDLE          51       //电表B相欠压，禁止充电  
#define AUDIO_METER_L_VOL_C_IDLE          52       //电表C相欠压，禁止充电  
                       
#define AUDIO_METER_H_CUR_A_IDLE          53       //电表A相过流，禁止充电  
#define AUDIO_METER_H_CUR_B_IDLE          54       //电表B相过流，禁止充电  
#define AUDIO_METER_H_CUR_C_IDLE          55       //电表C相过流，禁止充电  
                      
#define AUDIO_METER_DUANXIANG_A_IDLE      56       //电表A相断相，禁止充电  
#define AUDIO_METER_DUANXIANG_B_IDLE      57       //电表B相断相，禁止充电  
#define AUDIO_METER_DUANXIANG_C_IDLE      58       //电表C相断相，禁止充电  
                    
#define AUDIO_METER_DUANLU_A_IDLE         59       //电表A相短路，禁止充电  
#define AUDIO_METER_DUANLU_B_IDLE         60       //电表B相短路，禁止充电  
#define AUDIO_METER_DUANLU_C_IDLE         61       //电表C相短路，禁止充电  
#define AUDIO_CURRENT_MONEY               62       //您当前的余额是

#define AUDIO_FUCTIONCARD                 63       //该卡为功能测试卡
#define AUDIO_MANAGECARD                  64       //该卡为充电管理卡
#define AUDIO_MODIFYKEYCARD               65       //该卡为修改密钥卡，正在修改密钥中
#define AUDIO_MODIFYKEYSUCESS             66       //修改密钥成功
#define AUDIO_MODIFYKEYFAIED              67       //修改密钥失败
#define AUDIO_KEYCARD_PALEIDLE_USE        68       //该卡为密钥修改卡，请在未充电时使用
#define AUDIO_REPAIRCARD_NOUSE            69       //该故障检修卡不能在此终端使用
#define AUDIO_MANAGECARD_NOUSE            70       //该充电管理卡不能再此终端使用
#define AUDIO_FUCTIONCARD_PALEIDE_USE     71       //该卡为功能测试卡，请在未充电状态下使用
#define AUDIO_START_TO_TEST               72       //开始进入功能测试
#define AUDIO_OPENDOOR                    73       //请打开插座门
#define AUDIO_TEST_COMPLETED              74       //功能测试完成

#define AUDIO_ZHONGDUAN_LAIDIAN           75       //终端来电
#define AUDIO_ZANTING_CHARGING            76       //维修门打开，已暂停充电，可插卡付费，结束本次充电。
#define AUDIO_BATERRY_VOL_LOW             77       //备用电池低，暂停服务
#define AUDIO_REPAIRCARD                  78       //该卡为故障检修卡
#define AUDIO_QIANUNCONNECT_CHARGING      79       //充电枪断开连接，充电结束，请插卡付费。


//add 07-23                                   
#define AUDIO_EXCEP_CHARGING              80        //充电桩故障，已结束充电，请刷卡付费
#define AUDIO_EXCEP_IDLE                  81        //充电桩故障，禁止充电                                              
#define AUDIO_CARDCOM_EXCEP_CHARGING      82        //读卡器通信故障，已结束充电，请勿刷卡付费
#define AUDIO_CHARGECOMPLETE              83        //充电完成,请刷卡付费
#define AUDIO_SELECTMODE                  84        //请选择充电模式
#define AUDIO_INPUT_ELE                   85        //请输入充电电量
#define AUDIO_INPUT_MON                   86        //请输入充电金额
#define AUDIO_INPUT_TIME                  87        //请输入充电时间
#define AUDIO_BRASHCARD                   88        //请刷卡
//#define AUDIO_OPENDOOR                    89        //请打开插座门
#define AUDIO_PUSHPLUG                    90        //请插好充电插头
#define AUDIO_PULLPLUG                    91        //请拔掉充电插头
#define AUDIO_INPUTERROR                  92        //输入错误，请重新输入
#define AUDIO_INPUTPRECHARGETIME          93        //请输入预约充电时间
#define AUDIO_NOMONEY_ENDCHARGE           94        //余额不足，已结束充电，请刷卡付费

#define AUDIO_SCREEN_EXCEP_IDLE           95        //显示屏通信故障，本桩暂停服务
#define AUDIO_SCREEN_EXCEP_CHARGINE       96        //显示屏通信故障，已结束充电，请刷卡付费。     
#define AUDIO_SCREEN_EXCEP_HF             97        //显示屏故障恢复

#define AUDIO_WELCOME                     98        //欢迎光临

#define AUDIO_PLUG_OUT_CHARGING           99        //充电枪断开，已结束充电，请刷卡付费

#define AUDIO_CARD_EXCEP_BRASHCARD        100       //读卡器通信故障，可退出服务
#define AUDIO_PLUG_OUT_BRASHCARD          101       //充电枪断开连接，请确认连接


typedef struct 
{
	ST_CHAR Year;
	ST_CHAR Month;
	ST_CHAR Day;
	ST_CHAR Hour;
	ST_CHAR Minite;
	ST_CHAR Second;

}TimeTP;

typedef struct 
{
	ST_CHAR SendHoutaiFlag;                       
	ST_CHAR sExchangeType;						
	ST_CHAR sExchangeSeq[5];					
	ST_CHAR sAreaCode[3];							
	ST_CHAR sStartCardNo[8];						
	ST_CHAR sEndCardNo[8];							
	ST_CHAR sStartCardType[2];						
	ST_CHAR sEndCardType[2];					
	ST_CHAR sStartEnerge[4];						
	ST_CHAR sStartEnerge1[4];						
	ST_CHAR sStartEnerge2[4];						
	ST_CHAR sStartEnerge3[4];						
	ST_CHAR sStartEnerge4[4];						
	ST_CHAR sEndEnerge[4];						
	ST_CHAR sEndEnerge1[4];						
	ST_CHAR sEndEnerge2[4];						
	ST_CHAR sEndEnerge3[4];						
	ST_CHAR sEndEnerge4[4];						
	ST_CHAR sPrice1[3];							
	ST_CHAR sPrice2[3];							
	ST_CHAR sPrice3[3];							
	ST_CHAR sPrice4[3];							
	ST_CHAR sParkFeePrice[3];						
	ST_CHAR sStartTime[6];							
	ST_CHAR sEndTime[6];							
	ST_CHAR sParkFee[3];							
	ST_CHAR sGasPrice[3];                        
	ST_CHAR sGasFee[4];							
	ST_CHAR sMoneyBefore[4];					
	ST_CHAR sMoneyAfter[4];						
	ST_CHAR sCardCounter[5];						
	ST_CHAR sTermNo[5];						
	ST_CHAR sCardVer[5];							
	ST_CHAR sPosNo[5];							
	ST_CHAR sCardStatus;					
	ST_CHAR sTimeLong[3];
//	ST_INT32 InerSeq;
}ChargeRecordTP;



typedef struct 
{
	ST_CHAR ChargeCardSeria[CARDSERIALEN+1];
    ST_CHAR ChargeCardType;
	ST_CHAR ChargeTmLong[3];   
	ST_FLOAT f_ChargeRate;
	ST_FLOAT f_TotalChargeEle;     
	ST_FLOAT f_CurrentChargeEle; 
	ST_FLOAT f_ChargeEle1;    
	ST_FLOAT f_ChargeEle2;   
	ST_FLOAT f_ChargeEle3;    
	ST_FLOAT f_ChargeEle4;      
	ST_FLOAT f_StartChargeEle;   
	ST_FLOAT f_StartChargeEle1;    
	ST_FLOAT f_StartChargeEle2;    
	ST_FLOAT f_StartChargeEle3;   
	ST_FLOAT f_StartChargeEle4;   
	ST_FLOAT f_EndChargeEle;	  
	ST_FLOAT f_EndChargeEle1;	 
	ST_FLOAT f_EndChargeEle2;	  
	ST_FLOAT f_EndChargeEle3;	  
	ST_FLOAT f_EndChargeEle4;	  

	ST_CHAR StartChargeMoney[4]; 
	ST_FLOAT f_StartChargeMoney; 
	ST_FLOAT f_EndChargeMoney;   
	ST_FLOAT f_CurrentBalance;	
	ST_FLOAT f_CurrentChargeMoney;

	ST_CHAR PaleStatus;	  
	TimeTP StartChargeTime;
	TimeTP EndChargeTime;
	ST_CHAR YuChongDianCount; 
	ChargeRecordTP LastRecord;
	ST_INT32 InerSeq;
	ST_UCHAR ChargeFullCount;
	ST_UCHAR SendTestSoftwareMsg[60];
	ST_INT32 ChargeFullNomoneyNoPulgCount;

}ChargeMsgTP;

typedef struct 
{
	ST_CHAR IsExistCardFlag; 
	ST_CHAR CardExcepFlag;
	ST_CHAR MeterExcepFlag;  
	ST_CHAR IsPatCardFlag;		// pat card 1, not pat card 0 刷卡标志： 刷卡，1； 未刷卡，0.
	//ST_CHAR IsChargeQiang;  
	ST_CHAR QiangConGood;     //插头连接好的标志，0：未连接好；1：连接好
	ST_CHAR StartChargeFlag;  //开始充电标记0:未充电状态，1：启动充电， 3:刷卡后，进入预约充电等待界面
	ST_CHAR CutdownEleFlag;  
	ST_INT32 SoundTipFlag;
	ST_CHAR ChargeFullFlag; //充满标志
	ST_CHAR ChargeFullCountFlag;
	ST_CHAR ChargeNomoneyNoPulgFg; //1：充电过程中，余额不足标志；0：充电过程中余额充足
	ST_CHAR ChargeExcepEndFlag;    //异常结束充电,bit0-bit5:读卡器故障，表故障，断电故障，维修门故障，显示屏故障，插头拔出故障
//	ST_CHAR ChargeExcepEndFlag_HF;    //异常结束充电,故障恢复标记，bit0-bit5:读卡器故障，表故障，断电故障，维修门故障，显示屏故障，插头拔出故障
	ST_CHAR ChongdianCangLedOnFg;
	//ST_CHAR Repair_Or_Manage_CardEndChargeFg;
	ST_CHAR RepairDoorOpenFg;
	ST_CHAR  NoSameCardFlag;     //1：显示非同一张卡，	
	ST_CHAR ChargeCompleteFlag;  //充电结束标志 1：充电完成。0：未完成
	ST_CHAR ExBoardCommFlag;     //按键通信故障标志 1：按键通信故障，0：正常

	//ST_CHAR MoneyNotEnoughFg;      //刷卡后余额不足，需重新输入
	ST_CHAR ScreenExcepFlag;  //显示屏故障，1：屏通信故障，0：正常
	ST_CHAR SendStartChargeCmdFg;//发送开始充电命令标记。1：发送了开始充电命令，0：未发送开始充电命令
	
}EventFlagTP;




typedef struct 
{
	ST_UCHAR LastPage;
	ST_UCHAR CurrentPage;
	ST_UCHAR NextPage;
	ST_UCHAR Displaybuf[60];

}DisplayMsgTP;

typedef struct 
{
	ST_UCHAR OverCur_VolTime; 
	ST_UCHAR OverCur_VolResuTime; 
	ST_UCHAR PhaseFailTime; 
	ST_UCHAR PhaseFail_ResuTime;
    
	ST_UCHAR AreaCode[3];    
	ST_UCHAR StopCarFeePrice[3];  
	ST_UCHAR IsDeductStopCarFee;  
	ST_UCHAR ZhongduanNum[5];   
	ST_UCHAR ManageCardnum1[8];  
	ST_UCHAR ManageCardnum2[8];
	ST_UCHAR ManageCardnum3[8];
	ST_UCHAR ManageCardnum4[8];
	ST_UCHAR ManageCardnum5[8];
	ST_UCHAR ManageCardnum6[8];
	ST_UCHAR ManageCardnum7[8];
	ST_UCHAR ManageCardnum8[8];
	ST_UCHAR ManageCardnum9[8];
	ST_UCHAR ManageCardnum10[8];
	ST_UCHAR PrepairCardnum1[8]; 
	ST_UCHAR PrepairCardnum2[8];
	ST_UCHAR PrepairCardnum3[8];
	ST_UCHAR PrepairCardnum4[8];
	ST_UCHAR PrepairCardnum5[8];
	ST_UCHAR PrepairCardnum6[8];
	ST_UCHAR PrepairCardnum7[8];
	ST_UCHAR PrepairCardnum8[8];
	ST_UCHAR PrepairCardnum9[8];
	ST_UCHAR PrepairCardnum10[8];
	ST_UCHAR ChargeMoneyLimit[3];   
	ST_UCHAR Rate1[3];         
	ST_UCHAR Rate2[3]; 
	ST_UCHAR Rate3[3];
	ST_UCHAR Rate4[3];
	ST_UCHAR MeterProtoType;
	ST_UCHAR OverCurProportion[2];  
	ST_UCHAR OverVolProportion[2];
	ST_UCHAR LowVolProportion[2];
	ST_UCHAR ChargeCurLimit[2];
	float fRate1;
	float fRate2;
	float fRate3;
	float fRate4;
	float fOverCurLimit;
	float fOverVolLimit;
	float fLowVolLimit;
	float fChargeCurLimit;
	float fChargeMoneyLimit;
		
}HouTaiDataTP;

typedef struct 
{
	ST_UCHAR PrintNoPaper:1;
	ST_UCHAR PrintMatch:1;
	ST_UCHAR PrintCut:1;
	ST_UCHAR Printfatal:1;
	ST_UCHAR PrintComError:1;
    ST_UCHAR :3;
	
}PrintExceptionTP;

union  PriAlarmTP
{
	PrintExceptionTP PrintException;
	ST_UCHAR  PrintAlarm;
};

extern union PriAlarmTP PriAlarm;


void  Tanka(void);
void SaveChargeRecData(ChargeRecordTP *ChargeRecord);

extern ChargeMsgTP ChargeMsg;


extern EventFlagTP EventFlag;

extern DisplayMsgTP DisplayMsg;

extern HouTaiDataTP HoutaiData;

extern DisplayInfoTP DisplayInfo;
extern SetChargeParaTP SetChargePara; //设置充电信息参数OUT1_QiangLed



extern ST_CHAR PrintDataLen; //打印数据内容长度，默认值为1:表示查询打印机状态，当需要打印数据时，该变量为打印数据长度
extern ST_UCHAR PrintData[100]; //打印内容buffer

extern ST_INT16 DefalutPhaseFlag;

extern ST_BOOL s_ProtolLandFlag;

extern ST_BOOL ProtocolExitFlag_Gprs;

ST_UCHAR* PackPrintData(ST_UCHAR *PrintData);
void Print(void);
void OpenDoor(void);
void CloseDoor(void);

ST_CHAR CheckDoorIsClosed(void);
ST_CHAR CheckDoorIsOpen(void);

#endif
