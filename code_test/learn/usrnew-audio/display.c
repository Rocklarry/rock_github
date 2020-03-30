#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <math.h>
#include <fcntl.h>
#include "mycom.h"
#include "DataType.h"
#include "public.h"
#include "display.h"
#include "KuoZhanBoard.h"
#include "audio_play.h"

static ST_UCHAR Window_ID = W_Welcom;
static ST_UCHAR Window_last = 0;
static ST_UCHAR Window_current = 0;
static ST_UCHAR err_num = 0;								// input error times
static ST_INT16 SCREENerr_num = 0;						// SCREEN error times
static ST_UCHAR CLR_Flag = 0;
static ST_UCHAR CLR_Timeout = 0;
static ST_UCHAR CLR_Sound = 0;

static ST_UCHAR Save_Abnormal_Window = W_Welcom; // save the window when the abnormal occur
static ST_UCHAR Save_Badcard_Window = 0; // save the window when the printer abnormal
ST_UCHAR Tem_Data[10];

extern SetChargeParaTP SetChargePara;
extern DisplayInfoTP DisplayInfo;
extern EventFlagTP EventFlag;
extern union PriAlarmTP PriAlarm;
extern ST_INT16 DefalutPhaseFlag;

/*
 * bit0-bit11分别为：
 * bit0:	A相欠压,   bit1:  B相欠压，bit2: C相欠压
 * bit3:	A相过压，bit4:  B相过压，bit5: C相过压
 * bit6:	A相过流，bit7:  B相过流，bit8: C相过流
 * bit9:	A相断相，bit10: B相断相，bit11:C相断相
 * bit12:A相短路，bit13: B相短路，bit14:C相短路
 *
 */
extern ST_BOOL ProtolLandFlag_Gprs;				//GPRS登录标记。0::未登录，1:登录
extern ST_BOOL s_ProtolLandFlag;	//有线网络登录标记，0：未登录1：登录

/****************For_Abnormal_Process_Start****************/
/*Initial in normal state*/
static ST_CHAR MeterExcepFlag_Stus = 0;
static ST_CHAR CardExcepFlag_Stus = 0;
static ST_CHAR CutdownEleFlag_Stus = 0;
static ST_CHAR RepairDoorOpenFg_Stus = 0;
static ST_CHAR ScreenExcepFlag_Stus = 0;
static ST_CHAR ProtolLandFlag_Gprs_Stus = 1;
static ST_CHAR s_ProtolLandFlag_Stus = 1;
static ST_INT16 DefalutPhaseFlag_Stus = 0;
//static ST_CHAR Printer_Status = 0;
static ST_CHAR Normal_Status = 0;
static ST_CHAR Abnormal_Status = 0;
/****************For_Abnormal_Process_End****************/

/********************For_Extern_Keyboard_Start****************/
static ST_UCHAR g_Key;
static ST_UCHAR INT_ON = ON;
static ST_UCHAR inter_number = 0;
static ST_UCHAR float_number = 0;
static ST_INT32 num = 0;

static ST_FLOAT float_num = 0.0;
static ST_FLOAT f = 0.1;
static ST_FLOAT f_tmp = 0;
static ST_UCHAR Sidekey = 0;
static ST_BOOL Time_Flag = true;
//static ST_FLOAT key_value = 0;
/*******8************For_Extern_Keyboard_End*****************/

