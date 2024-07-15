/*******************************************************************************
 * @file
 * @note
 * @brief
 *
 * @author
 * @date     2021-05-02
 * @version  V1.0.0
 *
 * @Description
 *
 * @note History:
 * @note     <author>   <time>    <version >   <desc>
 * @note
 * @warning
 *******************************************************************************/
#ifndef _CH_TASK_H_
#define _CH_TASK_H_

#include <stdint.h>
#include <time.h>
#include <rtthread.h>
#include "ch_port.h"
#include "user_lib.h"


#define CP_12V_MAX                  13000
#define CP_12V_MIN                  10001

#define CP_9V_MAX                   10000
#define CP_9V_MIN                   7501

#define CP_6V_MAX                   7500
#define CP_6V_MIN                   4500

#warning "YXY 临时修改电流"
#define CH_CURR_THRESHOLD           100
//#define CH_CURR_THRESHOLD           5

#define CH_CURR_MAX                 3500
#define CH_VOLT_MAX                 2530
#define CH_VOLT_MIN                 1870

/* 操作系统 tick 是 1ms */
#define CH_TIME_20_MSEC             20
#define CH_TIME_1_MSEC             1
#define CH_TIME_30_MSEC             30

#define CH_TIME_500_MSEC            500ul
#define CH_TIME_1_SEC               1000ul
#define CH_TIME_5_SEC               (5 * CH_TIME_1_SEC)
#define CH_TIME_10_SEC              (10 * CH_TIME_1_SEC)
#define CH_TIME_15_SEC              (15 * CH_TIME_1_SEC)
#define CH_TIME_30_SEC              (30 * CH_TIME_1_SEC)
#define CH_TIME_60_SEC              (60 * CH_TIME_1_SEC)

#define CH_DATA_SEC                 (60 * 60 * 24)
#define CH_HALFHOUR_SEC             (60 * 30)


#define CH_DEF_FLG_DIS              0
#define CH_DEF_FLG_EN               1

#define USERN8209       1  //1板子内部电表芯片   0外部电表
#define NOUSERN8209     0
#define B0card   0x4230  //一卡一桩免费
#define B1card   0x4231  //一卡多桩免费
#define C0card   0x4330  //一卡一桩收费
#define C1card   0x4331  //一卡多桩收费

enum ch_main_stat
{
    STANDBY = 0,
    INSERT,
    WAIT_CAR_READY,
    PRECHARGE,
    CHARGING,
    WAIT_STOP,
    CHARGER_FAULT,
    POWER_DOWN,
};

enum cp_stat
{
    CP_12V = 0,
    CP_9V,
    CP_6V,
    CP_ABNORMAL,
};

enum ch_stt_stp_ctl
{
    CH_IDLE = 0,
    CH_START_CTL,
    CH_STOP_CTL,
};

typedef struct
{
    uint8_t   ucFlg;
    uint32_t  uiTick;
} LAST_T;

typedef struct
{
    uint16_t  usCurrent;                 // rn8209 电流 单位0.01A
    uint16_t  usVolt;                 // rn8209 电压 单位0.1V
    uint32_t  uiE; 	                  // rn8209 总电量 0.001 kWH
    uint32_t  uiGetTime;              // 获取rn8209的时间
} RN8209_T;

__packed typedef struct
{
    uint32_t     uiDChQ; 			//上一次电能读数
    uint32_t     uiChargeEnergy ;   //充电电量小数点三位


    SYSTEM_RTCTIME	uiChStartTime ;    //开始充电时间
    SYSTEM_RTCTIME	uiChStoptTime;		//结束充电时间
    time_t 			timestart;			//开始充电距离1970年的秒数
    time_t 			timestop;			//停止充电距离1970年的秒数
    time_t      Reservetime;  //预约充电时间
    uint32_t     uiaChargeEnergy[4] ;  	//尖峰平谷的总费用，小数点4位
    uint32_t     uifwChargeEnergy[4] ;	//尖峰平谷服务费用，小数点4位
    uint32_t     jfpg_kwh[4];			//尖峰平谷kwh   三位小数
    //安培 云快充需要上传
    uint32_t	uifsChargeEnergy[48];	//每个时间段的总费用，小数点4位
    uint32_t	uifsfwChargeEnergy[48];	//每个时间段的服务费用，小数点4位

    uint32_t     uiAllEnergy;  			//总费用，小数点4位
    STOP_REASON reason;					//停止原因
} CH_T;

typedef struct
{
    uint8_t  ucCpStat;  //当前cp电压所在的对应状态
    uint8_t  ucCount  ;
    uint8_t  ucCount_6v  ;
    uint8_t  ucRev ;
    uint32_t uiCpVolt;
	uint8_t  PWMcycle;
} GUN_T;


typedef struct
{
    uint8_t ucFrist;
    uint32_t uiFaultFlg;
} FAULT_T;

typedef enum
{
//		0-离线
    PT_STATE_OFFLINE = 0,
//		1-故障
    PT_FAULTED = 1,
//      2-空闲
    PT_STATE_IDLE = 2,
//		3-充电中
    PT_STATE_CHARGING = 3,
// 		4-插枪(未充电)
    PT_STATE_INSERT_GUN_NOCHARGING = 4,
//      5-充电结束未拔枪
    PT_STATE_CHARGING_STOP_NOT_GUN = 5,
} E_PT_STATE;


typedef struct
{
    uint8_t                   ucState;
    E_PT_STATE                ucRunState;
    uint32_t                  uiJumpTick;
    uint32_t                  uiRealUpdateTime;
    RN8209_T     stRn8209;
    CH_CTL_T                  stChCtl;   	//启停充电相关信息
    GUN_T                     stGunStat;
    FAULT_T                   stIOFault; 		//IO 故障
    CH_T                      stCh;        	//充电信息
    LAST_T       stLowCurr;
    LAST_T       stOverCurr;
    LAST_T       stOverVolt;
    LAST_T       stUnderVolt;
    LAST_T       stCP9Vstate;
    uint8_t   Agun_flag;   //AB枪的标志位
    uint8_t   Bgun_flag;
    uint8_t   clearflag; //是否清空标示位 0是不清空，1是清空
} CH_TASK_T;


enum fault_code
{
    C_DOOR,                 /* 门禁 */
    C_EM,                   /* 急停 */
};

int      cp_pwm_init(void);
void     ch_ctl_init(void);
int32_t  cp_adc_init(void);
int32_t  cp2_adc_init(void);
uint32_t ch_get_cp_volt(_GUN_NUM gun,uint32_t *puiCpVolt);
void     ch_loop_proc(_GUN_NUM gun,CH_TASK_T *pstChTcb);
void     input_io_init(void *pstRn8209);
//void     ch_rate_init(void);
void     ch_ctl_enable(_GUN_NUM gun);
void     ch_ctl_disable(_GUN_NUM gun);
uint8_t ch_is_over_volt(_GUN_NUM gun,CH_TASK_T *pstChTcb);
uint8_t ch_is_under_volt(_GUN_NUM gun,CH_TASK_T *pstChTcb);
uint8_t ch_is_over_current(_GUN_NUM gun,CH_TASK_T *pstChTcb);
extern CH_T		DChargeInfo[GUN_MAX];		//掉电充电存储信息	
//extern uint8_t mode5(void);
//void HardFault_Handler(void);
extern CH_TASK_T stChTcb[GUN_MAX];
#endif




