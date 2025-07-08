
#include "TIM_External_clock.h"
#include "led.h"
#include <stdio.h>

TIM_HandleTypeDef g_TIM_ExternalClock_Handle = {0};
TIM_ClockConfigTypeDef g_sClockSourceConfig_Handle = {0};


void TIM_External_Test(void)
{
    LED_init();
    uartInit(USART1, 115200); // 初始化串口1,波特率115200
    printf("TIM External Clock Test Start !!!\r\n");
    
    // 初始化外部时钟定时器
    TIM_External_clock_Init(EXTERNAL_TIM, 6, 1); //6次按键边沿，产生一次更新

    while (1)
    {
        if(__HAL_TIM_GET_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_UPDATE))
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_UPDATE); // 清除更新事件标志位
            LED1_TOGGLE(); // 翻转LED1状态
            printf("TIM External Clock Update Event Polling !!!\r\n");

        }
        if(__HAL_TIM_GET_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_TRIGGER))
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_TRIGGER); // 清除触发标志位
            LED0_TOGGLE(); // 翻转LED0状态
            printf("TIM External Clock Trigger Event ,this is %d !!!\r\n", __HAL_TIM_GET_COUNTER(&g_TIM_ExternalClock_Handle));
        }
    }

}



void TIM_External_clock_Init(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc)
{ 
    // 1. 初始化定时器外部时钟功能
    // 2. 配置定时器为外部时钟模式
    // 3. 启动定时器
    g_TIM_ExternalClock_Handle.Instance = TimInstance; // 选择TIM2作为外部时钟定时器
    g_TIM_ExternalClock_Handle.Init.Prescaler = psc - 1; // 预分频
    g_TIM_ExternalClock_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_TIM_ExternalClock_Handle.Init.Period = arr - 1;
    //定时器输入捕获模块的时钟分频
    g_TIM_ExternalClock_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&g_TIM_ExternalClock_Handle) != HAL_OK)
    {
        // 初始化失败，调用错误处理函数
        Error_Handler();
    }
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    // sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE; // SMS=111
    // sSlaveConfig.InputTrigger = TIM_TS_NONE;          // 触发源选择ETR 
    // HAL_TIM_SlaveConfigSynchronization(&g_TIM_ExternalClock_Handle, &sSlaveConfig);
#if TIM_EXTERNAL_MODE1_TI1ED_ENABLE
    /**
     * @note :1.实现功能->每当TI1FED发生变化时，产生一次更新事件[相当于是双边沿检测触发]
     * @warning :这里的TI1ED是边沿检测模式->按下产生1次触发事件       
     *                                  ->抬起产生1次触发事件
     */
    g_sClockSourceConfig_Handle.ClockSource = TIM_CLOCKSOURCE_TI1ED; // 使用TI1ED作为外部时钟源---这里采用外部引脚
    g_sClockSourceConfig_Handle.ClockPolarity = EXTERNAL_CLK_Polarity; // 外部时钟上升沿触发
#endif

#if TIM_EXTERNAL_MODE1_TIxFPx_ENABLE
    /**
     * @note :1.实现功能->每当TI1发生变化时，产生一次更新事件[相当于是单边沿检测触发]
     *        2.一定要配置ClockDivision参数->ClockFilter->这个参数作为输入捕获1滤波器的采样频率
     * @warning :这里的TI1是边沿检测模式-> TIM_CLOCKPOLARITY_RISING ->按下产生1次触发事件
     *                                   TIM_CLOCKPOLARITY_BOTHEDGE->双边沿触发事件
     */
    g_sClockSourceConfig_Handle.ClockSource = TIM_CLOCKSOURCE_TI1; 
    g_sClockSourceConfig_Handle.ClockPolarity = EXTERNAL_CLK_Polarity;        // 外部时钟上升沿触发
#endif

#if TIM_EXTERNAL_MODE1__ETRMODE1_ENABLE
    /**
     * @note :1.实现功能->每当ETR发生变化时，产生一次更新事件[相当于是单边沿检测触发]
     *        2.一定要配置ClockDivision参数->ClockFilter->这个参数作为输入捕获1滤波器的采样频率
     * @warning :这里的ETR是边沿检测模式-> TIM_CLOCKPOLARITY_INVERTED ->反向触发事件
     */
    g_sClockSourceConfig_Handle.ClockSource = TIM_CLOCKSOURCE_ETRMODE1; 
    g_sClockSourceConfig_Handle.ClockPolarity = TIM_CLOCKPOLARITY_INVERTED; // 外部时钟ETR引脚的极性->反相

#endif
    g_sClockSourceConfig_Handle.ClockPrescaler = EXTERNAL_CLK_Prescaler; // 外部时钟不分频
    g_sClockSourceConfig_Handle.ClockFilter = EXTERNAL_CLK_Filter; // 外部时钟滤波
    if (HAL_TIM_ConfigClockSource(&g_TIM_ExternalClock_Handle, &g_sClockSourceConfig_Handle) != HAL_OK)
    {
        // 配置失败，调用错误处理函数
        Error_Handler();
    }
    
    
//    __HAL_TIM_SET_COUNTER(&g_TIM_ExternalClock_Handle, 0); // 清除计数器值
#if TIM_POLLING_ENABLE
    // 轮询模式下启动定时器
    // 启动定时器
    if (HAL_TIM_Base_Start(&g_TIM_ExternalClock_Handle) != HAL_OK)
    {
        // 启动失败，调用错误处理函数
        Error_Handler();
    }
#endif // TIM_POLLING_ENABLE
    __HAL_TIM_CLEAR_FLAG(&g_TIM_ExternalClock_Handle,TIM_FLAG_TRIGGER); // 清除触发事件标志位
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == EXTERNAL_TIM)
    {
        // 使能外部时钟定时器时钟
        __HAL_RCC_EXTERNAL_TIM_CLK_ENABLE();

        // 配置外部时钟引脚
        GPIO_External_Input_Init();
    }
}


static void GPIO_External_Input_Init(void)
{
    // 1. 初始化GPIO外部时钟引脚
    // 2. 配置GPIO为输入模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_EXTERNAL_CLK_ENABLE(); // 使能GPIOA时钟

    GPIO_InitStruct.Pin = EXTERNAL_Pin; // 选择外部时钟引脚
    GPIO_InitStruct.Mode = EXTERNAL_GPIO_Mode; // 设置为输入模式
    GPIO_InitStruct.Pull = EXTERNAL_GPIO_Pull; // 设置上拉/下拉
    GPIO_InitStruct.Alternate = EXTERNAL_GPIO_AF_FOR_TIM; // 设置复用功能
    GPIO_InitStruct.Speed = EXTERNAL_GPIO_Speed; // 设置GPIO速度
    HAL_GPIO_Init(EXTERNAL_GPIO_Port, &GPIO_InitStruct); // 初始化GPIO
}
