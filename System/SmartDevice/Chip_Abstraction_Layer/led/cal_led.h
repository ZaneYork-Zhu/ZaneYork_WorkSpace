#ifndef __CAL_LED_DEVICE_H__
#define __CAL_LED_DEVICE_H__

#include "led_device.h"

int CAL_LedDeviceInit(struct LedDevice *ptLedDevice);
int CAL_LedDeviceControl(struct LedDevice *ptLedDevice, int status);

#endif  // __CAL_LED_DEVICE_H__
