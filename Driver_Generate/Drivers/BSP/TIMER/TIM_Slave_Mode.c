#include "TIM_Slave_Mode.h"
#include <stdio.h>
#include "led.h"


uint32_t g_TIM_SlaveReset_TriggerNum = 0; // 触发事件计数


TIM_HandleTypeDef g_TIM_SlaveReset_Base_Handle = {0};
TIM_IC_InitTypeDef g_TIM_IC_Handle = {0};
TIM_SlaveConfigTypeDef g_TIM_Slave_Handle = {0};

TIM_HandleTypeDef TIM_Base_Struct = {
    .Instance = TIMx_SLAVE_INSTANCE,    //定时器实例
    .Init = {
        .Period = 0x100000000-1, //自动重载寄存器值
        .Prescaler = 8400-1,   //预分频值
        .CounterMode = TIM_COUNTERMODE_UP, //计数模式
        .ClockDivision = TIM_CLOCKDIVISION_DIV1, //时钟分频：不分频
        .RepetitionCounter = 0, //重复计数器值：不重复
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE //自动重载预载：关闭
    },
    .Channel = HAL_TIM_ACTIVE_CHANNEL_1,   //通道1
};

TIM_SlaveConfigTypeDef TIM_Slave_Struct = {
#if TIM_SLAVE_RESET_MODE
    .SlaveMode = TIM_SLAVEMODE_RESET, //从模式：复位模式
    .InputTrigger = TIM_TS_TI1F_ED, //输入触发源：双边沿触发
    .TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING, //触发极性：上升沿
    .TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1, //触发预分频：不分频
    .TriggerFilter = 0x08,                           //触发滤波器：滤波器值
#elif TIM_SLAVE_GATED_MODE
    .SlaveMode = TIM_SLAVEMODE_GATED, //从模式：门控模式
    .InputTrigger = TIM_TS_TI1FP1, //输入触发源：TIMx_CH1
    .TriggerPolarity = TIM_TRIGGERPOLARITY_RISING, //触发极性：上升沿
    .TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1, //触发预分频：不分频
    .TriggerFilter = 0x08                           //触发滤波器：滤波器值
#elif TIM_SLAVE_TRIGGER_MODE
    .SlaveMode = TIM_SLAVEMODE_TRIGGER, //从模式：触发模式
    .InputTrigger = TIM_TS_TI1FP1, //输入触发源：TIMx_CH1
    .TriggerPolarity = TIM_TRIGGERPOLARITY_RISING, //触发极性：上升沿
    .TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1, //触发预分频：不分频
    .TriggerFilter = 0x08                           //触发滤波器：滤波器值
#elif TIM_SLAVE_EXTERNAL_MODE
    .SlaveMode = TIM_SLAVEMODE_EXTERNAL1, //从模式：外部时钟+触发模式
    .InputTrigger = TIM_TS_TI1FP1, //输入触发源：TIMx_CH1
    .TriggerPolarity = TIM_TRIGGERPOLARITY_RISING, //触发极性：上升沿
    .TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1, //触发预分频：不分频
    .TriggerFilter = 0x08                           //触发滤波器：滤波器值
#endif
};

nTIM_IC_InitTypeDef TIM_IC_Struct = {
    .IC_Init = {
        .ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING, //输入极性：上升沿
        .ICSelection = TIM_ICSELECTION_DIRECTTI, //输入选择：直接输入   
        .ICPrescaler = TIM_ICPSC_DIV1, //输入预分频：不分频
        .ICFilter = 0x08, //输入滤波器：滤波器值
    },
    .IC_Channel = TIM_CHANNEL_1 //通道1
};

void TIM_Slave_Mode_Test(void)
{
    uartInit(USART1, 115200); // 初始化串口
    printf("TIM Slave Reset Mode Init...\r\n");
    TIM_SlaveReset_Mode_Init(&TIM_Base_Struct, &TIM_Slave_Struct, &TIM_IC_Struct); // 初始化定时器从模式
    while (1)
    {
        printf("Current TIM Counter Value: %d\r\n",__HAL_TIM_GET_COUNTER(&g_TIM_SlaveReset_Base_Handle)); // 打印当前计数值
        if(__HAL_TIM_GET_FLAG(&g_TIM_SlaveReset_Base_Handle, TIM_FLAG_CC1)){
            __HAL_TIM_CLEAR_FLAG(&g_TIM_SlaveReset_Base_Handle, TIM_FLAG_CC1); // 清除捕获/
            g_TIM_SlaveReset_TriggerNum = 0; // 重置触发事件计数
            printf("TIM Capture/Compare 1 Event generated!\r\n");
        }
        if(__HAL_TIM_GET_FLAG(&g_TIM_SlaveReset_Base_Handle, TIM_FLAG_TRIGGER)){
            __HAL_TIM_CLEAR_FLAG(&g_TIM_SlaveReset_Base_Handle, TIM_FLAG_TRIGGER); // 清除触发标志
            printf("TIM Trigger Event generated Num = %lu!!\r\n", ++g_TIM_SlaveReset_TriggerNum); // 打印触发事件计数
        }
        HAL_Delay(100); // 延时1秒

    }
}
    


