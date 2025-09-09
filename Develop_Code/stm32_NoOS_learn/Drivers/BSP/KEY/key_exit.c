#include "key_exit.h"
#include "led.h"

GPIO_InitTypeDef g_GPIO_InitStruct = {0};
void keYExit_text_init(void)
{
    LED_init();                             /* 初始化LED */
    key_exit_init();                           /* 初始化外部中断输入 */
    LED0(0);                                /* 先点亮红灯 */
}

void keYExit_text_while(void)
{
    HAL_Delay(1000);
}


/**
 * @brief       外部中断输入初始化
 * @param       无
 */
void key_exit_init(void)
{

    __HAL_RCC_KEY0_CLK_ENABLE();         /* 使能KEY0引脚时钟 */
    __HAL_RCC_KEY1_CLK_ENABLE();         /* 使能KEY1引脚时钟 */
    __HAL_RCC_KEY2_CLK_ENABLE();         /* 使能KEY2引脚时钟 */ 
    __HAL_RCC_WKUP_CLK_ENABLE();         /* 使能WKUP引脚时钟 */ 

    /*Configure GPIO pins : KEY0_Pin*/
    g_GPIO_InitStruct.Pin = KEY0_Pin;
    g_GPIO_InitStruct.Mode = KEY0_GPIO_Mode;
    g_GPIO_InitStruct.Pull = KEY0_GPIO_Pull;
    HAL_GPIO_Init(KEY0_GPIO_Port, &g_GPIO_InitStruct);
    /*Configure GPIO pins : KEY1_Pin*/
    g_GPIO_InitStruct.Pin = KEY1_Pin;
    g_GPIO_InitStruct.Mode = KEY1_GPIO_Mode;
    g_GPIO_InitStruct.Pull = KEY1_GPIO_Pull;
    HAL_GPIO_Init(KEY1_GPIO_Port, &g_GPIO_InitStruct);
    /*Configure GPIO pins : KEY2_Pin*/
    g_GPIO_InitStruct.Pin = KEY2_Pin;
    g_GPIO_InitStruct.Mode = KEY2_GPIO_Mode;
    g_GPIO_InitStruct.Pull = KEY2_GPIO_Pull;
    HAL_GPIO_Init(KEY2_GPIO_Port, &g_GPIO_InitStruct);
    /*Configure GPIO pin : WKUP_Pin */
    g_GPIO_InitStruct.Pin = WKUP_Pin;
    g_GPIO_InitStruct.Mode = WKUP_GPIO_Mode;
    g_GPIO_InitStruct.Pull = WKUP_GPIO_Pull;
    HAL_GPIO_Init(WKUP_GPIO_Port, &g_GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(WKUP_EXTI_IRQn, WKUP_PreemptPriority, WKUP_SubPriority);
    HAL_NVIC_EnableIRQ(WKUP_EXTI_IRQn);

    HAL_NVIC_SetPriority(KEY2_EXTI_IRQn, KEY2_PreemptPriority, KEY2_SubPriority);
    HAL_NVIC_EnableIRQ(KEY2_EXTI_IRQn);

    HAL_NVIC_SetPriority(KEY1_EXTI_IRQn, KEY1_PreemptPriority, KEY1_SubPriority);
    HAL_NVIC_EnableIRQ(KEY1_EXTI_IRQn);

    HAL_NVIC_SetPriority(KEY0_EXTI_IRQn, KEY0_PreemptPriority, KEY0_SubPriority);
    HAL_NVIC_EnableIRQ(KEY0_EXTI_IRQn);

}


/**
 * @brief       KEY0 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY0_EXTI_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_Pin);         /* 调用中断处理公用函数 清除KEY0所在中断线 的中断标志位 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_Pin);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       KEY1 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY1_EXTI_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY1_Pin);         /* 调用中断处理公用函数 清除KEY1所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_Pin);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       KEY2 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY2_EXTI_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY2_Pin);        /* 调用中断处理公用函数 清除KEY2所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY2_Pin);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       WK_UP 外部中断服务程序
 * @param       无
 * @retval      无
 */
void WKUP_EXTI_IRQHandler(void)
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
