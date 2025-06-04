#include "key_exit.h"
#include "led.h"


void keYExit_text_init(void)
{
    led_init();                             /* 初始化LED */
    key_exit_init();                           /* 初始化外部中断输入 */
    LED0(0);                                /* 先点亮红灯 */
}

void keYExit_text_while(void)
{
    HAL_Delay(1000);
}



void key_exit_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    /*Configure GPIO pins : KEY2_Pin KEY1_Pin KEY0_Pin */
    GPIO_InitStruct.Pin = KEY2_Pin|KEY1_Pin|KEY0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
        
        
    /*Configure GPIO pin : WKUP_Pin */
    GPIO_InitStruct.Pin = WKUP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 3);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/**
 * @brief       KEY0 外部中断服务程序
 * @param       无
 * @retval      无
 */
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_Pin);         /* 调用中断处理公用函数 清除KEY0所在中断线 的中断标志位 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_Pin);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       KEY1 外部中断服务程序
 * @param       无
 * @retval      无
 */
void EXTI3_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY1_Pin);         /* 调用中断处理公用函数 清除KEY1所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_Pin);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       KEY2 外部中断服务程序
 * @param       无
 * @retval      无
 */
void EXTI2_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY2_Pin);        /* 调用中断处理公用函数 清除KEY2所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY2_Pin);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       WK_UP 外部中断服务程序
 * @param       无
 * @retval      无
 */
void EXTI0_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(WKUP_Pin);        /* 调用中断处理公用函数 清除KEY_UP所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(WKUP_Pin);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       中断服务程序中需要做的事情
 *              在HAL库中所有的外部中断服务函数都会调用此函数
 * @param       GPIO_Pin:中断引脚号
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for(uint16_t t = 0;t<2000;t++){;}
    switch(GPIO_Pin)
    {
        case KEY0_Pin:
            if (KEY0 == 0)
            {
                LED0_TOGGLE();  /* LED0 状态取反 */ 
            }
            break;

        case KEY1_Pin:
            if (KEY1 == 0)
            {
                LED1_TOGGLE();  /* LED1 状态取反 */ 
            }
            break;

        case KEY2_Pin:
            if (KEY2 == 0)
            {
                LED1_TOGGLE();  /* LED1 状态取反 */
                LED0_TOGGLE();  /* LED0 状态取反 */ 
            }
            break;

        case WKUP_Pin:
            if (WK_UP == 1)
            {
                LED0(0);
                LED1(0);
            }
            break;

        default : break;
    }
}
