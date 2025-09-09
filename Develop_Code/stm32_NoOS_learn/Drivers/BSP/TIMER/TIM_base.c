
#include "TIM_base.h"
#include "led.h"
#include <stdio.h>

TIM_HandleTypeDef g_tim_base_handle = {0};
DMA_HandleTypeDef g_dma_UpdateEvent_handle = {0};

uint16_t g_tim_base_dma_buffer[] = {20000,30000,40000,50000}; // DMA传输数据缓冲区
static uint8_t DMA_TIM_Update_FinishFlag = 0; // DMA传输完成标志位
void Tim_base_Test(void)
{
    
    LED_init();
    uartInit(USART1, 115200); // 初始化串口1,波特率115200
    printf("BASE_TIM <<<<Update>>>> test start !!!\r\n");
    // 初始化定时器基础功能
    Tim_base_Init(BASE_TIM, 10000, 16800, 0); // BASE_TIM, 自动重装载值10000, 预分频系数16800, 不使能自动重载预装载
    while (1)
    {
#if TIM_POLLING_ENABLE
        // 轮询方式处理定时器更新事件
        if(__HAL_TIM_GET_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE)){
            __HAL_TIM_CLEAR_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE); // 清除更新事件标志位
            LED1_TOGGLE(); // 翻转LED1状态
            printf("TIM base Update Event Polling %d !!!\r\n", num);
        switch (num){
            case 5:
                Set_TIM_Period(5000); // 修改定时器周期为5000
                break;
            case 10:
                printf("TIM base Count Polling !!! count: %d\r\n", num);
                HAL_TIM_Base_Stop(&g_tim_base_handle); // 停止定时器
                break;

            default:
                break;
        }
        num++;
    }
#endif
#if TIM_IT_ENABLE
        // 中断方式处理定时器更新事件
        // 在中断回调函数中处理更新事件
        // 这里不需要额外的代码，具体处理在 HAL_TIM_PeriodElapsedCallback 中实现
#endif
    }
    
}


void Set_TIM_Period(uint32_t period) 
{ 
    if (g_tim_base_handle.Init.AutoReloadPreload == TIM_AUTORELOAD_PRELOAD_ENABLE) { 
        // ARPE 为 1，直接修改 ARR 值 
        __HAL_TIM_SET_AUTORELOAD(&g_tim_base_handle, period - 1); 
        __HAL_TIM_CLEAR_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE); // 清除更新事件标志位 
    } 
    else { 
        // ARPE 为 0，需要先停止定时器、修改 ARR 值再启动定时器 
        HAL_TIM_Base_Stop(&g_tim_base_handle); // 停止定时器 
        __HAL_TIM_SET_COUNTER(&g_tim_base_handle, 0); // 重置计数器 
        g_tim_base_handle.Init.Period = period - 1; 
        HAL_TIM_Base_Init(&g_tim_base_handle); 
        HAL_TIM_Base_Start(&g_tim_base_handle); 
        __HAL_TIM_CLEAR_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE); // 清除更新事件标志位 
    } 
} 


void Tim_base_Init(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc, uint8_t setARPE)
{
    // 设置定时器实例
    g_tim_base_handle.Instance = TimInstance;
    // 设置预分频系数
    g_tim_base_handle.Init.Prescaler = psc - 1;
    // 设置计数模式为向上计数
    g_tim_base_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    // 设置自动重装载寄存器值
    g_tim_base_handle.Init.Period = arr - 1;
    // 设置自动重载预装载使能
    g_tim_base_handle.Init.AutoReloadPreload = setARPE ? TIM_AUTORELOAD_PRELOAD_ENABLE : TIM_AUTORELOAD_PRELOAD_DISABLE;

#if REPETITION_COUNTER_ENABLE
    // 设置重复计数器值（如果需要）
    g_tim_base_handle.Init.RepetitionCounter = REPETITION_COUNTER_VALUE; // 对于基本定时器，这通常为0
#endif // REPETITION_COUNTER_ENABLE
    // 初始化定时器基础功能
    if (HAL_TIM_Base_Init(&g_tim_base_handle) != HAL_OK)
    {
        // 初始化失败，调用错误处理函数
        Error_Handler();
    }
    __HAL_TIM_CLEAR_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE); // 清除更新事件标志位
