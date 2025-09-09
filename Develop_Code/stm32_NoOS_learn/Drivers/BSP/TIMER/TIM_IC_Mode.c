#include "TIM_IC_Mode.h"
#include <stdio.h>
#include "led.h"


TIM_HandleTypeDef g_TIM_HandleTypeStruct = {0};
TIM_IC_InitTypeDef sConfigIC = {0};
DMA_HandleTypeDef g_hdma = {0};
static uint32_t icOldValue = 0,icNewValue = 0,icGapValue = 0; // 捕获值
static uint32_t icNumCycle = 0; // 输入捕获计数
static uint32_t icEdgeFlag = 1; // 边沿标志位
static uint8_t dmaFlag = 0; // DMA半传输标志位
uint32_t pDMAbuffer[2] = {0}; // DMA缓冲区



#if TIM_SINGLE_MODE_ENABLE && !TIM_COMBINE_MODE_ENABLE
TIM_HandleTypeDef g_TIM_Base_Handle = {
    .Instance = TIMx_IC_INSTANCE,
    .Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE, // 自动重载预装载使能
    .Init.ClockDivision = TIM_CLOCKDIVISION_DIV1, // 时钟分频
    .Init.CounterMode = TIM_COUNTERMODE_UP, // 计数模式
    .Init.Period =  0x10000 - 1, // 自动重载寄存器值
    .Init.Prescaler =  8400 - 1, // 预分频值
    .Init.RepetitionCounter = 0, // 重复计数器值
};

NEW_TIM_HandleTypeDef g_NewHandleStruct = {
    .HAL_Handle = &g_TIM_Base_Handle,
#if TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE
    .ICchannel = TIM_CHANNEL_1,   // 通道
#else
    .ICchannel = TIM_CHANNEL_2,   // 通道
#endif  /* TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE */

};

TIM_IC_InitTypeDef g_TIM_IC_Handle = {
    .ICFilter = 0x03, // 输入滤波器值：0x03
    .ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING, // 输入极性：上升沿捕获
#if TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE
    .ICSelection = TIM_ICSELECTION_DIRECTTI, // 输入选择：CH1->TI1
#else
    .ICSelection = TIM_ICSELECTION_INDIRECTTI, // 输入选择：CH1->TI2
#endif // TIM_CHNNEL_FOR_DIRECT_MODE_ENABLE
    .ICPrescaler = TIM_ICPSC_DIV1, // 输入预分频值：不分频
};


/**
  * @brief  TIM IC MODE 测试
  * @param  None
  * @retval None
  */
void TIM_IC_MODE_Test(void)
{
    LED_init();
    uartInit(USART1, 115200); // 初始化串口1,波特率115200
    printf("TIM IC Mode Test Start !!!\r\n");

    // 初始化定时器输入捕获模式
    TIM_IC_MODE_Init(&g_NewHandleStruct, &g_TIM_IC_Handle);

    while (1)
    {
#if TIM_IC_MODE_POLLING_ENABLE
        // 轮询方式处理输入捕获事件
        if(__HAL_TIM_GET_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC1))
        {
            static uint32_t ic_num = 0;
            __HAL_TIM_CLEAR_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC1); // 清除输入捕获标志位
            uint32_t captureValue = HAL_TIM_ReadCapturedValue(&g_TIM_HandleTypeStruct, TIM_CHANNEL_1); // 读取捕获值
            printf("TIM IC Capture Value: %lu, Capture Count: %lu\r\n", captureValue, ic_num);
            LED1_TOGGLE(); // 翻转LED1状态
            ic_num++;
        }else{
            LED0_TOGGLE(); // 翻转LED0状态
            HAL_Delay(250); // 延时250ms
        }
#endif // TIM_IC_MODE_POLLING_ENABLE
#if TIM_IC_MODE_IT_ENABLE || TIM_IC_MODE_DMA_ENABLE
        LED0(1); // 翻转LED0状态
        HAL_Delay(250); // 延时250ms
        LED0(0); // 关闭LED0
        HAL_Delay(250); // 延时250ms
#endif // TIM_IC_MODE_IT_ENABLE
    }
}

