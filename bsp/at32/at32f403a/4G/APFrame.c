/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*---------------------------------------------------------------------------------------
* FileName			: GPRSMain.c
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* //2010		    : V
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
/**********************************问题点********************************/
/*
1、计费需要重新处理下
2、B2长度不可变的码
*/
#include "4GRecv.h"
#include "APFrame.h"
#include <string.h>
#include "dwin_com_pro.h"
#include "common.h"
#include "chtask.h"
#include "DwinProtocol.h"
#include "mfrc522.h"
#include "rn8209d.h"
#include "fal.h"
#include "dlt645_port.h"
/* Private define-----------------------------------------------------------------------------*/
#define AP_RECV_TABLE_NUM   15
#define AP_SEND_FRAME_LEN	7

#if NET_YX_SELCT == XY_AP
__packed typedef struct{
	//CP56格式
	uint16_t Second;  //0.001秒
	uint8_t	Minute;
	uint8_t	Hour;
	uint8_t	Day;
	uint8_t	Month;
	uint8_t	Year;
}AP_TIME;

//充电实时数据
__packed typedef struct{
	uint8_t      	devnum[8];		//充电桩编号
	uint8_t     	gun_index;		//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t			gunstate;		//连接确认开关状态     布尔型, 变化上传;0关，1开是否插枪，0表示未插枪，1表示已插枪
	uint16_t 		work_stat;		//工作状态  0离线1故障2待机（空闲）8充电启动中3工作（充电）7 预约9 充电完成10 定时充电
	uint8_t			DCUVol;			//直流母线输出过压告警
	uint8_t			DCQVol;			//直流母线输出欠压告警
	uint8_t			batteryUCur;	//蓄电池充电过流告警
	//精确到小数点后一位
	uint16_t 		ac_a_vol;		//交流A相充电电压	
	uint16_t 		ac_b_vol;		//流B相充电电压
	uint16_t 		ac_c_vol;		//交流C相充电电压
	uint16_t 		ac_a_cur;		//交流A相充电电流
	uint16_t 		ac_b_cur;		//交流B相充电电流
	uint16_t 		ac_c_cur;		//交流C相充电电流
	uint16_t		InYPower;		//输入侧总有功功率
	uint16_t		InWPower;		//输入侧无功功率
	uint16_t		InPF;			//输入侧功率因数
	uint16_t		dc_charge_voltage;		//直流充电电压	
	uint16_t 		dc_charge_current;		//直流充电电流    精确到小数点后二位
	uint8_t			OutDCSWState;			//输出继电器状态   布尔型, 变化上传;0关，1开
	uint8_t			BMSState;				//BMS通信异常    布尔型, 变化上传；0正常，1异常
	uint8_t			batteryLinkState;		//电池连接状态
	uint16_t		batteryMaxVol;			//单体电池最高电压  精确到小数点后三位
	uint16_t		batteryMinVol;			//单体电池最低电压  精确到小数点后三位
	uint32_t 		ChargeKwh; 				//有功总电量  精确到小数点后三位
	uint32_t 		ChargeJKwh; 			//尖电量  精确到小数点后三位
	uint32_t 		ChargeFKwh; 			//峰电量  精确到小数点后三位
	uint32_t 		ChargePKwh; 			//平电量  精确到小数点后三位
	uint32_t 		ChargeGKwh; 			//谷电量  精确到小数点后三位
	uint16_t		soc;					//整数
	uint16_t		ChargeTime;				//累计充电时间	单位：min
	uint8_t			CarVin[32];				//车辆vin码
	uint8_t			Allfail[21];			//没有具体写了。。
	uint32_t		ChargePowerMoney;		//充电费  精确到小数点后两位	
	uint32_t		ChargeFWMoney;		//服务费  精确到小数点后两位
	uint32_t charge_full_time_left;		//距离满电剩余充电时间(min)	
	uint8_t OrderNum[16];					//本次充电的订单号，非充电中时清空
	uint8_t	FailNum[8];					//其他故障代码 0正常，非0 故障，具体查看协议。
}AP_send_B1;


//刷卡鉴权
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		CardNum[8];			//物理卡号  压缩BCD码 8Byte
	uint8_t		Code[16];			//密码	都填写0
	uint32_t		Money;				//卡余额	圈存卡内余额  都填写0
	uint8_t CarVin[32];				//车Vin
}AP_send_B6;

//VIN鉴权
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t		Code[16];			//密码	都填写0
	uint8_t CarVin[32];				//车Vin
}AP_send_B8;

//刷卡Vin启动
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		CardNum[8];			//物理卡号  压缩BCD码 8Byte
	uint8_t		Code[16];			//密码	都填写0
	uint32_t		Money;				//卡余额	圈存卡内余额  都填写0
	uint8_t CarVin[32];				//车Vin
	uint8_t	 ChargeType;				//启动充电控制方式	1：电量控制充电，2：时间控制充电，3：金额控制充电，4：充满为止  写04
	uint8_t	StartCmdData[4];			//启动充电控制数据  启动控制数据，全部为0
}AP_send_B10;

//充电实时数据
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t			bmsversion[3];		//BMS通信协议版本号
	uint8_t battype;			//01H:铅酸;02镍氢;03磷酸铁锂;04锰酸锂;05钴酸锂;06三元材料;07聚合物锂离子;08钛酸锂;FFH其他电池
	uint16_t batedcapacity;						//整车动力蓄电池系统额定容量0.1
	uint16_t ratedvol;								//整车动力蓄电池系统额定总电压0.1
	uint8_t battername[4];				//电池生产厂商名称
	uint8_t batterinfo[19];			//电池信息，具体为细分详见协议  都填写 0xff
	uint16_t unitbatterymaxvol;						//0.01V/位 数据范围0-24V
	uint16_t bmsMaxcurrent;							//0.1A/位 -400A偏移量
	uint16_t batterywholeenergy;						//动力蓄电池标称总能量0.1
	uint16_t chgmaxoutvol;							//最高允许充电总电压 0.1
	uint8_t MaxTemprature;							//1摄氏度/位  -50度偏移量  数据范围-50+200
	uint16_t batterysoc;								//0.1%/位 数据范围0-100% 充电之前的SOC
	uint16_t chargevolmeasureval;				//整车动力蓄电池当前电池电压  0.1V/位
	uint16_t  bmsneedvolt;			//电压需求  0.1V/位  充电过程中动态变化
	uint16_t  bmsneedcurr;			//电流需求   0.1A/位,-400A偏移量 充电过程中 动态变化
	uint8_t  chargemode;				//充电模式 0x01恒压充电, 0x02恒流充电
	uint16_t outvolt;					//充电电压测量值    0.1V/位
	uint16_t outcurr;					//充电电流测量值   0.1A/位，-400A偏移量
	uint16_t MaxVolGroup;			//最高单体动力蓄电池电压及其组号 1-12位电池电压 0.01V/位，数据范围0-24V, 13-16位 组号,1/位数据范围 0-15
	uint16_t  remaindertime;			//估算剩余充电时间 0-600分钟，超过600按600发送
	uint16_t MaxVolNum;			//最高单体动力蓄电池电压所在编号   数据范围1-256
	uint8_t  batpackmaxtemp;			//最高单体动力蓄电池电压所在编号 50度偏移 数据范围 -50-+200
	uint8_t  batpackmaxnum;			//最高温度监测点编号
	uint8_t  batpackmintemp;			//最低动力储电池温度
	uint8_t  batpackminnum;			//最低动力蓄电池温度检测点编号
	uint8_t batstate[3];			//未细分
	uint8_t ifcharge;				//充电允许    00禁止,01允许
	uint8_t moduleinfo[10];		//模块信息目前都为0
	uint16_t  DCouttemp1;				//充电桩温度精确到小数点后二位
	uint16_t  DCouttemp2;				// 充电枪温度 精确到小数点后二位
	uint16_t  Moduletemp3;				//充电模块温度 精确到小数点后二位
	uint16_t  batmaxvolt;				//单体电池最高电压  0.001  精确到小数点后三位
	uint16_t  batminvolt;				//单体电池最低电压 	0.001  精确到小数点后三位

}AP_SEND_B37;

//启停应答发送
//充电实时数据
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		ifsuccess;			//成功标志 0：成功，1：失败
	uint8_t 		failcause[2];		//启动充电失败原因   0000：成功，0001：正在充电中，0002：系统故障， 0003：其他原因
	uint8_t Cmd;						//控制命令    0：停止充电，1：启动充电，2：定时充电启动，3：预约充电启动
	AP_TIME CmdTime;				//控制时间  CP56Time2a格式
	uint8_t DrderNum[16];				//订单号     平台订单号
}AP_SEND_B5;	

//发送交易记录上传
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		DrderNum[16];		//交易流水号  压缩BCD码
	uint8_t		CardNum[8];		//物理卡号  16位编码
	AP_TIME	StartTime;				//开始时间  CP56Time2a格式
	AP_TIME StopTime;				//结束时间  CP56Time2a格式
	uint16_t	ChargeTime;				//累计充电时间   单位：min
	uint8_t 	MeterInfo[32];			//尖峰平古起始停止电表读数，目前都为0
	uint32_t 		ChargeJKwh; 			//尖电量  精确到小数点后2位
	uint32_t 		ChargeFKwh; 			//峰电量  精确到小数点后2位
	uint32_t 		ChargePKwh; 			//平电量  精确到小数点后2位
	uint32_t 		ChargeGKwh; 			//谷电量  精确到小数点后2位
	uint32_t 		ChargeAllKwh; 			//总电量  精确到小数点后2位
	uint32_t 		ChargeStartKwh; 		//总起示值  精确到小数点后2位
	uint32_t 		ChargeStopKwh; 			//总止示值  精确到小数点后2位
	uint16_t		StartSoc;				//充电前SOC
	uint16_t		StopSoc;				//结束后SOC
	uint8_t 	CardVin[32];				//电动汽车唯一标识
	uint16_t	StopReason;					//停止原因，详见协议
	uint32_t ChargeFee;					//充电费    精确到小数点后两位
	uint32_t ServeFee;					//服务费    精确到小数点后两位
	uint8_t CarNmb[8];					//车牌号	
	uint8_t ifCarNmb;						//确认车牌号功能是否开启   桩的参数设置，确认车牌号开关是否开启（0，未开启；1,开启）Boss增加参数控制，下发给桩，控制参数的开关
}AP_SEND_B12;	

//离线交易记录
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		DrderNum[16];		//交易流水号  压缩BCD码
	uint8_t		CardNum[8];		//物理卡号  16位编码
	AP_TIME	StartTime;				//开始时间  CP56Time2a格式
	AP_TIME StopTime;				//结束时间  CP56Time2a格式
	uint16_t	ChargeTime;				//累计充电时间   单位：min
	uint8_t 	MeterInfo[32];			//尖峰平古起始停止电表读数，目前都为0
	uint16_t 	type;					//计量类型  0001-里程 0002-充电量 0003-放电量
	uint32_t 		ChargeStartKwh; 		//总起示值  精确到小数点后2位
	uint32_t 		ChargeStopKwh; 			//总止示值  精确到小数点后2位
//	uint8_t PricInfo[48];					//不具体细分   目前全部为0
	
	uint32_t		JBill;				//尖电价  精确到小数点后五位
	uint32_t 		ChargeJKwh; 			//尖电量  精确到小数点后2位
	uint32_t		JMoney;				//尖金额  精确到小数点后2位
	
	uint32_t		FBill;				//峰电价  精确到小数点后五位
	uint32_t 		ChargeFKwh; 			//峰电量  精确到小数点后2位
	uint32_t		FMoney;				//尖金额  精确到小数点后2位
	
	
	uint32_t		PBill;				//平电价  精确到小数点后五位
	uint32_t 		ChargePKwh; 			//平电量  精确到小数点后2位
	uint32_t		PMoney;				//尖金额  精确到小数点后2位
	
	uint32_t		GBill;				//谷电价  精确到小数点后五位
	uint32_t 		ChargeGKwh; 			//谷电量  精确到小数点后2位
	uint32_t		GMoney;				//尖金额  精确到小数点后2位
	
	uint32_t 		ChargeAllKwh; 			//总电量  精确到小数点后2位
	uint16_t		StartSoc;				//充电前SOC
	uint16_t		StopSoc;				//结束后SOC
	uint8_t 	CardVin[32];				//电动汽车唯一标识
	uint16_t	StopReason;					//停止原因，0：未知，1：充满，2：主动停止（按键），3：主动停止（远程），4：紧急停机，5：故障停止，6：其他（BMS协议的原因）
	uint32_t ChargeFee;					//充电费    精确到小数点后两位
	uint32_t ServeFee;					//服务费    精确到小数点后两位
	uint8_t CarNmb[8];					//车牌号	
	uint8_t ifCarNmb;						//确认车牌号功能是否开启   桩的参数设置，确认车牌号开关是否开启（0，未开启；1,开启）Boss增加参数控制，下发给桩，控制参数的开关
}AP_SEND_B15;	


__packed typedef struct{
	_TIME_QUANTUM CurTimeQuantum;		//当前处于哪个时间段  尖 峰 平 古
	uint32_t Power;				//电量  精度：0.01度(千瓦时)
	uint32_t Price;				//电费  精度：0.01度(千瓦时)
	uint32_t	ServeFee;			//服务费
}_PRIC_INFO;
//发送交易记录上传     充电完成后，在B12发送完成后上报。
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		DrderNum[16];		//交易流水号  压缩BCD码
	uint8_t		PricTypeID[8];		//计费模型ID     运营管理系统产生
	uint8_t		StartTime[7];		//切换时间  CP56Time2a格式
	uint8_t		Timenum;			//时段个数
    _PRIC_INFO	PricInfo[TIME_QUANTUM_MAX];	//费率信息，最多十二个时间段
}AP_SEND_B53;
	

