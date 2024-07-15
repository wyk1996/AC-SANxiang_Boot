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
#ifndef _CH_PORT_H_
#define _CH_PORT_H_

#include <stdint.h>
#include <time.h>
#include <data_def.h>
#include "common.h"


typedef struct
{

    _GUN_NUM   gun;
    /* 1:启动     2:停止 其他无意义 */
    uint8_t   ucCtl;
    /* 0:app启动  1:刷卡启动 */
    uint8_t   ucChStartMode;
    /* 0:自动充满 1:按金额充电 2:按时间充电 3:按电量充电 */
    uint8_t   ucChMode;
    uint32_t  uiStopParam;
} CH_CTL_T;

enum ch_start_failed_reason
{
    START_SUCCESS = 0,
    DEVICE_ = 0x4100,
    NO_GUN_DETECTED,
    SYSTEM_ERROR = 0x4201,
    SERVIE_PAUSED,
    METER_OFFLINE,
};

typedef enum
{
    NORMAL = 0,				/* 0：正常 */
    R_POWER_DOWN,			/* 1: 检测到掉电 */
    LEAKAGE_FAULT,			/* 2：漏电故障 */
    SPD_FAULT,				/* 3：防雷故障 */
    OTHER_FAULT,            /* 4: 门打开 */
    DOOR_OPEN,				/* 5: 门打开 */
    EM_STOP,				/* 6: 急停按下 */
    OVER_VOLT,				/* 7：过压 */
    OVER_CURRENT,			/* 8：过流 */
    READY_TIMEOUT,			/* 9：车就绪超时,9V->6V失败 */
    CP_LINK_ABN,			/* 10：cp连接异常 */
    UNPLUG,				    /* 11：非法拔枪 */
    END_CONDITION,			/* 12：到达充电结束条件 */
    NO_CURRENT,				/* 13：无电流 */
    E_END_BY_APP,			/* 14：app结束 */
    E_END_BY_CARD,			/* 15：刷卡结束 */
    UNDER_VOLT,				/* 16：欠压 */
    STOP_CHARGEFULL,		/* 17:充满*/
    STOP_OTHEN,				/* 18：其他 */
    E_END_APP_BALANC, /*19 app余额不足*/
		NO_CURRENT_9V,     /*20 无电流时9v停止*/
    STOP_MAX,         /*19:最大*/
} STOP_REASON;				//停止原因

typedef struct
{
    uint16_t  usMsec;
    uint8_t   ucMin : 6;
    uint8_t   ucRes0: 1;
    uint8_t   ucIV  : 1;
    uint8_t   ucHour:5;
    uint8_t   ucRes1:2;
    uint8_t   ucSU  :1;
    uint8_t   ucDay: 5;
    uint8_t   ucWeek:3;
    uint8_t   ucMon: 4;
    uint8_t   ucRes2:4;
    uint8_t   ucYear:7;
    uint8_t   ucRes3:1;
} CP56TIME2A_T;




typedef struct
{
    /* 1:启动    2:停止 其他无意义 */
    uint8_t   ucCtl;
    /* 0:app启动 1:刷卡启动 其他无意义 */
    uint8_t   ucChStartMode;
    /* 0:成功    1:失败 其他无意义 */
    uint8_t   ucResult;
    /* 见协议 */
    uint8_t   ucResultCode;
} CHCTL_ACK_T;


void localtime_to_cp56time(time_t tTime,CP56TIME2A_T *pstCp56);

void send_up_chinfo_result(uint8_t ucResult);


//uint8_t get_ch_rate(RATE_T *pstRate);
//uint8_t save_ch_info(CHINFO_T *pstChInfo);

//uint8_t up_ch_info(CHINFO_T *pstChInfo);

#endif


