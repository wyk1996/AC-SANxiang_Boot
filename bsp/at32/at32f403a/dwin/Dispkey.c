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
#include <stdio.h>
#include <string.h>
#include "Dispkey.h"
#include "DispKeyFunction.h"
#include "MenuDisp .h"
#include "dwin_com_pro.h"
#include "4GMain.h"
#include "DwinProtocol.h"
#include "chtask.h"

#ifndef MIN
#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#endif

extern void PrintIcon(uint16_t VariableAddress,uint16_t num);


//======点击3次进入管理员模式
uint8_t Ad_entry(void)
{
    static uint8_t keynum=0;  //隐藏按钮点击次数，默认必须连续三次点击才可进入
    static uint32_t curtime=0,lasttime=0;
    curtime = rt_tick_get();
    if((curtime-lasttime) < 3000)
    {
        lasttime = curtime; //当前的值赋值
        if(++keynum>=3)
        {
            keynum=0;
            return RT_TRUE;
        }
    }
    else
    {
        lasttime = curtime; //当前的值赋值
        keynum=0;
    }
    return RT_FALSE;
}
/**
 * @brief
 * @param[in] _LCD_KEYVALUE *keyval 按键值指针
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent25(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {

    case LCD_KEY2:				//返回上一页 可能是第一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;

    case LCD_KEY5:
//        DisplayCommonMenu(&HYMenu12,&HYMenu25);
        if(Ad_entry())  //自动键控
        {
            DispControl.CurSysState = DIP_STATE_SYS;
            DisplayCommonMenu(&HYMenu12,NULL);

            rt_thread_mdelay(50); //延迟一下，不延迟模拟点击不能用
            simulation_Print(0x0064,0x0096); //自动键控值
        }
        break;
    default:
        break;
    }
}

extern _DISP_CONTROL DispControl;
/**
 * @brief
 * @param[in] _LCD_KEYVALUE *keyval 按键值指针
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent1(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY5:  //点击右上角3次，进入管理员模式
        if(Ad_entry())
        {
            DispControl.CurSysState = DIP_STATE_SYS;
            DisplayCommonMenu(&HYMenu12,NULL);
            //发送自动点击指令集
            rt_thread_mdelay(50); //延迟一下，不延迟模拟点击不能用
            simulation_Print(0x0064,0x0096); //自动键控值
        }
        break;

    case LCD_KEY4:
        DispControl.CurSysState = DIP_STATE_SYS;
        //DisplayCommonMenu(&HYMenu27,&HYMenu1);
        Munu27_ShowSysInfo(0);   //显示费率
        break;

    case LCD_KEY6: //显示记录查询页面
        DispControl.CurSysState = DIP_STATE_SYS;
        APP_ClearRecodeInfo();
        APP_SelectCurChargeRecode();  //首次进入选择当前的充电记录
        break;

    case LCD_KEY7: //卡解锁
        DispControl.CurSysState = DIP_STATE_SYS;
        DisplayCommonMenu(&HYMenu33,&HYMenu1);
        break;

    case LCD_KEY8: //A枪刷充电按钮
        Click_card(GUN_A);  //插枪时 才能点击，才可以跳转25刷卡界面
        break;

    case LCD_KEY9: //B枪刷充电按钮
        Click_card(GUN_B);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in] _LCD_KEYVALUE *keyval 按键值指针
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent2(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY5:
        if(Ad_entry())
        {
            DispControl.CurSysState = DIP_STATE_SYS;
            DisplayCommonMenu(&HYMenu12,NULL);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent3(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY5:
        //DispControl.CurSysState = DIP_STATE_SYS;
        DisplayCommonMenu(&HYMenu12,&HYMenu3);
        break;
    case LCD_KEY4:				//进入停止刷卡界面
        DisplayCommonMenu(&HYMenu5,&HYMenu3);
        break;
    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent4(_LCD_KEYVALUE *keyval)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent5(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DisplayCommonMenu(HYMenu5.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent6(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DisplayCommonMenu(HYMenu6.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent7(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(HYMenu7.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent8(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(HYMenu8.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent9(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(HYMenu9.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent10(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(HYMenu10.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent11(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(HYMenu11.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent12(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY3:				//返回上一页
    case LCD_KEY1:
        DispControl.CurSysState = DIP_STATE_NORMAL;
        //DisplayCommonMenu(HYMenu12.Menu_PrePage,NULL);
        break;

    case LCD_KEY4:				//下发密码
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent13(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页
        DispControl.CurSysState = DIP_STATE_NORMAL;    //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;

    case LCD_KEY2:				//下一页
        //Munu13_ShowSysInfo();
        Disp_Showsettime(&HYMenu14); //显示当前的时间
        DisplayCommonMenu(&HYMenu14,NULL);
        break;


    case LCD_KEY3:				//上一页
        DisplayCommonMenu(&HYMenu12,NULL);
        break;

    case LCD_KEY4:
        Munu27_ShowSysInfo(0);   //显示费率
        break;

    case LCD_KEY15:
        DisSysConfigInfo.standaloneornet=DISP_NET;   //网络版
        Munu13_ShowSysInfo();
        rt_thread_mdelay(2000);
        nvic_system_reset();
        break;

    case LCD_KEY16:
        DisSysConfigInfo.standaloneornet=DISP_CARD; 	//单机正常版
        Munu13_ShowSysInfo();
        rt_thread_mdelay(2000);
        nvic_system_reset();
        break;

    case LCD_KEY17:
        DisSysConfigInfo.standaloneornet=DISP_CARD_mode; 	//单机预约版
        Munu13_ShowSysInfo();
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent14(_LCD_KEYVALUE *keyval)   //系统配置第2页
{
    switch(*keyval)
    {

    case LCD_KEY1:				//返回主页
        DispControl.CurSysState = DIP_STATE_NORMAL;    //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;

    case LCD_KEY2:				//返回上一页
        DisplayCommonMenu(&HYMenu13,NULL);
        break;
    case LCD_KEY3:				//下一页
        DisplayCommonMenu(&HYMenu15,NULL);
        break;

    case LCD_KEY6:
        Clear_record();  //清除记录
        rt_thread_mdelay(2000);
        nvic_system_reset();
        break;

    case LCD_KEY7:      //板子内部电表
        DisSysConfigInfo.energymeter=USERN8209;
        Munu13_ShowSysInfo();
        rt_thread_mdelay(2000);
        nvic_system_reset();
        break;
    case LCD_KEY8:      //外部电表
        DisSysConfigInfo.energymeter=NOUSERN8209;
        Munu13_ShowSysInfo();
        rt_thread_mdelay(2000);
        nvic_system_reset();
        break;


//    case LCD_KEY9:      //B0
//        DisSysConfigInfo.cardtype=B0card;
//        Munu13_ShowSysInfo();
//        break;
//    case LCD_KEY10:      //B1
//        DisSysConfigInfo.cardtype=B1card;
//        Munu13_ShowSysInfo();
//        break;

//    case LCD_KEY11:      //C0
//        DisSysConfigInfo.cardtype=C0card;
//        Munu13_ShowSysInfo();
//        break;
//    case LCD_KEY12:      //C1
//        DisSysConfigInfo.cardtype=C1card;
//        Munu13_ShowSysInfo();
//        break;


    case LCD_KEY18:				//时间确定按钮
        ReadDwinVariable(0x1705,15);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent15(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//主页
        DispControl.CurSysState = DIP_STATE_NORMAL;    //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;

    case LCD_KEY2:				//上一页
        DisplayCommonMenu(&HYMenu14,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent16(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY3:				//模式4 自动充满
//        Auto_charging();
        break;

    case LCD_KEY4:   //按照电量 跳到17
        DispControl.CurSysState = DIP_CARD_SHOW;  //无需界面再次切换
        DisplayCommonMenu(&HYMenu17,NULL);
        break;

    case LCD_KEY5:   //按照金额充电 跳到18
        DispControl.CurSysState = DIP_CARD_SHOW;  //无需界面再次切换
        DisplayCommonMenu(&HYMenu18,NULL);
        break;

    case LCD_KEY6:   //按照时间充电 跳到19
        DispControl.CurSysState = DIP_CARD_SHOW;  //无需界面再次切换
        DisplayCommonMenu(&HYMenu19,NULL);
        break;

    case LCD_KEY7:   //定时充 跳到20
        DispControl.CurSysState = DIP_CARD_SHOW;  //无需界面再次切换
        DisplayCommonMenu(&HYMenu20,NULL);
        break;


    case LCD_KEY15:
        if(Ad_entry())  //自动键控
        {
            DispControl.CurSysState = DIP_STATE_SYS;
            DisplayCommonMenu(&HYMenu12,NULL);

            rt_thread_mdelay(50); //延迟一下，不延迟模拟点击不能用
            simulation_Print(0x0064,0x0096); //自动键控值
        }
        break;


    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent17(_LCD_KEYVALUE *keyval)  //按照电量充电
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(&HYMenu16,NULL);
        break;

    case LCD_KEY8:				//3度
        PrintNum16uVariable(0x188D,3);
        break;
    case LCD_KEY9:				//6度
        PrintNum16uVariable(0x188D,6);
        break;
    case LCD_KEY10:				//9度
        PrintNum16uVariable(0x188D,9);
        break;
    case LCD_KEY11:				//12度
        PrintNum16uVariable(0x188D,12);
        break;

    case LCD_KEY14:				//按电量确定预约
        ReadDwinVariable(0x188D,1); //读出：数据变量显示里面的数据，然后发送串口，接收后，来处理
        break;
    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent18(_LCD_KEYVALUE *keyval)  //按照金额充电
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(&HYMenu16,NULL);
        break;

    case LCD_KEY8:				//5元
        PrintNum16uVariable(0x190D,5);
        break;
    case LCD_KEY9:				//10元
        PrintNum16uVariable(0x190D,10);
        break;
    case LCD_KEY10:				//15元
        PrintNum16uVariable(0x190D,15);
        break;
    case LCD_KEY11:				//20元
        PrintNum16uVariable(0x190D,20);
        break;

    case LCD_KEY14:				//确定按钮：按金额
        ReadDwinVariable(0x190D,1); //读出：数据变量显示里面的数据，然后发送串口，接收后，来处理
        break;

    default:
        break;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent19(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(&HYMenu16,NULL);
        break;

    case LCD_KEY8:				//充30分钟
        PrintNum16uVariable(0x198D,30);
        break;
    case LCD_KEY9:				//充60分钟
        PrintNum16uVariable(0x198D,60);
        break;
    case LCD_KEY10:				//充90分钟
        PrintNum16uVariable(0x198D,90);
        break;
    case LCD_KEY11:				//充120分钟
        PrintNum16uVariable(0x198D,120);
        break;
    case LCD_KEY14:				//确定按钮：按时间
        ReadDwinVariable(0x198D,1); //读出：数据变量显示里面的数据，然后发送串口，接收后，来处理
        break;

    default:
        break;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent20(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        DisplayCommonMenu(&HYMenu16,NULL);
        break;

    case LCD_KEY8:				//6点
        PrintNum16uVariable(0x1A0D,6);
        break;
    case LCD_KEY9:				//12点
        PrintNum16uVariable(0x1A0D,12);
        break;
    case LCD_KEY10:				//18点
        PrintNum16uVariable(0x1A0D,18);
        break;
    case LCD_KEY11:				//23点
        PrintNum16uVariable(0x1A0D,23);
        break;
    case LCD_KEY14:				//确定按钮：按时间
        ReadDwinVariable(0x1A0D,1); //读出：数据变量显示里面的数据，然后发送串口，接收后，来处理
        break;

    default:
        break;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent21(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DisplayCommonMenu(HYMenu14.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent22(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    case LCD_KEY2:				//返回上一页（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    case LCD_KEY18:				//时间确定按钮
        ReadDwinVariable(0x1B05,15);
        break;

    case LCD_KEY9: //B枪刷充电按钮
        Click_card(GUN_B);  //插枪时 才能点击，才可以跳转25刷卡界面
        break;


    default:
        break;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent23(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DisplayCommonMenu(HYMenu14.Menu_PrePage,NULL);
        break;

    case LCD_KEY8: //A枪刷充电按钮
        Click_card(GUN_A);  //插枪时 才能点击，才可以跳转25刷卡界面
        break;

    default:
        break;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent24(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY2:				//返回上一页
        DisplayCommonMenu(HYMenu14.Menu_PrePage,NULL);
        break;

    default:
        break;
    }
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYKeyEvent26(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回上一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}
void HYKeyEvent27(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;
    case LCD_KEY2:				//返回上一页
        Munu27_ShowSysInfo(1);
        break;

    case LCD_KEY4:				//下一页
        Munu27_ShowSysInfo(2);
        break;

    default:
        break;
    }
}

void HYKeyEvent28(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;
    case LCD_KEY2:				//返回上一页
        Munu27_ShowSysInfo(1);
        break;
    default:
        break;
    }
}


void HYKeyEvent30(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;
    case LCD_KEY2:				//返回上一页 可能是第一页，还有可能是记录信息
        APP_SelectUpNChargeRecode(1);
        break;

    case LCD_KEY3:				//下一页 一定是数据
        APP_SelectNextNChargeRecode(1);
        break;

    case LCD_KEY4:				//上5条
        APP_SelectUpNChargeRecode(10);
        break;

    case LCD_KEY5:				//下5条
        APP_SelectNextNChargeRecode(10);
        break;

    default:
        break;
    }
}

void HYKeyEvent31(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;

    case LCD_KEY2:				//返回上一页 可能是第一页，还有可能是记录信息
        APP_SelectUpNChargeRecode(1);
        break;

    case LCD_KEY3:				//上5条
        APP_SelectUpNChargeRecode(10);
        break;

    default:
        break;
    }
}


void HYKeyEvent32(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
    case LCD_KEY2:				//返回上一页 可能是第一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}


void HYKeyEvent33(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;
    case LCD_KEY2:				//返回上一页 可能是第一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}


void HYKeyEvent34(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;
    case LCD_KEY2:				//返回上一页 可能是第一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}


void HYKeyEvent35(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;  //状态标志位，非配置和系统升级页面==[为了主页面实时判断状态]
        break;
    case LCD_KEY2:				//返回上一页 可能是第一页
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}


void HYKeyEvent36(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;

    case LCD_KEY2:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}


void HYKeyEvent37(_LCD_KEYVALUE *keyval)
{
    switch(*keyval)
    {
    case LCD_KEY1:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;

    case LCD_KEY2:				//返回主页面（可以是故障页面，还有可能是充电页面）
        DispControl.CurSysState = DIP_STATE_NORMAL;
        break;
    default:
        break;
    }
}




/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void DealWithKey(_LCD_KEYVALUE *keyval)
{
    const ST_Menu* CurMenu = GetCurMenu();

    if (CurMenu || CurMenu->function2)
    {
        CurMenu->function2(keyval);
    }
}
