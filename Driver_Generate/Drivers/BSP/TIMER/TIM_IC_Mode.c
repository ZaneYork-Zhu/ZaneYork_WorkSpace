#include "TIM_IC_Mode.h"
#include <stdio.h>
#include "led.h"


TIM_HandleTypeDef g_TIM_HandleTypeStruct = {0};
TIM_IC_InitTypeDef sConfigIC = {0};
static uint32_t icEdgeFlag = 1; // 边沿标志位



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
#endif
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
    if(HAL_TIM_IC_Start_DMA(&g_TIM_HandleTypeStruct, deviceInit->ICchannel, (uint32_t *)pData, Length) != HAL_OK)
    {
        printf("TIM IC Start DMA Error !!!\r\n");
        return;
    }
#endif // TIM_IC_MODE_DMA_ENABLE

}

#if TIM_IC_MODE_DMA_ENABLE
static void DMA_IC_CHx_Init(TIM_HandleTypeDef *htim)
{
    DMA_HandleTypeDef hdma = {0};
    
    __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE(); // 使能DMA时钟

    hdma.Instance = TIM_IC_DMA_Stream; // 设置DMA流
    hdma.Init.Channel = TIM_IC_DMA_Channel; // 设置DMA通道
    hdma.Init.Direction = DMA_MEMORY_TO_PERIPH; // 设置传输方向
    hdma.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不递增
    hdma.Init.MemInc = DMA_MINC_ENABLE; // 内存地址递增
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // 外设数据对齐方式
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; // 内存数据对齐方式
#if TIM_DMA_NORMAL_ENABLE
    hdma.Init.Mode = DMA_NORMAL; // 设置DMA模式
#else
    hdma.Init.Mode = DMA_CIRCULAR; // 设置DMA为循环模式
#endif // TIM_DMA_NORMAL_ENABLE
    hdma.Init.Priority = DMA_PRIORITY_VERY_HIGH; // 设置DMA优先级

    if(HAL_DMA_Init(&hdma) != HAL_OK)
    {
        printf("DMA IC Init Error !!!\r\n");
        return;
    }

    __HAL_LINKDMA(htim, hdma[TIMx_IC_CHx], hdma); // 关联DMA句柄到TIM句柄
    // 设置DMA中断优先级
    {
        HAL_NVIC_SetPriority(TIM_IC_DMA_IRQn, TIM_DMA_PreemptPriority, TIM_DMA_SubPriority);
        HAL_NVIC_EnableIRQ(TIM_IC_DMA_IRQn);
    }
}
#endif // TIM_IC_MODE_DMA_ENABLE

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

static uint32_t TIM_setEdge(NEW_TIM_HandleTypeDef devStruct , uint32_t edge)
{
    uint32_t retVal = HAL_TIM_ReadCapturedValue(devStruct.HAL_Handle, devStruct.ICchannel); // 读取当前捕获值
    HAL_TIM_IC_Stop_IT(devStruct.HAL_Handle, devStruct.ICchannel); // 停止输入捕获中断
    __HAL_TIM_SET_COUNTER(devStruct.HAL_Handle, 0); // 清除计数器
    TIM_RESET_CAPTUREPOLARITY(devStruct.HAL_Handle, devStruct.ICchannel); // 重置输入捕获极性
    TIM_SET_CAPTUREPOLARITY(devStruct.HAL_Handle, devStruct.ICchannel,edge); // 设置为指定边沿捕获
    HAL_TIM_IC_Start_IT(devStruct.HAL_Handle, devStruct.ICchannel); // 重新启动输入捕获中断
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
#endif // TIM_IC_MODE_DMA_ENABLE
    }
}

#if TIM_IC_MODE_IT_ENABLE

void TIMx_IC_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_HandleTypeStruct);
    __HAL_TIM_CLEAR_FLAG(&g_TIM_HandleTypeStruct, TIM_FLAG_CC1); // 清除输入捕获标志位
    
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t icOldValue,icNewValue,icGapValue;
    icOldValue = icNewValue = icGapValue = 0; // 初始化捕获值
    static uint32_t icNumCycle = 0; // 输入捕获计数
    if (htim->Instance == TIMx_IC_INSTANCE)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
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
    }
}


#endif // TIM_IC_MODE_IT_ENABLE
