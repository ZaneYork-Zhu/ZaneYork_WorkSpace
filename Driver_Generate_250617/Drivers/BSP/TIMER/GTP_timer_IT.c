#include "GTP_timer_IT.h"
#include "led.h"

TIM_HandleTypeDef tim_GTP_handle = {0};

void GTP_text_init(void)
{

    led_init();
    GTP_TimerItInit(GTP_TIMX_INT,8400,5000);
}

void GTP_text_while(void)
{
    LED0_TOGGLE();
    HAL_Delay(500);
}

/* TIM3 init function */
void  GTP_TimerItInit(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc)
{


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  tim_GTP_handle.Instance = GTP_TIMX_INT;
  tim_GTP_handle.Init.Prescaler = psc-1;
  tim_GTP_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim_GTP_handle.Init.Period = arr-1;
  tim_GTP_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tim_GTP_handle, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim_GTP_handle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  HAL_TIM_Base_Start_IT(&tim_GTP_handle);
  /* USER CODE END TIM3_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {

    GTP_TIMX_INT_CLK_ENABLE();


    HAL_NVIC_SetPriority(GTP_TIMX_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GTP_TIMX_INT_IRQn);

  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {

    __HAL_RCC_TIM3_CLK_DISABLE();

    HAL_NVIC_DisableIRQ(GTP_TIMX_INT_IRQn);
  }
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void GTP_TIMX_INT_IRQHandler(void)
{

  HAL_TIM_IRQHandler(&tim_GTP_handle);

}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == GTP_TIMX_INT) { // 判断是否为TIM3触发中断 
    HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10); // 示例：翻转PA0引脚状态 
  }
}
/* USER CODE END 1 */