void TIM_IC_MODE_Init(NEW_TIM_HandleTypeDef *devStruct, TIM_IC_InitTypeDef *ic_device)
{ 
    if(devStruct == NULL || ic_device == NULL)
    {
        printf("TIM IC Init Error !!!\r\n");
        return;
    }
    /* 定时器初始化 */
    g_TIM_HandleTypeStruct.Instance = devStruct->HAL_Handle->Instance; // 选择TIMx作为输入捕获定时器
    g_TIM_HandleTypeStruct.Init.Period =  devStruct->HAL_Handle->Init.Period; // 设置自动重载寄存器值
    g_TIM_HandleTypeStruct.Init.Prescaler =  devStruct->HAL_Handle->Init.Prescaler; // 设置预分频值
    g_TIM_HandleTypeStruct.Init.AutoReloadPreload =  devStruct->HAL_Handle->Init.AutoReloadPreload; // 自动重载预装载使能
    g_TIM_HandleTypeStruct.Init.RepetitionCounter =  devStruct->HAL_Handle->Init.RepetitionCounter; // 设置重复计数器
    g_TIM_HandleTypeStruct.Init.CounterMode =  devStruct->HAL_Handle->Init.CounterMode; // 设置计数模式
    g_TIM_HandleTypeStruct.Init.ClockDivision =  devStruct->HAL_Handle->Init.ClockDivision; // 设置时钟分频

    if(HAL_TIM_IC_Init(&g_TIM_HandleTypeStruct) != HAL_OK)
    {
        printf("TIM IC Init Error !!!\r\n");
        return;
    }

    /* 输入捕获初始化 */
    sConfigIC.ICPolarity = ic_device->ICPolarity; // 设置输入捕获极性
    sConfigIC.ICSelection = ic_device->ICSelection; // 设置输入捕获选择
    sConfigIC.ICPrescaler = ic_device->ICPrescaler; // 设置输入捕获预分频
    sConfigIC.ICFilter = ic_device->ICFilter; // 设置输入捕获滤波器
    // CH1->TI1直接连接
    if(HAL_TIM_IC_ConfigChannel(&g_TIM_HandleTypeStruct, &sConfigIC,devStruct->ICchannel) != HAL_OK)
    {
        printf("TIM IC Config Channel Error !!!\r\n");
        return;
    }

#if TIM_IC_MODE_POLLING_ENABLE
    /* 轮询模式 */
    if(HAL_TIM_IC_Start(&g_TIM_HandleTypeStruct,  devStruct->ICchannel) != HAL_OK)
    {
        printf("TIM IC Start Polling Error !!!\r\n");
        return;
    }    
#endif // TIM_IC_MODE_POLLING_ENABLE

#if TIM_IC_MODE_IT_ENABLE
    /* 中断模式 */
    if(HAL_TIM_IC_Start_IT(&g_TIM_HandleTypeStruct,devStruct->ICchannel) != HAL_OK)
    {
        printf("TIM IC Start Interrupt Error !!!\r\n");
        return;
    }
#endif // TIM_IC_MODE_INTERRUPT_ENABLE

#if TIM_IC_MODE_DMA_ENABLE
    /* DMA模式 */
    if(HAL_TIM_IC_Start_DMA(&g_TIM_HandleTypeStruct,TIM_CHANNEL_1, pDMAbuffer, sizeof(pDMAbuffer)/sizeof(pDMAbuffer[0])) != HAL_OK)
    {
        printf("TIM IC Start DMA Error !!!\r\n");
        return;
    }
#endif // TIM_IC_MODE_DMA_ENABLE

}

#if TIM_IC_MODE_DMA_ENABLE
static void DMA_IC_CHx_Init(TIM_HandleTypeDef *htim)
{

    
    __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE(); // 使能DMA时钟

    g_hdma.Instance = TIM_IC_DMA_Stream; // 设置DMA流
    g_hdma.Init.Channel = TIM_IC_DMA_Channel; // 设置DMA通道
    g_hdma.Init.Direction = DMA_PERIPH_TO_MEMORY; // 设置传输方向
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
        HAL_NVIC_SetPriority(TIM_IC_DMA_IRQn, TIM_DMA_PreemptPriority, TIM_DMA_SubPriority);
        HAL_NVIC_EnableIRQ(TIM_IC_DMA_IRQn);
    }
}
#endif // TIM_IC_MODE_DMA_ENABLE



