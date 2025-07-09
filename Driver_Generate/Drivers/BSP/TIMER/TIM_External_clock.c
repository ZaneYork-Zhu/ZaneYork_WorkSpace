
#include "TIM_External_clock.h"
#include "led.h"
#include <stdio.h>

TIM_HandleTypeDef TIM_ExternalClock_Handle = {0};
TIM_ClockConfigTypeDef sClockSourceConfig_Handle = {0};

void TIM_External_clock_Init(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc, uint8_t setARPE)
{ 
    // 1. 初始化定时器外部时钟功能
    // 2. 配置定时器为外部时钟模式
    // 3. 启动定时器
    TIM_ExternalClock_Handle.Instance = TimInstance; // 选择TIM2作为外部时钟定时器
    TIM_ExternalClock_Handle.Init.Prescaler = psc - 1; // 预分频
    TIM_ExternalClock_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM_ExternalClock_Handle.Init.Period = arr - 1;
    TIM_ExternalClock_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM_ExternalClock_Handle.Init.AutoReloadPreload = setARPE ? TIM_AUTORELOAD_PRELOAD_ENABLE : TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&TIM_ExternalClock_Handle) != HAL_OK)
    {
        // 初始化失败，调用错误处理函数
        Error_Handler();
    }
    // 配置外部时钟源
    sClockSourceConfig_Handle.ClockSource = TIM_CLOCKSOURCE_ETRMODE1;
    sClockSourceConfig_Handle.ClockPolarity = TIM_CLOCKPOLARITY_RISING; // 外部时钟上升沿触发
    sClockSourceConfig_Handle.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1; // 外部时钟不分频
    sClockSourceConfig_Handle.ClockFilter = 0; // 外部时钟滤波
    if (HAL_TIM_ConfigClockSource(&TIM_ExternalClock_Handle, &sClockSourceConfig_Handle) != HAL_OK)
    {
        // 配置失败，调用错误处理函数
        Error_Handler();
    }
}