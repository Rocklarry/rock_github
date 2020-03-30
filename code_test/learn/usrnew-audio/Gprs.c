/*************************************Copyright@ 2011********************************************
**                       Shenzhen Busbar Technology Development CO.,Ltd.  
**                                  http://www.ebusbar.net/ 
** 文件名   :   gprs.c  
** 作者     :   刘泉承  
** 日期     :   2011-05-30
** 版本     :   V0.01    
*************************************************************************************************/

#include <stdio.h>      /* 标准输入/输出定义 */
#include <termios.h>    /* POSIX终端控制定义 */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "Gprs.h"
#include "DataType.h"
#include "cdzProtocol.h"
#include "public.h"
#include "AlarmRecord.h"
#include "convertion.h"
#include "mycom.h"
#include "cdzDataType.h"
#include "cdzChargeRecord.h"


//ST_INT32 GPRSfd;
ST_INT32 rc;
//ST_UCHAR rbuf[512];                                                       /* 接收数据缓冲区 */

ST_CHAR wbuf[256]="AT+CIPSTART=\"TCP\",\"113.89.207.191\",1115\r\n";

//ST_UCHAR wbuf[256]="AT+CIPSTART=\"TCP\",\"113.87.169.63\",1985\r\n";    /* 发送数据缓冲区 */
//ST_UCHAR fsend[1024];

static CDZ_GPRS_PROTOCOL_PARAM s_cdzProtocolParam_Gprs;

extern ChargeMsgTP ChargeMsg;
  


ST_BOOL ProtolLandFlag_Gprs = false;           /* 规约登陆标志 */  
static ST_UCHAR FSeq_Gprs = 1;
static ST_UCHAR LastFSeq_Gprs = 0;
static ST_BOOL KeeALiveFlag_Gprs;
static ST_BOOL GprsConnectFlag = false; 

static ST_UCHAR s_RtuA_Gprs[4] = {0x82, 0x00, 0x00, 0x32};
ST_CHAR rbuf[512];


static void cdzProtocolSendLoadFrame_Gprs(void);
static void cdzProtocolUpload_Gprs(ST_UCHAR *psFSeq, ST_UCHAR *pLastFeq);

/*************************************************************************************************************
** 函数名称：readport()
** 功能描述：读数据，串口参数、BUF、长度、超时时间 
** 输入：    GPRSfd:文件描述符      buf:        缓冲区                
**           len:   长度            maxwaittime:超时时间
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改记录：
*************************************************************************************************************/

ST_INT32 readport(ST_INT32 GPRSfd,ST_CHAR *buf,ST_INT32 len,ST_INT32 maxwaittime)  
{
	 ST_INT32 selectnum,rcnum;
   
   struct timeval tv;                                                //用来描述一段时间
   fd_set readfd;
   tv.tv_sec=maxwaittime/1000;                                       //10秒
   tv.tv_usec=maxwaittime%1000*1000;                                 //0 微妙
   FD_ZERO(&readfd);                                                 //清除指定的文件描述符
   FD_SET(GPRSfd,&readfd);                                           //在文件描述符集中添加一个新的文件描述符
   /* 实现多路复用输入/输出型，如果在指定的时间内没有事件发生，select将会超时 */
   selectnum=select(GPRSfd+1,&readfd,NULL,NULL,&tv);                 
   if(selectnum>0)
   {
      rcnum=read(GPRSfd,buf,len);
      return rcnum; 
   }
   else
      return -1;
}

/*************************************************************************************************************
** 函数名称：writeport()
** 功能描述：通过串口写缓存区内的数据 
** 输入：    GPRSfd:文件描述符             buf:缓冲区            len：长度
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改日期：
*************************************************************************************************************/
ST_INT32 writeport(ST_INT32 GPRSfd,ST_CHAR *buf,ST_INT32 len)             //向GPRS模块发送数据
{
	 ST_INT32 writenum;
   writenum=write(GPRSfd,buf,len);                                         //通过串口发送缓冲区的数据
   return writenum;
}

/*************************************************************************************************************
** 函数名称：check_recv()
** 功能描述：判断接收数据
** 输入：    buf:缓冲区                 num:传递到函数中的整数
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改日期：
*************************************************************************************************************/
ST_INT32 check_recv(ST_CHAR *buf,ST_INT32 num)
{
   switch(num)
   {
   	  case 1:
   	  case 5:
   	  case 6:
   	  	   if(strcmp(&buf[2],"OK\r\n")==0)
   	  	   {
   	  	      return 0;
   	  	   }
   	  	   else
   	  	   	  return -1;
   	  	   break;
   	  case 2:
   	  	   if(strcmp(&buf[2],"+CPIN: READY\r\n\r\nOK\r\n")==0)
   	  	   {
   	  	      return 0;
   	  	   }
   	  	   else
   	  	   	  return -1;
   	  	   break;
   	  case 3:
   	  	   if(strcmp(&buf[2],"+CGREG: 0,1\r\n\r\nOK\r\n")==0)
   	  	   {
   	  	      return 0;
   	  	   }
   	  	   else
   	  	   	  return -1;
   	  	   break;
   	  case 4:
   	  	   if(strcmp(&buf[2],"+CGATT: 1\r\n\r\nOK\r\n")==0)
   	  	   {
   	  	      return 0;
   	  	   }
   	  	   else
   	  	   	  return -1;
   	  	   break;
   	  case 7:
           if(strcmp(&buf[2],"SHUT OK\r\n")==0) 
           { 
              return 0;
           }
           else
              return -1;
           break;
      case 8:
           if(strcmp((ST_CHAR *)&buf[2],"CONNECT OK\r\n")==0)
           { 
              return 0;
           }
           else
              return -1;
           break;
      case 9:
           if(strcmp(&buf[2],"> ")==0) 
           { 
              return 0;
           }
           else
              return -1;
           break;
      case 10:
           if(strcmp(&buf[2],"SEND OK\r\n")==0) 
           { 
              return 0;
           }
           else
              return -1;
           break; 
   	  default:
   	  	   return -1;
   }
}

