/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
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
#include "4GMain.h"
#include "4GRecv.h"
#include <string.h>
#include "common.h"
#include "dwin_com_pro.h"
/* Private define-----------------------------------------------------------------------------*/
#define GPRS_TEMP_BUF_LEN          (2500u)      //显示任务保存一帧数据的buf大小

/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
#define UART_SUBFRAME_TIME   10  //串口分帧时间
uint8_t GPRSTempBuf[GPRS_TEMP_BUF_LEN];            // 显示保存一帧数据缓存


_RECV_DATA_CONTROL RecvDataControl[LINK_NET_NUM];

int hy4g_port_init(void);
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/
//专门做GPRS接收处理


/*****************************************************************************
* Function     : APP_RecvDataControl
* Description  :
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年6月14日
*****************************************************************************/
_RECV_DATA_CONTROL	* APP_RecvDataControl(uint8_t num)
{
    if(num >= NetConfigInfo[NET_YX_SELCT].NetNum)
    {
        return NULL;
    }
    return &RecvDataControl[num];
}

//专门做GPRS接收处理
/*****************************************************************************
* Function     : TaskGPRSRecv
* Description  : 串口测试任务
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年6月16日
*****************************************************************************/
void Task4GRecv(void *pdata)
{
    uint8_t err,i;
    int iRxLen = 0;
    //连续10次未收到数据，则主动读取数据,连续40次则重启
    static uint32_t NetRset[LINK_NET_NUM] = {0};


    static uint32_t lastSysTime[LINK_NET_NUM] = {0};
    static uint32_t nowSysTime[LINK_NET_NUM] = {0};
    rt_thread_mdelay(2000);
    if(DisSysConfigInfo.standaloneornet != DISP_NET)
    {
        return;
    }
    hy4g_port_init();
    for(i = 0; i < NetConfigInfo[NET_YX_SELCT].NetNum; i++)
    {
        nowSysTime[i] = rt_tick_get();
        lastSysTime[i] = nowSysTime[i];
    }
    RT_ASSERT(Uart4GCom.read != RT_NULL);
    while(1)
    {
        //从串口读取一个消息GPRSTempBuf[GPRS_TEMP_BUF_LEN]
        for(i = 0; i < NetConfigInfo[NET_YX_SELCT].NetNum; i++)
        {
            nowSysTime[i] = rt_tick_get();
        }
        if(NetConfigInfo[NET_YX_SELCT].NetNum  == 0)
        {
            iRxLen = 0;
        }
        iRxLen = Uart4GCom.read(&Uart4GCom,GPRSTempBuf,GPRS_TEMP_BUF_LEN);
        if(iRxLen > 0 && iRxLen <= GPRS_TEMP_BUF_LEN)
        {
            //如果已经连接上服务器
            if (NetConfigInfo[NET_YX_SELCT].NetNum == 1)
            {
                if(APP_GetModuleConnectState(0) == STATE_OK) //已经连接上后台了
                {
//					if((GPRSTempBuf[0] == 0x0d) && (GPRSTempBuf[1] == 0x0a) )
//					{
//						APP_SetNetNotConect(0);   //调试的时候出现0d 0a
//					}
//
                    if(_4G_RecvFrameDispose(GPRSTempBuf,iRxLen))  //数据透传
                    {
                        lastSysTime[0] = nowSysTime[0];
                    }

                    if(APP_GetSIM7600Mode() == MODE_HTTP)
                    {
                        SIM7600_RecvDesposeCmd(GPRSTempBuf,iRxLen); //未连接上服务器，AT指令处理
                        GPRSTempBuf[2000] = '\0';  //防止无线打印
                        rt_kprintf("rx %s\r\n",GPRSTempBuf);
                    }
                    else
                    {
                        rt_kprintf("rx len:%d,rx data:",iRxLen);
                        for(i = 0; i < iRxLen; i++)
                        {
                            rt_kprintf("%02x ",GPRSTempBuf[i]);
                        }
                        rt_kprintf("\r\n");
                    }
                }
                else
                {
                    SIM7600_RecvDesposeCmd(GPRSTempBuf,iRxLen); //未连接上服务器，AT指令处理

                    GPRSTempBuf[2000] = '\0';  //防止无线打印
                    rt_kprintf("rx %s\r\n",GPRSTempBuf);

                }
            }
            else
            {
                //未连接上服务器，AT指令处理
                SIM7600_RecvDesposeCmd(GPRSTempBuf,iRxLen);
                for(i = 0; i < NetConfigInfo[NET_YX_SELCT].NetNum; i++)
                {
                    if(RecvDataControl[i].RecvStatus == RECV_FOUND_DATA)
                    {
                        RecvDataControl[i].RecvStatus = RECV_NOT_DATA;
                        //接收数据处理

                        //临时接收什么发送什么
                        //ModuleSIM7600_SendData(i,RecvDataControl[i].DataBuf,RecvDataControl[i].len);
                        if(i == 0)
                        {
                            _4G_RecvFrameDispose(RecvDataControl[i].DataBuf,RecvDataControl[i].len);
                        }
                        else
                        {
                            //政府平台,有数据返回就是注册成功
                            APP_SetAppRegisterState(1,STATE_OK);
                        }
                        lastSysTime[i] = nowSysTime[i];
                        NetRset[i] = 0;
                    }
                }
            }
        }

        for(i = 0; i < NetConfigInfo[NET_YX_SELCT].NetNum; i++)
        {
            if(APP_GetModuleConnectState(i) == STATE_OK) //已经连接上后台了
            {
                if (NetConfigInfo[NET_YX_SELCT].NetNum > 1)
                {
                    if(++NetRset[i] >= (CM_TIME_15_SEC /UART_SUBFRAME_TIME))  // 15s
                    {
                        NetRset[i] = 0;
                        /*if(i == 0)
                        {
                        	send_message[i].MsgID = BSP_4G_MAIN;
                        	send_message[i].DivNum = BSP_4G_SENDNET1;
                        }else{
                        	send_message[i].MsgID = BSP_4G_MAIN;
                        	send_message[i].DivNum = BSP_4G_SENDNET2;
                        }
                        OSQPost(psendevent, &send_message[i]);*/
                    }
                }

                //重启时间根据周期性发送数据再放余量
#if(NET_YX_SELCT == XY_AP)
                {
                    if((nowSysTime[i] >= lastSysTime[i]) ? ((nowSysTime[i] - lastSysTime[i]) >= CM_TIME_60_SEC) : \
                            ((nowSysTime[i] + (0xffffffff - lastSysTime[i])) >= CM_TIME_60_SEC))
                    {
                        lastSysTime[i] = nowSysTime[i];
                        APP_SetNetNotConect(i);
                    }
                }
#else
                {
                    if((nowSysTime[i] >= lastSysTime[i]) ? ((nowSysTime[i] - lastSysTime[i]) >= CM_TIME_30_SEC) : \
                            ((nowSysTime[i] + (0xffffffff - lastSysTime[i])) >= CM_TIME_30_SEC))
                    {
                        lastSysTime[i] = nowSysTime[i];
                        APP_SetNetNotConect(i);
                    }
                }
#endif

            }
            else
            {
                lastSysTime[i] = nowSysTime[i];
            }
        }
    }

}