static uint32_t TIM_setEdge(NEW_TIM_HandleTypeDef devStruct , uint32_t edge)
{
    uint32_t retVal = HAL_TIM_ReadCapturedValue(devStruct.HAL_Handle, devStruct.ICchannel); // 读取当前捕获值
    TIM_RESET_CAPTUREPOLARITY(devStruct.HAL_Handle, devStruct.ICchannel); // 重置输入捕获极性
    TIM_SET_CAPTUREPOLARITY(devStruct.HAL_Handle, devStruct.ICchannel,edge); // 设置为指定边沿捕获
    return retVal; // 返回捕获值
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE)
    {
        __TIMx_IC_INSTANCE_CLK_ENABLE(); // 使能TIMx时钟
        GPIO_IC_CHx_Init(); // 配置输入捕获引脚
#if TIM_IC_MODE_IT_ENABLE
        // 配置定时器中断
        HAL_NVIC_SetPriority(TIMx_IC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_IC_INSTANCE_IRQn); // 使能定时器中断
#endif // TIM_IC_MODE_IT_ENABLE
#if TIM_IC_MODE_DMA_ENABLE
        DMA_IC_CHx_Init(htim); // 配置DMA
        // 配置定时器中断
        HAL_NVIC_SetPriority(TIMx_IC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_IC_INSTANCE_IRQn); //
#endif // TIM_IC_MODE_DMA_ENABLE
    }
}

#if TIM_IC_MODE_DMA_ENABLE


#if TIM_DMA_NORMAL_ENABLE
void TIM_IC_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_hdma); // 处理DMA中断
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE) // 判断是否是定时器实例
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            if(dmaFlag == 0) // 判断是否为半传输
            {
                dmaFlag = 1; // 设置为1，表示已处理半传输
                icOldValue = pDMAbuffer[0]; // 读取CH1捕获值
                printf("CH1: %d\r\n", icOldValue);
                HAL_TIM_IC_Start_DMA(htim, TIM_CHANNEL_1, (uint32_t *)pDMAbuffer[1], 1); // 重新启动DMA
            }else{
                __HAL_TIM_SET_COUNTER(htim, 0); // 清除计数器
                dmaFlag = 0; // 设置为0，表示已处理全传输
                icNewValue = pDMAbuffer[1]; // 读取CH1捕获值
                printf("CH2: %d\r\n", icNewValue);
                icGapValue = icNewValue - icOldValue; // 计算捕获间隔
                icNumCycle++;
                printf("TIM IC Capture Value: %lu, Capture Count: %lu\r\n", icGapValue, icNumCycle);
            }
        }
    }
}
#else

void TIM_IC_DMA_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_hdma); // 处理DMA中断
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE) // 判断是否是定时器实例
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            icNewValue = pDMAbuffer[1]; // 读取CH1捕获值
            __HAL_TIM_SET_COUNTER(htim, 0); // 清除计数器
            icGapValue = icNewValue - icOldValue; // 计算捕获间隔
            icNumCycle++;
            printf("TIM IC Capture Value: %lu, Capture Count: %lu\r\n", icGapValue, icNumCycle);
        }
    }
}

void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE) // 判断是否是定时器实例
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            icOldValue = pDMAbuffer[0]; // 读取CH1捕获值
        }
    }
}

#endif // TIM_IC_MODE_DMA_NORMAL_ENABLE

#endif // TIM_IC_MODE_DMA_ENABLE


#if TIM_IC_MODE_IT_ENABLE

void TIMx_IC_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_HandleTypeStruct);
#if TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE
    __HAL_TIM_CLEAR_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC1); // 清除输入捕获标志位
#else
    __HAL_TIM_CLEAR_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC2); // 清除输入捕获标志位  
#endif // TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE
    
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE)
    {
#if TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            if(icEdgeFlag){
                icEdgeFlag = 0; // 清除上升沿标志位
                icOldValue = TIM_setEdge(g_NewHandleStruct,TIM_ICPOLARITY_FALLING); // 设置为下降沿捕获
            }else{
                icEdgeFlag = 1; // 清除下降沿标志位
                icNewValue = TIM_setEdge(g_NewHandleStruct,TIM_ICPOLARITY_RISING); // 设置为上升沿捕获
                __HAL_TIM_SET_COUNTER(htim, 0); // 清除计数器
                icGapValue = icNewValue - icOldValue;
                icNumCycle++;
                printf("TIM IC Capture Value: %lu, Capture Count: %lu\r\n", icGapValue, icNumCycle);
            }
        }
