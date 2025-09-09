#ifndef __KAL_LED_DEVICE_H__
#define __KAL_LED_DEVICE_H__
#include "led_device.h"


int KAL_LedDeviceInit(struct LedDevice *ptLedDevice);
int KAL_LedDeviceControl(struct LedDevice *ptLedDevice, int status);

#endif  // __KAL_LED_DEVICE_H__
