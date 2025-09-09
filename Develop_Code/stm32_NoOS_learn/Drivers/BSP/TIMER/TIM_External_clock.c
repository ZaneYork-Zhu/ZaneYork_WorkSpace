
#include "TIM_External_clock.h"
#include "led.h"
#include <stdio.h>

TIM_HandleTypeDef g_TIM_ExternalClock_Handle = {0};
TIM_ClockConfigTypeDef g_sClockSourceConfig_Handle = {0};
DMA_HandleTypeDef g_TIM_ExternalClock_DMA_Handle = {0};

static const uint32_t g_TIM_DMA_ARRbuffer[] = {5,6,7,8,9}; // DMA传输数据缓冲区

void TIM_External_Test(void)
{
    LED_init();
    uartInit(USART1, 115200); // 初始化串口1,波特率115200
    printf("TIM External Clock Test Start !!!\r\n");
    
    // 初始化外部时钟定时器
    TIM_External_clock_Init(EXTERNAL_TIM, 6, 1, 0, 0,TIM_COUNTERMODE_UP); //6次按键边沿，产生一次更新

    while (1)
    {

#if TIM_POLLING_ENABLE
        if(__HAL_TIM_GET_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_UPDATE))
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_UPDATE); // 清除更新事件标志位
            //LED1_TOGGLE(); // 翻转LED1状态
            printf("TIM External Clock Update Event Polling !!!\r\n");

        }
        if(__HAL_TIM_GET_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_TRIGGER))
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_ExternalClock_Handle, TIM_FLAG_TRIGGER); // 清除触发标志位
            //LED0_TOGGLE(); // 翻转LED0状态
            printf("TIM External Clock Trigger Event ,this is %d !!!\r\n", __HAL_TIM_GET_COUNTER(&g_TIM_ExternalClock_Handle));
        }else{
            //这里LED的引脚和外部时钟模式引脚短接->模拟
            LED0_TOGGLE(); // 翻转LED0状态
            HAL_Delay(250); // 延时250ms
        }
#endif // TIM_POLLING_ENABLE
#if TIM_IT_ENABLE || TIM_DMA_ENABLE
        //这里LED的引脚和外部时钟模式引脚短接->模拟
        LED0_TOGGLE(); // 翻转LED0状态
        HAL_Delay(250); // 延时250ms
#endif // TIM_IT_ENABLE || TIM_DMA_ENABLE
    }

}



void TIM_External_clock_Init(TIM_TypeDef* TimInstance, uint32_t arr, uint32_t psc,uint32_t setARPE ,uint32_t rep ,uint32_t countMode)
{ 
    g_TIM_ExternalClock_Handle.Instance = TimInstance; // 选择TIM2作为外部时钟定时器
    g_TIM_ExternalClock_Handle.Init.Prescaler = psc - 1; // 预分频
    g_TIM_ExternalClock_Handle.Init.CounterMode = countMode;
    g_TIM_ExternalClock_Handle.Init.Period = arr - 1;
    g_TIM_ExternalClock_Handle.Init.AutoReloadPreload =setARPE? TIM_AUTORELOAD_PRELOAD_ENABLE:TIM_AUTORELOAD_PRELOAD_DISABLE; // 自动重载预装载使能
    g_TIM_ExternalClock_Handle.Init.RepetitionCounter = rep; // 重复计数器值
    //定时器输入捕获模块的时钟分频
    g_TIM_ExternalClock_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&g_TIM_ExternalClock_Handle) != HAL_OK)
    {
        // 初始化失败，调用错误处理函数
        Error_Handler();
    }
    /**
     * @note: 这里必须关闭，防止被其他定时器进行触发
     */
    {
        TIM_SlaveConfigTypeDef sSlaveConfig = {0};
        sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE; // SMS=111
        sSlaveConfig.InputTrigger = TIM_TS_NONE;          // 触发源选择ETR 
        HAL_TIM_SlaveConfigSynchronization(&g_TIM_ExternalClock_Handle, &sSlaveConfig);
    }
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

#if TIM_EXTERNAL_MODE1_ETRMODE1_ENABLE
    /**
     * @note :1.实现功能->每当ETR发生变化时，产生一次更新事件[相当于是单边沿检测触发]
     *        2.一定要配置ClockDivision参数->ClockFilter->这个参数作为输入捕获1滤波器的采样频率
     * @warning :这里的ETR是边沿检测模式-> TIM_CLOCKPOLARITY_INVERTED ->反向触发事件
     */
    g_sClockSourceConfig_Handle.ClockSource = TIM_CLOCKSOURCE_ETRMODE1; 
    g_sClockSourceConfig_Handle.ClockPolarity = TIM_CLOCKPOLARITY_INVERTED; // 外部时钟ETR引脚的极性->反相