ST_UCHAR ExBoardAnalysis( ST_UCHAR *rcvbuf )
{
	ST_UINT16 crc, temp;
//	ST_UINT16 n = 0;
	if( rcvbuf[0] != 0x01 )						// Station ID 0x01
		return 0;
	switch( rcvbuf[1] )
	{
		case 0x01:							// KEY msg
			crc = CRC16( rcvbuf, 4 );
			temp = ( rcvbuf[4] << 8 ) + rcvbuf[5];
			if( crc != temp )
				return 0;
			return 1;
			break;
			/*case 0x02:					// print msg
			 n = rcvbuf[2] + 3;                              // rcvbuf[2] = data length
			 crc = CRC16(rcvbuf,n);
			 temp = (rcvbuf[n]<<8)+rcvbuf[n+1];
			 if(crc!= temp)
			 return 0;

			 return 1;
			 break;*/
		default:
			break;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       ReadKey
 ** Descriptions:						read extern board key board
 ** input parameters:			None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void ReadKey( void )
{
#if 0    //modify 08-13 by yan
	ST_INT32 rxnum = 0;
	memset( Uart[EXBOARDCOM].revbuf, 0, sizeof( Uart[EXBOARDCOM].revbuf ) );// Clear receive buffer

	rxnum = ReadCom( ExBoardfd, Uart[EXBOARDCOM].revbuf,
		sizeof( Uart[EXBOARDCOM].revbuf ), Com_Delay * 1000 ); // Read Extern Board
	//Show_Buffer(Uart[EXBOARDCOM].revbuf, sizeof(Uart[EXBOARDCOM].revbuf));
	//write(ExBoardfd,Uart[EXBOARDCOM].revbuf, sizeof(Uart[EXBOARDCOM].revbuf));	// Send data
#endif
#if Debug_ExtBoard
	if( rxnum > 0 )		// there are data
	{
		printf( "Rcv Data:\n" );
		Show_Buffer( Uart[EXBOARDCOM].sendbuf, 8 );
	}
#endif
//	if( ExBoardAnalysis( Uart[EXBOARDCOM].revbuf ) )	// data is available
	{
		if( Uart[EXBOARDCOM].revbuf[3] != 0 )
		{
			g_Key = Uart[EXBOARDCOM].revbuf[3];					// get key value
#if Debug_ExtBoard
			printf(
				"************************************g_Key = %c***%d********************************\n",
				g_Key, g_Key );
#endif
		}
	}
}

/*********************************************************************************************************
 ** Function name:       SideKey_Value
 ** Descriptions:						change g_key to side key
 ** input parameters:			None
 ** output parameters:   None
 ** Returned value:      key --key value
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_UCHAR SideKey_Value( void )
{
	ST_UCHAR key = 0;
	switch( g_Key )
	{
		case 0x01:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x02:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x04:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x08:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x10:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x20:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x40:
			key = g_Key;
			g_Key = 0;
			break;
		case 0x80:
			key = g_Key;
			g_Key = 0;
			break;
		default:
			break;
	}
	return key;
}

/*********************************************************************************************************
 ** Function name:       Key_Input_Time
 ** Descriptions:						use extern board key input time, include hour, minute
 ** input parameters:			None
 ** output parameters:   None
 ** Returned value:      key --key value
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_UCHAR Key_Input_Time(
	ST_INT16 RegAddr, ST_UCHAR Next_Wind, ST_INT16 LB_Hour,
	ST_INT16 LB_Min )
{
	char x = 0;
	ST_UCHAR key_value = 0;
	ST_UCHAR Charg_Data[5];
	x = g_Key;
	g_Key = 0;
	if( x != 0 )
	{
		//		if(x == 'k')														// jump between hour and  minute blank
		{

#if 0
			x = 0;
			if(Time_Flag == true)		// jump to minute blank
			{
				Time_Flag = false;
				inter_number = 0;
				num = 0;
			}
			else if(Time_Flag == false)					// jump to hour blank
			{
				Time_Flag = true;
				inter_number = 0;
				num = 0;
			}
#endif
		}

		if( x == 'c' )									// Clear hour and minute
		{
			x = 0;
			Time_Flag = true;
			g_Key = 0;
			x = 0;
			inter_number = 0;
			num = 0;
			key_value = 0;
			memset( Charg_Data, 0, sizeof( Charg_Data ) );
			WriteRegister_16bit( RegAddr, 2, &Charg_Data[0] );
			return key_value;
		}

		if( x >= '0' && x <= '9' )			// form time
		{
			WriteRegisterLB( LB_Hour, OFF );		// Disable keyboard on LCD
			WriteRegisterLB( LB_Min, OFF );
			g_Key = 0;
			x = x - '0';
#if Debug_ExtBoard
			printf( "x-'0' = %d\n", x );
#endif

			if( inter_number < 2 )
			{
				num *= 10;

#if Debug_ExtBoard
				printf( "num *= 10 num = %d\n", num );
#endif

				num += x;

#if Debug_ExtBoard
				printf( "num += x num = %d\n", num );
#endif

				inter_number++;

#if Debug_ExtBoard
				printf( "inter_number++; inter_number = %d\n", inter_number );
#endif

				//x = 0;
			}

			key_value = num;

#if Debug_ExtBoard
			printf( "key_value = num; key_value = %d\n", key_value );
#endif

			if( Time_Flag )		// hour
			{
				if( key_value > Max_Chage_Time_Hour)	// bigger than max hour
				{
					key_value = Max_Chage_Time_Hour;
				}
				Charg_Data[0] = 0;
				Charg_Data[1] = key_value;
				WriteRegister_16bit( RegAddr, 1, &Charg_Data[0] );// show hour on the LCD
				if( key_value / 10 )
				{
					Time_Flag = false;
					inter_number = 0;
					num = 0;
					key_value = 0;
				}
			}

			if( !Time_Flag )	// minute
			{
				if( key_value > Max_Chage_Time_Minute)// bigger than max minute
				{
					key_value = Max_Chage_Time_Minute;
				}
				Charg_Data[0] = 0;
				Charg_Data[1] = key_value;
				WriteRegister_16bit( RegAddr + 1, 1, &Charg_Data[0] );// show minute on the LCD
			}

			if( inter_number >= 2 )
			{
				if( Time_Flag == false)					// jump to hour blank
				{
					Time_Flag = true;
					inter_number = 0;
					num = 0;
				}
				else if( Time_Flag == true)		// jump to minute blank
				{
					Time_Flag = false;
					inter_number = 0;
					num = 0;
				}
			}
			x = 0;
			return key_value;
		}
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       Key_Input
 ** Descriptions:						use external board key input money or electric
 ** input parameters:
 **   													RegAddr 		--write in which LCD register
 **   													Int_num			--the bit of integer number
 **   													Float_num	--the bit of float number
 ** output parameters:   None
 ** Returned value:      key_value 	--float number
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_FLOAT Key_Input(
	ST_INT16 RegAddr, ST_INT16 LBAddr, ST_UCHAR Int_num,
	ST_UCHAR Float_num )
{
	char x = 0;
	ST_FLOAT key_value = 0.0;
	ST_UCHAR Charg_Data[20];
	x = g_Key;
	if( x != 0 )			 		// available
	{
		WriteRegisterLB( LBAddr, OFF );
		if( x == 'c' )		// Clear
		{
			g_Key = 0;
			x = 0;
			INT_ON = ON;
			inter_number = 0;
			float_number = 0;
			num = 0;
			float_num = 0.0;
			f = 0.1;
			f_tmp = 0;
			key_value = 0;
			Float2Arry( 0, &Charg_Data[0] );
			WriteRegister_16bit( RegAddr, 2, &Charg_Data[0] );
			return key_value;
		}

		if( x == '.' )
		{
			g_Key = 0;
			INT_ON = OFF;						// change to float part
		}

		if( x >= '0' && x <= '9' )
		{
			g_Key = 0;
			x = x - '0';
#if Debug_ExtBoard
			printf( "x-'0' = %d\n", x );
#endif
			if( INT_ON == ON)		// integer part
			{
				if( inter_number < Int_num )
				{
					num *= 10;
#if Debug_ExtBoard
					printf( "num *= 10 num = %d\n", num );
#endif

					num += x;
#if Debug_ExtBoard
					printf( "num += x num = %d\n", num );
#endif

					inter_number++;
#if Debug_ExtBoard
					printf( "inter_number++; inter_number = %d\n",
						inter_number );
#endif

					x = 0;
				}
			}

			if( INT_ON == OFF) // float part
			{
				if( float_number < Float_num )
				{
					f_tmp = x * f;
#if Debug_ExtBoard
					printf( "f_tmp= x * f f_tmp = %f\n", f_tmp );
#endif

					float_num += f_tmp;
#if Debug_ExtBoard
					printf( "float_num += f_tmp; float_num = %f\n", float_num );
#endif

					f /= 10;
#if Debug_ExtBoard
					printf( "f /= 10; f = %f\n", f );
#endif

					float_number++;
#if Debug_ExtBoard
					printf( "float_number; float_number = %d\n", float_number );
#endif

					x = 0;
				}
			}

			key_value = float_num + num;
#if Debug_ExtBoard
			printf( "key_value = float_num + num; key_value = %f\n",
				key_value );
#endif

			if( key_value < 0 )
			{
				key_value = 0;
			}
			if( fabs( key_value ) > 0.009 )
			{
				Float2Arry( key_value, &Charg_Data[0] );
				WriteRegister_16bit( RegAddr, 2, &Charg_Data[0] );
			}
			return key_value;
		}
	}
	return 0;
}

/*******************************************End Test***************************/

void ExternBoard( void )
{
	//TestCom_2(HongWaiCom,&Hongwaifd,COM1_ID);		// Open HongWaiCom, for Key
//	while( 1 )
	{
		ReadKey();													// Get key
		Sidekey = SideKey_Value();								// Get side key
		//	usleep(SLEEP*1000);
	}
}

int InitDisplay( void )
{
	ST_INT32 result;

	pthread_t Displaythread, Externthread;
#if 1
	result = pthread_create( &Displaythread, PTHREAD_CREATE_JOINABLE,
		( void * ) DisplayControl, NULL );
	if( result )
	{
		perror( "pthread_create: MeterComSend.\n" );
		exit( EXIT_FAILURE );
	}
	#endif

#if 0
	result = pthread_create( &Externthread, PTHREAD_CREATE_JOINABLE,
		( void * ) ExternBoard, NULL );
	if( result )
	{
		perror( "pthread_create: MeterComRcv.\n" );
		exit( EXIT_FAILURE );
	}
	while( 1 )
		;
#endif
	return 0;
}

void TestCom( void )
{
	ST_UCHAR Window_Temp = 0;

	EventFlag.MeterExcepFlag = 0;
	EventFlag.CardExcepFlag = 0;
	EventFlag.CutdownEleFlag = 0;
	DefalutPhaseFlag = 0;
	EventFlag.RepairDoorOpenFg = 0;
	EventFlag.ScreenExcepFlag = 0;
	ProtolLandFlag_Gprs = 1;
	s_ProtolLandFlag = 1;
#if 0
	DisplayInfo.SysTime[0] = 0;
#endif
	//Window_ID = W_ChargingAutoFull;
	while( 1 )
	{
		//usleep(SLEEP*1000);													// release CPU
		Abnormal_Process();								// process abnormal
		if( ( Window_Temp = Read_Window_ID() ) >= 10 )
		{
			Window_current = Window_Temp;
		}
		if( Window_current != Window_ID )					// change window
		{
			DisplayInfo.LastPage = Window_current;		// save last page
			Window_last = Window_current;
			DisplayInfo.CurrentPage = Window_ID;		// save current page
			Window_current = Window_ID;

			if( Window_current >= 10 )
			{
				Change_Window( Window_current );			// change window
			}

			CLR_Sound = 1;		// New window, set clear play sound flag
			CLR_Flag = 1;				// New window, set clear flag
			CLR_Timeout = 1;	// New window, set clear timeout flag
		}

		switch( Window_ID )
		{
			case W_Welcom:
				W_Welcom_Fction();								// Welcome
				break;
			case W_Welcom_Nopaper:
				W_Welcom_Fction();						// Welcome No paper
				break;
			case W_ImmediatelyCharge:
				W_ImmediatelyCharge_Fction();		// Immediately Charge
				break;
			case W_PatCard_Balance:
				W_PatCard_Balance_Fction();						// Pat Card
				break;
			case W_CardBalance:
				W_CardBalance_Fction();					// Show Card Balance
				break;
			case W_ChargeByEle:
				W_ChargeByEle_Fction();			// Select Charge by Electric
				break;
			case W_ChargeByMoney:
				W_ChargeByMoney_Fction();		// Select Charge by Money
				break;
			case W_ChargeByTime:
				W_ChargeByTime_Fction();			// Select Charge by Time
				break;
			case W_CntPlug:
				W_CntPlug_Fction();							// Connect Plug
				break;
			case W_ChargingEle:
				W_ChargingEle_Fction();				// charging by Electric
				break;
			case W_PayMoney:
				W_PayMoney_Fction();							// Pay money
				break;
			case W_RemovePlug:
				W_RemovePlug_Fction();						// Remove Plug
				break;
			case W_ThanksEnd:
				W_ThanksEnd_Fction();						// charge end
				break;
			case W_NoSameCard:
				W_NoSameCard_Fction();						// No same card
				break;
			case W_FinishFullEle:
				W_FinishFullEle_Fction();		// finished, in electric mode
				break;
			case W_FinishFullEle_Badcard:
				W_FinishFullEle_Fction();// finished, card abnormal, in electric mode
				break;
			case W_Full_Ele:
				W_FinishFullEle_Fction();	// finished, in electric mode
				break;
			case W_Full_Ele_Badcard:
				W_FinishFullEle_Fction();	// finished, in electric mode
				break;
			case W_TimeCharge:
				W_TimeCharge_Fction();						// Time Charge
				break;
			case W_NodeInfo:
				W_NodeInfo_Fction();					// Show Node Info
				break;
			case W_InputEle:
				W_InputEle_Fction();		// show Input electric window
				break;
			case W_InputMoney:
				W_InputMoney_Fction();			// show Input money window
				break;
			case W_InputTime:
				W_InputTime_Fction();			// show Input time window
				break;
			case W_CardLock_R:
				W_CardLock_R_Fction();	// Card locked and record is found
				break;
			case W_WaitCharge:
				W_WaitCharge_Fction();					// Wait to charge
				break;
			case W_CardLock_N:
				W_CardLock_N_Fction();	// Card locked but record is not found
				break;
			case W_ChargingMoney:
				W_ChargingMoney_Fction();				// charging by Money
				break;
			case W_ChargingTime:
				W_ChargingTime_Fction();				// charging by Time
				break;
			case W_ChargingAutoFull:
				W_ChargingAutoFull_Fction();		// charging by auto full
				break;
			case W_FinishFullMoney:
				W_FinishFullMoney_Fction();		// finished, in money mode
				break;
			case W_FinishFullMoney_Badcard:
				W_FinishFullMoney_Fction();		// finished, in money mode
				break;
			case W_Full_Money:
				W_FinishFullMoney_Fction();		// finished, in money mode
				break;
			case W_Full_Money_Badcard:
				W_FinishFullMoney_Fction();		// finished, in money mode
				break;
			case W_FinishFullTime:
				W_FinishFullTime_Fction();		// finished, in time mode
				break;
			case W_FinishFullTime_Badcard:
				W_FinishFullTime_Fction();		// finished, in time mode
				break;
			case W_Full_Time:
				W_FinishFullTime_Fction();		// finished, in time mode
				break;
			case W_Full_Time_Badcard:
				W_FinishFullTime_Fction();		// finished, in time mode
				break;
			case W_FinishFullAuto:
				W_FinishFullAuto_Fction();		// finished, in auto mode
				break;
			case W_FinishFullAuto_Badcard:
				W_FinishFullAuto_Fction();		// finished, in auto mode
				break;
			case W_InputErr:
				W_InputErr_Fction();					// Show Input error
				break;
			case W_Abnormal:
				W_Abnormal_Fction();							// Abnormal
				break;
			case W_ShowAbnormal_A:
				W_ShowAbnormal_A_Fction();				// Show abnormal A
				break;
			case W_ShowAbnormal_B:
				W_ShowAbnormal_B_Fction();				// Show abnormal B
				break;
			case W_OpenDoor:
				W_OpenDoor_Fction();							// Open door
				break;
			case W_CloseDoor:
				W_CloseDoor_Fction();							// Open door
				break;
			case W_OpenDoor_End:
				W_OpenDoor_End_Fction();					// Open door end
				break;
			case W_CloseDoor_End:
				W_CloseDoor_End_Fction();				// Close door end
				break;
			case W_Balance_little:
				W_Balance_little_Fction();					// No money
				break;
			case W_Input_Again:
				W_Input_Again_Fction();						// Input again
				break;
			case W_InputErr_Quit:
				W_InputErr_Quit_Fction();					// Input again
				break;
			case W_ChargeByFull:
				W_ChargeByFull_Fction();						// AutoFull
				break;
			case W_Finish_Abnmal_Ele:
				W_Finish_Abnmal_Ele_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Ele_Badcard:
				W_Finish_Abnmal_Ele_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Money:
				W_Finish_Abnmal_Money_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Money_Badcard:
				W_Finish_Abnmal_Money_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Time:
				W_Finish_Abnmal_Time_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Time_Badcard:
				W_Finish_Abnmal_Time_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Auto:
				W_Finish_Abnmal_Auto_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Abnmal_Auto_Badcard:
				W_Finish_Abnmal_Auto_Fction();	// abnormal occur, finish
				break;
			case W_Finish_Nomoney_Ele:
				W_Finish_Nomoney_Ele_Fction();			// no money, finish
				break;
			case W_Finish_Nomoney_Ele_Badcard:
				W_Finish_Nomoney_Ele_Fction();			// no money, finish
				break;
			case W_Finish_Nomoney_Money:
				W_Finish_Nomoney_Money_Fction();		// no money, finish
				break;
			case W_Finish_Nomoney_Money_Badcard:
				W_Finish_Nomoney_Money_Fction();		// no money, finish
				break;
			case W_Finish_Nomoney_Time:
				W_Finish_Nomoney_Time_Fction();			// no money, finish
				break;
			case W_Finish_Nomoney_Time_Badcard:
				W_Finish_Nomoney_Time_Fction();			// no money, finish
				break;
			case W_Finish_Nomoney_Auto:
				W_Finish_Nomoney_Auto_Fction();			// no money, finish
				break;
			case W_Finish_Nomoney_Auto_Badcard:
				W_Finish_Nomoney_Auto_Fction();			// no money, finish
				break;
			case W_Print_Abnormal:
				W_Print_Abnormal_Fction();	// Can't print, printer abnormal
				break;
			case W_Print_Nopaper:
				W_Print_Nopaper_Fction();	// Can't print, printer no paper
				break;
			case W_PatCard_Button:
				W_PatCard_Button_Fction();						// Pat Card
				break;
			case W_Precharge_Confirm:
				W_Precharge_Confirm_Fction();	// Confirm precharge time
				break;
			case W_CardAbnormal_Quit:
				W_CardAbnormal_Quit_Fction();	// Card machine abnormal, quit
				break;
			case W_Plug_Falloff:
				W_Plug_Falloff_Fction();			// Plug fall off, quit
				break;
			case W_Balance_Abnormal:
				W_Balance_Abnormal_Fction();// checking Balance, card machine abnormal
				break;
			case W_Finish_NoPlug_Ele:
				W_Finish_NoPlug_Ele_Fction();	// plug remove, charge stopped
				break;
			case W_Finish_NoPlug_Money:
				W_Finish_NoPlug_Money_Fction();	// plug remove, charge stopped
				break;
			case W_Finish_NoPlug_Time:
				W_Finish_NoPlug_Time_Fction();	// plug remove, charge stopped
				break;
			case W_Finish_NoPlug_Auto:
				W_Finish_NoPlug_Auto_Fction();	// plug remove, charge stopped
				break;
			case W_Finish_NoPlug_Ele_Badcard:
				W_Finish_NoPlug_Ele_Fction();// plug remove, charge stopped, card machine abnormal
				break;
			case W_Finish_NoPlug_Money_Badcard:
				W_Finish_NoPlug_Money_Fction();	// plug remove, charge stopped, card machine abnormal
				break;
			case W_Finish_NoPlug_Time_Badcard:
				W_Finish_NoPlug_Time_Fction();// plug remove, charge stopped, card machine abnormal
				break;
			case W_Finish_NoPlug_Auto_Badcard:
				W_Finish_NoPlug_Auto_Fction();// plug remove, charge stopped, card machine abnormal
				break;
			default:
				break;
		}
		usleep( 10 * 1000 );

	}

}

/*********************************************************************************************************
 ** Function name:       Show_Buffer
 ** Descriptions:        show the data in buffer
 ** input parameters:    Buff[]	--the address of data
 ** 														Lenth 	--the length of data
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Show_Buffer( ST_UCHAR Buff[], ST_UCHAR Lenth )
{
#if Union_Debug
	ST_UCHAR i = 0;
	for( i = 0; i < Lenth; i++ )
	{
		printf( "Buffer[%d] = %d\n", i, Buff[i] );
	}
#endif
}
/*********************************************************************************************************
 ** Function name:       Abnormal_Process
 ** Descriptions:        Welcome Window Function
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Abnormal_Process( void )
{

	if( SCREENerr_num >= SrcError_Times )
	{
		SCREENerr_num = SrcError_Times;
		EventFlag.ScreenExcepFlag = 1;
#if Debug
		printf( "SCREEN ERROR!\n" );
		//	getchar();
#endif
	}
	else
	{
		EventFlag.ScreenExcepFlag = 0;
	}

#if Debug
	if( EventFlag.ScreenExcepFlag == 1 )
	{
		EventFlag.StartChargeFlag = 0;
	}
	else
	{
		EventFlag.StartChargeFlag = 1;
	}
	//	getchar();
#endif
	// In welcome or charging state, check the abnormal
	if( ( W_Welcom_Nopaper == Window_ID ) || ( W_Welcom == Window_ID ) || ( W_ChargingEle == Window_ID ) || ( W_ChargingMoney == Window_ID ) || ( W_ChargingTime == Window_ID ) || ( W_ChargingAutoFull == Window_ID ) || ( W_Abnormal == Window_ID ) || ( W_ShowAbnormal_A == Window_ID ) || ( W_ShowAbnormal_B == Window_ID ) || ( W_Finish_Abnmal_Ele == Window_ID ) || ( W_Finish_Abnmal_Money == Window_ID ) || ( W_Finish_Abnmal_Time == Window_ID ) || ( W_Finish_Abnmal_Auto == Window_ID ) )//|| (W_PatCard_Button == Window_ID)
	/*if( ( W_ChargingEle == Window_ID ) || ( W_ChargingMoney == Window_ID )
	 || ( W_ChargingTime == Window_ID )
	 || ( W_ChargingAutoFull == Window_ID )
	 || ( W_Abnormal == Window_ID ) || ( W_ShowAbnormal_A == Window_ID )
	 || ( W_ShowAbnormal_B == Window_ID )
	 || ( W_Finish_Abnmal_Ele == Window_ID )
	 || ( W_Finish_Abnmal_Money == Window_ID )
	 || ( W_Finish_Abnmal_Time == Window_ID )
	 || ( W_Finish_Abnmal_Auto == Window_ID ) )//|| (W_PatCard_Button == Window_ID)*/
	{
#if Debug
		static ST_UCHAR c = 0;
		//static ST_CHAR runtime = 0;
		if( c != 8 )
		{
			printf( "Abnormal:\n" );
			//c = getchar();
		}
		if( c == '1' )
		{
			EventFlag.MeterExcepFlag = 1;
			EventFlag.CardExcepFlag = 0;
			EventFlag.CutdownEleFlag = 1;
			DefalutPhaseFlag = 8;
			EventFlag.RepairDoorOpenFg = 1;
			EventFlag.ScreenExcepFlag = 1;
			ProtolLandFlag_Gprs = 0;
			s_ProtolLandFlag = 0;
			c = 0;
			EventFlag.StartChargeFlag = 1;
		}
		if( c == '4' )
		{
			EventFlag.MeterExcepFlag = 1;
			EventFlag.CardExcepFlag = 0;
			EventFlag.CutdownEleFlag = 1;
			DefalutPhaseFlag = 8;
			EventFlag.RepairDoorOpenFg = 1;
			EventFlag.ScreenExcepFlag = 1;
			ProtolLandFlag_Gprs = 0;
			s_ProtolLandFlag = 0;
			c = 0;
			EventFlag.StartChargeFlag = 0;
			c = 8;
		}
		if( c == '2' )
		{
			EventFlag.MeterExcepFlag = 0;
			EventFlag.CardExcepFlag = 0;
			EventFlag.CutdownEleFlag = 0;
			DefalutPhaseFlag = 0;
			EventFlag.RepairDoorOpenFg = 0;
			EventFlag.ScreenExcepFlag = 0;
			ProtolLandFlag_Gprs = 1;
			s_ProtolLandFlag = 1;
			EventFlag.StartChargeFlag = 0;
			c = 0;
		}
		if( c == '3' )
		{
			EventFlag.MeterExcepFlag = 0;
			EventFlag.CardExcepFlag = 0;
			EventFlag.CutdownEleFlag = 0;
			DefalutPhaseFlag = 0;
			EventFlag.RepairDoorOpenFg = 0;
			EventFlag.ScreenExcepFlag = 0;
			ProtolLandFlag_Gprs = 1;
			s_ProtolLandFlag = 1;
			c = 0;
			EventFlag.StartChargeFlag = 1;
		}

#endif

		// Abnormal status changed
		if( ( MeterExcepFlag_Stus != EventFlag.MeterExcepFlag )	// || ( Printer_Status != PriAlarm.PrintAlarm )
		|| ( CardExcepFlag_Stus != EventFlag.CardExcepFlag )
			|| ( CutdownEleFlag_Stus != EventFlag.CutdownEleFlag )
			|| ( RepairDoorOpenFg_Stus != EventFlag.RepairDoorOpenFg )
			|| ( ScreenExcepFlag_Stus != EventFlag.ScreenExcepFlag )
			|| ( ProtolLandFlag_Gprs_Stus != ProtolLandFlag_Gprs )
			|| ( s_ProtolLandFlag_Stus != s_ProtolLandFlag )
			|| ( DefalutPhaseFlag_Stus != DefalutPhaseFlag ) )
		{
			// save abnormal status
			MeterExcepFlag_Stus = EventFlag.MeterExcepFlag;
			MeterExcepFlag_Stus = EventFlag.MeterExcepFlag;
			CardExcepFlag_Stus = EventFlag.CardExcepFlag;
			CutdownEleFlag_Stus = EventFlag.CutdownEleFlag;
			RepairDoorOpenFg_Stus = EventFlag.RepairDoorOpenFg;
			ScreenExcepFlag_Stus = EventFlag.ScreenExcepFlag;
			ProtolLandFlag_Gprs_Stus = ProtolLandFlag_Gprs;
			s_ProtolLandFlag_Stus = s_ProtolLandFlag;
			DefalutPhaseFlag_Stus = DefalutPhaseFlag;
			//Printer_Status = PriAlarm.PrintAlarm;

			// abnormal
			if( ( EventFlag.MeterExcepFlag == 1 )// || (PriAlarm.PrintAlarm & 0x1E) != 0 )
			|| ( EventFlag.CardExcepFlag == 1 )
				|| ( EventFlag.CutdownEleFlag == 1 )
				|| ( DefalutPhaseFlag != 0 )
				|| ( EventFlag.RepairDoorOpenFg == 1 )
				|| ( EventFlag.ScreenExcepFlag == 1 )
				|| ( ProtolLandFlag_Gprs == 0 )
				|| ( s_ProtolLandFlag == 0 ) )
			{
				Abnormal_Status = 1;
			}
			// normal
			else
			{
				switch( Save_Abnormal_Window )
				{
					case W_Welcom:
						Window_ID = W_Welcom;
						Save_Abnormal_Window = W_Welcom;
						break;
					case W_Welcom_Nopaper:
						Window_ID = W_Welcom;
						Save_Abnormal_Window = W_Welcom;
						break;
					case W_ChargingEle:
						Normal_Status = 1;							// normal
						if( EventFlag.StartChargeFlag == 1 )// start charge again
						{
							Window_ID = W_ChargingEle;
							Normal_Status = 0;
							Save_Abnormal_Window = W_Welcom;
						}
						break;
					case W_ChargingMoney:
						Normal_Status = 1;							// normal
						if( EventFlag.StartChargeFlag == 1 )// start charge again
						{
							Window_ID = W_ChargingMoney;
							Normal_Status = 0;
							Save_Abnormal_Window = W_Welcom;
						}
						break;
					case W_ChargingTime:
						Normal_Status = 1;							// normal
						if( EventFlag.StartChargeFlag == 1 )// start charge again
						{
							Window_ID = W_ChargingTime;
							Normal_Status = 0;
							Save_Abnormal_Window = W_Welcom;
						}
						break;
					case W_ChargingAutoFull:
						Normal_Status = 1;							// normal
						if( EventFlag.StartChargeFlag == 1 )// start charge again
						{
							Window_ID = W_ChargingAutoFull;
							Normal_Status = 0;
							Save_Abnormal_Window = W_Welcom;
						}
						break;
					default:
						break;
				}
			}
		}

		if( Normal_Status == 1 )
		{
			if( EventFlag.StartChargeFlag == 1 )
			{
				Normal_Status = 0;
				switch( Save_Abnormal_Window )
				{
					case W_ChargingEle:
						Window_ID = W_ChargingEle;
						Save_Abnormal_Window = W_Welcom;
						break;
					case W_ChargingMoney:
						Window_ID = W_ChargingMoney;
						Save_Abnormal_Window = W_Welcom;
						break;
					case W_ChargingTime:
						Window_ID = W_ChargingTime;
						Save_Abnormal_Window = W_Welcom;
						break;
					case W_ChargingAutoFull:
						Window_ID = W_ChargingAutoFull;
						Save_Abnormal_Window = W_Welcom;
						break;
					default:
						break;
				}
			}
		}

		if( Abnormal_Status == 1 )							// abnormal occur
		{
			if( EventFlag.StartChargeFlag == 0 )		// charge stopped
			{
				Abnormal_Status = 0;
				switch( Window_ID )
				{
					case W_Welcom:
						Save_Abnormal_Window = Window_ID;// save the window when the abnormal occur
						Window_ID = W_Abnormal;
						break;
					case W_Welcom_Nopaper:
						Save_Abnormal_Window = Window_ID;// save the window when the abnormal occur
						Window_ID = W_Abnormal;
						break;
					case W_ChargingEle:
						Save_Abnormal_Window = Window_ID;// save the window when the abnormal occur
						Window_ID = W_Finish_Abnmal_Ele;
						break;
					case W_ChargingMoney:
						Save_Abnormal_Window = Window_ID;// save the window when the abnormal occur
						Window_ID = W_Finish_Abnmal_Money;
						break;
					case W_ChargingTime:
						Save_Abnormal_Window = Window_ID;// save the window when the abnormal occur
						Window_ID = W_Finish_Abnmal_Time;
						break;
					case W_ChargingAutoFull:
						Save_Abnormal_Window = Window_ID;// save the window when the abnormal occur
						Window_ID = W_Finish_Abnmal_Auto;
						break;
					default:
						break;
				}
			}
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_Welcom_Fction
 ** Descriptions:        Welcome Window Function
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Welcom_Fction( void )
{

#if Debug
	ST_UCHAR flag = 0;
	printf( "Print No paper:" );
	//flag = getchar();
	if( flag == '1' )
	{
		PriAlarm.PrintException.PrintNoPaper = 1;
	}
	else if( flag == '2' )
	{
		PriAlarm.PrintException.PrintNoPaper = 0;
	}
	else if( flag == '3' )
	{
		EventFlag.ExBoardCommFlag = 1;
	}
	printf( "Print No paper:%d\n", PriAlarm.PrintAlarm );
#endif
#if 0//Debug
	printf( "Printer test: \n" );
	ST_CHAR flag = 0;
	flag = getchar();
	if(flag == '1')
	{
		PriAlarm.PrintAlarm = 0x1E;
		flag = 0;
	}
	if(flag == '2')
	{
		PriAlarm.PrintAlarm = 0;
		flag = 0;
	}
	if(flag == '3')
	{
		PriAlarm.PrintAlarm = 1;
		flag = 0;
	}
#endif

	if( ( PriAlarm.PrintAlarm & 0x1F ) || ( EventFlag.ExBoardCommFlag == 1 ) )// Printer no paper or abnormal or External board abnormal
	{
		Window_ID = W_Welcom_Nopaper;
	}
	else
	{
		Window_ID = W_Welcom;
		if( CLR_Sound == 1 )
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_WELCOME, NULL );
		}
	}
	SysInit_Clr();
	Show_TimeonLCD();
	ReadRegisterLB( 1, 2 ); 	// Read button status
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // available data
	{
		if( ( Sidekey != 0) )		// side key available
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_Welcom_Fction Read OK! \n" );
#endif
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_TimeCharge;
				SetChargePara.SetStartChargeMode = 1; 	// Precharge
				break;
			case 0x02:
				Window_ID = W_ImmediatelyCharge;
				SetChargePara.SetStartChargeMode = 0; 	// Immediately Charge
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:     	W_ImmediatelyCharge_Fction
 ** Descriptions:      	Immediately Charge Window Function
 ** input parameters:  	None
 ** output parameters: 	None
 ** Returned value:    	None
 ** Created by:        	Brian.Yang
 ** Created Date:      	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ImmediatelyCharge_Fction( void )
{
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_SELECTMODE, NULL );
	}
	if( CLR_Flag == 1 )													// clear
	{
		Clear_KeyboardBuff();				// clear for precharge time input
		Clear_Blank();
	}

	TimeOut( Operation_Delay, 18, W_Welcom );// 3 minute no operation,return to welcome

	ReadRegisterLB( 10, 8 ); 			// Read button status, get charge mode

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_ImmediatelyCharge_Fction Read OK! \n" );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_Welcom; 						// return
				break;
			case 0x02:
				break;
			case 0x04:
				Window_ID = W_NodeInfo;					// show Node Info
				SetChargePara.SetChargeMode = M_NodeInfo;
				break;
			case 0x08:
				Window_ID = W_ChargeByTime; 		// SetChargeTime
				SetChargePara.SetChargeMode = M_ChargeTime;
				SetChargePara.SetChargeEle = 0.0;
				SetChargePara.SetChargeMoney = 0.0;
				break;
			case 0x10:
				Window_ID = W_PatCard_Balance; // Please pat card to show CardBalance
				SetChargePara.SetChargeMode = M_PatCard;
				break;
			case 0x20:
				Window_ID = W_ChargeByMoney;		// SetChargeMoney
				SetChargePara.SetChargeMode = M_ChargeMoney;
				SetChargePara.SetChargeEle = 0.0;
				memset( SetChargePara.SetChargeTime, 0,
					sizeof( SetChargePara.SetChargeTime ) );
				break;
			case 0x40:

				Window_ID = W_ChargeByFull;
				SetChargePara.SetChargeMode = M_AutoFull;
				SetChargePara.SetChargeEle = 0.0;
				SetChargePara.SetChargeMoney = 0.0;
				memset( SetChargePara.SetChargeTime, 0,
					sizeof( SetChargePara.SetChargeTime ) );
				break;
			case 0x80:
				Window_ID = W_ChargeByEle;			// SetChargeEle
				SetChargePara.SetChargeMode = M_ChargeEle;
				SetChargePara.SetChargeMoney = 0.0;
				memset( SetChargePara.SetChargeTime, 0,
					sizeof( SetChargePara.SetChargeTime ) );
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:     		W_PatCard_Balance_Fnction
 ** Descriptions:       	Pat Card Window Function
 ** input parameters:    None
 ** output parameters:  	None
 ** Returned value:     	None
 ** Created by:         	Brian.Yang
 ** Created Date:       	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_PatCard_Balance_Fction( void )
{
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_BRASHCARD, NULL );
	}
	TimeOut( Operation_Delay, 20, W_Welcom );// if time out return to ImmediatelyCharge window
#if Union_Debug
#if Debug
	printf( "PatCard:" );
	ST_UCHAR flag = 0;
	flag = getchar();									// just for debug
	if( flag == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		DisplayInfo.FindLastChargeRec = Card_Lock_R;
		flag = 0;
	}
	if( flag == '3' )
	{
		DisplayInfo.FindLastChargeRec = Card_Lock_N;
		flag = 0;
	}
	if( flag == '4' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
#endif
#endif

	if( DisplayInfo.FindLastChargeRec == Card_Lock_R)
	{
		Window_ID = W_CardLock_R;
		DisplayInfo.FindLastChargeRec = 0;
	}
	else if( DisplayInfo.FindLastChargeRec == Card_Lock_N)
	{
		Window_ID = W_CardLock_N;
		DisplayInfo.FindLastChargeRec = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// Pat card
	{
		Window_ID = W_CardBalance; 						//Show card balance
		EventFlag.IsPatCardFlag = 0;
	}
	else if( EventFlag.CardExcepFlag == 1 )	// Card machine abnormal
	{
		Window_ID = W_Balance_Abnormal;
	}
}

/*********************************************************************************************************
 ** Function name:     		W_PatCard_Button_Fction
 ** Descriptions:       	Pat Card Window Function
 ** input parameters:    None
 ** output parameters:  	None
 ** Returned value:     	None
 ** Created by:         	Brian.Yang
 ** Created Date:       	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_PatCard_Button_Fction( void )
{
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_BRASHCARD, NULL );
	}
	ReadRegisterLB( 210, 1 ); 		// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyze received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_OpenDoor_End;			// "Quit" is pressed
				break;
			default:
				break;
		}
	}

	Clear_Blank();
	err_num = 0;									// clear input error times
	TimeOut( Operation_Delay, 211, W_OpenDoor_End );// 3 minute, not pat card, finish!

#if Union_Debug
#if Debug
	EventFlag.IsPatCardFlag = 0;
	printf( "PatCard:" );
	ST_UCHAR flag = 0;
	flag = getchar();										// just for debug
	if( flag == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		EventFlag.StartChargeFlag = 1;
		DisplayInfo.CardBalance = 100.0;
		SetChargePara.SetChargeMoney = 20;
		DisplayInfo.MaxChargeEle = 100;
		SetChargePara.SetChargeEle = 20;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.IsPatCardFlag = 1;
		DisplayInfo.FindLastChargeRec = Card_Lock_R;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.IsPatCardFlag = 1;
		DisplayInfo.FindLastChargeRec = Card_Lock_N;
		flag = 0;
	}
	if( flag == '4' )						// balance is not enough,quit
	{
		EventFlag.IsPatCardFlag = 1;
		EventFlag.ChargeNomoneyNoPulgFg = 1;
		DisplayInfo.CardBalance = 0.2;
		flag = 0;
	}
	if( flag == '5' )						// balance is not enough,quit
	{
		EventFlag.IsPatCardFlag = 1;
		EventFlag.ChargeNomoneyNoPulgFg = 1;
		DisplayInfo.CardBalance = 5;
		DisplayInfo.MaxChargeEle = 5;
		flag = 0;
	}
	if( flag == '6' )						// balance is not enough,quit
	{
		EventFlag.IsPatCardFlag = 1;
		EventFlag.StartChargeFlag = 3;
		flag = 0;
	}
	if( flag == '7' )
	{
		EventFlag.QiangConGood = 0;
		flag = 0;
	}
	if( flag == '8' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
#endif
#endif
	//if( ( EventFlag.QiangConGood == 0 ) && ( EventFlag.StartChargeFlag == 0 ) )	// Plug fall off
	if( ( EventFlag.QiangConGood == 0 ) && ( EventFlag.SendStartChargeCmdFg == 0 ) )// Plug fall off
	{
		Window_ID = W_Plug_Falloff;
	}
	else if( ( EventFlag.CardExcepFlag == 1 ) && ( EventFlag.StartChargeFlag == 0 ) )// Card machine abnormal
	{
		Window_ID = W_CardAbnormal_Quit;
	}
	else if( EventFlag.IsPatCardFlag == 1 )
	{
		if( DisplayInfo.FindLastChargeRec == Card_Lock_R)// Card locked, there is charged record
		{
			Window_ID = W_CardLock_R;
			DisplayInfo.FindLastChargeRec = 0;
			EventFlag.IsPatCardFlag = 0;
		}
		else if( DisplayInfo.FindLastChargeRec == Card_Lock_N)// Card locked, there is NOT charged record
		{
			Window_ID = W_CardLock_N;
			DisplayInfo.FindLastChargeRec = 0;
			EventFlag.IsPatCardFlag = 0;
		}
		else if( DisplayInfo.CardBalance < DEPOSIT)	// balance is not enough,quit
		{
			Window_ID = W_Balance_little;
			EventFlag.IsPatCardFlag = 0;
		}
		else if( ( DisplayInfo.CardBalance - DEPOSIT ) < SetChargePara.SetChargeMoney
			|| DisplayInfo.MaxChargeEle < SetChargePara.SetChargeEle )// balance is not enough, please select again
		{
			Window_ID = W_Input_Again;
			EventFlag.IsPatCardFlag = 0;
		}
		else if( EventFlag.StartChargeFlag == 1 )			// start to charge
		{

			//if(SetChargePara.SetStartChargeMode == PreCharge)	// Precharge
			//{
			//	Window_ID = W_WaitCharge;
			//}
			//else
			switch( SetChargePara.SetChargeMode )
			// Charging mode
			{
				case M_ChargeEle:
					Window_ID = W_ChargingEle;
					break;
				case M_ChargeMoney:
					Window_ID = W_ChargingMoney;
					break;
				case M_ChargeTime:
					Window_ID = W_ChargingTime;
					break;
				case M_AutoFull:
					Window_ID = W_ChargingAutoFull;
					break;
				default:
					break;
			}
			EventFlag.IsPatCardFlag = 0;
		}
		else if( EventFlag.StartChargeFlag == 3 )		// start to precharge
		{
			Window_ID = W_WaitCharge;
			EventFlag.IsPatCardFlag = 0;
		}
		//EventFlag.IsPatCardFlag = 0;
	}
}

/*********************************************************************************************************
 ** Function name:       W_TimeCharge_Fction
 ** Descriptions:        Time Charge Window Function
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_TimeCharge_Fction( void )
{
	TimeOut( 120, 254, W_Welcom );	// 3 minute no operation,return to W_Welcom
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUTPRECHARGETIME, NULL );
	}

	ReadRegisterLB( 250, 2 );    						// Read button status
	//WriteRegisterLB(258, OFF);
	//WriteRegisterLB(259, OFF);
	//Key_Input_Time(252);
	//Key_Input_Time( 252, W_ImmediatelyCharge, 258, 259 );

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) )// Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_TimeCharge_Fction Read OK! \n" );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_Welcom;          			// Return
				//Clear_Blank();
				break;
			case 0x02:
				//Clear_KeyboardBuff();
				Get_PreChargeTime();									// OK
				break;
			default:
				break;
		}
	}
}
/*********************************************************************************************************
 ** Function name:       Get_PreChargeTime
 ** Descriptions:        Get the time when starting charge
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Get_PreChargeTime( void )
{
	ReadRegister_16bit( 252, 2 );        					// Read hour, minute
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) )      // Analyse received data
	{
#if Debug
		printf( "Get_PreChargeTime Read OK! \n" );
		printf( "Hour:%d minute:%d\n", Uart[SCREENCOM].revbuf[4],
			Uart[SCREENCOM].revbuf[6] );
#endif

		SetChargePara.PreChargeTime[1] = Uart[SCREENCOM].revbuf[4];	// save hour
		SetChargePara.PreChargeTime[0] = Uart[SCREENCOM].revbuf[6]; // save minute

		//Window_ID = W_ImmediatelyCharge;
		Window_ID = W_Precharge_Confirm;
	}
#if Debug
	printf(
		"***********************SetChargePara.PreChargeTime Hour:%d Minute:%d\n",
		SetChargePara.PreChargeTime[1], SetChargePara.PreChargeTime[0] );
#endif
}

/*********************************************************************************************************
 ** Function name:       W_ChargeByEle_Fction
 ** Descriptions:       	Charging car by electric
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargeByEle_Fction( void )
{
	static ST_UCHAR Ele = 0;								// charge electric

	memset( Tem_Data, 0, sizeof( Tem_Data ) );  	// Clear buffer
	TimeOut( 120, 49, ImmediatelyCharge ); // 2 minute no operation,return to ImmediatelyCharge

	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUT_ELE, NULL );
	}
	ReadRegisterLB( 40, 8 ); 							// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_ChargeByEle_Fction Read OK! \n" );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ImmediatelyCharge; // return to Immediately Charge window
				//ClearRegister_16bit(48,1);						// clear input electric in blank
				break;
			case 0x02:												// ok button
				if( Ele == 0 )									// input is zero
				{
					Window_ID = W_InputErr;
					//ClearRegister_16bit(48,1);
				}
				else
				{
					SetChargePara.SetChargeEle = Ele;	// save user selected
#if Debug
					printf( "Ele = %d\n", Ele );
#endif
					Ele = 0;
					Input_Preprocess();				// Pat card or open the door
#if 0
					if(EventFlag.QiangConGood == 1)	// Plug is connect
					{
						if(DisplayInfo.CardBalance < SetChargePara.SetChargeMoney || DisplayInfo.MaxChargeEle < SetChargePara.SetChargeEle)// input is bigger than balance
						{
							err_num++;
							Window_ID = W_ImmediatelyCharge;
							if(err_num > 3)		// 3 times input error
							{
								err_num = 0;
								Window_ID = W_OpenDoor_End;	// open door to end
							}
						}
						else// input is small than balance , user select time or autofull charge mode
						{
							Window_ID = W_PatCard;
						}
					}
					else
					{
						Window_ID = W_OpenDoor;						// Open door
					}
#endif
				}
				break;
			case 0x04:
				Ele = 50;
				Tem_Data[1] = Ele;
				WriteRegister_16bit( 48, 1, Tem_Data );		// Show number
				break;
			case 0x08:
				Window_ID = W_InputEle;			// jump to input electric window
				Clear_KeyboardBuff();
				ClearRegister_16bit( 272, 2 );// clear input  electric in blank
				break;
			case 0x10:
				Ele = 30;
				Tem_Data[1] = Ele;
				WriteRegister_16bit( 48, 1, Tem_Data );		// Show number
				break;
			case 0x20:
				Ele = 40;
				Tem_Data[1] = Ele;
				WriteRegister_16bit( 48, 1, Tem_Data );		// Show number
				break;
			case 0x40:
				Ele = 10;
				Tem_Data[1] = Ele;
				WriteRegister_16bit( 48, 1, Tem_Data );		// Show number
				break;
			case 0x80:
				Ele = 20;
				Tem_Data[1] = Ele;
				WriteRegister_16bit( 48, 1, Tem_Data );		// Show number
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_ChargeByMoney_Fction
 ** Descriptions:       	Charging car by money
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargeByMoney_Fction( void )
{
	static ST_UCHAR Money = 0;									// charge money
	memset( Tem_Data, 0, sizeof( Tem_Data ) );  	// Clear buffer
	TimeOut( 120, 59, ImmediatelyCharge ); // 2 minute no operation,return to ImmediatelyCharge
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUT_MON, NULL );
	}
	ReadRegisterLB( 50, 8 ); 							// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_ChargeByMoney_Fction Read OK! \n" );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ImmediatelyCharge; // return to Immediately Charge window
				//ClearRegister_16bit(58,1);						// clear input money    in blank
				break;
			case 0x02:
				if( Money == 0 )								// input is zero
				{
					Window_ID = W_InputErr;
					//ClearRegister_16bit(58,1);
				}
				else
				{
					SetChargePara.SetChargeMoney = Money * 1.0;	// save user selected charge money
#if Debug
					printf( "Money = %d\n", Money );
#endif
					Money = 0;
					Input_Preprocess();				// Pat card or open the door
					//Window_ID = W_CntPlug;
					//Window_ID = W_OpenDoor;
				}
				break;
			case 0x04:
				Money = 50;
				Tem_Data[1] = Money;
				WriteRegister_16bit( 58, 1, Tem_Data );		// Show number
				break;
			case 0x08:
				Window_ID = W_InputMoney;		// jump to input money window
				Clear_KeyboardBuff();
				ClearRegister_16bit( 282, 2 );// clear input  money    in blank
				break;
			case 0x10:
				Money = 30;
				Tem_Data[1] = Money;
				WriteRegister_16bit( 58, 1, Tem_Data );		// Show number
				break;
			case 0x20:
				Money = 40;
				Tem_Data[1] = Money;
				WriteRegister_16bit( 58, 1, Tem_Data );		// Show number
				break;
			case 0x40:
				Money = 10;
				Tem_Data[1] = Money;
				WriteRegister_16bit( 58, 1, Tem_Data );		// Show number
				break;
			case 0x80:
				Money = 20;
				Tem_Data[1] = Money;
				WriteRegister_16bit( 58, 1, Tem_Data );		// Show number
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_ChargeByTime_Fction
 ** Descriptions:       	Charging car by time
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargeByTime_Fction( void )
{
	static ST_UCHAR Hour = 0;									// charge hour
	memset( Tem_Data, 0, sizeof( Tem_Data ) );  	// Clear buffer
	TimeOut( 120, 69, ImmediatelyCharge ); // 2 minute no operation,return to ImmediatelyCharge
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUT_TIME, NULL );
	}
	ReadRegisterLB( 60, 8 ); 							// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_ChargeByTime_Fction Read OK! \n" );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ImmediatelyCharge; // return to Immediately Charge window
				//ClearRegister_16bit(68,1);						// clear input hour     in blank
				break;
			case 0x02:
				if( Hour == 0 )									// input is zero
				{
					Window_ID = W_InputErr;
					//ClearRegister_16bit(68,1);
				}
				else
				{
					SetChargePara.SetChargeTime[0] = 0;		// second
					SetChargePara.SetChargeTime[1] = 0;		// minute
					SetChargePara.SetChargeTime[2] = Hour;	// hour
#if Debug
					printf( "Hour = %d\n", Hour );
#endif
					Hour = 0;									// clear static
					Input_Preprocess();
					//Window_ID = W_CntPlug;
					//Window_ID = W_OpenDoor;
				}
				break;
			case 0x04:
				Hour = 9;
				Tem_Data[1] = Hour;
				WriteRegister_16bit( 68, 1, Tem_Data );		// Show number
				break;
			case 0x08:
				Window_ID = W_InputTime;			// jump to input time window
				Clear_KeyboardBuff();
				ClearRegister_16bit( 292, 2 );		// clear input time in blank
				break;
			case 0x10:
				Hour = 5;
				Tem_Data[1] = Hour;
				WriteRegister_16bit( 68, 1, Tem_Data );		// Show number
				break;
			case 0x20:
				Hour = 7;
				Tem_Data[1] = Hour;
				WriteRegister_16bit( 68, 1, Tem_Data );		// Show number
				break;
			case 0x40:
				Hour = 1;
				Tem_Data[1] = Hour;
				WriteRegister_16bit( 68, 1, Tem_Data );		// Show number
				break;
			case 0x80:
				Hour = 3;
				Tem_Data[1] = Hour;
				WriteRegister_16bit( 68, 1, Tem_Data );		// Show number
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_ChargeByFull_Fction
 ** Descriptions:       	Charging car by Auto Full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargeByFull_Fction( void )
{
	Input_Preprocess();
}

/*********************************************************************************************************
 ** Function name:       W_CntPlug_Fction
 ** Descriptions:       	let user to connect plug
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CntPlug_Fction( void )
{
	Audio_Time_Alarm( Audio_Time_Slot, AUDIO_PUSHPLUG );
#if 0
	ClearRegister_16bit(48,1);					// clear input electric in blank
	ClearRegister_16bit(58,1);// clear input money    in blank
	ClearRegister_16bit(68,1);// clear input hour     in blank
	ClearRegister_16bit(252,2);
	ClearRegister_16bit(272,2);
	ClearRegister_16bit(282,2);
	ClearRegister_16bit(292,2);
#endif
	Clear_Blank();
	TimeOut( Operation_Delay, 70, W_Welcom );// 3 minute time out return to connect Welcom window

#if Debug
	ST_UCHAR flag = 0;
	printf( "EventFlag.QiangConGood:" );
	flag = getchar();										// just for Debug
	if( flag == '1' )
	{
		EventFlag.QiangConGood = 1;
	}
	printf( "flag: %c\n", flag );
	printf( "QiangConGood : %d\n", EventFlag.QiangConGood );
#endif
	CloseDoor();
	if( EventFlag.QiangConGood == 1 )	// Plug is connected
	{
		//Window_ID = W_PatCard;
		Window_ID = W_CloseDoor;
	}
}

/*********************************************************************************************************
 ** Function name:       W_ChargingEle_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modif:ied by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargingEle_Fction( void )
{
	ST_UCHAR Data[40];
	memset( Data, 0, sizeof( Data ) );						// clear buffer

	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( BEGINECHARGE, NULL );
	}

#if Debug																				// for Debug
	DisplayInfo.CardNo[0] = 0x31;				// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargingEle = 29.88;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 1.5;
	DisplayInfo.ChargedTime[2] = 16;				// hour
	DisplayInfo.ChargedTime[1] = 55;				// minute
	DisplayInfo.ChargedTime[0] = 2;				// second
#endif

	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          	// ID
	Float2Arry( DisplayInfo.ChargedEle, &Data[8] );		// ChargedEle
	Float2Arry( DisplayInfo.ChargingEle, &Data[12] );		// ChargingEle
	Float2Arry( DisplayInfo.ChargedMoney, &Data[16] );  	// ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Data[20] );  	// CardBalance
	Data[25] = DisplayInfo.ChargedTime[2];								// hour
	Data[27] = DisplayInfo.ChargedTime[1];							// minute
	Data[29] = DisplayInfo.ChargedTime[0];							// second

#if Debug
	printf( "\n************************Data******************************\n" );
	Show_Buffer( Data, sizeof( Data ) );
#endif

	//WriteRegister_16bit( 8990,4 , &Data[0]  );         // ID
	WriteRegister_16bit( 8990, 2, &Data[0] );         // ID
	WriteRegister_16bit( 8992, 2, &Data[4] );         // ID
	WriteRegister_16bit( 8994, 2, &Data[8] );         // ChargedEle
	WriteRegister_16bit( 8996, 2, &Data[12] );         // ChargingEle
	WriteRegister_16bit( 8998, 2, &Data[16] );         // ChargedMoney
	WriteRegister_16bit( 8980, 2, &Data[20] );         // CardBalance
	//WriteRegister_16bit( 8982,3 , &Data[24] );						// Time
	WriteRegister_16bit( 8982, 2, &Data[24] );						// Time
	WriteRegister_16bit( 8984, 1, &Data[28] );						// Time

#if Union_Debug
#if Debug

	printf( "ChargingEle:\n" );
	printf( "1.EventFlag.ChargeCompleteFlag = 1;\n" );
	printf( "2.EventFlag.IsPatCardFlag = 1;\n" );
	printf( "3.EventFlag.NoSameCardFlag = 1;\n" );
	printf( "4.EventFlag.ChargeFullFlag = 1;\n" );
	printf( "5.DisplayInfo.CardBalance = 0.2;\n" );
	printf( "6.PriAlarm.PrintAlarm = 0x04;		EventFlag.IsPatCardFlag = 1;\n" );
	printf( "7.PriAlarm.PrintAlarm = 0x01;		EventFlag.IsPatCardFlag = 1;\n" );
	printf( "8.EventFlag.ScreenExcepFlag = 1;\n" );
	printf( "9.EventFlag.QiangConGood = 0;		EventFlag.StartChargeFlag = 0;\n" );

	ST_UCHAR flag = 0;
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.ChargeCompleteFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag = 0;
	}
	if( flag == '4' )
	{
		EventFlag.ChargeFullFlag = 1;
		flag = 0;
	}
	if( flag == '5' )
	{
		DisplayInfo.CardBalance = 0.2;
		flag = 0;
	}
	if( flag == '6' )
	{
		PriAlarm.PrintAlarm = 0x04;
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}
	if( flag == '7' )
	{
		PriAlarm.PrintAlarm = 0x01;
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}

	if( flag == '8' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	if( flag == '9' )
	{
		EventFlag.QiangConGood = 0;
		EventFlag.StartChargeFlag = 0;
		flag = 0;
	}

#endif
#endif
	if( ( EventFlag.QiangConGood == 0 ) && ( EventFlag.StartChargeFlag == 0 ) )	// Plug fall off
	{
		Window_ID = W_Finish_NoPlug_Ele;
	}
	else if( EventFlag.ChargeCompleteFlag == 1 )	// charge Complete Finish
	{
		Window_ID = W_FinishFullEle;
	}
	else if( EventFlag.ChargeFullFlag == 1 )			// charge full Finish
	{
		Window_ID = W_Full_Ele;
	}
	else if( EventFlag.NoSameCardFlag == 1 )			// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// Pat card to finish
	{
		//Window_ID = W_FinishFullEle;
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;				// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}

	}
	//else if( DisplayInfo.CardBalance < Min_Banlace)		// money is not enough
	else if( EventFlag.ChargeNomoneyNoPulgFg == 1 )		// Balance is not enough
	{
		Window_ID = W_Finish_Nomoney_Ele;
	}
}

/*********************************************************************************************************
 ** Function name:       W_ChargingMoney_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modif:ied by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargingMoney_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );			// clear buffer
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( BEGINECHARGE, NULL );
	}
	#if Debug																	// for Debug
	DisplayInfo.CardNo[0] = 0x31;	// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.ChargingMoney = 4.96;
	DisplayInfo.CardBalance = 2.4;
	DisplayInfo.ChargedTime[2] = 16;	// hour
	DisplayInfo.ChargedTime[1] = 55;	// minute
	DisplayInfo.ChargedTime[0] = 2;	// second
#endif

	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Data[8] );  	// ChargedMoney
	Float2Arry( DisplayInfo.ChargingMoney, &Data[12] );  	// ChargingMoney
	Float2Arry( DisplayInfo.ChargedEle, &Data[16] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Data[20] );		// CardBalance
	Data[25] = DisplayInfo.ChargedTime[2];								// hour
	Data[27] = DisplayInfo.ChargedTime[1];							// minute
	Data[29] = DisplayInfo.ChargedTime[0];							// second

	//WriteRegister_16bit( 8960,4 , &Data[0]  );          		// ID
	WriteRegister_16bit( 8960, 2, &Data[0] );          		// ID
	WriteRegister_16bit( 8962, 2, &Data[4] );          		// ID
	WriteRegister_16bit( 8964, 2, &Data[8] );          		// ChargedMoney
	WriteRegister_16bit( 8966, 2, &Data[12] );         		// ChargingMoney
	WriteRegister_16bit( 8968, 2, &Data[16] );          		// ChargedEle
	WriteRegister_16bit( 8950, 2, &Data[20] );          		// CardBalance
	//WriteRegister_16bit( 8952,3 , &Data[24] );								// Time
	WriteRegister_16bit( 8952, 2, &Data[24] );							// Time
	WriteRegister_16bit( 8954, 1, &Data[28] );							// Time
#if Union_Debug
#if Debug
	printf( "ChargingMoney:" );
	printf( "1.EventFlag.ChargeCompleteFlag = 1;\n" );
	printf( "2.EventFlag.IsPatCardFlag = 1;\n" );
	printf( "3.EventFlag.NoSameCardFlag = 1;\n" );
	printf( "4.EventFlag.ChargeFullFlag = 1;\n" );
	printf( "5.DisplayInfo.CardBalance = 0.2;\n" );
	//printf( "6.PriAlarm.PrintAlarm = 0x04;		EventFlag.IsPatCardFlag = 1;\n" );
	//printf( "7.PriAlarm.PrintAlarm = 0x01;		EventFlag.IsPatCardFlag = 1;\n" );
	printf( "8.EventFlag.ScreenExcepFlag = 1;\n" );
	printf( "9.EventFlag.QiangConGood = 0;		EventFlag.StartChargeFlag = 0;\n" );

	ST_UCHAR flag = 0;
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.ChargeCompleteFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag = 0;
	}
	if( flag == '4' )
	{
		EventFlag.ChargeFullFlag = 1;
		flag = 0;
	}
	if( flag == '5' )
	{
		DisplayInfo.CardBalance = 0.2;
		flag = 0;
	}
	if( flag == '8' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	if( flag == '9' )
	{
		EventFlag.QiangConGood = 0;
		EventFlag.StartChargeFlag = 0;
		flag = 0;
	}
#endif
#endif
	if( ( EventFlag.QiangConGood == 0 ) && ( EventFlag.StartChargeFlag == 0 ) )	// Plug fall off
	{
		Window_ID = W_Finish_NoPlug_Money;
	}
	else if( EventFlag.ChargeCompleteFlag == 1 )			// Complete, Finish
	{
		Window_ID = W_FinishFullMoney;
	}
	else if( EventFlag.ChargeFullFlag == 1 )			// charge full Finish
	{
		Window_ID = W_Full_Money;
	}
	else if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// Pat card to finish
	{
		//Window_ID = W_FinishFullMoney;
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;		// show pay money
		//EventFlag.IsPatCardFlag = 0;
	}
//	else if( DisplayInfo.CardBalance < Min_Banlace)		// money is not enough
	else if( EventFlag.ChargeNomoneyNoPulgFg == 1 )		// Balance is not enough
	{
		Window_ID = W_Finish_Nomoney_Money;
	}

}

/*********************************************************************************************************
 ** Function name:       W_ChargingTime_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modif:ied by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargingTime_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );			// clear buffer
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( BEGINECHARGE, NULL );
	}
	#if Debug																	// for Debug
	DisplayInfo.CardNo[0] = 0x31;				// ID 
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedTime[2] = 16;				// hour
	DisplayInfo.ChargedTime[1] = 55;				// minute
	DisplayInfo.ChargedTime[0] = 2;				// second

	DisplayInfo.ChargingTime[2] = 4;
	DisplayInfo.ChargingTime[1] = 2;
	DisplayInfo.ChargingTime[0] = 39;

	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 4.4;
#endif

	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          // ID
	Data[9] = DisplayInfo.ChargedTime[2];
	Data[11] = DisplayInfo.ChargedTime[1];
	Data[13] = DisplayInfo.ChargedTime[0];

	Data[15] = DisplayInfo.ChargingTime[2];
	Data[17] = DisplayInfo.ChargingTime[1];
	Data[19] = DisplayInfo.ChargingTime[0];
	Float2Arry( DisplayInfo.ChargedMoney, &Data[20] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Data[24] );  // CardBalance

	Data[29] = DisplayInfo.ChargedTime[2];
	Data[31] = DisplayInfo.ChargedTime[1];
	Data[33] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8940,4 , &Data[0]  );        		// ID
	WriteRegister_16bit( 8940, 2, &Data[0] );        		// ID
	WriteRegister_16bit( 8942, 2, &Data[4] );        		// ID
	//WriteRegister_16bit( 8944,3 , &Data[8]  );							// ChargedTime
	WriteRegister_16bit( 8944, 2, &Data[8] );					// ChargedTime
	WriteRegister_16bit( 8946, 1, &Data[12] );					// ChargedTime
	//WriteRegister_16bit( 8947,3 , &Data[14] );							// ChargingTime
	WriteRegister_16bit( 8947, 2, &Data[14] );					// ChargingTime
	WriteRegister_16bit( 8949, 1, &Data[18] );					// ChargingTime
	WriteRegister_16bit( 8930, 2, &Data[20] );          	// ChargedMoney
	WriteRegister_16bit( 8932, 2, &Data[24] );          	// CardBalance
#if Union_Debug
#if Debug
	printf( "ChargingTime:" );
	printf( "1.EventFlag.ChargeCompleteFlag = 1;\n" );
	printf( "2.EventFlag.IsPatCardFlag = 1;\n" );
	printf( "3.EventFlag.NoSameCardFlag = 1;\n" );
	printf( "4.EventFlag.ChargeFullFlag = 1;\n" );
	printf( "5.DisplayInfo.CardBalance = 0.2;\n" );
	//printf( "6.PriAlarm.PrintAlarm = 0x04;		EventFlag.IsPatCardFlag = 1;\n" );
	//printf( "7.PriAlarm.PrintAlarm = 0x01;		EventFlag.IsPatCardFlag = 1;\n" );
	printf( "8.EventFlag.ScreenExcepFlag = 1;\n" );
	printf( "9.EventFlag.QiangConGood = 0;		EventFlag.StartChargeFlag = 0;\n" );

	ST_UCHAR flag = 0;
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.ChargeCompleteFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag = 0;
	}
	if( flag == '4' )
	{
		EventFlag.ChargeFullFlag = 1;
		flag = 0;
	}
	if( flag == '5' )
	{
		DisplayInfo.CardBalance = 0.2;
		flag = 0;
	}
	if( flag == '8' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	if( flag == '9' )
	{
		EventFlag.QiangConGood = 0;
		EventFlag.StartChargeFlag = 0;
		flag = 0;
	}
#endif
#endif
	if( ( EventFlag.QiangConGood == 0 ) && ( EventFlag.StartChargeFlag == 0 ) )	// Plug fall off
	{
		Window_ID = W_Finish_NoPlug_Time;
	}
	else if( EventFlag.ChargeCompleteFlag == 1 )			// Complete, Finish
	{
		Window_ID = W_FinishFullTime;
	}
	else if( EventFlag.ChargeFullFlag == 1 )			// charge full Finish
	{
		Window_ID = W_Full_Time;
	}
	else if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// Pat card to finish
	{
		//Window_ID = W_FinishFullTime;
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;		// show pay money
		//EventFlag.IsPatCardFlag = 0;
	}
	//else if( DisplayInfo.CardBalance < Min_Banlace)		// money is not enough
	else if( EventFlag.ChargeNomoneyNoPulgFg == 1 )		// Balance is not enough
	{
		Window_ID = W_Finish_Nomoney_Time;
	}
}

/*********************************************************************************************************
 ** Function name:       W_ChargingAutoFull_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modif:ied by:
 ** Modified date:
 *********************************************************************************************************/
void W_ChargingAutoFull_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );						// clear buffer
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( BEGINECHARGE, NULL );
	}
	#if Debug																				// for Debug
	DisplayInfo.CardNo[0] = 0x31;				// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 5.4;
	DisplayInfo.ChargedTime[2] = 16;				// hour
	DisplayInfo.ChargedTime[1] = 55;				// minute
	DisplayInfo.ChargedTime[0] = 2;				// second
#endif

	memcpy( &Data[0], DisplayInfo.CardNo, 8 );  	// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Data[8] );  	// ChargedMoney
	Float2Arry( DisplayInfo.ChargedEle, &Data[12] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Data[16] );  	// CardBalance
	Data[21] = DisplayInfo.ChargedTime[2];								// hour
	Data[23] = DisplayInfo.ChargedTime[1];							// minute
	Data[25] = DisplayInfo.ChargedTime[0];							// second

	//WriteRegister_16bit(8920,4 , &Data[0]);           		// ID
	WriteRegister_16bit( 8920, 2, &Data[0] );           		// ID
	WriteRegister_16bit( 8922, 2, &Data[4] );           		// ID
	WriteRegister_16bit( 8924, 2, &Data[8] );           		// ChargedMoney
	WriteRegister_16bit( 8926, 2, &Data[12] );          		// ChargedEle
	WriteRegister_16bit( 8928, 2, &Data[16] );          		// CardBalance
	//WriteRegister_16bit(8937,3 , &Data[20]);									// Time
	WriteRegister_16bit( 8937, 2, &Data[20] );							// Time
	WriteRegister_16bit( 8939, 1, &Data[24] );							// Time
#if Union_Debug
#if Debug
	printf( "ChargingAutoFull:" );
	printf( "1.DisplayInfo.CardBalance = 0.5;\n" );
	printf( "2.EventFlag.IsPatCardFlag = 1;\n" );
	printf( "3.EventFlag.NoSameCardFlag = 1;\n" );
	printf( "4.EventFlag.ChargeFullFlag = 1;\n" );
	//printf( "5.DisplayInfo.CardBalance = 0.2;\n" );
	//printf( "6.PriAlarm.PrintAlarm = 0x04;		EventFlag.IsPatCardFlag = 1;\n" );
	//printf( "7.PriAlarm.PrintAlarm = 0x01;		EventFlag.IsPatCardFlag = 1;\n" );
	printf( "8.EventFlag.ScreenExcepFlag = 1;\n" );
	printf( "9.EventFlag.QiangConGood = 0;		EventFlag.StartChargeFlag = 0;\n" );

	ST_UCHAR flag = 0;
	flag = getchar();
	if( flag == '1' )
	{
		DisplayInfo.CardBalance = 0.5;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag = 0;
	}
	if( flag == '4' )
	{
		EventFlag.ChargeFullFlag = 1;
		flag = 0;
	}
	if( flag == '8' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	if( flag == '9' )
	{
		EventFlag.QiangConGood = 0;
		EventFlag.StartChargeFlag = 0;
		flag = 0;
	}
#endif
#endif
	if( ( EventFlag.QiangConGood == 0 ) && ( EventFlag.StartChargeFlag == 0 ) )	// Plug fall off
	{
		Window_ID = W_Finish_NoPlug_Auto;
	}
	else if( EventFlag.ChargeFullFlag == 1 )			// Charge Full, Finish
	{
		Window_ID = W_FinishFullAuto;
	}
	else if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// Pat card to finish
	{
		//Window_ID = W_FinishFullAuto;
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;		// show pay money
		//EventFlag.IsPatCardFlag = 0;
	}
	//else if( DisplayInfo.CardBalance < Min_Banlace)		// money is not enough
	else if( EventFlag.ChargeNomoneyNoPulgFg == 1 )		// Balance is not enough
	{
		Window_ID = W_Finish_Nomoney_Auto;
	}
}

/*********************************************************************************************************
 ** Function name:      	Float2Arry
 ** Descriptions:
 ** input parameters:   	f_num : float number
 **                     	Arry[] : array which save data
 ** output parameters:   None
 ** Returned value:     	None
 ** Created by:         	Brian.Yang
 ** Created Date:       	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Float2Arry( ST_FLOAT f_num, ST_UCHAR Arry[] )
{
	ST_UINT32 temp = 0;

#if Debug
	printf( "\n***************************************f_num = %f\n", f_num );
#endif

	temp = ( ST_UINT32 ) ( f_num * 100 + 0.005);

#if Debug
	printf( "temp = %d\n", temp );
#endif

	Arry[0] = ( temp >> 8 ) & 0xFF;
	Arry[1] = ( temp >> 0 ) & 0xFF;
	Arry[2] = ( temp >> 24 ) & 0xFF;
	Arry[3] = ( temp >> 16 ) & 0xFF;

#if Debug
	Show_Buffer( Arry, sizeof( Arry ) );
#endif
}

/*********************************************************************************************************
 ** Function name:      	Arry2Float
 ** Descriptions:        read hex data from array, change it to float.
 ** input parameters:    f_num  : save float number
 **                     	Arry[] : array
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:         	Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Arry2Float( ST_UCHAR Arry[], ST_FLOAT *f_num )
{
	ST_UINT32 temp = 0;
	temp += ( Arry[2] << 24 );
	temp += ( Arry[3] << 16 );
	temp += ( Arry[0] << 8 );
	temp += ( Arry[1] );

#if Debug
	printf( "temp = %d\n", temp );
#endif

	*f_num = ( ST_FLOAT ) ( temp / 100.0);

#if Debug
	printf( "f_num = %f\n", *f_num );
#endif
}

/*********************************************************************************************************
 ** Function name:       W_PayMoney_Fction
 ** Descriptions:         	Show user how mouch he cost at this time
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_PayMoney_Fction( void )
{
	ST_UCHAR Data[20];
	ST_UCHAR Audio_Balance[9];
	ST_UCHAR Audio_PAY[9];
	memset( Data, 0, sizeof( Data ) );							// clear buffer
	memset( Audio_Balance, 0, sizeof( Audio_Balance ) );		// clear buffer
	memset( Audio_PAY, 0, sizeof( Audio_PAY ) );				// clear buffer

	if( PriAlarm.PrintAlarm & 0x01 )		// No paper
	{
		Window_ID = W_Print_Nopaper;				// show pay money
		EventFlag.IsPatCardFlag = 0;				// clear pat card flag
	}
	//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
	else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
	{
		Window_ID = W_Print_Abnormal;		// show pay money
		EventFlag.IsPatCardFlag = 0;				// clear pat card flag
	}
	TimeOut( 180, 116, W_OpenDoor_End );					//time out process
#if Debug																// for Debug
	DisplayInfo.ChargedMoney = 99999.06;
	DisplayInfo.CardBalance = 88888.94;
#endif

	Float2Arry( DisplayInfo.ChargedMoney, &Data[0] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Data[4] );  // CardBalance

	WriteRegister_16bit( 110, 2, &Data[0] );					// Show number
	WriteRegister_16bit( 112, 2, &Data[4] );          	// Show number

	if( CLR_Sound == 1 )										// play sound
	{
		CLR_Sound = 0;
		sprintf( ( ST_CHAR* ) Audio_Balance, "%8.2f", DisplayInfo.CardBalance );
		sprintf( ( ST_CHAR* ) Audio_PAY, "%8.2f", DisplayInfo.ChargedMoney );
#if Debug
		printf( "DDDDDDDDDDDDDDDDDDDDDDAudio_BalanceDDDDDDDDDDDDDDDDDDDDDDd" );
		Show_Buffer( Audio_Balance, sizeof( Audio_Balance ) );
		printf( "DDDDDDDDDDDDDDDDDDDDDDAudio_PAYDDDDDDDDDDDDDDDDDDDDDD" );
		Show_Buffer( Audio_PAY, sizeof( Audio_PAY ) );
#endif
		PlaySound_Money( CURRENTPAID, Audio_PAY );
		PlaySound_Money( CURRENTBANLANCE, Audio_Balance );
	}

	ReadRegisterLB( 114, 2 );
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_PayMoney_Fction Read OK! \n" );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				//EventFlag.IsNeedPrint = 2;		// don't print
				//Window_ID = W_RemovePlug;
				//Window_ID = W_ThanksEnd;
				Window_ID = W_OpenDoor_End;			// finish charge, open door
				break;
			case 0x02:
				Print();
				//EventFlag.IsNeedPrint = 1;		// print
				//Window_ID = W_RemovePlug;
				//Window_ID = W_ThanksEnd;
				Window_ID = W_OpenDoor_End;			// finish charge, open door
				break;
			default:
				break;
		}
	}

	if( EventFlag.ScreenExcepFlag == 1 )
	{
		Window_ID = W_OpenDoor_End;			// SCREEN abnormal, open door
	}
}

/*********************************************************************************************************
 ** Function name:       W_Print_Nopaper_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Print_Nopaper_Fction( void )
{
	ST_UCHAR Data[20];
	ST_UCHAR Audio_Balance[9];
	ST_UCHAR Audio_PAY[9];
	memset( Data, 0, sizeof( Data ) );							// clear buffer
	memset( Audio_Balance, 0, sizeof( Audio_Balance ) );		// clear buffer
	memset( Audio_PAY, 0, sizeof( Audio_PAY ) );				// clear buffer

	TimeOut( 5, 140, W_OpenDoor_End );		//time out process

#if Debug																	// for Debug
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
#endif

	Float2Arry( DisplayInfo.ChargedMoney, &Data[0] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Data[4] );  // CardBalance

	WriteRegister_16bit( 110, 2, &Data[0] );					// Show number
	WriteRegister_16bit( 112, 2, &Data[4] );          	// Show number
	if( CLR_Sound == 1 )										// play sound
	{
		CLR_Sound = 0;
		sprintf( ( ST_CHAR* ) Audio_Balance, "%8.2f", DisplayInfo.CardBalance );
		sprintf( ( ST_CHAR* ) Audio_PAY, "%8.2f", DisplayInfo.ChargedMoney );
#if Debug
		printf( "DDDDDDDDDDDDDDDDDDDDDDAudio_BalanceDDDDDDDDDDDDDDDDDDDDDDd" );
		Show_Buffer( Audio_Balance, sizeof( Audio_Balance ) );
		printf( "DDDDDDDDDDDDDDDDDDDDDDAudio_PAYDDDDDDDDDDDDDDDDDDDDDD" );
		Show_Buffer( Audio_PAY, sizeof( Audio_PAY ) );
#endif
		PlaySound_Money( CURRENTPAID, Audio_PAY );
		PlaySound_Money( CURRENTBANLANCE, Audio_Balance );
	}
}

/*********************************************************************************************************
 ** Function name:       W_Print_Abnormal_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Print_Abnormal_Fction( void )
{
	ST_UCHAR Data[20];
	ST_UCHAR Audio_Balance[9];
	ST_UCHAR Audio_PAY[9];
	memset( Data, 0, sizeof( Data ) );							// clear buffer
	memset( Audio_Balance, 0, sizeof( Audio_Balance ) );		// clear buffer
	memset( Audio_PAY, 0, sizeof( Audio_PAY ) );				// clear buffer

	TimeOut( 5, 130, W_OpenDoor_End );		//time out process

#if Debug																	// for Debug
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
#endif

	Float2Arry( DisplayInfo.ChargedMoney, &Data[0] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Data[4] );  // CardBalance

	WriteRegister_16bit( 110, 2, &Data[0] );					// Show number
	WriteRegister_16bit( 112, 2, &Data[4] );          	// Show number
	printf( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BF   CLR_Sound=%d\n", CLR_Sound );
	if( CLR_Sound == 1 )										// play sound
	{
		printf( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@MD   CLR_Sound=%d\n", CLR_Sound );
		CLR_Sound = 0;
		sprintf( ( ST_CHAR* ) Audio_Balance, "%8.2f", DisplayInfo.CardBalance );
		sprintf( ( ST_CHAR* ) Audio_PAY, "%8.2f", DisplayInfo.ChargedMoney );
#if Debug
		printf( "DDDDDDDDDDDDDDDDDDDDDDAudio_BalanceDDDDDDDDDDDDDDDDDDDDDDd" );
		Show_Buffer( Audio_Balance, sizeof( Audio_Balance ) );
		printf( "DDDDDDDDDDDDDDDDDDDDDDAudio_PAYDDDDDDDDDDDDDDDDDDDDDD" );
		Show_Buffer( Audio_PAY, sizeof( Audio_PAY ) );
#endif
		PlaySound_Money( CURRENTPAID, Audio_PAY );
		PlaySound_Money( CURRENTBANLANCE, Audio_Balance );
		printf( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@AF   CLR_Sound=%d\n", CLR_Sound );
	}
}
/*********************************************************************************************************
 ** Function name:       W_RemovePlug_Fction
 ** Descriptions:        let user to remove plug
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_RemovePlug_Fction( void )
{
	Audio_Time_Alarm( Audio_Time_Slot, AUDIO_PULLPLUG );

	TimeOut( 180, 120, W_Welcom );					//time out process
#if Debug
	printf( "EventFlag.QiangConGood:" );
	ST_UCHAR flag = 0;
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.QiangConGood = 0;
	}
#endif
	CloseDoor();
	if( EventFlag.QiangConGood == 0 )		// removed plug
	{
		Window_ID = W_CloseDoor_End;
	}
}

/*********************************************************************************************************
 ** Function name:       W_ThanksEnd_Fction
 ** Descriptions:	       	let user to remove plug
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ThanksEnd_Fction( void )
{
	TimeOut( 3, 150, W_Welcom );
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( THANKBYE, NULL );
	}
}

/*********************************************************************************************************
 ** Function name:       W_NoSameCard_Fction
 ** Descriptions:	       	let user to remove plug
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_NoSameCard_Fction( void )
{

	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( NOTSAMECARD, NULL );
	}
	TimeOut( 5, 160, Window_last );
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Abnmal_Ele_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Abnmal_Ele_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );				// clear buffer
#if 0//Debug
		printf("QiangconGood:");
		ST_CHAR flagQiang = 0;
		flagQiang = getchar();
		if(flagQiang == '1')
		{
			EventFlag.QiangConGood = 0;
			flagQiang = 0;
		}
#endif

	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}

#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	printf( "1.EventFlag.CardExcepFlag = 1;\n" );
	printf( "2.EventFlag.CardExcepFlag = 0;\n	EventFlag.ScreenExcepFlag = 0;\n" );
	printf( "3.EventFlag.ScreenExcepFlag = 1;\n" );
	DisplayInfo.SysTime[0]++;
	printf( "DisplayInfo.SysTime[0] = %d;\n", DisplayInfo.SysTime[0] );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		EventFlag.ScreenExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	#endif

	if( EventFlag.ScreenExcepFlag == 1 )		// SCREEN abnormal
	{
		if( EventFlag.CardExcepFlag == 1 )			// card machine abnormal
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_CARDCOM_EXCEP_CHARGING );

		}
		else
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_SCREEN_EXCEP_CHARGINE );
		}
	}

	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Abnmal_Ele_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Abnmal_Ele;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_EXCEP_CHARGING, NULL );
		}
	}

	Read_SingleButton( 170, W_ShowAbnormal_A, RIGHT );// read button status, if pressed return to Show abnormal A window

#if Debug																									// for Debug
	DisplayInfo.CardNo[0] = 0x31;                 	// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 5;                 	// hour
	DisplayInfo.ChargedTime[1] = 0;                 	// minute
	DisplayInfo.ChargedTime[0] = 0;                 	// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[8] );   	// ChargedEle
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[12] );  	// ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[16] );  	// CardBalance
	Charg_Data[21] = DisplayInfo.ChargedTime[2];         		// hour
	Charg_Data[23] = DisplayInfo.ChargedTime[1];          	// minute
	Charg_Data[25] = DisplayInfo.ChargedTime[0];					// second

	Float2Arry( DisplayInfo.ChargingEle, &Charg_Data[26] );   	// ChargingEle

	//WriteRegister_16bit( 8970,4 , &Charg_Data[0]  );       	// ID
	WriteRegister_16bit( 8970, 2, &Charg_Data[0] );       	// ID 0
	WriteRegister_16bit( 8972, 2, &Charg_Data[4] );       	// ID 1
	WriteRegister_16bit( 8974, 2, &Charg_Data[8] );       	// ChargedEle
	WriteRegister_16bit( 8996, 2, &Charg_Data[26] );    // ChargingEle added !!!
	WriteRegister_16bit( 8976, 2, &Charg_Data[12] );       	// ChargedMoney
	WriteRegister_16bit( 8978, 2, &Charg_Data[16] );       	// CardBalance
	//WriteRegister_16bit( 8987,3 , &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8987, 2, &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8989, 1, &Charg_Data[24] );       	// Time
#if Union_Debug
#if Debug
	printf( "Finish_Abnmal_Ele:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Abnmal_Money_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Abnmal_Money_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );		// clear buffer

	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}

#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	printf( "1.EventFlag.CardExcepFlag = 1;\n" );
	printf( "2.EventFlag.CardExcepFlag = 0;\n	EventFlag.ScreenExcepFlag = 0;\n" );
	printf( "3.EventFlag.ScreenExcepFlag = 1;\n" );
	DisplayInfo.SysTime[0]++;
	printf( "DisplayInfo.SysTime[0] = %d;\n", DisplayInfo.SysTime[0] );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		EventFlag.ScreenExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	#endif

	if( EventFlag.ScreenExcepFlag == 1 )		// SCREEN abnormal
	{
		if( EventFlag.CardExcepFlag == 1 )			// card machine abnormal
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_CARDCOM_EXCEP_CHARGING );
		}
		else
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_SCREEN_EXCEP_CHARGINE );
		}
	}
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Abnmal_Money_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Abnmal_Money;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_EXCEP_CHARGING, NULL );
		}
	}

	Read_SingleButton( 180, W_ShowAbnormal_A, RIGHT );// read button status, if pressed return to Show abnormal A window

#if Debug																										// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.ChargingMoney = 4.88;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[8] );  	// ChargedMoney
	Float2Arry( DisplayInfo.ChargingMoney, &Charg_Data[12] );  	// ChargingMoney
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[16] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[20] );		// CardBalance
	Charg_Data[25] = DisplayInfo.ChargedTime[2];
	Charg_Data[27] = DisplayInfo.ChargedTime[1];
	Charg_Data[29] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8960,4 , &Charg_Data[0]  );           	// ID
	WriteRegister_16bit( 8960, 2, &Charg_Data[0] );           	// ID 0
	WriteRegister_16bit( 8962, 2, &Charg_Data[4] );           	// ID	1
	WriteRegister_16bit( 8964, 2, &Charg_Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8966, 2, &Charg_Data[12] );          	// ChargingMoney
	WriteRegister_16bit( 8968, 2, &Charg_Data[16] );          	// ChargedEle
	WriteRegister_16bit( 8950, 2, &Charg_Data[20] );          	// CardBalance
	//WriteRegister_16bit( 8952,3 , &Charg_Data[24] );								// Time
	WriteRegister_16bit( 8952, 2, &Charg_Data[24] );					// Time
	WriteRegister_16bit( 8954, 1, &Charg_Data[28] );					// Time
#if Union_Debug
#if Debug
	printf( "FinishFullMoney:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Abnmal_Time_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Abnmal_Time_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );	// clear buffer

	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}

#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	printf( "1.EventFlag.CardExcepFlag = 1;\n" );
	printf( "2.EventFlag.CardExcepFlag = 0;\n	EventFlag.ScreenExcepFlag = 0;\n" );
	printf( "3.EventFlag.ScreenExcepFlag = 1;\n" );
	DisplayInfo.SysTime[0]++;
	printf( "DisplayInfo.SysTime[0] = %d;\n", DisplayInfo.SysTime[0] );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		EventFlag.ScreenExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	#endif

	if( EventFlag.ScreenExcepFlag == 1 )		// SCREEN abnormal
	{
		if( EventFlag.CardExcepFlag == 1 )			// card machine abnormal
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_CARDCOM_EXCEP_CHARGING );
		}
		else
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_SCREEN_EXCEP_CHARGINE );
		}
	}
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Abnmal_Time_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Abnmal_Time;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_EXCEP_CHARGING, NULL );
		}
	}

	Read_SingleButton( 190, W_ShowAbnormal_A, RIGHT );// read button status, if pressed return to Show abnormal A window

#if Debug																							// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second

	DisplayInfo.ChargingTime[2] = 4;
	DisplayInfo.ChargingTime[1] = 2;
	DisplayInfo.ChargingTime[0] = 39;

	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
#endif

	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          // ID
	Charg_Data[9] = DisplayInfo.ChargedTime[2];
	Charg_Data[11] = DisplayInfo.ChargedTime[1];
	Charg_Data[13] = DisplayInfo.ChargedTime[0];
	Charg_Data[15] = DisplayInfo.ChargingTime[2];
	Charg_Data[17] = DisplayInfo.ChargingTime[1];
	Charg_Data[19] = DisplayInfo.ChargingTime[0];

	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[20] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[24] );  // CardBalance

	Charg_Data[29] = DisplayInfo.ChargedTime[2];
	Charg_Data[31] = DisplayInfo.ChargedTime[1];
	Charg_Data[33] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8940,4 , &Charg_Data[0]  );          	// ID
	WriteRegister_16bit( 8940, 2, &Charg_Data[0] );          	// ID	0
	WriteRegister_16bit( 8942, 2, &Charg_Data[4] );          	// ID	1
	//WriteRegister_16bit( 8944,3 , &Charg_Data[8]  );							// ChargedTime
	WriteRegister_16bit( 8944, 2, &Charg_Data[8] );				// ChargedTime
	WriteRegister_16bit( 8946, 1, &Charg_Data[12] );			// ChargedTime
	//WriteRegister_16bit( 8947,3 , &Charg_Data[14] );							// ChargingTime
	WriteRegister_16bit( 8947, 2, &Charg_Data[14] );			// ChargingTime
	WriteRegister_16bit( 8949, 1, &Charg_Data[18] );			// ChargingTime
	WriteRegister_16bit( 8930, 2, &Charg_Data[20] );          	// ChargedMoney
	WriteRegister_16bit( 8932, 2, &Charg_Data[24] );          	// CardBalance
#if Union_Debug
#if Debug
	printf( "FinishFullTime:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Abnmal_Auto_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Abnmal_Auto_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );			// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}

#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	printf( "1.EventFlag.CardExcepFlag = 1;\n" );
	printf( "2.EventFlag.CardExcepFlag = 0;\n	EventFlag.ScreenExcepFlag = 0;\n" );
	printf( "3.EventFlag.ScreenExcepFlag = 1;\n" );
	DisplayInfo.SysTime[0]++;
	printf( "DisplayInfo.SysTime[0] = %d;\n", DisplayInfo.SysTime[0] );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		EventFlag.ScreenExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.ScreenExcepFlag = 1;
		flag = 0;
	}
	#endif

	if( EventFlag.ScreenExcepFlag == 1 )		// SCREEN abnormal
	{
		if( EventFlag.CardExcepFlag == 1 )			// card machine abnormal
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_CARDCOM_EXCEP_CHARGING );
		}
		else
		{
			CLR_Sound = 0;
			Audio_Time_Alarm_BadSCREEN( Audio_Time_Slot, AUDIO_SCREEN_EXCEP_CHARGINE );
		}
	}
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Abnmal_Auto_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Abnmal_Auto;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_EXCEP_CHARGING, NULL );
		}
	}

	Read_SingleButton( 200, W_ShowAbnormal_A, RIGHT );// read button status, if pressed return to Show abnormal A window
#if Debug																	// For Debug
	DisplayInfo.CardNo[0] = 0x31;	// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;	// hour
	DisplayInfo.ChargedTime[1] = 55;	// minute
	DisplayInfo.ChargedTime[0] = 2;	// second
#endif
	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          	// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Data[8] );  		// ChargedMoney
	Float2Arry( DisplayInfo.ChargedEle, &Data[12] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Data[16] );  	// CardBalance
	Data[21] = DisplayInfo.ChargedTime[2];
	Data[23] = DisplayInfo.ChargedTime[1];
	Data[25] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit(8920,4 , &Data[0]);           	// ID
	WriteRegister_16bit( 8920, 2, &Data[0] );           	// ID	0
	WriteRegister_16bit( 8922, 2, &Data[4] );           	// ID	1
	WriteRegister_16bit( 8924, 2, &Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8926, 2, &Data[12] );          	// ChargedEle
	WriteRegister_16bit( 8928, 2, &Data[16] );          	// CardBalance
	//WriteRegister_16bit(8937,3 , &Data[20]);								// Time
	WriteRegister_16bit( 8937, 2, &Data[20] );							// Time
	WriteRegister_16bit( 8939, 1, &Data[24] );							// Time
#if Union_Debug
#if Debug
	printf( "FinishFullAuto:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Nomoney_Ele_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Nomoney_Ele_Fction( void )
{
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Nomoney_Ele_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Nomoney_Ele;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_NOMONEY_ENDCHARGE, NULL );
		}
	}
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );				// clear buffer

#if Debug																									// for Debug
	DisplayInfo.CardNo[0] = 0x31;                 	// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 5;                 	// hour
	DisplayInfo.ChargedTime[1] = 0;                 	// minute
	DisplayInfo.ChargedTime[0] = 0;                 	// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[8] );   	// ChargedEle
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[12] );  	// ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[16] );  	// CardBalance
	Charg_Data[21] = DisplayInfo.ChargedTime[2];         		// hour
	Charg_Data[23] = DisplayInfo.ChargedTime[1];          	// minute
	Charg_Data[25] = DisplayInfo.ChargedTime[0];					// second

	//WriteRegister_16bit( 8970,4 , &Charg_Data[0]  );       	// ID
	WriteRegister_16bit( 8970, 2, &Charg_Data[0] );       	// ID	0
	WriteRegister_16bit( 8972, 2, &Charg_Data[4] );       	// ID	1
	WriteRegister_16bit( 8974, 2, &Charg_Data[8] );       	// ChargedEle
	WriteRegister_16bit( 8976, 2, &Charg_Data[12] );       	// ChargedMoney
	WriteRegister_16bit( 8978, 2, &Charg_Data[16] );       	// CardBalance
	//WriteRegister_16bit( 8987,3 , &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8987, 2, &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8989, 1, &Charg_Data[24] );       	// Time
#if Union_Debug
#if Debug
	printf( "Finish_Abnmal_Ele:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Nomoney_Money_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Nomoney_Money_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );		// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Nomoney_Money_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Nomoney_Money;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_NOMONEY_ENDCHARGE, NULL );
		}
	}

#if Debug																										// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.ChargingMoney = 4.88;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[8] );  	// ChargedMoney
	Float2Arry( DisplayInfo.ChargingMoney, &Charg_Data[12] );  	// ChargingMoney
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[16] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[20] );		// CardBalance
	Charg_Data[25] = DisplayInfo.ChargedTime[2];
	Charg_Data[27] = DisplayInfo.ChargedTime[1];
	Charg_Data[29] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8960,4 , &Charg_Data[0]  );           	// ID
	WriteRegister_16bit( 8960, 2, &Charg_Data[0] );           	// ID	0
	WriteRegister_16bit( 8962, 2, &Charg_Data[4] );           	// ID
	WriteRegister_16bit( 8964, 2, &Charg_Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8966, 2, &Charg_Data[12] );          	// ChargingMoney
	WriteRegister_16bit( 8968, 2, &Charg_Data[16] );          	// ChargedEle
	WriteRegister_16bit( 8950, 2, &Charg_Data[20] );          	// CardBalance
	//WriteRegister_16bit( 8952,3 , &Charg_Data[24] );								// Time
	WriteRegister_16bit( 8952, 2, &Charg_Data[24] );					// Time
	WriteRegister_16bit( 8954, 1, &Charg_Data[28] );					// Time
#if Union_Debug
#if Debug
	printf( "FinishFullMoney:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Nomoney_Time_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Nomoney_Time_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );	// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Nomoney_Time_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Nomoney_Time;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_NOMONEY_ENDCHARGE, NULL );
		}
	}

#if Debug																							// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second

	DisplayInfo.ChargingTime[2] = 4;
	DisplayInfo.ChargingTime[1] = 2;
	DisplayInfo.ChargingTime[0] = 39;

	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
#endif

	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          // ID
	Charg_Data[9] = DisplayInfo.ChargedTime[2];
	Charg_Data[11] = DisplayInfo.ChargedTime[1];
	Charg_Data[13] = DisplayInfo.ChargedTime[0];
	Charg_Data[15] = DisplayInfo.ChargingTime[2];
	Charg_Data[17] = DisplayInfo.ChargingTime[1];
	Charg_Data[19] = DisplayInfo.ChargingTime[0];

	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[20] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[24] );  // CardBalance

	Charg_Data[29] = DisplayInfo.ChargedTime[2];
	Charg_Data[31] = DisplayInfo.ChargedTime[1];
	Charg_Data[33] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8940,4 , &Charg_Data[0]  );          	// ID
	WriteRegister_16bit( 8940, 2, &Charg_Data[0] );          	// ID
	WriteRegister_16bit( 8942, 2, &Charg_Data[4] );          	// ID
	//WriteRegister_16bit( 8944,3 , &Charg_Data[8]  );							// ChargedTime
	WriteRegister_16bit( 8944, 2, &Charg_Data[8] );				// ChargedTime
	WriteRegister_16bit( 8946, 1, &Charg_Data[12] );			// ChargedTime
	//WriteRegister_16bit( 8947,3 , &Charg_Data[14] );							// ChargingTime
	WriteRegister_16bit( 8947, 2, &Charg_Data[14] );			// ChargingTime
	WriteRegister_16bit( 8949, 1, &Charg_Data[18] );			// ChargingTime
	WriteRegister_16bit( 8930, 2, &Charg_Data[20] );          	// ChargedMoney
	WriteRegister_16bit( 8932, 2, &Charg_Data[24] );          	// CardBalance
#if Union_Debug
#if Debug
	printf( "FinishFullTime:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_Nomoney_Auto_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_Nomoney_Auto_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );			// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_Nomoney_Auto_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_Nomoney_Auto;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_NOMONEY_ENDCHARGE, NULL );
		}
	}
	#if Debug																	// For Debug
	DisplayInfo.CardNo[0] = 0x31;				// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;				// hour
	DisplayInfo.ChargedTime[1] = 55;				// minute
	DisplayInfo.ChargedTime[0] = 2;				// second
#endif
	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          	// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Data[8] );  		// ChargedMoney
	Float2Arry( DisplayInfo.ChargedEle, &Data[12] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Data[16] );  	// CardBalance
	Data[21] = DisplayInfo.ChargedTime[2];
	Data[23] = DisplayInfo.ChargedTime[1];
	Data[25] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit(8920,4 , &Data[0]);           	// ID
	WriteRegister_16bit( 8920, 2, &Data[0] );           	// ID
	WriteRegister_16bit( 8922, 2, &Data[4] );           	// ID
	WriteRegister_16bit( 8924, 2, &Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8926, 2, &Data[12] );          	// ChargedEle
	WriteRegister_16bit( 8928, 2, &Data[16] );          	// CardBalance
	//WriteRegister_16bit(8937,3 , &Data[20]);								// Time
	WriteRegister_16bit( 8937, 2, &Data[20] );							// Time
	WriteRegister_16bit( 8939, 1, &Data[24] );							// Time
#if Union_Debug
#if Debug
	printf( "FinishFullAuto:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_FinishFullEle_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_FinishFullEle_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );				// clear buffer
	if( EventFlag.QiangConGood == 0 )						// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
		switch( Window_ID )
		{
			case W_FinishFullEle:								// complete
				Window_ID = W_FinishFullEle_Badcard;
				Save_Badcard_Window = W_FinishFullEle_Badcard;
				break;
			case W_Full_Ele:											// full
				Window_ID = W_Full_Ele_Badcard;
				Save_Badcard_Window = W_Full_Ele_Badcard;
				break;
			default:
				break;
		}
	}
	else
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CHARGECOMPLETE, NULL );
		}
		switch( Save_Badcard_Window )
		{
			case W_FinishFullEle_Badcard:
				Window_ID = W_FinishFullEle;
				Save_Badcard_Window = 0;
				break;
			case W_Full_Ele_Badcard:
				Window_ID = W_Full_Ele;
				Save_Badcard_Window = 0;
				break;
			default:
				break;
		}
	}

#if Debug																									// for Debug
	DisplayInfo.CardNo[0] = 0x31;                 	// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargingEle = 29.88;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 5;                 	// hour
	DisplayInfo.ChargedTime[1] = 0;                 	// minute
	DisplayInfo.ChargedTime[0] = 0;                 	// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[8] );   	// ChargedEle
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[12] );  	// ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[16] );  	// CardBalance
	Charg_Data[21] = DisplayInfo.ChargedTime[2];         		// hour
	Charg_Data[23] = DisplayInfo.ChargedTime[1];          	// minute
	Charg_Data[25] = DisplayInfo.ChargedTime[0];					// second

	Float2Arry( DisplayInfo.ChargingEle, &Charg_Data[26] );   	// ChargingEle

	//WriteRegister_16bit( 8970,4 , &Charg_Data[0]  );       	// ID
	WriteRegister_16bit( 8970, 2, &Charg_Data[0] );       	// ID	0
	WriteRegister_16bit( 8972, 2, &Charg_Data[4] );       	// ID	1
	WriteRegister_16bit( 8974, 2, &Charg_Data[8] );       	// ChargedEle
	WriteRegister_16bit( 8996, 2, &Charg_Data[26] );    // ChargingEle added !!!
	WriteRegister_16bit( 8976, 2, &Charg_Data[12] );       	// ChargedMoney
	WriteRegister_16bit( 8978, 2, &Charg_Data[16] );       	// CardBalance
	//WriteRegister_16bit( 8987,3 , &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8987, 2, &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8989, 1, &Charg_Data[24] );       	// Time
#if Union_Debug
#if Debug
	printf( "FinishFullEle:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_FinishFullMoney_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_FinishFullMoney_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );		// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
		switch( Window_ID )
		{
			case W_FinishFullMoney:								// complete
				Window_ID = W_FinishFullMoney_Badcard;
				Save_Badcard_Window = W_FinishFullMoney_Badcard;
				break;
			case W_Full_Money:											// full
				Window_ID = W_Full_Money_Badcard;
				Save_Badcard_Window = W_Full_Money_Badcard;
				break;
			default:
				break;
		}
	}
	else
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CHARGECOMPLETE, NULL );
		}
		switch( Save_Badcard_Window )
		{
			case W_FinishFullMoney_Badcard:
				Window_ID = W_FinishFullMoney;
				Save_Badcard_Window = 0;
				break;
			case W_Full_Money_Badcard:
				Window_ID = W_Full_Money;
				Save_Badcard_Window = 0;
				break;
			default:
				break;
		}
	}
	#if Debug																										// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.ChargingMoney = 4.88;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[8] );  	// ChargedMoney
	Float2Arry( DisplayInfo.ChargingMoney, &Charg_Data[12] );  	// ChargingMoney
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[16] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[20] );		// CardBalance
	Charg_Data[25] = DisplayInfo.ChargedTime[2];
	Charg_Data[27] = DisplayInfo.ChargedTime[1];
	Charg_Data[29] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8960,4 , &Charg_Data[0]  );           	// ID
	WriteRegister_16bit( 8960, 2, &Charg_Data[0] );           	// ID
	WriteRegister_16bit( 8962, 2, &Charg_Data[4] );           	// ID
	WriteRegister_16bit( 8964, 2, &Charg_Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8966, 2, &Charg_Data[12] );          	// ChargingMoney
	WriteRegister_16bit( 8968, 2, &Charg_Data[16] );          	// ChargedEle
	WriteRegister_16bit( 8950, 2, &Charg_Data[20] );          	// CardBalance
	//WriteRegister_16bit( 8952,3 , &Charg_Data[24] );								// Time
	WriteRegister_16bit( 8952, 2, &Charg_Data[24] );					// Time
	WriteRegister_16bit( 8954, 1, &Charg_Data[28] );					// Time
#if Union_Debug
#if Debug
	printf( "FinishFullMoney:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Wind	ow_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_FinishFullTime_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_FinishFullTime_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );	// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
		switch( Window_ID )
		{
			case W_FinishFullTime:								// complete
				Window_ID = W_FinishFullTime_Badcard;
				Save_Badcard_Window = W_FinishFullTime_Badcard;
				break;
			case W_Full_Time:											// full
				Window_ID = W_Full_Time_Badcard;
				Save_Badcard_Window = W_Full_Time_Badcard;
				break;
			default:
				break;
		}
	}
	else
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CHARGECOMPLETE, NULL );
		}
		switch( Save_Badcard_Window )
		{
			case W_FinishFullTime_Badcard:
				Window_ID = W_FinishFullTime;
				Save_Badcard_Window = 0;
				break;
			case W_Full_Time_Badcard:
				Window_ID = W_Full_Time;
				Save_Badcard_Window = 0;
				break;
			default:
				break;
		}
	}
	#if Debug																							// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second

	DisplayInfo.ChargingTime[2] = 4;
	DisplayInfo.ChargingTime[1] = 2;
	DisplayInfo.ChargingTime[0] = 39;

	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
#endif

	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          // ID
	Charg_Data[9] = DisplayInfo.ChargedTime[2];
	Charg_Data[11] = DisplayInfo.ChargedTime[1];
	Charg_Data[13] = DisplayInfo.ChargedTime[0];
	Charg_Data[15] = DisplayInfo.ChargingTime[2];
	Charg_Data[17] = DisplayInfo.ChargingTime[1];
	Charg_Data[19] = DisplayInfo.ChargingTime[0];

	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[20] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[24] );  // CardBalance

	Charg_Data[29] = DisplayInfo.ChargedTime[2];
	Charg_Data[31] = DisplayInfo.ChargedTime[1];
	Charg_Data[33] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8940,4 , &Charg_Data[0]  );          	// ID
	WriteRegister_16bit( 8940, 2, &Charg_Data[0] );          	// ID
	WriteRegister_16bit( 8942, 2, &Charg_Data[4] );          	// ID
	//WriteRegister_16bit( 8944,3 , &Charg_Data[8]  );							// ChargedTime
	WriteRegister_16bit( 8944, 2, &Charg_Data[8] );				// ChargedTime
	WriteRegister_16bit( 8946, 1, &Charg_Data[12] );			// ChargedTime
	//WriteRegister_16bit( 8947,3 , &Charg_Data[14] );							// ChargingTime
	WriteRegister_16bit( 8947, 2, &Charg_Data[14] );			// ChargingTime
	WriteRegister_16bit( 8949, 1, &Charg_Data[18] );			// ChargingTime
	WriteRegister_16bit( 8930, 2, &Charg_Data[20] );          	// ChargedMoney
	WriteRegister_16bit( 8932, 2, &Charg_Data[24] );          	// CardBalance
#if Union_Debug
#if Debug
	printf( "FinishFullTime:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_FinishFullAuto_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 **********************************************************************************************************/

ST_CHAR W_FinishFullAuto_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );			// clear buffer
	if( EventFlag.QiangConGood == 0 )	// Plug is remove
	{
		Window_ID = W_Welcom;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_FinishFullAuto_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_FinishFullAuto;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CHARGECOMPLETE, NULL );
		}
	}