//刷卡白名单下发
__packed typedef struct
{
	uint8_t	CardNum[8];			//卡号
	uint8_t ifSuccess;			//1-成功 2-失败
}_AP_CARD_STATE;

__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	AP_TIME     time;				//时间戳序号
	uint8_t 		CardNub[8];			//白名单编号
	uint8_t 		Nnm;				//白名单数量  不多于255 ，一次最多12张
	_AP_CARD_STATE	CardState[12];	//最多十二个
}AP_SEND_B18;

//接收VIN
__packed typedef struct
{
	uint8_t	VinNum[17];			//卡号
	uint8_t ifSuccess;			//1-成功 2-失败
}_AP_VIN_STATE;

__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	AP_TIME     time;				//时间戳序号
	uint8_t 		VinNub[8];			//白名单编号
	uint8_t 		Nnm;				//白名单数量  不多于255 ，一次最多12张
	_AP_VIN_STATE	VinState[12];	//最多十二个
}AP_SEND_B20;

//清空白名单应答
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	AP_TIME     time;				//时间戳序号
	uint8_t 		VinNub[8];			//白名单编号
	uint8_t		ifseccess;			//0成功 1失败
}AP_SEND_B22;


//计费回复B3
extern const struct fal_partition *charge_rete_info;
extern _m1_card_info m1_card_info;		 //M1卡相关信息
extern SYSTEM_RTCTIME gs_SysTime;
extern RATE_T	           stChRate;			//费率信息
extern CH_TASK_T stChTcb[GUN_MAX];
AP_send_B1 APSendB1[GUN_MAX]; 	//发送充电桩实时数据
AP_send_B6 APSendB6[GUN_MAX];	//在线刷卡鉴权
AP_send_B8 APSendB8[GUN_MAX];	//在线Vin鉴权
AP_send_B10 APSendB10[GUN_MAX];	//在线刷卡启动充电
AP_SEND_B37 APSendB37[GUN_MAX]; //车辆信息
AP_SEND_B5	APSendStartAckB5[GUN_MAX];			//发送启动应答信息
AP_SEND_B5	APSendStopAckB5[GUN_MAX];			//发送停止应答信息
AP_SEND_B12	APSendBilB12[GUN_MAX];			//上传在线交易记录
AP_SEND_B15 APSendBilB15[GUN_MAX];			//上传离线交易记录     结构体与实际的报文长度对应不上
AP_SEND_B53 APSendBilB53[GUN_MAX];			//上传分时交易记录   充电完成后，在B12发送完成后上报。
AP_SEND_B18	APSendCardB18;					//卡白名单下发
AP_SEND_B20	APSendVinB20;					//VIN白名单下发
AP_SEND_B22 APSendVinCardResB20;			//清空白名单
_4G_START_TYPE APStartType[GUN_MAX] ={ _4G_APP_START,_4G_APP_START};					//0表示App启动 1表示刷卡启动


uint8_t  SendB53State = 0;			//是否发送分时计费  0表示不发送分时计费   1表示发送分时计费
/*******************************************接收数据*************************************/
//接收数据
typedef struct
{
	uint8_t * pdata;
	uint8_t len;
	 uint8_t  (*recvfunction)(uint8_t *,uint16_t);				//传入数据长度
}_AP_RECV_DISPOSE;

//下发费率1
__packed typedef struct{
	uint8_t      	devnum[8];		//充电桩编号
	uint8_t			PricTypeID[8];	//计费模型ID     运营管理系统产生
	uint8_t			StartTime[7];	//生效时间  CP56Time2a格式
	uint8_t			StopTime[7];	//失效时间
	uint16_t		UserState;		//执行状态  01-有效 02-无效
	uint16_t		type;			//计量类型 01-里程 02-充电量 03-放电量
	uint8_t			FricNum[48];	//费率号
	uint8_t			FricNumber;		//费率数量  必须为4
	uint32_t		Fric[4];			//4个费率  精确到小数点后五位
	uint32_t		ServiceFric;
}AP_RECV_B2;

//下发费率2
__packed typedef struct{
	uint8_t 	CurTimeQuantum;			//充电桩编号  1：尖2：峰3：平4：谷
	uint8_t 	StartTime[2];			//24小时制，格式：时分（HHmm）  BCD码
	uint8_t 	StopTime[2];			//24小时制，格式：时分（HHmm）	BCD码
	uint32_t 	Fric;					//精确到小数点后五位
	uint32_t	ServiceFric;			//精确到小数点后五位
}SETPRIC_INFO;
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t		PricTypeID[8];		//计费模型ID     运营管理系统产生
	uint8_t		StartTime[7];		//生效时间  CP56Time2a格式
	uint8_t		StopTime[7];		//失效时间
	uint16_t		UserState;			//执行状态  01-有效 02-无效
	uint8_t		FricNumber;			//最多不超过12个
	SETPRIC_INFO SetPricInfo[12];	//费率信息
}AP_RECV_B47;


//下发启停充电
__packed typedef struct{
	uint8_t 	devnum[8];		//充电桩编号
	uint8_t     	gun_index;		//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t     	CmdST;			//0：停止充电，1：启动充电
	uint8_t StartTJ;				//启动充电条件 0：即时充电，1：定时充电
	uint8_t StartType;			//1：电量控制充电，2：时间控制充电，3：金额控制充电，4：充满为止
	uint32_t StartCmdData;		//启动充电控制数据
	uint8_t UserNum[8];			//用户编号    压缩BCD码  8Byte  平台交易流水后16位编码，用做在线交易记录中的物理卡号
	uint8_t DrderNum[16];			//订单号  压缩BCD码
}AP_RECV_B4;

//刷卡鉴权回复（在线刷卡充电）
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		CardNum[8];			//物理卡号  压缩BCD码 8Byte
	uint8_t CarVin[32];				//车Vin
	uint8_t BillBum[8];				//计费模型编码 BCD 8Byte
	uint32_t Money;					//账户余额  精确到小数点后两位
	uint8_t	IfSuccess;				//鉴权成功标志
	uint8_t FailCause[2];				//鉴权失败原因  BCD  5001:无效卡号5003:余额不足5004:卡锁住5007:无效卡5011:套餐余额不足5015:无效车状态5016:无效账户状态5017:密码错误一次5018:密码错误两次5019:密码错误三次5099:系统错误
	uint32_t	SurplusKM;				//剩余里程 精确到小数点后两位
	uint32_t	ChargeKwH;				//可充电量 精确到小数点后两位
	uint32_t SurplusNum;				//剩余次数  精确到小数点后两位
	uint8_t ChargeType;				//充电方式 0 : 有卡充电	1 : 无卡充电
	//uint8_t CarNum[8];				//车牌号
}AP_RECV_B7;


//刷卡启动
__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t ifSuccess;				//通知成功标志 布尔型（1，通知成功；0，通知失败）
	uint8_t FailInfo[2];				//通知失败原因 BCD 5001:无效卡号5003:余额不足5004:卡锁住5007:无效卡5011:套餐余额不足5015:无效车状态 5016:无效账户状态5017:密码错误一次5018:密码错误两次5019:密码错误三次5099:系统错误
	uint8_t DrderNum[16];				//订单号  压缩BCD码
//	uint8_t CarInfo[17];				//车架号
//	uint8_t CarNum[8];				//车牌号
}AP_RECV_B11;


//刷卡白名单下发
__packed typedef struct
{
	uint8_t	CardNum[8];			//卡号
	uint8_t State;				//1-新增 2-删除，第N个白名单状态	
}_AP_CARD_INFO;

__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	AP_TIME     time;				//时间戳序号
	uint8_t 		CardNub[8];			//白名单编号
	uint8_t 		Nnm;				//白名单数量  不多于255 ，一次最多12张
	_AP_CARD_INFO	CardInfo[12];	//最多十二个
}_AP_RECV_B17;

//接收VIN
__packed typedef struct
{
	uint8_t	VIN[17];			//卡号
	uint8_t State;				//1-新增 2-删除，第N个白名单状态	
}_AP_VIN_INFO;

__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	AP_TIME     time;				//时间戳序号
	uint8_t 		VinNub[8];			//白名单编号
	uint8_t 		Nnm;				//白名单数量  不多于255 ，一次最多12张
	_AP_VIN_INFO	VinInfo[12];	//最多十二个
}_AP_RECV_B19;

__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	AP_TIME     time;				//时间戳序号
	uint8_t 		VinNub[8];			//白名单编号
	uint8_t 		type;				//1:充点卡，2:VIN码
}_AP_RECV_B21;


__packed typedef struct{
	uint8_t      	devnum[8];			//充电桩编号
	uint8_t     	gun_index;			//充电接口标识  充电桩为一桩多充时用来标记接口号，一桩一充时此项为0。
	uint8_t 		ifSuccess;			//0：成功，1：失败
	uint8_t DrderNum[16];				//32位交易代码，桩自己生成 
}_AP_RECV_B16;

AP_RECV_B2 	APRecvB2;					//费率设置B2	
AP_RECV_B47 APRecvB47;					//费率设置B47
AP_TIME APRecvSetTime;			//设置时间
AP_RECV_B4		APRecvStartCmd[GUN_MAX];	//接收到启动命令
AP_RECV_B4		APRecvStopCmd[GUN_MAX];		//接收到启动命令
AP_RECV_B7		APRecvCardCmd[GUN_MAX];		//接收到卡鉴权命令
AP_RECV_B11    	APRecvCardVinStart[GUN_MAX];	//接收到刷卡启动命令
_AP_RECV_B17	APCardWL;					//卡白名单接收
_AP_RECV_B19	APVinWL;					//VIN白名单
_AP_RECV_B21	APVinCardRes;				//白名单清空
_AP_RECV_B16	APOFFLineRec;				//离线交易记录
//（除了B47,B52,B53,B55以外）：充电桩编号前内容不用变，按照示例来，充电桩编号后内容根据实际值来。B47,B52,B53,B55 的长度是可扩展的。
uint8_t RecvRegister[2] = {0x68,0x01};			//注册帧返回（协议返回）
//费率下发返回 B2
//当数据长度为0xff,则数据长度为可变，则不比较数据长度
uint8_t RecvCardB7[15] = {0x68,0x5A,0x00,0x00,0x00,0x00,0x85,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x02};
uint8_t RecvCardB9[15] = {0x68,0x52,0x00,0x00,0x00,0x00,0x85,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x34};
uint8_t RecvCardStartB11[15] = {0x68,0x29,0x00,0x00,0x00,0x00,0x85,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x0C};
uint8_t RecvPricB2[15] = {0x68,0x74,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x05};
uint8_t RecvPricB47[15] = {0x68,0xff,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x40};
uint8_t RecvStartApp[6] = {0x68,0x04,0x0B,0x00,0x00,0x00};
uint8_t RecvHear[6] = {0x68,0x04,0x83,0x00,0x00,0x00};
uint8_t RecvTime[14] = {0x68,0x13,0x00,0x00,0x00,0x00,0x67,0x00,0x06,0x00,0x00,0x00,0x00,0x00};		//对时间
uint8_t RecvSTCmd[15]  = {0x68,0x35,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x15}; 	//启停命令
uint8_t RecvOnlineBillCmd[15]  = {0x68,0x27,0x00,0x00,0x00,0x00,0x82,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x02}; 	//在线交易记录
uint8_t RecvB54BillCmd[15]  = {0x68,0x27,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x43}; 		//在线分时明细交易记录
uint8_t RecvB17CardWL[15]  = {0x68,0xff,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x32}; 		//卡白名单接收
uint8_t RecvB19VINWL[15]  = {0x68,0xff,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x33}; 		//vin白名单接收
uint8_t RecvB21VINCardRes[15]  = {0x68,0x25,0x00,0x00,0x00,0x00,0x85,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x35}; 		//vin白名单接收
uint8_t RecvB16APOFFLineRec[15]  = {0x68,0x27,0x00,0x00,0x00,0x00,0x82,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x03}; 		//vin白名单接收
static uint8_t   AP_RegisteeAck(uint8_t *pdata,uint16_t len);   		//注册帧返回（协议返回）
static uint8_t   AP_RecvPricB2(uint8_t *pdata,uint16_t len);   			//费率设置
static uint8_t   AP_RecvPricB47(uint8_t *pdata,uint16_t len);   			//费率设置
static uint8_t   AP_RecvStartAppAck(uint8_t *pdata,uint16_t len);   	//启动确认帧
static uint8_t   AP_RecvHearAck(uint8_t *pdata,uint16_t len);   		//心跳确认帧
static uint8_t   AP_RecvTime(uint8_t *pdata,uint16_t len);   			//校准时间
static uint8_t   AP_RecvOnlineBillAck(uint8_t *pdata,uint16_t len);	//返回在线交易记录
static uint8_t   AP_RecvB54BillAck(uint8_t *pdata,uint16_t len);		//在线分时明细交易记录
static uint8_t   AP_RecvSTCmd(uint8_t *pdata,uint16_t len);			//接收倒启停命令
static uint8_t   AP_RecvCardB7(uint8_t *pdata,uint16_t len);			//接收到卡鉴权
static uint8_t   AP_RecvCardB9(uint8_t *pdata,uint16_t len);
static uint8_t   AP_RecvCardVinStartB11(uint8_t *pdata,uint16_t len);	//接收到卡VIN启动命令
static uint8_t   AP_RecvB17CardWL(uint8_t *pdata,uint16_t len);		//开白名单接收
static uint8_t AP_RecvB21VINCardRes(uint8_t *pdata,uint16_t len);			//Vin白名单接收
static uint8_t AP_RecvB16OFFLineRco(uint8_t *pdata,uint16_t len);			//离线交易记录应答
_AP_RECV_DISPOSE  APRecvDisposeTable[AP_RECV_TABLE_NUM] = {
	{RecvRegister,			2,		AP_RegisteeAck					},  //注册帧返回（协议返回）
	
	{RecvPricB2,			15,		AP_RecvPricB2						},  //费率设置
	
	{RecvPricB47,			15,		AP_RecvPricB47						},  //费率设置
	
	{RecvStartApp,			6,		AP_RecvStartAppAck				},  //登录
	
	{RecvHear,				6,		AP_RecvHearAck					},  //心跳确认帧
	
	{RecvTime,				14,		AP_RecvTime						},  //校时
	
	{RecvOnlineBillCmd,		15,		AP_RecvOnlineBillAck			},  //返回在线交易记录
	
	{RecvB54BillCmd,		15,		AP_RecvB54BillAck				},  //在线分时明细交易记录
	
	{RecvSTCmd,				15,		AP_RecvSTCmd					},  //接收到启停命令
	
	{RecvCardB7,			15,		AP_RecvCardB7					},  //卡鉴权
	
	{RecvCardStartB11,		15,		AP_RecvCardVinStartB11			},  //接收到卡Vin启动命令
	
	{RecvB17CardWL,			15,		AP_RecvB17CardWL				},	//接收到卡白名单
	
	
	{RecvB21VINCardRes,		15,		AP_RecvB21VINCardRes			},	//接收到白名单清空
	
	{RecvB16APOFFLineRec,	15,		AP_RecvB16OFFLineRco			},	//接收到白名单清空	
	
};

