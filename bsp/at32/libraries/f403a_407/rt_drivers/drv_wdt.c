/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-03-15     shelton           first version
 */

#include <drivers/watchdog.h>
#include "board.h"

#ifdef RT_USING_WDT

#define LICK_VALUE                      40000

//#define DRV_DEBUG
#define LOG_TAG                         "drv.wdt"
#include <drv_log.h>

struct at32_wdt_obj
{
    wdt_type *wdt_x;
    wdt_division_type div;
    rt_uint32_t reload;
    rt_uint16_t is_start;
};
static struct at32_wdt_obj at32_wdt;
static struct rt_watchdog_ops ops;
static rt_watchdog_t watchdog;

static rt_err_t wdt_init(rt_watchdog_t *wdt)
{
    return RT_EOK;
}

static rt_err_t wdt_control(rt_watchdog_t *wdt, int cmd, void *arg)
{
    switch (cmd)
    {
    /* feed the watchdog */
    case RT_DEVICE_CTRL_WDT_KEEPALIVE:
        wdt_counter_reload();
        break;
    /* set watchdog timeout */
    case RT_DEVICE_CTRL_WDT_SET_TIMEOUT:
#if defined(LICK_VALUE)
        if(LICK_VALUE)
        {
            at32_wdt.reload = (*((rt_uint32_t*)arg)) * LICK_VALUE / 256 ;
        }
        else
        {
            LOG_E("Please define the value of LICK_VALUE!");
        }
        if(at32_wdt.reload > 0xFFF)
        {
            LOG_E("wdg set timeout parameter too large, please less than %ds",0xFFF * 256 / LICK_VALUE);
            return -RT_EINVAL;
        }
#else
  #error "Please define the value of LICK_VALUE!"
#endif
        if(at32_wdt.is_start)
        {
            wdt_register_write_enable(TRUE);
            wdt_divider_set(at32_wdt.div);
            wdt_reload_value_set(at32_wdt.reload);
            wdt_register_write_enable(FALSE);
            wdt_enable();
        }
        break;
    case RT_DEVICE_CTRL_WDT_GET_TIMEOUT:
#if defined(LICK_VALUE)
        if(LICK_VALUE)
        {
            (*((rt_uint32_t*)arg)) = at32_wdt.reload * 256 / LICK_VALUE;
        }
        else
        {
            LOG_E("Please define the value of LICK_VALUE!");
        }
#else
  #error "Please define the value of LICK_VALUE!"
#endif
        break;
    case RT_DEVICE_CTRL_WDT_START:
        wdt_register_write_enable(TRUE);
        wdt_divider_set(at32_wdt.div);
        wdt_reload_value_set(at32_wdt.reload);
        wdt_register_write_enable(FALSE);
        wdt_enable();
        at32_wdt.is_start = 1;
        break;
    default:
        LOG_W("This command is not supported.");
        return -RT_ERROR;
    }
    return RT_EOK;
}

int rt_hw_wdt_init(void)
{
    at32_wdt.wdt_x = WDT;
    at32_wdt.div = WDT_CLK_DIV_256;
    at32_wdt.reload = 0x00000FFF;
    at32_wdt.is_start = 0;

    ops.init = &wdt_init;
    ops.control = &wdt_control;
    watchdog.ops = &ops;
    /* register watchdog device */
    if (rt_hw_watchdog_register(&watchdog, "wdt", RT_DEVICE_FLAG_DEACTIVATE, RT_NULL) != RT_EOK)
    {
        LOG_E("wdt device register failed.");
        return -RT_ERROR;
    }
    LOG_D("wdt device register success.");
    return RT_EOK;
}

INIT_BOARD_EXPORT(rt_hw_wdt_init);

/**CSH230605   看门狗程序*/
#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */
static rt_device_t wdg_dev;         /* 看门狗设备句柄 */
static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    //rt_kprintf("feed the dog!\n ");//正式使用时去掉
}

int wdt_sample(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t timeout = 10;        /* 溢出时间，单位：秒 */
    char device_name[RT_NAME_MAX]=WDT_DEVICE_NAME;

    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(device_name);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 设置看门狗溢出时间 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", device_name);
        return RT_ERROR;
    }
    /* 启动看门狗 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        rt_kprintf("start %s failed!\n", device_name);
        return -RT_ERROR;
    }
    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);

    return ret;
}



#endif /* RT_USING_WDT */