#if Debug																	// For Debug
	DisplayInfo.CardNo[0] = 0x31;				// ID 
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;				// hour
	DisplayInfo.ChargedTime[1] = 55;				// minute
	DisplayInfo.ChargedTime[0] = 2;				// second
#endif
	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          	// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Data[8] );  		// ChargedMoney
	Float2Arry( DisplayInfo.ChargedEle, &Data[12] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Data[16] );  	// CardBalance
	Data[21] = DisplayInfo.ChargedTime[2];
	Data[23] = DisplayInfo.ChargedTime[1];
	Data[25] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit(8920,4 , &Data[0]);           	// ID
	WriteRegister_16bit( 8920, 2, &Data[0] );           	// ID
	WriteRegister_16bit( 8922, 2, &Data[4] );           	// ID
	WriteRegister_16bit( 8924, 2, &Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8926, 2, &Data[12] );          	// ChargedEle
	WriteRegister_16bit( 8928, 2, &Data[16] );          	// CardBalance
	//WriteRegister_16bit(8937,3 , &Data[20]);								// Time
	WriteRegister_16bit( 8937, 2, &Data[20] );							// Time
	WriteRegister_16bit( 8939, 1, &Data[24] );							// Time
#if Union_Debug
#if Debug
	printf( "FinishFullAuto:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_CardBalance_Fction
 ** Descriptions:       	Show card balance
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CardBalance_Fction( void )
{
	ST_UCHAR Data[50];
	ST_UCHAR Audio_Balance[8];
	memset( Data, 0, sizeof( Data ) );							// clear buffer
	memset( Audio_Balance, 0, sizeof( Audio_Balance ) );	// clear buffer
#if Debug 																											// For Debug
	DisplayInfo.CardBalance = 99996.61;
#endif

	TimeOut( 60, 30, W_ImmediatelyCharge );		// Time out process

	Float2Arry( DisplayInfo.CardBalance, Data );  	// CardBalance
	WriteRegister_16bit( 31, 2, Data );          		// CardBalance

	ReadRegisterLB( 33, 1 ); 							// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ImmediatelyCharge;
				break;
			default:
				break;
		}
	}
	if( CLR_Sound == 1 )
	{
		CLR_Sound = 0;
		sprintf( ( ST_CHAR* ) Audio_Balance, "%8.2f", DisplayInfo.CardBalance );
#if Debug
		//printf( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" );
		Show_Buffer( Audio_Balance, sizeof( Audio_Balance ) );
#endif
		PlaySound_Money( CURRENTBANLANCE, Audio_Balance );
	}
}

