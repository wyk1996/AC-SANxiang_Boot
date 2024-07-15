/*******************************************************************************
 *          Copyright (c) 2020-2050, wanzhuangwulian Co., Ltd.
 *                              All Right Reserved.
 * @file data_def.c
 * @note 
 * @brief 
 * 
 * @author   
 * @date     
 * @version
 * 
 * @Description 
 *  
 * @note History:        
 * @note     <author>   <time>    <version >   <desc>
 * @note  
 * @warning
 *******************************************************************************/
#ifndef   __DATA_DEF_H__
#define   __DATA_DEF_H__ 

#include <stdint.h>
#include <rtdevice.h>

/* 常用类型和常量定义 */
#ifdef __cplusplus
extern "C" {
#endif

/* 软件版本号 */
#define FIRMWARE_VERSION	"23728"

#define COM_TYPE_GPRS       0x3C

#define  NET_CH395          0 
#define  NET_L501           1 

#define  NET_TYPE           NET_L501

#define  TIME_20_MSEC       20ul 
#define  TIME_100_MSEC      100ul
#define  TIME_500_MSEC      500ul 
#define  TIME_1_SEC         1000ul 
#define  TIME_2_SEC         2000ul 
#define  TIME_5_SEC         5000ul 
#define  TIME_10_SEC        10ul * TIME_1_SEC 
#define  TIME_15_SEC        15ul * TIME_1_SEC
#define  TIME_30_SEC        30ul * TIME_1_SEC
#define  TIME_1_MIN         60ul * TIME_1_SEC 
#define  TIME_2_MIN         2ul * TIME_1_MIN
#define  TIME_5_MIN         5ul * TIME_1_MIN
#define  TIME_10_MIN        10ul * TIME_1_MIN 
#define  TIME_30_MIN        30ul * TIME_1_MIN
#define  TIME_1_HOU         60ul * TIME_1_MIN

#define  DATA_GET_MIN(a,b)  (((a) <= (b)) ? (a) : (b))	
#define  DATA_GET_MAX(a,b)  (((a) >= (b)) ? (a) : (b))

#define  BIT0	((uint16_t)0x0001)
#define  BIT1	((uint16_t)0x0002)
#define  BIT2	((uint16_t)0x0004)
#define  BIT3	((uint16_t)0x0008)
#define  BIT4	((uint16_t)0x0010)
#define  BIT5	((uint16_t)0x0020)
#define  BIT6	((uint16_t)0x0040)
#define  BIT7	((uint16_t)0x0080)
         
#define  BIT8   ((uint16_t)0x0100)
#define  BIT9   ((uint16_t)0x0200)
#define  BIT10  ((uint16_t)0x0400)
#define  BIT11  ((uint16_t)0x0800)
#define  BIT12  ((uint16_t)0x1000)
#define  BIT13  ((uint16_t)0x2000)
#define  BIT14  ((uint16_t)0x4000)
#define  BIT15  ((uint16_t)0x8000)
         
#define  BIT16	0x00010000ul
#define  BIT17	0x00020000ul
#define  BIT18	0x00040000ul
#define  BIT19	0x00080000ul
#define  BIT20	0x00100000ul
#define  BIT21	0x00200000ul
#define  BIT22	0x00400000ul
#define  BIT23	0x00800000ul
#define  BIT24  0x01000000ul
#define  BIT25  0x02000000ul
#define  BIT26  0x04000000ul
#define  BIT27  0x08000000ul
#define  BIT28  0x10000000ul
#define  BIT29  0x20000000ul
#define  BIT30  0x40000000ul
#define  BIT31  0x80000000ul

#ifdef __cplusplus
}
#endif
#endif

