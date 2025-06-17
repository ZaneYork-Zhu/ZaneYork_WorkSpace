#include "led.h"

void LED_init(void)
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
    LED0(0);
    LED1(0);
}

void LED_text(void)
{
    while (1)
    {
        LED0(1);
        LED1(0);
        HAL_Delay(500);
        LED0(0);
        LED1(1);
        HAL_Delay(50);
    }
    
}
