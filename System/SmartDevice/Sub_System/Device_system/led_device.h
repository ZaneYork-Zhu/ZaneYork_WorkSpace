#ifndef __LED_DEVICE_H__ 
#define __LED_DEVICE_H__ 
 

#ifndef NULL
#define NULL ((void *)0) 
#endif


// 定义 LED 颜色常量 
typedef enum { 
    LED_COLOR_RED = 0x01, 
    LED_COLOR_GREEN = 0x02, 
    LED_COLOR_BLUE = 0x04, 
    LED_COLOR_WHITE = 0x08, 
} LedColor;
 
/* 
 * @file led_device.h 
 * @brief LED 设备控制接口定义 
 * 
 * 该文件定义了 LED 设备的结构体和相关函数指针，用于控制 LED 设备的初始化、释放、状态控制、颜色设置和亮度设置。 
 */ 
typedef struct LedDevice { 
    // pin 用于标识 LED 所在的引脚 
    int pin; 
    // 初始化 LED 设备的函数指针 
    int (*Initialize)(struct LedDevice *ptLedDevice); 
    // 释放 LED 设备资源的函数指针 
    int (*Deinitialize)(struct LedDevice *ptLedDevice); 
    // 控制 LED 设备状态的函数指针 
    int (*ControlStatus)(struct LedDevice *ptLedDevice, int status); 
    // 设置 LED 设备颜色的函数指针 
    int (*SetLedColor)(struct LedDevice *ptLedDevice, int color); 
    // 设置 LED 设备亮度的函数指针 
    int (*SetLedBrightness)(struct LedDevice *ptLedDevice, int brightness); 
} LedDevice, *PLedDevice; 
 
// 根据指定引脚获取 LED 设备的函数 
PLedDevice GetLedDevice(int ledPin); 

 
#endif  // __LED_DEVICE_H__ 
