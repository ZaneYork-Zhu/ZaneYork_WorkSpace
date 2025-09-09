/*
 * 文件名: kal_gpio_key.c
 * 功能概述: GPIO按键设备驱动实现文件 
 * 版本历史:
 *   v1.0 2025-06-29 初始版本 
 */
#include "cal_gpio_key.h"
#include "kal_gpio_key.h"
void KAL_GpioKeyInit(void)
{
    /*对于裸机：注册中断*/
    CAL_GpioKeyInit();
    /*对于RTOS: 创建任务*/

}
