#include <stdlib.h>
#include <stdio.h>
#include "DataType.h"
#include "mytcpip.h"


#define TCPIP_LOCAL_PORT                      1035
#define TRY_CONNECT_TIME                      2           /* 重试连接，s单位 */
#define WAIT_THREAD_EXIT_TIME                 20          /* 等待线程退出时间 */
#define CDZ_FRAME_START                       0x68        /* 帧起始符 */
#define CDZ_FRAME_END                         0x16        /* 帧结束符 */
#define DIR_M_T_C                             0           /* 主站发出的命令 */
#define DIR_C_T_M                             1           /* 由终端发出的应答帧 */
#define UPLOAD_MSTA                           0           /* 终端上报地址 */
#define FRAME_OK                              0           /* 确认帧 */
#define FRAME_ERR                              1          /* 否定帧 */
 
#define READ_CURRENT_DATA                     0x01       /* 读当前数据 */                     
#define READ_TASK_DATA                        0x02       /* 读任务数据 */                     
#define READ_CHARGE_RECORD                    0x03       /* 读充电记录 */                     
#define READ_PROGRAM_LOG                      0x04       /* 读编程日志 */                     
#define RT_WRITE_OBJECT_PARAM                 0x07       /* 实时写对象 */                     
#define WRITE_OBJECT_PARAM                    0x08       /* 写对象 */                         
#define READ_ALARM                            0x09       /* 读告警记录 */                     
#define ALARM_CONFIRM                         0x0A       /* 告警确认 */                       
#define CHARGEREC_CONFIRM                     0x0B       /* 主站对终端上送充电记录的确认 */   
#define GET_ESAM_RAND                         0x30       /* 取ESAM模块随机数 */               
#define WRITE_KEY_PARAM                       0x31       /* 以密钥加密的方式下发写参数命令 */ 
#define RT_WRITE_KEY_PARAM                    0x33       /* 以密钥加密的方式实时写参数命令 */ 
#define KEEP_ALIVE                            0x24       /* 心跳报文帧 */                     
#define KEEP_ALIVE_RES                        0xA4       /* 心跳报文帧响应帧 */               
#define KEEP_FRAME_LAND                       0xA1       /*  报文登陆帧*/                     
#define LAND_CONFIRM                          0x21       /*  登陆应答帧*/                     
#define ZJ_COMMAND_NORETURN                   0x01       /* 中继命令没有返回 */ 
#define SET_PARAM_INVALID                     0x02       /* 设置内容非法 */     
#define PASSWORD_AUTH_INVALID                 0x03       /* 密码权限不足 */     
#define DATA_NOT_EXIST                        0x04       /* 无此项数据 */       
#define MB_ADDR_NOT_EXIST                     0x05       /* 目标地址不存在 */   
#define DATA_SEND_FAIL                        0x06       /* 发送失败 */         
#define SHORT_MSG_TOO_LONG                    0x07       /* 短消息帧太长 */   


#define READRECORD_MAXLEN 5  //后台一次最多读取十条记录

#define LENGTH 1000
typedef struct
{
	ST_UCHAR remoteIP[16];		
	ST_INT32 remotePort;			
	ST_UINT32 keepAliveVal;			
	ST_SOCKET sSocket;				
	ST_INT32 iProtoType;			
	ST_UCHAR TermAdd[4];			
    	ST_UCHAR  Sendbuf[LENGTH];               
	ST_UCHAR Rcvbuf[LENGTH];                
}CDZ_PROTOCOL_PARAM;


ST_RET cdzProtocolInit(void);

ST_RET cdzProtocolExit(void);

 ST_RET cdzProtocolCheckDataBuf(const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen);
  ST_UINT16 cdzProtocolSendErrFrame(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, ST_UCHAR *sSendResbuf,char ErrCode);
 ST_UINT16 cdzProtocosReadCurDataRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosReadTaskDataRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);		
 ST_UINT16 cdzProtocosReadChargeRecordRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosReadProgramLogRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosRtWriteParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosWriteParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosReadAlarmRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosGetEsamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosWriteKeyParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosRtWriteKeyParamRes(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosAlarmConfirmRes(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
ST_INT16 cdzProtocosChargeRecConfirmRes(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
 ST_UINT16 cdzProtocosKeepALiveRes(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);		
void cdzProtocosConfirmLand(ST_UCHAR sLastFSeq, ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, const ST_UCHAR *sDatabuf, ST_UINT16 sDataLen,
									ST_UCHAR *sSendResbuf);
ST_UCHAR cdzCalcCS(const ST_UCHAR *sDataBuf, ST_UINT16 sDataLen);		
 ST_UINT16 cdzProtocolSendKeepAliveFrame(ST_UCHAR sFSeq, ST_UCHAR sFuncCode, ST_UCHAR sMstaAddr,
									const ST_UCHAR *sRtuAddr, ST_UCHAR *sSendResbuf);
 ST_UCHAR cdcCalcFSeq(ST_UCHAR sSFeq);																
