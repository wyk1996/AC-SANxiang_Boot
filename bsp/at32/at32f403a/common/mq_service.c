/*******************************************************************************
 * @file
 * @note
 * @brief
 *
 * @author
 * @date     2021-10-23
 * @version  V1.0.0
 *
 * @Description  消息队列服务
 *
 * @note History:
 * @note     <author>   <time>    <version >   <desc>
 * @note
 * @warning
 *******************************************************************************/

#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>
#include <string.h>
#include "common.h"

/*
===============================================================================================================================
                                            ****** 类型定义/宏定义 ******
===============================================================================================================================
*/

#define        MQ_SERVICE_MSG_MAX_SIZE     sizeof(MQ_MSG_T)
#define        MQ_SERVICE_MSG_MAX_NUM      8

typedef struct
{
    uint32_t       uiModuleId;          /* 功能模块ID */
    rt_mq_t        pMQ;                 /* 消息队列 */
    const char    *sMqName;             /* 消息队列名称 */
} MQ_ID_MAPP_T;

/* 消息与功能模块ID映射节点 */
typedef struct _mq_id_mapp_node
{

    /* 指向下一个节点 */
    struct _mq_id_mapp_node   *pstNext;
    /* 消息id映射 */
    MQ_ID_MAPP_T               stMqMapp;
} MQ_ID_MAPP_NODE;

/* 消息与功能模块ID映射有效链表 */
typedef struct
{
    /* 头节点 */
    MQ_ID_MAPP_NODE    *pstHead;
    /* 尾节点 */
    MQ_ID_MAPP_NODE    *pstTail;
} MQ_ID_MAPP_LIST;

/*
===============================================================================================================================
                                            ****** 变量定义 ******
===============================================================================================================================
*/

MQ_ID_MAPP_LIST      staMqMappList = {0};
/*
===============================================================================================================================
                                            ****** 函数定义/声明 ******
===============================================================================================================================
*/

/**
 * @brief 功能模块id是否已经存在
 * @param[in] uiModuleId:功能模块ID
 * @param[out]
 * @return RT_NULL:功能模块ID没有绑定 非空:功能模块ID对应的节点指针
 * @note
 */
MQ_ID_MAPP_NODE  * mq_service_moduleid_exist(uint32_t uiModuleId)
{
    MQ_ID_MAPP_NODE  *pstNextNode = RT_NULL;

    pstNextNode = staMqMappList.pstHead;

    while(pstNextNode != RT_NULL)
    {
        /* 已经存在  */
        if(pstNextNode->stMqMapp.uiModuleId == uiModuleId)
        {
            return pstNextNode;
        }

        pstNextNode = pstNextNode->pstNext;
    }

    return RT_NULL;
}

/**
 * @brief  功能模块id与消息队列绑定
 * @param[in]
 * @param[out]
 * @return 0 绑定成功
 * @note
 */
uint8_t mq_service_bind(uint32_t  uiModuleId,const char *sMqName)
{
    MQ_ID_MAPP_NODE  *pstNode = RT_NULL;
    rt_mq_t          pMq      = RT_NULL;

    /* 模块ID是否已经存在 */
    if(mq_service_moduleid_exist(uiModuleId) != RT_NULL)
    {
        rt_kprintf("module id:%d already bound\r\n",uiModuleId);
        return 1;
    }

    /* 分配节点 */
    pstNode = rt_malloc(sizeof(MQ_ID_MAPP_NODE));
    if(pstNode == RT_NULL)
    {
        rt_kprintf("Failed to allocate node when module ID is bound,module id:%d\r\n",uiModuleId);
        return 2;
    }

    /* 申请消息队列 */
    pMq = rt_mq_create(sMqName, MQ_SERVICE_MSG_MAX_SIZE, MQ_SERVICE_MSG_MAX_NUM, RT_IPC_FLAG_FIFO);
    if(pMq == RT_NULL)
    {
        rt_free(pstNode);
        rt_kprintf("Failed to allocate mq when module ID is bound,module id:%d\r\n",uiModuleId);
        return 3;
    }

    pstNode->pstNext              = RT_NULL;
    pstNode->stMqMapp.uiModuleId  = uiModuleId;
    pstNode->stMqMapp.sMqName     = sMqName;
    pstNode->stMqMapp.pMQ         = pMq;

    rt_enter_critical();

    /* 队列里还没有有效绑定的节点 */
    if(staMqMappList.pstHead == RT_NULL)
    {
        /* 直接添加到头节点 */
        staMqMappList.pstHead = pstNode;
        staMqMappList.pstTail = pstNode;
        rt_exit_critical();
        return 0;
    }

    /* 队列里已经有节点了，尾节点为空链表出现异常 */
    if(staMqMappList.pstTail == RT_NULL)
    {
        rt_free(pstNode);
        rt_mq_delete(pMq);
        rt_kprintf("The linked list is abnormal when the module ID is bound ,module id:%d\r\n",uiModuleId);
        rt_exit_critical();
        return 4;
    }

    /* 添加到尾节点下面 */
    staMqMappList.pstTail->pstNext = pstNode;
    /* 尾节点指向当前节点 */
    staMqMappList.pstTail  = pstNode;
    rt_exit_critical();

    return 0;
}

