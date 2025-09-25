#include "TIM_Synchronization_RESET.h"
#include <stdio.h>
#include "led.h"

TIM_HandleTypeDef g_TIM_Base_SynMaster_Handle = {0};
TIM_HandleTypeDef g_TIM_Base_SynSlave_Handle = {0};
TIM_MasterConfigTypeDef g_TIM_Master_SynMaster_Config = {0};
TIM_SlaveConfigTypeDef g_TIM_Slave_SynSlave_Config = {0};



TIM_HandleTypeDef TIM_MasterStruct = {
    .Instance = TIMx_MASTER_INSTANCE,
    .Init = {
        .Prescaler = 84000 - 1,                  // 预分频器
        .CounterMode = TIM_COUNTERMODE_UP,       // 向上计数模式
        .Period = 1000 - 1,                      // 自动重装载值
        .ClockDivision = TIM_CLOCKDIVISION_DIV1, // 时钟分频
        .RepetitionCounter = 0,                   // 重复计数器
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE, // 自动重装载预装载使能
    },
};
TIM_MasterConfigTypeDef TIM_MasterConfigStruct = {
    .MasterOutputTrigger = TIM_TRGO_UPDATE,             // 触发输出选择为更新事件
    .MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE,      // 主从模式使能
};

TIM_HandleTypeDef TIM_SlaveStruct = {
    .Instance = TIMx_SLAVE_INSTANCE,
    .Init = {
        .Prescaler = 84000 - 1,                     // 预分频器
        .CounterMode = TIM_COUNTERMODE_UP,       // 向上计数模式
        .Period = 10000 - 1,                       // 自动重装载值
        .ClockDivision = TIM_CLOCKDIVISION_DIV1, // 时钟分频
        .RepetitionCounter = 0,                   // 重复计数器
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE, // 自动重装载预装载使能
    },
};

TIM_SlaveConfigTypeDef TIM_SlaveConfigStruct = {
    .SlaveMode = TIM_SLAVEMODE_RESET,      // 从模式选择为外部1
    .InputTrigger = TIM_TS_ITR0,               // 输入触发源选择为内部触发0（对应TIM1）
    .TriggerPolarity = TIM_TRIGGERPOLARITY_RISING, // 触发极性选择为上升沿
    .TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1, // 触发预分频器选择为不分频
    .TriggerFilter = 0,                        // 触发滤波器设置为0（无滤波）
};




void TIM_Syn_Test(void)
{
    uartInit(USART1,115200);
    printf("TIM_Syn_Test\r\n");
    TIM_Syn_Init(&TIM_MasterStruct, &TIM_MasterConfigStruct, &TIM_SlaveStruct, &TIM_SlaveConfigStruct);
    while (1)
    {
        printf("master timer count: %d\r\n", __HAL_TIM_GET_COUNTER(&g_TIM_Base_SynMaster_Handle));
        printf("slave timer count: %d\r\n", __HAL_TIM_GET_COUNTER(&g_TIM_Base_SynSlave_Handle));
    }
    
}



