#include "TIM_OC_PWM_Mode.h"
#include <stdio.h>


TIM_HandleTypeDef g_TIM_OCPwm_Handle = {0};

TIM_OC_InitTypeDef g_sConfigOC_Handle = {0};
DMA_HandleTypeDef g_hdma = {0};



#if TIM_OC_PWM_MODE_ENABLE

uint8_t g_Pwm_Flag = 0; // 翻转标志位
uint32_t g_PwmPolar_oldValue = 0; 
uint32_t g_PwmPolar_newValue = 0; 
// 低 100->高300-100 = 200 ->低 600-300 = 300 ->高 1000-600 = 400 ->低 1500-1000 = 500 ->高
//
uint32_t g_DMA_CCRBuffer[4] = {300 - 1,600 - 1,1000 - 1,1500 - 1}; // DMA CCR寄存器值缓冲区



NEW_TIM_HandleTypeDef g_TIM_OC_Base_Struct = {
    .HAL_Handle.Instance = TIMx_OC_INSTANCE, // 定时器实例
    .HAL_Handle.Init = {
        .Period = 10000 - 1,                                 // 1s-> 10000计数 ->1个计数值 = 0.1ms
        .Prescaler = 8400 - 1,                              //10khz 
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,            // 时钟分频系数：不分频
        /*计数模式：中心对齐模式3->计数上和下都触发标志位,模式1和模式2是单边触发*/
        .CounterMode = TIM_COUNTERMODE_UP,                  
        .RepetitionCounter = 0,                             // 重复计数值：0
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE // 自动重载预装载：禁用
    },
    .OCchannel = TIM_CHANNEL_1,                             // 输出通道：CH1
};

TIM_OC_InitTypeDef g_TIM_OC_Channel_Struct = {
    .OCMode = TIM_OCMODE_PWM1,                            // 输出比较模式：PWM1模式
    .Pulse = 2000 - 1,                                         
    //如果TIM_OCPOLARITY_LOW->0~pulse会产生一个[短暂]高电平脉冲
    .OCPolarity = TIM_OCPOLARITY_HIGH,                      // 输出极性：高电平有效
};

/**
 * @note : 和toggle比较，PWM模式翻转2次电平，CNT = CCR 翻转一次 和CNT = ARR 翻转一次
 *        1.Pwm1模式下——>UP/down计数
 *          翻转条件：CNT<CCR=有效电平，CNT>=CCR=无效电平
 * 极限值：pulse = 0->CNT 恒等于 CCR = 0
 *          极性：
 *             1`TIM_OCPOLARITY_HIGH[高有效]:  全低电平
 *             2`TIM_OCPOLARITY_LOW[低有效]:   全高电平
 *          周期：无
 *          占空比：无
 * 极限值：pulse = ARR->
 *          极性：
 *             1`TIM_OCPOLARITY_HIGH[高有效]:  短暂脉冲CNT = CCR 低-->其他高电平
 *             2`TIM_OCPOLARITY_LOW[低有效]:   短暂脉冲CNT = CCR 高-->其他低电平
 *          周期：ARR
 *          占空比：ARR值 / ARR值 = 100% (固定)
 * 中间态：pulse = 0~ARR[暂取2000]
 *          极性：
 *             1`TIM_OCPOLARITY_HIGH[高有效]:  高20% ->低80% 
 *          周期：ARR
 *          占空比：CCR值 / ARR值               
 *             2`TIM_OCPOLARITY_LOW[低有效]:   低80% ->高20%
 *          周期：ARR
 *          占空比：(ARR值 - CCR值) / ARR值        
 *       2.Pwm模式下，中心计数
 *          翻转条件：CNT<CCR=有效电平，CNT>=CCR=无效电平
 * 极限值：pulse = 0->CNT 恒等于 CCR = 0
 *          极性：
 *             1`TIM_OCPOLARITY_HIGH[高有效]:  全低电平
 *             2`TIM_OCPOLARITY_LOW[低有效]:   全高电平
 *          周期：无
 *          占空比：无
 * 极限值：pulse = ARR->
 *          极性：
 *             1`TIM_OCPOLARITY_HIGH[高有效]:  全高[中心计数没有CNT = CCR的情况->最大CNT = ARR/2]
 *             2`TIM_OCPOLARITY_LOW[低有效]:   全低[中心计数没有CNT = CCR的情况->最大CNT = ARR/2]
 *          周期：无
 *          占空比：无
 * 中间态：pulse = 0~ARR[暂取2000]
 *          极性：
 *             1`TIM_OCPOLARITY_HIGH[高有效]:  高20% ->低80% 
 *          周期：2*ARR
 *          占空比：CCR值 / ARR值               
 *             2`TIM_OCPOLARITY_LOW[低有效]:   低80% ->高20%
 *          周期：2*ARR
 *          占空比：(ARR值 - CCR值) / ARR值  
 *      
 * 初始电平->PA0 [PULLDOWN] -> 0V
 * 翻转电平->PA0 [PULLUP] -> 3.3V
 * @brief  定时器输出比较模式测试函数
 * @param  None
 * @retval None
 */