/**
 * @brief 发送消息到指定的功能模块
 * @param[in]  uiSrcMoudleId  ：消息源头的功能模块
 *             uiDestMoudleId ：需要发送到的功能模块
 *             uiMsgCode      ：消息码
 *             uiMsgVar       ：消息参数
 *             uiLoadLen      ：消息有效载荷长度
 *             pucLoad        ：指向需要发送的有效载荷地址
 * @param[out]
 * @return 0: 发送成功  非0: 发送失败
 * @note 内部调用
 */
uint8_t mq_service_send_msg(uint32_t uiSrcMoudleId, uint32_t  uiDestMoudleId, uint32_t  uiMsgCode,
                            uint32_t uiMsgVar, uint32_t  uiLoadLen, uint8_t  *pucLoad)
{
    MQ_ID_MAPP_NODE  *pstNode = RT_NULL;
    MQ_MSG_T         stMsg    = {0};
    uint8_t          *pucBuf  = RT_NULL;
    rt_err_t         rtErrT   = RT_EOK;

    pstNode = mq_service_moduleid_exist(uiDestMoudleId);

    /* 目标功能模块还未绑定消息队列 */
    if(pstNode == RT_NULL)
    {
        rt_kprintf("Message sending failed, the target module ID has not been bound to the message queue\r\n");
        return 1;
    }

    if((pucLoad != RT_NULL) && (uiLoadLen > 0))
    {
        pucBuf = rt_malloc(uiLoadLen);

        if(pucBuf == RT_NULL)
        {
            rt_kprintf("Message sending failed, failed to allocate buffer \r\n");
            return 2;
        }
        memcpy(pucBuf,pucLoad,uiLoadLen);
    }

    stMsg.uiSrcMoudleId    = uiSrcMoudleId;
    stMsg.uiDestMoudleId   = uiDestMoudleId;
    stMsg.uiMsgCode        = uiMsgCode;
    stMsg.uiMsgVar         = uiMsgVar;
    stMsg.uiLoadLen        = uiLoadLen;
    stMsg.pucLoad          = pucBuf;

    rtErrT = rt_mq_send(pstNode->stMqMapp.pMQ,&stMsg,sizeof(MQ_MSG_T));

    /* 调用操作系统消息队列消息发送失败 */
    if(rtErrT != RT_EOK)
    {
        if(pucBuf != RT_NULL)
        {
            rt_free(pucBuf);
        }
        rt_kprintf("uiSrcMoudleId: %d send msg to uiDestMoudleId : %d failed,\
		   call the os mq to send the message API to feedback the sending failure,err code:%d\r\n",uiSrcMoudleId,uiDestMoudleId,rtErrT) ;
        return 3;
    }
    return 0;
}
/**
 * @brief   指定的功能模块接收消息
 * @param[in]  uiSrcMoudleId ：需要接收消息源头的功能模块
 *             uiBufSize     ：接收buf的尺寸
 *             rtTimeout     ：等待超时的时间
 * @param[out] pstMsg ： 指向存储接收到的消息缓冲区
*              pucMsgBuf：指向存储接收的消息的有效载荷
 * @return 0 ：发送成功  非0：发送失败
 * @note
 */
uint8_t mq_service_recv_msg(uint32_t uiSrcMoudleId,MQ_MSG_T *pstMsg,uint8_t *pucMsgBuf,uint32_t uiBufSize,rt_int32_t rtTimeout)
{
    MQ_ID_MAPP_NODE  *pstNode = RT_NULL;
    uint32_t          uiRxValidLen = 0;

    RT_ASSERT(pstMsg    != RT_NULL);
    RT_ASSERT(pucMsgBuf != RT_NULL);
    RT_ASSERT(uiBufSize > 0);

    pstNode = mq_service_moduleid_exist(uiSrcMoudleId);
    if(pstNode == RT_NULL)                                         	/* 该功能模块还未绑定消息队列 */
    {
        rt_kprintf("Message sending failed, the target module ID has not been bound to the message queue\r\n");
        return 1;
    }

    if (rt_mq_recv(pstNode->stMqMapp.pMQ, pstMsg, sizeof(MQ_MSG_T) , rtTimeout) == RT_EOK)          /*接收消息 */
    {
        if(pstMsg->pucLoad != RT_NULL)        /* 有有效载荷，需要copy到应用层提供的缓冲区,因为消息发送使用动态内存分配，需要释放 */
        {
            uiRxValidLen  = CM_DATA_GET_MIN(uiBufSize,pstMsg->uiLoadLen);

            memcpy(pucMsgBuf,pstMsg->pucLoad,uiRxValidLen);
            rt_free(pstMsg->pucLoad);

            pstMsg->pucLoad    = pucMsgBuf;
            pstMsg->uiLoadLen  = uiRxValidLen;
        }

        return 0;
    }

    return 2;
}
/**
 * @brief 功能模块内部发送消息给功能模块自己
 * @param[in]  uiMoudleId ：功能模块ID
 *             uiMsgCode  ：消息码
 *             uiMsgVar   ：消息参数
 *             uiLoadLen  ：消息有效载荷长度
 *             pucLoad    ：指向需要发送的有效载荷地址
 * @param[out]
 * @return 0 ：发送成功  非0：发送失败
 * @note
 */
