/*******************************************************************************
 *          Copyright (c) 2020-2050, .
 *                              All Right Reserved.
 * @file user_lib.h
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
#ifndef _USER_LIB_H_
#define _USER_LIB_H_

#include "at32f403a_407.h"
#include "crc.h"

#define LIB_Max(x, y) 		((x) >= (y) ? (x) : (y))
#define LIB_Min(x, y) 		((x) > (y) ? (y) : (x))

/* size_t 型,即unsigned int */
#define OFFSETOF(s, m)		((size_t)&(((s *)0)->m))

/* 求一个数组的维度 */
#define dim(x) 				(sizeof(x)/sizeof(x[0]))

#define sw16(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))
#define sw32(A) ((((uint32_t)(A) & 0xff000000) >> 24) | (((uint32_t)(A) & 0x00ff0000) >> 8) | (((uint32_t)(A) & 0x0000ff00) << 8) | (((uint32_t)(A) & 0x000000ff) << 24))
#define sw64(A) ((uint64_t)(\
				(((uint64_t)(A)& (uint64_t)0x00000000000000ffULL) << 56) | \
				(((uint64_t)(A)& (uint64_t)0x000000000000ff00ULL) << 40) | \
				(((uint64_t)(A)& (uint64_t)0x0000000000ff0000ULL) << 24) | \
				(((uint64_t)(A)& (uint64_t)0x00000000ff000000ULL) << 8) | \
				(((uint64_t)(A)& (uint64_t)0x000000ff00000000ULL) >> 8) | \
				(((uint64_t)(A)& (uint64_t)0x0000ff0000000000ULL) >> 24) | \
				(((uint64_t)(A)& (uint64_t)0x00ff000000000000ULL) >> 40) | \
				(((uint64_t)(A)& (uint64_t)0xff00000000000000ULL) >> 56) ))

#define SW16(A)	sw16(((uint16_t)A))	
#define SW32(A)	sw32(((uint32_t)A))
#define SW64(A)	sw64(((uint64_t)A))
	
typedef struct sEnumToString
{
  const char *pstr;
  int Index;
} S_ENUM_TO_STRING;

 /* 定义MCU自带的时间类参数结构 */
typedef __packed struct s_api_time
{
	/* 秒0-59,hex格式,下同 */
	unsigned char ucSec;
	/* 分0-59 */
	unsigned char ucMin;
	/* 时0-23 */
	unsigned char ucHour;
	/* 日0-30 */
	unsigned char ucDay;
	/* 月0-11 */
	unsigned char ucMonth;
	/* 年0-255 */
	unsigned char ucYear;							
}S_API_TIME;

/* 定义的时间类型 */
typedef __packed struct SYS_TEMTIME
{
	unsigned char ucSec;
	unsigned char ucMin;
	unsigned char ucHour;
	/* 星期:0-6,sunday-saturday */
	unsigned char ucWeek;
	unsigned char ucDay;
	unsigned char ucMonth;
	unsigned char ucYear;
}SYSTEM_RTCTIME;


extern const uint8_t gCuc_Hex2Char[];
extern int max_func(int a[],int n);
extern uint32_t BitToValue(uint32_t uiBit);
extern void StringToHex(char * dest, char * src, int len);
extern uint16_t uint8_to_uint16_SE(uint8_t* pucSrc);
extern void LIB_uint16_to_uint8p(uint8_t* pucDst, uint16_t uiSrc);
extern void uint16_to_uint8_SE(uint8_t* pucDst, uint16_t uiSrc);
extern void uint16_to_uint8_BE(uint8_t* pucDst, uint16_t uiSrc);
extern void uint32_to_uint8_SE(uint8_t* pucDst, uint32_t ulSrc);
extern void uint32_to_uint8_BE(uint8_t* pucDst, uint32_t ulSrc);
extern uint32_t LIB_U24BitBCD2Hex(uint32_t ulBCD);
extern uint32_t LIB_uint8_to_uint24_SE(uint8_t* pucSrc);
extern uint32_t LIB_uint8_to_uint32_SE(uint8_t* pucSrc);
extern uint32_t LIB_uint8_to_uint32_BE(uint8_t* pucSrc);
extern uint32_t LIB_UlongBCD2Hex(uint32_t ulBCD);
extern int LIB_CalcTimeGap(S_API_TIME* psClk1, S_API_TIME* psClk2);
extern uint32_t LIB_CalcDaySecondGap(S_API_TIME* psClk1, S_API_TIME* psClk2);
extern uint32_t LIB_CalcAbsSecond(S_API_TIME* psClk);
extern SYSTEM_RTCTIME *LIB_TimestampsToRtc(uint32_t timestamp);
extern uint32_t LIB_RtcToTimestamps(SYSTEM_RTCTIME *pRtc);
extern SYSTEM_RTCTIME *LIB_UtcToRtc(SYSTEM_RTCTIME *pRtc);
extern SYSTEM_RTCTIME *LIB_RtcToUtc(SYSTEM_RTCTIME *pRtc);
extern void LIB_UtcToApiTime(SYSTEM_RTCTIME *pRtc,S_API_TIME *pApi);

