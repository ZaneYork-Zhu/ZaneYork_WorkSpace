#include "TIM_OC_Force_Mode.h"
#include <stdio.h>

TIM_HandleTypeDef g_TIM_OC_Force_Handle = {0};
TIM_OC_InitTypeDef g_sConfigOC_Handle = {0};

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


/**
 * @brief  【不影响产生匹配标志位】
 * @note 1:TIM_OCMODE_FORCED_ACTIVE模式->"强制有效模式"[与CCR无关->强制输出有效电平]
 *          1`TIM_OCPOLARITY_HIGH[有效高电平]
 *              全高
 *          2`TIM_OCPOLARITY_LOW[有效低电平] ->
 *              全低
 *       2:TIM_OCMODE_FORCED_INACTIVE模式->"强制无效模式"[与CCR无关->强制输出无效电平]
 *          1`TIM_OCPOLARITY_HIGH[有效高电平] ->
 *              全低
 *          2`TIM_OCPOLARITY_LOW[有效低电平] ->
 *              全高
 */

TIM_OC_InitTypeDef g_TIM_OC_Channel_Struct = {
#if TIM_OC_FORCE_MODE_ENABLE
    .OCMode = TIM_OCMODE_FORCED_ACTIVE,                        // 输出比较模式：强制有效模式
#else
    .OCMode = TIM_OCMODE_FORCED_INACTIVE,                      // 输出比较模式：强制无效模式
#endif /* TIM_OC_ACTIVE_MODE_ENABLE */
    .Pulse = 5000 - 1,                                         
    //如果TIM_OCPOLARITY_LOW->0~pulse会产生一个[短暂]高电平脉冲
    .OCPolarity = TIM_OCPOLARITY_HIGH,                      // 输出极性：高电平有效
};

void TIM_OC_MODE_Test(void)
{
    // 初始化LED灯
    LED_Display_init();
    // 初始化串口
    uartInit(USART1,115200); 
    printf("TIM OC MODE Test Start !!\r\n");
    // 初始化定时器输出比较模式：TIM_OCMODE_ACTIVE
    TIM_OC_Force_Mode_Init(&g_TIM_OC_Base_Struct, &g_TIM_OC_Channel_Struct);


    while (1)
    {
#if TIM_OC_MODE_POLLING_ENABLE
        // 轮询输出比较
        if (__HAL_TIM_GET_FLAG(&g_TIM_OC_Force_Handle, TIM_FLAG_CC1))
        {
            __HAL_TIM_CLEAR_FLAG(&g_TIM_OC_Force_Handle, TIM_FLAG_CC1); // 清除输出比较通道1标志位
            printf("TIM OC Event,CNT_Value = %d CCR_Value =  %d !!\r\n", 
                __HAL_TIM_GET_COUNTER(&g_TIM_OC_Force_Handle), 
                __HAL_TIM_GET_COMPARE(&g_TIM_OC_Force_Handle, TIM_CHANNEL_1));
        }
#endif /* TIM_OC_MODE_POLLING_ENABLE */
    }
}


void TIM_OC_Force_Mode_Init(NEW_TIM_HandleTypeDef *pBaseStr, TIM_OC_InitTypeDef *pOCInitStr)
{
    g_TIM_OC_Force_Handle.Instance = pBaseStr->HAL_Handle.Instance; // 定时器实例
    g_TIM_OC_Force_Handle.Init.Period = pBaseStr->HAL_Handle.Init.Period; // 自动重载寄存器值
    g_TIM_OC_Force_Handle.Init.Prescaler = pBaseStr->HAL_Handle.Init.Prescaler; // 预分频系数
    g_TIM_OC_Force_Handle.Init.ClockDivision = pBaseStr->HAL_Handle.Init.ClockDivision; // 时钟分频系数
    g_TIM_OC_Force_Handle.Init.CounterMode = pBaseStr->HAL_Handle.Init.CounterMode; // 计数模式
    g_TIM_OC_Force_Handle.Init.RepetitionCounter = pBaseStr->HAL_Handle.Init.RepetitionCounter; // 重复计数值
    if(HAL_TIM_OC_Init(&g_TIM_OC_Force_Handle) != HAL_OK)
    {        
        printf("TIM OC Init Error!\n");
        return;
    }
    g_sConfigOC_Handle.OCMode = pOCInitStr->OCMode; // 输出比较模式
    g_sConfigOC_Handle.Pulse = pOCInitStr->Pulse; // 输出比较寄存器值
    g_sConfigOC_Handle.OCPolarity = pOCInitStr->OCPolarity; // 输出极性：高电平有效
    if(HAL_TIM_OC_ConfigChannel(&g_TIM_OC_Force_Handle, &g_sConfigOC_Handle, pBaseStr->OCchannel) != HAL_OK)
    {        
        printf("TIM OC Config Channel Error!\n");
        return;
    }

#if TIM_OC_MODE_POLLING_ENABLE
    // 启动定时器输出比较轮询
    HAL_TIM_OC_Start(&g_TIM_OC_Force_Handle, pBaseStr->OCchannel);
#endif /* TIM_OC_MODE_POLLING_ENABLE */

#if TIM_OC_MODE_IT_ENABLE
    // 启动定时器输出比较中断
    HAL_TIM_OC_Start_IT(&g_TIM_OC_Force_Handle, pBaseStr->OCchannel);
#endif /* TIM_OC_MODE_IT_ENABLE */

#if TIM_OC_MODE_DMA_ENABLE
    // 启动定时器输出比较DMA
    if(HAL_TIM_OC_Start_DMA(&g_TIM_OC_Force_Handle, pBaseStr->OCchannel, g_DMA_CCRBuffer, sizeof(g_DMA_CCRBuffer) / sizeof(g_DMA_CCRBuffer[0])) != HAL_OK)
    {
        printf("TIM OC Start DMA Error!\n");
        return;
    }
    __HAL_TIM_ENABLE_IT(&g_TIM_OC_Force_Handle, TIM_IT_CC1); // 使能输出比较通道1中断
#endif /* TIM_OC_MODE_DMA_ENABLE */

}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIMx_OC_INSTANCE)
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
        DMA_OC_CHx_Init(htim);
        // 启用定时器输出比较中断
        HAL_NVIC_SetPriority(TIMx_OC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_OC_INSTANCE_IRQn);         // 使能中断
#endif /* TIM_OC_MODE_DMA_ENABLE */
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
    GPIO_Init_Handle.Pull = TIMx_OC_CHx_Pull;                       // 引脚下拉
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