/*********************************************************************************************************
 ** Function name:       W_NodeInfo_Fction
 ** Descriptions:       	Show Node Info
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_NodeInfo_Fction( void )
{
	ST_UCHAR Data[50];
	TimeOut( 60, 268, W_ImmediatelyCharge );	// Time out process
	memset( Data, 0, sizeof( Data ) );						// clear buffer
#if Debug																				// For Debug
	DisplayInfo.NodeNo[0] = '1';
	DisplayInfo.NodeNo[1] = '1';
	DisplayInfo.NodeNo[2] = '2';
	DisplayInfo.NodeNo[3] = '3';
	DisplayInfo.NodeNo[4] = '4';
	DisplayInfo.NodeNo[5] = '5';
	DisplayInfo.NodeNo[6] = '6';
	DisplayInfo.NodeNo[7] = '7';
	DisplayInfo.Rated_Voltage = 220.00;
	DisplayInfo.Rated_Current = 32.00;
#endif

	memcpy( Data, DisplayInfo.NodeNo, 8 );          				// Node ID
	Float2Arry( DisplayInfo.Rated_Voltage, &Data[8] );		// Rated Voltage
	Float2Arry( DisplayInfo.Rated_Current, &Data[12] );		// Rated Current
	//WriteRegister_16bit( 260,4 , &Data[0]  );           			// Show Node ID
	WriteRegister_16bit( 260, 2, &Data[0] );           			// Show Node ID
	WriteRegister_16bit( 262, 2, &Data[4] );           			// Show Node ID
	WriteRegister_16bit( 264, 2, &Data[8] );           	// Show Rated Voltage
	WriteRegister_16bit( 266, 2, &Data[12] );           // Show Rated Current

	ReadRegisterLB( 269, 1 ); 							// Read button status
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ImmediatelyCharge;
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_InputErr_Fction
 ** Descriptions:       	Show user error, return to last window
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_InputErr_Fction( void )
{
	Clear_Blank();
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUTERROR, NULL );
	}
	TimeOut( 3, 1911, Window_last );
	ReadRegisterLB( 1910, 1 ); 		// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = Window_last;
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_ShowAbnormal_A_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-08-09
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ShowAbnormal_A_Fction( void )
{

	ST_UCHAR Data[20];
	memset( Data, 0, sizeof( Data ) );

	if( ( PriAlarm.PrintAlarm & 0x1E ) != 0 )			// 打印机故障
	{
		Data[1] = 1;
	}
	if( EventFlag.CutdownEleFlag == 1 )				// 充电桩已断电
	{
		Data[3] = 1;
	}
	if( EventFlag.MeterExcepFlag == 1 )				// 电表通信异常
	{
		Data[5] = 1;
	}
	if( EventFlag.CardExcepFlag == 1 )				// 读卡器通信异常
	{
		Data[7] = 1;
	}
	if( ProtolLandFlag_Gprs == 0 )							// GPRS未登录
	{
		Data[9] = 1;
	}
	if( s_ProtolLandFlag == 0 )									// 有线网络未登录
	{
		Data[11] = 1;
	}
	if( EventFlag.ExBoardCommFlag == 1 )			// 扩展板通信异常
	{
		Data[13] = 1;
	}
	if( EventFlag.RepairDoorOpenFg == 1 )		// 维修门打开
	{
		Data[15] = 1;
	}
	//WriteRegister_16bit(1922, 8, Data);
	WriteRegister_16bit( 1922, 2, &Data[0] );
	WriteRegister_16bit( 1924, 2, &Data[4] );
	WriteRegister_16bit( 1926, 2, &Data[8] );
	WriteRegister_16bit( 1928, 2, &Data[12] );

	ReadRegisterLB( 1920, 2 );							// Read button status
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) )// Analyze received data
	{
		if( ( Sidekey != 0) )
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}

#if Debug
		printf( "W_ShowAbnormal_A_Fction Read OK! \n" );
#endif

		if( 0x01 == Uart[SCREENCOM].revbuf[3] )			//return
		{
			switch( Save_Abnormal_Window )
			{
				case W_Welcom:
					Window_ID = W_Abnormal;
					break;
				case W_ChargingEle:
					Window_ID = W_Finish_Abnmal_Ele;
					break;
				case W_ChargingMoney:
					Window_ID = W_Finish_Abnmal_Money;
					break;
				case W_ChargingTime:
					Window_ID = W_Finish_Abnmal_Time;
					break;
				case W_ChargingAutoFull:
					Window_ID = W_Finish_Abnmal_Auto;
					break;
				default:
					break;
			}
		}
		else if( 0x02 == Uart[SCREENCOM].revbuf[3] )// jump to show abnormal B window
		{
			Window_ID = W_ShowAbnormal_B;	// jump to show abnormal B window
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_ShowAbnormal_B_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-08-09
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_ShowAbnormal_B_Fction( void )
{
	ST_UCHAR Data[32];
	memset( Data, 0, sizeof( Data ) );

	if( DefalutPhaseFlag != 0 )								// 过流过压等
	{
		Data[1] = 0x0001 & DefalutPhaseFlag;
		Data[3] = 0x0002 & DefalutPhaseFlag;
		Data[5] = 0x0004 & DefalutPhaseFlag;
		Data[7] = 0x0008 & DefalutPhaseFlag;
		Data[9] = 0x0010 & DefalutPhaseFlag;
		Data[11] = 0x0020 & DefalutPhaseFlag;
		Data[13] = 0x0040 & DefalutPhaseFlag;
		Data[15] = 0x0080 & DefalutPhaseFlag;
		Data[17] = ( 0x0100 & DefalutPhaseFlag ) >> 8;
		Data[19] = ( 0x0200 & DefalutPhaseFlag ) >> 8;
		Data[21] = ( 0x0400 & DefalutPhaseFlag ) >> 8;
		Data[23] = ( 0x0800 & DefalutPhaseFlag ) >> 8;
		Data[25] = ( 0x1000 & DefalutPhaseFlag ) >> 8;
		Data[27] = ( 0x2000 & DefalutPhaseFlag ) >> 8;
		Data[29] = ( 0x4000 & DefalutPhaseFlag ) >> 8;
	}
	//WriteRegister_16bit(1931, 15, &Data[0]);					// Show status on lamp
	WriteRegister_16bit( 1931, 2, &Data[0] );			// Show status on lamp
	WriteRegister_16bit( 1933, 2, &Data[4] );			// Show status on lamp
	WriteRegister_16bit( 1935, 2, &Data[8] );			// Show status on lamp
	WriteRegister_16bit( 1937, 2, &Data[12] );			// Show status on lamp
	WriteRegister_16bit( 1939, 2, &Data[16] );			// Show status on lamp
	WriteRegister_16bit( 1941, 2, &Data[20] );			// Show status on lamp
	WriteRegister_16bit( 1943, 2, &Data[24] );			// Show status on lamp
	WriteRegister_16bit( 1945, 1, &Data[28] );			// Show status on lamp

	Read_SingleButton( 1930, W_ShowAbnormal_A, LEFT );// read button status, if pressed return to Show abnormal A window
}

/*********************************************************************************************************
 ** Function name:       W_Abnormal_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-08-09
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Abnormal_Fction( void )
{
	Read_SingleButton( 1900, W_ShowAbnormal_A, RIGHT );
#if Debug
	printf( "void W_Abnormal_Fction( void )@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
	printf( "EventFlag.ScreenExcepFlag = %d @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", EventFlag.ScreenExcepFlag );
	printf( "CLR_Sound = %d @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", CLR_Sound );
#endif
	if( EventFlag.ScreenExcepFlag == 1 )				// SCREEN abnormal
	{
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_SCREEN_EXCEP_IDLE, NULL );
		}
	}
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_EXCEP_IDLE, NULL );
	}
}
/*********************************************************************************************************
 ** Function name:       W_InputEle_Fction
 ** Descriptions:       	Show input electric window
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_InputEle_Fction( void )
{
	TimeOut( 120, 275, W_ChargeByEle );	// 3 minute no operation,return to W_ChargeByEle
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUT_ELE, NULL );
	}
	ClearRegister_16bit( 48, 1 );				// clear input electric in blank
	Key_Input( 272, 274, 5, 2 );							// Input Electric
	ReadRegisterLB( 270, 2 );    						// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			if( Sidekey == 0x02 )
			{
				Uart[SCREENCOM].revbuf[3] = 0x02;
				Sidekey = 0;
			}
			else if( Sidekey == 0x08 )
			{
				Uart[SCREENCOM].revbuf[3] = 0x01;
				Sidekey = 0;
			}
		}
		#if Debug
		printf( "W_InputEle_Fction Read OK! \n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ChargeByEle;          // Return
				//Clear_Blank();

				break;
			case 0x02:
				Get_InputEle();											// OK
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       Get_InputEle
 ** Descriptions:        Get user inputed electric
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Get_InputEle( void )
{
	ST_FLOAT Ele = 0.0;
	ReadRegister_16bit( 272, 2 );        						// Read electric
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) )      // Analyze received data
	{
		Arry2Float( &Uart[SCREENCOM].revbuf[3], &Ele );
#if Debug
		printf( "Ele = %f\n", Ele );
		printf( "Get_InputEle**************************:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
		printf( "Get_InputEle Read OK! \n" );
#endif

		if( fabs( Ele ) <= EPSILON)
		{
			Window_ID = W_InputErr;
		}
		else
		{
			SetChargePara.SetChargeEle = Ele; // save user selected charge electric
#if Debug
			printf( "Ele = %f\n", Ele );
#endif
			Ele = 0.0;			// clear static
			Input_Preprocess();						// Pat card or open the door
			//Window_ID = W_CntPlug;
			//ClearRegister_16bit(272,2);
		}
	}
#if Debug
	printf( "SetChargePara.SetChargeEle %f**************************:\n",
		SetChargePara.SetChargeEle );
#endif
}

/*********************************************************************************************************
 ** Function name:       W_InputMoney_Fction
 ** Descriptions:       	Show input Money window
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_InputMoney_Fction( void )
{
	TimeOut( 120, 285, W_ChargeByMoney );// 3 minute no operation,return to W_ChargeByMoney
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUT_MON, NULL );
	}
	ClearRegister_16bit( 58, 1 );			// clear select money    in blank

	Key_Input( 282, 284, 5, 2 );
	ReadRegisterLB( 280, 2 );    // Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			if( Sidekey == 0x02 )
			{
				Uart[SCREENCOM].revbuf[3] = 0x02;
				Sidekey = 0;
			}
			else if( Sidekey == 0x08 )
			{
				Uart[SCREENCOM].revbuf[3] = 0x01;
				Sidekey = 0;
			}
		}
		#if Debug
		printf( "W_InputMoney_Fction Read OK! \n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ChargeByMoney;          // Return
				//Clear_Blank();
				break;
			case 0x02:
				Get_InputMoney();		// OK
				break;
			default:
				break;
		}
	}
}

void Get_InputMoney( void )
{
	ST_FLOAT Money = 0.0;
	ReadRegister_16bit( 282, 2 );        				// Read Money

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) )      // Analyse received data
	{
		Arry2Float( &Uart[SCREENCOM].revbuf[3], &Money );
#if Debug
		printf( "Money = %f\n", Money );

		printf( "Get_InputMoney**************************:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
#endif
#if Debug
		printf( "Get_InputMoney Read OK! \n" );
#endif

		if( fabs( Money ) <= EPSILON)
		{
			Window_ID = W_InputErr;
		}
		else
		{
			SetChargePara.SetChargeMoney = Money; // save user selected charge electric
#if Debug
			printf( "##########################################Money = %f\n",
				Money );
#endif
			Money = 0.0;			// clear static
			Input_Preprocess();						// Pat card or open the door
			//Window_ID = W_CntPlug;
			//ClearRegister_16bit(282,2);
		}
	}
#if Debug
	printf( "SetChargePara.SetChargeMoney %f**************************:\n",
		SetChargePara.SetChargeMoney );
#endif
}

/*********************************************************************************************************
 ** Function name:       W_InputTime_Fction
 ** Descriptions:       	Show input Money window
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_InputTime_Fction( void )
{
	TimeOut( 120, 294, W_ChargeByTime );// 3 minute no operation,return to W_ChargeByTime
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_INPUT_TIME, NULL );
	}
	ClearRegister_16bit( 68, 1 );				// clear input hour     in blank
	//Key_Input_Time(292);
	//Key_Input_Time( 292, W_CntPlug, 298, 299 );

	ReadRegisterLB( 290, 2 );    // Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( ( Sidekey != 0) )
		{
			if( Sidekey == 0x02 )
			{
				Uart[SCREENCOM].revbuf[3] = 0x02;
				Sidekey = 0;
			}
			else if( Sidekey == 0x08 )
			{
				Uart[SCREENCOM].revbuf[3] = 0x01;
				Sidekey = 0;
			}
		}
		#if Debug
		printf( "W_InputTime_Fction Read OK! \n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
#endif

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_ChargeByTime;          // Return
				//Clear_Blank();
				break;
			case 0x02:
				//Clear_KeyboardBuff();
				Get_InputTime();		// OK

				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_CardLock_R_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CardLock_R_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );							// clear buffer
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( PAIDLASTFEE, NULL );
	}

	TimeOut( 30, 375, W_Welcom );							// Time out process
#if Debug																								// For Debug
	DisplayInfo.LastChargeEle = 10.25;					//	Last charge electric
	DisplayInfo.LastChargeMoney = 5.13;						// Last charge money
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.LastChargeEndTime[0] = 3;
	DisplayInfo.LastChargeEndTime[1] = 5;
	DisplayInfo.LastChargeEndTime[2] = 17;
	DisplayInfo.LastChargeEndTime[3] = 14;
	DisplayInfo.LastChargeEndTime[4] = 8;
	DisplayInfo.LastChargeEndTime[5] = 2011;
#endif

	Float2Arry( DisplayInfo.LastChargeEle, &Data[0] );  //	Last charge electric
	Float2Arry( DisplayInfo.LastChargeMoney, &Data[4] );	// Last charge money
	Float2Arry( DisplayInfo.CardBalance, &Data[8] );  	// CardBalance

	Data[13] = DisplayInfo.LastChargeEndTime[0];					// Second
	Data[15] = DisplayInfo.LastChargeEndTime[1];					// Minute
	Data[17] = DisplayInfo.LastChargeEndTime[2];						// Hour
	Data[19] = DisplayInfo.LastChargeEndTime[3];					// Date
	Data[21] = DisplayInfo.LastChargeEndTime[4];						// Month
	Data[22] = DisplayInfo.LastChargeEndTime[5] >> 8;				// year High
	Data[23] = DisplayInfo.LastChargeEndTime[5] & 0x00FF;	// year Low

	WriteRegister_16bit( 350, 2, &Data[0] );           	// Last charge electric
	WriteRegister_16bit( 352, 2, &Data[4] );           	//	Last charge money
	WriteRegister_16bit( 373, 2, &Data[8] );          		// CardBalance
	//WriteRegister_16bit( 354, 6, &Data[12] );          	// Last charge time
	WriteRegister_16bit( 354, 2, &Data[12] );          	// Last charge time
	WriteRegister_16bit( 356, 2, &Data[16] );          	// Last charge time
	WriteRegister_16bit( 358, 2, &Data[20] );          	// Last charge time

	ReadRegisterLB( 366, 1 ); 		// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_Welcom;
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       W_WaitCharge_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_WaitCharge_Fction( void )
{
#if Union_Debug
#if Debug
	DisplayInfo.SysTime[2] = 8;	// System time Hour
	DisplayInfo.SysTime[1] = 50;	// System time Minute
	DisplayInfo.SysTime[0] = 35;	// System time Second
	ST_UCHAR flag = 0;
	flag = getchar();
	EventFlag.StartChargeFlag = 0;
	if( flag == '1' )
	{
		switch( SetChargePara.SetChargeMode )
		// Charging mode
		{
			case M_ChargeEle:
				Window_ID = W_ChargingEle;
				break;
			case M_ChargeMoney:
				Window_ID = W_ChargingMoney;
				break;
			case M_ChargeTime:
				Window_ID = W_ChargingTime;
				break;
			case M_AutoFull:
				Window_ID = W_ChargingAutoFull;
				break;
			default:
				break;
		}
	}
	else if( flag == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
	}
	else if( flag == '3' )
	{
		EventFlag.IsPatCardFlag = 1;
	}
#endif
#endif

	ST_UCHAR Data[20];
	ST_CHAR Systime[3];
	ST_CHAR Pretime[3];
	ST_CHAR Dwntime[3];
	ST_UCHAR PreHour = 0;
	ST_UCHAR PreMinute = 0;

	memset( Data, 0, sizeof( Data ) );							// clear buffer
	memset( Systime, 0, sizeof( Systime ) );					// clear buffer
	memset( Pretime, 0, sizeof( Pretime ) );					// clear buffer
	memset( Dwntime, 0, sizeof( Dwntime ) );					// clear buffer

	PreHour = SetChargePara.PreChargeTime[1]; 		// Precharge Hour
	PreMinute = SetChargePara.PreChargeTime[0]; 	// Precharge Minute

	Systime[2] = ( ST_CHAR ) DisplayInfo.SysTime[2];		// System time Hour
	Systime[1] = ( ST_CHAR ) DisplayInfo.SysTime[1];	// System time Minute
	Systime[0] = ( ST_CHAR ) DisplayInfo.SysTime[0];	// System time Second
	Pretime[2] = ( ST_CHAR ) SetChargePara.PreChargeTime[1];// Precharge Hour
	Pretime[1] = ( ST_CHAR ) SetChargePara.PreChargeTime[0];// Precharge Minute
	Pretime[0] = 0;											// Precharge Second

	//if( Systime[2] == PreHour && Systime[1] == PreMinute )
	if( EventFlag.StartChargeFlag == 1 )				// start to charge
	{
		// reach precharge time
		switch( SetChargePara.SetChargeMode )
		// Charging mode
		{
			case M_ChargeEle:
				Window_ID = W_ChargingEle;
				break;
			case M_ChargeMoney:
				Window_ID = W_ChargingMoney;
				break;
			case M_ChargeTime:
				Window_ID = W_ChargingTime;
				break;
			case M_AutoFull:
				Window_ID = W_ChargingAutoFull;
				break;
			default:
				break;
		}
	}
	Down_Time( Systime, Pretime, Dwntime );		// calculate rest time

	Data[1] = DisplayInfo.SysTime[2];	// System time Hour
	Data[3] = DisplayInfo.SysTime[1];	// System time Minute
	Data[5] = DisplayInfo.SysTime[0];	// System time Second

	Data[7] = Dwntime[2];		// rest time Hour
	Data[9] = Dwntime[1];		// rest time Minute
	Data[11] = Dwntime[0];		// rest time Second

	//WriteRegister_16bit( 360, 6, &Data[0] );           	// Show system time
	WriteRegister_16bit( 360, 2, &Data[0] );           	// Show system time
	WriteRegister_16bit( 362, 2, &Data[4] );           	// Show system time
	WriteRegister_16bit( 364, 2, &Data[8] );           	// Show system time

	if( EventFlag.NoSameCardFlag == 1 )						// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// Pat card to quit waiting precharge
	{
		EventFlag.IsPatCardFlag = 0;
		Window_ID = W_OpenDoor_End;
	}
}

void W_Precharge_Confirm_Fction( void )
{
	ST_UCHAR Data[20];
	ST_CHAR Systime[3];
	ST_CHAR Pretime[3];
	ST_CHAR Dwntime[3];
	TimeOut( Operation_Delay, 222, W_Welcom );				// Time out process
	memset( Data, 0, sizeof( Data ) );							// clear buffer
	memset( Systime, 0, sizeof( Systime ) );					// clear buffer
	memset( Pretime, 0, sizeof( Pretime ) );					// clear buffer
	memset( Dwntime, 0, sizeof( Dwntime ) );					// clear buffer

	Systime[2] = ( ST_CHAR ) DisplayInfo.SysTime[2];		// System time Hour
	Systime[1] = ( ST_CHAR ) DisplayInfo.SysTime[1];	// System time Minute
	Systime[0] = ( ST_CHAR ) DisplayInfo.SysTime[0];	// System time Second
	Pretime[2] = ( ST_CHAR ) SetChargePara.PreChargeTime[1];// Precharge Hour
	Pretime[1] = ( ST_CHAR ) SetChargePara.PreChargeTime[0];// Precharge Minute
	Pretime[0] = 0;											// Precharge Second

	Down_Time( Systime, Pretime, Dwntime );		// calculate rest time

	Data[1] = DisplayInfo.SysTime[2];	// System time Hour
	Data[3] = DisplayInfo.SysTime[1];	// System time Minute
	Data[5] = DisplayInfo.SysTime[0];	// System time Second

	Data[7] = Dwntime[2];		// rest time Hour
	Data[9] = Dwntime[1];		// rest time Minute
	Data[11] = Dwntime[0];		// rest time Second

	//WriteRegister_16bit(360, 6 , &Data[0]);           	// Show system time
	WriteRegister_16bit( 360, 1, &Data[0] );           	// Show system time
	WriteRegister_16bit( 361, 1, &Data[2] );           	// Show system time
	WriteRegister_16bit( 362, 1, &Data[4] );           	// Show system time
	WriteRegister_16bit( 363, 1, &Data[6] );           	// Show system time
	WriteRegister_16bit( 364, 1, &Data[8] );           	// Show system time
	WriteRegister_16bit( 365, 1, &Data[10] );           	// Show system time

	ReadRegisterLB( 220, 2 ); 	// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // available data
	{
		if( ( Sidekey != 0) )		// side key available
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_Precharge_Confirm_Fction Read OK! \n" );
#endif
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_TimeCharge;
				ClearRegister_16bit( 252, 2 );// clear input  precharge time in blank
				Clear_KeyboardBuff();
				break;
			case 0x02:
				Window_ID = W_ImmediatelyCharge;
				ClearRegister_16bit( 252, 2 );// clear input  precharge time in blank
				Clear_KeyboardBuff();
				break;
			default:
				break;
		}
	}
}
/*********************************************************************************************************
 ** Function name:       W_CardLock_N_Fction
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CardLock_N_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );						// clear buffer
#if Debug 																				// For Debug
	DisplayInfo.CardBalance = 99996.65;
#endif
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( CARDLOCKED, NULL );
	}
	TimeOut( 30, 372, W_Welcom );		// Time out process

	Float2Arry( DisplayInfo.CardBalance, Data );  	// CardBalance
	WriteRegister_16bit( 370, 2, Data );          	// CardBalance

	ReadRegisterLB( 376, 1 ); 		// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_Welcom;
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:       Get_InputTime
 ** Descriptions:
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Get_InputTime( void )
{
	ST_UCHAR Hour = 0;
	ST_UCHAR Minute = 0;
	ReadRegister_16bit( 292, 2 );        				// Read hour, minute

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) )      // Analyse received data
	{
		Hour = Uart[SCREENCOM].revbuf[4];			// get hour
		Minute = Uart[SCREENCOM].revbuf[6];		// get minute
#if Debug
		printf( "Hour = %d, Minute = %d\n", Hour, Minute );
		printf( "Get_InputTime**************************:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
#endif
#if Debug
		printf( "Get_InputTime Read OK! \n" );
#endif

		if( Hour == 0 && Minute == 0 )
		{
			Window_ID = W_InputErr;
		}
		else
		{
			SetChargePara.SetChargeTime[1] = Minute;	// save minute
			SetChargePara.SetChargeTime[2] = Hour;	// save hour
#if Debug
			printf( "SetChargePara.SetChargeTime Hour = %d, Minute = %d\n",
				SetChargePara.SetChargeTime[2],
				SetChargePara.SetChargeTime[1] );
#endif
			Hour = 0;
			Minute = 0;
			Input_Preprocess();
			//Window_ID = W_OpenDoor;								// Open door
			//Window_ID = W_CntPlug;
			//ClearRegister_16bit(292,2);
		}
	}
}
/*********************************************************************************************************
 ** Function name:       TestCom_2
 ** Descriptions:						Open Com2
 ** input parameters:
 ** output parameters:
 ** Returned value:
 *********************************************************************************************************/
