#ifndef _MYTCPIP_H_
#define _MYTCPIP_H_

#include "DataType.h"

#define PROTOCOLTP_TCP 0
#define PROTOCOLTP_UDP 1
#define REMOTE_PORT    5000



typedef struct __CDZ_SOCKET_INFO__
{
	ST_INT32 Socketfd;
	ST_INT32 ProcoType;
	ST_ULONG RemoteIP;
	ST_INT32 Port;

}CDZ_SOCKET_INFO;
                              

void InitTCPIP(void);

ST_INT32 SendData(ST_INT32 socketfd,ST_UCHAR *sendbuf, ST_INT32 len);
ST_INT32 ReciveData(ST_INT32 socketfd, ST_UCHAR *revbuf, ST_INT32 len);
ST_INT32 CreateSocket(const ST_INT32 procotoltype);
ST_INT32 ConnectServer(const ST_UCHAR *remoteip);
void CloseSocket(void);


#endif
