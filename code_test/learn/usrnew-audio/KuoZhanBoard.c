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
#include "KuoZhanBoard.h"
#include "display.h"


ExBoardMsgTP ExBoardMsg;

union  PriAlarmTP PriAlarm;

void ExpandBoardComRcv(void);
void ExpandBoardComSend(void);
void PackSendData(void);
void PressKeySend(void);
ST_UCHAR AnasysRcvExBoardData(void);
/********************************************************************************* 
 ������ƣ� InitKuoZhanBoard
 ����������	��ʼ����?�������պͷ����߳�
 ��    �룺	��
 ��	   ���� ��
 �� �� ֵ�� ��
 ��    �ߣ�	yansudan
 ��    �ڣ�	2011.8.2
 �޸ļ�¼��
*********************************************************************************/
void InitExpandBoard(void)
{
	ST_INT32 result;
	pthread_t EBSendthread, EBRcvthread;
	#if 1
	GetProtocolType();
	result = pthread_create(&EBSendthread, PTHREAD_CREATE_JOINABLE, (void *)ExpandBoardComSend ,NULL);
	if(result)
	{
		perror("pthread_create: MeterComSend.\n");
		exit(EXIT_FAILURE);
	}
	result = pthread_create(&EBRcvthread, PTHREAD_CREATE_JOINABLE, (void *)ExpandBoardComRcv ,NULL);
	if(result)
	{
		perror("pthread_create: MeterComRcv.\n");
		exit(EXIT_FAILURE);
	}
	#if 0
	result = pthread_create(&PressKeythread, PTHREAD_CREATE_JOINABLE, (void *)PressKeySend ,NULL);
	if(result)
	{
		perror("pthread_create: MeterComRcv.\n");
		exit(EXIT_FAILURE);
	}	
	#endif
	#endif
}

/********************************************************************************* 
 ������ƣ� ExpandBoardComSend
 ����������	��չ�弶�����ڵķ����̻߳ص�����
 ��    �룺	��
 ��	   ���� ��
 �� �� ֵ�� ��
 ��    �ߣ�	yansudan
 ��    �ڣ�	2011.8.2
 �޸ļ�¼��
*********************************************************************************/

void ExpandBoardComSend(void)
{
	ExBoardMsg.SendCmd = KEY_CMD;
	while(1)
	{
		if(0 == ExBoardMsg.SendFinishFlag)
		{
			switch(ExBoardMsg.SendCmd)
			{
				#if 1
				case KEY_CMD:
				{
					PackSendData();
			//		ExBoardMsg.SendCount++;
					ExBoardMsg.SendFinishFlag = 1;
				//	printf("send keycmd\n");
					usleep(300*1000);
					
				}
				break;
				#endif
				case METER_CMD:
				{				
					MeterComSend();
					ExBoardMsg.SendFinishFlag = 1;	
				//	usleep(300*1000);
			//		printf("send metercmd\n");		
					
				}
				break;
				case PRINT_CMD:
				{
					PackSendData();
					ExBoardMsg.SendCount++;
					ExBoardMsg.SendFinishFlag = 1;
			//		printf("send printcmd\n");
					usleep(300*1000);
					
				}
				break;
				default:break;				
				
			}
			if(ExBoardMsg.SendCount>30)
			{					
				EventFlag.ExBoardCommFlag = 1;
				printf("EventFlag.ExBoardCommFlag is %d \n",EventFlag.ExBoardCommFlag);
				ExBoardMsg.SendCount = 0;
				ExBoardMsg.SendCmd = METER_CMD;
			}
		}
		usleep(10*1000);

	}
	
}



void PressKeySend(void)
{

	while(1)
	{
		if(0 == ExBoardMsg.SendFinishFlag)
		{
			if(KEY_CMD == ExBoardMsg.SendCmd)
			{
				PackSendData();	
				ExBoardMsg.SendFinishFlag = 1;
			}
		}
		usleep(300*1000);
	}
}

