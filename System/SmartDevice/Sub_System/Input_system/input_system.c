/* 
* 文件名: input_system.c 
* 功能概述: 该文件实现了一个输入设备管理系统，负责管理所有注册的输入设备。 
* 
* 交互原理: 
* 此文件是整个输入管理系统的核心，维护着一个全局的输入设备链表 `g_ptInputDeviceList`。 
* 它提供了三个主要的函数：`InputDeviceRegister`、`AddInputDevices` 和 `InitInputDevices`。 
* - `InputDeviceRegister` 函数是底层的设备插入函数，用于将单个输入设备添加到全局链表中，采用头插法，确保新设备总是位于链表头部。 
* - `AddInputDevices` 函数是上层的设备管理函数，用于调用具体的设备添加函数，将不同类型的输入设备批量添加到链表中。
*    具体过程为：
                该函数会调用其他具体的设备添加函数（如 `AddGPIOKeyInputDevice`），
                这些具体的函数内部会调用 `InputDeviceRegister` 函数来完成单个设备的插入。 
* - `InitInputDevices` 函数用于对已注册到链表中的所有输入设备进行初始化操作，它会遍历链表并调用每个设备的 `DeviceInit` 函数。 
* 
* 输入管理系统实现方式: 
* 系统通过维护一个全局的输入设备链表，允许不同类型的输入设备（如 GPIO 按键设备）注册到系统中。 
* 当有新的输入设备需要被系统识别和处理时:
        先调用 `AddInputDevices` 函数将设备添加到链表中,
        （内部会调用 `InputDeviceRegister` 进行注册），
        之后调用 `InitInputDevices` 函数对已注册的设备进行初始化。 
* 这样，系统就可以统一管理和调度所有注册的输入设备。 
*/ 
 
#include "input_system.h" 
#include "gpio_key.h" // 包含 GPIO 按键设备的头文件 
 
// 全局输入设备链表 
static PInputDevice g_ptInputDeviceList; 
 
/** 
* @brief 注册单个输入设备到链表 
* @param ptInputDevice 输入设备指针 
* 
* 该函数是底层的设备插入函数，将单个输入设备添加到全局输入设备链表中。 
* 采用头插法，如果链表为空，则新设备成为链表的头部。 
* 此函数不关心设备的具体类型，只负责维护链表结构。 
*/ 
void InputDeviceRegister(PInputDevice ptInputDevice) 
{ 
    if (ptInputDevice == NULL) { 
        return; // 错误处理：输入设备指针为空 
    } 
    // 将新设备的 pNextDevice 指向当前链表头 
    ptInputDevice->pNextDevice = g_ptInputDeviceList; 
    // 更新链表头指针指向新设备 
    g_ptInputDeviceList = ptInputDevice; 
} 
 
/** 
* @brief 批量添加不同类型的输入设备到链表 
* 
* 该函数是上层的设备管理函数，用于调用具体的设备添加函数， 
* 将不同类型的输入设备批量添加到链表中。 
* 它不直接进行链表操作，而是调用其他具体的设备添加函数， 
* 这些具体的函数内部会调用 `InputDeviceRegister` 函数来完成单个设备的插入。 
*/ 
void AddInputDevices(void) 
{ 
    AddGPIOKeyInputDevice(); // 添加 KEY 输入设备 
    // 可以在这里添加其他类型的输入设备，例如触摸屏、OLED 显示等
    // AddTouchInputDevice(); // 添加触摸屏输入设备
    // AddOLEDInputDevice(); // 添加 OLED 显示输入设备
} 
 
/** 
 * 初始化输入设备链表。 
 * 该函数会遍历所有已注册的输入设备，并调用它们的 
 * `DeviceInit` 函数进行初始化。 
 * 此函数在 `AddInputDevices` 调用之后使用，确保对已注册的设备进行初始化。 
 */ 
void InitInputDevices(void) 
{ 
    PInputDevice pDevice = g_ptInputDeviceList; // 获取当前输入设备链表头 
    while (pDevice != NULL) 
    { 
        pDevice->DeviceInit(); // 调用设备的初始化函数 
        pDevice = pDevice->pNextDevice; // 移动到下一个设备 
    } 
} 