/***********************发送数据*********************************************/
static uint8_t  AP_SendStartApp(void);	//登录下发U帧
static uint8_t  AP_SendHear(void);		//下发心跳帧
static uint8_t   AP_SendRegister(void);	//发送注册帧
static uint8_t  AP_SendChargeInfoA(void);	//A	枪状态帧
static uint8_t  AP_SendChargeInfoB(void); //B 枪状态帧
static uint8_t AP_SendCarInfoA(void); 	//A枪车辆信息，充电中发送
static uint8_t AP_SendCarInfoB(void); 	//B枪车辆信息，充电中发送

static uint8_t  AP_SendSetPricB2Ack(void);
static uint8_t  AP_SendSetPricB47Ack(void);
_4G_SEND_TABLE APSendTable[AP_SEND_FRAME_LEN] = {
	{0,    0,    CM_TIME_10_SEC, 	AP_SendStartApp			},  //U帧

	{0,    0,    CM_TIME_30_SEC, 	AP_SendHear				},	//心跳
	
	{0,    0,    CM_TIME_10_SEC, 	AP_SendRegister			},	//注册帧

	{0,    0,    CM_TIME_30_SEC, 	AP_SendChargeInfoA		},	//充电桩A状态
	
	{0,    0,    CM_TIME_30_SEC, 	AP_SendChargeInfoB		},	//充电桩B状态
	
	{0,    0,    CM_TIME_30_SEC, 	AP_SendCarInfoA			},	//A枪车辆信息，充电中发送
	
	{0,    0,    CM_TIME_30_SEC, 	AP_SendCarInfoB			},	//B枪车辆信息，充电中发送

};

uint8_t FlashDisposeBuf[3000];				//flash处理缓存


/*****************************************************************************
* Function     : AP_CardWhiteListDispose
* Description  : 卡白名单处理
* Input        :
				pcardnud 卡号
				num		 个数
				state    1 表示新增  0表示删减
				
				flash说明
				出厂时第一个字节为0，后面所有都为0XFF
				1第一个字节表示白名单数量,后面都为开号，卡号为8个字节	
		
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_CardWhiteListDispose(uint8_t*  pcardnum,uint16_t num,uint8_t state)
{
	uint8_t curnum,i,j,ifwriter;
	if(pcardnum == NULL || num > 12)		//一次增减少最多十二个
	{
		return RT_FALSE;
	}
	ifwriter  = 0;
	curnum = FlashDisposeBuf[0];
	if(state)  //增加卡号
	{
		for(j = 0;j < num;j++)
		{
			if(curnum == 255)
			{
				break;
			}
			for(i = 0;i < curnum;i++)
			{
			
				if(CmpNBuf(&FlashDisposeBuf[1+i*8],&pcardnum[8*j],8))
				{
					break;  //说明之前就有
				}
			}
			if(i == curnum)
			{
				//说明之前没有这个卡号，添加进来
				memcpy(&FlashDisposeBuf[1+curnum*8],&pcardnum[8*j],8);
				ifwriter = 1;
				curnum++;
				FlashDisposeBuf[0] = curnum;
			}
		}
	}
	else
	{
		//减少卡号
		for(j = 0;j < num;j++)
		{
			if(curnum == 0)
			{
				break;
			}
			for(i = 0;i < curnum;i++)
			{
				if(CmpNBuf(&FlashDisposeBuf[1+i*8],&pcardnum[8*j],8))
				{
					  //说明之前就有
					memset(&FlashDisposeBuf[1+i*8],0xff,8);
					curnum --;
					ifwriter = 1;
					if(curnum > i)
					{
						//后面数据往前面移动
						memcpy(&FlashDisposeBuf[1+i*8],&FlashDisposeBuf[1+curnum*8],8);
						memset(&FlashDisposeBuf[1+curnum*8],0xff,8);
					}
				}
			}
		}
		FlashDisposeBuf[0] = curnum;
	}
	if(ifwriter)
	{
		APP_RWCardWl(FLASH_ORDER_WRITE,FlashDisposeBuf,sizeof(FlashDisposeBuf));
	}
	return RT_TRUE;
}


/*****************************************************************************
* Function     : APP_GetAPSta
* Description  : 获取安培快充启动方式
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   APP_GetAPStartType(uint8_t gun)
{
	if(gun >= GUN_MAX)
	{
		return _4G_APP_START;
	}
	return (uint8_t)APStartType[gun];
}


/*****************************************************************************
* Function     : APP_SetAPStartType
* Description  : 设置安培快充启动方式
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   APP_SetAPStartType(uint8_t gun ,_4G_START_TYPE  type)
{
	if((type >=  _4G_APP_MAX) || (gun >= GUN_MAX))
	{
		return RT_FALSE;
	}
	
	APStartType[gun] = type;
	return RT_TRUE;
}


/*****************************************************************************
* Function     : HY_SendRateAck
* Description  : 费率设置应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   AP_SendRateAck(uint8_t cmd)
{
	if(cmd == 1)
	{
		AP_SendSetPricB2Ack();
	}
	else
	{
		AP_SendSetPricB47Ack();
	}
	return RT_TRUE;
}

/*****************************************************************************
* Function     : APP_GetAPBatchNum
* Description  : 获取交易流水号
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
******************************************************************************/
uint8_t *  APP_GetAPBatchNum(uint8_t gun)
{
	static uint8_t buf[16];		//交易流水号位16个字节，ASICC 因此取后16个数字

	if(gun >= GUN_MAX)
	{
		return NULL;
	}
	memcpy(buf,APRecvStartCmd[gun].DrderNum,16);		//订单号的后16位唯一
	return buf;
}

/*****************************************************************************
* Function     : APP_GetAPQGNetMoney
* Description  :获取账户余额
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
******************************************************************************/
uint32_t APP_GetAPQGNetMoney(uint8_t gun)
{
	if(gun >= GUN_MAX)
	{
		return 0;
	}
	return 0; //目前全部返回0
}

/*****************************************************************************
* Function     : HY_SendFrameDispose
* Description  : 汇誉接收帧处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   AP_SendFrameDispose(void)
{
	uint8_t i;

	for(i = 0;i < AP_SEND_FRAME_LEN;i++)
	{
		if(APSendTable[i].cycletime == 0)
		{
			continue;
		}
		APSendTable[i].curtime = rt_tick_get();
		if((APSendTable[i].curtime >= APSendTable[i].lasttime) ? ((APSendTable[i].curtime - APSendTable[i].lasttime) >= APSendTable[i].cycletime) : \
		((APSendTable[i].curtime + (0xFFFFFFFFu - APSendTable[i].lasttime)) >= APSendTable[i].cycletime))
		{
			APSendTable[i].lasttime = APSendTable[i].curtime;
			if(APSendTable[i].Sendfunc != NULL)
			{
				APSendTable[i].Sendfunc();
			}
		}
		
	}
	return RT_TRUE;
}


/*****************************************************************************
* Function     : AP_SendRegister
* Description  : 注册 (协议选择)
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_SendRegister(void)
{
	uint8_t Sendbuf[20];
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	if(APP_GetAppRegisterState(LINK_NUM) == STATE_OK)	//显示已经注册成功了
	{
		return  RT_FALSE;
	}
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x01;	//协议
	
	//设备编号
	//8字节终端号(BCD)
	Sendbuf[9] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	Sendbuf[8] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	Sendbuf[7] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	Sendbuf[6] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	Sendbuf[5] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	Sendbuf[4] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	Sendbuf[3] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	Sendbuf[2] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	return ModuleSIM7600_SendData(0, Sendbuf,12); //发送数据
}

/*****************************************************************************
* Function     : AP_SendStartApp
* Description  : 用于启动应用传输层
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendStartApp(void)
{
	if(APP_GetAppRegisterState(LINK_NUM) == STATE_OK)	//显示已经注册成功了
	{
		return  RT_FALSE;
	}
	uint8_t Sendbuf[20];
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x04;
	Sendbuf[2] = 0x07;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	return ModuleSIM7600_SendData(0, Sendbuf,6); //发送数据
}


/*****************************************************************************
* Function     : AP_SendHear
* Description  : 发送心跳 30s发送一次
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendHear(void)
{
	uint8_t Sendbuf[20];
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x04;
	Sendbuf[2] = 0x43;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	return ModuleSIM7600_SendData(0, Sendbuf,6); //发送数据
}

/*****************************************************************************
* Function     : AP_SendChargeInfo
* Description  : 上报频率，充电中是30秒，待机2分钟；
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendChargeInfoA(void)
{
	static uint8_t laststate = 2;
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	//memset(APSendB1[GUN_A].devnum,0,sizeof(AP_send_B1));
	if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)	//没有注册成功
	{
		return  RT_FALSE;
	}
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0xA6;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x86;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	//设备编号
	APSendB1[GUN_A].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendB1[GUN_A].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendB1[GUN_A].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendB1[GUN_A].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendB1[GUN_A].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendB1[GUN_A].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendB1[GUN_A].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendB1[GUN_A].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	

	APSendB1[GUN_A].gun_index = 0;
	if(stChTcb[GUN_A].ucState == INSERT)
	{
		APSendB1[GUN_A].gunstate = 0;
	}
	else
	{
		APSendB1[GUN_A].gunstate = 1;
	}
	if(stChTcb[GUN_A].ucState == CHARGING)
	{
		APSendB1[GUN_A].work_stat = 3;
	}else if(stChTcb[GUN_A].ucState == WAIT_CAR_READY)
	{
		APSendB1[GUN_A].work_stat = 8;
	}else if(stChTcb[GUN_A].ucState == CHARGER_FAULT)
	{
		APSendB1[GUN_A].work_stat = 1; //故障
	}else
	{
		APSendB1[GUN_A].work_stat = 2; //待机
	}
	//具体故障原因
	memset(APSendB1[GUN_A].Allfail,0,sizeof(APSendB1[GUN_A].Allfail));
	if(stChTcb[GUN_A].stIOFault.uiFaultFlg & ((0x00000001) << INPUT_YX3_EM))
	{
		APSendB1[GUN_A].Allfail[3] = 1;	//急停
	}
	else if( (ch_is_over_volt(GUN_A,&stChTcb[GUN_A])) ||   (ch_is_under_volt(GUN_A,&stChTcb[GUN_A])))
	{
		APSendB1[GUN_A].Allfail[6] = 1;	//交流输入异常
	}
	else if(ch_is_over_current(GUN_A,&stChTcb[GUN_A]))
	{
		APSendB1[GUN_A].Allfail[11] = 1;	//输出过流告警
	}
	else
	{
		__NOP;
	}
	
	if(stChTcb[GUN_A].ucState == CHARGING)
	{
		if(DisSysConfigInfo.energymeter == 1)
		{
		APSendB1[GUN_A].dc_charge_voltage = stChTcb[GUN_A].stRn8209.usVolt;			//电压
		APSendB1[GUN_A].dc_charge_current  = stChTcb[GUN_A].stRn8209.usCurrent;	//电流  精确到小数点后二位
		}
		else
		{
			APSendB1[GUN_A].dc_charge_voltage = dlt645_info[GUN_A].out_vol * 10;
			APSendB1[GUN_A].dc_charge_current = dlt645_info[GUN_A].out_cur* 100;
		}

		APSendB1[GUN_A].ChargeKwh = stChTcb[GUN_A].stCh.uiChargeEnergy / 10;  
		APSendB1[GUN_A].ChargeJKwh = (stChTcb[GUN_A].stCh.uiaChargeEnergy[0]*1000) / (stChRate.Prices[0] + stChRate.fwPrices[0]);
		APSendB1[GUN_A].ChargeFKwh = (stChTcb[GUN_A].stCh.uiaChargeEnergy[1]*1000) / (stChRate.Prices[1] + stChRate.fwPrices[1]);
		APSendB1[GUN_A].ChargePKwh = (stChTcb[GUN_A].stCh.uiaChargeEnergy[2]*1000) / (stChRate.Prices[2] + stChRate.fwPrices[2]);
		APSendB1[GUN_A].ChargeGKwh = (stChTcb[GUN_A].stCh.uiaChargeEnergy[3]*1000) / (stChRate.Prices[3] + stChRate.fwPrices[3]);
		APSendB1[GUN_A].ChargeTime = (time(RT_NULL) - stChTcb[GUN_A].stCh.timestart) / 60;		//累计充电时间	单位：min
		APSendB1[GUN_A].ChargePowerMoney = stChTcb[GUN_A].stCh.uiAllEnergy/100;
		APSendB1[GUN_A].ChargeFWMoney = (stChTcb[GUN_A].stCh.uifwChargeEnergy[0] + stChTcb[GUN_A].stCh.uifwChargeEnergy[1] + stChTcb[GUN_A].stCh.uifwChargeEnergy[2]  \
		+ stChTcb[GUN_A].stCh.uifwChargeEnergy[3])/100;
		//	uint8_t OrderNum;					//本次充电的订单号，非充电中时清空	
		if(APStartType[GUN_A] == _4G_APP_START)
		{

			memcpy(APSendB1[GUN_A].OrderNum,APRecvStartCmd[GUN_A].DrderNum,sizeof(APSendB1[GUN_A].OrderNum));
		}
		else
		{
			memcpy(APSendB1[GUN_A].OrderNum,APRecvCardVinStart[GUN_A].DrderNum,sizeof(APRecvCardVinStart[GUN_A].DrderNum));
		}

	}
	memcpy(&Sendbuf[11],(uint8_t*)&APSendB1[GUN_A],sizeof(AP_send_B1));

	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_send_B1) +11); //发送数据*/
}

