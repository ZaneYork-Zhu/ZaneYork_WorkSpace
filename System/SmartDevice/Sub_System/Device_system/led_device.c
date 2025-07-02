#include "led_device.h"
#include "kal_led.h"

// 初始化 LED 设备的函数指针 
static int LedDeviceInit(struct LedDevice *ptLedDevice)
{
    /* 调用 KAL_LedDeviceInit 函数 */
    return KAL_LedDeviceInit(ptLedDevice);
} 


// 控制 LED 设备状态的函数指针 
static int LedDeviceControl(struct LedDevice *ptLedDevice, int status)
{
    /* 调用 KAL_LedDeviceControl 函数 */
    return KAL_LedDeviceControl(ptLedDevice, status);
}

static LedDevice g_tLedDevices[] = {
    {
        .pin = LED_COLOR_RED,
        .Initialize = LedDeviceInit,
        .ControlStatus = LedDeviceControl,
    },
    {
        .pin = LED_COLOR_GREEN,
        .Initialize = LedDeviceInit,
        .ControlStatus = LedDeviceControl,
    },
    // 可以添加更多颜色的 LED 设备

};

// 获取 LED 设备的函数指针
PLedDevice GetLedDevice(int ledPin)
{
    for (int deviceId = 0; deviceId < sizeof(g_tLedDevices) / sizeof(LedDevice); deviceId++) {
        if (g_tLedDevices[deviceId].pin == ledPin) {
            return &g_tLedDevices[deviceId];
        }
    }
    return NULL; // 如果没有找到对应的 LED 设备
}