#else
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            if(icEdgeFlag){
                icEdgeFlag = 0; // 清除上升沿标志位
                icOldValue = TIM_setEdge(g_NewHandleStruct,TIM_ICPOLARITY_FALLING); // 设置为下降沿捕获
            }else{
                icEdgeFlag = 1; // 清除下降沿标志位
                icNewValue = TIM_setEdge(g_NewHandleStruct,TIM_ICPOLARITY_RISING); // 设置为上升沿捕获
                icGapValue = icNewValue - icOldValue;
                icNumCycle++;
                printf("TIM IC Capture Value: %lu, Capture Count: %lu\r\n", icGapValue, icNumCycle);
            }
        }
#endif // TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE

    }
}


#endif // TIM_IC_MODE_IT_ENABLE


#endif // TIM_SINGLE_MODE_ENABLE

#if TIM_COMBINE_MODE_ENABLE && !TIM_SINGLE_MODE_ENABLE
/**
  * @brief  TIM IC MODE 测试
  * @param  None
  * @retval None
  */
void TIM_IC_MODE_Test(void)
{
    
    LED_init();
    uartInit(USART1, 115200); // 初始化串口1,波特率115200
    printf("TIM IC Mode Test Start !!!\r\n");

    // 初始化定时器输入捕获模式
    TIM_IC_combine_Mode_Init();
    while (1)
    {
#if TIM_IC_MODE_POLLING_ENABLE  
        if(__HAL_TIM_GET_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC1))
        {
            icOldValue = HAL_TIM_ReadCapturedValue(&g_TIM_HandleTypeStruct, TIM_CHANNEL_1); // 读取捕获值
            printf("TIM2_CC1 = %d\r\n", icOldValue);
            __HAL_TIM_CLEAR_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC1);
        }else if(__HAL_TIM_GET_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC2))
        {
            icNewValue = HAL_TIM_ReadCapturedValue(&g_TIM_HandleTypeStruct, TIM_CHANNEL_2); // 读取捕获值
            printf("TIM2_CC2: %d\r\n", icNewValue);
            __HAL_TIM_CLEAR_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC2);
            __HAL_TIM_SET_COUNTER(&g_TIM_HandleTypeStruct, 0); // 清除计数器
            icGapValue = icNewValue - icOldValue; // 计算捕获间隔
            printf("captureValue = %d\r\n", icGapValue);
        }
        // else{   
        //     LED0(1); // 翻转LED0状态
        //     HAL_Delay(500);
        //     LED0(0); // 关闭LED0
        //     HAL_Delay(500);
        // }
#endif

#if TIM_IC_MODE_IT_ENABLE
        LED0(1); // 翻转LED0状态
        HAL_Delay(500);
        LED0(0); // 关闭LED0
        HAL_Delay(500);
#endif // TIM_IC_MODE_IT_ENABLE
    }
}

