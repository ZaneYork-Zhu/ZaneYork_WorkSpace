#include "TIM_IC_PWMandSlave.h"
#include <stdio.h>



TIM_HandleTypeDef g_TIM_IC_Base_Handle = {0};
TIM_IC_InitTypeDef g_TIM_IC1_Handle = {0};  // 通道1
TIM_IC_InitTypeDef g_TIM_IC2_Handle = {0};  // 通道2
TIM_SlaveConfigTypeDef g_TIM_Slave_Handle = {0};

TIM_HandleTypeDef TIM_Base_Struct = {
    .Instance = TIMx_IC_PWM_INSTANCE,           // 定时器实例
    .Init = {
        .Prescaler = 84 - 1,                     // 84Mhz/84=1Mhz
        .CounterMode = TIM_COUNTERMODE_UP,       // 向上计数模式
        .Period = 0xFFFF,                        // 自动重装载值
        .ClockDivision = TIM_CLOCKDIVISION_DIV1, // 时钟分频因子
        .RepetitionCounter = 0,                  // 重复计数器值
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE,
    },
    .Channel = HAL_TIM_ACTIVE_CHANNEL_1,
    .State = HAL_TIM_STATE_RESET,
};

/*FP1->作为从模式：复位信号【触发输入】*/
TIM_SlaveConfigTypeDef TIM_IC1_Set_Slave_Struct = {
    .SlaveMode = TIM_SLAVEMODE_RESET,              // 复位模式
    .InputTrigger = TIM_TS_TI1FP1,                 // 触发源选择为TI1
    .TriggerPolarity = TIM_TRIGGERPOLARITY_RISING, // 触发极性为上升沿
    .TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1, // 不分频
    .TriggerFilter = 0x08,                            // 不滤波
};

/*FP1->直连*/
nTIM_IC_InitTypeDef TIM_IC1_Struct = {
    .IC_Init = {
        .ICPolarity = TIM_ICPOLARITY_RISING,       // 上升沿捕获
        .ICSelection = TIM_ICSELECTION_DIRECTTI,    // 直接连接到定时器输入
        .ICPrescaler = TIM_ICPSC_DIV1,             // 不分频
        .ICFilter = 0x08,                             // 不滤波
    },
    .IC_Channel = TIM_CHANNEL_1,                   // 通道1
};

/*FP2->通道交叉*/
nTIM_IC_InitTypeDef TIM_IC2_Struct = {
    .IC_Init = {
        .ICPolarity = TIM_ICPOLARITY_FALLING,       // 下降沿捕获
        .ICSelection = TIM_ICSELECTION_INDIRECTTI,  // 非直连->通道交叉
        .ICPrescaler = TIM_ICPSC_DIV1,              // 不分频
        .ICFilter = 0x08,                           // 不滤波
    },
    .IC_Channel = TIM_CHANNEL_2,                    // 通道2
};





void TIM_IC_PWMandSlave_Test(void)
{
    uartInit(USART1, 115200); // 初始化串口
    printf("TIM IC and PWM input Init...\r\n");
    TIM_IC_PWMandSlave_Init(&TIM_Base_Struct, &TIM_IC1_Struct, &TIM_IC2_Struct, &TIM_IC1_Set_Slave_Struct);
    while (1)
    {
        /* code */
    }
    
}


/**
 * @note :初始化定时器模式
 */
