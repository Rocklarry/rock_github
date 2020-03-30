/********************************************************************************
 
文 件 名：	cdzProtocolData.h
版    本：	1.0
概    述：	深圳充电桩主站的数据项定义及操作

日    期：	2010.02.6
修改记录：
*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include "cdzDataType.h"


#define CFG_TERM_PARA_FILE_PATH			"TermParam.cfg"
#define CFG_MEA_PARA_FILE_PATH			"MeaPointParam.cfg"

/* 终端参数定义 */
static CDZ_TERM_PARA s_tcdzTermPara;

/* 测量点参数定义 */
static CDZ_MEA_POINT_PARA s_tcdzMeaPointPara[CDZ_MEA_POINT_NUM];

/* 测量点数据定义 */
static CDZ_MEA_POINT_DATA s_tcdzMeaPointData[CDZ_MEA_POINT_NUM];

/* 出厂参数定义--终端 */
static const CDZ_TERM_PARA s_defaultTermPara = 
{
	/* 主站通讯地址 */
	{0x04, 0xAA, 0xAA, 0xC0, 0xA8, 0x10, 0x71, 0x13, 0x88},
	/* 备用通讯地址1 */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	/* 备用通讯地址2 */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	/* 网关地址 */
	{0xAA, 0xAA, 0xAA, 0xAA, 0x0A, 0x62, 0x65, 0x07},	
	/* APN */			
	{'C', 'M', 'N', 'E', 'T'},	
	/* 终端地市区县码 */										
	{0x07, 0x55},														
	/* 终端地址 */
	{0x10, 0x00},														
	/* 心跳时间 */
	0x30,																
	/* GPRS登陆APN用户名 */
	{'C', 'M', 'N', 'E', 'T',},											
	
	/* GPRS 登陆APN 密码 */
	{'C', 'M', 'N', 'E', 'T',},											
	/* TCP/UDP 标识（缺省为TCP） */
	0x00,																
	/* 普通密码 */
	{0x00, 0x00, 0x00},													
	/* 设置密码（低级权限） */
	{0x00, 0x00, 0x00},													
	/* 管理员密码 */
	{0x11, 0x11, 0x11},													
	/* 终端复位 */
	0x00,
	/* 终端时间 */
	{0x11, 0x02, 0x06, 0x00, 0x00, 0x00},								
	/* 对时规则 */
	{0x10, 0x24, 0x23, 0x43},								
	/* 告警屏蔽字 */
	{0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	/* 判断电流过负荷、过压、欠压的时间 */
	0x01,
	/* 判断电流过负荷、过压、欠压恢复的时间 */
	0x01,
	/* D0～D3 位对应遥信输入，D4 位为门开关接点、D5 为预留接点。0 为常开，1 为常闭。(缺省值: FF 为常闭)*/
	0xFF,
	/* NN：启动电压，缺省为额定电压的78％LL：恢复电压，缺省为额定电压85％MM：启动电流，缺省为额定电流10％，缺相（即失压，下同）或断相报警的电流判断阀值。低于该阀值是断相，高于该阀值是缺相 */
	{0x78, 0x85, 0x10},
	/* 判断断相、缺相时间，缺省值1 分钟 */
	0x01,
	/* 判断断相、缺相恢复时间，缺省值1 分钟 */
	0x01,
	/* 现有任务总数 */
	0x00,
	/* 任务号 */
	{0x00,},
	/* 任务状态 */
	{0x00,},
	/* 终端软件版本号 */
	{0x11, 0x02, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00},
	/* 终端硬件版本号 */
	{0x02, 0x06},
	/* 级联地址 */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	/* 交易类型 */
	0x00,
	/* 地区代码 */
	{0x00, 0x00, 0x00},
	/* 停车费小时单价 */
	{0x00, 0x00, 0x00},
	/* 是否扣除停车费 */
	0x00,
	/*主终端等待从终端应答的超时判定时间，缺省值5 秒*/
	0x00,
	/*从终端等待主终端级联控制命令的超时判定时间，缺省值5 秒*/
	0x00,
	/*终端号*/
	{0x00, 0x00, 0x00, 0x00, 0x00},
	/* 充电管理卡 */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	/* 故障检修卡 */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	/* 终端是否启用 */
	0x00,
	/* 提示余额低金额 */
	{0x00, 0x15, 0x00},
	/* 蓄电池组标识 */
	{0x00,},
	{0x00,},
	{0x00,},
	/* 当前交易费率 */
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x00},
	/* 备用交易费率 */
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF},
	/* 备用费率电量切换时间 */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x00,0x00,0x00},
};






/* 出厂参数定义--测量点 */
static const CDZ_MEA_POINT_PARA s_tdefaultcdzMeaPointPara[CDZ_MEA_POINT_NUM] = 
{
	/* 测量点零参数*/
	{
		/*测量点的状态 0：无效；1：有效 */
		0x01,
		/* 测量点性质： 01:485表，02:模拟量，03:脉冲量，04:计算值 05 交流采样 */
		0x02,
		/* 测量点地址（仅对于485量）(缺省值: 999999999999)    */
		{0x99, 0x99, 0x99, 0x99, 0x99, 0x99},
		/* 测量点通讯规约（仅对于485 量） 10:DL/T 645-1997 规约；11：DL/T 645-2007规约；12：广东电网公司三相多功能载波电能表485接口通讯规约；13：广电单相电子式电能表通讯规约；(缺省值: 11：DL/T 645-2007规约) */		
		0x11,
		/* 测量点端口号（仅对于485量） */			
		0x00,
		/*测量点的波特率，NN：HEX码，端口通信波特率/300，缺省为4即1200bps*/

		0x04,
		/* 终端额定供电电压。(缺省值: 220.0) */

		{0x00, 0x22},
		/* 额定电流。(测量点为0时对应终端，缺省值：单相16.00，三相32.00)。（测量点为1时对应电表，缺省值: 5.00) */

		{0x00, 0x16},
		/* 判断电流过负荷的相对额定值的比例（缺省为1.25）(电流过负荷后停止充电) */	
		{0x25, 0x01},
		/* 判断电流过负荷恢复的相对额定值的比例（缺省为1.00） */

		{0x00, 0x01},
		/* 奇偶校验位0：无校验，1：偶校验，2：奇校验 */	

		0x01,
		/* 数据位 */

		0x08,
		/* 停止位 */

		0x00,
		/* 电能表通讯密码 */

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		/* 电能表通讯用户名 */

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		/* 判断过压相对额定电压的比例 */

		{0x15, 0x01},
		/* 判断过压恢复相对额定电压的比例 */

		{0x05, 0x01},
		/* 判断欠压相对额定电压的比例 */

		{0x80, 0x00},
		/* 判断欠压恢复相对额定电压的比例 */
		{0x95, 0x00},
		/* 充电最小许可电流与电表额定电流比值 */

		{0x05, 0x00},
		/* 终端最大输出电流 */

		{0x00, 0x16}
	},


	{

		0x01,

		0x01,

		{0x99, 0x99, 0x99, 0x99, 0x99, 0x99},

		0x11,

		0x00,

		0x04,

		{0x00, 0x22},

		{0x00, 0x05},

		{0x25, 0x01},

		{0x00, 0x01},

		0x01,

		0x08,

		0x00,

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		{0x15, 0x01},

		{0x05, 0x01},

		{0x80, 0x00},

		{0x95, 0x00},

		{0x05, 0x00},

		{0x00, 0x00}
	},

	{

		0x01,

		0x01,

		{0x99, 0x99, 0x99, 0x99, 0x99, 0x99},

		0x11,

		0x00,

		0x04,

		{0x00, 0x22},

		{0x00, 0x05},

		{0x25, 0x01},
		{0x00, 0x01},

		0x01,

		0x08,

		0x00,

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		{0x15, 0x01},

		{0x05, 0x01},

		{0x80, 0x00},

		{0x95, 0x00},

		{0x05, 0x00},

		{0x00, 0x00}
	},

	{

		0x01,

		0x02,

		{0x99, 0x99, 0x99, 0x99, 0x99, 0x99},

		0x11,

		0x00,

		0x04,

		{0x00, 0x22},

		{0x00, 0x05},

		{0x25, 0x01},

		{0x00, 0x01},

		0x01,

		0x08,

		0x00,

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		{0x15, 0x01},

		{0x05, 0x01},

		{0x80, 0x00},

		{0x95, 0x00},

		{0x05, 0x00},

		{0x00, 0x00}
	},
};