#if 0
void TestCom_2(ST_CHAR *com,ST_INT32 *comfd_r,const ST_CHAR DisplayComID)
{
	comfd_h= comfd_r;
	ST_INT32 ret = 0;
	if((ret = OpenCom(com)>0))
	{
		printf("open com2 OK!\n");
	}
	else
	{
		printf("open com2 failed\n");
	}
}
#endif						

void ComTest( void )
{
	//TestCom_2(HongWaiCom,&Hongwaifd,COM1_ID);		// Open HongWaiCom, for Key
	TestCom();						// Open MeterCom, for LCD
}

void DisplayControl( void )
{
	ComTest();
}
/*********************************************************************************************************
 ** Function name:       ModbusAnalysis
 ** Descriptions:
 ** input parameters:
 ** output parameters:
 ** Returned value:
 *********************************************************************************************************/ST_UCHAR ModbusAnalysis(
	ST_UCHAR *rcvbuf )
{
	ST_UINT16 crc, temp;
	ST_UINT16 n = 0;
	if( rcvbuf[0] != 0x01 )
		return 0;
	switch( rcvbuf[1] )
	{
		case 0x02:
			case 0x06:
			case 0x10:
			crc = CRC16( rcvbuf, 6 );
			temp = ( rcvbuf[6] << 8 ) + rcvbuf[7];
			if( crc != temp )
				return 0;
			return 1;
			break;
		case 0x04:
			n = rcvbuf[2] + 3;				// rcvbuf[2] = data length
			crc = CRC16( rcvbuf, n );
			temp = ( rcvbuf[n] << 8 ) + rcvbuf[n + 1];
			if( crc != temp )
				return 0;
			return 1;
			break;
		default:
			break;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:	ReadRegisterLB
 ** Descriptions:     	Read button status
 ** input parameters:    startAddr   : start register
 **			registerNUm : register number
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void WriteRegisterLB( ST_INT16 startAddr, ST_UCHAR ON_OFF )
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;
	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );

	if( ON_OFF == ON)
	{
		Uart[SCREENCOM].sendbuf[4] = 0xFF;
		Uart[SCREENCOM].sendbuf[5] = 00;
	}
	else
	{
		Uart[SCREENCOM].sendbuf[4] = 00;
		Uart[SCREENCOM].sendbuf[5] = 00;
	}

	Uart[SCREENCOM].sendbuf[0] = 0x01;	// Station ID
	Uart[SCREENCOM].sendbuf[1] = 0x05;	// Function code
	Uart[SCREENCOM].sendbuf[2] = ( startAddr >> 8 ) & 0xFF;		//	Addr Hi
	Uart[SCREENCOM].sendbuf[3] = startAddr & 0xFF;					//	Addr Lo

	crc = CRC16( Uart[SCREENCOM].sendbuf, 6 );
	Uart[SCREENCOM].sendbuf[6] = ( crc & 0xFF00 ) >> 8;
	Uart[SCREENCOM].sendbuf[7] = ( crc & 0x00FF);
	write( Screenfd, Uart[SCREENCOM].sendbuf, 8 );	// Send data

#if Debug
	printf( "Send Data:\n" );
	Show_Buffer( Uart[SCREENCOM].sendbuf, 8 );
#endif

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );
	memset( Uart[SCREENCOM].revbuf, 0, sizeof( Uart[SCREENCOM].revbuf ) );
	rxnum = ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 ); // uart receive function

	if( rxnum > 0 )
	{
		SCREENerr_num = 0;
#if Debug
		printf( "rxnum is %d\n", rxnum );
		printf( "Read Receive data:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 10 );
#endif
	}
	else
	{
		SCREENerr_num++;
#if Debug
		printf( "time out\n" );
#endif
	}
}
/*********************************************************************************************************
 ** Function name:	ReadRegisterLB
 ** Descriptions:     	Read button status
 ** input parameters:    startAddr   : start register
 **			registerNUm : register number
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void ReadRegisterLB( ST_INT16 startAddr, ST_UCHAR resisterNum )
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;

	Uart[SCREENCOM].sendbuf[0] = 0x01;	// Station ID
	Uart[SCREENCOM].sendbuf[1] = 0x02;	// Read bit data
	Uart[SCREENCOM].sendbuf[2] = ( startAddr >> 8 ) & 0xFF;
	Uart[SCREENCOM].sendbuf[3] = startAddr & 0xFF;
	Uart[SCREENCOM].sendbuf[5] = resisterNum;
	crc = CRC16( Uart[SCREENCOM].sendbuf, 6 );
	Uart[SCREENCOM].sendbuf[6] = ( crc & 0xFF00 ) >> 8;
	Uart[SCREENCOM].sendbuf[7] = ( crc & 0x00FF);
	write( Screenfd, Uart[SCREENCOM].sendbuf, 8 );	// Send data

#if Debug
	printf( "Send Data:\n" );
	Show_Buffer( Uart[SCREENCOM].sendbuf, 8 );
#endif

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );
	memset( Uart[SCREENCOM].revbuf, 0, sizeof( Uart[SCREENCOM].revbuf ) );
	rxnum = ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 ); // uart receive function

	if( rxnum > 0 )
	{
		SCREENerr_num = 0;
#if Debug
		printf( "rxnum is %d\n", rxnum );
		printf( "Read Receive data:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 10 );
#endif
	}
	else
	{
		SCREENerr_num++;
#if Debug
		printf( "time out\n" );
#endif
	}
}

#if 0
/*********************************************************************************************************
 ** Function name:	WriteRegisterLB
 ** Descriptions:     	Read button status
 ** input parameters:    startAddr   : start register
 **			registerNUm : register number
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void WriteRegisterLB(ST_INT16 startAddr,ST_UCHAR resisterNum)
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;

	Uart[SCREENCOM].sendbuf[0] = 0x01;	// Station ID
	Uart[SCREENCOM].sendbuf[1] = 0x02;// Read bit data
	Uart[SCREENCOM].sendbuf[2] = (startAddr >> 8)& 0xFF;
	Uart[SCREENCOM].sendbuf[3] = startAddr& 0xFF;
	Uart[SCREENCOM].sendbuf[5] = resisterNum;
	crc = CRC16(Uart[SCREENCOM].sendbuf,6);
	Uart[SCREENCOM].sendbuf[6] = (crc &0xFF00) >> 8;
	Uart[SCREENCOM].sendbuf[7] = (crc &0x00FF);
	write(Screenfd,Uart[SCREENCOM].sendbuf,8);// Send data

#if Debug
	printf("Send Data:\n");
	Show_Buffer(Uart[SCREENCOM].sendbuf, 8);
#endif

	memset(Uart[SCREENCOM].sendbuf,0,sizeof(Uart[SCREENCOM].sendbuf));
	memset(Uart[SCREENCOM].revbuf,0,sizeof(Uart[SCREENCOM].revbuf));
	rxnum = ReadCom(Screenfd,Uart[SCREENCOM].revbuf, sizeof(Uart[SCREENCOM].revbuf), Com_Delay*1000); // uart receive function

#if Debug
	if(rxnum>0)
	{
		printf("rxnum is %d\n",rxnum);
		printf("Read Receive data:\n");
		Show_Buffer(Uart[SCREENCOM].revbuf, 10);
	}
	else
	{
		printf("time out\n");
	}
#endif
}
#endif
/*********************************************************************************************************
 ** Function name:       WriteRegister_16bit
 ** Descriptions:        Write register
 ** input parameters:    startAddr   : start register
 **                      registerNUm : register number
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void WriteRegister_16bit( ST_INT16 startAddr, ST_UCHAR resisterNum,
	ST_UCHAR Data[] )
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;
	ST_UCHAR datalen = 2 * resisterNum;
	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );
	Uart[SCREENCOM].sendbuf[0] = 0x01;	// Station ID
	Uart[SCREENCOM].sendbuf[1] = 0x10;	// Write 16bit register

	Uart[SCREENCOM].sendbuf[2] = ( startAddr >> 8 ) & 0xFF;
	Uart[SCREENCOM].sendbuf[3] = startAddr & 0xFF;

	Uart[SCREENCOM].sendbuf[5] = resisterNum;
	Uart[SCREENCOM].sendbuf[6] = datalen;
	memcpy( &Uart[SCREENCOM].sendbuf[7], Data, datalen );

	crc = CRC16( Uart[SCREENCOM].sendbuf, datalen + 7 );
	Uart[SCREENCOM].sendbuf[datalen + 7] = ( crc & 0xFF00 ) >> 8;
	Uart[SCREENCOM].sendbuf[datalen + 8] = ( crc & 0x00FF);
	write( Screenfd, Uart[SCREENCOM].sendbuf, datalen + 9 );	// Send data

#if Debug
	printf( "Send Data:\n" );
	Show_Buffer( Uart[SCREENCOM].sendbuf, datalen + 9 );
#endif

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) ); // Clear send bufer
	memset( Uart[SCREENCOM].revbuf, 0, sizeof( Uart[SCREENCOM].revbuf ) ); // Clear receive buffer
	rxnum = ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 ); // uart receive function

	if( rxnum > 0 )
	{
		SCREENerr_num = 0;
#if Debug
		printf( "rxnum is %d\n", rxnum );
		printf( "Read Receive data:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 10 );
#endif
	}
	else
	{
		SCREENerr_num++;
#if Debug
		printf( "time out\n" );
#endif
	}
}

/*********************************************************************************************************
 ** Function name:       ReadRegister_16bit
 ** Descriptions:        Read Data which is in the Edit blank
 ** input parameters:    startAddr   : start register
 **                      registerNUm : register number
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void ReadRegister_16bit( ST_INT16 startAddr, ST_UCHAR resisterNum )
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );
	Uart[SCREENCOM].sendbuf[0] = 0x01;	// Station ID
	Uart[SCREENCOM].sendbuf[1] = 0x04;	// Read 16bit register

	Uart[SCREENCOM].sendbuf[2] = ( startAddr >> 8 ) & 0xFF;
	Uart[SCREENCOM].sendbuf[3] = startAddr & 0xFF;

	Uart[SCREENCOM].sendbuf[5] = resisterNum;
	crc = CRC16( Uart[SCREENCOM].sendbuf, 6 );
	Uart[SCREENCOM].sendbuf[6] = ( crc & 0xFF00 ) >> 8;
	Uart[SCREENCOM].sendbuf[7] = ( crc & 0x00FF);
	write( Screenfd, Uart[SCREENCOM].sendbuf, 8 );	// Send data

#if Debug
	printf( "ReadRegister_16bit Send Data:\n" );
	Show_Buffer( Uart[SCREENCOM].sendbuf, 8 );
#endif

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) ); // Clear send bufer
	memset( Uart[SCREENCOM].revbuf, 0, sizeof( Uart[SCREENCOM].revbuf ) ); // Clear receive buffer
	rxnum = ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 ); // uart receive function

	if( rxnum > 0 )
	{
		SCREENerr_num = 0;
#if Debug
		printf( "rxnum is %d\n", rxnum );
		printf( "Read Receive data:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 10 );
#endif
	}
	else
	{
		SCREENerr_num++;
#if Debug
		printf( "time out\n" );
#endif
	}
}

/*********************************************************************************************************
 ** Function name:       ClearRegister_16bit
 ** Descriptions:        Clear Data which is in the Edit blank
 ** input parameters:    startAddr   : start register
 **                      registerNUm : register number
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void ClearRegister_16bit( ST_INT16 startAddr, ST_UCHAR resisterNum )
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;
	ST_UCHAR datalen = 2 * resisterNum;
	ST_UCHAR data[30];
	memset( data, 0, sizeof( data ) );
	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );
	Uart[SCREENCOM].sendbuf[0] = 0x01;      // Station ID
	Uart[SCREENCOM].sendbuf[1] = 0x10;      // Write 16bit register

	Uart[SCREENCOM].sendbuf[2] = ( startAddr >> 8 ) & 0xFF;
	Uart[SCREENCOM].sendbuf[3] = startAddr & 0xFF;

	Uart[SCREENCOM].sendbuf[5] = resisterNum;
	Uart[SCREENCOM].sendbuf[6] = datalen;
	memcpy( &Uart[SCREENCOM].sendbuf[7], data, datalen );

	crc = CRC16( Uart[SCREENCOM].sendbuf, datalen + 7 );
	Uart[SCREENCOM].sendbuf[datalen + 7] = ( crc & 0xFF00 ) >> 8;
	Uart[SCREENCOM].sendbuf[datalen + 8] = ( crc & 0x00FF);
	write( Screenfd, Uart[SCREENCOM].sendbuf, datalen + 9 );      // Send data
#if Debug
	printf( "Send Data:\n" );
	Show_Buffer( Uart[SCREENCOM].sendbuf, datalen + 9 );
#endif
	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) ); // Clear send bufer
	memset( Uart[SCREENCOM].revbuf, 0, sizeof( Uart[SCREENCOM].revbuf ) ); // Clear receive buffer
	rxnum = ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 ); // uart receive function
#if Debug
	if( rxnum > 0 )
	{
		printf( "rxnum is %d\n", rxnum );
		printf( "Read Receive data:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 10 );
	}
	else
	{
		printf( "time out\n" );
	}
#endif
}

/*********************************************************************************************************
 ** Function name:       CRC16
 ** Descriptions:
 ** input parameters:
 ** output parameters:
 ** Returned value:
 ********************************************************************************************************/
