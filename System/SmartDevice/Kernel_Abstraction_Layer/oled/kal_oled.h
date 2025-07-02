#ifndef __KAL_OLED_H__
#define __KAL_OLED_H__

#include "display_device.h"

int KAL_OledDeviceInit(DisplayDevice *ptDisplayDevice);
int KAL_OledDeviceDeInit(DisplayDevice *ptDisplayDevice);
int KAL_OledDeviceSetPixel(DisplayDevice *ptDisplayDevice, int x, int y);
void KAL_OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice);

#endif  // __KAL_OLED_H__
