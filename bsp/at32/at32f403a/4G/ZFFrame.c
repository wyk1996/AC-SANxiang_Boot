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
#include "4GMain.h"
#include "4GRecv.h"
#include "common.h"
#include "dwin_com_pro.h"

// Private define

//政府平台目前简单用一下，接收不管他


#define LINK_ZF    1

#define ZF_SEND_FRAME_LEN    3


static uint8_t ZF_SendRegister(void);		//发送注册帧
static uint8_t ZF_SendHear(void);		//心跳
static uint8_t ZF_SendDevState(void);		//充电桩状态

_4G_SEND_TABLE ZFSendTable[ZF_SEND_FRAME_LEN] = {
	{0,    0,    CM_TIME_1_SEC, 	ZF_SendRegister			},  //发送注册帧

	{0,    0,    CM_TIME_10_SEC, 		ZF_SendHear				},	//心跳
	
	{0,    0,    CM_TIME_30_SEC, 	ZF_SendDevState		},	//充电桩状态
};


/*****************************************************************************
* Function     : ZF_SendFrameDispose
* Description  : 周期性发送处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   ZF_SendFrameDispose(void)
{
//	uint8_t i;

//	for(i = 0;i < ZF_SEND_FRAME_LEN;i++)
//	{
//		if(ZFSendTable[i].cycletime == 0)
//		{
//			continue;
//		}
//		ZFSendTable[i].curtime = OSTimeGet();
//		if((ZFSendTable[i].curtime >= ZFSendTable[i].lasttime) ? ((ZFSendTable[i].curtime - ZFSendTable[i].lasttime) >= ZFSendTable[i].cycletime) : \
//		((ZFSendTable[i].curtime + (0xFFFFFFFFu - ZFSendTable[i].lasttime)) >= ZFSendTable[i].cycletime))
//		{
//			ZFSendTable[i].lasttime = ZFSendTable[i].curtime;
//			if(ZFSendTable[i].Sendfunc != NULL)
//			{
//				ZFSendTable[i].Sendfunc();
//			}
//		}
//		
//	}
//	return TRUE;
}

/*****************************************************************************
* Function     : ZF_SendRegister
* Description  : 注册
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   ZF_SendRegister(void)
{
//	uint8_t sendbuf[20];
//	uint8_t * pdevnum = DisSysConfigInfo.DevNum; 
//	
//	if(APP_GetAppRegisterState(LINK_ZF) == STATE_OK)	//显示已经注册成功了
//	{
//		return  FALSE;
//	}
//	sendbuf[0] = 0x68;
//	sendbuf[1] = 0x01;
//	//8字节终端号(BCD)
//	sendbuf[2] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
//	sendbuf[3] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
//	sendbuf[4] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
//	sendbuf[5] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
//	sendbuf[6] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
//	sendbuf[7] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
//	sendbuf[8] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
//	sendbuf[9] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
//	sendbuf[10] = 0x00;
//	sendbuf[11] = 0x00;
//	ModuleSIM7600_SendData(LINK_ZF, sendbuf,12); //发送数据
//	//OSTimeDly(SYS_DELAY_50ms); 
//	return TRUE;
}

/*****************************************************************************
* Function     : ZF_SendRegister
* Description  : 心跳
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   ZF_SendHear(void)
{
//	uint8_t sendbuf[20];
//	uint8_t * pdevnum = DisSysConfigInfo.DevNum; 
//	//68 04 33 01 06 00 19 98 17 83 43 00 00 00
//	if(APP_GetAppRegisterState(LINK_ZF) != STATE_OK)	//显示已经注册成功了
//	{
//		return  FALSE;
//	}
//	sendbuf[0] = 0x68;
//	sendbuf[1] = 0x04;
////	sendbuf[2] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
////	sendbuf[3] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
////	sendbuf[4] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
////	sendbuf[5] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
////	sendbuf[6] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
////	sendbuf[7] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
////	sendbuf[8] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
////	sendbuf[9] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
//	sendbuf[2] = 0x43;
//	sendbuf[3] = 0x00;
//	sendbuf[4] = 0x00;
//	sendbuf[5] = 0x00;
//	ModuleSIM7600_SendData(LINK_ZF, sendbuf,6); //发送数据
//	//OSTimeDly(SYS_DELAY_50ms); 
}


/*****************************************************************************
* Function     : ZF_SendRegister
* Description  : 心跳
* Input        :
* Output       : 
* Return       :
* Note(s)      :政府备案为单枪，当双枪情况下，只传A枪
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   ZF_SendDevState(void)
{
//	//68 22 00 00 00 00 86 00 06 00 00 33 01 06 00 19 98 17 83   00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00
//	//备案为单枪，当双枪情况下，只传A枪
//	uint8_t sendbuf[50] = {0};
//	uint8_t * pdevnum = DisSysConfigInfo.DevNum; 
//	USERINFO * puserinfo  = GetChargingInfo(GUN_A);
//	uint16_t data16;
//	uint32_t data32;
//	_DISP_CONTROL* pdisp_conrtol = APP_GetDispControl();
//	if(APP_GetAppRegisterState(LINK_ZF) !=  STATE_OK)	//显示已经注册成功了
//	{
//		return  FALSE;
//	}
//	sendbuf[0] = 0x68;
//	sendbuf[1] = 0x22;
//	sendbuf[2] = 0x00;
//	sendbuf[3] = 0x00;
//	sendbuf[4] = 0x00;
//	sendbuf[5] = 0x00;
//	sendbuf[6] = 0x86;
//	sendbuf[7] = 0x00;
//	sendbuf[8] = 0x06;
//	sendbuf[9] = 0x00;
//	sendbuf[10] = 0x00;
//	//8字节终端号(BCD)
//	sendbuf[11] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
//	sendbuf[12] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
//	sendbuf[13] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
//	sendbuf[14] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
//	sendbuf[15] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
//	sendbuf[16] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
//	sendbuf[17] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
//	sendbuf[18] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
//	sendbuf[19] = 0x00;    //如果是单枪，此处为0，如果是双枪，1号枪是00，2号枪是01，以此类推，保证与档案里一致）
//	//连接状态  	 	1字节
//	if(APP_GetGunState(GUN_A) == GUN_UNCONNECT)
//	{
//		sendbuf[20] = 0;
//	}
//	else
//	{
//		sendbuf[20] = 1;
//	}
//	//工作状态   		2字节
//	sendbuf[21]= 0x00;   //充电
//	if(pdisp_conrtol->NetGunState[GUN_A] == GUN_CHARGEING)
//	{
//		sendbuf[22]= 0x03;   //充电
//	}else if(pdisp_conrtol->NetGunState[GUN_A] == GUN_FAIL)
//	{
//		sendbuf[22]= 0x01; //故障
//	}else
//	{
//		sendbuf[22]= 0x02; //待机
//	}
//	if(pdisp_conrtol->NetGunState[GUN_A] == GUN_CHARGEING)
//	{
//		//充电输出电压		2字节
//		data16 = CcuYCContext[GUN_A].outvolt;
//		sendbuf[23] = (data16 >> 8) & 0x00ff;
//		sendbuf[24] = data16 & 0x00ff;
//		//充电输出电流		2字节
//		data16 = (4000- CcuYCContext[GUN_A].outcurr)*10;
//		sendbuf[25] = (data16 >> 8) & 0x00ff;
//		sendbuf[26] = data16 & 0x00ff;
//		//输出继电器状态	1字节
//		sendbuf[27] = 1;
//		//已充电量			4字节
//		data32 = puserinfo->TotalPower /10;
//		sendbuf[28] = (data32 >> 24) & 0x000000ff;
//		sendbuf[29] = (data32 >> 16) & 0x000000ff;
//		sendbuf[30] = (data32 >> 8) & 0x000000ff;
//		sendbuf[31] = data32 & 0x000000ff;
//		//充电时间			2字节
//		data16 = puserinfo->ChargeTime;
//		sendbuf[32] = (data16 >> 8) & 0x00ff;
//		sendbuf[33] = data16 & 0x00ff;
//		//soc				2字节
//		sendbuf[34] = 0x00;
//		sendbuf[35] =  CcuYCContext[GUN_A].batterysoc;
//	}
//	ModuleSIM7600_SendData(LINK_ZF, sendbuf,36); //发送数据
//	//OSTimeDly(SYS_DELAY_50ms); 
//	return TRUE;
}

/*****************************************************************************
* Function     : ZF_SendStartCharge
* Description  : 启动帧
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   ZF_SendStartCharge(void)
{
//	uint8_t sendbuf[20];
//	uint8_t * pdevnum = DisSysConfigInfo.DevNum; 
//	//68 04 33 01 06 00 19 98 17 83 43 00 00 00
//	if(APP_GetAppRegisterState(LINK_ZF) != STATE_OK)	//显示已经注册成功了
//	{
//		return  FALSE;
//	}
//	sendbuf[0] = 0x68;
//	sendbuf[1] = 0x04;
//	sendbuf[2] = 0x07;
//	sendbuf[3] = 0x00;
//	sendbuf[4] = 0x00;
//	sendbuf[5] = 0x00;
//	ModuleSIM7600_SendData(LINK_ZF, sendbuf,6); //发送数据
	//OSTimeDly(SYS_DELAY_50ms); 
}


/*****************************************************************************
* Function     : ZF_SendBill
* Description  : 发送订单
* Input        :
* Output       : 
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t ZF_SendBill(void)
{
//	//683800000000820006000000000002330108001790592600A0280F1004081369780311040813000007E3000000000000000000000000000007E3
//	//备案为单枪，当双枪情况下，只传A枪
//	uint8_t sendbuf[100] = {0};
//	uint8_t * pdevnum = DisSysConfigInfo.DevNum; 
//	uint16_t second;
//	USERINFO * puserinfo  = GetChargingInfo(GUN_A);
//	TEMPDATA * pmeter =APP_GetMeterPowerInfo(GUN_A);
//	uint16_t data16;
//	uint32_t data32;
//	_DISP_CONTROL* pdisp_conrtol = APP_GetDispControl();
//	if(APP_GetAppRegisterState(LINK_ZF) !=  STATE_OK)	//显示已经注册成功了
//	{
//		return  FALSE;
//	}
//	sendbuf[0] = 0x68;
//	sendbuf[1] = 0x38;
//	sendbuf[2] = 0x00;
//	sendbuf[3] = 0x00;
//	sendbuf[4] = 0x00;
//	sendbuf[5] = 0x00;
//	sendbuf[6] = 0x82;
//	sendbuf[7] = 0x00;
//	sendbuf[8] = 0x06;
//	sendbuf[9] = 0x00;
//	sendbuf[10] = 0x00;
//	sendbuf[11] = 0x00;
//	sendbuf[12] = 0x00;
//	sendbuf[13] = 0x00;
//	sendbuf[14] = 0x02;
//	//8字节终端号(BCD)
//	sendbuf[15] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
//	sendbuf[16] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
//	sendbuf[17] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
//	sendbuf[18] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
//	sendbuf[19] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
//	sendbuf[20] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
//	sendbuf[21] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
//	sendbuf[22] = (pdevnum[14] - '0') *0x10 +  (pdevnum[15] - '0');
//	sendbuf[23] = 0x00;    //如果是单枪，此处为0，如果是双枪，1号枪是00，2号枪是01，以此类推，保证与档案里一致）
//	
//	//开始时间 7字节
//	second =  (((puserinfo->StartTime.Second & 0xf0)  >> 4) * 10 + (puserinfo->StartTime.Second & 0x0f))*1000;
//	sendbuf[24] = (second >> 8) & 0x00ff;
//	sendbuf[25] = (second) & 0x00ff;
//	sendbuf[26] = ((puserinfo->StartTime.Minute & 0xf0)  >> 4 ) * 10 + (puserinfo->StartTime.Minute & 0x0f);
//	sendbuf[27] = ((puserinfo->StartTime.Hour & 0xf0)  >> 4) * 10 + (puserinfo->StartTime.Hour & 0x0f);
//	sendbuf[28] = ((puserinfo->StartTime.Day & 0xf0)  >> 4) * 10 + (puserinfo->StartTime.Day & 0x0f);
//	sendbuf[29] = ((puserinfo->StartTime.Month & 0xf0)  >> 4) * 10 + (puserinfo->StartTime.Month & 0x0f);
//	sendbuf[30] =  ((puserinfo->StartTime.Year & 0xf0)  >> 4) * 10 + (puserinfo->StartTime.Year & 0x0f);
//	//结束时间 7字节
//	second =  (((puserinfo->EndTime.Second & 0xf0)  >> 4) * 10 + (puserinfo->EndTime.Second & 0x0f))*1000;
//	sendbuf[31] = (second >> 8) & 0x00ff;
//	sendbuf[32] = (second) & 0x00ff;
//	sendbuf[33] = ((puserinfo->EndTime.Minute & 0xf0)  >> 4) * 10 + (puserinfo->EndTime.Minute & 0x0f);
//	sendbuf[34] = ((puserinfo->EndTime.Hour & 0xf0)  >> 4) * 10 + (puserinfo->EndTime.Hour & 0x0f);
//	sendbuf[35] = ((puserinfo->EndTime.Day & 0xf0)  >> 4) * 10 + (puserinfo->EndTime.Day & 0x0f);
//	sendbuf[36] = ((puserinfo->EndTime.Month & 0xf0)  >>4) * 10 + (puserinfo->EndTime.Month & 0x0f);
//	sendbuf[37] =  ((puserinfo->EndTime.Year & 0xf0)  >> 4) * 10 + (puserinfo->EndTime.Year & 0x0f);
//	//电量1 	4字节   0.01
//	sendbuf[41] = (pmeter->CurPower -pmeter->StartPower)  & 0x000000ff;
//	sendbuf[40] = ((pmeter->CurPower -pmeter->StartPower) >> 8) & 0x000000ff;
//	sendbuf[39]= ((pmeter->CurPower -pmeter->StartPower) >> 16) & 0x000000ff;
//	sendbuf[38] = ((pmeter->CurPower -pmeter->StartPower) >> 24)& 0x000000ff;
//	//电量2 	4字节   0.01
//	sendbuf[42] = 0;
//	sendbuf[43] = 0;
//	sendbuf[44] = 0;
//	sendbuf[45] = 0;
//	//电量3 	4字节   0.01
//	sendbuf[46] = 0;
//	sendbuf[47] = 0;
//	sendbuf[48] = 0;
//	sendbuf[49] = 0;
//	//电量4 	4字节   0.01
//	sendbuf[50] = 0;
//	sendbuf[51] = 0;
//	sendbuf[52] = 0;
//	sendbuf[53] = 0;
//	//电量5 	4字节   0.01
//	sendbuf[57] = (pmeter->CurPower -pmeter->StartPower)  & 0x000000ff;
//	sendbuf[56] = ((pmeter->CurPower -pmeter->StartPower) >> 8) & 0x000000ff;
//	sendbuf[55]= ((pmeter->CurPower -pmeter->StartPower) >> 16) & 0x000000ff;
//	sendbuf[54] = ((pmeter->CurPower -pmeter->StartPower) >> 24)& 0x000000ff;
//	ModuleSIM7600_SendData(LINK_ZF, sendbuf,58); //发送数据
//	//OSTimeDly(SYS_DELAY_50ms); 
//	return TRUE;
}
/************************(C)COPYRIGHT 2020 杭州汇誉*****END OF FILE****************************/

