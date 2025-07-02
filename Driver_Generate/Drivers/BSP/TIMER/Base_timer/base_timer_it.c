#include "base_timer_it.h"
#include "led.h"

TIM_HandleTypeDef tim_base_handle = {0};


void base_text_init(void)
{

    led_init();
    baseTimerItInit(BASE_TIMX_INT,5000,8400);
}

void base_text_while(void)
{
    LED0_TOGGLE();
    HAL_Delay(200);
}

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 * @param       tim_baseHandle: 定时器
 * @param       arr: 自动重装值
 * @param       psc: 预分频系数
 * @retval      无
 */
void  baseTimerItInit(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc)
{
    

    /* 通用定时器x */
    tim_base_handle.Instance = TIM;
    /* 预分频系数 */
    tim_base_handle.Init.Prescaler =psc -1;
     /* 递增计数模式 */
    tim_base_handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
    /* 自动装载值 */
    tim_base_handle.Init.Period = arr-1;
    /* auto影子寄存器*/
    tim_base_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    /* 初始化定时器 */
    if (HAL_TIM_Base_Init(&tim_base_handle) != HAL_OK)
    {
    Error_Handler();
    }
    /*用于配置定时器（TIM）主模式（Master Mode） */
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&tim_base_handle, &sMasterConfig) != HAL_OK)
    {
    Error_Handler();
    }
    /* USER CODE BEGIN TIM6_Init 2 */
    
    /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_Base_Start_IT(&tim_base_handle);
    /* USER CODE END TIM6_Init 2 */
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==BASE_TIMX_INT)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* 使能TIMx时钟 */
    BASE_TIMX_INT_CLK_ENABLE();

    /* 设置中断优先级，抢占优先级，子优先级 */
    HAL_NVIC_SetPriority(BASE_TIMX_INT_IRQn, 0, 0);
    /* 开启ITMx中断 */
    HAL_NVIC_EnableIRQ(BASE_TIMX_INT_IRQn);
  /* USER CODE BEGIN TIM6_MspInit 1 */

  /* USER CODE END TIM6_MspInit 1 */
  }
}
/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void BASE_TIMX_INT_IRQHandler(void)
{
    /*服务函数会自动清除IT*/
    HAL_TIM_IRQHandler(&tim_base_handle);
    /*这里手动再清理一次*/
    __HAL_TIM_CLEAR_IT(&tim_base_handle, TIM_IT_UPDATE);  /* 清除定时器溢出中断标志位 */
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==BASE_TIMX_INT)
  {
    LED1_TOGGLE();
    
  }

}
