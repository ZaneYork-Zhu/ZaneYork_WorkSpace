#include "GTP_pulse_count.h"
#include "debug_com.h"
#include "led.h"

/* USER CODE BEGIN 0 */
/* ��¼��ʱ�����������������, ���������������� */
uint32_t g_timxchy_cnt_ofcnt = 0 ;              /* ����������� */
/* USER CODE END 0 */

TIM_HandleTypeDef GTP_timPulseCountHandle;

  uint32_t curcnt = 0;
  uint32_t oldcnt = 0;
  uint8_t key = 0;
  uint8_t t = 0;
  
void  GTP_pulse_text_init(void)
{
    led_init();
    GTP_PULSE_GPIO_Init();
    UART_Init(115200);
    printf("hello world!!\r\n");
    GTP_pulse_init(GTP_TIMX_PULSE,50,1);
    /*����������*/
    TIM_cnt_chy_restart();
    
}

void  GTP_pulse_text_while(void)
{

while (1)
  {
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */
    key = key_scan(0);
    if(KEY0_PRES == key)
    {
      /*���¼���*/
      TIM_cnt_chy_restart();
    }

    /*��ȡ��ǰ���벶�����ֵ*/
    curcnt = TIM_cnt_chy_get_count();
    if(oldcnt != curcnt)
    {
      oldcnt = curcnt;
      printf("CNT:%d\r\n",oldcnt);
    }
    t++;
    if(t>20)
    {
      t = 0;
      LED0_TOGGLE();
    }
    HAL_Delay(10);
  }
}


void GTP_PULSE_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : KEY2_Pin KEY1_Pin KEY0_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin|KEY1_Pin|KEY0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);


}


/* TIM2 init function */
void GTP_pulse_init(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  GTP_timPulseCountHandle.Instance = TIM;
  GTP_timPulseCountHandle.Init.Prescaler = psc-1;
  GTP_timPulseCountHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  GTP_timPulseCountHandle.Init.Period = arr-1;
  GTP_timPulseCountHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  GTP_timPulseCountHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&GTP_timPulseCountHandle) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&GTP_timPulseCountHandle, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&GTP_timPulseCountHandle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* ʹ��ͨ������ */
  HAL_TIM_IC_Start(&GTP_timPulseCountHandle, TIM_CHANNEL_1);
  /* ʹ�ܸ����ж� */
  __HAL_TIM_ENABLE_IT(&GTP_timPulseCountHandle,TIM_IT_UPDATE);
  /* USER CODE END TIM2_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_baseHandle->Instance==GTP_TIMX_PULSE)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    GTP_TIMX_PULSE_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA0-WKUP     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = WK_UP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(WK_UP_GPIO_Port, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(GTP_TIMX_PULSE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GTP_TIMX_PULSE_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==GTP_TIMX_PULSE)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA0-WKUP     ------> TIM2_CH1
    */
    HAL_GPIO_DeInit(WK_UP_GPIO_Port, WK_UP_Pin);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void GTP_TIMX_PULSE_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&GTP_timPulseCountHandle);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}


/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(GTP_TIMX_PULSE == htim->Instance)
  {
    /*�ۼƼ������������*/
    g_timxchy_cnt_ofcnt++;
    /*��ʱ�����ָʾ��*/
    HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);
  }

}

uint32_t TIM_cnt_chy_get_count(void)
{
    uint32_t count = 0;
    /*������������ �����ļ���*/
    count = g_timxchy_cnt_ofcnt*50;
    count += __HAL_TIM_GET_COUNTER(&GTP_timPulseCountHandle);
    return count;
}

void TIM_cnt_chy_restart(void)
{
    __HAL_TIM_DISABLE(&GTP_timPulseCountHandle); /* �رն�ʱ�� TIMX */
    g_timxchy_cnt_ofcnt = 0; /* �ۼ������� */
    __HAL_TIM_SET_COUNTER(&GTP_timPulseCountHandle, 0); /* ���������� */
    __HAL_TIM_ENABLE(&GTP_timPulseCountHandle); /* ʹ�ܶ�ʱ�� TIMX */
}

/*��ֵ��ȡ����*/
uint8_t key_scan(uint8_t mode)
{
  /* �����ɿ���־*/
  static uint8_t key_up = 1;
  /* ��ֵ*/
  uint8_t keyval = 0;

  if(mode)        /*������Ƿ��ɿ�*/
  {
    key_up = 1;
  }
  if(key_up&&(KEY0 == 0||KEY1 == 0|| KEY2 == 0||WK_UP == 1))
  {
    HAL_Delay(10);  /*������*/
    key_up = 0;     /*��������*/
    if (KEY0 == 0)
    {
      keyval = KEY0_PRES;
    }
    if (KEY1 == 0)
    {
      keyval = KEY1_PRES;
    }
    if (KEY2 == 0)
    {
      keyval = KEY2_PRES;
    }
    if (WK_UP == 1)
    {
      keyval = WK_UP_PRES;
    }
  }
  else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)
  {
    key_up = 1;
  }
  return keyval;
  
}

/* USER CODE END 1 */
