#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/************************Parameter-START****************************/
#define  Max_Chage_Time_Hour 							23
#define  Max_Chage_Time_Minute						59
/************************Parameter-END****************************/
#define  Debug 									0
#define  Union_Debug						0
#define  Debug_ExtBoard				0
#define		EPSILON   							0.000001
#define  SLEEP 									300
#define 	ON  											1
#define 	OFF 											0
#define 	OpenDoor_Delay				60
#define 	Operation_Delay			180
#define 	Min_Banlace						1.0
#define 	Max_InputTimes				3
#define 	Com_Delay							30
#define 	LEFT											0x01
#define 	RIGHT										0x02
#define 	Audio_Time_Slot			5
#define 	SrcError_Times				100
#define 	Waite_PatCard_Time	300
/*用户选择的启动充电模式*/
#define  PreCharge							1
#define  ImmediatelyCharge		0

#define W_ChargeByFull					0
#define W_Welcom									10
#define W_Welcom_Nopaper			90

#define W_ImmediatelyCharge 	11
#define W_PatCard_Balance			12			// Pat card show balance
#define W_CardBalance					13
#define W_ChargeByEle					14
#define W_ChargeByMoney 			15
#define W_ChargeByTime					16
#define W_CntPlug								17
#define W_ChargingEle					20
#define W_PayMoney          	21
#define W_RemovePlug						22
#define W_Print_Abnormal    	23
#define W_Print_Nopaper     	24
#define W_ThanksEnd							25
#define W_NoSameCard						26
#define W_Finish_Abnmal_Ele 			27
#define W_Finish_Abnmal_Money 	28
#define W_Finish_Abnmal_Time 		29
#define W_Finish_Abnmal_Auto 		30
#define W_PatCard_Button			 		31		// Pat card to start charging
#define W_Precharge_Confirm  	 	32
#define W_FinishFullEle   	 	33
#define W_Balance_Abnormal	 	34
#define W_TimeCharge      	 	35
#define W_NodeInfo							36
#define W_InputEle							37
#define W_InputMoney						38
#define W_InputTime							39
#define W_Full_Ele							40
#define W_Full_Money						41
#define W_Full_Time							42
#define W_CardAbnormal_Quit	43
#define W_Plug_Falloff					44
#define W_CardLock_R						45
#define W_WaitCharge						46
#define W_CardLock_N						47
#define W_ChargingMoney				48
#define W_ChargingTime					49
#define W_ChargingAutoFull		58
#define W_OpenDoor							59
#define W_FinishFullMoney   	66
#define W_FinishFullTime    	67
#define W_FinishFullAuto    	68

#define W_CloseDoor							80
#define W_OpenDoor_End					81
#define W_CloseDoor_End				82
#define W_Balance_little			83
#define W_Input_Again					84
#define W_InputErr_Quit				85

#define W_Finish_Nomoney_Ele 			86
#define W_Finish_Nomoney_Money 		87
#define W_Finish_Nomoney_Time 		88
#define W_Finish_Nomoney_Auto 		89

#define W_Finish_Nomoney_Ele_Badcard 			91
#define W_Finish_Nomoney_Money_Badcard 	92
#define W_Finish_Nomoney_Time_Badcard 		93
#define W_Finish_Nomoney_Auto_Badcard 		94
#define W_FinishFullEle_Badcard 						95
#define W_FinishFullMoney_Badcard 					96
#define W_FinishFullTime_Badcard 					97
#define W_FinishFullAuto_Badcard 					98
#define W_Full_Ele_Badcard										99
#define W_Full_Money_Badcard									100
#define W_Full_Time_Badcard									101
#define W_Finish_Abnmal_Ele_Badcard				102
#define W_Finish_Abnmal_Money_Badcard			103
#define W_Finish_Abnmal_Time_Badcard			104
#define W_Finish_Abnmal_Auto_Badcard			105

#define W_Finish_NoPlug_Ele									106
#define W_Finish_NoPlug_Money								107
#define W_Finish_NoPlug_Time									108
#define W_Finish_NoPlug_Auto									109
#define W_Finish_NoPlug_Ele_Badcard				110
#define W_Finish_NoPlug_Money_Badcard			111
#define W_Finish_NoPlug_Time_Badcard			112
#define W_Finish_NoPlug_Auto_Badcard			113

#define W_Abnormal							200
#define W_InputErr							201
#define W_ShowAbnormal_A			202
#define W_ShowAbnormal_B			203

#define M_ChargeEle 						0x01
#define M_ChargeMoney 					0x02
#define M_ChargeTime						0x03
#define M_AutoFull		  				0x04
#define M_PatCard		  					0x05
#define M_NodeInfo		  				0x06
/*Card status*/
#define Card_Normal							0x00		// ��Ϊ�����״̬
#define Card_Lock_R							0x01		//	��Ϊ��״̬������׮���ҵ�����¼
#define Card_Lock_N							0x02		//	��Ϊ��״̬������׮��δ�ҵ�����¼
void W_NoSameCard_Fction( void );
ST_CHAR W_Finish_Abnmal_Ele_Fction( void );
ST_CHAR W_Finish_Abnmal_Money_Fction( void );
ST_CHAR W_Finish_Abnmal_Time_Fction( void );
ST_CHAR W_Finish_Abnmal_Auto_Fction( void );

