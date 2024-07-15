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
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "chtask.h"
#include "ch_port.h"
#include "rn8209d.h"
#include "common.h"
#include "w25qxx.h"
#include "dwin_com_pro.h"
#define CH_THREAD_STK_SIZE     2048
#define CH_THREAD_PRIO         RT_THREAD_PRIORITY_MAX-10
#define CH_THREAD_TIMESLICE    10
#define CH_MSG_MAX_SIZE        256

rt_thread_t pstChThread = RT_NULL;
CH_TASK_T stChTcb[GUN_MAX] = {0};
CH_TASK_T *pstCh;

/* 消息队列中用到的放置消息的内存池 */
uint8_t ucaMsgBuf[CH_MSG_MAX_SIZE] = {0};



/**
 * @brief 充电任务线程数据初始化
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void ch_task_init(void)
{
    mq_service_bind(CM_CHTASK_MODULE_ID,"ch task mq");

    cp_adc_init();

    cp2_adc_init(); //CP2 ADC初始化

    ch_ctl_init();

    cp_pwm_init();

//	ch_rate_init();

//	ch_ctl_enable();
//	rt_thread_mdelay(300);
    ch_ctl_disable(GUN_A);
    ch_ctl_disable(GUN_B);

    rt_thread_mdelay(200);
    rn8209d_init(); //电表初始化
    if(DisSysConfigInfo.energymeter == 1)
    {
        input_io_init(&stChTcb[GUN_A].stRn8209);
        input_io_init(&stChTcb[GUN_B].stRn8209);
    }
    else
    {
        input_io_init(&stChTcb[GUN_A].stCh);
			  //input_io_init(&stChTcb[GUN_B].stCh);
    }

    spiflash_init();  //flash初始化
    return;
}

/**
 * @brief 获取cp状态信息(12v,9v,6v)
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void ch_get_cp_state_info(_GUN_NUM gun)
{
    if(gun >= GUN_MAX)
    {
        return;
    }
	
    stChTcb[gun].stGunStat.PWMcycle = ch_get_cp_volt(gun,&stChTcb[gun].stGunStat.uiCpVolt); //返回PWM占空比

    if((CP_12V_MAX >= stChTcb[gun].stGunStat.uiCpVolt) && (CP_12V_MIN  <= stChTcb[gun].stGunStat.uiCpVolt))          /* 在12V 范围 */
    {
        stChTcb[gun].stGunStat.ucCpStat = CP_12V;
        return;
    }

    if((CP_9V_MAX >= stChTcb[gun].stGunStat.uiCpVolt) && (CP_9V_MIN <= stChTcb[gun].stGunStat.uiCpVolt))              /* 在9V 范围 */
    {
        stChTcb[gun].stGunStat.ucCpStat = CP_9V;
        return;
    }

    if((CP_6V_MAX >= stChTcb[gun].stGunStat.uiCpVolt) && (CP_6V_MIN <= stChTcb[gun].stGunStat.uiCpVolt))             /* 在6V 范围 */
    {
        stChTcb[gun].stGunStat.ucCpStat = CP_6V;
        return;
    }
    stChTcb[gun].stGunStat.ucCpStat = CP_ABNORMAL;                    /* 异常 */
}

extern SYSTEM_RTCTIME gs_SysTime;
/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
static uint8_t ch_from_ctl_msg(CH_CTL_T *pstChCtl)
{
    if(pstChCtl->gun >= GUN_MAX)
    {
        return FALSE;
    }
    if(CH_START_CTL == pstChCtl->ucCtl)                                             /* 充电启动 */
    {
        if((stChTcb[pstChCtl->gun].ucState == CHARGING) ||(stChTcb[pstChCtl->gun].ucState == WAIT_CAR_READY)||(stChTcb[pstChCtl->gun].ucState == PRECHARGE))
        {
            return 0;
        }
        if(stChTcb[pstChCtl->gun].ucState == INSERT)                                               /* 在插枪状态可以启动 */
        {
            stChTcb[pstChCtl->gun].stChCtl.ucCtl = CH_START_CTL;
            stChTcb[pstChCtl->gun].stChCtl.ucChStartMode = pstChCtl->ucChStartMode;
            stChTcb[pstChCtl->gun].stChCtl.ucChMode = pstChCtl->ucChMode;
            stChTcb[pstChCtl->gun].stChCtl.uiStopParam = pstChCtl->uiStopParam;

        }
        else
        {
            memset(&stChTcb[pstChCtl->gun].stCh,0,sizeof(CH_T));
            stChTcb[pstChCtl->gun].stCh.uiChStartTime = gs_SysTime;
            stChTcb[pstChCtl->gun].stCh.timestart = time(RT_NULL);
            mq_service_ch_send_dip(CH_TO_DIP_STARTFAIL ,UNPLUG ,pstChCtl->gun,NULL);  //枪未插

        }
    }
    else
    {
        if((CH_START_CTL == stChTcb[pstChCtl->gun].stChCtl.ucCtl) )
        {
            stChTcb[pstChCtl->gun].stChCtl.ucCtl        = CH_STOP_CTL;
        }
    }
    //mq_service_ch_send_msg_to_tcu(CH_TO_TCU_CHCTL_ACK,0,sizeof(CHCTL_ACK_T),(uint8_t *)&stChCtlAckData);
    return 1;

}