/********************************************************************************* 
 ������ƣ� ExpandBoardComRcv
 ����������	��չ�弶�����ڵĽ����̻߳ص�����
 ��    �룺	��
 ��	   ���� ��
 �� �� ֵ�� ��
 ��    �ߣ�	yansudan
 ��    �ڣ�	2011.8.2
 �޸ļ�¼��
*********************************************************************************/

void ExpandBoardComRcv(void)
{
	ST_UCHAR RcvResult = 0;
	
	while(1)
	{		
		Uart[EXBOARDCOM].rxnum = (ST_INT32)ReadCom(ExBoardfd, Uart[EXBOARDCOM].revbuf, sizeof(Uart[EXBOARDCOM].revbuf),100*1000);
		if(Uart[EXBOARDCOM].rxnum>0)
		{
			RcvResult = AnasysRcvExBoardData();		
			
			switch(ExBoardMsg.SendCmd)
			{
				case KEY_CMD:
				{
					#if 1
					if(1 == RcvResult)
					{
						//memcpy(ExBoardMsg.RcvKeyData,&Uart[EXBOARDCOM].revbuf[3],Uart[EXBOARDCOM].revbuf[2]);	
						printf("rcv key data ok!!!!!!!!!!!!!!!!!\n");
						if(EventFlag.ScreenExcepFlag!=1)
						ExternBoard();
					}
					#if 1
					if(DISP_IDLE==DisplayInfo.CurrentPage||DISP_EXCEP_IDLE==DisplayInfo.CurrentPage||1==EventFlag.StartChargeFlag||EventFlag.ChargeExcepEndFlag!=0||DISP_NOPAPLE_IDLE==DisplayInfo.CurrentPage)
					{
						ExBoardMsg.SendCmd = METER_CMD;		
					}
					else
					{
						ExBoardMsg.SendCmd = PRINT_CMD;
					}
					#endif	
					#endif
					ExBoardMsg.SendFinishFlag = 0;
					
					printf("DisplayInfo.CurrentPage is %d EventFlag.StartChargeFlag is %d EventFlag.ChargeExcepEndFlag is %d",DisplayInfo.CurrentPage,EventFlag.StartChargeFlag,EventFlag.ChargeExcepEndFlag );
					ExBoardMsg.SendCmd = METER_CMD;		
			//		printf("result is %d.rcvok,keytometer\n",RcvResult);	
					RcvResult = 0;	
										
				}
				break;
				case METER_CMD:
				{
					MeterComRcv();
					ExBoardMsg.SendFinishFlag = 0;
					memset(Uart[EXBOARDCOM].revbuf,0,sizeof(Uart[EXBOARDCOM].revbuf));
					Uart[EXBOARDCOM].rxnum = 0;
					ExBoardMsg.SendCmd = PRINT_CMD;
			//		printf("rcvok,metertoprint\n");
				}
				
				break;
				case PRINT_CMD:
				{
					if(1 == RcvResult)
					{
						PriAlarm.PrintAlarm = Uart[EXBOARDCOM].revbuf[3];	
						printf("PriAlarm.PrintException.PrintNoPaper is %d PriAlarm.PrintAlarm is %x \n",PriAlarm.PrintException.PrintNoPaper,PriAlarm.PrintAlarm);					
						
					}
					ExBoardMsg.SendFinishFlag = 0;
					
					ExBoardMsg.SendCmd = KEY_CMD;
					
				//	printf("result is %d.rcvok,printtokey\n",RcvResult);	
					RcvResult = 0;									
				}
				break;
				default:break;				
				
			}			
		}
#if 1
		else
		{
			switch(ExBoardMsg.SendCmd)
			{
				case KEY_CMD:
				{
					
					if(ExBoardMsg.SendFinishFlag==1)
					{
						printf("key to meter\n");
						ExBoardMsg.SendFinishFlag = 0;
						if(DISP_IDLE==DisplayInfo.CurrentPage||DISP_EXCEP_IDLE == DisplayInfo.CurrentPage||
							1==EventFlag.StartChargeFlag||EventFlag.ChargeExcepEndFlag!=0||DISP_NOPAPLE_IDLE==DisplayInfo.CurrentPage
							||DISP_EXCEP_IDLE_A==DisplayInfo.CurrentPage||DISP_EXCEP_IDLE_B==DisplayInfo.CurrentPage)
						{
							ExBoardMsg.SendCmd = METER_CMD;	
						}
						else 
						{
							ExBoardMsg.SendCmd = PRINT_CMD;
						}
					}			
										
				}
				break;
				case METER_CMD:
				{
					
				//	MeterComRcv();
					if(ExBoardMsg.SendFinishFlag==1)
					{
				//		printf("meter to print\n");
						ExBoardMsg.SendFinishFlag = 0;
						ExBoardMsg.SendCmd = PRINT_CMD;
					}
				}
				
				break;
				case PRINT_CMD:
				{
					
					if(ExBoardMsg.SendFinishFlag==1)
					{
						printf("print to key\n");
						ExBoardMsg.SendFinishFlag = 0;
						ExBoardMsg.SendCmd = KEY_CMD;	
					}									
				}
				break;
				default:break;					
			}
		}
#endif
		usleep(10*1000);

	}
	
}