/************************************************************************************************************************************ */
HAL_StatusTypeDef TIM_SlaveReset_Mode_Init(TIM_HandleTypeDef *pBaseStr, TIM_SlaveConfigTypeDef *pSlaveStr,nTIM_IC_InitTypeDef *pICStr)
{
    /* TIM Base Config */
    g_TIM_SlaveReset_Base_Handle.Instance = pBaseStr->Instance;
    g_TIM_SlaveReset_Base_Handle.Init.Period = pBaseStr->Init.Period;
    g_TIM_SlaveReset_Base_Handle.Init.Prescaler = pBaseStr->Init.Prescaler;
    g_TIM_SlaveReset_Base_Handle.Init.CounterMode = pBaseStr->Init.CounterMode;
    g_TIM_SlaveReset_Base_Handle.Init.ClockDivision = pBaseStr->Init.ClockDivision;
    g_TIM_SlaveReset_Base_Handle.Init.RepetitionCounter = pBaseStr->Init.RepetitionCounter;
    g_TIM_SlaveReset_Base_Handle.Init.AutoReloadPreload = pBaseStr->Init.AutoReloadPreload;
    if(HAL_TIM_IC_Init(&g_TIM_SlaveReset_Base_Handle) != HAL_OK)
    {
        printf("TIM Base Init Error!\n");
        return HAL_ERROR;
    }
    /*TIM Slave Config*/
    g_TIM_Slave_Handle.SlaveMode = pSlaveStr->SlaveMode;
    g_TIM_Slave_Handle.InputTrigger = pSlaveStr->InputTrigger;
    g_TIM_Slave_Handle.TriggerPolarity = pSlaveStr->TriggerPolarity;
    g_TIM_Slave_Handle.TriggerPrescaler = pSlaveStr->TriggerPrescaler;
    g_TIM_Slave_Handle.TriggerFilter = pSlaveStr->TriggerFilter;
    if(HAL_TIM_SlaveConfigSynchro(&g_TIM_SlaveReset_Base_Handle, &g_TIM_Slave_Handle) != HAL_OK)
    {
        printf("TIM Slave Config Error!\n");
        return HAL_ERROR;
    }
    /*TIM IC Config*/
    g_TIM_IC_Handle.ICFilter = pICStr->IC_Init.ICFilter;
    g_TIM_IC_Handle.ICSelection = pICStr->IC_Init.ICSelection;
    g_TIM_IC_Handle.ICPolarity = pICStr->IC_Init.ICPolarity;
    g_TIM_IC_Handle.ICPrescaler = pICStr->IC_Init.ICPrescaler;
    if(HAL_TIM_IC_ConfigChannel(&g_TIM_SlaveReset_Base_Handle, &g_TIM_IC_Handle, pICStr->IC_Channel) != HAL_OK)
    {
        printf("TIM IC Config Error!\n");
        return HAL_ERROR;
    }
#if TIM_POLLING_ENABLE
    if(HAL_TIM_IC_Start(&g_TIM_SlaveReset_Base_Handle, pICStr->IC_Channel) != HAL_OK)
    {
        printf("TIM IC Start Error!\n");
        return HAL_ERROR;
    }
#elif TIM_IT_ENABLE
    if(HAL_TIM_IC_Start_IT(&g_TIM_SlaveReset_Base_Handle, pICStr->IC_Channel) != HAL_OK)
    {
        printf("TIM IC Start IT Error!\n");
        return HAL_ERROR;
    }
#elif TIM_DMA_ENABLE
    if(HAL_TIM_IC_Start_DMA(&g_TIM_SlaveReset_Base_Handle, pICStr->IC_Channel, pICStr->IC_DMA_Buffer, pICStr->IC_DMA_Buffer_Size) != HAL_OK)
    {
        printf("TIM IC Start DMA Error!\n");
        return HAL_ERROR;
    }
#endif /* TIM_IC_MODE_POLLING_ENABLE */
    return HAL_OK;
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    __TIMx_SLAVE_INSTANCE_CLK_ENABLE(); // 使能TIMx时钟
    GPIO_IC_CHx_Init(); // 初始化GPIO

}

/**
 * @brief  配置TIMx的输入捕获通道
 * @param  None
 * @retval None
 */
static void GPIO_IC_CHx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_TIMx_SLAVE_CHx_CLK_ENABLE();
    GPIO_InitStruct.Pin = TIMx_SLAVE_CHx_Pin; 
    GPIO_InitStruct.Mode = TIMx_SLAVE_CHx_Mode; 
    GPIO_InitStruct.Pull = TIMx_SLAVE_CHx_Pull; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    GPIO_InitStruct.Alternate = TIMx_SLAVE_CHx_AF_FOR_TIM; // 设置复用功能
    HAL_GPIO_Init(TIMx_SLAVE_CHx_GPIO_Port, &GPIO_InitStruct);
}
