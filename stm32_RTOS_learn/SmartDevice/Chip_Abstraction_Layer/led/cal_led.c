/*
 * 文件名:cal_led.c
 * 功能概述: LED设备驱动实现文件
 * 交互关系:
 *  - 对于裸机/FreeRTOS: 调用CAL_LedDeviceInit和CAL_LedDeviceControl函数
 *  - 对于RT-Thread: 需要实现对应的LED设备驱动
 *  - 对于Linux: 需要实现对应的LED设备驱动 
 * 版本历史:
 *   v1.0 2025-06-29 初始版本 
 */

#include "cal_led.h"
#include "LED.h"
int CAL_LedDeviceInit(struct LedDevice *ptLedDevice)
{
    /*调用芯片相关代码：注册GPIO中断*/
    /*对于ST芯片*/
    LED_Init();
    /*对于RTOS: 创建任务*/
    return 0; // 返回成功

}

int CAL_LedDeviceControl(struct LedDevice *ptLedDevice, int status)
{
    /*调用芯片相关代码：控制GPIO状态*/
    /*对于ST芯片*/
    return HAL_LedDeviceControl(ptLedDevice, status);
    /*对于RTOS: 发送消息给任务*/

}
