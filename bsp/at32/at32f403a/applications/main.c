/*******************************************************************************
 *          Copyright (c) 2020-2050, wanzhuangwulian Co., Ltd.
 *                              All Right Reserved.
 * @file
 * @note
 * @brief
 *
 * @author
 * @date
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
#include <time.h>
#include <board.h>
#include <fal.h>
#include <string.h>
#include "user_lib.h"
#include "easyflash.h"
#include "user_lib.h"
#include "ch_port.h"
#include "mfrc522.h"
#include "w25qxx.h"
#include "4GMain.h"
#include "cmsis_armcc.h"
#include "at32f403a_407_conf.h"
extern _m1_card_info m1_card_info;

SYSTEM_RTCTIME gs_SysTime;

CP56TIME2A_T gsCP56Time;

/* 用于获取RTC毫秒 */
int32_t gsi_RTC_Counts;

uint32_t gui_RTC_millisecond;

//extern S_DEVICE_CFG gs_DevCfg;
//extern S_APP_CHARGE gs_AppCharge;
extern long list_mem(void);
extern int wdt_sample();
/* UNIQUE_ID[31: 0] */
uint32_t Unique_ID1;
/* UNIQUE_ID[63:32] */
uint32_t Unique_ID2;
/* UNIQUE_ID[95:63] */
uint32_t Unique_ID3;
uint32_t test = 0;



#define  SECTOR_SIZE   2048            //AT32F403AVGT7  一共1024K      片1/片2=512k(0-255扇区：每个扇区2K)     2k=2*1024=2048byte
#define FLASH_SIZE   	1024         //flash一共大小
//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint8_t *writeData,uint16_t countToWrite)
{
    if(startAddress<FLASH_BASE||((startAddress+countToWrite)>=(FLASH_BASE+1024*FLASH_SIZE)))
    {
        __NOP();
        return;//非法地址
    }

    flash_unlock(); //解锁写保护
    uint32_t offsetAddress=startAddress-FLASH_BASE;               //计算去掉0X08000000后的实际偏移地址
    uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;            //计算扇区地址，对于STM32F103VET6为0~255

    uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;    //对应扇区的首地址

    flash_sector_erase(sectorStartAddress);//擦除这个扇区

    uint16_t dataIndex;
    for(dataIndex=0; dataIndex<countToWrite; dataIndex++)
    {
        flash_byte_program(startAddress+dataIndex,writeData[dataIndex]);
        //FLASH_ProgramByte(startAddress+dataIndex,writeData[dataIndex]);
    }
    flash_lock();//上锁写保护
}


void flash_read(uint32_t read_addr, uint8_t *p_buffer, uint16_t num_read)
{
    uint16_t i;
    for(i = 0; i < num_read; i++)
    {
        p_buffer[i] = *(uint8_t *)(read_addr);
        read_addr += 1;
    }
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */

//SYSTEM_RTCTIME gs_SysTime;
//pFunction Jump_To_LoadCode;
//uint32_t JumpAddress;
////跳转到烧写代码部分
//#define LoadCodeAddress    0x8032000  //跳转到烧写代码的位置，通过这个位置执行程序，将应用代码从FLASH里面导出来烧写到应用程序区域
//void  JumpToProgramCode(void)
//{
//    rt_hw_interrupt_disable();  //关闭所有中断后，才可以跳转：如果不关闭，升级直接成死板
//    JumpAddress = *(__IO uint32_t*) (LoadCodeAddress + 4);
//    Jump_To_LoadCode = (pFunction) JumpAddress;
//    //初始化用户程序的堆栈指针
//    __set_MSP(*(__IO uint32_t*) LoadCodeAddress);
//    Jump_To_LoadCode();
//}


const struct fal_partition *dwin_info_part = NULL;
const struct fal_partition *charge_rete_info = NULL;
const struct fal_partition *online_bill_info = NULL;    //在线交易记录
const struct fal_partition *offline_bill_info = NULL;   //离线交易记录
const struct fal_partition *offfsline_bill_info = NULL; //离线分时交易记录
const struct fal_partition *Record_query_info = NULL;   //存放记录
const struct fal_partition *card_wl = NULL; 		   //白名单卡
const struct fal_partition *Outage_Recharging_info = NULL; 	//存储断电续充的标志位（有）
const struct fal_partition *charge_Storage_info = NULL; 	//存储断电时的充电信息
const struct fal_partition *APPOTA_Code_info = NULL; 	//APPOTA更新下载程序

#define  FALSE_W_BASE    0x8000000 //APP1 主程序
uint8_t  buf[2048];		 //2k读取
int main(void)
{
    rt_device_t device;
    struct tm *Net_time;
    time_t time_now = 0;
    uint32_t timeout = rt_tick_get();
    uint32_t mem_timeout = rt_tick_get();
    device = rt_device_find("rtc");
    if (device == RT_NULL)
    {
        return -RT_ERROR;
    }
    RT_ASSERT(fal_init() > 0);
    spiflash_init();  //flash初始化

    if ((APPOTA_Code_info = fal_partition_find("APPOTA_Code")) == RT_NULL)
    {
        rt_kprintf("Firmware download failed! Partition (%s) find error!", "APPOTA_Code");
    }

    uint8_t updatastate = 0;
    uint32_t applen =0;  //updatastate  =0x55 标示升级成功  applen升级包长度
    uint32_t wflashlen = 0;   //已经写了flash的长度

    fal_partition_read(APPOTA_Code_info,0,&updatastate,sizeof(updatastate));
    fal_partition_read(APPOTA_Code_info,1,(uint8_t*)&applen,sizeof(uint32_t));

    if((updatastate != 0x55) || (applen < 10*1024) || (applen > (200*1024 - 5)))  //升级不成功误跳入 app程序小于10k，直接跳转到应用程序
    {
        nvic_system_reset();  //程序直接复位，会直接跳转到以0x8000000  开始的程序的位置
    }

    while(1)
    {
//        static uint8_t bufflag = 0x59;
//        uint8_t bufflag123 = 0;
//        FLASH_WriteMoreData(0x804B000,&bufflag,sizeof(bufflag));
//        flash_read(0x804B000, &bufflag123, sizeof(bufflag));
        fal_partition_read(APPOTA_Code_info,5+wflashlen,(uint8_t*)&buf,sizeof(buf)); //0位是状态   1234位是升级长度
        FLASH_WriteMoreData(wflashlen + FALSE_W_BASE,buf,sizeof(buf));
        wflashlen += sizeof(buf);
        if(wflashlen + sizeof(buf) > applen)
        {
            if(wflashlen == applen)
            {
                break;
            }
            fal_partition_read(APPOTA_Code_info,5+wflashlen,(uint8_t*)&buf,applen - wflashlen);
            FLASH_WriteMoreData(wflashlen + FALSE_W_BASE,buf,applen - wflashlen);
            break;
        }
        //rt_thread_mdelay(1000);
    }
    updatastate = 0xaa;
    fal_partition_write(APPOTA_Code_info,0,&updatastate,sizeof(updatastate));

    nvic_system_reset();
    return 0;
}
