#include "LED.h"
#include "led_device.h"
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* GPIO Ports Clock Enable */
    __LED0_GPIO_CLK_ENABLE();
    __LED1_GPIO_CLK_ENABLE();

    /*Configure GPIO pins : LED0_Pin LED1_Pin */
    GPIO_InitStruct.Pin = LED0_Pin;
    GPIO_InitStruct.Mode = LED0_GPIO_MODE; // GPIO_MODE_OUTPUT_PP
    GPIO_InitStruct.Pull = LED0_GPIO_PULL; // GPIO_NOPULL
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(LED0_GPIO_Port,&GPIO_InitStruct);

    /*Configure GPIO pins : LED0_Pin LED1_Pin */
    GPIO_InitStruct.Pin = LED1_Pin;
    GPIO_InitStruct.Mode = LED1_GPIO_MODE; // GPIO_MODE_OUTPUT_PP
    GPIO_InitStruct.Pull = LED1_GPIO_PULL; // GPIO_NOPULL
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(LED1_GPIO_Port,&GPIO_InitStruct);

    /*initialize LEDs to off state*/
    LED0_RED(0);
    LED1_GREEN(0);
}

int HAL_LedDeviceControl(struct LedDevice *ptLedDevice, int status)
{
    if (ptLedDevice == NULL) {
        return -1; // 错误：LED设备指针为空
    }
    switch (ptLedDevice->pin)
    {
        case LED_COLOR_RED:{
            LED0_RED(status); // 控制LED0的红色状态
            break;
        }
        case LED_COLOR_GREEN:{
            LED1_GREEN(status); // 控制LED1的绿色状态
            break;
        }
        // 其他颜色可以根据需要添加
        
        default:
            return -2; // 错误：不支持的LED颜色
    }
    return 0; // 成功
}

