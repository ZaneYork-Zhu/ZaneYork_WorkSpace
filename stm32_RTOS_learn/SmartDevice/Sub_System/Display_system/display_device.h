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
    int color; // 颜色
} DisplayDeviceData;

// 定义一个枚举类型 OperationResult，用于表示操作的结果 
// 枚举类型是一种用户自定义的数据类型，它可以将一组相关的常量组合在一起 
typedef enum { 
    OP_SUCCESS = 1,  // 表示操作成功，将其赋值为 1 
    OP_FAILURE = 0   // 表示操作失败，将其赋值为 0 
} OperationResult; 

/**
 * @brief 显示设备结构体
 *
 * 该结构体定义了显示设备的基本属性和操作函数指针。
 */
typedef struct DisplayDevice {
     
    char *name; // 设备名称
    char *GramAddress; // 显存地址
    DisplayDeviceData *ptData; // 显示设备数据指针
    OperationResult OperationState; // 操作结果状态
    //初始化显示设备的函数指针
    int (*Initialize)(void);
    // 释放显示设备资源的函数指针
    int (*Deinitialize)(struct DisplayDevice *ptDisplayDevice);
    // 清除显示设备的函数指针
    int (*SetPixel)(struct DisplayDevice *ptDisplayDevice,int x,int y,int isLit);
    // 刷新显示设备的函数指针
    void (*FlushGramForDisplay)(struct DisplayDevice *ptDisplayDevice);
    // Dispaly设备链表
    struct DisplayDevice *pNextDevice; // 指向下一个显示设备的指针
    
} DisplayDevice, *PDisplayDevice; 


// 全局变量，用于保存所有注册的显示设备
void DisplayDeviceRegister(PDisplayDevice ptDisplayDevice);
// 获取显示设备的函数声明
PDisplayDevice __GetDisplayDevice(char *name); 
#endif  // __DISPLAY_DEVICE_H__