HAL_StatusTypeDef TIM_Syn_Init( TIM_HandleTypeDef *pBaseMasterStr, 
                                TIM_MasterConfigTypeDef *pMasterStr,
                                TIM_HandleTypeDef *pBaseSlaveStr, 
                                TIM_SlaveConfigTypeDef *pSlaveStr)
{

    /* TIM Base->Master Config*/
    g_TIM_Base_SynMaster_Handle.Instance = pBaseMasterStr->Instance;
    g_TIM_Base_SynMaster_Handle.Init.Prescaler         = pBaseMasterStr->Init.Prescaler;
    g_TIM_Base_SynMaster_Handle.Init.CounterMode       = pBaseMasterStr->Init.CounterMode;
    g_TIM_Base_SynMaster_Handle.Init.Period            = pBaseMasterStr->Init.Period;
    g_TIM_Base_SynMaster_Handle.Init.ClockDivision     = pBaseMasterStr->Init.ClockDivision;
    g_TIM_Base_SynMaster_Handle.Init.RepetitionCounter = pBaseMasterStr->Init.RepetitionCounter;
    g_TIM_Base_SynMaster_Handle.Init.AutoReloadPreload = pBaseMasterStr->Init.AutoReloadPreload;
    if (HAL_TIM_Base_Init(&g_TIM_Base_SynMaster_Handle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
        printf("TIM_Syn_Init: HAL_TIM_Base_Init error\r\n");
        return HAL_ERROR;
    }
    /* TIM Master Config */
    g_TIM_Master_SynMaster_Config.MasterOutputTrigger = pMasterStr->MasterOutputTrigger;
    g_TIM_Master_SynMaster_Config.MasterSlaveMode     = pMasterStr->MasterSlaveMode;
    if (HAL_TIMEx_MasterConfigSynchronization(&g_TIM_Base_SynMaster_Handle, &g_TIM_Master_SynMaster_Config) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
        printf("TIM_Syn_Init: HAL_TIMEx_MasterConfigSynchronization error\r\n");
        return HAL_ERROR;
    }
    /* TIM Base->Slave Config*/
    g_TIM_Base_SynSlave_Handle.Instance = pBaseSlaveStr->Instance;
    g_TIM_Base_SynSlave_Handle.Init.Prescaler         = pBaseSlaveStr->Init.Prescaler;
    g_TIM_Base_SynSlave_Handle.Init.CounterMode       = pBaseSlaveStr->Init.CounterMode;
    g_TIM_Base_SynSlave_Handle.Init.Period            = pBaseSlaveStr->Init.Period;
    g_TIM_Base_SynSlave_Handle.Init.ClockDivision     = pBaseSlaveStr->Init.ClockDivision;
    g_TIM_Base_SynSlave_Handle.Init.RepetitionCounter = pBaseSlaveStr->Init.RepetitionCounter;
    g_TIM_Base_SynSlave_Handle.Init.AutoReloadPreload = pBaseSlaveStr->Init.AutoReloadPreload;
    if (HAL_TIM_Base_Init(&g_TIM_Base_SynSlave_Handle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
        printf("TIM_Syn_Init: HAL_TIM_Base_Init error\r\n");
        return HAL_ERROR;
    }
    /* TIM Slave Config */
    g_TIM_Slave_SynSlave_Config.SlaveMode        = pSlaveStr->SlaveMode;
    g_TIM_Slave_SynSlave_Config.InputTrigger     = pSlaveStr->InputTrigger;
    g_TIM_Slave_SynSlave_Config.TriggerPolarity  = pSlaveStr->TriggerPolarity;
    g_TIM_Slave_SynSlave_Config.TriggerPrescaler = pSlaveStr->TriggerPrescaler;
    g_TIM_Slave_SynSlave_Config.TriggerFilter    = pSlaveStr->TriggerFilter;
    if (HAL_TIM_SlaveConfigSynchro(&g_TIM_Base_SynSlave_Handle, &g_TIM_Slave_SynSlave_Config) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
        printf("TIM_Syn_Init: HAL_TIM_SlaveConfigSynchro error\r\n");
        return HAL_ERROR;
    }
    __HAL_TIM_URS_ENABLE(&g_TIM_Base_SynSlave_Handle); // 使能更新请求源选择位，只能通过软件设置，硬件无法清除该位

    /* Start the TIM Base generation in interrupt mode */
    HAL_TIM_Base_Start_IT(&g_TIM_Base_SynMaster_Handle);
    HAL_TIM_Base_Start_IT(&g_TIM_Base_SynSlave_Handle);
    __HAL_TIM_ENABLE_IT(&g_TIM_Base_SynSlave_Handle, TIM_IT_TRIGGER); // 使能更新中断

    return HAL_OK;

}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_Base_SynMaster_Handle.Instance){
        /* TIMx Peripheral clock enable */
        __TIMx_MASTER_INSTANCE_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIMx_MASTER_INSTANCE_IRQn, TIM_MASTER_IT_PreemptPriority, TIM_MASTER_IT_SubPriority);
        HAL_NVIC_EnableIRQ(TIMx_MASTER_INSTANCE_IRQn);
    }
    else if(htim->Instance == g_TIM_Base_SynSlave_Handle.Instance){
        /* TIMx Peripheral clock enable */
        __TIMx_SLAVE_INSTANCE_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIMx_SLAVE_INSTANCE_IRQn, TIM_SLAVE_IT_PreemptPriority, TIM_SLAVE_IT_SubPriority);
        HAL_NVIC_EnableIRQ(TIMx_SLAVE_INSTANCE_IRQn);
    }
}

void TIMx_MASTER_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_Base_SynMaster_Handle);
}

void TIMx_SLAVE_INSTANCE_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_TIM_Base_SynSlave_Handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == g_TIM_Base_SynMaster_Handle.Instance){
        // 主定时器中断回调
        printf("master timer interrupt callback\r\n");
        printf("slave timer count: %d\r\n", __HAL_TIM_GET_COUNTER(&g_TIM_Base_SynSlave_Handle));
    }
    else if(htim->Instance == g_TIM_Base_SynSlave_Handle.Instance){
        // 从定时器中断回调
        printf("slave timer interrupt callback\r\n");
    }
}