extern uint8_t GetUtcTime(char *pt,SYSTEM_RTCTIME *pTime);
extern char * GetTimeAscii(char *pTime);
extern void LIB_uint16_to_uint8p_BE(uint8_t* pucDst, uint16_t uiSrc);
extern uint16_t LIB_uint8_to_uint16_BE(uint8_t* pucSrc) ;
extern uint8_t  LIB_CheckSumByte(uint8_t* p, uint32_t ulLen);
extern uint16_t uint16_H_to_L(uint16_t usSrc);
extern uint32_t uint32split_H_to_L(uint32_t uiSrc);
extern uint8_t  LIB_UcharHex2BCD(uint8_t ucHex);
extern uint8_t BcdToDec(uint8_t Bcd_Data);
extern uint8_t DecToBcd(uint8_t DecData);
extern uint8_t  LIB_UcharBCD2Hex(uint8_t ucBCD);
extern uint16_t LIB_UshortBCD2Hex(uint16_t uiBCD);
extern uint16_t LIB_UintHex2BCD(uint16_t uiHex);
extern uint8_t  LIB_Asc2Bcd(uint8_t *input, uint32_t inputLen, uint8_t *output);
extern uint8_t LIB_Bcd2Asc(uint8_t *input, uint32_t inputLen, uint8_t *output);
extern void LIB_U32DectoBCD(uint32_t Dec, uint8_t *Bcd, uint32_t length);
extern void LIB_U64DectoBCD(uint64_t Dec, uint8_t *Bcd, uint32_t length);

extern void my_memcpy(char *dst, char *src, uint16_t uiLen);
extern uint16_t memremove(char *src, char *dst, uint16_t usTotalLen,uint16_t usLen);
extern void memcpy_reverse(char *dst, char *src, uint16_t uiLen);
extern char* strnstr(char* s1, char* s2, int len);
extern char* strrnstr(char* s1, char* s2, int len);
extern uint16_t GetStrstr(char *src, char *dst, uint16_t uslen );
extern uint8_t LIB_Dec2Ascii(uint32_t dec, char *dst);
extern int tolower(int c);
extern uint8_t Asc2Hex( uint8_t *p);
extern int htoi(const char s[],int start,int len);
extern char atox(char *src,char *dst,uint16_t src_len);
extern char *itoa( int value, char *string, int radix );
extern void LIB_Hex2Ascii(uint8_t* pucDst, uint8_t* pucSrc, uint8_t ucLen);
extern void LIB_Hex2Ascii_Blank(uint8_t* pucDst, uint8_t* pucSrc, uint8_t ucLen);
extern uint16_t Str_ConvertTo_Dec(const char *str);
extern uint32_t FloatToUint32(float *pdata);
extern uint16_t ModbusCRC16(uint8_t *ptr, uint16_t len);
extern uint16_t StrTrim(char *pStr, char *pDst, uint16_t len);
extern uint8_t *ConvertSntoBcd(uint8_t *ucSrcDevSn, uint8_t *ucDestDevSn, uint32_t length);
extern void str_reverse(uint8_t *pdata, uint16_t usLen);
#endif

