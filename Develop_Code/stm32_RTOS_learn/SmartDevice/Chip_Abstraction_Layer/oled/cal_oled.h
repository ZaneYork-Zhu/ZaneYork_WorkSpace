#ifndef __CAL_OLED_H__
#define __CAL_OLED_H__

#include "display_device.h"


int CAL_OledDeviceInit(void);
int CAL_OledDeviceDeInit(DisplayDevice *ptDisplayDevice);
int CAL_OledDeviceSetPixel(DisplayDevice *ptDisplayDevice, int x, int y, int isLit);
void CAL_OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice);



#endif  // __CAL_OLED_H__
