#include "display_system.h"
#include "oled_device.h"


/**
 * @brief 添加OLED显示设备到系统
 * 该函数将OLED显示设备注册到显示设备系统中。
 * @details 该函数完成以下工作:
 * 1. 调用DisplayDeviceRegister()函数将OLED显示设备注册到显示设备系统中。
 * 2. 调用OLED显示设备的初始化函数进行初始化。
 * * @return 无返回值
 */
void AddDisplayDevices(void) 
{
    AddOledDisplayDevice(); // 添加OLED显示设备
    // 可以在这里添加其他类型的显示设备
}

/**
 * @brief 获取指定名称的显示设备
 * @param name 显示设备的名称
 * @return 返回指定名称的显示设备，如果未找到则返回NULL
 * * 该函数用于根据设备名称获取对应的显示设备指针。
 * * @note 该函数会遍历显示设备列表，并返回与指定名称匹配的显示设备指针。
 * * 如果找不到对应的设备，则返回NULL。
 */
PDisplayDevice GetDisplayDevice(char *name)
{
    return __GetDisplayDevice(name);
}
