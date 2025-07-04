/** 
 * @file    base_timer.c 
 * @author  [frank] 
 * @date    [2025年6月26日21:36:14] 
 * @brief   该文件实现了通用定时器TIMX定时中断的初始化功能。 
 *          适用于STM32F407系列，其中TIM6/TIM7为基本定时器。 
 *          通用定时器时钟来自APB1，当PPRE1 ≥ 2分频时，通用定时器时钟为APB1时钟的2倍（84MHz）。 
 *          定时器溢出时间可通过公式Tout = ((arr + 1) * (psc + 1)) / Ft us计算，Ft为定时器工作频率（单位：MHz）。 
 * 
 * @brief   This file implements the initialization function of the general timer TIMX timing interrupt. 
 *          It is applicable to the STM32F407 series, where TIM6/TIM7 are basic timers. 
 *          The clock of the general timer comes from APB1. When PPRE1 ≥ 2 division, 
 *          the clock of the general timer is twice the APB1 clock (84MHz). 
 *          The timer overflow time can be calculated by the formula Tout = ((arr + 1) * (psc + 1)) / Ft us, 
 *          where Ft is the timer operating frequency (unit: MHz). 
 */
#include "base_timer.h"
#include "led.h"

TIM_HandleTypeDef gTim_base_handle = {0};

void BaseTimer_Test(void){
  LED_init();
  uartInit(USART1,115200);
  printf("BaseTimer_Test_Init\r\n");
  /*实现2s钟溢出一次*/
  baseTimer_SetARPE_Init(BASE_TIMX_INT,20000,8400,0);
  while (1)
  {
#if BASE_TIMER_POLL_ENABLE
          /*检测溢出更新事件*/
    if(__HAL_TIM_GET_FLAG(&gTim_base_handle, TIM_FLAG_UPDATE)){
      /*清除更新事件*/
      __HAL_TIM_CLEAR_FLAG(&gTim_base_handle, TIM_FLAG_UPDATE);
      /*闪烁LED*/
      LED0_TOGGLE();
      printf("BaseTimer_Test_While\r\n");
    }
#endif /* BASE_TIMER_POLL_ENABLE */
#if BASE_TIMER_IT_ENABLE
    HAL_Delay(1000);  /* 延时1秒 */
#endif
  }
}
    

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @brief       stm32F407->TIM6/TIM7是基本定时器
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = (arr*psc) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 * @param       tim_baseHandle: 定时器
 * @param       arr: 自动重装值
 * @param       psc: 预分频系数
 * @retval      无
 */
void  baseTimer_SetARPE_Init(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc ,uint8_t setARPE)
{
    /* 通用定时器x */
    gTim_base_handle.Instance = TimInstance;
    /* TIMx_PSC->预分频系数 */
    gTim_base_handle.Init.Prescaler =psc -1;
    /* TIMx_CNT->递增计数模式 */
    /* 基本定时器->只有递增计数功能 */
    gTim_base_handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
    /* TIMx_ARR->自动装载值 */
    gTim_base_handle.Init.Period = arr-1;
        HAL_TIM_Base_Init(&gTim_base_handle);
    
    HAL_TIM_Base_Start(&gTim_base_handle);                       /* 使能定时器x和定时器更新中断 */
//     {
//       /* 自动装载使能选择 */ 
//       if(setARPE){gTim_base_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;}
//       else{gTim_base_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;}
//     }
//     /* 初始化定时器 */
//     if (HAL_TIM_Base_Init(&gTim_base_handle) != HAL_OK){
//       Error_Handler();
//     }
//     /*用于配置定时器（TIM）主模式（Master Mode） */
//     TIM_MasterConfigTypeDef sMasterConfig = {0};
//     sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//     sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//     if (HAL_TIMEx_MasterConfigSynchronization(&gTim_base_handle, &sMasterConfig) != HAL_OK){
//       Error_Handler();
//     }
// #if BASE_TIMER_POLL_ENABLE
//     /* 使能定时器x */
//     if (HAL_TIM_Base_Start(&gTim_base_handle) != HAL_OK){
//       Error_Handler();
//     }
// #endif
// #if BASE_TIMER_IT_ENABLE
//     /* 使能定时器x和定时器x更新中断 */
//     HAL_TIM_Base_Start_IT(&gTim_base_handle);
// #endif
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==BASE_TIMX_INT)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* 使能TIMx时钟 */
    __BASE_TIMX_INT_CLK_ENABLE();
#if BASE_TIMER_IT_ENABLE
    // /* 设置中断优先级，抢占优先级，子优先级 */
    HAL_NVIC_SetPriority(BASE_TIMX_INT_IRQn, 3, 0);
    /* 开启ITMx中断 */
    HAL_NVIC_EnableIRQ(BASE_TIMX_INT_IRQn);
#endif

  /* USER CODE END TIM6_MspInit 1 */
  }
}

#if BASE_TIMER_IT_ENABLE
/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void BASE_TIMX_INT_IRQHandler(void)
{
    /*服务函数会自动清除IT*/
    HAL_TIM_IRQHandler(&gTim_base_handle);
    /*这里手动再清理一次*/
    __HAL_TIM_CLEAR_IT(&gTim_base_handle, TIM_IT_UPDATE);  /* 清除定时器溢出中断标志位 */
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==BASE_TIMX_INT)
  {
    LED1_TOGGLE();
    printf("BaseTimer_Test_IT\r\n");
  }

}

#endif /* BASE_TIMER_IT_ENABLE */ 