#if TIM_POLLING_ENABLE
    // 启动定时器基础功能
    if (HAL_TIM_Base_Start(&g_tim_base_handle) != HAL_OK)
    {
        Error_Handler();
    }
#endif // TIM_POLLING_ENABLE

#if TIM_IT_ENABLE
    // 启动定时器基础功能中断
    if (HAL_TIM_Base_Start_IT(&g_tim_base_handle) != HAL_OK)
    {
        Error_Handler();
    }
#endif // TIM_IT_ENABLE

#if TIM_DMA_ENABLE
    // 启动定时器DMA+IT
    __HAL_TIM_ENABLE_IT(&g_tim_base_handle, TIM_IT_UPDATE);
    // 启动定时器基础功能DMA
    if (HAL_TIM_Base_Start_DMA(&g_tim_base_handle, (const uint32_t *)g_tim_base_dma_buffer, sizeof(g_tim_base_dma_buffer)/sizeof(g_tim_base_dma_buffer[0])) != HAL_OK)
    {
        Error_Handler();
    }
    

#endif // TIM_DMA_ENABLE


}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BASE_TIM)
    {
        // TIM9时钟使能
        __BASE_TIM_CLK_ENABLE();
#if TIM_IT_ENABLE
        // 配置NVIC中断优先级和使能
        HAL_NVIC_SetPriority(BASE_TIM_IRQn, TIM_PreemptPriority, TIM_SubPriority);
        HAL_NVIC_EnableIRQ(BASE_TIM_IRQn);
#endif // TIM_IT_ENABLE

#if TIM_DMA_ENABLE  
        // DMA时钟使能
        __HAL_RCC_DMA2_CLK_ENABLE(); // 假设使用DMA2
        // 设置DMA端口
        g_dma_UpdateEvent_handle.Instance =  DMA2_Stream5; // 假设使用DMA2_Stream5
        g_dma_UpdateEvent_handle.Init.Channel = DMA_CHANNEL_6; // 假设使用DMA通道6
        g_dma_UpdateEvent_handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 从内存到外设
        g_dma_UpdateEvent_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不自增
        g_dma_UpdateEvent_handle.Init.MemInc = DMA_MINC_ENABLE; // 内存地址自增
        // 设置DMA传输数据大小 = 半字（16位）
        g_dma_UpdateEvent_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据对齐方式
        g_dma_UpdateEvent_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; // 内存数据对齐方式
#if TIM_DMA_NORMAL_ENABLE
        // 设置DMA传输模式 = 普通模式[非循环模式]
        g_dma_UpdateEvent_handle.Init.Mode = DMA_NORMAL; // 普通模式
#else
        // 设置DMA传输模式 = 循环模式
        g_dma_UpdateEvent_handle.Init.Mode = DMA_CIRCULAR; // 循环模式
#endif // TIM_DMA_NORMAL_ENABLE
        g_dma_UpdateEvent_handle.Init.Priority = DMA_PRIORITY_MEDIUM; // 中优先级
        g_dma_UpdateEvent_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO模式
        // 初始化DMA
        if (HAL_DMA_Init(&g_dma_UpdateEvent_handle) != HAL_OK)
        {
            // DMA初始化失败，调用错误处理函数
            Error_Handler();
        }
        // 将DMA句柄与定时器句柄关联
        __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_UPDATE], g_dma_UpdateEvent_handle);
        // 配置DMA中断优先级和使能
        {
            HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, TIM_DMA_PreemptPriority, TIM_DMA_SubPriority);
            HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
        }
        // 配置IT更新中断优先级和使能
        {
            HAL_NVIC_SetPriority(BASE_TIM_IRQn, TIM_IT_PreemptPriority, TIM_IT_SubPriority);
            HAL_NVIC_EnableIRQ(BASE_TIM_IRQn);
        }
#endif // TIM_DMA_ENABLE

        
    }
}


#if TIM_DMA_ENABLE

