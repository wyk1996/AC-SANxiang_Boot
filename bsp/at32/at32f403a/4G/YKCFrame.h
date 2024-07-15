/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: GPRSMain.h
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
#ifndef	__YKCFRAME_H_
#define	__YKCFRAME_H_
#include "stdint.h"
#include "common.h"
#include "4GMain.h"
/* Private define-----------------------------------------------------------------------------*/
/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/
#if NET_YX_SELCT == XY_YKC
/*****************************************************************************
* Function     : YKC_RecvFrameDispose
* Description  : 合肥乾古接收帧处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   YKC_RecvFrameDispose(uint8_t * pdata,uint16_t len);


/*****************************************************************************
* Function     : ZH_SendFrameDispose
* Description  : 汇誉接收帧处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   YKC_SendFrameDispose(void);


/*****************************************************************************
* Function     : ZH_SendStartAck
* Description  : 启动应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendStartAck(_GUN_NUM gun);

/*****************************************************************************
* Function     : PreZHBill
* Description  : 保存订单
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   PreYKCBill(_GUN_NUM gun,uint8_t *pdata);

/*****************************************************************************
* Function     : ZH_SendBill
* Description  : 发送订单
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t YKC_SendBill(_GUN_NUM gun);

uint8_t YKC_Balance_judgment(_GUN_NUM gun);  //判断YKC金额
/*****************************************************************************
* Function     : ZH_SendStopAck
* Description  : 停止应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendStopAck(_GUN_NUM gun);



/*****************************************************************************
* Function     : APP_GetZHBatchNum
* Description  : 获取交易流水号
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
******************************************************************************/
uint8_t *  APP_GetYKCBatchNum(uint8_t gun);

/*****************************************************************************
* Function     : ZH_SendRateAck
* Description  : 费率设置应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendRateAck(uint8_t cmd);

/*****************************************************************************
* Function     : APP_GetYKCNetMoney
* Description  :获取账户余额
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
******************************************************************************/
uint32_t APP_GetYKCNetMoney(uint8_t gun);

/*****************************************************************************
* Function     : YKC_SendBillData
* Description  : 云快充重发订单数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t YKC_SendSetTimeAck(void);

/*****************************************************************************
* Function     : YKC_SendBillData
* Description  : 云快充重发订单数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t YKC_SendBillData(uint8_t * pdata,uint8_t len);


uint8_t  YKC_SendCardInfo(_GUN_NUM gun);
uint8_t _YKC_RestUpdataData(_GUN_NUM gun);
/*****************************************************************************
* Function     : YKC_SendPriModel
* Description  : 发送计费模型
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendPriModel(void);

/*****************************************************************************
* Function     : YKC_SendPriReq
* Description  : 发送计费请求
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendPriReq(void);
uint8_t   APP_GetYKCStartType(uint8_t gun);
uint8_t   APP_SetYKCStartType(uint8_t gun ,_4G_START_TYPE  type);


#endif
#endif
/************************(C)COPYRIGHT 2020 杭州汇誉*****END OF FILE****************************/

