#include "TIM_OC_Mode.h"
#include <stdio.h>
#include "led.h"


TIM_HandleTypeDef g_TIM_OCToggle_Handle = {0};

TIM_OC_InitTypeDef g_sConfigOC_Handle = {0};
DMA_HandleTypeDef g_hdma = {0};



NEW_TIM_HandleTypeDef g_TIM_OC_Base_Struct = {
    .HAL_Handle.Instance = TIMx_OC_INSTANCE, // 定时器实例
    .HAL_Handle.Init = {
        .Period = 10000 - 1,                                // 10ms
        .Prescaler = 8400 - 1,                              // 1ms
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,            // 时钟分频系数：不分频
        .CounterMode = TIM_COUNTERMODE_UP,                  // 计数模式：上升计数
        .RepetitionCounter = 0,                             // 重复计数值：0
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE // 自动重载预装载：禁用
    },
    .OCchannel = TIM_CHANNEL_1,                             // 输出通道：CH1
};

TIM_OC_InitTypeDef g_TIM_OC_Channel_Struct = {
    .OCMode = TIM_OCMODE_TOGGLE,                            // 输出比较模式：翻转模式         
    .Pulse = 10000/2,                                       // 50% 占空比
    .OCPolarity = TIM_OCPOLARITY_HIGH,                      // 输出极性：高电平有效
    .OCNPolarity = TIM_OCNPOLARITY_HIGH,                    // 互补输出极性：高电平有效
    .OCFastMode = TIM_OCFAST_DISABLE,                       // 快速模式：禁用
    .OCIdleState = TIM_OCIDLESTATE_RESET,                   // 空闲状态下输出状态：复位 
};

void TIM_OC_MODE_Test(void)
{
    // 初始化LED灯
    LED_Display_init();
    // 初始化串口
    uartInit(USART1,115200); 
    // 初始化定时器输出比较模式为翻转模式，设置自动重装载寄存器值、预分频系数等参数
    TIM_OC_Toggle_Mode_Init(&g_TIM_OC_Base_Struct, &g_TIM_OC_Channel_Struct);


    while (1)
    {
        // 轮询输出比较
        if (__HAL_TIM_GET_FLAG(&g_TIM_OCToggle_Handle, TIM_FLAG_CC1))
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_OCToggle_Handle, TIM_FLAG_CC1);
            printf("TIM OC Event,CCR_Value =  %d !!\r\n", __HAL_TIM_GET_COMPARE(&g_TIM_OCToggle_Handle, TIM_CHANNEL_1));
            
        }
    }
}

void TIM_OC_Toggle_Mode_Init(NEW_TIM_HandleTypeDef *pBaseStr, TIM_OC_InitTypeDef *pOCInitStr)
{
    g_TIM_OCToggle_Handle.Instance = TIMx_OC_INSTANCE;
    g_TIM_OCToggle_Handle.Init.Period = pBaseStr->HAL_Handle.Init.Period;
    g_TIM_OCToggle_Handle.Init.Prescaler = pBaseStr->HAL_Handle.Init.Prescaler;
    g_TIM_OCToggle_Handle.Init.ClockDivision = pBaseStr->HAL_Handle.Init.ClockDivision;
    g_TIM_OCToggle_Handle.Init.CounterMode = pBaseStr->HAL_Handle.Init.CounterMode;
    g_TIM_OCToggle_Handle.Init.RepetitionCounter = pBaseStr->HAL_Handle.Init.RepetitionCounter;
    if(HAL_TIM_OC_Init(&g_TIM_OCToggle_Handle) != HAL_OK)
    {
        printf("TIM OC Init Error!\n");
        return;
    }
    g_sConfigOC_Handle.OCMode = pOCInitStr->OCMode;
    g_sConfigOC_Handle.Pulse = pOCInitStr->Pulse;
    g_sConfigOC_Handle.OCPolarity = pOCInitStr->OCPolarity;
    g_sConfigOC_Handle.OCNPolarity = pOCInitStr->OCNPolarity;
    g_sConfigOC_Handle.OCFastMode = pOCInitStr->OCFastMode;
    if(HAL_TIM_OC_ConfigChannel(&g_TIM_OCToggle_Handle, &g_sConfigOC_Handle, pBaseStr->OCchannel) != HAL_OK)
    {
        printf("TIM OC Config Channel Error!\n");
        return;
    }

#if TIM_OC_MODE_POLLING_ENABLE
    // 启动定时器输出比较中断
    HAL_TIM_OC_Start(&g_TIM_OCToggle_Handle, pBaseStr->OCchannel);
#endif /* TIM_OC_MODE_POLLING_ENABLE */

}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_OCToggle_Handle.Instance)
    {
        __TIMx_OC_INSTANCE_CLK_ENABLE(); // 启用定时器时钟
        GPIO_OC_CHx_Init(); // 初始化GPIO引脚
    }
}

/**
 * @brief  GPIO初始化
 * @param  None
 * @retval None
 */
static void GPIO_OC_CHx_Init(void)
{
    GPIO_InitTypeDef GPIO_Init_Handle = {0};

    __HAL_RCC_GPIO_OC_CHx_CLK_ENABLE();                         // 启用GPIO时钟
    GPIO_Init_Handle.Pin = TIMx_OC_CHx_Pin;                     // 定义输出比较通道引脚
    GPIO_Init_Handle.Mode = TIMx_OC_CHx_Mode;                   // 设置引脚模式输出
    GPIO_Init_Handle.Pull = GPIO_NOPULL;                        // 引脚上拉
    GPIO_Init_Handle.Speed = TIMx_OC_CHx_Speed;                 // 设置引脚速度
    GPIO_Init_Handle.Alternate = TIMx_OC_CHx_AF_FOR_TIM;        // 设置引脚复用功能
    
    HAL_GPIO_Init(TIMx_OC_CHx_GPIO_Port, &GPIO_Init_Handle);    // 初始化GPIO
}



/**
 * @note : OC输出引脚->通过跳线连接LED引脚
 * @brief  LED初始化
 * @param  None
 * @retval None
 */
static void LED_Display_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOF_CLK_ENABLE();

    /*Configure GPIO pins : LED0_Pin */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);

}

