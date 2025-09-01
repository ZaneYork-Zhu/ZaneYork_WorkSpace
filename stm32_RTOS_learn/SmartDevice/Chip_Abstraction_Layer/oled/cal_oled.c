#include "cal_oled.h"
#include "oled_ssd1306.h"
int CAL_OledDeviceInit(void) {
    // 初始化OLED显示设备
    // 这里可以添加具体的初始化代码.
    OperationResult eResult = HAL_OledDeviceInit();
    return (int)eResult; // 返回0表示成功
}

int CAL_OledDeviceDeInit(DisplayDevice *ptDisplayDevice) {
    // 释放OLED显示设备资源
    // 这里可以添加具体的释放资源代码
    return 0;
}

int CAL_OledDeviceSetPixel(DisplayDevice *ptDisplayDevice, int x, int y, int isLit) {

    // 设置OLED显示设备的像素点
    // 这里可以添加具体的设置像素点代码
    return HAL_OledDeviceSetPixel(ptDisplayDevice, x, y, isLit);
}

void CAL_OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice) {
    // 刷新OLED显示设备的显存到屏幕
    // 这里可以添加具体的刷新代码
    HAL_OledDeviceFlushGramForDisplay(ptDisplayDevice);
}