ST_CHAR W_Finish_Nomoney_Ele_Fction( void );
ST_CHAR W_Finish_Nomoney_Money_Fction( void );
ST_CHAR W_Finish_Nomoney_Time_Fction( void );
ST_CHAR W_Finish_Nomoney_Auto_Fction( void );

ST_CHAR W_Finish_NoPlug_Ele_Fction( void );
ST_CHAR W_Finish_NoPlug_Money_Fction( void );
ST_CHAR W_Finish_NoPlug_Time_Fction( void );
ST_CHAR W_Finish_NoPlug_Auto_Fction( void );



void W_CardLock_R_Fction( void );
void W_WaitCharge_Fction( void );
void W_Precharge_Confirm_Fction( void );

void W_CardLock_N_Fction( void );
void W_OpenDoor_Fction( void );
void W_CloseDoor_Fction( void );
void W_OpenDoor_End_Fction( void );
void W_CloseDoor_End_Fction( void );

void W_Welcom_Fction( void );
void W_ImmediatelyCharge_Fction( void );
void W_PatCard_Balance_Fction( void );
void W_PatCard_Button_Fction( void );

void W_TimeCharge_Fction( void );
void W_ChargeByEle_Fction( void );
void W_ChargeByMoney_Fction( void );
void W_ChargeByTime_Fction( void );
void W_ChargeByFull_Fction( void );
void W_CntPlug_Fction( void );
void W_ChargingEle_Fction( void );
void W_ChargingMoney_Fction( void );
void W_ChargingTime_Fction( void );
void W_ChargingAutoFull_Fction( void );
void W_PayMoney_Fction( void );
void W_RemovePlug_Fction( void );
void W_Print_Abnormal_Fction( void );
void W_Print_Nopaper_Fction( void );
void W_ThanksEnd_Fction( void );
ST_CHAR W_FinishFullEle_Fction( void );
ST_CHAR W_FinishFullMoney_Fction( void );
ST_CHAR W_FinishFullTime_Fction( void );
ST_CHAR W_FinishFullAuto_Fction( void );
void W_CardBalance_Fction( void );
void W_NodeInfo_Fction( void );
void W_InputEle_Fction( void );
void W_InputMoney_Fction( void );
void W_InputTime_Fction( void );
void W_InputErr_Fction( void );
void W_Abnormal_Fction( void );
void W_ShowAbnormal_A_Fction( void );
void W_ShowAbnormal_B_Fction( void );
void Abnormal_Process( void );

void Clear_Blank( void );

void W_Balance_little_Fction( void );
void W_Input_Again_Fction( void );
void W_InputErr_Quit_Fction( void );

void W_CardAbnormal_Quit_Fction( void );
void W_Plug_Falloff_Fction( void );
void W_Balance_Abnormal_Fction( void );



int InitDisplay( void );
void ExternBoard( void );
void DisplayControl( void );
void ComTest( void );
void TestCom( void );
void Change_Window( ST_UCHAR W_num );
void WriteRegisterLB( ST_INT16 startAddr, ST_UCHAR ON_OFF );
void ReadRegisterLB( ST_INT16 startAddr, ST_UCHAR resisterNum );
void WriteRegister_16bit( ST_INT16 startAddr, ST_UCHAR resisterNum,
	ST_UCHAR Data[] );
void ReadRegister_16bit( ST_INT16 startAddr, ST_UCHAR resisterNum );
void Get_PreChargeTime( void );
void ClearRegister_16bit( ST_INT16 startAddr, ST_UCHAR resisterNum );
//void TimeOut( ST_UCHAR Time, ST_INT16 RegAddr, ST_UCHAR LastWind );
void TimeOut( ST_UINT16 Time, ST_INT16 RegAddr, ST_UCHAR LastWind );
void Get_InputEle( void );
void Get_InputMoney( void );
void Get_InputTime( void );
void Float2Arry( ST_FLOAT f_num, ST_UCHAR Arry[] );
void Arry2Float( ST_UCHAR Arry[], ST_FLOAT* f_num );
void Show_Buffer( ST_UCHAR Buff[], ST_UCHAR Lenth );
unsigned short CRC16( const unsigned char *ptr, int Len );
ST_UCHAR ModbusAnalysis( ST_UCHAR *rcvbuf );

ST_UCHAR ExBoardAnalysis( ST_UCHAR *rcvbuf );
void ReadKey( void );
ST_UCHAR SideKey_Value( void );
//ST_FLOAT Key_Input(ST_INT16 RegAddr,ST_UCHAR Int_num, ST_UCHAR Float_num);
ST_FLOAT Key_Input( ST_INT16 RegAddr, ST_INT16 LBAddr, ST_UCHAR Int_num,
	ST_UCHAR Float_num );