/* 创建数据项索引表1--终端参数 */
static const CDZ_DATA_ITEM s_cdzTermParamTable[] = 
{
	{0x8010,  s_tcdzTermPara.StationCommAdd,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									9,	0},		
	{0x8011,  s_tcdzTermPara.ResStationCommAdd1,				CDZ_OPT_RW,		CDZ_AUTH_HIGH,									9,  0},		
	{0x8012,  s_tcdzTermPara.ResStationCommAdd2,				CDZ_OPT_RW,		CDZ_AUTH_HIGH,									9,	0},		
	{0x8014,  s_tcdzTermPara.DefaultGateWayAdd,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									8,	0},		
	{0x8015,  s_tcdzTermPara.APN,								CDZ_OPT_RW,		CDZ_AUTH_HIGH,									16,	0},		
	{0x8016,  s_tcdzTermPara.TermDistCode,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									2,	0},		
	{0x8017,  s_tcdzTermPara.TermAdd,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									2,	0},		
	{0x8018,  &s_tcdzTermPara.HeartBeatInter,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x8019,  s_tcdzTermPara.EnterUserName,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									32,	0},		
	{0x801A,  s_tcdzTermPara.EnterPassWord,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									32,	0},		
	{0x801C,  &s_tcdzTermPara.TCPUDPIdentCode,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x801F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH,									0,	11},	


	{0x8020,  s_tcdzTermPara.CommPass,							CDZ_OPT_WO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0x8021,  s_tcdzTermPara.SetPass,							CDZ_OPT_WO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0x8022,  s_tcdzTermPara.AdminPass,							CDZ_OPT_WO,		CDZ_AUTH_HIGH,									3,	0},		
	{0x8023,  &s_tcdzTermPara.TermReset,						CDZ_OPT_WO,		CDZ_AUTH_HIGH,									1,	0},		
	{0x802F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH,									0,	4},		

	{0x8030,  s_tcdzTermPara.TermTime,							CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	6,	0},		
	{0x8031,  s_tcdzTermPara.TimeRule,							CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					4,	0},		
	{0x8033,  s_tcdzTermPara.AlarmMask,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									16,	0},		
	{0x8034,  &s_tcdzTermPara.IOverTime,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,  0},		
	{0x8035,  &s_tcdzTermPara.IOverReturnTime,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x8037,  &s_tcdzTermPara.DiConfig,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x8038,  s_tcdzTermPara.StartParam,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									3,	0},		

	{0x8065,  &s_tcdzTermPara.PhaseFaultTime,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x8066,  &s_tcdzTermPara.PhaseFaultReturnTime,				CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x806F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH,									0,	2},		

	{0x8100,  &s_tcdzTermPara.CurrentTaskNum,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,  0},		
	{0x8101,  s_tcdzTermPara.TaskNo,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									254,0},     
	{0x81FE,  s_tcdzTermPara.TaskStatus,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									32, 0},		

	{0x8809,  s_tcdzTermPara.TermSoftVer,						CDZ_OPT_RO,		CDZ_AUTH_NO,									8,	0},		
	{0x880A,  s_tcdzTermPara.TermHardWareVer,					CDZ_OPT_RO,		CDZ_AUTH_NO,									2,	0},		

	{0x881A,  s_tcdzTermPara.SlaveTermAddr1,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									16,	0},		
	{0x881B,  s_tcdzTermPara.SlaveTermAddr2,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									16,	0},		
	{0x881C,  s_tcdzTermPara.SlaveTermAddr3,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									16,	0},		
	{0x881D,  s_tcdzTermPara.SlaveTermAddr4,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									16,	0},		

	{0x8820,  &s_tcdzTermPara.ExchangeType,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,	0},		
	{0x8821,  s_tcdzTermPara.CityCode,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									3,	0},		
	{0x8822,  s_tcdzTermPara.ParkFee,							CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0x8823,  &s_tcdzTermPara.KcParkFeeFlag,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					1,	0},	
	{0x8824,  &s_tcdzTermPara.HostWaitTime,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,  0},		
	{0x8825,  &s_tcdzTermPara.SlaveWaitTime,					CDZ_OPT_RW,		CDZ_AUTH_HIGH,									1,  0},		
	{0x882C,  s_tcdzTermPara.SlaveNo,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									5,  0},		
	{0x882F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH,									0,  7},		

	{0x8830,  s_tcdzTermPara.ChargeManCardNo1,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8831,  s_tcdzTermPara.ChargeManCardNo2,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8832,  s_tcdzTermPara.ChargeManCardNo3,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8833,  s_tcdzTermPara.ChargeManCardNo4,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8834,  s_tcdzTermPara.ChargeManCardNo5,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8835,  s_tcdzTermPara.ChargeManCardNo6,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8836,  s_tcdzTermPara.ChargeManCardNo7,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8837,  s_tcdzTermPara.ChargeManCardNo8,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8838,  s_tcdzTermPara.ChargeManCardNo9,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x8839,  s_tcdzTermPara.ChargeManCardNo10,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,  0},		
	{0x883F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  10},	

	{0x8880,  s_tcdzTermPara.FaultRepairCard1,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8881,  s_tcdzTermPara.FaultRepairCard2,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8882,  s_tcdzTermPara.FaultRepairCard3,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8883,  s_tcdzTermPara.FaultRepairCard4,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8884,  s_tcdzTermPara.FaultRepairCard5,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8885,  s_tcdzTermPara.FaultRepairCard6,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8886,  s_tcdzTermPara.FaultRepairCard7,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8887,  s_tcdzTermPara.FaultRepairCard8,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8888,  s_tcdzTermPara.FaultRepairCard9,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x8889,  s_tcdzTermPara.FaultRepairCard10,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					8,	0},		
	{0x888F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	10},	

	{0x8840,  &s_tcdzTermPara.TermUseFlag,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					1,	0},		
	{0x8841,  s_tcdzTermPara.LowMoneyAnn,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0x884F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  2},		

	{0x8850,  s_tcdzTermPara.CellFlag,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									32,	0},		
	{0x8851,  s_tcdzTermPara.CellType,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									32,	0},		
	{0x8852,  s_tcdzTermPara.CellParam,							CDZ_OPT_RW,		CDZ_AUTH_HIGH,									32,	0},		
	{0x885F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	3},		

	{0xC421,  s_tcdzTermPara.CurChargeFee1,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC422,  s_tcdzTermPara.CurChargeFee2,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC423,  s_tcdzTermPara.CurChargeFee3,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC424,  s_tcdzTermPara.CurChargeFee4,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC425,  s_tcdzTermPara.CurChargeFee5,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC426,  s_tcdzTermPara.CurChargeFee6,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC427,  s_tcdzTermPara.CurChargeFee7,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC428,  s_tcdzTermPara.CurChargeFee8,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC42F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	8},		

	{0xC431,  s_tcdzTermPara.ResChargeFee1,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC432,  s_tcdzTermPara.ResChargeFee2,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC433,  s_tcdzTermPara.ResChargeFee3,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC434,  s_tcdzTermPara.ResChargeFee4,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC435,  s_tcdzTermPara.ResChargeFee5,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC436,  s_tcdzTermPara.ResChargeFee6,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC437,  s_tcdzTermPara.ResChargeFee7,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC438,  s_tcdzTermPara.ResChargeFee8,						CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
	{0xC43F,  NULL,												CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	8},		

	{0xC442,  s_tcdzTermPara.ResChargeFeeSwitchTime,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					5,	0},	
	{0xC443,  s_tcdzTermPara.GasPrice,			                CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					3,	0},		
};

static const ST_UINT16 s_cdzTermParamTableNum = sizeof(s_cdzTermParamTable) / sizeof(CDZ_DATA_ITEM);

/* 创建数据项索引表2--测量点0（终端）参数 */
static const CDZ_DATA_ITEM s_MeaPointParamTermTable[] = 
{
    {0x8900,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].MeaPointStatus,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8901,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].MeaPointAttrib,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8902,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].MeaPointAddr,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	6,	0},		
	{0x8903,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].MeaPointProtocol,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8904,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].MeaPointNo,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8905,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].MeaPointBaud,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
    {0x890F,  NULL,															CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	6},		

	{0x8913,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].TermVoltage,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8914,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateCurrent,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x891F,  NULL,															CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8925,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateCurrentGFH,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8926,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateCurrentGFHRetrun,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x892F,  NULL,															CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8930,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].Parity,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8931,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].Databit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8932,  &s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].Stopbit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8933,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].Password,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
	{0x8934,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].User,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
    {0x893F,  NULL,															CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,	5},		

	{0x8940,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateGY,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8941,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateGYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8942,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateQY,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8943,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateQYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x894F,  NULL,															CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  4},		

	{0x8963,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].RateChargeCurrentMix,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8964,  s_tcdzMeaPointPara[CDZ_TERM_MEA_POINT].OutputMaxCurrent,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x896F,  NULL,															CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  2},		
};

