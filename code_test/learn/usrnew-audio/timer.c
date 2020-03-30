
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include "public.h"
#include "cdzDataType.h"
#include "timer.h"
extern ST_INT32 CurrentKeepLiveCount;  


/********************************************************************************* 
 函数名称： timeout_info
 功能描述：	定时器回调函数
 输    入：	signo：信号量
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void timeout_info(int signo)
{
	ST_UCHAR CurrentTime[7];
	struct tm *stShowNowTime;
	time_t now;
	now = time(0);
	stShowNowTime = localtime(&now);
	if(EventFlag.StartChargeFlag==1)
	{
		ChargeMsg.ChargeTmLong[0]++;
									                                                              
		if(ChargeMsg.ChargeTmLong[0]==60)
		{
			ChargeMsg.ChargeTmLong[1]++;
			ChargeMsg.ChargeTmLong[0] = 0;	
		}
		if(ChargeMsg.ChargeTmLong[1]==60)
		{
			ChargeMsg.ChargeTmLong[2]++;
			ChargeMsg.ChargeTmLong[1] = 0;	
		}
		
		memcpy(DisplayInfo.ChargedTime,ChargeMsg.ChargeTmLong,3);
		printf("SetChargePara.SetChargeTime is %d:%d:%d\n",SetChargePara.SetChargeTime[2],SetChargePara.SetChargeTime[1],SetChargePara.SetChargeTime[0]);
				
		 if(SetChargePara.SetChargeMode == 3)
		{
			static ST_CHAR temp[3];
			memcpy(temp,SetChargePara.SetChargeTime,3);
			if(temp[0]<DisplayInfo.ChargedTime[0])
			{
				if(temp[1]==0)
				{					
					temp[2]--;				
					temp[1]+=60;
				}
				temp[1]--;
				temp[0]+=60;
			}

			printf("temp[1] is %d DisplayInfo.ChargedTime[1] is %d",temp[1],DisplayInfo.ChargedTime[1]);			
			DisplayInfo.ChargingTime[0] = temp[0]-DisplayInfo.ChargedTime[0];
			DisplayInfo.ChargingTime[1] = temp[1]-DisplayInfo.ChargedTime[1];
			DisplayInfo.ChargingTime[2] = temp[2]-DisplayInfo.ChargedTime[2];
			printf("SetChargePara.SetChargeTime is %d:%d:%d\n",DisplayInfo.ChargingTime[2],DisplayInfo.ChargingTime[1],DisplayInfo.ChargingTime[0]);
		}		

	}			

	if(EventFlag.ChargeFullCountFlag==1)
	{

		ChargeMsg.ChargeFullCount++;

	}

	CurrentKeepLiveCount++;

	

	CurrentTime[0] = (ST_UCHAR)(stShowNowTime->tm_year % 100);
	CurrentTime[1] = (ST_UCHAR)(stShowNowTime->tm_mon+1);
	CurrentTime[2] = (ST_UCHAR)(stShowNowTime->tm_mday);
	CurrentTime[3] = (ST_UCHAR)(stShowNowTime->tm_wday);
	CurrentTime[4] = (ST_UCHAR)(stShowNowTime->tm_hour);
	CurrentTime[5] = (ST_UCHAR)(stShowNowTime->tm_min);
	CurrentTime[6] = (ST_UCHAR)(stShowNowTime->tm_sec);
	
	DisplayInfo.SysTime[0] = stShowNowTime->tm_sec;
	DisplayInfo.SysTime[1] = stShowNowTime->tm_min;
	DisplayInfo.SysTime[2] = stShowNowTime->tm_hour;
	DisplayInfo.SysTime[3] = stShowNowTime->tm_mday;
	DisplayInfo.SysTime[4] = stShowNowTime->tm_mon+1;
	DisplayInfo.SysTime[5] = (stShowNowTime->tm_year%100)+2000;
	//printf("**************************DisplayInfo.SysTime[5] is %d DisplayInfo.SysTime[0] is %d\n",DisplayInfo.SysTime[5],DisplayInfo.SysTime[0]);
	
	cdzWriteDataItemContentNoAuth(CDZ_TERM_MEA_POINT, 0xC010, CurrentTime);
	cdzWriteDataItemContentNoAuth(CDZ_TERM_MEA_POINT, 0xC011, &CurrentTime[4]);
	
	
	//add by 11-03-26
	#ifdef GUANGHUI_TESTSOFTWARE
	
	PackTestSoftwareMsg();
	#endif
	
	if(EventFlag.ChargeNomoneyNoPulgFg==1)
	{
		ChargeMsg.ChargeFullNomoneyNoPulgCount++;
	}
		
}

/********************************************************************************* 
 函数名称： init_sigaction
 功能描述：	初始化定时器参数
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void init_sigaction(void)
{
    struct sigaction act;

    act.sa_handler = timeout_info;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGPROF, &act, NULL);
} 

/********************************************************************************* 
 函数名称： init_time
 功能描述：	启动定时器
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void init_time(void)
{
    struct itimerval val;

    val.it_value.tv_sec = 1;
    val.it_value.tv_usec = 0;
    val.it_interval = val.it_value;
    setitimer(ITIMER_PROF, &val, NULL);
}

/********************************************************************************* 
 函数名称： InitTimer
 功能描述：	初始化定时器
 输    入： 无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void InitTimer(void)
{
    init_sigaction();
    init_time();

}
