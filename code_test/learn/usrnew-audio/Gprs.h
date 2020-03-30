#include "DataType.h"
//#include "cdzProtocol.h"
//#define LENGTH 1024
typedef struct
{
	ST_UCHAR remoteIP[16];		
	ST_INT32 remotePort;			
	ST_UINT32 keepAliveVal;			
	ST_INT32 Gprsfd;				
	ST_INT32 iProtoType;			
	ST_UCHAR TermAdd[4];			
  	ST_UCHAR  Sendbuf[1000];               
	ST_CHAR Rcvbuf[1000];                
}CDZ_GPRS_PROTOCOL_PARAM;

extern ST_BOOL ProtolLandFlag_Gprs;
ST_RET cdzGprsInit(void);
