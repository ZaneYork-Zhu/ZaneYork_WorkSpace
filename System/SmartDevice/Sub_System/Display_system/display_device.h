#ifndef __DISPLAY_DEVICE_H__ 
#define __DISPLAY_DEVICE_H__ 


#ifndef NULL
#define NULL ((void *)0) 
#endif

// 定义显示设备类型
typedef enum { 
    Display_OLED = 0, // OLED显示设备
    Display_LCD,      // LCD显示设备
    Display_printf, // printf显示设备
} DisplayDeviceType;

typedef struct device_data{
    int iX; // X坐标
    int iY; // Y坐标
    int iBpp; // 每像素位数
} DisplayDeviceData;


/**
 * @brief 显示设备结构体
 *
 * 该结构体定义了显示设备的基本属性和操作函数指针。
 */
typedef struct DisplayDevice {
     
    char *name; // 设备名称
    char *GramAddress; // 显存地址
    DisplayDeviceData *ptData; // 显示设备数据指针
    //初始化显示设备的函数指针
    int (*Initialize)(struct DisplayDevice *ptDisplayDevice);
    // 释放显示设备资源的函数指针
    int (*Deinitialize)(struct DisplayDevice *ptDisplayDevice);
    // 清除显示设备的函数指针
    int (*SetPixel)(struct DisplayDevice *ptDisplayDevice, int x, int y);
    // 刷新显示设备的函数指针
    void (*FlushGramForDisplay)(struct DisplayDevice *ptDisplayDevice);
} DisplayDevice, *PDisplayDevice; 

// 根据指定显存地址获取显示设备的函数 
PDisplayDevice GetDisplayDevice(char *gramAddress); 

#endif  // __DISPLAY_DEVICE_H__