void TIM_IC_combine_Mode_Init(void)
{
    /* TIM base*/
    g_TIM_HandleTypeStruct.Instance = TIMx_IC_INSTANCE; // 选择TIMx作为输入捕获定时器
    g_TIM_HandleTypeStruct.Init.Period =  0x10000 - 1; // 设置自动重载寄存器值
    g_TIM_HandleTypeStruct.Init.Prescaler =  8400 - 1; // 设置预分频值
    g_TIM_HandleTypeStruct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 自动重载预装载使能
    g_TIM_HandleTypeStruct.Init.RepetitionCounter = 0; // 设置重复计数器
    g_TIM_HandleTypeStruct.Init.CounterMode = TIM_COUNTERMODE_UP; // 设置计数模式
    g_TIM_HandleTypeStruct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 设置时钟分频
    if(HAL_TIM_IC_Init(&g_TIM_HandleTypeStruct) != HAL_OK)
    {
        printf("TIM IC Init Error !!!\r\n");
        return;
    }
    /* TIM CH1 配置 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; // 设置输入捕获极性为上升沿
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; // 设置输入选择为CH1->TI1
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0x03;
    if (HAL_TIM_IC_ConfigChannel(&g_TIM_HandleTypeStruct, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("TIM IC Config Channel Error !!!\r\n");
        return;
    }
    /* TIM CH2 配置 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING; // 设置输入捕获极性为下降沿
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI; // 设置输入选择为CH2->TI1
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0x03;
    if (HAL_TIM_IC_ConfigChannel(&g_TIM_HandleTypeStruct, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
    {
        printf("TIM IC Config Channel Error !!!\r\n");
        return;
    }
#if TIM_IC_MODE_POLLING_ENABLE
    /* 轮询模式 */
    if(HAL_TIM_IC_Start(&g_TIM_HandleTypeStruct,TIM_CHANNEL_1) != HAL_OK)
    {
        printf("TIM IC Start Polling Error !!!\r\n");
        return;
    }    
    if(HAL_TIM_IC_Start(&g_TIM_HandleTypeStruct,TIM_CHANNEL_2) != HAL_OK)
    {
        printf("TIM IC Start Polling Error !!!\r\n");
        return;
    }
#endif // TIM_IC_MODE_POLLING_ENABLE

#if TIM_IC_MODE_IT_ENABLE
    /* 中断模式 */
    if(HAL_TIM_IC_Start_IT(&g_TIM_HandleTypeStruct,TIM_CHANNEL_1) != HAL_OK)
    {
        printf("TIM IC Start Interrupt Error !!!\r\n");
        return;
    }
    if(HAL_TIM_IC_Start_IT(&g_TIM_HandleTypeStruct,TIM_CHANNEL_2) != HAL_OK)
    {
        printf("TIM IC Start Interrupt Error !!!\r\n");
        return;
    }
#endif // TIM_IC_MODE_INTERRUPT_ENABLE
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE)
    {
        __TIMx_IC_INSTANCE_CLK_ENABLE(); // 使能TIMx时钟
        GPIO_IC_CHx_Init(); // 配置输入捕获引脚
#if TIM_IC_MODE_IT_ENABLE
        // 配置定时器中断
        HAL_NVIC_SetPriority(TIMx_IC_INSTANCE_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority); // 设置中断优先级
        HAL_NVIC_EnableIRQ(TIMx_IC_INSTANCE_IRQn); // 使能定时器中断 
#endif // TIM_IC_MODE_IT_ENABLE
    }
}

void TIMx_IC_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_HandleTypeStruct);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{   

    if(htim->Instance == TIMx_IC_INSTANCE)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            icOldValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 读取CH1捕获值
            __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1); // 清除输入捕获标志位
            printf("TIM2_CC1 =  %d\r\n", icOldValue);
        }else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            icNewValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // 读取CH2捕获值
            __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC2); // 清除输入捕获标志位
            __HAL_TIM_SET_COUNTER(htim, 0); // 清除计数器
            printf("TIM2_CC2 =  %d\r\n", icNewValue);
            printf("TIM2_CC1 =  %d\r\n", icOldValue);
            icGapValue = icNewValue - icOldValue;
            icNumCycle++;
            printf("TIM IC Capture Value: %lu, Capture Count: %lu\r\n", icGapValue, icNumCycle);
        }
    }
}

#endif // TIM_COMBINE_MODE_ENABLE


#if TIM_DMA_BURST_ENABLE

static void GPIO_IC_CH2_DMAburst_Init(void);
static void DMA_IC_CH2_Init(TIM_HandleTypeDef *htim);


/**
  * @brief  TIM IC MODE 测试
  * @param  None
  * @retval None
  */
void TIM_IC_MODE_Test(void)
{
    
    LED_init();
    uartInit(USART1, 115200); // 初始化串口1,波特率115200
    printf("TIM DMABurst Test Start !!!\r\n");

    // 初始化定时器输入捕获模式
    TIM_IC_DMAburst_Mode_Init();
    while (1)
    {
        LED0(1); // 翻转LED0状态
        HAL_Delay(1000);
        LED0(0); // 关闭LED0
        HAL_Delay(1000);
    }
}