#endif

#if TIM_EXTERNAL_MODE2_ETRMODE2_ENABLE 
    /**
     * @note :1.实现功能->每当ETR发生变化时，产生一次更新事件[相当于是单边沿检测触发]
     *        2.一定要配置ClockDivision参数->ClockFilter->这个参数作为输入捕获1滤波器的采样频率
     * @warning :这里的ETR是边沿检测模式-> TIM_CLOCKPOLARITY_RISING ->按下产生1次触发事件
     */
    g_sClockSourceConfig_Handle.ClockSource = TIM_CLOCKSOURCE_ETRMODE2; 
    g_sClockSourceConfig_Handle.ClockPolarity = TIM_CLOCKPOLARITY_INVERTED; // 外部时钟ETR引脚的极性->正相

#endif
    g_sClockSourceConfig_Handle.ClockPrescaler = EXTERNAL_CLK_Prescaler; // 外部时钟不分频
    g_sClockSourceConfig_Handle.ClockFilter = EXTERNAL_CLK_Filter; // 外部时钟滤波
    if (HAL_TIM_ConfigClockSource(&g_TIM_ExternalClock_Handle, &g_sClockSourceConfig_Handle) != HAL_OK)
    {
        // 配置失败，调用错误处理函数
        Error_Handler();
    }
    
#if TIM_POLLING_ENABLE
    // 轮询模式下启动定时器
    // 启动定时器
    if (HAL_TIM_Base_Start(&g_TIM_ExternalClock_Handle) != HAL_OK)
    {
        // 启动失败，调用错误处理函数
        Error_Handler();
    }
#endif // TIM_POLLING_ENABLE

#if TIM_IT_ENABLE
    // 中断模式下启动定时器
    // 启动定时器
    if (HAL_TIM_Base_Start_IT(&g_TIM_ExternalClock_Handle) != HAL_OK)
    {
        // 启动失败，调用错误处理函数
        Error_Handler();
    }
    __HAL_TIM_ENABLE_IT(&g_TIM_ExternalClock_Handle, TIM_IT_TRIGGER); // 使能触发中断
#endif // TIM_IT_ENABLE

#if TIM_DMA_ENABLE
    // DMA模式下启动定时器
    // 启动定时器
    if (HAL_TIM_Base_Start_DMA(&g_TIM_ExternalClock_Handle, g_TIM_DMA_ARRbuffer, sizeof(g_TIM_DMA_ARRbuffer)/sizeof(g_TIM_DMA_ARRbuffer[0])) != HAL_OK)
    {
        // 启动失败，调用错误处理函数
        Error_Handler();
    }
    __HAL_TIM_ENABLE_IT(&g_TIM_ExternalClock_Handle, TIM_IT_UPDATE | TIM_IT_TRIGGER); // 使能更新中断+触发中断
    // for(uint32_t i = 0; i < sizeof(g_TIM_DMA_ARRbuffer)/sizeof(g_TIM_DMA_ARRbuffer[0]); i++)
    // {   
    //     printf("TIM External Clock DMA Buffer[%d] = %d\r\n", i, g_TIM_DMA_ARRbuffer[i]); // 打印DMA缓冲区数据
    // }
#endif // TIM_DMA_ENABLE
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == EXTERNAL_TIM)
    {
        // 使能外部时钟定时器时钟
        __HAL_RCC_EXTERNAL_TIM_CLK_ENABLE();

        // 配置外部时钟引脚
        GPIO_External_Input_Init();
#if TIM_IT_ENABLE
        // 配置定时器中断
        HAL_NVIC_SetPriority(EXTERNAL_TIM_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority);
        HAL_NVIC_EnableIRQ(EXTERNAL_TIM_IRQn); // 使能定时器中断
#endif // TIM_IT_ENABLE
#if TIM_DMA_ENABLE
        {
            // 使能DMA时钟
            __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE();

            // 配置DMA
            g_TIM_ExternalClock_DMA_Handle.Instance = EXTERNAL_DMA_Stream; // 选择DMA流
            g_TIM_ExternalClock_DMA_Handle.Init.Channel = EXTERNAL_DMA_Channel; // 选择DMA通道
            g_TIM_ExternalClock_DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 内存到外设传输
            g_TIM_ExternalClock_DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不递增
            g_TIM_ExternalClock_DMA_Handle.Init.MemInc = DMA_MINC_ENABLE; // 内存地址递增
            g_TIM_ExternalClock_DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // 外设数据对齐方式
            g_TIM_ExternalClock_DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; // 内存数据对齐方式
#if TIM_DMA_NORMAL_ENABLE
            g_TIM_ExternalClock_DMA_Handle.Init.Mode = DMA_NORMAL; // 正常模式
#else
            g_TIM_ExternalClock_DMA_Handle.Init.Mode = DMA_CIRCULAR; // 循环模式
#endif
            g_TIM_ExternalClock_DMA_Handle.Init.Priority = DMA_PRIORITY_VERY_HIGH; // 优先级中  

            if (HAL_DMA_Init(&g_TIM_ExternalClock_DMA_Handle) != HAL_OK)
            {
                // 初始化DMA失败，调用错误处理函数
                Error_Handler();
            }

            __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_UPDATE], g_TIM_ExternalClock_DMA_Handle); // 将DMA句柄链接到定时器句柄上
            // 配置DMA中断
            {
                HAL_NVIC_SetPriority(EXTERNAL_DMA_IRQn, TIM_DMA_PreemptPriority, TIM_DMA_SubPriority);
                HAL_NVIC_EnableIRQ(EXTERNAL_DMA_IRQn);
            }
            //配置IT中断
            {
                HAL_NVIC_SetPriority(EXTERNAL_TIM_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority);
                HAL_NVIC_EnableIRQ(EXTERNAL_TIM_IRQn);
            }

        }
