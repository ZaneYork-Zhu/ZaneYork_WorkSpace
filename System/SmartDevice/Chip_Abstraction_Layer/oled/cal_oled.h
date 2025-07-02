#ifndef __CAL_OLED_H__
#define __CAL_OLED_H__

#include "display_device.h"


int CAL_OledDeviceInit(DisplayDevice *ptDisplayDevice);
int CAL_OledDeviceDeInit(DisplayDevice *ptDisplayDevice);
int CAL_OledDeviceSetPixel(DisplayDevice *ptDisplayDevice, int x, int y);
void CAL_OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice);



#endif  // __CAL_OLED_H__
