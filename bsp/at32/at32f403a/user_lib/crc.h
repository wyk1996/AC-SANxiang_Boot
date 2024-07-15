/*******************************************************************************
 *          Copyright (c) 2020-2050, .
 *                              All Right Reserved.
 * @file crc.h
 * @note 
 * @brief
 * 
 * @author   
 * @date     2020-12-01
 * @version  V1.0.0
 * 
 * @Description 
 *  
 * @note History:        
 * @note     <author>   <time>    <version >   <desc>
 * @note  
 * @warning
 *******************************************************************************/
#ifndef _CRC_H_
#define _CRC_H_

#include <stdio.h>
#include "at32f403a_407.h"

extern uint16_t LIB_Modbus_CRC(uint8_t *pucMsg, uint16_t usLen);
extern uint16_t PbCrc16(uint8_t *updata, uint16_t len);
uint16_t LIB_CCITT_CRC(uint8_t* pucBuf, uint16_t usLen, uint16_t usInit);
extern uint16_t GetModbusCrc16(uint16_t crc_seed, const void *ptr, int len);
uint8_t GetCrc8(uint8_t crc_seed, const void *a_ptr, int len);
uint8_t CheckSumCRC8(uint8_t* p, uint16_t usLen);
uint16_t CheckSumCRC16(uint8_t* p, uint16_t usLen);
uint16_t LIB_InCRC(uint8_t* pucBuf, uint16_t usLen);

#endif