unsigned short CRC16( const unsigned char *ptr, int Len )
{
	int i, j;
	unsigned short polinomio = 0xA001;
	unsigned short result = 0xFFFF;
	unsigned short CRCResult;
	for( i = 0; i < Len; i++ )
	{
		unsigned char tmp = ptr[i];
		result = ( unsigned short ) ( tmp ^ result);
		for( j = 0; j < 8; j++ )
		{
			if( ( result & 0x0001 ) == 1 )
			{
				result = ( unsigned short ) ( ( result >> 1 ) ^ polinomio);
			}
			else
			{
				result = ( unsigned short ) ( result >> 1);
			}
		}
	}
	CRCResult = ( unsigned short ) ( ( result << 8 ) + ( result >> 8 ));
	return CRCResult;
}

void Change_Window( ST_UCHAR W_num )
{
	ST_UINT16 crc;
	ST_INT32 rxnum = 0;

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) );
	Uart[SCREENCOM].sendbuf[0] = 0x01;	// station ID
	Uart[SCREENCOM].sendbuf[1] = 06;		// write single register
	Uart[SCREENCOM].sendbuf[2] = 0x00;	// window register Hi
	Uart[SCREENCOM].sendbuf[3] = 0x00;	// window register Lo
	Uart[SCREENCOM].sendbuf[4] = 0x00;	// window num Hi
	Uart[SCREENCOM].sendbuf[5] = W_num; // window num Lo

	crc = CRC16( Uart[SCREENCOM].sendbuf, 6 );
	Uart[SCREENCOM].sendbuf[6] = ( crc & 0xFF00 ) >> 8;
	Uart[SCREENCOM].sendbuf[7] = ( crc & 0x00FF);
	write( Screenfd, Uart[SCREENCOM].sendbuf, 8 );