/*****************************************************************************
* Function     : AP_SendChargeInfo
* Description  : 上报频率，充电中是30秒，待机2分钟；
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendChargeInfoB(void)
{
	static uint8_t laststate = 2;
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	//memset(APSendB1[GUN_A].devnum,0,sizeof(AP_send_B1));
	if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)	//没有注册成功
	{
		return  RT_FALSE;
	}
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0xA6;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x86;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	//设备编号
	APSendB1[GUN_B].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendB1[GUN_B].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendB1[GUN_B].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendB1[GUN_B].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendB1[GUN_B].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendB1[GUN_B].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendB1[GUN_B].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendB1[GUN_B].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	

	APSendB1[GUN_B].gun_index = 1;
	if(stChTcb[GUN_B].ucState == INSERT)
	{
		APSendB1[GUN_B].gunstate = 0;
	}
	else
	{
		APSendB1[GUN_B].gunstate = 1;
	}
	if(stChTcb[GUN_B].ucState == CHARGING)
	{
		APSendB1[GUN_B].work_stat = 3;
	}else if(stChTcb[GUN_B].ucState == WAIT_CAR_READY)
	{
		APSendB1[GUN_B].work_stat = 8;
	}else if(stChTcb[GUN_B].ucState == CHARGER_FAULT)
	{
		APSendB1[GUN_B].work_stat = 1; //故障
	}else
	{
		APSendB1[GUN_B].work_stat = 2; //待机
	}
	//具体故障原因
	memset(APSendB1[GUN_B].Allfail,0,sizeof(APSendB1[GUN_B].Allfail));
	if(stChTcb[GUN_B].stIOFault.uiFaultFlg & ((0x00000001) << INPUT_YX3_EM))
	{
		APSendB1[GUN_B].Allfail[3] = 1;	//急停
	}
	else if( (ch_is_over_volt(GUN_B,&stChTcb[GUN_B])) ||   (ch_is_under_volt(GUN_B,&stChTcb[GUN_B])))
	{
		APSendB1[GUN_B].Allfail[6] = 1;	//交流输入异常
	}
	else if(ch_is_over_current(GUN_B,&stChTcb[GUN_B]))
	{
		APSendB1[GUN_B].Allfail[11] = 1;	//输出过流告警
	}
	else
	{
		__NOP;
	}
	
	if(stChTcb[GUN_B].ucState == CHARGING)
	{
		if(DisSysConfigInfo.energymeter == 1)
		{
		APSendB1[GUN_B].dc_charge_voltage = stChTcb[GUN_B].stRn8209.usVolt;			//电压
		APSendB1[GUN_B].dc_charge_current  = stChTcb[GUN_B].stRn8209.usCurrent;	//电流  精确到小数点后二位
		}
		else
		{
			APSendB1[GUN_B].dc_charge_voltage = dlt645_info[GUN_B].out_vol * 10;
			APSendB1[GUN_B].dc_charge_current = dlt645_info[GUN_B].out_cur* 100;
		}

		APSendB1[GUN_B].ChargeKwh = stChTcb[GUN_B].stCh.uiChargeEnergy / 10;  
		APSendB1[GUN_B].ChargeJKwh = (stChTcb[GUN_B].stCh.uiaChargeEnergy[0]*1000) / (stChRate.Prices[0] + stChRate.fwPrices[0]);
		APSendB1[GUN_B].ChargeFKwh = (stChTcb[GUN_B].stCh.uiaChargeEnergy[1]*1000) / (stChRate.Prices[1] + stChRate.fwPrices[1]);
		APSendB1[GUN_B].ChargePKwh = (stChTcb[GUN_B].stCh.uiaChargeEnergy[2]*1000) / (stChRate.Prices[2] + stChRate.fwPrices[2]);
		APSendB1[GUN_B].ChargeGKwh = (stChTcb[GUN_B].stCh.uiaChargeEnergy[3]*1000) / (stChRate.Prices[3] + stChRate.fwPrices[3]);
		APSendB1[GUN_B].ChargeTime = (time(RT_NULL) - stChTcb[GUN_B].stCh.timestart) / 60;		//累计充电时间	单位：min
		APSendB1[GUN_B].ChargePowerMoney = stChTcb[GUN_B].stCh.uiAllEnergy/100;
		APSendB1[GUN_B].ChargeFWMoney = (stChTcb[GUN_B].stCh.uifwChargeEnergy[0] + stChTcb[GUN_B].stCh.uifwChargeEnergy[1] + stChTcb[GUN_B].stCh.uifwChargeEnergy[2]  \
		+ stChTcb[GUN_B].stCh.uifwChargeEnergy[3])/100;
		//	uint8_t OrderNum;					//本次充电的订单号，非充电中时清空	
		if(APStartType[GUN_B] == _4G_APP_START)
		{

			memcpy(APSendB1[GUN_B].OrderNum,APRecvStartCmd[GUN_B].DrderNum,sizeof(APSendB1[GUN_B].OrderNum));
		}
		else
		{
			memcpy(APSendB1[GUN_B].OrderNum,APRecvCardVinStart[GUN_B].DrderNum,sizeof(APRecvCardVinStart[GUN_B].DrderNum));
		}

	}
	memcpy(&Sendbuf[11],(uint8_t*)&APSendB1[GUN_B],sizeof(AP_send_B1));

	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_send_B1) +11); //发送数据*/
}


