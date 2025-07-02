#include "oled_device.h"
#include "kal_oled.h"
#include <stdio.h>


//128x64 OLED显示设备的显存地址
static unsigned char g_OledGramAddress[OLED_PAGE_MAX][OLED_COLUMN_MAX] = {0};

DisplayDeviceData g_OledDeviceData = {
    .iX = OLED_WIDTH, // 初始X坐标
    .iY = OLED_HEIGHT, // 初始Y坐标
    .iBpp = OLED_BIT_PER_PIXEL // 每像素位数
};


static int OledDeviceInit() {
    // 初始化OLED显示设备
    // 这里可以添加具体的初始化代码
    KAL_OledDeviceInit(); 
    return 0; // 返回0表示成功
}

static int OledDeviceDeInit(DisplayDevice *ptDisplayDevice) {
    // 释放OLED显示设备资源
    // 这里可以添加具体的释放资源代码
    KAL_OledDeviceDeInit(ptDisplayDevice);
    return 0; // 返回0表示成功
}

static int OledDeviceSetPixel(struct DisplayDevice *ptDisplayDevice, int x, int y, int isLit) {

    // 设置OLED显示设备的像素点
    KAL_OledDeviceSetPixel(ptDisplayDevice, x, y, isLit);
    return 0; // 返回0表示成功
}

static void OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice) {
    // 刷新OLED显示设备的显存到屏幕
    // 这里可以添加具体的刷新代码
    // 例如，将g_OledGramAddress中的数据写入OLED屏幕
    KAL_OledDeviceFlushGramForDisplay(ptDisplayDevice);
}


// 定义OLED显示设备
DisplayDevice g_tOledDevice ={
    .name = "OLED Display",
    .GramAddress = (char *)g_OledGramAddress,
    .ptData = &g_OledDeviceData, // 这里可以设置实际的显示设备数据指针
    .Initialize = OledDeviceInit, // 这里可以设置初始化函数
    .Deinitialize = OledDeviceDeInit, // 这里可以设置释放资源函数
    .SetPixel = OledDeviceSetPixel, // 这里可以设置清除像素函数
    .FlushGramForDisplay = OledDeviceFlushGramForDisplay // 这里可以设置刷新显示函数
};

/**
 * @brief 注册OLED显示设备
 * * 该函数将OLED显示设备注册到输入设备系统中，以便在系统初始化时调用。
 * * @details 该函数完成以下工作:
 *   1. 调用InputDeviceRegister()函数将OLED显示设备注册到输入设备系统中
 *   2. 通过g_tOledDevice结构体传递OLED显示设备的相关信息
 *   3. 注册后设备可通过输入管理系统统一管理
 */
void AddOledDisplayDevice(void)
{
    DisplayDeviceRegister(&g_tOledDevice);
    
}
