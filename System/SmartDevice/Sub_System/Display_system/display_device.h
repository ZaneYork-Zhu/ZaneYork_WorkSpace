#ifndef __DISPLAY_DEVICE_H__ 
#define __DISPLAY_DEVICE_H__ 


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
typedef struct DisplayDevice {
     
    char *name; // 设备名称
    // pin 用于标识 LED 所在的引脚 
    int pin; 
    // 初始化 LED 设备的函数指针 
    int (*Initialize)(struct DisplayDevice *ptDisplayDevice); 
    // 释放 LED 设备资源的函数指针 
    int (*Deinitialize)(struct DisplayDevice *ptDisplayDevice); 
    // 控制 LED 设备状态的函数指针 
    int (*ControlStatus)(struct DisplayDevice *ptDisplayDevice, int status); 
    // 设置 LED 设备颜色的函数指针 
    int (*SetLedColor)(struct DisplayDevice *ptDisplayDevice, int color); 
    // 设置 LED 设备亮度的函数指针 
    int (*SetLedBrightness)(struct DisplayDevice *ptDisplayDevice, int brightness); 
} DisplayDevice, *PDisplayDevice; 
 
// 根据指定引脚获取 LED 设备的函数 
PDisplayDevice GetLedDevice(int ledPin); 

 
#endif  // __DISPLAY_DEVICE_H__ 