/*****************************************************************************
* Function     : AP_SendCarInfo
* Description  : 充电过程中上报频率30秒
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendCarInfoA(void)
{
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	memset(APSendB37[GUN_A].devnum,0,sizeof(AP_SEND_B37));
	
	if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)	//没有注册成功
	{
		return  RT_FALSE;
	}
	if(stChTcb[GUN_A].ucState == CHARGING)
	{
		return RT_TRUE;		//不在充电中不发送
	}
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x6C;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x1E;
	//设备编号
	APSendB37[GUN_A].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendB37[GUN_A].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendB37[GUN_A].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendB37[GUN_A].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendB37[GUN_A].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendB37[GUN_A].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendB37[GUN_A].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendB37[GUN_A].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendB37[GUN_A].gun_index = 0x00;
	memcpy(&Sendbuf[15],(uint8_t*)&APSendB37[GUN_A],sizeof(AP_SEND_B37));

	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_SEND_B37) +15); //发送数据
	
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_SendCarInfo
* Description  : 充电过程中上报频率30秒
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendCarInfoB(void)
{
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	memset(APSendB37[GUN_B].devnum,0,sizeof(AP_SEND_B37));
	
	if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)	//没有注册成功
	{
		return  RT_FALSE;
	}
	if(stChTcb[GUN_B].ucState == CHARGING)
	{
		return RT_TRUE;		//不在充电中不发送
	}
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x6C;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x1E;
	//设备编号
	APSendB37[GUN_B].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendB37[GUN_B].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendB37[GUN_B].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendB37[GUN_B].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendB37[GUN_B].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendB37[GUN_B].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendB37[GUN_B].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendB37[GUN_B].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendB37[GUN_B].gun_index = 0x01;
	memcpy(&Sendbuf[15],(uint8_t*)&APSendB37[GUN_B],sizeof(AP_SEND_B37));

	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_SEND_B37) +15); //发送数据
	
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_SendCardInfo
* Description  :
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendCardInfo(_GUN_NUM gun)
{
	uint8_t Sendbuf[200] = {0};
	uint32_t CardNum;
	uint8_t * pdevnum = DisSysConfigInfo.DevNum; 

	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	CardNum = (m1_card_info.uidByte[0]) | (m1_card_info.uidByte[1] << 8) |\
				(m1_card_info.uidByte[2] << 16) | (m1_card_info.uidByte[3] << 24);
	memset(APSendB6[gun].devnum,0,sizeof(AP_send_B6));
	
	//685200000000820006000000000001
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x52;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x01;
	APSendB6[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendB6[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendB6[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendB6[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendB6[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendB6[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendB6[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendB6[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendB6[gun].gun_index = gun;
	//卡号需要反过来？ 20210623
	APSendB6[gun].CardNum[7] = HEXtoBCD(CardNum / 100000000);
	APSendB6[gun].CardNum[6] = HEXtoBCD(CardNum % 100000000 /1000000);
	APSendB6[gun].CardNum[5] = HEXtoBCD(CardNum % 1000000 /10000);
	APSendB6[gun].CardNum[4] = HEXtoBCD(CardNum % 10000 /100);
	APSendB6[gun].CardNum[3] = HEXtoBCD(CardNum % 100 /1);
	memcpy(&Sendbuf[15],(uint8_t*)&APSendB6[gun],sizeof(AP_send_B6));
	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_send_B6) +15); //发送数据
}

/*****************************************************************************
* Function     : AP_SendVinInfo
* Description  :
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendVinInfo(_GUN_NUM gun)
{
	
	return RT_TRUE;
}


/*****************************************************************************
* Function     : AP_SendCardVinStart
* Description  :刷卡Vin启动
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendCardVinStart(_GUN_NUM gun)
{
	uint8_t Sendbuf[200] = {0};
	uint32_t CardNum;
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	memset(APSendB10[gun].devnum,0,sizeof(AP_send_B10));
	if(APStartType[gun] == _4G_APP_CARD)
	{
		CardNum = (m1_card_info.uidByte[0]) | (m1_card_info.uidByte[1] << 8) |\
				(m1_card_info.uidByte[2] << 16) | (m1_card_info.uidByte[3] << 24);
	}
	//68570000000082000600000000000E
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x57;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x0E;
	APSendB10[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendB10[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendB10[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendB10[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendB10[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendB10[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendB10[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendB10[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendB10[gun].gun_index = gun;
	//卡号需要反过来？ 20210623
	if(APStartType[gun] == _4G_APP_CARD)
	{
		APSendB10[gun].CardNum[7] = HEXtoBCD(CardNum / 100000000);
		APSendB10[gun].CardNum[6] = HEXtoBCD(CardNum % 100000000 /1000000);
		APSendB10[gun].CardNum[5] = HEXtoBCD(CardNum % 1000000 /10000);
		APSendB10[gun].CardNum[4] = HEXtoBCD(CardNum % 10000 /100);
		APSendB10[gun].CardNum[3] = HEXtoBCD(CardNum % 100 /1);
	}
	APSendB10[gun].ChargeType = 0x04;			//自动充满
	memcpy(&Sendbuf[15],(uint8_t*)&APSendB10[gun],sizeof(AP_send_B10));
	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_send_B10) +15); //发送数据
	
}


/*****************************************************************************
* Function     : AP_SendStartAck
* Description  : 发送启动应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendStartAck(_GUN_NUM gun)
{
	
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;

	if(gun >= GUN_MAX)
	{
		return  RT_FALSE;
	}
	
	if(APStartType[gun] != _4G_APP_START)
	{
		return RT_TRUE;			//卡启动不用发送启动应答
	}
	
	memset(APSendStartAckB5[gun].devnum,0,sizeof(AP_SEND_B5));
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x31;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x85;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x15;
	APSendStartAckB5[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendStartAckB5[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendStartAckB5[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendStartAckB5[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendStartAckB5[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendStartAckB5[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendStartAckB5[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendStartAckB5[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendStartAckB5[gun].gun_index = gun;
	
	if(stChTcb[gun].ucState == CHARGING)
	{
		APSendStartAckB5[gun].ifsuccess = 0;  //成功标志 0：成功，1：失败
		APSendStartAckB5[gun].failcause[1] = 0x00;  //启动充电失败原因   0000：成功，0001：正在充电中，0002：系统故障， 0003：其他原因
	}
	else
	{
		APSendStartAckB5[gun].ifsuccess = 1;  //成功标志 0：成功，1：失败
		APSendStartAckB5[gun].failcause[1] = 0x03;  //启动充电失败原因   0000：成功，0001：正在充电中，0002：系统故障， 0003：其他原因
	}
	APSendStartAckB5[gun].Cmd = 0x01;   //控制命令    0：停止充电，1：启动充电，2：定时充电启动，3：预约充电启动
	
	APSendStartAckB5[gun].CmdTime.Second = (gs_SysTime.ucSec) * 1000;
	APSendStartAckB5[gun].CmdTime.Day = (gs_SysTime.ucDay);
	APSendStartAckB5[gun].CmdTime.Hour = (gs_SysTime.ucHour);
	APSendStartAckB5[gun].CmdTime.Minute = (gs_SysTime.ucMin);
	APSendStartAckB5[gun].CmdTime.Month = (gs_SysTime.ucMonth);
	APSendStartAckB5[gun].CmdTime.Year = (gs_SysTime.ucYear - 100);;
	memcpy(APSendStartAckB5[gun].DrderNum,APRecvStartCmd[gun].DrderNum,16);
	
	memcpy(&Sendbuf[15],(uint8_t*)&APSendStartAckB5[gun],sizeof(AP_SEND_B5));
	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_SEND_B5) +15); //发送数据
	
}


/*****************************************************************************
* Function     : AP_SendStopAck
* Description  : 发送停止应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendStopAck(_GUN_NUM gun)
{
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;

	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	} 
	memset(APSendStopAckB5[gun].devnum,0,sizeof(AP_SEND_B5));
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0x31;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x85;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x15;
	APSendStopAckB5[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendStopAckB5[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendStopAckB5[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendStopAckB5[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendStopAckB5[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendStopAckB5[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendStopAckB5[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendStopAckB5[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendStopAckB5[gun].gun_index = gun;

	APSendStopAckB5[gun].ifsuccess = 0;  //成功标志 0：成功，1：失败
	APSendStopAckB5[gun].failcause[1] = 0x00;  //启动充电失败原因   0000：成功，0001：正在充电中，0002：系统故障， 0003：其他原因
	APSendStopAckB5[gun].Cmd = 0x00;   //控制命令    0：停止充电，1：启动充电，2：定时充电启动，3：预约充电启动
	APSendStopAckB5[gun].CmdTime.Second = (gs_SysTime.ucSec) * 1000;
	APSendStopAckB5[gun].CmdTime.Day = (gs_SysTime.ucDay);
	APSendStopAckB5[gun].CmdTime.Hour = (gs_SysTime.ucHour);
	APSendStopAckB5[gun].CmdTime.Minute = (gs_SysTime.ucMin);
	APSendStopAckB5[gun].CmdTime.Month = (gs_SysTime.ucMonth);
	APSendStopAckB5[gun].CmdTime.Year = (gs_SysTime.ucYear - 100);;
	memcpy(APSendStopAckB5[gun].DrderNum,APRecvStartCmd[gun].DrderNum,16);
	
	memcpy(&Sendbuf[15],(uint8_t*)&APSendStopAckB5[gun],sizeof(AP_SEND_B5));
	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_SEND_B5) +15); //发送数据
	
}

/*****************************************************************************
* Function     : AP_SendOnlineBill
* Description  : 发送在线交易记录
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendBill(_GUN_NUM gun)
{
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	uint32_t ChargeFee;
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	
	memset(APSendBilB12[gun].devnum,0,sizeof(AP_SEND_B12));
	
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0xB1;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x02;
	APSendBilB12[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendBilB12[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendBilB12[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendBilB12[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendBilB12[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendBilB12[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendBilB12[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendBilB12[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendBilB12[gun].gun_index = gun;
	if(APStartType[gun] == _4G_APP_START)
	{
		//memcpy(APSendBilB12[gun].CardNum,APRecvStartCmd[gun].UserNum,sizeof(APRecvStartCmd[gun].UserNum));
		memcpy(APSendBilB12[gun].DrderNum,APRecvStartCmd[gun].DrderNum,sizeof(APRecvStartCmd[gun].UserNum));
		//memcpy(APSendBilB12[gun].CardVin,CcuStartOverContext[GUN_A].vin,17);
	}
	else
	{
		//卡启动
		memcpy(APSendBilB12[gun].CardNum,APRecvCardCmd[gun].CardNum,sizeof(APRecvCardCmd[gun].CardNum));
		memcpy(APSendBilB12[gun].DrderNum,APRecvCardVinStart[gun].DrderNum,sizeof(APRecvCardVinStart[gun].DrderNum));
		//memcpy(APSendBilB12[gun].CardVin,CcuStartOverContext[GUN_A].vin,17);
	}

		APSendBilB12[gun].StartTime.Second = (stChTcb[gun].stCh.uiChStartTime.ucSec) * 1000;
		APSendBilB12[gun].StartTime.Day = (stChTcb[gun].stCh.uiChStartTime.ucDay);
		APSendBilB12[gun].StartTime.Hour = (stChTcb[gun].stCh.uiChStartTime.ucHour);
		APSendBilB12[gun].StartTime.Minute = (stChTcb[gun].stCh.uiChStartTime.ucMin);
		APSendBilB12[gun].StartTime.Month = (stChTcb[gun].stCh.uiChStartTime.ucMonth);
		APSendBilB12[gun].StartTime.Year = (stChTcb[gun].stCh.uiChStartTime.ucYear - 100);
		
		APSendBilB12[gun].StopTime.Second = (stChTcb[gun].stCh.uiChStoptTime.ucSec) * 1000;
		APSendBilB12[gun].StopTime.Day = (stChTcb[gun].stCh.uiChStoptTime.ucDay);
		APSendBilB12[gun].StopTime.Hour = (stChTcb[gun].stCh.uiChStoptTime.ucHour);
		APSendBilB12[gun].StopTime.Minute = (stChTcb[gun].stCh.uiChStoptTime.ucMin);
		APSendBilB12[gun].StopTime.Month = (stChTcb[gun].stCh.uiChStoptTime.ucMonth);
		APSendBilB12[gun].StopTime.Year = (stChTcb[gun].stCh.uiChStoptTime.ucYear - 100);
	
		APSendBilB12[gun].ChargeTime = (stChTcb[gun].stCh.timestop - stChTcb[gun].stCh.timestart) /60;
	
		APSendBilB12[gun].ChargeAllKwh = stChTcb[gun].stCh.uiChargeEnergy / 10;  
		APSendBilB12[gun].ChargeJKwh = (stChTcb[gun].stCh.uiaChargeEnergy[0]*1000) / (stChRate.Prices[0] + stChRate.fwPrices[0]);
		APSendBilB12[gun].ChargeFKwh = (stChTcb[gun].stCh.uiaChargeEnergy[1]*1000) / (stChRate.Prices[1] + stChRate.fwPrices[1]);
		APSendBilB12[gun].ChargePKwh = (stChTcb[gun].stCh.uiaChargeEnergy[2]*1000) / (stChRate.Prices[2] + stChRate.fwPrices[2]);
		APSendBilB12[gun].ChargeGKwh = (stChTcb[gun].stCh.uiaChargeEnergy[3]*1000) / (stChRate.Prices[3] + stChRate.fwPrices[3]);

		if(DisSysConfigInfo.energymeter == 1)
		{
			APSendBilB12[gun].ChargeStartKwh = (stChTcb[gun].stRn8209.uiE - stChTcb[gun].stCh.uiChargeEnergy) / 10; //充电前电表读数
			APSendBilB12[gun].ChargeStopKwh =stChTcb[gun].stRn8209.uiE / 10;  ;
		}
		else
		{
			 APSendBilB12[gun].ChargeStartKwh = ((dlt645_info[gun].cur_hwh * 1000) - stChTcb[gun].stCh.uiChargeEnergy )/ 10;
			 APSendBilB12[gun].ChargeStopKwh = dlt645_info[gun].cur_hwh* 100;
		}

		if((stChTcb[gun].stCh.reason == NO_CURRENT) || (stChTcb[gun].stCh.reason == NO_CURRENT_9V) || (stChTcb[gun].stCh.reason == STOP_CHARGEFULL) )
		{
			APSendBilB12[gun].StopReason  = 1; //充满停止
		}
		else if((stChTcb[gun].stCh.reason == E_END_BY_APP) || (stChTcb[gun].stCh.reason == E_END_BY_CARD))
		{
			APSendBilB12[gun].StopReason  = 3; 	//主动停止
		}
		else if(stChTcb[gun].stCh.reason == EM_STOP)
		{
			APSendBilB12[gun].StopReason  = 4;		//急停
		}
		else
		{
			APSendBilB12[gun].StopReason  = 6;		//其他原因
		}
		

		APSendBilB12[gun].ServeFee = (stChTcb[gun].stCh.uifwChargeEnergy[0] + stChTcb[gun].stCh.uifwChargeEnergy[1] + stChTcb[gun].stCh.uifwChargeEnergy[2]  \
		+ stChTcb[gun].stCh.uifwChargeEnergy[3])/100;
		ChargeFee = (stChTcb[gun].stCh.uiAllEnergy/100) - ((stChTcb[gun].stCh.uifwChargeEnergy[0] + stChTcb[gun].stCh.uifwChargeEnergy[1] + stChTcb[gun].stCh.uifwChargeEnergy[2]  \
		+ stChTcb[gun].stCh.uifwChargeEnergy[3])/100);
		if(ChargeFee > 100000)
		{
			//大于1000元保护
			ChargeFee  = 0;
		}
		APSendBilB12[gun].ChargeFee =ChargeFee;

	memcpy(&Sendbuf[15],(uint8_t*)&APSendBilB12[gun],sizeof(AP_SEND_B12));
	
	WriterFmBill(gun,1);
	ResendBillControl[gun].CurTime = rt_tick_get();	
	ResendBillControl[gun].CurTime = ResendBillControl[gun].LastTime;
	return ModuleSIM7600_SendData(0, Sendbuf,sizeof(AP_SEND_B12) +15); //发送数据
	
}


/*****************************************************************************
* Function     : PreAPBill
* Description  : 保存安培快充订单
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   PreAPBill(_GUN_NUM gun,uint8_t *pdata)
{
	uint8_t Sendbuf[200] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	uint32_t ChargeFee; 
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	
	memset(APSendBilB12[gun].devnum,0,sizeof(AP_SEND_B12));
	
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0xB1;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x02;
	APSendBilB12[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendBilB12[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendBilB12[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendBilB12[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendBilB12[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendBilB12[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendBilB12[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendBilB12[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendBilB12[gun].gun_index = gun;
	if(APStartType[gun] == _4G_APP_START)
	{
		//memcpy(APSendBilB12[gun].CardNum,APRecvStartCmd[gun].UserNum,sizeof(APRecvStartCmd[gun].UserNum));
		memcpy(APSendBilB12[gun].DrderNum,APRecvStartCmd[gun].DrderNum,sizeof(APRecvStartCmd[gun].UserNum));
		//memcpy(APSendBilB12[gun].CardVin,CcuStartOverContext[GUN_A].vin,17);
	}
	else 
	{
		//卡启动
		memcpy(APSendBilB12[gun].CardNum,APRecvCardCmd[gun].CardNum,sizeof(APRecvCardCmd[gun].CardNum));
		memcpy(APSendBilB12[gun].DrderNum,APRecvCardVinStart[gun].DrderNum,sizeof(APRecvCardVinStart[gun].DrderNum));
		//memcpy(APSendBilB12[gun].CardVin,CcuStartOverContext[GUN_A].vin,17);
	}

	APSendBilB12[gun].StartTime.Second = (stChTcb[gun].stCh.uiChStartTime.ucSec) * 1000;
	APSendBilB12[gun].StartTime.Day = (stChTcb[gun].stCh.uiChStartTime.ucDay);
	APSendBilB12[gun].StartTime.Hour = (stChTcb[gun].stCh.uiChStartTime.ucHour);
	APSendBilB12[gun].StartTime.Minute = (stChTcb[gun].stCh.uiChStartTime.ucMin);
	APSendBilB12[gun].StartTime.Month = (stChTcb[gun].stCh.uiChStartTime.ucMonth);
	APSendBilB12[gun].StartTime.Year = (stChTcb[gun].stCh.uiChStartTime.ucYear - 100);
	
	APSendBilB12[gun].StopTime.Second = (stChTcb[gun].stCh.uiChStoptTime.ucSec) * 1000;
	APSendBilB12[gun].StopTime.Day = (stChTcb[gun].stCh.uiChStoptTime.ucDay);
	APSendBilB12[gun].StopTime.Hour = (stChTcb[gun].stCh.uiChStoptTime.ucHour);
	APSendBilB12[gun].StopTime.Minute = (stChTcb[gun].stCh.uiChStoptTime.ucMin);
	APSendBilB12[gun].StopTime.Month = (stChTcb[gun].stCh.uiChStoptTime.ucMonth);
	APSendBilB12[gun].StopTime.Year = (stChTcb[gun].stCh.uiChStoptTime.ucYear - 100);

	APSendBilB12[gun].ChargeTime = (stChTcb[gun].stCh.timestop - stChTcb[gun].stCh.timestart) /60;

	APSendBilB12[gun].ChargeAllKwh = stChTcb[gun].stCh.uiChargeEnergy / 10;  
	APSendBilB12[gun].ChargeJKwh = (stChTcb[gun].stCh.uiaChargeEnergy[0]*1000) / (stChRate.Prices[0] + stChRate.fwPrices[0]);
	APSendBilB12[gun].ChargeFKwh = (stChTcb[gun].stCh.uiaChargeEnergy[1]*1000) / (stChRate.Prices[1] + stChRate.fwPrices[1]);
	APSendBilB12[gun].ChargePKwh = (stChTcb[gun].stCh.uiaChargeEnergy[2]*1000) / (stChRate.Prices[2] + stChRate.fwPrices[2]);
	APSendBilB12[gun].ChargeGKwh = (stChTcb[gun].stCh.uiaChargeEnergy[3]*1000) / (stChRate.Prices[3] + stChRate.fwPrices[3]);


	if(DisSysConfigInfo.energymeter == 1)
	{
		APSendBilB12[gun].ChargeStartKwh = (stChTcb[gun].stRn8209.uiE - stChTcb[gun].stCh.uiChargeEnergy) / 10; //充电前电表读数
		APSendBilB12[gun].ChargeStopKwh =stChTcb[gun].stRn8209.uiE / 10;  ;
	}
	else
	{
	 APSendBilB12[gun].ChargeStartKwh = ((dlt645_info[gun].cur_hwh * 1000) - stChTcb[gun].stCh.uiChargeEnergy )/ 10;
	 APSendBilB12[gun].ChargeStopKwh = dlt645_info[gun].cur_hwh* 100;
	}

	if((stChTcb[gun].stCh.reason == NO_CURRENT) || (stChTcb[gun].stCh.reason == NO_CURRENT_9V) || (stChTcb[gun].stCh.reason == STOP_CHARGEFULL) )
	{
		APSendBilB12[gun].StopReason  = 1; //充满停止
	}
	else if((stChTcb[gun].stCh.reason == E_END_BY_APP) || (stChTcb[gun].stCh.reason == E_END_BY_CARD))
	{
		APSendBilB12[gun].StopReason  = 3; 	//主动停止
	}
	else if(stChTcb[gun].stCh.reason == EM_STOP)
	{
		APSendBilB12[gun].StopReason  = 4;		//急停
	}
	else
	{
		APSendBilB12[gun].StopReason  = 6;		//其他原因
	}
	

	APSendBilB12[gun].ServeFee = (stChTcb[gun].stCh.uifwChargeEnergy[0] + stChTcb[gun].stCh.uifwChargeEnergy[1] + stChTcb[gun].stCh.uifwChargeEnergy[2]  \
	+ stChTcb[gun].stCh.uifwChargeEnergy[3])/100;
	ChargeFee = (stChTcb[gun].stCh.uiAllEnergy/100) - ((stChTcb[gun].stCh.uifwChargeEnergy[0] + stChTcb[gun].stCh.uifwChargeEnergy[1] + stChTcb[gun].stCh.uifwChargeEnergy[2]  \
	+ stChTcb[gun].stCh.uifwChargeEnergy[3])/100);
	if(ChargeFee > 100000)
	{
		//大于1000元保护
		ChargeFee  = 0;
	}
	APSendBilB12[gun].ChargeFee =ChargeFee;

	memcpy(&Sendbuf[15],(uint8_t*)&APSendBilB12[gun],sizeof(AP_SEND_B12));
	memcpy(pdata,Sendbuf,sizeof(AP_SEND_B12) + 15);
	
	return RT_TRUE;
}


/*****************************************************************************
* Function     : PreAPOffLineBill
* Description  : 保存离线交易记录
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  PreAPOffLineBill(_GUN_NUM gun,uint8_t *pdata)
{
	uint8_t Sendbuf[300] = {0};
	uint8_t i = 0;
	uint32_t CardNum;
	uint8_t * pdevnum =DisSysConfigInfo.DevNum;
	uint32_t ChargeFee;
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	
	memset(APSendBilB15[gun].devnum,0,sizeof(AP_SEND_B15));
	CardNum = (m1_card_info.uidByte[0]) | (m1_card_info.uidByte[1] << 8) |\
				(m1_card_info.uidByte[2] << 16) | (m1_card_info.uidByte[3] << 24);
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 0xD3;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x03;
	APSendBilB15[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendBilB15[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendBilB15[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendBilB15[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendBilB15[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendBilB15[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendBilB15[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendBilB15[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendBilB15[gun].gun_index = gun;
	if(APStartType[gun] == _4G_APP_CARD)
	{
		//卡启动
		APSendBilB15[gun].CardNum[7] = HEXtoBCD(CardNum / 100000000);
		APSendBilB15[gun].CardNum[6] = HEXtoBCD(CardNum % 100000000 /1000000);
		APSendBilB15[gun].CardNum[5] = HEXtoBCD(CardNum % 1000000 /10000);
		APSendBilB15[gun].CardNum[4] = HEXtoBCD(CardNum % 10000 /100);
		APSendBilB15[gun].CardNum[3] = HEXtoBCD(CardNum % 100 /1);
		
		//memcpy(APSendBilB12[gun].CardVin,CcuStartOverContext[GUN_A].vin,17);
	}
	//生成一个唯一订单号
	
	//memcpy(APSendBilB15[gun].DrderNum,APRecvCardVinStart[gun].DrderNum,sizeof(APRecvCardVinStart[gun].DrderNum));
	APSendBilB15[gun].DrderNum[9] = 0x20;
	
		APSendBilB12[gun].StartTime.Second = (stChTcb[gun].stCh.uiChStartTime.ucSec) * 1000;
		APSendBilB12[gun].StartTime.Day = (stChTcb[gun].stCh.uiChStartTime.ucDay);
		APSendBilB12[gun].StartTime.Hour = (stChTcb[gun].stCh.uiChStartTime.ucHour);
		APSendBilB12[gun].StartTime.Minute = (stChTcb[gun].stCh.uiChStartTime.ucMin);
		APSendBilB12[gun].StartTime.Month = (stChTcb[gun].stCh.uiChStartTime.ucMonth);
		APSendBilB12[gun].StartTime.Year = (stChTcb[gun].stCh.uiChStartTime.ucYear - 100);
	
	APSendBilB15[gun].DrderNum[8] = stChTcb[gun].stCh.uiChStartTime.ucYear - 100;
	APSendBilB15[gun].DrderNum[7] = stChTcb[gun].stCh.uiChStartTime.ucMonth;
	APSendBilB15[gun].DrderNum[6] = stChTcb[gun].stCh.uiChStartTime.ucDay;
	APSendBilB15[gun].DrderNum[5] = stChTcb[gun].stCh.uiChStartTime.ucHour;
	APSendBilB15[gun].DrderNum[4] = stChTcb[gun].stCh.uiChStartTime.ucMin;
	APSendBilB15[gun].DrderNum[3] = stChTcb[gun].stCh.uiChStartTime.ucSec;
	APSendBilB15[gun].DrderNum[2] = stChTcb[gun].stCh.uiChStartTime.ucSec;
	APSendBilB15[gun].DrderNum[1] = stChTcb[gun].stCh.uiChStartTime.ucSec;
	APSendBilB15[gun].DrderNum[0] = stChTcb[gun].stCh.uiChStartTime.ucSec;
	
	
	
	APSendBilB15[gun].StopTime.Second = (stChTcb[gun].stCh.uiChStoptTime.ucSec) * 1000;
	APSendBilB15[gun].StopTime.Day = (stChTcb[gun].stCh.uiChStoptTime.ucDay);
	APSendBilB15[gun].StopTime.Hour = (stChTcb[gun].stCh.uiChStoptTime.ucHour);
	APSendBilB15[gun].StopTime.Minute = (stChTcb[gun].stCh.uiChStoptTime.ucMin);
	APSendBilB15[gun].StopTime.Month = (stChTcb[gun].stCh.uiChStoptTime.ucMonth);
	APSendBilB15[gun].StopTime.Year = (stChTcb[gun].stCh.uiChStoptTime.ucYear - 100);
	
	APSendBilB15[gun].ChargeTime = stChTcb[gun].stCh.timestop - stChTcb[gun].stCh.timestart;
	APSendBilB15[gun].ChargeAllKwh = stChTcb[gun].stCh.uiChargeEnergy / 10;   //小数点2位
	
	if(DisSysConfigInfo.energymeter == 1)
	{
	   APSendBilB15[gun].ChargeStartKwh = (stChTcb[gun].stRn8209.uiE > stChTcb[gun].stCh.uiChargeEnergy) ? \
	   (stChTcb[gun].stRn8209.uiE - stChTcb[gun].stCh.uiChargeEnergy): (stChTcb[gun].stRn8209.uiE + RN8209D_MAX_E - stChTcb[gun].stCh.uiChargeEnergy); //充电前电表读数
		APSendBilB15[gun].ChargeStopKwh = stChTcb[gun].stRn8209.uiE / 10;
	}
	else
	{
		 APSendBilB15[gun].ChargeStartKwh = ((dlt645_info[gun].cur_hwh * 1000) - stChTcb[gun].stCh.uiChargeEnergy )/ 10;
	}
	
	if((stChTcb[gun].stCh.reason == NO_CURRENT) || (stChTcb[gun].stCh.reason == NO_CURRENT_9V) || (stChTcb[gun].stCh.reason == STOP_CHARGEFULL) )
	{
		APSendBilB12[gun].StopReason  = 1; //充满停止
	}
	else if((stChTcb[gun].stCh.reason == E_END_BY_APP) || (stChTcb[gun].stCh.reason == E_END_BY_CARD))
	{
		APSendBilB12[gun].StopReason  = 3; 	//主动停止
	}
	else if(stChTcb[gun].stCh.reason == EM_STOP)
	{
		APSendBilB12[gun].StopReason  = 4;		//急停
	}
	else
	{
		APSendBilB12[gun].StopReason  = 6;		//其他原因
	}
	
	APSendBilB15[gun].ChargeJKwh = (stChTcb[gun].stCh.uiaChargeEnergy[0]*1000) / (stChRate.Prices[0] + stChRate.fwPrices[0]);
	APSendBilB15[gun].ChargeFKwh = (stChTcb[gun].stCh.uiaChargeEnergy[1]*1000) / (stChRate.Prices[1] + stChRate.fwPrices[1]);
	APSendBilB15[gun].ChargePKwh = (stChTcb[gun].stCh.uiaChargeEnergy[2]*1000) / (stChRate.Prices[2] + stChRate.fwPrices[2]);
	APSendBilB15[gun].ChargeGKwh = (stChTcb[gun].stCh.uiaChargeEnergy[3]*1000) / (stChRate.Prices[3] + stChRate.fwPrices[3]);
	
	APSendBilB15[gun].JMoney = stChTcb[gun].stCh.uiaChargeEnergy[0]/100;
	APSendBilB15[gun].FMoney = stChTcb[gun].stCh.uiaChargeEnergy[1]/100;
	APSendBilB15[gun].PMoney = stChTcb[gun].stCh.uiaChargeEnergy[2]/100;
	APSendBilB15[gun].GMoney = stChTcb[gun].stCh.uiaChargeEnergy[3]/100;
	
	uint16_t Price[TIME_PERIOD_MAX];				//电价 	0.01元 	    
	_TIME_QUANTUM CurTimeQuantum[TIME_PERIOD_MAX];		//当前处于哪个时间段  尖 峰 平 古
	

	APSendBilB15[gun].JBill = stChRate.Prices[0] + stChRate.fwPrices[0];
	APSendBilB15[gun].FBill = stChRate.Prices[1] + stChRate.fwPrices[1];
	APSendBilB15[gun].PBill = stChRate.Prices[2] + stChRate.fwPrices[2];
	APSendBilB15[gun].GBill = stChRate.Prices[3] + stChRate.fwPrices[3];
	
	
	APSendBilB15[gun].ServeFee = (stChTcb[gun].stCh.uifwChargeEnergy[0] + stChTcb[gun].stCh.uifwChargeEnergy[1] + \
	stChTcb[gun].stCh.uifwChargeEnergy[2] + stChTcb[gun].stCh.uifwChargeEnergy[3])/100;
	ChargeFee = (stChTcb[gun].stCh.uiDChQ/100) - ((stChTcb[gun].stCh.uifwChargeEnergy[0] + stChTcb[gun].stCh.uifwChargeEnergy[1] + stChTcb[gun].stCh.uifwChargeEnergy[2]  \
	+ stChTcb[gun].stCh.uifwChargeEnergy[3])/100);
	if(ChargeFee > 100000)
	{
		//大于1000元保护
		ChargeFee  = 0;
	}
	APSendBilB15[gun].ChargeFee = ChargeFee;
	memcpy(&Sendbuf[15],(uint8_t*)&APSendBilB15,sizeof(AP_SEND_B15));
	memcpy(pdata,Sendbuf,0xD5);
	return RT_TRUE;

}


/*****************************************************************************
* Function     : PreAPFSOffLineBill
* Description  : 上传分时交易记录   充电完成后，在B12发送完成后上报。
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  PreAPFSOffLineBill(_GUN_NUM gun,uint8_t *pdata)
{
	uint8_t Sendbuf[250] = {0};
	uint8_t QuantumNum = 0;
	uint8_t CurQuan;	
	uint8_t i;
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	memset(APSendBilB53[gun].devnum,0,sizeof(AP_SEND_B53));
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 54+sizeof(_PRIC_INFO)*stChRate.TimeQuantumNum;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x28;   //应该是B55
	APSendBilB53[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendBilB53[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendBilB53[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendBilB53[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendBilB53[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendBilB53[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendBilB53[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendBilB53[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendBilB53[gun].gun_index = gun;
	memcpy(APSendBilB53[gun].DrderNum,APRecvStartCmd[gun].DrderNum,sizeof(APRecvStartCmd[gun].UserNum));
	memcpy(APSendBilB53[gun].PricTypeID,APRecvB2.PricTypeID,sizeof(APRecvB2.PricTypeID));	//计费模型ID     运营管理系统产生
	memcpy(APSendBilB53[gun].StartTime,APRecvB2.StartTime,sizeof(APRecvB2.StartTime));		//切换时间  CP56Time2a格式
	APSendBilB53[gun].Timenum= stChRate.TimeQuantumNum;
	

	
	for(i = 0;i <  stChRate.TimeQuantumNum;i++)
	{

		
		for(; QuantumNum<47; QuantumNum++)
		{
			if(stChRate.ucSegNum[QuantumNum]!=stChRate.ucSegNum[QuantumNum+1])
			{
				CurQuan = stChRate.ucSegNum[QuantumNum];
				break;
			}
		}
		APSendBilB53[gun].PricInfo[i].CurTimeQuantum = CurQuan;  //当前处于哪个时间段
		
		APSendBilB53[gun].PricInfo[i].ServeFee = stChTcb[gun].stCh.uifsfwChargeEnergy[i]/100;
		APSendBilB53[gun].PricInfo[i].Price = (stChTcb[gun].stCh.uifsChargeEnergy[i] - stChTcb[gun].stCh.uifsfwChargeEnergy[i])/100;
		if(CurQuan < 4)
		{
			APSendBilB53[gun].PricInfo[i].Power = stChTcb[gun].stCh.uifsChargeEnergy[i] *1000 / stChRate.Prices[CurQuan];
		}
		
	}
	memcpy(&Sendbuf[15],(uint8_t*)&APSendBilB53,sizeof(AP_SEND_B53));
	pdata[0] = Sendbuf[1]+2;	//应为是可变的，所以第一个字节为长度
	memcpy(&pdata[1],Sendbuf,Sendbuf[1]+2);
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_SendBillData
* Description  : 发送订单数据
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t AP_SendBillData(uint8_t * pdata,uint8_t len)
{
	return ModuleSIM7600_SendData(0, pdata,MIN(sizeof(AP_SEND_B12) +15,len)); //发送数据
}

/*****************************************************************************
* Function     : AP_SendOffLineBillData
* Description  : 发送离线交易记录订单
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t AP_SendOffLineBillData(uint8_t * pdata,uint16_t len)
{
	return ModuleSIM7600_SendData(0, pdata,MIN(0xD5,len)); //发送数据
}

/*****************************************************************************
* Function     : AP_SendOffLineBillData
* Description  : 发送离线交易记录订单
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t AP_SendOffLineBillFSData(uint8_t * pdata,uint16_t len)
{
	if(SendB53State == 0)
	{
		return RT_FALSE;
	}
	//第一个字节为数据长度
	return ModuleSIM7600_SendData(0, &pdata[1],MIN(len,pdata[0])); //发送数据
}

/*****************************************************************************
* Function     : AP_SendTimeSharBill
* Description  : 上传分时交易记录   充电完成后，在B12发送完成后上报。
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendTimeSharBill(_GUN_NUM gun)
{
	uint8_t Sendbuf[250] = {0};
	uint8_t i;
	uint8_t QuantumNum = 0;
	uint8_t CurQuan;	
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	if(SendB53State == 0)   //是否需要发送分时记录
	{
		return RT_FALSE;
	}
	memset(APSendBilB53[gun].devnum,0,sizeof(AP_SEND_B53));
	Sendbuf[0] = 0x68;  //固定
	Sendbuf[1] = 54+sizeof(_PRIC_INFO)*stChRate.TimeQuantumNum;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x06;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x27;
	APSendBilB53[gun].devnum[7] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	APSendBilB53[gun].devnum[6] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	APSendBilB53[gun].devnum[5] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	APSendBilB53[gun].devnum[4] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	APSendBilB53[gun].devnum[3] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	APSendBilB53[gun].devnum[2] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	APSendBilB53[gun].devnum[1] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	APSendBilB53[gun].devnum[0] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	APSendBilB53[gun].gun_index = gun;
	memcpy(APSendBilB53[gun].DrderNum,APRecvStartCmd[gun].DrderNum,sizeof(APRecvStartCmd[gun].UserNum));
	memcpy(APSendBilB53[gun].PricTypeID,APRecvB2.PricTypeID,sizeof(APRecvB2.PricTypeID));	//计费模型ID     运营管理系统产生
	memcpy(APSendBilB53[gun].StartTime,APRecvB2.StartTime,sizeof(APRecvB2.StartTime));		//切换时间  CP56Time2a格式
	APSendBilB53[gun].Timenum= stChRate.TimeQuantumNum;
	for(i = 0;i <  stChRate.TimeQuantumNum;i++)
	{
		for(; QuantumNum<47; QuantumNum++)
		{
			if(stChRate.ucSegNum[QuantumNum]!=stChRate.ucSegNum[QuantumNum+1])
			{
				CurQuan = stChRate.ucSegNum[QuantumNum];
				break;
			}
		}
		APSendBilB53[gun].PricInfo[i].CurTimeQuantum = CurQuan;  //当前处于哪个时间段
		
		APSendBilB53[gun].PricInfo[i].ServeFee = stChTcb[gun].stCh.uifsfwChargeEnergy[i]/100;
		APSendBilB53[gun].PricInfo[i].Price = (stChTcb[gun].stCh.uifsChargeEnergy[i] - stChTcb[gun].stCh.uifsfwChargeEnergy[i])/100;
		if(CurQuan < 4)
		{
			APSendBilB53[gun].PricInfo[i].Power = stChTcb[gun].stCh.uifsChargeEnergy[i] *1000 / stChRate.Prices[CurQuan];
		}
	}
    memcpy(&Sendbuf[15],(uint8_t*)&APSendBilB53[gun],sizeof(AP_SEND_B53));
	return ModuleSIM7600_SendData(0, Sendbuf,Sendbuf[1]+2); //发送数据
}



/*****************************************************************************
* Function     : AP_SendSetTimeAck
* Description  : 设置时间应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendSetTimeAck(void)
{
	uint8_t Sendbuf[21] = {0};
	Sendbuf[0] = 0x68;
	Sendbuf[1] = 0x13;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x67;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	memcpy(&Sendbuf[14],(uint8_t*)&APRecvSetTime,7);
	return ModuleSIM7600_SendData(0, Sendbuf,21); //发送数据
}


/*****************************************************************************
* Function     : AP_SendCardWLAck
* Description  : 发送卡白名单应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendCardWLAck(void)
{
	uint8_t i = 0;
	
	uint8_t Sendbuf[150] = {0};
	Sendbuf[0] = 0x68;
	Sendbuf[1] = 0x91 - (12 - APCardWL.Nnm)*sizeof(_AP_CARD_INFO);
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x14;
	
	
	memcpy(&APSendCardB18,&APCardWL,sizeof(AP_SEND_B18));
	if(APSendCardB18.Nnm > 12)
	{
		return RT_FALSE;
	}
	for(i = 0;i < 12;i++)
	{
		APSendCardB18.CardState[i].ifSuccess = 0x01;		//都显示成功
	}
	memcpy(&Sendbuf[15],(uint8_t*)&APSendCardB18,sizeof(APSendCardB18));
	return ModuleSIM7600_SendData(0, Sendbuf,0x91 - (12 - APCardWL.Nnm)*sizeof(_AP_CARD_INFO) + 2); //发送数据
}


/*****************************************************************************
* Function     : AP_SendVinWLAck
* Description  : 发送VIN白名单应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendVinWLAck(void)
{
	uint8_t i = 0;
	
	uint8_t Sendbuf[255] = {0};
	Sendbuf[0] = 0x68;
	Sendbuf[1] = 0xFD - (12 - APVinWL.Nnm)*sizeof(_AP_VIN_INFO);
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x14;

	memcpy(&APSendVinB20,&APVinWL,sizeof(APSendVinB20));
	if(APSendVinB20.Nnm > 12)
	{
		return RT_FALSE;
	}
	for(i = 0;i < 12;i++)
	{
		APSendVinB20.VinState[i].ifSuccess = 0x01;		//都显示成功
	}
	memcpy(&Sendbuf[15],(uint8_t*)&APSendVinB20,sizeof(APSendVinB20));
	return ModuleSIM7600_SendData(0, Sendbuf,0xFD - ((12 - APVinWL.Nnm)*sizeof(_AP_VIN_INFO)) + 2); //发送数据
}


/*****************************************************************************
* Function     : AP_SendVinCardResAck
* Description  : 清空白名单应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t  AP_SendVinCardResAck(void)
{
	//682500000000820007000000000017
	
	uint8_t Sendbuf[50] = {0};
	Sendbuf[0] = 0x68;
	Sendbuf[1] = 0x25;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x17;

	memcpy(&APSendVinCardResB20,&APVinCardRes,sizeof(APSendVinCardResB20));
	
	memcpy(&Sendbuf[15],(uint8_t*)&APSendVinCardResB20,sizeof(APSendVinCardResB20));
	return ModuleSIM7600_SendData(0, Sendbuf,Sendbuf[1] + 2); //发送数据
}



/*****************************************************************************
* Function     : AP_SendSetTimeAck
* Description  : 设置费率应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendSetPricB2Ack(void)
{
	uint8_t Sendbuf[50] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	
	Sendbuf[0] = 0x68;
	Sendbuf[1] = 0x1E;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x06;
	
	Sendbuf[22] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	Sendbuf[21] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	Sendbuf[20] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	Sendbuf[19] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	Sendbuf[18] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	Sendbuf[17] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	Sendbuf[16] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	Sendbuf[15] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	memcpy(&Sendbuf[23],APRecvB2.PricTypeID,sizeof(APRecvB2.PricTypeID));
	Sendbuf[31] = 0x00; //0表示成功 1表示失败
	
	return ModuleSIM7600_SendData(0, Sendbuf,32); //发送数据
}

/*****************************************************************************
* Function     : AP_SendSetPricB47Ack
* Description  : 设置费率应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t  AP_SendSetPricB47Ack(void)
{
	uint8_t Sendbuf[50] = {0};
	uint8_t * pdevnum = DisSysConfigInfo.DevNum;
	Sendbuf[0] = 0x68;
	Sendbuf[1] = 0x1F;
	Sendbuf[2] = 0x00;
	Sendbuf[3] = 0x00;
	Sendbuf[4] = 0x00;
	Sendbuf[5] = 0x00;
	Sendbuf[6] = 0x82;
	Sendbuf[7] = 0x00;
	Sendbuf[8] = 0x07;
	Sendbuf[9] = 0x00;
	Sendbuf[10] = 0x00;
	Sendbuf[11] = 0x00;
	Sendbuf[12] = 0x00;
	Sendbuf[13] = 0x00;
	Sendbuf[14] = 0x24;
	
	Sendbuf[22] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
	Sendbuf[21] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
	Sendbuf[20] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
	Sendbuf[19] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
	Sendbuf[18] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
	Sendbuf[17] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
	Sendbuf[16] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
	Sendbuf[15] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
	Sendbuf[23] = APRecvB47.gun_index;
	memcpy(&Sendbuf[24],APRecvB47.PricTypeID,sizeof(APRecvB47.PricTypeID));
	Sendbuf[32] = 0x00; //0表示成功 1表示失败
	
	return ModuleSIM7600_SendData(0, Sendbuf,33); //发送数据
}
/***********************接收数据*********************************************/
/*****************************************************************************
* Function     : AP_Registee
* Description  : 注册帧返回（协议返回）
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RegisteeAck(uint8_t *pdata,uint16_t len)  
{
	if((pdata == NULL) || (len !=10))
	{
		return RT_FALSE;
	}
	APP_SetAppRegisterState(LINK_NUM,STATE_OK);
}

/*****************************************************************************
* Function     : AP_RecvPricB2
* Description  : 费率设置
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvPricB2(uint8_t *pdata,uint16_t len)   
{
	uint8_t i = 0,curseg,lastseg,count = 0;	
	RATE_T rate = {0};
	if(len != (sizeof(AP_RECV_B2)) )
	{
		return RT_FALSE;
	}
	memcpy((uint8_t*)&APRecvB2,pdata,len);
	
	if((APRecvB2.FricNumber != 4) || (APRecvB2.UserState == 2) )
	{
		return RT_FALSE;			//时间段不为4或者失效
	}
	memset(&rate,0,sizeof(RATE_T));
	curseg = APRecvB2.FricNum[0];
	lastseg = curseg;
	for(i = 1;i < 48;i++)
	{
		curseg = APRecvB2.FricNum[i];
		rate.ucSegNum[i] = APRecvB2.FricNum[i] - 1;
		if(lastseg != curseg)
		{
			if((lastseg == 0) || (lastseg >4) )
			{
				return RT_FALSE;
			}
			lastseg = curseg;
			count++;
			if(count >= TIME_PERIOD_MAX)
			{
				break;		//最多为12个时间段
			}
		}
		
	}
	
	for(i = 0;i < 4;i++)
	{
		rate.Prices[i] = APRecvB2.Fric[i];
		rate.fwPrices[i] = APRecvB2.ServiceFric * 1000;
	}
	rate.TimeQuantumNum =  count + 1;
	
	memcpy(&stChRate,&rate,sizeof(rate));
	fal_partition_erase(charge_rete_info, 0, sizeof(RATE_T));
	fal_partition_write(charge_rete_info,0,(uint8_t*)&stChRate,sizeof(RATE_T)); 
	mq_service_send_to_4gsend(APP_RATE_ACK,1 ,0 ,NULL);

	SendB53State = 0;			//不需要发送分时计费记录
	
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_RecvPricB47
* Description  : 费率设置1
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvPricB47(uint8_t *pdata,uint16_t len)  
{
	uint8_t i,num = 0,lastnum = 0,count = 0;
	RATE_T rate = {0};
	
	if(len < 40)
	{
		return RT_FALSE;
	}
	memcpy((uint8_t*)&APRecvB47,pdata,MIN(len,sizeof(APRecvB47)));
	if((APRecvB47.FricNumber == 0) || (APRecvB47.FricNumber > 12))
	{
		return RT_FALSE;
	}
	rate.TimeQuantumNum = APRecvB47.FricNumber;
	
	for(i = 0; i < APRecvB47.FricNumber;i++)
	{
		if((APRecvB47.SetPricInfo[i].CurTimeQuantum == 0) || (APRecvB47.SetPricInfo[i].CurTimeQuantum > 4) )
		{
			return RT_FALSE;
		}
		num = (BCDtoHEX(APRecvB47.SetPricInfo[i].StopTime[0]) + BCDtoHEX(APRecvB47.SetPricInfo[i].StopTime[1]) * 60) /30;
		if(num > 0)
		{
			for(count = lastnum;count < num;count++)
			{
				rate.ucSegNum[count] = APRecvB47.SetPricInfo[i].CurTimeQuantum - 1;
			}
			lastnum =count;
		}
		else
		{
			rate.ucSegNum[0] = APRecvB47.SetPricInfo[0].CurTimeQuantum - 1;
		}
		rate.Prices[APRecvB47.SetPricInfo[i].CurTimeQuantum -1] = APRecvB47.SetPricInfo[i].Fric;
		rate.fwPrices[APRecvB47.SetPricInfo[i].CurTimeQuantum -1] = APRecvB47.SetPricInfo[i].ServiceFric;				//服务费
	}
	
	memcpy(&stChRate,&rate,sizeof(rate));
	fal_partition_erase(charge_rete_info, 0, sizeof(RATE_T));
	fal_partition_write(charge_rete_info,0,(uint8_t*)&stChRate,sizeof(RATE_T)); 
	mq_service_send_to_4gsend(APP_RATE_ACK,2 ,0 ,NULL);
	SendB53State = 1;			//发送分时计费记录	
	
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_RecvStartAppAck
* Description  : 启动确认帧
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvStartAppAck(uint8_t *pdata,uint16_t len)
{
	return RT_TRUE;
}	

/*****************************************************************************
* Function     : AP_RecvHearAck
* Description  : 心跳确认帧
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvHearAck(uint8_t *pdata,uint16_t len)
{
	return RT_TRUE;
}	


/*****************************************************************************
* Function     : AP_RecvOnlineBillAck
* Description  : 返回在线交易记录
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvOnlineBillAck(uint8_t *pdata,uint16_t len)
{
	uint8_t gun = 0;
	uint8_t ifsuccess;
	gun = pdata[8];
	ifsuccess = pdata[9];
	if(ifsuccess == 0)
	{
		ResendBillControl[gun].ResendBillState = RT_FALSE;			//订单确认，不需要重发订单
		ResendBillControl[gun].SendCount = 0;
        /*WriterFmBill((_GUN_NUM)gun,0);*/
	}
	return RT_TRUE;
}	