/*************************************************************************************************************
** 函数名称：module_shut()
** 功能描述：关闭模块
** 输入：    GPRSfd: 文件描述符
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改记录：
*************************************************************************************************************/
ST_INT32 module_shut(ST_INT32 GPRSfd)
{
	ST_INT32 nread=0;
	ST_CHAR rbuf[512];
	printf("Send:AT+CIPSHUT\n");
	writeport(GPRSfd,"AT+CIPSHUT\r\n",strlen("AT+CIPSHUT\r\n"));
	nread=readport(GPRSfd,rbuf,sizeof(rbuf),10000);
	rbuf[nread]='\0';
	if(check_recv(rbuf,7)!=0)
	{
		printf("Send:AT+CIPSHUT %s !ERROR!",rbuf);
		return 0;
	}
	else
		printf("Recv:%s",rbuf);
	return 1; 
}


void CloseGprs(ST_INT32 GPRSfd)
{
	module_shut(GPRSfd);
	close(GPRSfd);
}


ST_RET cdzProtocolExit_Gprs(ST_INT32 GPRSfd)
{
	ProtocolExitFlag_Gprs = true;
	sleep(WAIT_THREAD_EXIT_TIME);

	CloseGprs(GPRSfd);

	return SD_SUCCESS;
}

/*************************************************************************************************************
** 函数名称：module_init()
** 功能描述：初始化模块
** 输入：    GPRSfd: 文件描述符
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改记录：
*************************************************************************************************************/
ST_INT32 module_init(ST_INT32 GPRSfd)
{
	 ST_INT32 readnum=-1,i=5;
	 ST_INT32 nwriteport,shutnum;
//	 ST_UCHAR rbuf[512];
 
   printf("Send:AT\n");
CHK: nwriteport=writeport(GPRSfd,"AT\r\n",strlen("AT\r\n"));
   readnum=readport(GPRSfd,rbuf,sizeof(rbuf),10000); 
   rbuf[readnum]='\0';
      
   if(check_recv(rbuf,1)!=0)
   {
      printf("There is something wrong:%s",rbuf);
      while(i--)
      {
         usleep(200000);
         goto CHK;
      }
      printf("Connect failure, Please check equipment.\n");
      return 0;
   }
   else 
      printf("Recv:%s",rbuf); 
       
   if(readnum>0)                                                          //查询模块是否设置PIN码
   {  
   	  printf("Send:AT+CPIN?\n"); 
      writeport(GPRSfd,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
      readnum=readport(GPRSfd,rbuf,sizeof(rbuf),10000);
      rbuf[readnum]='\0';
      if(check_recv(rbuf,2)!=0)
      {
      	 printf("Recv:%s",rbuf);      
         return 0;
      }
      else
      {
      	 printf("Recv:%s",rbuf); 
      }
   }
   
   if(readnum>0)                                                          //查询模块是否注册网络
   {  
   	  printf("Send:AT+CGREG?\n"); 
      writeport(GPRSfd,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
      readnum=readport(GPRSfd,rbuf,sizeof(rbuf),10000);
      rbuf[readnum]='\0';
      if(check_recv(rbuf,3)!=0)
      { 
      	 printf("Recv:%s",rbuf);     
         return 0;
      }
      else
      {
      	 printf("Recv:%s",rbuf); 
      }
   }
   
   if(readnum>0)                                                          //查询模块是否附着GPRS网络
   {  
   	  printf("Send:AT+CGATT?\n"); 
      writeport(GPRSfd,"AT+CGATT?\r\n",strlen("AT+CGATT?\r\n"));
      readnum=readport(GPRSfd,rbuf,sizeof(rbuf),10000);
      rbuf[readnum]='\0';
      if(check_recv(rbuf,4)!=0)
      { 
      	 printf("Recv:%s",rbuf);     
         return 0;
      }
      else
      {
      	 printf("Recv:%s",rbuf); 
      }
   }
   
   if(readnum>0)                                                          //设置APN
   {  
   	  printf("send:AT+CSTT\n"); 
      writeport(GPRSfd,"AT+CSTT\r\n",strlen("AT+CSTT\r\n"));
      readnum=readport(GPRSfd,rbuf,sizeof(rbuf),10000);
      rbuf[readnum]='\0';
      if(check_recv(rbuf,5)!=0)
      {  
      	 printf("Recv:%s",rbuf);    
         if(strcmp(&rbuf[2],"ERROR\r\n")==0)
         {
         	  shutnum=module_shut(GPRSfd);                                  //GPRS模块关闭连接
         	  if(shutnum>0)
               printf("The module connectting is shut successfully\n");
            else 
               printf("The module connectting is shut ERROR!!!!!!!!!!\n"); 
         }
         return 0;
      }
      else
      {
      	 printf("Recv:%s",rbuf); 
      }
   }
   
   if(readnum>0)                                                          //激活移动场景
   {  
   	  printf("send:AT+CIICR\n"); 
      writeport(GPRSfd,"AT+CIICR\r\n",strlen("AT+CIICR\r\n"));
      readnum=readport(GPRSfd,rbuf,sizeof(rbuf),10000);
      rbuf[readnum]='\0';
      if(check_recv(rbuf,6)!=0)
      {     
         return 0;
      }
      else
      {
      	 printf("Recv:%s",rbuf); 
         return 1;
      }
   }
   return 0;
}

/*************************************************************************************************************
** 函数名称：module_connect()
** 功能描述：模块网络连接
** 输入：    GPRSfd: 文件描述符         sbuf: 缓冲区
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改记录：2011.5.31
*************************************************************************************************************/
ST_INT32 module_connect(ST_INT32 GPRSfd,ST_CHAR *sbuf)
{
	 ST_INT32 rc,shutnum;
	 ST_CHAR rbuf[512];
	 
	printf("Rend:%s\n",sbuf);  
	writeport(GPRSfd,sbuf,strlen(sbuf));                        //发送IP地址，连接网络
   
   rc=readport(GPRSfd,rbuf,sizeof(rbuf),10000);                        //等待接收”OK“
   rbuf[rc]='\0';
   printf("Recvnum:%d\n",rc);   
   printf("Recv:%s\n",rbuf);
   
   rc=readport(GPRSfd,rbuf,sizeof(rbuf),10000);                        //等待接收"CONNECT OK"
   rbuf[rc]='\0';
   printf("Recvnum:%d\n",rc);   
   printf("Recv:%s\n",rbuf);
   
   if(check_recv(rbuf,8)!=0)
   {
      printf("recv:%s",rbuf);    
      if(strcmp(&rbuf[2],"ERROR\r\n")==0)
      {
         shutnum = module_shut(GPRSfd);                                  //GPRS 模块关闭连接
         if(shutnum>0)
            printf("The module connectting is shut successfully\n");
         else 
            printf("The module connectting is shut ERROR!!!!!!!!!!\n"); 
      }
      else if(strcmp(&rbuf[2],"ERROR\r\n\r\nALREADY CONNECT\r\n")==0)
      {
      	 shutnum=module_shut(GPRSfd);                                  //GPRS 模块关闭连接
         if(shutnum>0)
            printf("The module connectting is shut successfully\n");
         else 
            printf("The module connectting is shut ERROR!!!!!!!!!!\n");
      }
      else if(strcmp(&rbuf[2],"OK\r\n")==0)
      {
      	 shutnum=module_shut(GPRSfd);                                  //GPRS 模块关闭连接
         if(shutnum>0)
            printf("The module connectting is shut successfully\n");
         else 
            printf("The module connectting is shut ERROR!!!!!!!!!!\n");
      }
      //goto CHKCONNECT;
      return 0;
   }
   else
   {
   	  return 1;
   } 
}

/*************************************************************************************************************
** 函数名称：module_send()
** 功能描述：发送数据
** 输入：    GPRSfd: 文件描述符          datasend: 发送数据
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改记录：
*************************************************************************************************************/
ST_INT32 module_send(ST_INT32 GPRSfd,ST_CHAR datasend[])
{
   ST_INT32 rc,len;
   ST_CHAR datasend_1[1024];
	ST_CHAR rbuf[512];
   
   printf("Send:AT+CIPSEND\n"); 
   writeport(GPRSfd,"AT+CIPSEND\r\n",strlen("AT+CIPSEND\r\n"));        //发送AT＋CIPSEND
   rc=readport(GPRSfd,rbuf,sizeof(rbuf),10000);                        //等待读 >  
   rbuf[rc]='\0';
   if(check_recv(rbuf,9)!=0)                                           //判断接收 >  
   {
      printf("Recv:%s",rbuf);
      return 0;
   }
   else
   {
   	  printf("Needed >:%s\n",rbuf);
   }        
  
   sprintf(datasend_1,"%s%c",datasend,0x1a);                           //打印到字符串
   len=strlen(datasend_1);   
   writeport(GPRSfd,datasend_1,len);
       
   rc = readport(GPRSfd,rbuf,sizeof(rbuf),10000);  
   rbuf[rc]='\0';
   if(check_recv(rbuf,10)!=0)                                          //判断接收 SEND OK 
   {
      printf("Send data failure:%s\n",rbuf);
      return 0;
   }
   else
   {
   	  printf("Needed \"Send OK\":\n-----%s-----\n",rbuf);
      return 1;
   }	
}


ST_INT32  SetRcvPara(ST_INT32 GPRSfd)
{
	ST_INT32 rc;
	ST_CHAR rbuf[512];
	
	printf("Send:AT+CIPHEAD=0\n"); 
	writeport(GPRSfd,"AT+CIPHEAD=0\r\n",strlen("AT+CIPHEAD=0\r\n"));        //发送AT+CIPHEAD=0
	rc=readport(GPRSfd,rbuf,sizeof(rbuf),10000);                              
	rbuf[rc]='\0';   
	if(check_recv(rbuf,1)!=0)
	{
	  printf("Recv:%s",rbuf);
	  return 0;
	}
	else
	{
	  printf("Recv:%s",rbuf);
	}      
	  
	printf("Send:AT+CIPSRIP=0\n"); 
	writeport(GPRSfd,"AT+CIPSRIP=0\r\n",strlen("AT+CIPSRIP=0\r\n"));        //发送AT+CIPSRIP=0
	rc=readport(GPRSfd,rbuf,sizeof(rbuf),10000);                              
	rbuf[rc]='\0';   
	if(check_recv(rbuf,1)!=0)
	{
	  printf("Recv:%s",rbuf);
	  return 0;
	}
	else
	{
	  printf("Recv:%s",rbuf);
	} 
	
	printf("Send:AT+CIPSHOWTP=0\n"); 
	writeport(GPRSfd,"AT+CIPSHOWTP=0\r\n",strlen("AT+CIPSHOWTP=0\r\n"));    //发送AT+CIPSHOWTP=0
	rc=readport(GPRSfd,rbuf,sizeof(rbuf),10000);                              
	rbuf[rc]='\0';   
	if(check_recv(rbuf,1)!=0)
	{
	  printf("Recv:%s",rbuf);
	  return 0;
	}
	else
	{
	  printf("Recv:%s",rbuf); 
	} 	
	return 0;
}
/*************************************************************************************************************ur
** 函数名称：module_receive()
** 功能描述：发送数据
** 输入：    GPRSfd: 文件描述符
** 输出：    无
** 返回值：  整型
** 作者：    刘泉承
** 日期：    2011.5.30
** 修改记录：
*************************************************************************************************************/
ST_INT32 module_receive(ST_INT32 GPRSfd,ST_CHAR Rcvbuf[])
{
	ST_INT32 rc; 

	printf("Receive data coming from remote server:\n");
	rc=readport(GPRSfd,Rcvbuf,sizeof(Rcvbuf),10000);
	Rcvbuf[rc]='\0';
	printf("Recv:%s\n",Rcvbuf);
	return 1;                                                               
}

/*************************************************************************************************************
** 函数名称：GprsOperation()
** 功能描述：GPRS操作
** 输入：    无
** 输出：    无
** 返回值：　无
** 作者：　　刘泉承
** 日期：    2011.5.30
** 修改记录：
*************************************************************************************************************/
#if 0
 GprsOperation(ST_VOID)

{
	 //开始执行AT命令
   rc=module_init(GPRSfd);                                            //GPRS模块初始化
   if(rc>0)
   	  printf("The module has been connected to the Serial Port\n"); 
   else
   	  printf("There is no module detected\n"); 
   
   rc=module_connect(GPRSfd,wbuf);                                    //GPRS模块使用IP地址连接网络
   if(rc>0)
      printf("The module has been successfully connected \n");
   else 
      printf("The module connectting ERROR!!!!!!!!!!\n"); 
      
   tcflush(GPRSfd,TCIOFLUSH);
   strcpy(fsend,"liuquancheng");
   rc=module_send(GPRSfd,fsend);                                      //GPRS模块发送数据 
   if(rc>0)
      printf("Data successfully send \n");
   else 
      printf("Data  send  ERROR!!!!!!!!!!\n");
      
   rc=module_receive(GPRSfd);                                         //GPRS模块接收数据   
   if(rc>0)
      printf("Data successfully receive \n");
   else 
      printf("Data  receive  ERROR!!!!!!!!!!\n");
      
   rc=module_shut(GPRSfd);                                            //GPRS模块关闭连接
   if(rc>0)
      printf("The module connectting is shut successfully\n");
   else 
      printf("The module connectting is shut ERROR!!!!!!!!!!\n");
   
   close(GPRSfd);
}

#endif

/********************************************************************************* 
 函数名称： cdzProtocolDataProcess
 功能描述：	规约数据处理主函数
 输    入：	sDataRecvbuf:接收到数据存放在此处
			sDataLen:接收到的数据长度
 输	   出： 无
 返 回 值： 发送数据的长度
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static ST_UINT16 cdzProtocolDataProcess(ST_UCHAR sLastFSeq, const ST_UCHAR *sDataRecvbuf, ST_UINT16 sDataLen,
										ST_UCHAR *sDataResbuf)
{
	ST_UCHAR sCityCode;					
	ST_UCHAR sCountryCode;				
	ST_UINT16 sConsoleAddr;				
	ST_UCHAR sMstaAddr;					
	ST_UCHAR sISeq;						
	ST_UCHAR sFSeq;						
	ST_UCHAR sFuncCode;					
	ST_UINT16 sDataDomainLen;			

	if (sDataRecvbuf == NULL || sDataLen == 0 || sDataResbuf == NULL) 
	{
		printf("failed sDataLen is 0\n");
		return 0 ;
	}


	if (cdzProtocolCheckDataBuf(sDataRecvbuf, sDataLen) == SD_FAILURE)
	{
		printf("failed is checkbuf\n");
	 	return 0;
	}

	sCityCode = sDataRecvbuf[1];
	sCountryCode = sDataRecvbuf[2];
	sConsoleAddr = sDataRecvbuf[3]*256 + sDataRecvbuf[4];

	sMstaAddr = sDataRecvbuf[5] & 0x3F;	
	sISeq = (sDataRecvbuf[6] & 0xE0) >> 5;	
	sFSeq = ((sDataRecvbuf[5] & 0xC0) >> 6) + (sDataRecvbuf[6] & 0x1F)<<2;		

	sFuncCode = sDataRecvbuf[8] & 0x3F;

	if ((sDataRecvbuf[8] & (1<<7)) != DIR_M_T_C)
	{
		return cdzProtocolSendErrFrame(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], sDataResbuf,SET_PARAM_INVALID);
	}

	if ((sDataRecvbuf[8] & (1<<6)) != FRAME_OK)
	{
		printf("failed is frame is not ok\n");
		return 0;
	}


	if (sISeq != 0)
	{
		return cdzProtocolSendErrFrame(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], sDataResbuf,SET_PARAM_INVALID);
	}

	sDataDomainLen = sDataRecvbuf[9] + sDataRecvbuf[10] * 256;


	switch(sFuncCode)
	{
	case READ_CURRENT_DATA:				
		return cdzProtocosReadCurDataRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		printf("read8010 is OK!\n");
		break;
	case READ_TASK_DATA:				
		return cdzProtocosReadTaskDataRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case READ_CHARGE_RECORD:			
		return cdzProtocosReadChargeRecordRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case READ_PROGRAM_LOG:		
		return cdzProtocosReadProgramLogRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case RT_WRITE_OBJECT_PARAM:	
		return cdzProtocosRtWriteParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case WRITE_OBJECT_PARAM:
		printf("set para frame\n");
		return cdzProtocosWriteParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case READ_ALARM:
		return cdzProtocosReadAlarmRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case GET_ESAM_RAND:	
		return cdzProtocosGetEsamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case WRITE_KEY_PARAM:
		return cdzProtocosWriteKeyParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case RT_WRITE_KEY_PARAM:
		return cdzProtocosRtWriteKeyParamRes(sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case ALARM_CONFIRM:	
		cdzProtocosAlarmConfirmRes(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;
	case CHARGEREC_CONFIRM:
		cdzProtocosChargeRecConfirmRes(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		printf("ChargeRecord OK!\n");
		break;
	case KEEP_ALIVE:
		return cdzProtocosKeepALiveRes(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		break;

	case LAND_CONFIRM:
		cdzProtocosConfirmLand(sLastFSeq, sFSeq, sFuncCode, sMstaAddr, &sDataRecvbuf[1], &sDataRecvbuf[11], sDataDomainLen, sDataResbuf);
		printf("Land OK!\n");
		break;
	default:
		break;
	}


	return 0;

}

/********************************************************************************* 
 函数名称： cdzProtocolProcess
 功能描述：	规约处理接收线程
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static void RcvGprsProtocolProcess(void)
{
	ST_INT32 RecvLen,SendLen; 
	ST_UCHAR RecvBuf[LENGTH];
	ST_UCHAR SendBuf[LENGTH];
	
	while(!ProtocolExitFlag_Gprs&&GprsConnectFlag==1)
	{
	
		RecvLen = module_receive(GPRSfd, (ST_CHAR *)RecvBuf);
		
		if (RecvLen == 0)
		{
			continue;
		}

		else if (RecvLen > 0)
		{
			printf("received data\n");
			SendLen = cdzProtocolDataProcess(LastFSeq_Gprs, RecvBuf, RecvLen, SendBuf);
			printf("SendLen is %d\n",SendLen);
			if (SendLen == 0xFFFF)
			{
				KeeALiveFlag_Gprs = true;
			}
			else if (SendLen > 0)
			{
				SendBuf[5] = RecvBuf[5];
				SendBuf[6] = RecvBuf[6];
				SendBuf[SendLen-2] =  cdzCalcCS(SendBuf, (SendLen-2));
				module_send(GPRSfd, (ST_CHAR *)&SendBuf[0]);
				printf("send data\n");
			}
		}		
		usleep(10 * 1000);	
	}
	sleep(1);
}

/********************************************************************************* 
 函数名称： cdzProtocolProcess
 功能描述：	规约处理发送线程
 输    入：	lparam:用户参数
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

extern ST_INT32 CurrentKeepLiveCount; 
static void SendGprsProtocolProcess(void *lparam)
{
	static ST_INT32 sLastSendKeepAliveTimeSec;
	//ST_UCHAR sRecvBuf[LENGTH];
	ST_UCHAR sSendBuf[LENGTH];
	ST_UINT16 sRecvLen;
	ST_UINT16 sTotalLen;
        static ST_INT32 GprsInitResult;
	ST_UINT16 sSendLen = 0;
	//ST_BOOL sSendFlag;
	ST_CHAR IP_PORTBuf[100];
	
	while(!ProtocolExitFlag_Gprs)
	{
		if (GprsInitResult == 0)
		{
			GprsInitResult = module_init(GPRSfd);	
			sleep(TRY_CONNECT_TIME);
			continue;
		}
		sprintf(IP_PORTBuf,"AT+CIPSTART=\"TCP\",\"113.89.207.191\",1115\r\n");	
	//	sprintf(IP_PORTBuf,"AT+CIPSTART=\"TCP\",%s,%d\r\n",s_cdzProtocolParam_Gprs.remoteIP,s_cdzProtocolParam_Gprs.remotePort);
		if (module_connect(GPRSfd,IP_PORTBuf) == 0)
		{

			//CloseGprs(GPRSfd);
			sleep(TRY_CONNECT_TIME);
			continue;
		}
		SetRcvPara(GPRSfd);
		printf("connect success\n");		
		GprsConnectFlag = true;
		KeeALiveFlag_Gprs = true;
		sLastSendKeepAliveTimeSec = CurrentKeepLiveCount;
		sTotalLen = 0;
		sRecvLen = 0;

		ProtolLandFlag_Gprs = false;
		
		while(!ProtocolExitFlag_Gprs)
		{
	        if(!ProtolLandFlag_Gprs) 
		    {
				cdzProtocolSendLoadFrame_Gprs();
                
				usleep(200*1000);
		    }
			if(ProtolLandFlag_Gprs)
			{

				cdzProtocolUpload_Gprs(&FSeq_Gprs, &LastFSeq_Gprs);
			}

			if ((CurrentKeepLiveCount - sLastSendKeepAliveTimeSec) > s_cdzProtocolParam_Gprs.keepAliveVal)
			{
				if (KeeALiveFlag_Gprs == false)
				{
					CloseGprs(GPRSfd);
					sleep(TRY_CONNECT_TIME);
					break;
				}

				sSendLen = cdzProtocolSendKeepAliveFrame(FSeq_Gprs, KEEP_ALIVE, UPLOAD_MSTA, s_RtuA_Gprs, sSendBuf);
				LastFSeq_Gprs = FSeq_Gprs;
				FSeq_Gprs = cdcCalcFSeq(FSeq_Gprs);
				
				module_send(GPRSfd, (ST_CHAR *)&sSendBuf[0]);
				KeeALiveFlag_Gprs = false;
				sLastSendKeepAliveTimeSec = CurrentKeepLiveCount;
			}
			usleep(10 * 1000);
		}
	}
}


/********************************************************************************* 
 函数名称： cdzGprsInit
 功能描述：	GPRS初始化
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
 		   SD_FAILURE:失败
 作    者：	wantao
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_RET cdzGprsInit(void)
{
	int result;
	pthread_t SendGprsProtocol,RcvGprsProtocol;  
					
	ST_UCHAR dataitemContent[9];
	ST_UCHAR i = 0;
	ST_INT16 ContentLen = 0;

	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8010,dataitemContent);
	if (ContentLen <=0 ) return SD_FAILURE;
	//if (dataitemContent[0] != 0x04) return SD_FAILURE;

	i ++;

	while(dataitemContent[i] == 0xAA)
	{
		i++;
	}
	if (i > 3) return SD_FAILURE;
	 printf("GPRSinitLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLl\n");

	sprintf((char *)s_cdzProtocolParam_Gprs.remoteIP, "%d.%d.%d.%d", dataitemContent[i], dataitemContent[i+1], dataitemContent[i+2], dataitemContent[i+3]);
	printf("remoteip:%s",s_cdzProtocolParam_Gprs.remoteIP);

	s_cdzProtocolParam_Gprs.remotePort = dataitemContent[i+4] * 256 + dataitemContent[i+5];

	ST_UCHAR TermCityCode[2];
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8016,TermCityCode);
	if (ContentLen <= 0 ) return SD_FAILURE;
	printf("citycode is\n");
	printf("%2x\n",TermCityCode[0]);
	printf("%2x\n",TermCityCode[1]);

	ST_UCHAR TermAdd[2];
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8017,TermAdd);
	if (ContentLen <= 0 ) return SD_FAILURE;
	printf("Termadd is\n");
	printf("%2X\n",TermAdd[0]);
	printf("%2X\n",TermAdd[1]);

	s_cdzProtocolParam_Gprs.TermAdd[0] = TermCityCode[0];
	s_cdzProtocolParam_Gprs.TermAdd[1] = TermCityCode[1];
	s_cdzProtocolParam_Gprs.TermAdd[2] = TermAdd[0];
	s_cdzProtocolParam_Gprs.TermAdd[3] = TermAdd[1];
	printf("cdztermadd is\n");
	printf("%2x\n",s_cdzProtocolParam_Gprs.TermAdd[0]);
	printf("%2x\n",s_cdzProtocolParam_Gprs.TermAdd[1]);
	printf("%2x\n",s_cdzProtocolParam_Gprs.TermAdd[2]);
	printf("%2x\n",s_cdzProtocolParam_Gprs.TermAdd[3]);
	
    ST_UCHAR heartContent;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x8018, &heartContent);
	if (ContentLen <=0 ) return SD_FAILURE;
	printf("heartcontent is\n");
	printf("%2x\n",heartContent);
	s_cdzProtocolParam_Gprs.keepAliveVal = CDZ_BCDTOHEX(heartContent);


	ST_UCHAR TCPUDPIdent;
	ContentLen = cdzReadDataItemContentMeaPoint(0x00, 0x801C, &TCPUDPIdent);
	if (ContentLen <=0 ) return SD_FAILURE;
    if(TCPUDPIdent==0x01)
	{
	   s_cdzProtocolParam_Gprs.iProtoType = PROTOCOLTP_UDP;
	}
	else
	{
	   s_cdzProtocolParam_Gprs.iProtoType = PROTOCOLTP_TCP;
	}

	result = pthread_create(&SendGprsProtocol, PTHREAD_CREATE_JOINABLE, (void *)SendGprsProtocolProcess ,NULL);
	if(result)
	{
		perror("pthread_create: MainControl.\n");
		exit(EXIT_FAILURE);
	}
	result = pthread_create(&RcvGprsProtocol, PTHREAD_CREATE_JOINABLE, (void *)RcvGprsProtocolProcess ,NULL);
	if(result)
	{
		exit(EXIT_FAILURE);
	}

	
	return SD_SUCCESS;

}

/********************************************************************************* 
 函数名称： cdzProtocolUpload
 功能描述：	主动上传的数据处理，主要有告警类和充电记录数据两类
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static void cdzProtocolUpload_Gprs(ST_UCHAR *psFSeq, ST_UCHAR *pLastFeq)
{
   // ST_UINT16 sDataLen;
    ST_INT32 sSendLen=0;
    ST_UCHAR sSendResbuf[LENGTH];

	if(!IsNewChargeRecord_Gprs())
	{
	   ChargeRecordTP  pChargeRecord;
	   static int ResendChargeRecordCount = 3;

	   static ST_UINT32 OldsInterSeq = 0xFFFFFFFF;
	   
	   if(!GetNewChargeRecord_Gprs(&pChargeRecord))
	   {


			if(OldsInterSeq != ChargeMsg.InerSeq)
			{
				ResendChargeRecordCount = 3;
				OldsInterSeq = ChargeMsg.InerSeq;
			}
			else
			{
				if(ResendChargeRecordCount == 0)
				{
					ConfirmChargeRecord_Gprs();
					return;
				}
			}

			

			sSendResbuf[sSendLen++] = CDZ_FRAME_START;

			sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[0] ;
			sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[1];
	    	sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[2] ;
			sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[3] ;
			sSendResbuf[sSendLen++] = ((*psFSeq & 0x03) << 6);
	   		sSendResbuf[sSendLen++] = ((*psFSeq & 0x7C) >> 2);
      		sSendResbuf[sSendLen++] = CDZ_FRAME_START;
			
			sSendResbuf[sSendLen++] = 0x83;
	
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
        
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
	
			sSendResbuf[sSendLen++] = 0x01;
        
     		sSendResbuf[sSendLen++] =pChargeRecord.sExchangeType;
      		ST_UCHAR i = 0;
		    for (i=0; i<5; i++)
		    {
				 sSendResbuf[sSendLen++] = pChargeRecord.sExchangeSeq[i] ;
		    }
	      
		    for (i=0; i<3; i++)
		    {
				sSendResbuf[sSendLen++] = pChargeRecord.sAreaCode[i];
		    }
        
			for (i=0; i<8; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartCardNo[i];
			}
			for (i=0; i<8; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndCardNo[i];
			}
			for (i=0; i<2; i++)
	    	{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartCardType[i];
			}
			for (i=0; i<2; i++)
	   	 	{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndCardType[i];
			}
				for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge1[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge2[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge3[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sStartEnerge4[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge[i];
			}
	
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge1[i];
			}
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge2[i];
			}
        	for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge3[i];
			}            
			
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sEndEnerge4[i];
			}
			
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice1[i];
			}
           	for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice2[i];
			}            
           
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice3[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPrice4[i];
			}
	
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sParkFeePrice[i];
			}
        
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[0];
		 	sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[1];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[2];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[3];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[4];
			sSendResbuf[sSendLen++] = pChargeRecord.sStartTime[5];
	
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[0];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[1];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[2];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[3];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[4];
			sSendResbuf[sSendLen++] = pChargeRecord.sEndTime[5];
	
	      	for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sParkFee[i];
			}
			for (i=0; i<3; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sGasPrice[i];
			}
			for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sGasFee[i];
			}
			
     		for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sMoneyBefore[i];
			}
	
      		for (i=0; i<4; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sMoneyAfter[i];
			}
	
      		for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sCardCounter[i];
			}
	
      		for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sTermNo[i];
			}
	
     		for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sCardVer[i];
			}
	
      		for (i=0; i<5; i++)
			{
				sSendResbuf[sSendLen++] = pChargeRecord.sPosNo[i];
			}
			sSendResbuf[sSendLen++] = pChargeRecord.sCardStatus;
	
			sSendResbuf[9]=(sSendLen-11)%256;
			sSendResbuf[10]=(sSendLen-11)/256;    
	
			sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_END;
			if(sSendLen < LENGTH)
			{
				ResendChargeRecordCount--;
			    module_send(GPRSfd, (ST_CHAR *)&sSendResbuf[0]);
			printf("GPRS send PPPPPPPPPPPPPPPPPPPPPPPPP\n");
			    *pLastFeq = *psFSeq;
			    *psFSeq = cdcCalcFSeq(*psFSeq); 
			    sleep(2);
				//cdzRecvData

			   // return;
			}
	   }
	}
  
   if(!IsNewAlarmRecord_Gprs())
   {
   		AlarmRecordTP vAlarmRecord,oldAlarmRecord; 		
	    static int AlarmResendCount = 3;
	   
	   	if(!GetNewAlarmRecord_Gprs(&vAlarmRecord))
	   	{
		    if(memcmp(&oldAlarmRecord,&vAlarmRecord,sizeof(AlarmRecordTP)) != 0)
		    {
				AlarmResendCount = 3;
				memcpy(&oldAlarmRecord,&vAlarmRecord,sizeof(AlarmRecordTP));  
		    }
			else 
			{
				if(AlarmResendCount==0)
				{
					ConfirmAlarmRecord_Gprs();
					return;
				}
			}
			
	    //    ST_UINT16 sDataLen;
			ST_UCHAR sSendResbuf[LENGTH];
			int sSendLen=0;
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_START;
	
			sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[0] ;
	        sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[1] ;
		    sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[2] ;
	        sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[3] ;
	        
			sSendResbuf[sSendLen++] = ((*psFSeq & 0x03) << 6);
			sSendResbuf[sSendLen++] = ((*psFSeq & 0x7C) >> 2);
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_START;
			
			
		
			sSendResbuf[sSendLen++] = 0x89;
	
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x00;
			sSendResbuf[sSendLen++] = 0x01;              //告警数量
				
			sSendResbuf[sSendLen++] = vAlarmRecord.MeaPointNum; //测量点号
			
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[0];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[1];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[2];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[3];
			sSendResbuf[sSendLen++] = vAlarmRecord.Time[4];
			
			sSendResbuf[sSendLen++] = vAlarmRecord.AlarmCode[0];
			sSendResbuf[sSendLen++] = vAlarmRecord.AlarmCode[1];
			ST_CHAR i ;
			for(i = 0;i<vAlarmRecord.AlarmContentLen;i++)
			{
				sSendResbuf[sSendLen++] = vAlarmRecord.AlarmContent[i];
			}
			sSendResbuf[9]=(sSendLen-11)%256;
			sSendResbuf[10]=(sSendLen-11)/256;    
	
			sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);
	
			sSendResbuf[sSendLen++] = CDZ_FRAME_END;
			if((sSendLen < LENGTH) && (sSendLen > 13))
			{
				AlarmResendCount--;
				module_send(GPRSfd, (ST_CHAR *)&sSendResbuf[0]);
				*pLastFeq = *psFSeq;
				*psFSeq = cdcCalcFSeq(*psFSeq); 
				sleep(2);
				return;
			}
		}
   }

}

/********************************************************************************* 
 函数名称： cdzProtocolSendLoadFrame
 功能描述：	发送登陆主站帧
 输    入：	sFSeq:帧序号
			sFuncCode:功能码
			sMstaAddr:主站地址
			sRtuAddr:终端逻辑地址
			sSendResbuf:响应数据的缓冲区
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static void cdzProtocolSendLoadFrame_Gprs(void)
{
	ST_UCHAR sSendResbuf[LENGTH];
	ST_UINT16 sSendLen = 0;
	

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[0];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[1];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[2];
	sSendResbuf[sSendLen++] = s_cdzProtocolParam_Gprs.TermAdd[3];


	sSendResbuf[sSendLen++] = ((0x00 & 0x03) << 6);
	sSendResbuf[sSendLen++] = ((0x00 & 0x7C) >> 2);

	sSendResbuf[sSendLen++] = CDZ_FRAME_START;

	sSendResbuf[sSendLen++] = 0xA1;

	sSendResbuf[sSendLen++] = 0x03;
	sSendResbuf[sSendLen++] = 0x00;
	sSendResbuf[sSendLen++] = 0x11;
	sSendResbuf[sSendLen++] = 0x11;
	sSendResbuf[sSendLen++] = 0x11;

	sSendResbuf[sSendLen++] = cdzCalcCS(sSendResbuf, sSendLen);

	sSendResbuf[sSendLen++] = CDZ_FRAME_END;

	if(sSendLen < LENGTH)
	{
	    module_send(GPRSfd, (ST_CHAR *)&sSendResbuf[0]);
		
		//s_ProtolLandFlag = true;
	    usleep(20*1000);
	    return;
	}
}