static const ST_UINT16 s_MeaPointParamTermTableNum = sizeof(s_MeaPointParamTermTable) / sizeof(CDZ_DATA_ITEM);

/* 创建数据项索引表3--测量点1（交流电能表）参数 */
static const CDZ_DATA_ITEM s_MeaPointParamAcMeterTable[] = 
{
    {0x8900,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointStatus,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8901,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointAttrib,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8902,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointAddr,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	6,	0},		
	{0x8903,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointProtocol,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8904,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointNo,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8905,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointBaud,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
    {0x890F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	6},		

	{0x8913,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].TermVoltage,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8914,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateCurrent,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x891F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8925,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateCurrentGFH,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8926,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateCurrentGFHRetrun,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x892F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8930,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].Parity,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8931,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].Databit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8932,  &s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].Stopbit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8933,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].Password,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
	{0x8934,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].User,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
    {0x893F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,	5},		

	{0x8940,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateGY,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8941,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateGYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8942,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateQY,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8943,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateQYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x894F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  4},		

	{0x8963,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].RateChargeCurrentMix,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8964,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].OutputMaxCurrent,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x896F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  2},		
};

static const ST_UINT16 s_MeaPointParamAcMeterTableNum = sizeof(s_MeaPointParamAcMeterTable) / sizeof(CDZ_DATA_ITEM);



/* 创建数据项索引表4--测量点2（直流电能表）参数 */
static const CDZ_DATA_ITEM s_MeaPointParamDcMeterTable[] = 
{
    {0x8900,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].MeaPointStatus,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8901,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].MeaPointAttrib,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8902,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].MeaPointAddr,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	6,	0},		
	{0x8903,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].MeaPointProtocol,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8904,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].MeaPointNo,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8905,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].MeaPointBaud,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
    {0x890F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	6},		

	{0x8913,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].TermVoltage,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8914,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateCurrent,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x891F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8925,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateCurrentGFH,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8926,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateCurrentGFHRetrun,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x892F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8930,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].Parity,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8931,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].Databit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8932,  &s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].Stopbit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8933,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].Password,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
	{0x8934,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].User,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
    {0x893F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,	5},		

	{0x8940,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateGY,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8941,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateGYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8942,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateQY,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8943,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateQYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x894F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  4},		

	{0x8963,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].RateChargeCurrentMix,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8964,  s_tcdzMeaPointPara[CDZ_DC_METER_MEA_POINT].OutputMaxCurrent,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x896F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  2},		
};

static const ST_UINT16 s_MeaPointParamDcMeterTableNum = sizeof(s_MeaPointParamDcMeterTable) / sizeof(CDZ_DATA_ITEM);