void TIM_OC_MODE_Test(void)
{
    // 初始化LED灯
    LED_Display_init();
    // 初始化串口
    uartInit(USART1,115200); 
    printf("TIM OC PWM MODE Test Start !!\r\n");
    // 初始化定时器输出比较模式为翻转模式，设置自动重装载寄存器值、预分频系数等参数
    TIM_OC_Pwm_Mode_Init(&g_TIM_OC_Base_Struct, &g_TIM_OC_Channel_Struct);


    while (1)
    {
#if TIM_OC_MODE_POLLING_ENABLE
        // 轮询输出比较
        if (__HAL_TIM_GET_FLAG(&g_TIM_OCPwm_Handle, TIM_FLAG_CC1))
        {
            
            __HAL_TIM_CLEAR_FLAG(&g_TIM_OCPwm_Handle, TIM_FLAG_CC1);

        }
#endif /* TIM_OC_MODE_POLLING_ENABLE */
    }
}

void TIM_OC_Pwm_Mode_Init(NEW_TIM_HandleTypeDef *pBaseStr, TIM_OC_InitTypeDef *pOCInitStr)
{
    g_TIM_OCPwm_Handle.Instance = pBaseStr->HAL_Handle.Instance; // 定时器实例
    g_TIM_OCPwm_Handle.Init.Period = pBaseStr->HAL_Handle.Init.Period; // 自动重载寄存器值
    g_TIM_OCPwm_Handle.Init.Prescaler = pBaseStr->HAL_Handle.Init.Prescaler; // 预分频系数
    g_TIM_OCPwm_Handle.Init.ClockDivision = pBaseStr->HAL_Handle.Init.ClockDivision; // 时钟分频系数
    g_TIM_OCPwm_Handle.Init.CounterMode = pBaseStr->HAL_Handle.Init.CounterMode; // 计数模式
    g_TIM_OCPwm_Handle.Init.RepetitionCounter = pBaseStr->HAL_Handle.Init.RepetitionCounter; // 重复计数值
    if(HAL_TIM_OC_Init(&g_TIM_OCPwm_Handle) != HAL_OK)
    {
        printf("TIM OC Init Error!\n");
        return;
    }
    g_sConfigOC_Handle.OCMode = pOCInitStr->OCMode; // 输出比较模式
    g_sConfigOC_Handle.Pulse = pOCInitStr->Pulse; // 输出比较寄存器值
    g_sConfigOC_Handle.OCPolarity = pOCInitStr->OCPolarity; // 输出极性
    if(HAL_TIM_OC_ConfigChannel(&g_TIM_OCPwm_Handle, &g_sConfigOC_Handle, pBaseStr->OCchannel) != HAL_OK)
    {
        printf("TIM OC Config Channel Error!\n");
        return;
    }

#if TIM_OC_MODE_POLLING_ENABLE
    // 启动定时器输出比较中断
    HAL_TIM_PWM_Start(&g_TIM_OCPwm_Handle, pBaseStr->OCchannel);
#endif /* TIM_OC_MODE_POLLING_ENABLE */

#if TIM_OC_MODE_IT_ENABLE
    // 启动定时器输出比较中断
    HAL_TIM_OC_Start_IT(&g_TIM_OCPwm_Handle, pBaseStr->OCchannel);
#endif /* TIM_OC_MODE_IT_ENABLE */

#if TIM_OC_MODE_DMA_ENABLE
    // 启动定时器输出比较DMA
    if(HAL_TIM_OC_Start_DMA(&g_TIM_OCPwm_Handle, pBaseStr->OCchannel, g_DMA_CCRBuffer, sizeof(g_DMA_CCRBuffer) / sizeof(g_DMA_CCRBuffer[0])) != HAL_OK)
    {
        printf("TIM OC Start DMA Error!\n");
        return;
    }
    __HAL_TIM_ENABLE_IT(&g_TIM_OCPwm_Handle, TIM_IT_CC1); // 使能输出比较通道1中断
#endif /* TIM_OC_MODE_DMA_ENABLE */

}