#if Debug
	printf( "Send Data:\n" );
	Show_Buffer( Uart[SCREENCOM].sendbuf, 8 );
#endif

	memset( Uart[SCREENCOM].sendbuf, 0, sizeof( Uart[SCREENCOM].sendbuf ) ); // Clear send bufer
	memset( Uart[SCREENCOM].revbuf, 0, sizeof( Uart[SCREENCOM].revbuf ) ); // Clear receive buffer
	rxnum = ReadCom( Screenfd, Uart[SCREENCOM].revbuf,
		sizeof( Uart[SCREENCOM].revbuf ), Com_Delay * 1000 ); // uart receive function

#if Debug
	if( rxnum > 0 )
	{
		printf( "rxnum is %d\n", rxnum );
		printf( "Read Receive data:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 10 );
	}
	else
	{
		printf( "time out\n" );
	}
#endif
}

/*********************************************************************************************************
 ** Function name:      	TimeOut
 ** Descriptions:       	when time out, change to last window
 ** input parameters:    Time : time out time
 **                     	RegAddr : write time on which register
 **															LastWind: when time out , change to which window
 ** output parameters:   None
 ** Returned value:     	None
 ** Created by:         	Brian.Yang
 ** Created Date:       	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void TimeOut( ST_UINT16 Time, ST_INT16 RegAddr, ST_UCHAR LastWind )
{
	//static ST_UCHAR Crrnt_Window = 0;
	ST_UCHAR C_Secnd = 0;
	static ST_INT16 cnt = -1;
	static ST_CHAR L_Secnd = -1;
	static ST_UINT16 timeout = 0;

	ReadRegister_16bit( 9017, 1 );			// Read second register, get second
	C_Secnd = Uart[SCREENCOM].revbuf[4]; 	// save second
	memset( Tem_Data, 0, sizeof( Tem_Data ) ); // Clear temp buffer
	if( CLR_Timeout == 1 )
	{
		cnt = -1;
		L_Secnd = -1;
		CLR_Timeout = 0;
	}
	if( C_Secnd != L_Secnd )				// second changed
	{
		L_Secnd = C_Secnd;
		cnt++;
		timeout = Time - cnt;
		Tem_Data[0] = ( timeout & 0xFF00 ) >> 8;
		Tem_Data[1] = timeout & 0x00FF;
		WriteRegister_16bit( RegAddr, 1, Tem_Data );

		if( timeout == 0 )					// time out, change window
		{
			Window_ID = LastWind;
			cnt = -1;
			L_Secnd = -1;
		}
	}
}

/*********************************************************************************************************
 ** Function name:      	Clear_Blank
 ** Descriptions:        Clear all data write in blank
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Clear_Blank( void )
{
	if( CLR_Flag == 1 )
	{
		ClearRegister_16bit( 48, 1 );		// clear select electric in blank
		ClearRegister_16bit( 58, 1 );		// clear select money    in blank
		ClearRegister_16bit( 68, 1 );		// clear select hour     in blank
		ClearRegister_16bit( 252, 2 );	// clear input  precharge time in blank
		ClearRegister_16bit( 272, 2 );		// clear input  electric in blank
		ClearRegister_16bit( 282, 2 );		// clear input  money    in blank
		ClearRegister_16bit( 292, 2 );		// clear input  time     in blank
		CLR_Flag = 0;
	}
}
/*********************************************************************************************************
 ** Function name:      	W_OpenDoor_CntPlug_Fction
 ** Descriptions:
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_OpenDoor_Fction( void )
{
	Audio_Time_Alarm( Audio_Time_Slot, AUDIO_OPENDOOR );
	TimeOut( OpenDoor_Delay, 490, W_Welcom );			// Time out process
	if( CLR_Flag == 1 )
	{
		Clear_KeyboardBuff();
		Clear_Blank();
	}
	OpenDoor();
	if( CheckDoorIsOpen() == 1 )			// Door is open
#if Debug
	EventFlag.QiangConGood = 0;
	ST_CHAR flag = 0;
	printf( "Open door:" );
	flag = getchar();
	if( flag == '1' )
	#endif
	{
		Window_ID = W_CntPlug;
	}

}

/*********************************************************************************************************
 ** Function name:      	W_CloseDoor_Fction
 ** Descriptions:
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CloseDoor_Fction( void )
{
	Audio_Time_Alarm( Audio_Time_Slot, CLOSEDOOR );				// 关闭充电舱门语音提示
	TimeOut( Operation_Delay, 700, W_Welcom );			// Time out process

#if Debug
	ST_CHAR flag = 0;
	printf( "Close door:" );
	flag = getchar();
	if( flag == '1' )
	#endif
	CloseDoor();
	if(CheckDoorIsClosed() == 1)
	{
		Window_ID = W_PatCard_Button;
	}
}

/*********************************************************************************************************
 ** Function name:      	W_OpenDoor_End_Fction
 ** Descriptions:
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_OpenDoor_End_Fction( void )
{
	Audio_Time_Alarm( Audio_Time_Slot, AUDIO_OPENDOOR );
	err_num = 0;
	TimeOut( OpenDoor_Delay, 710, W_Welcom );			// Time out process

#if Debug
	ST_CHAR flag = 0;
	printf( "OpenDoor_End:" );
	flag = getchar();
	if( flag == '1' )
	#endif
	OpenDoor();
		if( CheckDoorIsOpen() == 1 )			// Door is open
	{
		Window_ID = W_RemovePlug;					// remove plug
	}
}

/*********************************************************************************************************
 ** Function name:      	W_CloseDoor_End_Fction
 ** Descriptions:
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CloseDoor_End_Fction( void )
{
	Audio_Time_Alarm( Audio_Time_Slot, CLOSEDOOR );
	TimeOut( Operation_Delay, 720, W_ThanksEnd );			// Time out process
#if Debug
	ST_CHAR flag = 0;
	printf( "CloseDoor_End :" );
	flag = getchar();
	if( flag == '1' )
	#endif
	CloseDoor();
	if( CheckDoorIsClosed() == 1 )			// Door is open
	{
		//Window_ID = W_PayMoney;		// show pay money
		Window_ID = W_ThanksEnd;
	}
}

/*********************************************************************************************************
 ** Function name:      	W_Balance_little_Fction
 ** Descriptions:						Balance is not enough, please charge your money
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Balance_little_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );						// clear buffer
#if Debug 																				// For Debug
	DisplayInfo.CardBalance = 0.65;
#endif
	if( CLR_Sound == 1 )
	{
		CLR_Sound = 0;
		PlaySound( BANLANCENOTENOUGH, NULL );
	}
	TimeOut( OpenDoor_Delay, 732, W_OpenDoor_End );		// Time out process

	Float2Arry( DisplayInfo.CardBalance, Data );  	// CardBalance
	WriteRegister_16bit( 730, 2, Data );          	// CardBalance

	ReadRegisterLB( 733, 1 ); 		// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyze received data
	{
		if( Sidekey == 0x02 )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_OpenDoor_End;
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:      	W_Input_Again_Fction
 ** Descriptions:						Balance is not enough, please input again
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Input_Again_Fction( void )
{
	ST_UCHAR Data[10];
	memset( Data, 0, sizeof( Data ) );						// clear buffer

	Clear_Blank();
	TimeOut( 60, 740, W_ImmediatelyCharge );		// Time out process

#if Debug																// for Debug
	DisplayInfo.CardBalance = 14.94;
	DisplayInfo.MaxChargeEle = 14.94;
#endif
	Float2Arry( DisplayInfo.CardBalance, &Data[0] );  	// CardBalance
	Float2Arry( DisplayInfo.MaxChargeEle, &Data[4] );  // Max Charge Ele
	WriteRegister_16bit( 743, 2, &Data[0] );					// Show number
	WriteRegister_16bit( 745, 2, &Data[4] );          	// Show number

	ReadRegisterLB( 741, 2 ); 	// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // available data
	{
		if( ( Sidekey != 0) )		// side key available
		{
			Uart[SCREENCOM].revbuf[3] = Sidekey;
			Sidekey = 0;
		}
		#if Debug
		printf( "W_Input_Again_Fction Read OK! \n" );
#endif
		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = W_OpenDoor_End;				// quit

				break;
			case 0x02:
				Window_ID = W_ImmediatelyCharge; // choose again

				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:      	W_InputErr_Quit_Fction
 ** Descriptions:						Balance is not enough, please input again
 ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_InputErr_Quit_Fction( void )
{
	Clear_Blank();
	TimeOut( 5, 750, W_OpenDoor_End );		// Time out process
}
/*********************************************************************************************************
 ** Function name:      	SysInit_Clr
 ** Descriptions:						clear flag and register
 ** ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void SysInit_Clr( void )
{
	if( CLR_Flag == 1 )
	{
		DisplayInfo.FindLastChargeRec = 0;
		Clear_Blank();
		Clear_KeyboardBuff();
		SetChargePara.SetChargeMode = 0;//用户选择的充电模式，1：按电量充电；2：按金额充电；3：按时间充电；4：自动充满，5：查询余额。
		SetChargePara.SetChargeEle = 0.0;			//按电量充电时，设定的充电电量
		SetChargePara.SetChargeMoney = 0.0;		//按金额充电时，设定的充电金额
		memset( SetChargePara.SetChargeTime, 0,
			sizeof( SetChargePara.SetChargeTime ) );		//按时间充电时，设定的充电时间
		SetChargePara.SetStartChargeMode = 0;	//用户选择的启动充电模式。1：预约充电；0：立即充电
		memset( SetChargePara.PreChargeTime, 0,
			sizeof( SetChargePara.PreChargeTime ) ); //通过界面设定的预充时间
		//Initial in normal state
		MeterExcepFlag_Stus = 0;
		CardExcepFlag_Stus = 0;
		CutdownEleFlag_Stus = 0;
		RepairDoorOpenFg_Stus = 0;
		ScreenExcepFlag_Stus = 0;
		ProtolLandFlag_Gprs_Stus = 1;
		s_ProtolLandFlag_Stus = 1;
		DefalutPhaseFlag_Stus = 0;
		//Printer_Status = 0;
		Normal_Status = 0;
		Abnormal_Status = 0;

#if Debug
		EventFlag.ChargeFullFlag = 0;
		EventFlag.ChargeCompleteFlag = 0;
		EventFlag.QiangConGood = 0;
		DisplayInfo.CardBalance = 100.0;
#endif
		CLR_Flag = 0;

		ST_UCHAR Data[50];
		memset( Data, 0, sizeof( Data ) );
		/*clear charging Ele register*/
		WriteRegister_16bit( 8990, 2, &Data[0] );         // ID
		WriteRegister_16bit( 8992, 2, &Data[4] );         // ID
		WriteRegister_16bit( 8994, 2, &Data[8] );         // ChargedEle
		WriteRegister_16bit( 8996, 2, &Data[12] );         // ChargingEle
		WriteRegister_16bit( 8998, 2, &Data[16] );         // ChargedMoney
		WriteRegister_16bit( 8980, 2, &Data[20] );         // CardBalance
		WriteRegister_16bit( 8982, 2, &Data[24] );						// Time
		WriteRegister_16bit( 8984, 1, &Data[28] );						// Time

		/*clear charging Money register*/
		/*clear Finish full money register*/
		/*clear Finish Abnmal Money register*/
		/*clear Finish Nomoney Money register*/
		WriteRegister_16bit( 8960, 2, &Data[0] );          		// ID
		WriteRegister_16bit( 8962, 2, &Data[4] );          		// ID
		WriteRegister_16bit( 8964, 2, &Data[8] );          		// ChargedMoney
		WriteRegister_16bit( 8966, 2, &Data[12] );         		// ChargingMoney
		WriteRegister_16bit( 8968, 2, &Data[16] );          	// ChargedEle
		WriteRegister_16bit( 8950, 2, &Data[20] );          	// CardBalance
		WriteRegister_16bit( 8952, 2, &Data[24] );						// Time
		WriteRegister_16bit( 8954, 1, &Data[28] );						// Time

		/*clear charging Time register*/
		/*clear Finish full time register*/
		/*clear Finish Abnmal time register*/
		WriteRegister_16bit( 8940, 2, &Data[0] );        	// ID
		WriteRegister_16bit( 8942, 2, &Data[4] );        	// ID
		WriteRegister_16bit( 8944, 2, &Data[8] );				// ChargedTime
		WriteRegister_16bit( 8946, 1, &Data[12] );				// ChargedTime
		WriteRegister_16bit( 8947, 2, &Data[14] );				// ChargingTime
		WriteRegister_16bit( 8949, 1, &Data[18] );				// ChargingTime
		WriteRegister_16bit( 8930, 2, &Data[20] );     	 	// ChargedMoney
		WriteRegister_16bit( 8932, 2, &Data[24] );       	// CardBalance

		/*clear charging Autofull register*/
		/*clear Finish full Auto register*/
		/*clear Finish Abnmal Auto register*/
		/*clear Finish Nomoney Auto register*/
		WriteRegister_16bit( 8920, 2, &Data[0] );           		// ID
		WriteRegister_16bit( 8922, 2, &Data[4] );           		// ID
		WriteRegister_16bit( 8924, 2, &Data[8] );           	// ChargedMoney
		WriteRegister_16bit( 8926, 2, &Data[12] );          	// ChargedEle
		WriteRegister_16bit( 8928, 2, &Data[16] );          	// CardBalance
		WriteRegister_16bit( 8937, 2, &Data[20] );						// Time
		WriteRegister_16bit( 8939, 1, &Data[24] );						// Time

		/*clear Finish full ele register*/
		/*clear Finish Abnmal Ele register*/
		/*clear Finish Nomoney Ele register*/
		WriteRegister_16bit( 8970, 2, &Data[0] );       	// ID	0
		WriteRegister_16bit( 8972, 2, &Data[4] );       	// ID	1
		WriteRegister_16bit( 8974, 2, &Data[8] );       	// ChargedEle
		WriteRegister_16bit( 8996, 2, &Data[26] );    // ChargingEle added !!!
		WriteRegister_16bit( 8976, 2, &Data[12] );       	// ChargedMoney
		WriteRegister_16bit( 8978, 2, &Data[16] );       	// CardBalance
		WriteRegister_16bit( 8987, 2, &Data[20] );       	// Time
		WriteRegister_16bit( 8989, 1, &Data[24] );       	// Time

		/*clear Finish full money register*/
		// up
		/*clear Finish full time register*/
		// up
		/*clear Finish full Auto register*/
		// up
		/*clear Finish Abnmal Ele register*/
		// up
		/*clear Finish Abnmal Money register*/
		// up
		/*clear Finish Abnmal time register*/
		// up
		/*clear Finish Abnmal Auto register*/
		// up
		/*clear Finish Nomoney Ele register*/
		// up
		/*clear Finish Nomoney Money register*/
		// up
		/*clear Finish Nomoney Time register*/
		// up
		/*clear Finish Nomoney Auto register*/
		// up
		/*clear pay money register*/
		WriteRegister_16bit( 110, 2, &Data[0] );				// Show number
		WriteRegister_16bit( 112, 2, &Data[4] );     // Show number
	}
}