static HAL_StatusTypeDef TIM_IC_PWMandSlave_Init(   TIM_HandleTypeDef *pBaseStr,
                                                    nTIM_IC_InitTypeDef *pIC1Str,
                                                    nTIM_IC_InitTypeDef *pIC2Str,
                                                    TIM_SlaveConfigTypeDef *pSlaveStr)
{
    /*TIM Base Config*/
    g_TIM_IC_Base_Handle.Instance = pBaseStr->Instance;
    g_TIM_IC_Base_Handle.Init.Prescaler = pBaseStr->Init.Prescaler;
    g_TIM_IC_Base_Handle.Init.CounterMode = pBaseStr->Init.CounterMode;
    g_TIM_IC_Base_Handle.Init.Period = pBaseStr->Init.Period;
    g_TIM_IC_Base_Handle.Init.ClockDivision = pBaseStr->Init.ClockDivision;
    g_TIM_IC_Base_Handle.Init.RepetitionCounter = pBaseStr->Init.RepetitionCounter;
    g_TIM_IC_Base_Handle.Init.AutoReloadPreload = pBaseStr->Init.AutoReloadPreload;
    if (HAL_TIM_IC_Init(&g_TIM_IC_Base_Handle) != HAL_OK)
    {
        printf("TIM_IC_Init Error\r\n");
        return HAL_ERROR;
    }
    /*TIM Slave Config*/
    g_TIM_Slave_Handle.SlaveMode = pSlaveStr->SlaveMode;
    g_TIM_Slave_Handle.InputTrigger = pSlaveStr->InputTrigger;
    g_TIM_Slave_Handle.TriggerPolarity = pSlaveStr->TriggerPolarity;
    g_TIM_Slave_Handle.TriggerPrescaler = pSlaveStr->TriggerPrescaler;
    g_TIM_Slave_Handle.TriggerFilter = pSlaveStr->TriggerFilter;
    if (HAL_TIM_SlaveConfigSynchronization(&g_TIM_IC_Base_Handle, &g_TIM_Slave_Handle) != HAL_OK)
    {
        printf("TIM_SlaveConfigSynchronization Error\r\n");
        return HAL_ERROR;
    }   

    /*TIM IC Config*/
    g_TIM_IC1_Handle.ICPolarity = pIC1Str->IC_Init.ICPolarity;
    g_TIM_IC1_Handle.ICSelection = pIC1Str->IC_Init.ICSelection;
    g_TIM_IC1_Handle.ICPrescaler = pIC1Str->IC_Init.ICPrescaler;
    g_TIM_IC1_Handle.ICFilter = pIC1Str->IC_Init.ICFilter;
    if (HAL_TIM_IC_ConfigChannel(&g_TIM_IC_Base_Handle, &g_TIM_IC1_Handle, pIC1Str->IC_Channel) != HAL_OK)
    {
        printf("TIM_IC1_ConfigChannel Error\r\n");
        return HAL_ERROR;
    }
    g_TIM_IC2_Handle.ICPolarity = pIC2Str->IC_Init.ICPolarity;
    g_TIM_IC2_Handle.ICSelection = pIC2Str->IC_Init.ICSelection;
    g_TIM_IC2_Handle.ICPrescaler = pIC2Str->IC_Init.ICPrescaler;
    g_TIM_IC2_Handle.ICFilter = pIC2Str->IC_Init.ICFilter;
    if (HAL_TIM_IC_ConfigChannel(&g_TIM_IC_Base_Handle, &g_TIM_IC2_Handle, pIC2Str->IC_Channel) != HAL_OK)
    {
        printf("TIM_IC2_ConfigChannel Error\r\n");
        return HAL_ERROR;
    }

    if (HAL_TIM_IC_Start_IT(&g_TIM_IC_Base_Handle, pIC1Str->IC_Channel) != HAL_OK)            // 启动输入捕获1
    {
        printf("TIM_IC1_Start_IT Error\r\n");
        return HAL_ERROR;
    }
    if (HAL_TIM_IC_Start_IT(&g_TIM_IC_Base_Handle, pIC2Str->IC_Channel) != HAL_OK)            // 启动输入捕获2
    {
        printf("TIM_IC2_Start_IT Error\r\n");
        return HAL_ERROR;
    }

    return HAL_OK;
}


void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_PWM_INSTANCE)
    {
        __TIMx_IC_PWM_CLK_ENABLE();   // 使能定时器时钟
        GPIO_IC1_Init();              // 初始化IC1引脚

        /* 配置定时器中断 */
        HAL_NVIC_SetPriority(TIMx_IC_PWM_INSTANCE_IRQn, TIM_IT_UPDATE_PreemptPriority, TIM_IT_UPDATE_SubPriority);
        HAL_NVIC_EnableIRQ(TIMx_IC_PWM_INSTANCE_IRQn);
    }
}


void TIMx_IC_PWM_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_IC_Base_Handle);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIMx_IC_PWM_INSTANCE)
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // 通道1捕获事件
        {
            uint32_t ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 读取捕获值
            printf("PWM周期 CCR1 Value: %lu\r\n", ic_val1);

            uint32_t ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // 读取捕获值
            printf("PWM Duty Cycle CCR2 Value: %lu\r\n", (ic_val2 * 100 )/ ic_val1);
        }
    }
}


static void GPIO_IC1_Init(void)
{
    __HAL_RCC_TIMx_IC_CH1_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TIMx_IC_CH1_Pin;
    GPIO_InitStruct.Mode = TIMx_IC_CH1_Mode;
    GPIO_InitStruct.Pull = TIMx_IC_CH1_Pull;
    GPIO_InitStruct.Speed = TIMx_IC_CH1_Speed;
    GPIO_InitStruct.Alternate = TIMx_IC_CH1_AF_FOR_TIM;
    HAL_GPIO_Init(TIMx_IC_CH1_GPIO_Port, &GPIO_InitStruct);
}