/********************************************************************************* 
 ������ƣ� ExpandBoardComRcv
 ����������	��չ�弶�����ڵĽ����̻߳ص�����
 ��    �룺	��
 ��	   ���� ��
 �� �� ֵ�� ��
 ��    �ߣ�	yansudan
 ��    �ڣ�	2011.8.2
 �޸ļ�¼��
*********************************************************************************/


void PackSendData(void)
{
	ST_INT16 crc;
	Uart[EXBOARDCOM].sendbuf[0] = STATION_ID;
	ExBoardMsg.SenDataLen = 1;
	int i;
	static ST_INT16 count = 0,printfg = 0;
	switch(ExBoardMsg.SendCmd)
	{
		case KEY_CMD:
		{
			ExBoardMsg.SenDataLen = 1;
			Uart[EXBOARDCOM].sendbuf[1] = KEY_ID;
			Uart[EXBOARDCOM].sendbuf[2] = ExBoardMsg.SenDataLen;
			Uart[EXBOARDCOM].sendbuf[3] = 0;
			crc = CRC16(Uart[EXBOARDCOM].sendbuf,ExBoardMsg.SenDataLen+3);
			Uart[EXBOARDCOM].sendbuf[ExBoardMsg.SenDataLen+3] = (crc &0xFF00) >> 8;
			Uart[EXBOARDCOM].sendbuf[ExBoardMsg.SenDataLen+4] = (crc &0x00FF);
			printf("send key cmd:\n");
			for(i = 0;i<6;i++)
			printf("%x ",Uart[EXBOARDCOM].sendbuf[i]);
			printf("\n");
			write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, ExBoardMsg.SenDataLen+5);	
				
		}
		break;
		case METER_CMD:
		{
			
		}
		break;
		case PRINT_CMD:
		{
			if(count == 5&&printfg==0)
			{
				count = 0;
				PrintDataLen = 21;
				printfg = 1;
				printf("It is time to print!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			}
			ExBoardMsg.SenDataLen = PrintDataLen;
			Uart[EXBOARDCOM].sendbuf[1] = PRINT_ID;
			Uart[EXBOARDCOM].sendbuf[2] = ExBoardMsg.SenDataLen;
			if(ExBoardMsg.SenDataLen == 1)
			{
				Uart[EXBOARDCOM].sendbuf[3] = 0;
				//count++;
				printfg = 0;
				
			}
			else
			{
				
				memcpy(&Uart[EXBOARDCOM].sendbuf[3],PackPrintData(PrintData),ExBoardMsg.SenDataLen);

			//	PrintDataLen = 1;
				
			}
			
			crc = CRC16(Uart[EXBOARDCOM].sendbuf,ExBoardMsg.SenDataLen+3);
			Uart[EXBOARDCOM].sendbuf[ExBoardMsg.SenDataLen+3] = (crc &0xFF00) >> 8;
			Uart[EXBOARDCOM].sendbuf[ExBoardMsg.SenDataLen+4] = (crc &0x00FF);
			printf("send print cmd:\n");
			for(i = 0;i<ExBoardMsg.SenDataLen+5;i++)
			printf("%x ",Uart[EXBOARDCOM].sendbuf[i]);
			printf("\n");
			write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, ExBoardMsg.SenDataLen+5);
			
			if(PrintDataLen==21)
			{
			//	write(ExBoardfd, Uart[EXBOARDCOM].sendbuf, ExBoardMsg.SenDataLen+5);
				PrintDataLen = 1;
				sleep(1);
			}
			
		}
		break;		
		default:break;
	}
	
}