//ST_UCHAR Key_Input_Time(ST_INT16 RegAddr);
ST_UCHAR Key_Input_Time( ST_INT16 RegAddr, ST_UCHAR Next_Wind, ST_INT16 LB_Hour,	ST_INT16 LB_Min );
void Show_TimeonLCD( void );
unsigned short CRC16( const unsigned char *ptr, int Len );

void SysInit_Clr( void );
void Input_Preprocess( void );
//void Read_SingleButton( ST_INT16 Regaddr, ST_UCHAR Win );
void Read_SingleButton( ST_INT16 Regaddr, ST_UCHAR Win, ST_CHAR LEFT_RIGHT );
ST_CHAR Down_Time( ST_CHAR Systime[], ST_CHAR Pretime[], ST_CHAR Dwntime[] );
void Clear_KeyboardBuff( void );
void Audio_Time_Alarm(ST_CHAR Secnd, ST_CHAR Soundtype);
void Audio_Time_Alarm_BadSCREEN( ST_CHAR Secnd, ST_CHAR Soundtype );
ST_UCHAR Read_Window_ID( void );
//以下是语音播放序号定义

#define AUDIO_PULLPUG                    20          //请拔掉充电枪
#define WELCOMEUSE                        1        // “欢迎使用中国南方电网公司为您提供的电动汽车充电服务”,“您当前的余额是XXXX.XX元”
#define BANLANCENOTENOUGH                 2        // “您的余额不足，请尽快充值”
#define PUSHPLUG                          3        // “请打开充电桩插座门，插入电源插头”
#define CLOSEDOOR                         4        // “请关闭插座门”
#define NOMOVETHECAR                      5        //“充电即将开始。请勿移动车辆”
#define BEGINECHARGE                      6        // “开始进行充电”
#define ENDCHARGING                       7        // “充电结束”
#define CURRENTPAID                       8        // “本次消费金额XXXX.XX元”
#define CURRENTBANLANCE                   9        // “您当前的余额是XXXX.XX元”
#define TAKEOUTCARD                       10       //“请取卡”
#define THANKBYE                          11       // “谢谢使用，再见”
#define NOTSAMECARD                       12       // “卡号不正确，请使用同一张卡”
#define USENANWANGCARD                    13       //“请使用南方电网公司汽车充电卡”
#define CHARGEFULL                        14       // “电已充满，请插卡付费”
#define CHARGEEXCEPTION                   15       // “充电故障，已终止充电”
#define CARDLOCKED                        16       // 该卡已锁，请解锁卡后使用
#define PUSHPLUGCLOSEDOOR                 17       //请插好充电插头，关好插座门
#define OPENDOORPULLPLUG                  18       //请打开插座门，取出充电插头
#define PULLPLUGCLOSEDOOR                 19       //请拔掉充电插头，关好插座门
#define NOMONEYSTOPCHARGE                 20       // 余额不足，已终止充电
#define PAIDLASTFEE                       21       // 您上次充电费用已扣除
#define METEREXCEPCHARGING                22       //电表通信故障，充电结束，请插卡付费
#define CARDCOMEXCEPCHARGING              23       //读卡器通信故障，充电结束，禁止插卡操作
#define REPAIRDOOROPENCHARGING            24       //维修门打开，充电结束，请插卡付费
#define REPAIRDOOROPENIDLE                25      //维修门打开，禁止充电
#define REPAIRDOOROPEN_FINDCARD           26      //维修门打开，请关闭维修门再充电
#define METERCOMEXCEP                     27      //电表通信异常，禁止充电
#define CARDCOMEXCEP                      28      //读卡器通信异常，禁止插卡操作
#define PALEEXCEP_HL_VOL                  29      //充电桩异常，暂停服务
//add 07-23
#define AUDIO_EXCEP_CHARGING              80        // 充电桩故障，已结束充电，请刷卡付费
#define AUDIO_EXCEP_IDLE                  81        // 充电桩故障，禁止充电
#define AUDIO_CARDCOM_EXCEP_CHARGING      82        // 读卡器通信故障，已结束充电，请勿刷卡付费
#define AUDIO_CHARGECOMPLETE              83        // 充电完成,请刷卡付费
#define AUDIO_SELECTMODE                  84        // 请选择充电模式
#define AUDIO_INPUT_ELE                   85        // 请输入充电电量
#define AUDIO_INPUT_MON                   86        // 请输入充电金额
#define AUDIO_INPUT_TIME                  87        // 请输入充电时间
#define AUDIO_BRASHCARD                   88        // 请刷卡
//#define AUDIO_OPENDOOR                    89        //请打开插座门
#define AUDIO_PUSHPLUG                    90        // 请插好充电插头
#define AUDIO_PULLPLUG                    91        // 请拔掉充电插头
#define AUDIO_INPUTERROR                  92        // 输入错误，请重新输入
#define AUDIO_INPUTPRECHARGETIME          93        // 请输入预约充电时间
#define AUDIO_NOMONEY_ENDCHARGE           94        // 余额不足，已结束充电，请刷卡付费
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
#endif