/* 创建数据项索引表5--测量点3（充电机模块）参数 */
static const CDZ_DATA_ITEM s_MeaPointParamChargeTable[] = 
{
    {0x8900,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].MeaPointStatus,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8901,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].MeaPointAttrib,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8902,  s_tcdzMeaPointPara[CDZ_AC_METER_MEA_POINT].MeaPointAddr,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	6,	0},		
	{0x8903,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].MeaPointProtocol,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8904,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].MeaPointNo,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
	{0x8905,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].MeaPointBaud,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,	0},		
    {0x890F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,	6},		

	{0x8913,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].TermVoltage,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8914,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateCurrent,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x891F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8925,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateCurrentGFH,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
	{0x8926,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateCurrentGFHRetrun,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	2,	0},		
    {0x892F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,  2},		

	{0x8930,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].Parity,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8931,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].Databit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8932,  &s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].Stopbit,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	1,  0},		
	{0x8933,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].Password,				CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
	{0x8934,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].User,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	8,	0},		
    {0x893F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW|CDZ_AUTH_HIGHEST,	0,	5},		

	{0x8940,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateGY,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8941,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateGYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8942,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateQY,					CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
	{0x8943,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateQYReturn,			CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x894F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  4},		

	{0x8963,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].RateChargeCurrentMix,	CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,	0},		
	{0x8964,  s_tcdzMeaPointPara[CDZ_CHARGE_MEA_POINT].OutputMaxCurrent,		CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					2,  0},		
    {0x896F,  NULL,																CDZ_OPT_RW,		CDZ_AUTH_HIGH|CDZ_AUTH_HIGHEST,					0,  2},		
};

static const ST_UINT16 s_MeaPointParamChargeTableNum = sizeof(s_MeaPointParamChargeTable) / sizeof(CDZ_DATA_ITEM);

/* 创建数据项索引表6--测量点0（终端）数据 */
static const CDZ_DATA_ITEM s_MeaPointDataTermTable[] = 
{
	{0xC010,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].Date,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									4,	0},		
	{0xC011,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].Time,						CDZ_OPT_RW,		CDZ_AUTH_HIGH,									3,	0},		

	{0xC601,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].OutPutVoltage,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xC602,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].OutPutCurrent,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC603,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].TermFaultStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC605,  &s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].ACSwitchStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0},		
	{0xC606,  &s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].DCSwitchStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0},		
    {0xC60F,  NULL,																CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						0,	5},		

	{0xC830,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExchangeSeq,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						5,	0},		
	{0xC831,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurStartCardNo,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						8,	0},		
	{0xC832,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExchangeEnerge,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC833,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExchangeEnergeFee1,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC834,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExchangeEnergeFee2,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC835,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExchangeEnergeFee3,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC836,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExchangeEnergeFee4,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC837,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExChangeStartTime,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						6,	0},		
	{0xC838,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurExChangeEndTime,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						6,	0},		
	{0xC839,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurParkFee,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC83A,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurBeforeExChangeMoney,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC83B,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurAfterExChangeMoney,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC83C,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurCardVerNo,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						5,	0},		
	{0xC83D,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurPosNo,					CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						5,	0},		
	{0xC83E,  &s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurCardStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0},		
	{0xC83F,  NULL,																CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						0,	15},	

	{0xC840,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurChargeStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xC841,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].CurStartType,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		

	{0xC850,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExchangeSeq,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						5,	0},		
	{0xC851,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastStartCardNo,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						8,	0},		
	{0xC852,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExchangeEnerge,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC853,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExchangeEnergeFee1,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC854,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExchangeEnergeFee2,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC855,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExchangeEnergeFee3,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC856,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExchangeEnergeFee4,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC857,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExChangeStartTime,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						6,	0},		
	{0xC858,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastExChangeEndTime,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						6,	0},		
	{0xC859,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastParkFee,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC85A,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastBeforeExChangeMoney,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC85B,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastAfterExChangeMoney,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC85C,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastCardVerNo,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						5,	0},		
	{0xC85D,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastPosNo,					CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						5,	0},		
	{0xC85E,  &s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastCardStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0},		
	{0xC85F,  NULL,																CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						0,	15},	

	{0xC860,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastEndCardNo,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						8,	0},		
	{0xC861,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastStartCardType,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xC862,  s_tcdzMeaPointData[CDZ_TERM_MEA_POINT].LastEndCardType,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
};

static const ST_UINT16 s_MeaPointDataTermTableNum = sizeof(s_MeaPointDataTermTable) / sizeof(CDZ_DATA_ITEM);


/* 创建数据项索引表6--测量点1（交流电能表）数据 */
static const CDZ_DATA_ITEM s_MeaPointDataAcMeterTable[] = 
{
	{0x9010,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].TotalP,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0x9011,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].TotalP1,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0x9012,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].TotalP2,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0x9013,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].TotalP3,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0x9014,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].TotalP4,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
    {0x901F,  NULL,																CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						0,	5},		

	{0xB611,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].VoltageA,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xB612,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].VoltageB,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xB613,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].VoltageC,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
    {0xB61F,  NULL,																CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						0,	3},		

	{0xB621,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].CurrentA,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xB622,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].CurrentB,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
	{0xB623,  s_tcdzMeaPointData[CDZ_AC_METER_MEA_POINT].CurrentC,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						2,	0},		
    {0xB62F,  NULL,																CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						0,  3},		
};

static const ST_UINT16 s_MeaPointDataAcMeterTableNum = sizeof(s_MeaPointDataAcMeterTable) / sizeof(CDZ_DATA_ITEM);



/* 创建数据项索引表6--测量点3（充电监控模块）数据 */
static const CDZ_DATA_ITEM s_MeaPointDataChargeTable[] = 
{
	{0xC880,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupVoltage,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC881,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupChargeCurrent,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC882,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupChargePower,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC883,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupChargeTime,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC884,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupChargeEnerge,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC885,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellVoltage,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC886,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellAh,					CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						4,	0},		
	{0xC887,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupTemp,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC888,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeOutputVoltage,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC889,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeOutputCurrent,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		
	{0xC88A,  s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeTemp,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						3,	0},		

	{0xC890,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupStatus,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC891,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].CellGroupFaultCode,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC892,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeStatus,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC893,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeFaultCode,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC894,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeACSwitchStatus,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC895,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeDCSwitchStatus,	CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC896,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeDCSwitchTrip,		CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC897,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].WatchFault,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC898,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].WatchCommFault,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC899,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeOnOff,			CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
	{0xC89A,  &s_tcdzMeaPointData[CDZ_CHARGE_MEA_POINT].ChargeStop,				CDZ_OPT_RO,		CDZ_AUTH_HIGH|CDZ_AUTH_LOW,						1,	0  },	
};

static const ST_UINT16 s_MeaPointDataChargeTableNum = sizeof(s_MeaPointDataChargeTable) / sizeof(CDZ_DATA_ITEM);

