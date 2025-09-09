#include "TIM_ICandOC_Combine_OPM.h"
#include <stdio.h>




#if TIM_OC_OPM_MODE_ENABLE

uint8_t g_OPM_Flag = 0; // 翻转标志位
uint32_t g_OPMPolar_oldValue = 0; 
uint32_t g_OPMPolar_newValue = 0; 
// 低 100->高300-100 = 200 ->低 600-300 = 300 ->高 1000-600 = 400 ->低 1500-1000 = 500 ->高
//
uint32_t g_DMA_CCRBuffer[4] = {300 - 1,600 - 1,1000 - 1,1500 - 1}; // DMA CCR寄存器值缓冲区

TIM_HandleTypeDef g_TIM_Base_Handle = {0};
TIM_OnePulse_InitTypeDef g_TIM_OPM_Handle = {0};

TIM_HandleTypeDef g_TIM_OC_Base_Struct = {
    .Instance = TIMx_OC_INSTANCE, // 定义定时器实例
    .Init.Period = 10000 - 1, // 设置自动重载寄存器值
    .Init.Prescaler = 8400 - 1, // 设置预分频系数
    .Init.ClockDivision = TIM_CLOCKDIVISION_DIV1, // 设置时钟分频系数
    .Init.CounterMode = TIM_COUNTERMODE_UP, // 设置计数模式
    .Init.RepetitionCounter = 0, // 设置重复计数值
    .Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE, // 设置自动重载预装载

};

new_TIM_OnePulse_InitTypeDef g_TIM_OPM_Channel_Structs = {
    .OnePulseMode = TIM_OPMODE_SINGLE,                              // 设置单脉冲模式
    //输出比较部分
    .TIM_OnePulse_Handle.OCMode = TIM_OCMODE_PWM1,                  // 设置输出比较模式为PWM1
    .TIM_OnePulse_Handle.Pulse = 4000 - 1,                          // 设置脉冲值
    .TIM_OnePulse_Handle.OCPolarity = TIM_OCPOLARITY_LOW,          // 设置输出极性为高
    .OCchannel = TIM_CHANNEL_1,                                     // 设置输出比较通道
    //输入捕获部分
    .TIM_OnePulse_Handle.ICPolarity = TIM_ICPOLARITY_RISING,        // 设置输入捕获极性为上升沿
#if TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE
    .TIM_OnePulse_Handle.ICSelection = TIM_ICSELECTION_DIRECTTI,    // 设置输入捕获选择为直接输入
#else
    .TIM_OnePulse_Handle.ICSelection = TIM_ICSELECTION_INDIRECTTI,  // 设置输入捕获选择为间接输入
#endif /* TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE */
    .TIM_OnePulse_Handle.ICFilter = 0x03,                           // 设置输入捕获滤波器    
    .ICchannel = TIM_CHANNEL_2,                                     // 设置输入捕获通道
};

/**
 * @note :1.OPM模式下，计数UP/DOWN输出比较通道1的输出状态在每次CCR = TIMx_CNT时翻转一次。
 *          周期：2*ARR值
 *          占空比：CCR值 / ARR值 = 50% (固定)
 * 极限值：pulse = 0->
 *          周期：2*ARR值
 *          占空比：50% (固定)
 * 极限值：pulse = ARR->
 *          周期：2*ARR值
 *          占空比：50% (固定)
 *        2.OPM模式下，计数Center[极限情况2ARR~4ARR]
 *          周期：2*ARR值
 *          占空比：（ARR值 - CCR值）/ ARR值
 * 极限值：pulse = 0->
 *          周期：4*ARR值
 *          占空比：50% (固定)
 * 极限值：pulse = ARR->
 *          周期：4*ARR值
 *          占空比：50% (固定)
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
    printf("TIM OC MODE Test Start !!\r\n");
    // 初始化定时器输出比较模式为翻转模式，设置自动重装载寄存器值、预分频系数等参数
    TIM_OC_OPM_Mode_Init(&g_TIM_OC_Base_Struct, &g_TIM_OPM_Channel_Structs);


    while (1)
    {
#if TIM_OC_MODE_POLLING_ENABLE
        if(__HAL_TIM_GET_FLAG(&g_TIM_Base_Handle, TIM_FLAG_CC1)) // 检查输出比较通道1标志位
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_Base_Handle, TIM_FLAG_CC1); // 清除标志位
            printf("TIM OPM Mode Output Compare Channel 1 Interrupt Triggered !!\r\n");
        }
        if(__HAL_TIM_GET_FLAG(&g_TIM_Base_Handle, TIM_FLAG_CC2)) // 检查输出比较通道2标志位
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_Base_Handle, TIM_FLAG_CC2); // 清除标志位
            printf("TIM OPM Mode Input Capture Channel 2 Interrupt Triggered !!\r\n");
        }
#endif /* TIM_OC_MODE_POLLING_ENABLE */
    }
}

