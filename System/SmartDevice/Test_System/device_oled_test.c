#include "device_oled_test.h"
#include "oled_device.h"
#include "display_system.h"
#include <string.h>
#include <stdio.h>
#include "oled_ssd1306.h"
void display_oled_test(void)
{
	PDisplayDevice ptDevice;
	char *device_name = "OLED Display";
	int setX,setY;
	
	AddDisplayDevices(); // 添加显示设备到系统

	ptDevice = GetDisplayDevice(device_name); // 获取显示设备
	if (ptDevice == NULL) {
		printf("Display device %s not found.\r\n", device_name);
		return;
	}

	// 1.初始化显示设备
	ptDevice->Initialize(); // 调用初始化函数
	// 2.清除屏幕
	memset(ptDevice->GramAddress, 0, 128*8); // 清除显存
	// 3.画线
	setY = ptDevice->ptData->iY / 2; // 设置Y坐标为屏幕中间
	for (setX = 0; setX < ptDevice->ptData->iX; setX++) { // 循环画线
		ptDevice->SetPixel(ptDevice, setX, setY, 1); // 画线
	}
	for(int i = 0;i<(ptDevice->ptData->iX * ptDevice->ptData->iY * ptDevice->ptData->iBpp/8);i++)
	{
		printf("%d 0x%02x\t",i,ptDevice->GramAddress[i]);
	}
	// 4 .刷新
	OLED_UpdateToGRAM_ALL(); // 刷新显存到屏幕
	// ptDevice->FlushGramForDisplay(ptDevice);
}
