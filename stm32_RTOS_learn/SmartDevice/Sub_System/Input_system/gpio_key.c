/*
 * 文件名: gpio_key.c
 * 功能概述: GPIO按键设备驱动实现文件 
 * 主要功能:
 *   1. 定义GPIO按键设备结构体
 *   2. 实现GPIO按键设备初始化函数 
 *   3. 实现GPIO按键设备注册函数
 * 交互关系:
 *   - 通过InputDeviceRegister()函数将设备注册到输入管理系统 
 *   - 通过GPIOKeyInit()函数初始化硬件GPIO
 * 版本历史:
 *   v1.0 2025-06-29 初始版本 
 */
 
#include "kal_gpio_key.h"
#include "input_system.h"
/**
 * @brief GPIO按键设备初始化函数
 * @return int 初始化状态 
 *     @retval 0 初始化成功
 *     @retval 负数 初始化失败
 * 
 * @details 该函数完成以下工作:
 *   1. 调用平台相关的GPIO初始化函数
 *   2. 配置GPIO引脚工作模式
 *   3. 设置GPIO中断相关参数 
 *   4. 其他必要的硬件初始化操作 
 * 
 * @note 该函数会被输入管理系统在初始化阶段调用
 */
static int GPIOKeyInit(void)
{
    KAL_GpioKeyInit();  // 平台相关的GPIO初始化
    /* 这里实现具体的GPIO配置逻辑 */
    return 0;  // 初始化成功
}

/**
 * @struct InputDevice
 * @brief GPIO按键设备结构体 
 * @var name 设备名称标识 
 * @var GetInputEventState 获取输入事件状态函数指针 
 * @var DeviceInit 设备初始化函数指针
 * @var DeviceDeInit 设备注销函数指针
 * @var pNextDevice 指向下一个设备的指针(用于链表管理)
 * 
 * @note 该结构体描述了一个完整的输入设备，包含设备基本信息和操作接口 
 */
static InputDevice g_tGpioKeyDevice = {
    .name = "GPIO_Key_Device",
    .GetInputEventState = NULL,  // 待实现的输入事件获取函数 
    .DeviceInit = GPIOKeyInit,   // 指向设备初始化函数
    .DeviceDeInit = NULL,        // 待实现的设备注销函数 
    .pNextDevice = NULL          // 初始化为空指针 
};
 

/**
 * @brief 注册GPIO按键设备到输入系统
 * 
 * @details 该函数完成以下工作:
 *   1. 将g_tGpioKeyDevice注册到输入设备链表 
 *   2. 通过InputDeviceRegister()实现设备注册 
 * 
 * @note 使用流程:
 *   1. 在系统初始化阶段调用此函数
 *   2. 该函数会调用InputDeviceRegister()完成实际注册
 *   3. 注册后设备可通过输入管理系统统一管理
 */ 
void AddGPIOKeyInputDevice(void)
{
    InputDeviceRegister(&g_tGpioKeyDevice);
}
