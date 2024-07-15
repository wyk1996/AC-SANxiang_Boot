/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtconfig.h>
#include <board.h>

#define NOR_FLASH_DEV_NAME             "norflash0"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev at32_onchip_flash;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &at32_onchip_flash,                                           \
    &nor_flash0,                                                     \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD,        "bl",     "onchip_flash",         0,   64*1024, 0}, \
    {FAL_PART_MAGIC_WORD,       "app",     "onchip_flash",   64*1024,  704*1024, 0}, \
    {FAL_PART_MAGIC_WORD, "easyflash", NOR_FLASH_DEV_NAME,         0, 1024*1024, 0}, \
    {FAL_PART_MAGIC_WORD,  "download", NOR_FLASH_DEV_NAME, 1024*1024, 1024*1024, 0}, \
	{FAL_PART_MAGIC_WORD, "charge_rate",  NOR_FLASH_DEV_NAME,            0, 4*1024, 0}, \
	{FAL_PART_MAGIC_WORD, "dwin_info",   NOR_FLASH_DEV_NAME,    4*1024, 1024*4, 0}, \
	{FAL_PART_MAGIC_WORD, "online_bill",   NOR_FLASH_DEV_NAME,    2*4*1024, 1024*4*2, 0}, \
	{FAL_PART_MAGIC_WORD, "offline_bill",   NOR_FLASH_DEV_NAME,   4*4*1024, 1024*4*110, 0}, \
	{FAL_PART_MAGIC_WORD, "offfsline_bill",   NOR_FLASH_DEV_NAME,   114*4*1024, 1024*4*110, 0}, \
	{FAL_PART_MAGIC_WORD, "Record_query",   NOR_FLASH_DEV_NAME,   224*4*1024, 1024*4*1010, 0}, \
	{FAL_PART_MAGIC_WORD, "Card_wl",   NOR_FLASH_DEV_NAME,   1234*4*1024, 1024*4*1, 0}, \
	{FAL_PART_MAGIC_WORD, "Outage_Recharging",   NOR_FLASH_DEV_NAME,   1235*4*1024, 1024*4*1, 0}, \
	{FAL_PART_MAGIC_WORD, "charge_Storage",   NOR_FLASH_DEV_NAME,   1236*4*1024, 1024*4*1, 0}, \
	{FAL_PART_MAGIC_WORD, "APPOTA_Code",   NOR_FLASH_DEV_NAME,   1237*4*1024, 50*4*1024, 0}, \
	{FAL_PART_MAGIC_WORD, "fm_area",    NOR_FLASH_DEV_NAME,    4208*1024, 256*4*1024, 0}, \
}



#endif /* FAL_PART_HAS_TABLE_CFG */
typedef enum
{
    FLASH_ORDER_READ = 0x01, //0x01:¶Á²Ù×÷
    FLASH_ORDER_WRITE,       //0x02:Ð´²Ù×÷
} _FLASH_ORDER;
#endif /* _FAL_CFG_H_ */