void TIM_OC_OPM_Mode_Init(TIM_HandleTypeDef *pBaseStr, new_TIM_OnePulse_InitTypeDef *pOPMInitStr)
{
    g_TIM_Base_Handle.Instance = pBaseStr->Instance; // 设置定时器实例
    g_TIM_Base_Handle.Init.Period = pBaseStr->Init.Period; // 设置自动重载寄存器值
    g_TIM_Base_Handle.Init.Prescaler = pBaseStr->Init.Prescaler; // 设置预分频系数
    g_TIM_Base_Handle.Init.ClockDivision = pBaseStr->Init.ClockDivision; // 设置时钟分频系数
    g_TIM_Base_Handle.Init.CounterMode = pBaseStr->Init.CounterMode; // 设置计数模式
    g_TIM_Base_Handle.Init.RepetitionCounter = pBaseStr->Init.RepetitionCounter; // 设置重复计数值
    g_TIM_Base_Handle.Init.AutoReloadPreload = pBaseStr->Init.AutoReloadPreload; // 设置自动重载预装载
    if(HAL_TIM_OnePulse_Init(&g_TIM_Base_Handle,pOPMInitStr->OnePulseMode) != HAL_OK)
    {
        printf("TIM OPM Init Error !!!\r\n");
        return;
    }
    //输出部分
    g_TIM_OPM_Handle.OCMode = pOPMInitStr->TIM_OnePulse_Handle.OCMode; // 设置输出比较模式
    g_TIM_OPM_Handle.Pulse = pOPMInitStr->TIM_OnePulse_Handle.Pulse; // 设置脉冲值
    g_TIM_OPM_Handle.OCPolarity = pOPMInitStr->TIM_OnePulse_Handle.OCPolarity; // 设置输出极性
    //输入部分
    g_TIM_OPM_Handle.ICPolarity = pOPMInitStr->TIM_OnePulse_Handle.ICPolarity; // 设置输入捕获极性
    g_TIM_OPM_Handle.ICSelection = pOPMInitStr->TIM_OnePulse_Handle.ICSelection; // 设置输入捕获选择
    g_TIM_OPM_Handle.ICFilter = pOPMInitStr->TIM_OnePulse_Handle.ICFilter; // 设置输入捕获滤波器
    if(HAL_TIM_OnePulse_ConfigChannel(&g_TIM_Base_Handle, &g_TIM_OPM_Handle, pOPMInitStr->OCchannel,pOPMInitStr->ICchannel) != HAL_OK)
    {
        printf("TIM OPM Config Channel Error !!!\r\n");
        return;
    }

#if TIM_OC_MODE_POLLING_ENABLE
    // 启动定时器输出比较中断
    HAL_TIM_OnePulse_Start(&g_TIM_Base_Handle, pOPMInitStr->OCchannel);
#endif /* TIM_OC_MODE_POLLING_ENABLE */

#if TIM_OC_MODE_IT_ENABLE
    // 启动定时器输出比较中断
    HAL_TIM_OC_Start_IT(&g_TIM_OCOPM_Handle, pBaseStr->OCchannel);
#endif /* TIM_OC_MODE_IT_ENABLE */

#if TIM_OC_MODE_DMA_ENABLE
    // 启动定时器输出比较DMA
    if(HAL_TIM_OC_Start_DMA(&g_TIM_OCOPM_Handle, pBaseStr->OCchannel, g_DMA_CCRBuffer, sizeof(g_DMA_CCRBuffer) / sizeof(g_DMA_CCRBuffer[0])) != HAL_OK)
    {
        printf("TIM OC Start DMA Error!\n");
        return;
    }
    __HAL_TIM_ENABLE_IT(&g_TIM_OCOPM_Handle, TIM_IT_CC1); // 使能输出比较通道1中断
#endif /* TIM_OC_MODE_DMA_ENABLE */

}