/*****************************************************************************
* Function     : AP_RecvB54BillAck
* Description  : 返回在线分时交易记录
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvB54BillAck(uint8_t *pdata,uint16_t len)
{
//	uint8_t gun = 0;
//	uint8_t ifsuccess;
//	gun = pdata[8];
//	ifsuccess = pdata[9];
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_RecvTime
* Description  : 校准时间
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvTime(uint8_t *pdata,uint16_t len)
{
	uint8_t Second;
	if(len != (sizeof(APRecvSetTime)) )
	{
		return RT_FALSE;
	}
	memcpy((uint8_t*)&APRecvSetTime,pdata,7);
	
	Second = APRecvSetTime.Second / 1000;
	//校准时间
	
	set_date(APRecvSetTime.Year + 2000,APRecvSetTime.Month,APRecvSetTime.Day);
	set_time(APRecvSetTime.Hour,APRecvSetTime.Minute,Second);
	
	mq_service_send_to_4gsend(APP_STE_TIME,GUN_A ,0 ,NULL);
	return RT_TRUE;
}	

/*****************************************************************************
* Function     : AP_RecvCardB7
* Description  : 接收到卡鉴权信息
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvCardB7(uint8_t *pdata,uint16_t len)
{
	uint8_t gun = 0;
	uint16_t datalen = 0;

	datalen = sizeof(AP_RECV_B7);
	
	if(len != datalen )
	{
		return RT_FALSE;
	}
	gun = pdata[8];
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	if(stChTcb[gun].ucState != INSERT)
	{
		return RT_FALSE;
	}
	memcpy((uint8_t*)&APRecvCardCmd[gun],pdata,len);
	if(APRecvCardCmd[gun].IfSuccess) //鉴权成功，发送
	{
		APStartType[gun] = _4G_APP_CARD;
		APP_SetStartNetState (GUN_A,NET_STATE_ONLINE);		//在线充电
		//发送刷卡启动
		mq_service_send_to_4gsend(APP_CARDVIN_CHARGE,GUN_A ,0 ,NULL);
	}
	else
	{
		//通知鉴权失败
		rt_pin_write(70, PIN_HIGH );   
		rt_thread_mdelay(100);
		rt_pin_write(70, PIN_LOW);
	}
	
	return RT_TRUE;
}


/*****************************************************************************
* Function     : AP_RecvCardStartB10
* Description  : 接收到卡Vin启动
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvCardVinStartB11(uint8_t *pdata,uint16_t len)
{

	uint8_t gun = 0;
	static uint8_t Vinstate = 0; //0失败 1成功

	
	if(len != (sizeof(AP_RECV_B11)) )
	{
		rt_pin_write(70, PIN_HIGH );   
		rt_thread_mdelay(100);
		rt_pin_write(70, PIN_LOW);
		return RT_FALSE;
	}
	gun = pdata[8];
	if(gun >= GUN_MAX)
	{
		rt_pin_write(70, PIN_HIGH );   
		rt_thread_mdelay(100);
		rt_pin_write(70, PIN_LOW);
		return RT_FALSE;
	}
	memcpy((uint8_t*)&APRecvCardVinStart[gun],pdata,len);
	if(APRecvCardVinStart[gun].ifSuccess) //成功，发送启动成功
	{
		ResendBillControl[gun].ResendBillState = RT_FALSE;	  //之前的订单无需发送了
		ResendBillControl[gun].SendCount = 0;
		//启动充电
		send_ch_ctl_msg(gun,1,0,4,0);
	}
	rt_pin_write(70, PIN_HIGH );   
	rt_thread_mdelay(100);
	rt_pin_write(70, PIN_LOW);
	
	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_RecvCardStartB10
* Description  : 接收到卡Vin启动
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvB17CardWL(uint8_t *pdata,uint16_t len)
{
	uint8_t num,i,addnum,delnum,j;
	uint8_t buf[96];
	
	

	if(len < 30)
	{
		return RT_FALSE;
	}
	//获取白名单数量
	num = pdata[23];
	//最多十二个	
	if((num == 0) || (num > 12) )
	{
		return RT_FALSE;
	}
	if(len != (sizeof(_AP_CARD_INFO) * num + 24) )
	{
		return RT_FALSE;
	}
	memcpy((uint8_t*)&APCardWL,pdata,len);
	//写白名单
	addnum = 0;
	delnum = 0;
	//增加
	for(i = 0;i < num;i++)
	{
		if(APCardWL.CardInfo[i].State == 1)
		{
			
			for(j = 0;j< 8;j++)
			{
				//卡号反过来的
				buf[addnum*8 + j] = APCardWL.CardInfo[i].CardNum[7-j];
			}
			//memcpy(&buf[addnum*8],APCardWL.CardInfo[i].CardNum,8);
			addnum++;
		}
	}
	if(addnum)
	{
		AP_CardWhiteListDispose(buf,addnum,1);
	//减少
	}
	for(i = 0;i < num;i++)
	{
		if(APCardWL.CardInfo[i].State == 2)
		{
			for(j = 0;j< 8;j++)
			{
				//卡号反过来的
				buf[delnum*8 + j] = APCardWL.CardInfo[i].CardNum[7-j];
			}
			delnum++;
		}
	}
	if(delnum)
	{
		AP_CardWhiteListDispose(buf,delnum,0);
	}
	
	mq_service_send_to_4gsend(APP_CARD_WL,GUN_A ,0 ,NULL);
	
	return RT_TRUE;
	
}



/*****************************************************************************
* Function     : AP_RecvB21VINCardRes
* Description  : 清空白名单
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t AP_RecvB21VINCardRes(uint8_t *pdata,uint16_t len)
{

	 //读取数据
	memset(FlashDisposeBuf,0xff,sizeof(FlashDisposeBuf));
	FlashDisposeBuf[0] = 0;
	
	if(len != sizeof(_AP_RECV_B21) )
	{
		return RT_FALSE;
	}
	memcpy(&APVinCardRes,pdata,len);
	if(APVinCardRes.type == 1)
	{
		APP_RWCardWl(FLASH_ORDER_WRITE,FlashDisposeBuf,sizeof(FlashDisposeBuf));
	}
	
	mq_service_send_to_4gsend(APP_VINCARD_RES,GUN_A ,0 ,NULL);

	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_RecvB16OFFLineRco
* Description  : 离线交易记录应答
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t AP_RecvB16OFFLineRco(uint8_t *pdata,uint16_t len)
{

	uint8_t data[300];
	_AP_RECV_B16 *poffline;
	if(len != sizeof(_AP_RECV_B16) )
	{
		return RT_FALSE;
	}
	memcpy(&APOFFLineRec,pdata,len);
	 APP_RWNetOFFLineRecode(ResendBillControl[GUN_A].OffLineNum,FLASH_ORDER_READ,data);   //读取离线交易记录
	poffline = (_AP_RECV_B16 *)&data[14];
	if(CmpNBuf(poffline->DrderNum,APOFFLineRec.DrderNum,sizeof(poffline->DrderNum) ) == RT_TRUE)
	{
		//订单号一致
		mq_service_send_to_4gsend(APP_OFFLINE_ACK,GUN_A ,0 ,NULL);
	}

	return RT_TRUE;
}

/*****************************************************************************
* Function     : AP_RecvSTCmd
* Description  : 接收到启动还是停止
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   AP_RecvSTCmd(uint8_t *pdata,uint16_t len)
{
	uint8_t gun = 0;

	if(len != (sizeof(AP_RECV_B4)) )
	{
		return RT_FALSE;
	}
	gun = pdata[8];
	if(gun >= GUN_MAX)
	{
		return RT_FALSE;
	}
	if(pdata[9] == 1)	//启动充电
	{
		ResendBillControl[gun].ResendBillState = RT_FALSE;	  //之前的订单无需发送了
		ResendBillControl[gun].SendCount = 0;
		memcpy((uint8_t*)&APRecvStartCmd[gun],pdata,sizeof(AP_RECV_B4));
		//发送启动应答
		APStartType[gun] = _4G_APP_START;
		APP_SetStartNetState (GUN_A,NET_STATE_ONLINE);		//在线充电
		send_ch_ctl_msg(gun,1,0,4,0);
		mq_service_send_to_4gsend(APP_START_CHARGE,GUN_A ,0 ,NULL);
	}else
	{
		mq_service_send_to_4gsend(APP_STOP_ACK,GUN_A ,0 ,NULL);
				//发送停止应答
	//	return TRUE;
		//停止充电
		send_ch_ctl_msg(gun,2,0,4,0);
	}
	
	return RT_TRUE;
}	

/*****************************************************************************
* Function     : AP_RecvFrameDispose
* Description  : 安培快充数据处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   AP_RecvFrameDispose(uint8_t * pdata,uint8_t len)
{
	uint8_t i = 0;
	uint8_t num;
	static uint8_t buf[300];
	uint8_t cmdlen;

	if((pdata == NULL) || (len < 6) )
	{
		return RT_FALSE;
	}
//	if((datalen+2) != len)
//	{
//		return FALSE;
//	}
	for(i = 0;i < AP_RECV_TABLE_NUM;i++)
	{	if(len < APRecvDisposeTable[i].len)
		{
			continue;
		}
		for(num = 0;num < APRecvDisposeTable[i].len;num++)
		{
			if(pdata[num] != APRecvDisposeTable[i].pdata[num])
			{
				break;
			}
		}
		if(num == APRecvDisposeTable[i].len)
		{
			if(APRecvDisposeTable[i].recvfunction != NULL)
			{
				cmdlen = APRecvDisposeTable[i].len;
				//提取数据
				memcpy(buf,&pdata[cmdlen],len - cmdlen);  //真正得数据内容
				APRecvDisposeTable[i].recvfunction(buf,len - cmdlen);
				return RT_TRUE;
			}
		}
		if(APRecvDisposeTable[i].pdata[1] == 0xff)  //表示接收数据可变长度，
		{
			for(num = 0;num < APRecvDisposeTable[i].len - 3;num++)
			{
				if(pdata[num + 3] != APRecvDisposeTable[i].pdata[num + 3])
				{
					break;
				}
			}
			if(num == APRecvDisposeTable[i].len - 3)
			{
				if(APRecvDisposeTable[i].recvfunction != NULL)
				{
					cmdlen = APRecvDisposeTable[i].len;
					//提取数据
					memcpy(buf,&pdata[cmdlen],len - cmdlen);  //真正得数据内容
					APRecvDisposeTable[i].recvfunction(buf,len - cmdlen);
					return RT_TRUE;
				}
			}
		}
	}

	return RT_TRUE;
}

#endif
/************************(C)COPYRIGHT 2020 杭州汇誉*****END OF FILE****************************/