/********************************************************************************* 
 ������ƣ� AnasysRcvExBoardData
 ����������	��չ�弶�����ڵĽ����̻߳ص�����
 ��    �룺	��
 ��	   ���� ��
 �� �� ֵ�� ��
 ��    �ߣ�	yansudan
 ��    �ڣ�	2011.8.2
 �޸ļ�¼��f
*********************************************************************************/

ST_UCHAR AnasysRcvExBoardData(void)
{
	ST_UINT16 crc = 1,temp,i;
	printf("ExBoardMsg.SendCmd is %d\n",ExBoardMsg.SendCmd);
	printf("Uart[EXBOARDCOM].rxnum is %d \n",Uart[EXBOARDCOM].rxnum);
	for(i=0;i<Uart[EXBOARDCOM].rxnum;i++)
	printf("%x ",Uart[EXBOARDCOM].revbuf[i]);
	
	if((Uart[EXBOARDCOM].rxnum>=6)&&(Uart[EXBOARDCOM].revbuf[0]==0x01))
	{	
		
		temp = (ST_UINT16)(((Uart[EXBOARDCOM].revbuf[Uart[EXBOARDCOM].rxnum-2]&0x00FF)<<8)+Uart[EXBOARDCOM].revbuf[Uart[EXBOARDCOM].rxnum-1]);
		crc = CRC16(Uart[EXBOARDCOM].revbuf,Uart[EXBOARDCOM].rxnum-2);
		
		printf("temp is %x,crc is %x\n",temp,crc);
		
		if((Uart[EXBOARDCOM].revbuf[1]!=KEY_ID||Uart[EXBOARDCOM].revbuf[1]!=PRINT_ID)
			&&(Uart[EXBOARDCOM].revbuf[2]!=Uart[EXBOARDCOM].rxnum-5)&&(crc!=temp))
		{
			memset(Uart[EXBOARDCOM].revbuf,0,sizeof(Uart[EXBOARDCOM].revbuf));
			Uart[EXBOARDCOM].rxnum = 0;
			
			printf("rcv error111\n");
			
			return 0;
		}
		
		if(Uart[EXBOARDCOM].revbuf[1]==KEY_ID)
		{
			ExBoardMsg.SendCmd = KEY_CMD;
		}
		else if(Uart[EXBOARDCOM].revbuf[1]==PRINT_ID)
		{
			ExBoardMsg.SendCmd = PRINT_CMD;
		}
		ExBoardMsg.SendCount = 0;
		EventFlag.ExBoardCommFlag = 0;
		printf("rcv ok 111\n");
		return 1;
		
	}
	if((Uart[EXBOARDCOM].revbuf[0]==0x68||Uart[EXBOARDCOM].revbuf[0]==0xfe)&&Uart[EXBOARDCOM].revbuf[Uart[EXBOARDCOM].rxnum-1]==0x16)
	{
		ExBoardMsg.SendCmd = METER_CMD;
		//ExBoardMsg.SendCount = 0;
		//EventFlag.ExBoardCommFlag = 0;
	}
	printf("rcv error\n");
	return 0;	
}