#if TIM_OC_MODE_DMA_ENABLE

static void DMA_OC_OPM_Init(TIM_HandleTypeDef *htim)
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
    if(htim->Instance == g_TIM_OCOPM_Handle.Instance)
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
    HAL_TIM_IRQHandler(&g_TIM_OCOPM_Handle); // 调用HAL库处理函数
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_OCOPM_Handle.Instance)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1); // 清除输出比较通道1的标志位
            // 输出比较事件回调函数
            printf("TIM OC Event,CNT_Value = %d CCR_Value =  %d !!\r\n", 
                __HAL_TIM_GET_COUNTER(&g_TIM_OCOPM_Handle), 
                __HAL_TIM_GET_COMPARE(&g_TIM_OCOPM_Handle, TIM_CHANNEL_1));
        }
    }
}



#endif /* TIM_OC_MODE_DMA_ENABLE */

void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance ==TIMx_OC_INSTANCE)
    {
        __TIMx_OC_INSTANCE_CLK_ENABLE(); // 启用定时器时钟
        GPIO_OC_CHx_Init(); // 初始化GPIO引脚
        GPIO_IC_CHx_Init(); // 初始化GPIO引脚
#if TIM_OC_MODE_IT_ENABLE
        // 启用定时器输出比较中断
        HAL_NVIC_SetPriority(TIMx_OC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_OC_INSTANCE_IRQn);         // 使能中断
#endif /* TIM_OC_MODE_IT_ENABLE */

#if TIM_OC_MODE_DMA_ENABLE
        // 初始化DMA
        DMA_OC_OPM_Init(htim);
        // 启用定时器输出比较中断
        HAL_NVIC_SetPriority(TIMx_OC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_OC_INSTANCE_IRQn);         // 使能中断
#endif /* TIM_OC_MODE_DMA_ENABLE */


    }
}

#if TIM_OC_MODE_IT_ENABLE

void TIMx_OC_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_OCOPM_Handle); // 调用HAL库处理函数
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_OCOPM_Handle.Instance)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1); // 清除输出比较通道1的标志位
            // 输出比较事件回调函数
            printf("TIM OC Event,CNT_Value = %d CCR_Value =  %d !!\r\n", 
                __HAL_TIM_GET_COUNTER(&g_TIM_OCOPM_Handle), 
                __HAL_TIM_GET_COMPARE(&g_TIM_OCOPM_Handle, TIM_CHANNEL_1));
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
    GPIO_Init_Handle.Pull = GPIO_PULLDOWN;                       // 引脚下拉
    GPIO_Init_Handle.Speed = TIMx_OC_CHx_Speed;                 // 设置引脚速度
    GPIO_Init_Handle.Alternate = TIMx_OC_CHx_AF_FOR_TIM;        // 设置引脚复用功能
    
    HAL_GPIO_Init(TIMx_OC_CHx_GPIO_Port, &GPIO_Init_Handle);    // 初始化GPIO
}

/**
 * @brief  GPIO初始化
 * @param  None
 * @retval None
 */
static void GPIO_IC_CHx_Init(void)
{
    GPIO_InitTypeDef GPIO_Init_Handle = {0};

    __HAL_RCC_GPIO_IC_CHx_CLK_ENABLE();                         // 启用GPIO时钟
    GPIO_Init_Handle.Pin = TIMx_IC_CHx_Pin;                     // 定义输入捕获通道引脚
    GPIO_Init_Handle.Mode = TIMx_IC_CHx_Mode;                   // 设置引脚模式输出
    GPIO_Init_Handle.Pull = TIMx_IC_CHx_Pull;                       // 引脚下拉
    GPIO_Init_Handle.Speed = TIMx_IC_CHx_Speed;                 // 设置引脚速度
    GPIO_Init_Handle.Alternate = TIMx_IC_CHx_AF_FOR_TIM;        // 设置引脚复用功能

    HAL_GPIO_Init(TIMx_IC_CHx_GPIO_Port, &GPIO_Init_Handle);    // 初始化GPIO
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

#endif /* TIM_OC_OPM_MODE_ENABLE */