/**
 * @brief 处理tcu消息
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void ch_task_tcu_msg_proc(MQ_MSG_T *pstMsg)
{
    switch(pstMsg->uiMsgCode)
    {
    case TCU_TO_CH_CHCLT:
        ch_from_ctl_msg((CH_CTL_T *)pstMsg->pucLoad);
        break;
    default:
        break;
    }
}


/**
 * @brief 消息处理
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
static void ch_msg_proc(MQ_MSG_T *pstMsg)
{
    switch(pstMsg->uiSrcMoudleId)
    {
        /* 目前TCU 模块和刷卡模块只向充电任务功能模块发送了充电控制消息 */
    case CM_TCUTASK_MODULE_ID:
        rt_kprintf("msg src module id:%d, pstMsg->uiMsgCode = %d\n\r",pstMsg->uiSrcMoudleId,pstMsg->uiMsgCode);
        ch_task_tcu_msg_proc(pstMsg);
        break;
    default:
        rt_kprintf("msg src module id:%d\n\r",pstMsg->uiSrcMoudleId);
        break;
    }
}

/**
 * @brief 充电任务等待消息
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
static void ch_wait_msg(void)
{
    MQ_MSG_T stMsg = {0};

    if(mq_service_recv_msg(CM_CHTASK_MODULE_ID,&stMsg,ucaMsgBuf,sizeof(ucaMsgBuf),CM_TIME_20_MSEC) == 0)
    {
        ch_msg_proc(&stMsg);
    }
}


uint32_t  uiTimeTest = 0;
/**
 * @brief 周期性获取rn8209信息
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void ch_cycle_get_8209(_GUN_NUM gun)
{
    uint32_t uiTick = 0;

    uiTick = rt_tick_get();

    if(gun >= GUN_MAX)
    {
        return;
    }
    if(uiTick - stChTcb[gun].stRn8209.uiGetTime > CH_TIME_500_MSEC)
    {
        stChTcb[gun].stRn8209.uiGetTime = uiTick;
        stChTcb[gun].stRn8209.usCurrent = rn8209d_get_current(gun);
        stChTcb[gun].stRn8209.usVolt = rn8209d_get_volt(gun);
        rn8209d_get_TotalE(gun,&stChTcb[gun].stRn8209.uiE);

        uiTimeTest  = time(RT_NULL);
    }
}





/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void ch_task_cycle_proc(_GUN_NUM gun)
{
    if(DisSysConfigInfo.energymeter == 1)
    {
        ch_cycle_get_8209(gun);
    }
}
/**
 * @brief 充电任务线程入口
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
static void ch_thread_entry(void* parg)
{
    uint8_t gun;
    ch_task_init();
    while(1)
    {
        ch_wait_msg();
        for(gun = 0; gun < DisSysConfigInfo.GunNum; gun++)
        {
            ch_get_cp_state_info(gun);   	//CP电压检测
            ch_task_cycle_proc(gun) ; 		//计量信息读取
            ch_loop_proc(gun,&stChTcb[gun]);  //充电机充电流程
        }
        rt_thread_mdelay(1);
    }
}

/**
 * @brief 充电任务线程初始化
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
static int ch_thread_init(void)
{
    pstChThread = rt_thread_create("CH_Thread",
                                   ch_thread_entry,
                                   RT_NULL,
                                   CH_THREAD_STK_SIZE,
                                   CH_THREAD_PRIO,
                                   CH_THREAD_TIMESLICE);

    if(RT_NULL != pstChThread)
    {
        rt_kprintf("The ch thread create suceess\r\n");
        rt_thread_startup(pstChThread);
    }
    else
    {
        goto __exit;
    }
    return RT_EOK;

__exit:
    if (pstChThread)
    {
        rt_thread_delete(pstChThread);
    }
    return RT_ERROR;
}
INIT_APP_EXPORT(ch_thread_init);
