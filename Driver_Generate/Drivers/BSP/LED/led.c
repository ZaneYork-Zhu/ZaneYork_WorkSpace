#include "led.h"

void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
      __HAL_RCC_GPIOF_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOF, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : LED0_Pin LED1_Pin */
    GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*LED输出高电平 默认不亮*/
    LED0(1);
    LED1(1);
}

void led_text(void)
{
    while (1)
    {
        LED0(0);
        LED1(0);
        HAL_Delay(500);
        LED0(1);
        LED1(1);
        HAL_Delay(500);
    }
    
}
