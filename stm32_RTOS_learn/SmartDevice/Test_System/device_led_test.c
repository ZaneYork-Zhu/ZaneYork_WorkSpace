//#include <stdio.h>
#include "device_led_test.h"
#include "led_device.h"
#include "kal_time.h"
void led_test(void)
{
	PLedDevice p1= GetLedDevice(LED_COLOR_RED);
	PLedDevice p2 = GetLedDevice(LED_COLOR_GREEN);
	if (p1 == NULL || p2 == NULL) {
		//printf("LED device not found!\n");
		return;
	}
	// 初始化LED设备
	p1->Initialize(p1);
	p2->Initialize(p2);

	while (1) {
		p1->ControlStatus(p1, 1); // 点亮红色LED
		p2->ControlStatus(p2, 0); // 熄灭绿色LED
		KAL_Delay(500);
		p1->ControlStatus(p1, 0); // 熄灭红色LED
		p2->ControlStatus(p2, 1); // 点亮绿色LED
		KAL_Delay(500);
	}
	
}
