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
#include "MenuDisp .h"
#include "DispKeyFunction.h"
#include <rtthread.h>
#include "stdio.h"

//主界面
ST_Menu HYMenu25 =
{
    NULL,
    25,
    HYKeyEvent25,
    HYDisplayMenu25,
    5,
};

//提示插枪界面
ST_Menu HYMenu1 =
{
    NULL,
    1,
    HYKeyEvent1,
    HYDisplayMenu1,
    0,
};

//故障界面
ST_Menu HYMenu2 =
{
    NULL,
    2,
    HYKeyEvent2,
    HYDisplayMenu2,
    0,
};

//充电中界面
ST_Menu HYMenu3 =
{
    NULL,
    3,
    HYKeyEvent3,
    HYDisplayMenu3,
    0,
};

//启动中界面
ST_Menu HYMenu4 =
{
    NULL,
    4,
    HYKeyEvent4,
    HYDisplayMenu4,
    0,
};

//提示刷卡停止充电界面
ST_Menu HYMenu5 =
{
    NULL,
    5,
    HYKeyEvent5,
    HYDisplayMenu5,
    15,
};

//结算界面
ST_Menu HYMenu6 =
{
    NULL,
    6,
    HYKeyEvent6,
    HYDisplayMenu6,
    15,
};

//卡信息提示界面
ST_Menu HYMenu7 =
{
    NULL,
    7,
    HYKeyEvent7,
    HYDisplayMenu7,
    4,
};

//卡信息提示界面
ST_Menu HYMenu8 =
{
    NULL,
    8,
    HYKeyEvent8,
    HYDisplayMenu8,
    2,
};

//刷卡结算
ST_Menu HYMenu9 =
{
    NULL,
    9,
    HYKeyEvent9,
    HYDisplayMenu9,
    2,
};

//非发行卡
ST_Menu HYMenu10 =
{
    NULL,
    10,
    HYKeyEvent10,
    HYDisplayMenu10,
    2,
};

//卡信息提示界面
ST_Menu HYMenu11 =
{
    NULL,
    11,
    HYKeyEvent11,
    HYDisplayMenu11,
    2,
};

ST_Menu HYMenu12 =
{
    NULL,
    12,
    HYKeyEvent12,
    HYDisplayMenu12,
    0,
};

ST_Menu HYMenu13 =
{
    NULL,
    13,
    HYKeyEvent13,
    HYDisplayMenu13,
    0,
};

//系统升级==修改成：系统配置
ST_Menu HYMenu14 =
{
    NULL,
    14,
    HYKeyEvent14,
    HYDisplayMenu14,
    0,
};

//系统升级
ST_Menu HYMenu15 =
{
    NULL,
    15,
    HYKeyEvent15,
    HYDisplayMenu15,
    0,
};

//系统升级
ST_Menu HYMenu16 =
{
    NULL,
    16,
    HYKeyEvent16,
    HYDisplayMenu16,
    0,
};

//按电量充
ST_Menu HYMenu17 =
{
    NULL,
    17,
    HYKeyEvent17,
    HYDisplayMenu17,
    30,
};

//按金额充
ST_Menu HYMenu18 =
{
    NULL,
    18,
    HYKeyEvent18,
    HYDisplayMenu18,
    30,
};

//按时间充
ST_Menu HYMenu19 =
{
    NULL,
    19,
    HYKeyEvent19,
    HYDisplayMenu19,
    30,
};

//定时充
ST_Menu HYMenu20 =
{
    NULL,
    20,
    HYKeyEvent20,
    HYDisplayMenu20,
    30,
};

//系统升级
ST_Menu HYMenu21 =
{
    NULL,
    21,
    HYKeyEvent21,
    HYDisplayMenu21,
    0,
};

//系统升级
ST_Menu HYMenu22 =
{
    NULL,
    22,
    HYKeyEvent22,
    HYDisplayMenu22,
    0,
};

//系统升级
ST_Menu HYMenu23 =
{
    NULL,
    23,
    HYKeyEvent23,
    HYDisplayMenu23,
    0,
};

//系统升级
ST_Menu HYMenu24 =
{
    NULL,
    24,
    HYKeyEvent24,
    HYDisplayMenu24,
    3,
};


//密码错误界面
ST_Menu HYMenu26 =
{
    NULL,
    26,
    HYKeyEvent26,
    HYDisplayMenu26,
    3,
};

//费率详情页面1
ST_Menu HYMenu27 =
{
    NULL,
    27,
    HYKeyEvent27,
    HYDisplayMenu27,
    0,
};

//费率详情页面2
ST_Menu HYMenu28 =
{
    NULL,
    28,
    HYKeyEvent28,
    HYDisplayMenu28,
    0,
};


ST_Menu HYMenu30 =
{
    NULL,
    30,
    HYKeyEvent30,
    HYDisplayMenu30,
    0,
};

ST_Menu HYMenu31 =
{
    NULL,
    31,
    HYKeyEvent31,
    HYDisplayMenu31,
    0,
};


ST_Menu HYMenu32 =
{
    NULL,
    32,
    HYKeyEvent32,
    HYDisplayMenu32,
    5,
};


//33.34是解锁界面
ST_Menu HYMenu33 =
{
    NULL,
    33,
    HYKeyEvent33,
    HYDisplayMenu33,
    5,
};

ST_Menu HYMenu34 =
{
    NULL,
    34,
    HYKeyEvent34,
    HYDisplayMenu34,
    2,
};


ST_Menu HYMenu35 =
{
    NULL,
    35,
    HYKeyEvent35,
    HYDisplayMenu35,
    2,
};


ST_Menu HYMenu36 =
{
    NULL,
    36,
    HYKeyEvent36,
    HYDisplayMenu36,
    3,
};
ST_Menu HYMenu37 =
{
    NULL,
    37,
    HYKeyEvent37,
    HYDisplayMenu37,
    2,
};