/**
 * 1.采用DMA+IT方式处理定时器更新事件
 * 2.在DMA传输完成回调函数中处理更新事件
 * 3.在HAL_TIM_PeriodElapsedCallback中处理更新事件
 * 
 * @note :实现功能如下：
 *         1.DMA传输完成后，翻转LED0状态
 *         2.每次IT更新事件翻转LED1状态
 *         3.每10次IT更新事件打印一次计数值，并停止定时器
 */
void DMA2_Stream5_IRQHandler(void)
{
    DMA_TIM_Update_FinishFlag = 1; // 设置DMA传输完成标志位
    HAL_DMA_IRQHandler(&g_dma_UpdateEvent_handle);
    DMA_TIM_Update_FinishFlag = 0; // 清除DMA传输完成标志位
}

void BASE_TIM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim_base_handle);
    __HAL_TIM_CLEAR_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE); // 清除更新事件标志位
}

/**
 * @note :实现功能如下：
 *         1.HAL_TIM_Base_GetState(htim) == HAL_TIM_STATE_READY部分
 *            1`每次IT更新事件翻转LED1状态
 *            每10次IT更新事件打印一次计数值，并停止定时器
 *         2.htim->hdma[TIM_DMA_ID_UPDATE]->State == HAL_DMA_STATE_READY部分
 *            DMA传输完成后，翻转LED0状态
 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim->Instance == BASE_TIM)
    {
#if TIM_DMA_NORMAL_ENABLE
        if(!DMA_TIM_Update_FinishFlag)
        {
            static uint8_t num = 0;
            /**
             * @note :注意这里没办法用标志位判断DMA[循环模式下]是否完成-->因为TIM_DMA中没有修改标志位
             */
            LED1_TOGGLE(); // 翻转LED1状态
            printf("TIM base Update Event num = %d ,ARRvalue = %d!!!\r\n",num,__HAL_TIM_GetAutoreload(&g_tim_base_handle));
            num++;
        }
        if(DMA_TIM_Update_FinishFlag)
        {
            DMA_TIM_Update_FinishFlag = 0; // 清除DMA传输完成标志位
            LED0_TOGGLE(); // 翻转LED0状态
            printf("TIM base Update Event DMA finish !!!\r\n");
            HAL_DMA_DeInit(&g_dma_UpdateEvent_handle); // 停止DMA传输
        }

#else 
        if(!DMA_TIM_Update_FinishFlag)
        {
            static uint8_t num = 1;
            /**
             * @note :注意这里没办法用标志位判断DMA[循环模式下]是否完成-->因为TIM_DMA中没有修改标志位
             */
            LED1_TOGGLE(); // 翻转LED1状态
            printf("TIM base Update Event num = %d ,ARRvalue = %d!!!\r\n",num,__HAL_TIM_GetAutoreload(&g_tim_base_handle));
            num++;
        }
        if(DMA_TIM_Update_FinishFlag)
        {
            DMA_TIM_Update_FinishFlag = 0; // 清除DMA传输完成标志位
            LED0_TOGGLE(); // 翻转LED0状态
            printf("TIM base Update Event DMA finish !!!\r\n");
            //HAL_DMA_DeInit(&g_dma_UpdateEvent_handle); // 停止DMA传输
        }

#endif // TIM_DMA_NORMAL_ENABLE
    }
}

#endif // TIM_DMA_ENABLE

#if TIM_IT_ENABLE
void BASE_TIM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim_base_handle);
    __HAL_TIM_CLEAR_FLAG(&g_tim_base_handle, TIM_FLAG_UPDATE); // 清除更新事件标志位
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BASE_TIM)
    {
        static uint8_t num = 0;
        LED0_TOGGLE(); // 翻转LED0状态
        printf("TIM base Update Event IT %d !!!\r\n", num);
        num++;
        if (num >= 10) // 每10次更新事件打印一次
        {
            printf("TIM base Count IT !!! count: %d\r\n", num);
            HAL_TIM_Base_Stop(&g_tim_base_handle); // 停止定时器
        }
    }
    // 可以添加更多定时器的中断处理代码
}

#endif // TIM_IT_ENABLE