/********************************************************************************* 
 函数名称： cdzSearchDataItemMeaPoint
 功能描述：	根据测量点查找数据项
 输    入：	cdzMeaPointFlag:测量点标志，8字节
			cdzIdentCode:标识编码
 输	   出： pMeaPontIndex:测量点号
 返 回 值： NULL:此数据项不存在,非NULL:数据项指针
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
static const CDZ_DATA_ITEM *cdzSearchDataItemMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode)
{
	ST_UINT16 k = 0;

	if (sMeaPointNo > CDZ_MEA_POINT_NUM) return NULL;

	switch(sMeaPointNo)
	{
	case CDZ_TERM_MEA_POINT:					
		{

			for (k=0; k<s_cdzTermParamTableNum; k++)
			{
				if (cdzIdentCode == s_cdzTermParamTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_cdzTermParamTable[k];
				}
			}


			for (k=0; k<s_MeaPointParamTermTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointParamTermTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointParamTermTable[k];
				}
			}


			for (k=0; k<s_MeaPointDataTermTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointDataTermTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointDataTermTable[k];
				}
			}
			
			break;
		}
	case CDZ_AC_METER_MEA_POINT:					
		{

			for (k=0; k<s_MeaPointParamAcMeterTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointParamAcMeterTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointParamAcMeterTable[k];
				}
			}

			for (k=0; k<s_MeaPointDataAcMeterTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointDataAcMeterTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointDataAcMeterTable[k];
				}
			}


			break;
		}
	case CDZ_DC_METER_MEA_POINT:			
		{

			for (k=0; k<s_MeaPointParamDcMeterTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointParamDcMeterTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointParamDcMeterTable[k];
				}
			}



			break;
		}
	case CDZ_CHARGE_MEA_POINT:
		{

			for (k=0; k<s_MeaPointParamChargeTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointParamAcMeterTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointParamAcMeterTable[k];
				}
			}


			for (k=0; k<s_MeaPointDataChargeTableNum; k++)
			{
				if (cdzIdentCode == s_MeaPointDataChargeTable[k].IdentCode)						
				{
					return (CDZ_DATA_ITEM *)&s_MeaPointDataChargeTable[k];
				}
			}

			break;
		}
	default:
		break;
	}

	return NULL;
}

/********************************************************************************* 
 函数名称： cdzSearchDataItem
 功能描述：	查找数据项
 输    入：	cdzMeaPointFlag:测量点标志，8字节
			cdzIdentCode:标识编码
 输	   出： pMeaPontIndex:测量点号
 返 回 值： NULL:此数据项不存在,非NULL:数据项指针
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

static const CDZ_DATA_ITEM *cdzSearchDataItem(const ST_UCHAR *cdzMeaPointFlag, ST_UINT16 cdzIdentCode, ST_UINT16 *pMeaPontIndex)
{
	ST_UINT16 i = 0, j = 0, k = 0;
	ST_UCHAR sMeaPointFlag = 0;
	ST_UCHAR sMeaPointNo = 0;

	if (cdzMeaPointFlag == NULL || pMeaPontIndex == NULL) return NULL;

	for (i=0; i<8; i++)
	{
		sMeaPointFlag = cdzMeaPointFlag[i];

		for (j=0; j<8; j++)
		{
			if ((sMeaPointFlag & (1<<j)) != 0)
			{
				sMeaPointNo = (i * 8 + j);
				if (sMeaPointNo < CDZ_MEA_POINT_NUM)
				{
					switch(sMeaPointNo)
					{
					case CDZ_TERM_MEA_POINT:					
						{
					
							for (k=0; k<s_cdzTermParamTableNum; k++)
							{
								if (cdzIdentCode == s_cdzTermParamTable[k].IdentCode)						
								{

									*pMeaPontIndex = CDZ_TERM_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_cdzTermParamTable[k];
								}
							}

					
							for (k=0; k<s_MeaPointParamTermTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointParamTermTable[k].IdentCode)						
								{

									*pMeaPontIndex = CDZ_TERM_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointParamTermTable[k];
								}
							}

					
							for (k=0; k<s_MeaPointDataTermTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointDataTermTable[k].IdentCode)						
								{
									*pMeaPontIndex = CDZ_TERM_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointDataTermTable[k];
								}
							}

							break;
						}
					case CDZ_AC_METER_MEA_POINT:				
						{

							for (k=0; k<s_MeaPointParamAcMeterTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointParamAcMeterTable[k].IdentCode)						
								{
									*pMeaPontIndex = CDZ_AC_METER_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointParamAcMeterTable[k];
								}
							}


							for (k=0; k<s_MeaPointDataAcMeterTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointDataAcMeterTable[k].IdentCode)						
								{
									*pMeaPontIndex = CDZ_AC_METER_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointDataAcMeterTable[k];
								}
							}


							break;
						}
					case CDZ_DC_METER_MEA_POINT:				
						{

							for (k=0; k<s_MeaPointParamDcMeterTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointParamDcMeterTable[k].IdentCode)						
								{
									*pMeaPontIndex = CDZ_DC_METER_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointParamDcMeterTable[k];
								}
							}


							break;
						}
					case CDZ_CHARGE_MEA_POINT:
						{
							for (k=0; k<s_MeaPointParamChargeTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointParamAcMeterTable[k].IdentCode)						
								{
									*pMeaPontIndex = CDZ_CHARGE_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointParamAcMeterTable[k];
								}
							}

							for (k=0; k<s_MeaPointDataChargeTableNum; k++)
							{
								if (cdzIdentCode == s_MeaPointDataChargeTable[k].IdentCode)						
								{
									*pMeaPontIndex = CDZ_CHARGE_MEA_POINT;
									return (CDZ_DATA_ITEM *)&s_MeaPointDataChargeTable[k];
								}
							}

							break;
						}
					default:
						break;
					}
				}
				else
				{
					return NULL;
				}
			}
		}
	}
	return NULL;
}

/********************************************************************************* 
 函数名称： cdzDataItemCheck
 功能描述：	检查待写参数的有效性
 输    入：	cdzIdentCode:标识编码
			sDataItemContent:数据内容
 输	   出： 无
 返 回 值： NULL:此数据项不存在,非NULL:数据项指针
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

static ST_RET cdzDataItemCheck(ST_UINT16 cdzIdentCode, const ST_UCHAR *sDataItemContent)
{
	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： GetFileSize
 功能描述：	获取文件大小函数
 输    入：	sFileName:文件名
	
 输	   出： 无
 返 回 值： 文件大小
 修改记录：2011 02 06
*********************************************************************************/ 

ST_INT32 GetFileSize(const   char   *sFileName) 
{ 
        struct   stat   buf; 
        if(stat(sFileName,   &buf)   !=   0   ) 
        {               
        	return(-1); 
        } 
        return(buf.st_size); 
} 

/********************************************************************************* 
 函数名称： cdzLoadTermParam
 功能描述：	从文件中加载终端参数文件，如果文件不存在，则加载缺省的配置文件
			并保存到文件中
 输    入：	无
 输	   出： SD_SUCCESS:成功
			SD_FAILURE:失败
 返 回 值： 无
 作    者：	yansudan
 日    期：	2010.10.25
 修改记录：2011 02 06
*********************************************************************************/

