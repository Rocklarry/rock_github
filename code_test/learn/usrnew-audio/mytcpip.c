#include <stdio.h> 
#include <stdlib.h>
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include "mytcpip.h"


static CDZ_SOCKET_INFO SoketInfo;  //soket信息

/********************************************************************************* 
 函数名称： CreateSocket
 功能描述： 创建soket套接字
 输    入：	无
 输	   出： 无
 返 回 值：创建失败返回SD_FAILURE；创建成功返回SoketInfo.Socketfd
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
       
ST_INT32 CreateSocket(const ST_INT32 procotoltype)
{
	if(procotoltype==PROTOCOLTP_TCP)
	{
		SoketInfo.ProcoType = PROTOCOLTP_TCP;
		SoketInfo.Socketfd = socket(AF_INET, SOCK_STREAM, 0);
		
	}
	if(procotoltype==PROTOCOLTP_UDP)
	{
		SoketInfo.ProcoType = PROTOCOLTP_UDP;
		SoketInfo.Socketfd  = socket(AF_INET, SOCK_DGRAM, 0);
		
	}

	return SoketInfo.Socketfd;
}


/********************************************************************************* 
 函数名称： ConnectServer
 功能描述： 连接到服务器
 输    入：	remoteip：远程服务器ip
 输	   出： 无
 返 回 值：连接服务器成功返回SD_SUCCESS；连接失败：SD_FAILURE
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


ST_INT32 ConnectServer(const ST_UCHAR *remoteip)
{
	struct sockaddr_in remote_addr;                             // Host address information
	remote_addr.sin_family = AF_INET;                           // Protocol Family
    remote_addr.sin_port = htons(REMOTE_PORT);                 // Port number
	
    inet_pton(AF_INET, (ST_CHAR *)remoteip, &remote_addr.sin_addr);     // Net Address
	bzero(&(remote_addr.sin_zero), 8);
	
	if (connect(SoketInfo.Socketfd, (struct sockaddr *)&remote_addr,  sizeof(struct sockaddr)) == -1) 
    {
        return SD_FAILURE;
    }  
	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： CloseSocket
 功能描述： 关闭连接
 输    入：无
 输	   出： 无
 返 回 值：无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

void CloseSocket(void)
{
	SoketInfo.Socketfd = -1;
	SoketInfo.ProcoType = -1;
	close(SoketInfo.Socketfd);
}

/********************************************************************************* 
 函数名称： ReciveData
 功能描述： 接收数据
 输    入：无
 输	   出： 无
 返 回 值：0：接收错误或断开连接，>0:接收到的数据个数
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_INT32 ReciveData(ST_INT32 socketfd, ST_UCHAR *revbuf, ST_INT32 len)
{
	ST_INT32 revnum = 0;
	if(SoketInfo.ProcoType==PROTOCOLTP_TCP)             //暂时先处理tcp协议，udp稍后处理
	{
		revnum = recv(socketfd, revbuf, len, 0);
		switch(revnum)
		{
			case -1:                                 //接收错误
				close(socketfd);
				return (0);
				
			case  0:                              //断开连接
				close(socketfd);
				return(0);
			  
			default:
			//	printf ("OK: Receviced numbytes = %d\n", num);
				break;
		}
	}
	return revnum;	
}

/********************************************************************************* 
 函数名称： SendData
 功能描述： 发送数据
 输    入：无
 输	   出： 无
 返 回 值：发送的字节数
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/


ST_INT32 SendData(ST_INT32 socketfd,ST_UCHAR *sendbuf, ST_INT32 len)
{
	ST_INT32 sendnum;
	sendnum = send(socketfd, sendbuf, len, 0);
 
   return sendnum;            
}