#endif
    }
}

#if TIM_IT_ENABLE

void EXTERNAL_TIM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_ExternalClock_Handle); // 调用定时器中断处理函数
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == EXTERNAL_TIM)
    {
        static uint32_t ARR_Update_Num = 1; // 计数器
        if(__HAL_TIM_GET_FLAG(htim, TIM_IT_UPDATE) == RESET){
            __HAL_TIM_CLEAR_FLAG(htim, TIM_IT_UPDATE); // 清除更新事件标志位
            printf("TIM External Clock Update Event Callback Num = %d ,ARR is %d !!!\r\n", ARR_Update_Num, __HAL_TIM_GET_AUTORELOAD(htim));
            ARR_Update_Num++;
        }
    }
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == EXTERNAL_TIM)
    {
        printf("TIM External Clock Trigger Event Callback ,this is %d !!!\r\n", __HAL_TIM_GET_COUNTER(htim));
    }
}

#endif // TIM_IT_ENABLE


#if TIM_DMA_ENABLE

void EXTERNAL_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_TIM_ExternalClock_DMA_Handle); // 调用DMA中断处理函数
    
}

void EXTERNAL_TIM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_ExternalClock_Handle); // 调用定时器中断处理函数
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
    if(htim->Instance == EXTERNAL_TIM) 
    { 
        static uint32_t ARR_Update_Num = 1; // 计数器，初始化为0 
        // 检查更新事件标志位
        if(__HAL_TIM_GET_FLAG(htim, TIM_IT_UPDATE) == RESET) { 
            __HAL_TIM_CLEAR_FLAG(htim, TIM_IT_UPDATE); // 清除更新事件标志位 
            printf("TIM External Clock Update Event Callback Num = %d ,ARR update is %d !!!\r\n", ARR_Update_Num, __HAL_TIM_GET_AUTORELOAD(htim));
            if(ARR_Update_Num >= (sizeof(g_TIM_DMA_ARRbuffer)/sizeof(g_TIM_DMA_ARRbuffer[0]))) {                
                ARR_Update_Num = 1; // 重置计数器 
                __HAL_TIM_ENABLE_IT(htim, TIM_IT_TRIGGER); // 重新使能触发中断 
            } else { 
                // 当达到数组末尾，不再执行额外操作 
                ARR_Update_Num++;
            }
            
        }
        if(__HAL_DMA_GET_FLAG(&g_TIM_ExternalClock_DMA_Handle, DMA_IT_TC) == RESET && __HAL_TIM_GET_FLAG(htim, TIM_IT_UPDATE) == SET)
        {
            __HAL_DMA_CLEAR_FLAG(&g_TIM_ExternalClock_DMA_Handle, DMA_IT_TC); // 清除DMA传输完成标志位
            printf("--------------DMA finish-------\r\n");  
            LED1_TOGGLE(); // 翻转LED0状态 
        } 

    } 
} 
 
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) 
{ 
    if(htim->Instance == EXTERNAL_TIM) 
    { 
        if(__HAL_TIM_GET_FLAG(htim, TIM_IT_TRIGGER) == RESET) { 
            __HAL_TIM_CLEAR_FLAG(htim, TIM_IT_TRIGGER); // 清除触发标志位 
            printf("TIM External Clock Trigger Event Callback ,this is %d !!!\r\n", __HAL_TIM_GET_COUNTER(htim)); 
        }
    } 
} 

#endif // TIM_DMA_ENABLE



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