static ST_RET cdzLoadTermParam(void)
{

	ST_INT32 hTermParaHandle = INVALID_HANDLE_VALUE;
	ST_INT32 filesize;
	DWORD bWrittenLen = 0;
	DWORD bReadLen = 0;
//	DWORD error;

	if ((hTermParaHandle=open(CFG_TERM_PARA_FILE_PATH, O_RDONLY)) == INVALID_HANDLE_VALUE)
	
	{
		if ((hTermParaHandle=open(CFG_TERM_PARA_FILE_PATH, O_CREAT|O_RDWR)) == INVALID_HANDLE_VALUE)
		{
			printf("Create file failed\n");
			close(hTermParaHandle);
			return SD_FAILURE;
		}
		else
		{
			
			memcpy (&s_tcdzTermPara, &s_defaultTermPara, sizeof(CDZ_TERM_PARA));
			bWrittenLen = write(hTermParaHandle, (void *)&s_tcdzTermPara, sizeof(CDZ_TERM_PARA));
	//		printf("bWrittenLen is %d\n",bWrittenLen);
			if (bWrittenLen ==INVALID_HANDLE_VALUE)
			{
				printf("write Param failed\n");
				close(hTermParaHandle);
				return SD_FAILURE;
			}
		
			if (bWrittenLen != sizeof(CDZ_TERM_PARA)) 
			{
				printf("bWrittenLen is error\n");
				close(hTermParaHandle);
				return SD_FAILURE;
			}
			close(hTermParaHandle);
		}
	}
	else
	{
		
		filesize = GetFileSize(CFG_TERM_PARA_FILE_PATH);
		printf("filesize is %d\n",filesize);

		if (filesize != sizeof(CDZ_TERM_PARA))
		{
			close(hTermParaHandle);
			return SD_FAILURE;
		}

		bReadLen = read(hTermParaHandle, (void *)&s_tcdzTermPara, sizeof(CDZ_TERM_PARA));
	//	printf("bReadLen is %d\n",bReadLen);
		if(bReadLen == INVALID_HANDLE_VALUE||bReadLen!=sizeof(CDZ_TERM_PARA))
		{
			printf("read termParam failed\n");
			close(hTermParaHandle);
			return SD_FAILURE;
		}

		close(hTermParaHandle);
	}

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzSaveTermParam
 功能描述：	存储参数到文件中
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
			SD_FAILURE:失败
 作    者：	yansudan
 日    期：	2010.10.25
 修改记录：2011 02 06
*********************************************************************************/
static ST_RET cdzSaveTermParam(void)
{

	ST_INT32 hTermParaHandle = INVALID_HANDLE_VALUE;
	DWORD bWrittenLen = 0;

	hTermParaHandle = open(CFG_TERM_PARA_FILE_PATH, O_RDWR);

	if (hTermParaHandle == INVALID_HANDLE_VALUE)
	{
		return SD_FAILURE;
	}

	if ((bWrittenLen = write(hTermParaHandle, (void *)&s_tcdzTermPara, sizeof(CDZ_TERM_PARA)))==INVALID_HANDLE_VALUE||bWrittenLen!=sizeof(CDZ_TERM_PARA))
	{
		close(hTermParaHandle);
		return SD_FAILURE;
	}

	close(hTermParaHandle);

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzLoadMeaPointParam
 功能描述：	从文件中加载测量点参数文件，如果文件不存在，则加载缺省的配置文件
			并保存到文件中
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
			SD_FAILURE:失败
 作    者：yansudan
 日    期：	2010.10.25
 修改记录：2011 02 06
*********************************************************************************/
static ST_RET cdzLoadMeaPointParam(void)
{

	ST_INT32 hMeaPointParaHandle = INVALID_HANDLE_VALUE;
	ST_INT32 filesize;
	DWORD bWrittenLen = 0;
	DWORD bReadLen = 0;

	if ((hMeaPointParaHandle=open(CFG_MEA_PARA_FILE_PATH, O_RDONLY)) == INVALID_HANDLE_VALUE)
	{
		if ((hMeaPointParaHandle=open(CFG_MEA_PARA_FILE_PATH, O_CREAT|O_RDWR)) == INVALID_HANDLE_VALUE)
		{
			printf("MeaPointfile Create failed\n");
			close(hMeaPointParaHandle);
			return SD_FAILURE;
		}
		else
		{

			memcpy (s_tcdzMeaPointPara, s_tdefaultcdzMeaPointPara, (sizeof(CDZ_MEA_POINT_PARA) * CDZ_MEA_POINT_NUM));
			bWrittenLen = write(hMeaPointParaHandle, (void *)&s_tcdzMeaPointPara, sizeof(CDZ_MEA_POINT_PARA)*CDZ_MEA_POINT_NUM);
			if (bWrittenLen ==INVALID_HANDLE_VALUE)
			{
				close(hMeaPointParaHandle);
				return SD_FAILURE;
			}
		
			if (bWrittenLen != sizeof(CDZ_MEA_POINT_PARA)*CDZ_MEA_POINT_NUM) 
			{

				close(hMeaPointParaHandle);
				return SD_FAILURE;
			}
			close(hMeaPointParaHandle);
		}
	}

	else
	{
		filesize = GetFileSize(CFG_MEA_PARA_FILE_PATH);

		if (filesize != sizeof(CDZ_MEA_POINT_PARA)*CDZ_MEA_POINT_NUM)
		{
			close(hMeaPointParaHandle);
			return SD_FAILURE;
		}

		bReadLen = read(hMeaPointParaHandle, (void *)&s_tcdzMeaPointPara, sizeof(CDZ_MEA_POINT_PARA)*CDZ_MEA_POINT_NUM);
		if(bReadLen ==INVALID_HANDLE_VALUE||bReadLen!=sizeof(CDZ_MEA_POINT_PARA)*CDZ_MEA_POINT_NUM)
		{
			printf("meapoint readparam failed\n");
			close(hMeaPointParaHandle);
			return SD_FAILURE;
		}

		close(hMeaPointParaHandle);
	}

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzSaveMeaPointParam
 功能描述：	存储参数到文件中
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
			SD_FAILURE:失败
 作    者：	yansudan
 日    期：	2010.10.25
 修改记录：2011 02 06
*********************************************************************************/
static ST_RET cdzSaveMeaPointParam(void)
{

	ST_INT32 hMeaPointParaHandle = INVALID_HANDLE_VALUE;
	DWORD bWrittenLen = 0;

	hMeaPointParaHandle = open(CFG_MEA_PARA_FILE_PATH, O_RDWR);

	if (hMeaPointParaHandle == INVALID_HANDLE_VALUE)
	{
		return SD_FAILURE;
	}

	if ((bWrittenLen = write(hMeaPointParaHandle, (void *)&s_tcdzMeaPointPara, sizeof(CDZ_MEA_POINT_PARA)))==INVALID_HANDLE_VALUE|| bWrittenLen!=sizeof(CDZ_MEA_POINT_PARA) * CDZ_MEA_POINT_NUM)
	{
		close(hMeaPointParaHandle);
		return SD_FAILURE;
	}

	close(hMeaPointParaHandle);

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzLoadParam
 功能描述：	从文件中加载参数
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
			SD_FAILURE:失败
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_RET cdzLoadParam(void)
{
	ST_RET sret = SD_FAILURE;

	sret = cdzLoadTermParam();

	if (sret != SD_SUCCESS)
	{
		printf("Term Param load failed\n");
		return sret;
	}

	sret = cdzLoadMeaPointParam();

	if (sret != SD_SUCCESS)
	{
		printf("MeaPointParam load failed\n");
		return sret;
	}

	return sret;
}

/********************************************************************************* 
 函数名称： cdzSaveParam
 功能描述：	存储参数
 输    入：	无
 输	   出： 无
 返 回 值： SD_SUCCESS:成功
			SD_FAILURE:失败
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/

ST_RET cdzSaveParam(void)
{
	ST_RET sret = SD_FAILURE;

	sret = cdzSaveTermParam();

	if (sret != SD_SUCCESS)
	{
		return sret;
	}

	sret = cdzSaveMeaPointParam();

	if (sret != SD_SUCCESS)
	{
		return sret;
	}

	return sret;
}

/********************************************************************************* 
 函数名称： cdzGetDataItemLenth
 功能描述：	获取数据项的长度
 输    入：	cdzMeaPointFlag:测量点标志，8字节
			cdzIdentCode:标识编码
 输	   出： 无
 返 回 值： 0:此数据项不存在,非0:此数据项的字节长度(BYTE)
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：2011。02.06
*********************************************************************************/
ST_UINT16 cdzGetDataItemLenth(const ST_UCHAR *cdzMeaPointFlag, ST_UINT16 cdzIdentCode)
{
	const CDZ_DATA_ITEM *pcdzDataItem;
	ST_UINT16 i = 0;
	ST_UINT16 sDataLen = 0;
	ST_UINT16 sDataItemCount = 0;
	ST_UINT16 sMeaPointNo = 0;

	if (cdzMeaPointFlag == NULL) return 0;

	if ((pcdzDataItem = cdzSearchDataItem(cdzMeaPointFlag, cdzIdentCode, &sMeaPointNo)) == NULL)
	{
		return 0;
	}

	
	if (pcdzDataItem->pData == NULL)
	{
		sDataItemCount = pcdzDataItem->DataItemCount;
		pcdzDataItem--;
		for (i=0; i<sDataItemCount && pcdzDataItem!=NULL; i++)
		{
			sDataLen += pcdzDataItem->Lenth;
			pcdzDataItem--;
		}
	}
	else
	{
		sDataLen = pcdzDataItem->Lenth;
	}

	return sDataLen;
}

/********************************************************************************* 
 函数名称： cdzGetDataItemLenthMeaPoint
 功能描述：	获取数据项的长度
 输    入：	cdzMeaPointFlag:测量点标志，8字节
			cdzIdentCode:标识编码
 输	   出： 无
 返 回 值： 0:此数据项不存在,非0:此数据项的字节长度(BYTE)
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_UINT16 cdzGetDataItemLenthMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode)
{
	const CDZ_DATA_ITEM *pcdzDataItem;
	ST_UINT16 i = 0;
	ST_UINT16 sDataLen = 0;
	ST_UINT16 sDataItemCount = 0;


	if ((pcdzDataItem = cdzSearchDataItemMeaPoint(sMeaPointNo, cdzIdentCode)) == NULL)
	{
		return 0;
	}


	if (pcdzDataItem->pData == NULL)
	{
		sDataItemCount = pcdzDataItem->DataItemCount;
		pcdzDataItem--;
		for (i=0; i<sDataItemCount && pcdzDataItem!=NULL; i++)
		{
			sDataLen += pcdzDataItem->Lenth;
			pcdzDataItem--;
		}
	}
	else
	{
		sDataLen = pcdzDataItem->Lenth;
	}

	return sDataLen;
}

/********************************************************************************* 
 函数名称： cdzReadDataItemContentMeaPoint
 功能描述：	获取数据项的内容
 输    入：	sMeaPointNo:测量点号
			cdzIdentCode:标识编码
			sDataItemContent:数据内容
 输	   出： 无
 返 回 值： 0:此数据项不存在,非0:此数据项的字节长度(BYTE),-1:读数据项出错
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：2011.02.06
*********************************************************************************/
ST_INT16 cdzReadDataItemContentMeaPoint(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   ST_UCHAR *sDataItemContent)
{
	const CDZ_DATA_ITEM *pcdzDataItem;
	ST_UINT16 sDataLen = 0;
	ST_UINT16 sDataItemCount = 0;
	ST_UINT16 i = 0;


	if (sDataItemContent == NULL) return 0;

	if ((pcdzDataItem = cdzSearchDataItemMeaPoint(sMeaPointNo, cdzIdentCode)) == NULL)
	{
		printf("pcdzDataItem is null\n");
		return 0;
	}

	if (pcdzDataItem->pData == NULL)
	{
		sDataItemCount = pcdzDataItem->DataItemCount;
		pcdzDataItem--;
		for (i=0; i<sDataItemCount && pcdzDataItem!=NULL; i++)
		{

			if ((pcdzDataItem->Atrrib & CDZ_OPT_RO) == 0) return -1;

			memcpy(&sDataItemContent[sDataLen], pcdzDataItem->pData, pcdzDataItem->Lenth);
			
			sDataLen += pcdzDataItem->Lenth;
			pcdzDataItem--;
		}
	}
	else
	{
	
		if ((pcdzDataItem->Atrrib & CDZ_OPT_RO) == 0) return -1;
		printf("Find pcdzDataItem\n");
		memcpy(sDataItemContent, pcdzDataItem->pData, pcdzDataItem->Lenth);

		sDataLen = pcdzDataItem->Lenth;
	}
	 printf("sDataLen %d\n",sDataLen);
	return sDataLen;
}

/********************************************************************************* 
 函数名称： cdzReadDataItemContent
 功能描述：	获取数据项的内容
 输    入：	cdzMeaPointFlag:测量点标志，8字节
			cdzIdentCode:标识编码
			sDataItemContent:数据内容
 输	   出： 无
 返 回 值： 0:此数据项不存在,非0:此数据项的字节长度(BYTE),-1:读数据项出错
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_INT16 cdzReadDataItemContent(const ST_UCHAR *cdzMeaPointFlag, ST_UINT16 cdzIdentCode,
							   ST_UCHAR *sDataItemContent)
{
	const CDZ_DATA_ITEM *pcdzDataItem;
	ST_UINT16 sDataLen = 0;
	ST_UINT16 sMeaPointNo = 0;
	ST_UINT16 sDataItemCount = 0;
	ST_UINT16 i = 0;


	if (cdzMeaPointFlag == NULL || sDataItemContent == NULL) return 0;

	if ((pcdzDataItem = cdzSearchDataItem(cdzMeaPointFlag, cdzIdentCode, &sMeaPointNo)) == NULL)
	{
		return 0;
	}


	if (pcdzDataItem->pData == NULL)
	{
		sDataItemCount = pcdzDataItem->DataItemCount;
		pcdzDataItem--;
		for (i=0; i<sDataItemCount&& pcdzDataItem!=NULL; i++)
		{

			if ((pcdzDataItem->Atrrib & CDZ_OPT_RO) == 0) return -1;

			memcpy(&sDataItemContent[sDataLen], pcdzDataItem->pData, pcdzDataItem->Lenth);

			sDataLen += pcdzDataItem->Lenth;
			pcdzDataItem--;
		}
	}
	else
	{

		if ((pcdzDataItem->Atrrib & CDZ_OPT_RO) == 0) return -1;

		memcpy(sDataItemContent, pcdzDataItem->pData, pcdzDataItem->Lenth);

		sDataLen = pcdzDataItem->Lenth;
	}

	return sDataLen;
}

/********************************************************************************* 
 函数名称： cdzWriteDataItemContentAuth
 功能描述：	更新数据项的内容
 输    入：	sMeaPointNo:测量点号
			cdzIdentCode:标识编码
			sDataItemContent:数据内容
			sAuthLevel:权限级别
			sPassword:密码
 输	   出： 无
 返 回 值： SD_SUCCESS:写成功，SD_FAILURE;写失败
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录： 2011.02.06
*********************************************************************************/
ST_RET cdzWriteDataItemContentAuth(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   const ST_UCHAR *sDataItemContent, ST_UCHAR sAuthLevel, 
							   const ST_UCHAR *sPassword)
{
	const CDZ_DATA_ITEM *pcdzDataItem;
	ST_UCHAR sAuthLocal = 0;
	ST_UINT16 sDataLen = 0;
	ST_UINT16 sDataItemCount = 0;
	ST_UINT16 i = 0;


	if (sDataItemContent == NULL
		|| sPassword == NULL) return SD_FAILURE;



	if (sAuthLevel == 0x00)	
	{
		if (sPassword[0] != s_tcdzTermPara.SetPass[0]
			|| sPassword[1] != s_tcdzTermPara.SetPass[1]
			|| sPassword[2] != s_tcdzTermPara.SetPass[2])
		{
			return SD_FAILURE;
		}

		sAuthLocal = CDZ_AUTH_LOW;
	}
	else if (sAuthLevel == 0x11)
	{
		if (sPassword[0] != s_tcdzTermPara.AdminPass[0]
		|| sPassword[1] != s_tcdzTermPara.AdminPass[1]
		|| sPassword[2] != s_tcdzTermPara.AdminPass[2])
		{
			return SD_FAILURE;
		}

		sAuthLocal = CDZ_AUTH_HIGH;
	}
	else
	{
		return SD_FAILURE;
	}

	if ((pcdzDataItem = cdzSearchDataItemMeaPoint(sMeaPointNo, cdzIdentCode)) == NULL)
	{
		return SD_FAILURE;
	}

	if ((pcdzDataItem->AuthLevel & sAuthLocal) == 0) return SD_FAILURE;


	if (pcdzDataItem->pData == NULL)
	{
		sDataItemCount = pcdzDataItem->DataItemCount;
		pcdzDataItem --;
		for (i=0; i<sDataItemCount&& pcdzDataItem!=NULL; i++)
		{

			if ((pcdzDataItem->Atrrib & CDZ_OPT_WO) == 0) return SD_FAILURE;

			if (cdzDataItemCheck(pcdzDataItem->IdentCode, &sDataItemContent[sDataLen]) == SD_FAILURE)
			{
				return SD_FAILURE;
			}

			memcpy(pcdzDataItem->pData, &sDataItemContent[sDataLen], pcdzDataItem->Lenth);
			sDataLen += pcdzDataItem->Lenth;

			pcdzDataItem --;
		}
	}
	else
	{

		if ((pcdzDataItem->Atrrib & CDZ_OPT_WO) == 0) return SD_FAILURE;


		if (cdzDataItemCheck(pcdzDataItem->IdentCode, &sDataItemContent[sDataLen]) == SD_FAILURE)
		{
			return SD_FAILURE;
		}
		memcpy(pcdzDataItem->pData, sDataItemContent, pcdzDataItem->Lenth);

	}

	printf("write data is compelete\n");
	cdzSaveParam();

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzWriteDataItemContentNoAuth
 功能描述：	更新数据项的内容
 输    入：	sMeaPointNo:测点号
			cdzIdentCode:标识编码
			sDataItemContent:数据内容
 输	   出： 无
 返 回 值： SD_SUCCESS:写成功，SD_FAILURE;写失败
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
ST_RET cdzWriteDataItemContentNoAuth(ST_UINT16 sMeaPointNo, ST_UINT16 cdzIdentCode,
							   const ST_UCHAR *sDataItemContent)
{
	const CDZ_DATA_ITEM *pcdzDataItem;
	ST_UCHAR sAuthLocal = 0;
	ST_UINT16 sDataLen = 0;
	ST_UINT16 sDataItemCount = 0;
	ST_UINT16 i = 0;


	if (sDataItemContent == NULL) return SD_FAILURE;

	if ((pcdzDataItem = cdzSearchDataItemMeaPoint(sMeaPointNo, cdzIdentCode)) == NULL)
	{
		return SD_FAILURE;
	}

	if ((pcdzDataItem->AuthLevel & sAuthLocal) == 0) return SD_FAILURE;


	if (pcdzDataItem->pData == NULL)
	{
		sDataItemCount = pcdzDataItem->DataItemCount;
		pcdzDataItem --;
		for (i=0; i<sDataItemCount&& pcdzDataItem!=NULL; i++)
		{
			if (cdzDataItemCheck(pcdzDataItem->IdentCode, &sDataItemContent[sDataLen]) == SD_FAILURE)
			{
				return SD_FAILURE;
			}

			memcpy(pcdzDataItem->pData, &sDataItemContent[sDataLen], pcdzDataItem->Lenth);

			sDataLen += pcdzDataItem->Lenth;

			pcdzDataItem--;
		}
	}
	else
	{

		if (cdzDataItemCheck(pcdzDataItem->IdentCode, &sDataItemContent[sDataLen]) == SD_FAILURE)
		{
			return SD_FAILURE;
		}

		memcpy(pcdzDataItem->pData, sDataItemContent, pcdzDataItem->Lenth);

	}

	return SD_SUCCESS;
}

/********************************************************************************* 
 函数名称： cdzInitDefaultData
 功能描述：	所有可写的参数恢复出厂设置（通信参数除外）
 输    入：	无
 输	   出： 无
 返 回 值： 无
 作    者：	yansudan
 日    期：	2011.2.6
 修改记录：
*********************************************************************************/
void cdzInitDefaultData(void)
{
	return;
}