uint8_t mq_service_xxx_send_msg_to_xxx(uint32_t uiMoudleId, uint32_t  uiMsgCode,
                                       uint32_t  uiMsgVar, uint32_t  uiLoadLen, uint8_t  *pucLoad)
{
    return mq_service_send_msg(uiMoudleId,uiMoudleId,uiMsgCode,uiMsgVar,uiLoadLen,pucLoad);
}

/**
 * @brief   发送消息到充电任务模块
 * @param[in]  uiSrcMoudleId ：消息源头的功能模块
 *             uiMsgCode     ：消息码
 *             uiMsgVar      ：消息参数
 *             uiLoadLen     ：消息有效载荷长度
 *             pucLoad       ：指向需要发送的有效载荷地址
 * @param[out]
* @return 0:发送成功  非0:发送失败
 * @note
 */
uint8_t mq_service_xxx_send_msg_to_chtask(uint32_t uiSrcMoudleId, uint32_t  uiMsgCode,
        uint32_t uiMsgVar, uint32_t uiLoadLen, uint8_t  *pucLoad)
{
    return mq_service_send_msg(uiSrcMoudleId,CM_CHTASK_MODULE_ID,uiMsgCode,uiMsgVar,uiLoadLen,pucLoad);
}



/**
 * @brief   刷卡任务发送消息到显示任务
 * @param[in]  uiMsgCode ：消息码
 *             uiMsgVar  ：消息参数
 *             uiLoadLen ：消息有效载荷长度
 *             pucLoad   ：指向需要发送的有效载荷地址
 * @param[out]
 * @return 0 ：发送成功  非0：发送失败
 * @note
 */
uint8_t mq_service_card_send_disp(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad)
{
    return mq_service_send_msg(CM_CARD_MODULE_ID,CM_DISP_MODULE_ID,uiMsgCode,uiMsgVar,uiLoadLen,pucLoad) ;
}



/**
* @brief   充电模块发送给显示任务
 * @param[in]  uiMsgCode ：停止原因
 *             uiMsgVar  ：消息参数
 *             uiLoadLen ：消息有效载荷长度
 *             pucLoad   ：指向需要发送的有效载荷地址
 * @param[out]
 * @return 0 ：发送成功  非0：发送失败
 * @note
 */
uint8_t mq_service_ch_send_dip(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad)
{
    return mq_service_send_msg(CM_CHTASK_MODULE_ID,CM_DISP_MODULE_ID,uiMsgCode,uiMsgVar,uiLoadLen,pucLoad) ;
}


/**
* @brief   发送给4G发送任务
 * @param[in]  uiMsgCode ：消息码
 *             uiMsgVar  ：消息参数
 *             uiLoadLen ：消息有效载荷长度
 *             pucLoad   ：指向需要发送的有效载荷地址
 * @param[out]
 * @return 0 ：发送成功  非0：发送失败
 * @note
 */
uint8_t mq_service_send_to_4gsend(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad)
{
    return mq_service_send_msg(CM_UNDEFINE_ID,CM_4GSEND_MODULE_ID,uiMsgCode,uiMsgVar,uiLoadLen,pucLoad) ;
}

/**
* @brief   迪文接收任务发送到显示任务
 * @param[in]  uiMsgCode ：消息码
 *             uiMsgVar  ：消息参数
 *             uiLoadLen ：消息有效载荷长度
 *             pucLoad   ：指向需要发送的有效载荷地址
 * @param[out]
 * @return 0 ：发送成功  非0：发送失败
 * @note
 */
uint8_t mq_service_dwinrecv_send_disp(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad)
{
    return mq_service_send_msg(CM_DWINRECV_MODULE_ID,CM_DISP_MODULE_ID,uiMsgCode,uiMsgVar,uiLoadLen,pucLoad) ;
}

