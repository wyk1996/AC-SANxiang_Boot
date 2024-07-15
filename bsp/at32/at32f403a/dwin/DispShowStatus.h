/*******************************************************************************
 *          Copyright (c) 2020-2050,  Co., Ltd.
 *                              All Right Reserved.
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
#ifndef __DISPSTATUS_H_
#define __DISPSTATUS_H_

#include <rtthread.h>
#include "stdint.h"
//#include "DwinProtocol.h"
#include "ch_port.h"

//显示状体
typedef enum
{
    SHOW_START_APP,  		//APP启动
    SHOW_START_CARD,		//刷卡启动
    SHOW_BILL,				//已结算
    SHOW_NOTBIL,			//未结算
    SHOW_STOP_ERR_NONE,		//正常停止
    SHOW_STOP_FAIL,			//故障停止
    SHOW_STOP_LESS,   //余额不足停止
    SHOW_XY_HY,   //汇誉
    SHOW_XY_AP,
    SHOW_XY_YKC,  //云快充
    SHOW_XY_HFQG,
    SHOW_XY_XH,  //小鹤
    SHOW_XY_YL2,
    SHOW_XY_YL3,
    SHOW_XY_YL4,
    SHOW_GUNA, //A枪
    SHOW_GUNB,	//B枪
	  SHOW_GUN_IN, //已插枪
    SHOW_GUN_NO,//未插枪
	  SHOW_GUN_NOPAY, //待结算
    SHOW_MAX,
} _SHOW_NUM;


uint8_t Dis_ShowStatus(uint16_t add_show,_SHOW_NUM show_num);
uint8_t Dis_ShowCopy(uint8_t* pdata,_SHOW_NUM show_num);
uint8_t Dis_Showstop_reason(uint8_t* pdata,STOP_REASON Stop_reasonnum);
#endif