void TIM_IC_DMAburst_Mode_Init(void)
{
    /* TIM base*/
    g_TIM_HandleTypeStruct.Instance = TIMx_IC_INSTANCE; // 选择TIMx作为输入捕获定时器
    g_TIM_HandleTypeStruct.Init.Period =  0x10000 - 1; // 设置自动重载寄存器值
    g_TIM_HandleTypeStruct.Init.Prescaler =  8400 - 1; // 设置预分频值
    g_TIM_HandleTypeStruct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 自动重载预装载使能
    g_TIM_HandleTypeStruct.Init.RepetitionCounter = 0; // 设置重复计数器
    g_TIM_HandleTypeStruct.Init.CounterMode = TIM_COUNTERMODE_UP; // 设置计数模式
    g_TIM_HandleTypeStruct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 设置时钟分频
    if(HAL_TIM_IC_Init(&g_TIM_HandleTypeStruct) != HAL_OK)
    {
        printf("TIM IC Init Error !!!\r\n");
        return;
    }
    /* TIM CH1 配置 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; // 设置输入捕获极性为上升沿
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI; // 设置输入选择为CH1->TI2
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0x03;
    if (HAL_TIM_IC_ConfigChannel(&g_TIM_HandleTypeStruct, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        printf("TIM IC Config Channel Error !!!\r\n");
        return;
    }
    /* TIM CH2 配置 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING; // 设置输入捕获极性为下降沿
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; // 设置输入选择为CH2->TI2
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0x03;
    if (HAL_TIM_IC_ConfigChannel(&g_TIM_HandleTypeStruct, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
    {
        printf("TIM IC Config Channel Error !!!\r\n");
        return;
    }
    HAL_TIM_IC_Start(&g_TIM_HandleTypeStruct,TIM_CHANNEL_1); // 启动CH1输入捕获
    HAL_TIM_IC_Start(&g_TIM_HandleTypeStruct,TIM_CHANNEL_2); // 启动CH2输入捕获
    // TIM DMA Burst 配置
    if(HAL_TIM_DMABurst_ReadStart(&g_TIM_HandleTypeStruct, TIM_DMABASE_CCR1, TIM_DMA_CC2,pDMAbuffer,TIM_DMABURSTLENGTH_2TRANSFERS) != HAL_OK)
    {
        printf("TIM IC Start DMA Burst Error !!!\r\n");
        return;
    }

}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_INSTANCE)
    {
        __TIMx_IC_INSTANCE_CLK_ENABLE(); // 使能TIMx时钟
        GPIO_IC_CH2_DMAburst_Init(); // 配置输入捕获引脚
        DMA_IC_CH2_Init(htim); // 配置DMA
    }
}

void DMA1_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_hdma);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{   

    if(htim->Instance == TIMx_IC_INSTANCE)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            icGapValue = pDMAbuffer[1] - pDMAbuffer[0]; // 读取CH2捕获值 - CH1捕获值
            __HAL_TIM_SET_COUNTER(htim, 0); // 清除计数器
            printf("icGapValue = %d\r\n", icGapValue);
        }
    }
}
/**
 * @brief  GPIO配置
 * @param  None-->配置TIM2_CH2通道
 */
static void GPIO_IC_CH2_DMAburst_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟
    /* TIM2_CH2通道初始化*/
    GPIO_InitStruct.Pin = GPIO_PIN_1; // 设置引脚
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 设置为复用推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 设置为无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 设置为高速
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2; // 设置复用功能
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}



static void DMA_IC_CH2_Init(TIM_HandleTypeDef *htim)
{

    
    __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE(); // 使能DMA时钟

    g_hdma.Instance = DMA1_Stream6; // 设置DMA流
    g_hdma.Init.Channel = DMA_CHANNEL_3; // 设置DMA通道
    g_hdma.Init.Direction = DMA_PERIPH_TO_MEMORY; // 设置传输方向
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
    __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_CC2], g_hdma); // 关联DMA到定时器
    if(HAL_DMA_Init(&g_hdma) != HAL_OK)
    {
        printf("DMA IC Init Error !!!\r\n");
        return;
    }   
    // 设置DMA中断优先级
    {
        HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, TIM_DMA_PreemptPriority, TIM_DMA_SubPriority);
        HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    }
}
#endif // TIM_DMA_BURST_ENABLE


static void GPIO_IC_CHx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_TIMx_IC_CHx_CLK_ENABLE(); // 使能GPIOx时钟

    GPIO_InitStruct.Pin = TIMx_IC_CHx_Pin;
    GPIO_InitStruct.Mode = TIMx_IC_CHx_Mode;
    GPIO_InitStruct.Pull = TIMx_IC_CHx_Pull;
    GPIO_InitStruct.Speed = TIMx_IC_CHx_Speed;
    GPIO_InitStruct.Alternate = TIMx_IC_CHx_AF_FOR_TIM; // 设置复用功能
    HAL_GPIO_Init(TIMx_IC_CHx_GPIO_Port, &GPIO_InitStruct);
}
