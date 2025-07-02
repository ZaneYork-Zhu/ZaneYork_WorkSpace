#ifndef __DISPLAY_SYSTEM_H__ 
#define __DISPLAY_SYSTEM_H__ 

#include "display_device.h"
#include <stdio.h>
// 添加显示设备的函数声明
void AddDisplayDevices(void);

PDisplayDevice GetDisplayDevice(char *name);
#endif  // __DISPLAY_SYSTEM_H__