/* l501 采集设备句柄 */
static rt_device_t hy_device = RT_NULL;
/* l501 采集接收信号量 */
static struct rt_semaphore hy_receive_sem;
/* 串口配置参数 */
struct serial_configure uart2_config = RT_SERIAL_CONFIG_DEFAULT;
/* 万桩使用的串口名 */
#define HY4G_SERIAL_NAME "uart3"
/* 硬件拓展结构体 */
typedef struct
{
    /* 用于串口接收的信号量 */
    rt_sem_t wz_sem;
    /* 字节间的超时时间 */
    uint32_t byte_timeout;
} hy_port_t;

/*硬件拓展结构体变量 */
static hy_port_t hy_port =
{
    /* 用于串口接收的信号量 */
    .wz_sem = RT_NULL,
    /* 接收字节间超时时间 */
    .byte_timeout = UART_SUBFRAME_TIME,
};

/* 串口接收数据回调函数 */
rt_err_t uart2_handler(rt_device_t dev, rt_size_t size)
{
    /* 接收到一个数据释放信号量 */
    rt_sem_release(&hy_receive_sem);
    return RT_EOK;
}
/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
int hy4g_port_init(void)
{
    /* 串口初始化 */
    hy_device = rt_device_find(HY4G_SERIAL_NAME);

    if (hy_device == RT_NULL)
    {
        rt_kprintf("cannot find device %s \r\n", HY4G_SERIAL_NAME);
        return -RT_ERROR;
    }

    if (rt_device_open(hy_device, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("cannot open device %s \r\n", HY4G_SERIAL_NAME);
        return -RT_ERROR;
    }
    else
    {
        uart2_config.baud_rate = BAUD_RATE_115200;
        uart2_config.data_bits = DATA_BITS_8;
        uart2_config.stop_bits = STOP_BITS_1;
        uart2_config.parity = PARITY_NONE;
        rt_device_control(hy_device, RT_DEVICE_CTRL_CONFIG, &uart2_config);
        rt_kprintf("device %s open success \r\n", HY4G_SERIAL_NAME);
    }

    /* 信号量初始化 */
    if (rt_sem_init(&hy_receive_sem, "receive_sem", 0, RT_IPC_FLAG_FIFO) == RT_EOK)
    {
        hy_port.wz_sem = &hy_receive_sem;
    }
    else
    {
        return -RT_ERROR;
    }

    /* 设置串口接收回调函数 */
    rt_device_set_rx_indicate(hy_device, uart2_handler);
    return  RT_EOK;
}

/**
 * @brief  硬件层接收数据
 * @param[in] @ctx:运行环境
 *            @msg:接收数据存放地址
 *            @len:数据最大接收长度
 * @param[out]
 * @return
 * @note
 */
static int hy_hw_read(HY_COM_T *ctx, uint8_t *msg ,uint16_t len)
{
    /* 实际接收长度 */
    int read_len = 0;
    /* 清缓存变量 */
//    uint8_t buf = 0;

//    /* 清空缓存 */
//    while(rt_device_read(hy_device,0,&buf,1));
    /* 等待串口接收到数据 */
    if(rt_sem_take(&hy_receive_sem, 1000) == -RT_ETIMEOUT)
    {
        return 0;
    }
    /* 每次读取一个字节的数据 */
    while (rt_device_read(hy_device, 0, msg + read_len, 1) == 1)
    {
//        if(read_len > len)
//        {
//            return 0;
//        }
//        else
//        {
        read_len ++;
        if(read_len >= len)             // 数据已经到达
        {
            break;
        }
//        }
        /* 读取超时标志一帧数据读取完成 */
        if (rt_sem_take(&hy_receive_sem, ((hy_port_t *)(ctx->port_data))->byte_timeout) == -RT_ETIMEOUT)
        {
            break;
        }
    }
    return read_len;
}

/**
 * @brief
 * @param[in] @ctx:运行环境
 *            @msg:接收数据存放地址
 *            @len:数据最大接收长度
 * @param[out]
 * @return
 * @note
 */
static int hy_hw_write(uint8_t *buf, uint16_t len)
{
    /* 串口发送数据 */
    return rt_device_write(hy_device,0,buf,len);
}

HY_COM_T   Uart4GCom=
{
    {0},
    0,
    hy_hw_write,
    hy_hw_read,
    (void *)&hy_port
};


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
static int _4G_recv_init(void)
{

    rt_thread_t _4g_recv_thread = NULL;


    _4g_recv_thread = rt_thread_create("_4G_recv_init",
                                       Task4GRecv,
                                       RT_NULL,
                                       4096,
                                       15,  		//优先级
                                       20);

    if (_4g_recv_thread != RT_NULL)
    {
        rt_thread_startup(_4g_recv_thread);
    }
    else
    {
        goto __exit;
    }

    return RT_EOK;

__exit:
    if (_4g_recv_thread)
    {
        rt_thread_delete(_4g_recv_thread);
    }

    return RT_ERROR;
}

INIT_APP_EXPORT(_4G_recv_init);
/************************(C)COPYRIGHT 2020 杭州汇誉*****END OF FILE****************************/