#if TIM_OC_MODE_DMA_ENABLE

static void DMA_OC_Pwm_Init(TIM_HandleTypeDef *htim)
{

    __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE(); // 使能DMA时钟

    g_hdma.Instance = TIM_OC_DMA_Stream; // 设置DMA流
    g_hdma.Init.Channel = TIM_OC_DMA_Channel; // 设置DMA通道
    g_hdma.Init.Direction = DMA_MEMORY_TO_PERIPH; // 设置传输方向
    g_hdma.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不递增
    g_hdma.Init.MemInc = DMA_MINC_ENABLE; // 内存地址递增
    g_hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // 外设数据对齐方式
    g_hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; // 内存数据对齐方式
#if TIM_DMA_NORMAL_ENABLE
    g_hdma.Init.Mode = DMA_NORMAL; // 设置DMA模式
#else
    g_hdma.Init.Mode = DMA_CIRCULAR; // 设置DMA为循环模式
#endif // TIM_DMA_NORMAL_ENABLE
    g_hdma.Init.Priority = DMA_PRIORITY_VERY_HIGH; // 设置DMA优先级
    /** @defgroup DMA_Handle_index TIM DMA Handle Index
     * @{
     */
    __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_CC1], g_hdma); // 关联DMA到定时器
    if(HAL_DMA_Init(&g_hdma) != HAL_OK)
    {
        printf("DMA IC Init Error !!!\r\n");
        return;
    }   
    // 设置DMA中断优先级
    {
        HAL_NVIC_SetPriority(TIM_OC_DMA_IRQn, TIM_DMA_PreemptPriority, TIM_DMA_SubPriority);
        HAL_NVIC_EnableIRQ(TIM_OC_DMA_IRQn);
    }
}


void TIM_OC_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_hdma); // 调用HAL库处理DMA中断
}



void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_OCPwm_Handle.Instance)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            // 输出比较事件回调函数
            printf("TIM DMA transfer finished !!! \r\n");
        }
    }
}


void TIMx_OC_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_OCPwm_Handle); // 调用HAL库处理函数
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_OCPwm_Handle.Instance)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1); // 清除输出比较通道1的标志位
            // 输出比较事件回调函数
            printf("TIM OC Event,CNT_Value = %d CCR_Value =  %d !!\r\n", 
                __HAL_TIM_GET_COUNTER(&g_TIM_OCPwm_Handle), 
                __HAL_TIM_GET_COMPARE(&g_TIM_OCPwm_Handle, TIM_CHANNEL_1));
        }
    }
}



#endif /* TIM_OC_MODE_DMA_ENABLE */

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance ==TIMx_OC_INSTANCE)
    {
        __TIMx_OC_INSTANCE_CLK_ENABLE(); // 启用定时器时钟
        GPIO_OC_CHx_Init(); // 初始化GPIO引脚
#if TIM_OC_MODE_IT_ENABLE
        // 启用定时器输出比较中断
        HAL_NVIC_SetPriority(TIMx_OC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_OC_INSTANCE_IRQn);         // 使能中断
#endif /* TIM_OC_MODE_IT_ENABLE */

#if TIM_OC_MODE_DMA_ENABLE
        // 初始化DMA
        DMA_OC_Pwm_Init(htim);
        // 启用定时器输出比较中断
        HAL_NVIC_SetPriority(TIMx_OC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_OC_INSTANCE_IRQn);         // 使能中断
#endif /* TIM_OC_MODE_DMA_ENABLE */


    }
}

#if TIM_OC_MODE_IT_ENABLE

void TIMx_OC_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_OCPwm_Handle); // 调用HAL库处理函数
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_OCPwm_Handle.Instance)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1); // 清除输出比较通道1的标志位
            // 输出比较事件回调函数
            printf("TIM OC Event,CNT_Value = %d CCR_Value =  %d !!\r\n", 
                __HAL_TIM_GET_COUNTER(&g_TIM_OCPwm_Handle), 
                __HAL_TIM_GET_COMPARE(&g_TIM_OCPwm_Handle, TIM_CHANNEL_1));
        }
    }
}

#endif /* TIM_OC_MODE_IT_ENABLE */





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
    GPIO_Init_Handle.Pull = GPIO_NOPULL;                        
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
    GPIO_InitStruct.Pull = GPIO_NOPULL; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);

}

#endif /* TIM_OC_Pwm_MODE_ENABLE */