/*********************************************************************************************************
 ** Function name:      	SysInit_Clr
 ** Descriptions:						clear flag and register
 ** ** input parameters:
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Input_Preprocess( void )
{
	if( EventFlag.QiangConGood == 1 )	// Plug is connect
	{
		if( DisplayInfo.CardBalance < DEPOSIT)	// balance is not enough,quit
		{
			Window_ID = W_Balance_little;
		}
		else if( ( DisplayInfo.CardBalance - DEPOSIT ) < SetChargePara.SetChargeMoney
			|| DisplayInfo.MaxChargeEle < SetChargePara.SetChargeEle )// input is bigger than balance
		{
			err_num++;
			Window_ID = W_Input_Again;
			if( err_num >= Max_InputTimes)				// 3 times input error
			{
				err_num = 0;
				Window_ID = W_InputErr_Quit;			// open door to end
			}
		}
		else// input is small than balance , user select time or autofull charge mode
		{
			Window_ID = W_PatCard_Button;
		}
	}
	else
	{
		Window_ID = W_OpenDoor;								// Open door
	}
}

/*********************************************************************************************************
 ** Function name:      	Read_SingleButton
 ** Descriptions:						Read button(address: Regaddr) status, if pushed, jump to window(Win)
 ** input parameters:			Regaddr	-- button address
 ** 														Win			-- Jump to which window
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Read_SingleButton( ST_INT16 Regaddr, ST_UCHAR Win, ST_CHAR LEFT_RIGHT )
{
	ReadRegisterLB( Regaddr, 1 ); 		// Read button status

	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) || ( Sidekey != 0 ) ) // Analyse received data
	{
		if( Sidekey == LEFT_RIGHT )
		{
			Uart[SCREENCOM].revbuf[3] = 0x01;
			Sidekey = 0;
		}

		switch( Uart[SCREENCOM].revbuf[3] )
		{
			case 0x01:
				Window_ID = Win;
				break;
			default:
				break;
		}
	}
}

/*********************************************************************************************************
 ** Function name:      	Down_Time
 ** Descriptions:						Depend system time and precharge time, calculate the rest time
 ** input parameters:			Systime[2]	-- system time hour
 ** 														Systime[1]	-- system time minute
 ** 														Systime[0]	-- system time second
 ** 														Pretime[2]	-- precharge time hour
 ** 														Pretime[1]	-- precharge time minute
 ** 														Pretime[0]	-- precharge time second
 ** output parameters:   Dwntime[2] -- rest time hour
 ** 														Dwntime[1] -- rest time minute
 ** 														Dwntime[0] -- rest time second
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/ST_CHAR Down_Time(
	ST_CHAR Systime[], ST_CHAR Pretime[], ST_CHAR Dwntime[] )
{
	int PreHour = Pretime[2];
	int PreMinute = Pretime[1];
	int PreSecond = Pretime[0];
	if( Systime[0] > 60 || Systime[1] > 60 || Systime[2] > 24 || Pretime[0] > 60
		|| Pretime[1] > 60 || Pretime[2] > 24 )
	{
		return 0;	// input is  unavailable
	}
	// process Pretime

	if( PreSecond < Systime[0] )	// Second
	{
		PreSecond = PreSecond + 60;
		PreMinute--;

		if( PreMinute < Systime[1] )
		{
			PreMinute = PreMinute + 60;
			PreHour--;
			if( PreHour < Systime[2] )
			{
				PreHour = PreHour + 24;
			}
		}
	}

	if( PreMinute < Systime[1] )	// Minute
	{
		PreMinute = PreMinute + 60;
		PreHour--;
		if( PreHour < Systime[2] )
		{
			PreHour = PreHour + 24;
		}
	}
	if( PreHour < Systime[2] )	// Hour
	{
		PreHour = PreHour + 24;
	}

	// calculate rest time
	if( PreHour >= Systime[2] )
	{
		Dwntime[2] = PreHour - Systime[2];
	}
	if( PreMinute >= Systime[1] )
	{
		Dwntime[1] = PreMinute - Systime[1];
	}
	if( PreSecond >= Systime[0] )
	{
		Dwntime[0] = PreSecond - Systime[0];
	}
	// adjust
	if( Dwntime[0] >= 60 )
	{
		Dwntime[0] -= 60;
		Dwntime[1]++;
	}
	if( Dwntime[1] >= 60 )
	{
		Dwntime[1] -= 60;
		Dwntime[2]++;
	}
	if( Dwntime[2] >= 24 )
	{
		Dwntime[2] -= 24;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:      	Clear_KeyboardBuff
 ** Descriptions:						Clear Keyboard Buffer
 ** input parameters:			None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Clear_KeyboardBuff( void )
{
	inter_number = 0;
	num = 0;
	g_Key = 0;	// add by YL 2011.0922 a number buffer
	Time_Flag = true;
}

/*********************************************************************************************************
 ** Function name:      	Audio_Time_Alarm
 ** Descriptions:						play sound(Soundtype) every (Secnd)second
 ** input parameters:			Secnd	-- time
 ** 															Soundtype	-- play which sound
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Audio_Time_Alarm( ST_CHAR Secnd, ST_CHAR Soundtype )
{
	ST_UCHAR Sys_Second = 0;
	static int Dis_Second = 0;
	static ST_UCHAR Last_Second = 0;

	ReadRegister_16bit( 9017, 1 );			// Read second register, get second
	Sys_Second = Uart[SCREENCOM].revbuf[4]; 	// save second
	Dis_Second = Sys_Second - Last_Second;
	if( Dis_Second < 0 )
	{
		Dis_Second += 60;
	}

	if( Dis_Second >= Secnd )								// Time on
	{
		PlaySound( Soundtype, NULL );		// Play sound
		Last_Second = Sys_Second;
		Dis_Second = 0;
	}
}

/*********************************************************************************************************
 ** Function name:      	Audio_Time_Alarm
 ** Descriptions:						play sound(Soundtype) every (Secnd)second
 ** input parameters:			Secnd	-- time
 ** 															Soundtype	-- play which sound
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void Audio_Time_Alarm_BadSCREEN( ST_CHAR Secnd, ST_CHAR Soundtype )
{
	ST_UCHAR Sys_Second = 0;
	static int Dis_Second = 0;
	static ST_UCHAR Last_Second = 0;

	//ReadRegister_16bit( 9017, 1 );			// Read second register, get second
	//Sys_Second = Uart[SCREENCOM].revbuf[4]; 	// save second
	Sys_Second = DisplayInfo.SysTime[0]; 	// save second
	Dis_Second = Sys_Second - Last_Second;
	if( Dis_Second < 0 )
	{
		Dis_Second += 60;
	}

	if( Dis_Second >= Secnd )								// Time on
	{
		PlaySound( Soundtype, NULL );		// Play sound
		Last_Second = Sys_Second;
		Dis_Second = 0;
	}
}
/*********************************************************************************************************
 ** Function name:      	W_CardAbnormal_Quit_Fction
 ** Descriptions:						check card machine is abnormal, quit
 ** input parameters:			None
 ** output parameters:   	None
 ** Returned value:      	None
 ** Created by:          	Brian.Yang
 ** Created Date:        	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_CardAbnormal_Quit_Fction( void )
{
#if Debug
	printf( "EventFlag.CardExcepFlag == 0, Y/N?\n" );
	ST_CHAR flag = 0;
	flag = getchar();
	if( flag == 'Y' || flag == 'y' )
	{
		EventFlag.CardExcepFlag = 0;
	}
	#endif
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_CARD_EXCEP_BRASHCARD, NULL );
	}
	if( EventFlag.CardExcepFlag == 0 )		// cardmachine normal
	{
		Window_ID = W_PatCard_Button;
	}
	TimeOut( Operation_Delay, 330, W_OpenDoor_End );// 3 minute, not pat card, finish!
	Read_SingleButton( 331, W_OpenDoor_End, RIGHT );
}
/*********************************************************************************************************
 ** Function name:      	W_Plug_Falloff_Fction
 ** Descriptions:						check plug is fall off, quit
 ** input parameters:			None
 ** output parameters:   	None
 ** Returned value:      	None
 ** Created by:          	Brian.Yang
 ** Created Date:        	2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Plug_Falloff_Fction( void )
{
#if Debug
	printf( "EventFlag.QiangConGood == 1, Y/N?\n" );
	ST_CHAR flag = 0;
	flag = getchar();
	if( flag == 'Y' || flag == 'y' )
	{
		EventFlag.QiangConGood = 1;
	}
	#endif
	if( CLR_Sound == 1 )	// play sound
	{
		CLR_Sound = 0;
		PlaySound( AUDIO_PLUG_OUT_BRASHCARD, NULL );
	}

	if( EventFlag.QiangConGood == 1 )		// Plug connected OK
	{
		Window_ID = W_PatCard_Button;
	}
	TimeOut( Operation_Delay, 340, W_OpenDoor_End );// 3 minute, not pat card, finish!
	Read_SingleButton( 341, W_OpenDoor_End, RIGHT );
}

/*********************************************************************************************************
 ** Function name:      	W_Balance_Abnormal_Fction
 ** Descriptions:						Balance, card machine abnormal
 ** input parameters:			None
 ** output parameters:   	None
 ** Returned value:      	None
 ** Created by:          	Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/
void W_Balance_Abnormal_Fction( void )
{
#if Debug
	printf( "EventFlag.CardExcepFlag == 0, Y/N?\n" );
	ST_CHAR flag = 0;
	flag = getchar();
	if( flag == 'Y' || flag == 'y' )
	{
		EventFlag.CardExcepFlag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 0 )		// card machine normal
	{
		Window_ID = W_ImmediatelyCharge;
	}
	TimeOut( Operation_Delay, 240, W_Welcom );// 3 minute, not pat card, finish!
	Read_SingleButton( 241, W_Welcom, RIGHT );
}

/*********************************************************************************************************
 ** Function name:      	Read_Window_ID
 ** Descriptions:						read LCD window ID
 ** input parameters:			None
 ** output parameters:   	None
 ** Returned value:      	None
 ** Created by:          	Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_UCHAR Read_Window_ID( void )
{
	ST_UCHAR Win_ID = 0;
	ReadRegister_16bit( 0, 1 );        						// Read Window_ID
	if( ModbusAnalysis( Uart[SCREENCOM].revbuf ) )  	// Analyze received data
	{
#if Debug
		printf( "Win_ID = %d\n", Uart[SCREENCOM].revbuf[4] );
		printf( "Get_InputEle*******WIND_ID*******************************************************:\n" );
		Show_Buffer( Uart[SCREENCOM].revbuf, 8 );
#endif
		Win_ID = Uart[SCREENCOM].revbuf[4];
	}
	else
	{
#if Debug
		printf( "Get_InputEle*******WIND_ID******Time OUT***************************************:\n" );
#endif
	}
	return Win_ID;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_NoPlug_Ele_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_NoPlug_Ele_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );				// clear buffer
	TimeOut( Waite_PatCard_Time, 960, W_Welcom );		// Time out process
	if( ( EventFlag.QiangConGood == 1 ) && ( EventFlag.StartChargeFlag == 1 ) )	// Plug is connected again
	{
		Window_ID = W_ChargingEle;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.QiangConGood = 1;
		EventFlag.StartChargeFlag = 1;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_NoPlug_Ele_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_NoPlug_Ele;
		if( CLR_Sound == 1 )
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_PLUG_OUT_CHARGING, NULL );
		}
	}

#if Debug																									// for Debug
	DisplayInfo.CardNo[0] = 0x31;                 	// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargingEle = 29.88;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 5;                 	// hour
	DisplayInfo.ChargedTime[1] = 0;                 	// minute
	DisplayInfo.ChargedTime[0] = 0;                 	// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[8] );   	// ChargedEle
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[12] );  	// ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[16] );  	// CardBalance
	Charg_Data[21] = DisplayInfo.ChargedTime[2];         		// hour
	Charg_Data[23] = DisplayInfo.ChargedTime[1];          	// minute
	Charg_Data[25] = DisplayInfo.ChargedTime[0];					// second

	Float2Arry( DisplayInfo.ChargingEle, &Charg_Data[26] );   	// ChargingEle

	//WriteRegister_16bit( 8970,4 , &Charg_Data[0]  );       	// ID
	WriteRegister_16bit( 8970, 2, &Charg_Data[0] );       	// ID	0
	WriteRegister_16bit( 8972, 2, &Charg_Data[4] );       	// ID	1
	WriteRegister_16bit( 8974, 2, &Charg_Data[8] );       	// ChargedEle
	WriteRegister_16bit( 8996, 2, &Charg_Data[26] );    // ChargingEle added !!!
	WriteRegister_16bit( 8976, 2, &Charg_Data[12] );       	// ChargedMoney
	WriteRegister_16bit( 8978, 2, &Charg_Data[16] );       	// CardBalance
	//WriteRegister_16bit( 8987,3 , &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8987, 2, &Charg_Data[20] );       	// Time
	WriteRegister_16bit( 8989, 1, &Charg_Data[24] );       	// Time
#if Union_Debug
#if Debug
	printf( "FinishFullEle:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_NoPlug_Money_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_NoPlug_Money_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );		// clear buffer
	TimeOut( Waite_PatCard_Time, 970, W_Welcom );		// Time out process
	if( ( EventFlag.QiangConGood == 1 ) && ( EventFlag.StartChargeFlag == 1 ) )	// Plug is connected again
	{
		Window_ID = W_ChargingMoney;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.QiangConGood = 1;
		EventFlag.StartChargeFlag = 1;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_NoPlug_Money_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_NoPlug_Money;
		if( CLR_Sound == 1 )
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_PLUG_OUT_CHARGING, NULL );
		}
	}
	#if Debug																										// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;
	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.ChargingMoney = 4.88;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second
#endif
	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          		// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[8] );  	// ChargedMoney
	Float2Arry( DisplayInfo.ChargingMoney, &Charg_Data[12] );  	// ChargingMoney
	Float2Arry( DisplayInfo.ChargedEle, &Charg_Data[16] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[20] );		// CardBalance
	Charg_Data[25] = DisplayInfo.ChargedTime[2];
	Charg_Data[27] = DisplayInfo.ChargedTime[1];
	Charg_Data[29] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8960,4 , &Charg_Data[0]  );           	// ID
	WriteRegister_16bit( 8960, 2, &Charg_Data[0] );           	// ID
	WriteRegister_16bit( 8962, 2, &Charg_Data[4] );           	// ID
	WriteRegister_16bit( 8964, 2, &Charg_Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8966, 2, &Charg_Data[12] );          	// ChargingMoney
	WriteRegister_16bit( 8968, 2, &Charg_Data[16] );          	// ChargedEle
	WriteRegister_16bit( 8950, 2, &Charg_Data[20] );          	// CardBalance
	//WriteRegister_16bit( 8952,3 , &Charg_Data[24] );								// Time
	WriteRegister_16bit( 8952, 2, &Charg_Data[24] );					// Time
	WriteRegister_16bit( 8954, 1, &Charg_Data[28] );					// Time
#if Union_Debug
#if Debug
	printf( "FinishFullMoney:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )				// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Wind	ow_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_NoPlug_Time_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 *********************************************************************************************************/

ST_CHAR W_Finish_NoPlug_Time_Fction( void )
{
	ST_UCHAR Charg_Data[50];
	memset( Charg_Data, 0, sizeof( Charg_Data ) );	// clear buffer
	TimeOut( Waite_PatCard_Time, 980, W_Welcom );		// Time out process
	if( ( EventFlag.QiangConGood == 1 ) && ( EventFlag.StartChargeFlag == 1 ) )	// Plug is connected again
	{
		Window_ID = W_ChargingTime;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.QiangConGood = 1;
		EventFlag.StartChargeFlag = 1;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_NoPlug_Time_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_NoPlug_Time;
		if( CLR_Sound == 1 )
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_PLUG_OUT_CHARGING, NULL );
		}
	}
	#if Debug																							// For Debug
	DisplayInfo.CardNo[0] = 0x31;										// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedTime[2] = 16;									// hour
	DisplayInfo.ChargedTime[1] = 55;								// minute
	DisplayInfo.ChargedTime[0] = 2;									// second

	DisplayInfo.ChargingTime[2] = 4;
	DisplayInfo.ChargingTime[1] = 2;
	DisplayInfo.ChargingTime[0] = 39;

	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
#endif

	memcpy( &Charg_Data[0], DisplayInfo.CardNo, 8 );          // ID
	Charg_Data[9] = DisplayInfo.ChargedTime[2];
	Charg_Data[11] = DisplayInfo.ChargedTime[1];
	Charg_Data[13] = DisplayInfo.ChargedTime[0];
	Charg_Data[15] = DisplayInfo.ChargingTime[2];
	Charg_Data[17] = DisplayInfo.ChargingTime[1];
	Charg_Data[19] = DisplayInfo.ChargingTime[0];

	Float2Arry( DisplayInfo.ChargedMoney, &Charg_Data[20] );  // ChargedMoney
	Float2Arry( DisplayInfo.CardBalance, &Charg_Data[24] );  // CardBalance

	Charg_Data[29] = DisplayInfo.ChargedTime[2];
	Charg_Data[31] = DisplayInfo.ChargedTime[1];
	Charg_Data[33] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit( 8940,4 , &Charg_Data[0]  );          	// ID
	WriteRegister_16bit( 8940, 2, &Charg_Data[0] );          	// ID
	WriteRegister_16bit( 8942, 2, &Charg_Data[4] );          	// ID
	//WriteRegister_16bit( 8944,3 , &Charg_Data[8]  );							// ChargedTime
	WriteRegister_16bit( 8944, 2, &Charg_Data[8] );				// ChargedTime
	WriteRegister_16bit( 8946, 1, &Charg_Data[12] );			// ChargedTime
	//WriteRegister_16bit( 8947,3 , &Charg_Data[14] );							// ChargingTime
	WriteRegister_16bit( 8947, 2, &Charg_Data[14] );			// ChargingTime
	WriteRegister_16bit( 8949, 1, &Charg_Data[18] );			// ChargingTime
	WriteRegister_16bit( 8930, 2, &Charg_Data[20] );          	// ChargedMoney
	WriteRegister_16bit( 8932, 2, &Charg_Data[24] );          	// CardBalance
#if Union_Debug
#if Debug
	printf( "FinishFullTime:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       W_Finish_NoPlug_Auto_Fction
 ** Descriptions:       	Charging finish with Battery full
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 **********************************************************************************************************/

ST_CHAR W_Finish_NoPlug_Auto_Fction( void )
{
	ST_UCHAR Data[50];
	memset( Data, 0, sizeof( Data ) );			// clear buffer
	TimeOut( Waite_PatCard_Time, 990, W_Welcom );		// Time out process

	if( ( EventFlag.QiangConGood == 1 ) && ( EventFlag.StartChargeFlag == 1 ) )	// Plug is connected again
	{
		Window_ID = W_ChargingAutoFull;
		return 0;
	}
#if Debug
	ST_CHAR flag = 0;
	printf( "Card abnormal:" );
	flag = getchar();
	if( flag == '1' )
	{
		EventFlag.CardExcepFlag = 1;
		flag = 0;
	}
	if( flag == '2' )
	{
		EventFlag.CardExcepFlag = 0;
		flag = 0;
	}
	if( flag == '3' )
	{
		EventFlag.QiangConGood = 1;
		EventFlag.StartChargeFlag = 1;
		flag = 0;
	}
	#endif
	if( EventFlag.CardExcepFlag == 1 )		// card abnormal
	{
		Window_ID = W_Finish_NoPlug_Auto_Badcard;
		if( CLR_Sound == 1 )	// play sound
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_CARDCOM_EXCEP_CHARGING, NULL );
		}
	}
	else
	{
		Window_ID = W_Finish_NoPlug_Auto;
		if( CLR_Sound == 1 )
		{
			CLR_Sound = 0;
			PlaySound( AUDIO_PLUG_OUT_CHARGING, NULL );
		}
	}

#if Debug																	// For Debug
	DisplayInfo.CardNo[0] = 0x31;				// ID
	DisplayInfo.CardNo[1] = 0x32;
	DisplayInfo.CardNo[2] = 0x33;
	DisplayInfo.CardNo[3] = 0x34;
	DisplayInfo.CardNo[4] = 0x35;
	DisplayInfo.CardNo[5] = 0x36;
	DisplayInfo.CardNo[6] = 0x37;
	DisplayInfo.CardNo[7] = 0x39;

	DisplayInfo.ChargedEle = 10.12;
	DisplayInfo.ChargedMoney = 5.06;
	DisplayInfo.CardBalance = 94.94;
	DisplayInfo.ChargedTime[2] = 16;				// hour
	DisplayInfo.ChargedTime[1] = 55;				// minute
	DisplayInfo.ChargedTime[0] = 2;				// second
#endif
	memcpy( &Data[0], DisplayInfo.CardNo, 8 );          	// ID
	Float2Arry( DisplayInfo.ChargedMoney, &Data[8] );  		// ChargedMoney
	Float2Arry( DisplayInfo.ChargedEle, &Data[12] );		// ChargedEle
	Float2Arry( DisplayInfo.CardBalance, &Data[16] );  	// CardBalance
	Data[21] = DisplayInfo.ChargedTime[2];
	Data[23] = DisplayInfo.ChargedTime[1];
	Data[25] = DisplayInfo.ChargedTime[0];

	//WriteRegister_16bit(8920,4 , &Data[0]);           	// ID
	WriteRegister_16bit( 8920, 2, &Data[0] );           	// ID
	WriteRegister_16bit( 8922, 2, &Data[4] );           	// ID
	WriteRegister_16bit( 8924, 2, &Data[8] );           	// ChargedMoney
	WriteRegister_16bit( 8926, 2, &Data[12] );          	// ChargedEle
	WriteRegister_16bit( 8928, 2, &Data[16] );          	// CardBalance
	//WriteRegister_16bit(8937,3 , &Data[20]);								// Time
	WriteRegister_16bit( 8937, 2, &Data[20] );							// Time
	WriteRegister_16bit( 8939, 1, &Data[24] );							// Time
#if Union_Debug
#if Debug
	printf( "FinishFullAuto:" );
	ST_UCHAR flag2 = 0;
	flag2 = getchar();
	if( flag2 == '1' )
	{
		EventFlag.IsPatCardFlag = 1;
		flag2 = 0;
	}
	if( flag2 == '2' )
	{
		EventFlag.NoSameCardFlag = 1;
		flag2 = 0;
	}
	#endif
#endif
	if( EventFlag.NoSameCardFlag == 1 )				// No same card
	{
		Window_ID = W_NoSameCard;
		EventFlag.NoSameCardFlag = 0;
	}
	else if( EventFlag.IsPatCardFlag == 1 )	// finish, pat card
	{
		if( PriAlarm.PrintAlarm & 0x01 )		// No paper
		{
			Window_ID = W_Print_Nopaper;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//else if( (PriAlarm.PrintAlarm & 0x1E) )	// Printer abnormal
		else if( ( PriAlarm.PrintAlarm & 0x1E ) || ( EventFlag.ExBoardCommFlag == 1 ) )	// Printer abnormal or External board abnormal
		{
			Window_ID = W_Print_Abnormal;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		else //if( (PriAlarm.PrintAlarm & 0x1F == 0) ) // printer normal
		{
			Window_ID = W_PayMoney;		// show pay money
			EventFlag.IsPatCardFlag = 0;				// clear pat card flag
		}
		//Window_ID = W_PayMoney;
		//Window_ID = W_OpenDoor_End;						// finish charge, open door
		//EventFlag.IsPatCardFlag = 0;
	}
	return 0;
}

/*********************************************************************************************************
 ** Function name:       Show_TimeonLCD
 ** Descriptions:       	Show system time on LCD
 ** input parameters:    None
 ** output parameters:   None
 ** Returned value:      None
 ** Created by:          Brian.Yang
 ** Created Date:        2011-07-19
 **--------------------------------------------------------------------------------------------------------
 ** Modified by:
 ** Modified date:
 **********************************************************************************************************/

void Show_TimeonLCD( void )
{
	ST_UCHAR Data[15];
	memset( Data, 0, sizeof( Data ) );							// clear buffer
#if Debug
	DisplayInfo.SysTime[0] = 34;							// Second
	DisplayInfo.SysTime[1] = 58;							// Minute
	DisplayInfo.SysTime[2] = 15;							// Hour
	DisplayInfo.SysTime[3] = 8;							// Date
	DisplayInfo.SysTime[4] = 9;							// Month
	DisplayInfo.SysTime[5] = 2011;			// year High

#endif
	Data[1] = DisplayInfo.SysTime[0];							// Second
	Data[3] = DisplayInfo.SysTime[1];							// Minute
	Data[5] = DisplayInfo.SysTime[2];							// Hour
	Data[7] = DisplayInfo.SysTime[3];							// Date
	Data[9] = DisplayInfo.SysTime[4];							// Month
	Data[10] = DisplayInfo.SysTime[5] >> 8;			// year High
	Data[11] = DisplayInfo.SysTime[5] & 0x00FF;	// year Low

	WriteRegister_16bit( 3, 2, &Data[0] );  	// Last charge time
	WriteRegister_16bit( 5, 2, &Data[4] );  	// Last charge time
	WriteRegister_16bit( 7, 2, &Data[8] );  	// Last charge time
}
