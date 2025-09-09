#ifndef __KAL_TIME_H__
#define __KAL_TIME_H__

#include "led_device.h"

#ifndef TIME_T
#define TIME_T unsigned long
#endif

/* 获取时间函数*/
TIME_T KAL_GetTime(void);
void KAL_Delay(TIME_T delay_ms);
#endif  // __KAL_TIME_H__
