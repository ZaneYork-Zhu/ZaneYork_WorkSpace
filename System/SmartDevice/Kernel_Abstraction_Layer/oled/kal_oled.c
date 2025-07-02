#include "kal_oled.h"
#include "cal_oled.h"
#include "System_Config.h"
int KAL_OledDeviceInit(void) {
    // 初始化OLED显示设备
    // 这里可以添加具体的初始化代码
#if defined(CONFIG_NO_OS)
    return CAL_OledDeviceInit();
#elif defined(CONFIG_FREE_RTOS)
    // 如果使用操作系统，可以在这里添加操作系统相关的初始化代码
    return FreeRtos_OledDeviceInit(ptDisplayDevice);
#elif defined(CONFIG_RT_THREADX)
    // 如果使用ThreadX，可以在这里添加ThreadX相关的初始化代码
    return Threadx_OledDeviceInit(ptDisplayDevice);
#endif
}

int KAL_OledDeviceDeInit(DisplayDevice *ptDisplayDevice) {
    // 释放OLED显示设备资源
    // 这里可以添加具体的释放资源代码
#if defined(CONFIG_NO_OS)
    return CAL_OledDeviceDeInit(ptDisplayDevice);
#elif defined(CONFIG_FREE_RTOS)
    // 如果使用操作系统，可以在这里添加操作系统相关的释放资源代码
    return FreeRtos_OledDeviceDeInit(ptDisplayDevice);
#elif defined(CONFIG_RT_THREADX)
    // 如果使用ThreadX，可以在这里添加ThreadX相关的释放资源代码
    return Threadx_OledDeviceDeInit(ptDisplayDevice);
#endif
}

int KAL_OledDeviceSetPixel(DisplayDevice *ptDisplayDevice, int x, int y, int isLit) {

    // 设置OLED显示设备的像素点
    // 这里可以添加具体的设置像素点代码
#if defined(CONFIG_NO_OS)   
    return CAL_OledDeviceSetPixel(ptDisplayDevice, x, y, isLit);
#elif defined(CONFIG_FREE_RTOS)
    // 如果使用操作系统，可以在这里添加操作系统相关的设置像素点代码
    return FreeRtos_OledDeviceSetPixel(ptDisplayDevice, x, y);
#elif defined(CONFIG_RT_THREADX)
    // 如果使用ThreadX，可以在这里添加ThreadX相关的设置像素点代码
    return Threadx_OledDeviceSetPixel(ptDisplayDevice, x, y);   
#endif
}

void KAL_OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice) {
    // 刷新OLED显示设备的显存到屏幕
    // 这里可以添加具体的刷新代码
#if defined(CONFIG_NO_OS)
    CAL_OledDeviceFlushGramForDisplay(ptDisplayDevice);
#elif defined(CONFIG_FREE_RTOS)
    // 如果使用操作系统，可以在这里添加操作系统相关的刷新代码
    FreeRtos_OledDeviceFlushGramForDisplay(ptDisplayDevice);
#elif defined(CONFIG_RT_THREADX)
    // 如果使用ThreadX，可以在这里添加ThreadX相关的刷新代码
    Threadx_OledDeviceFlushGramForDisplay(ptDisplayDevice);
#endif
}
