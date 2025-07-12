#include "TIM_IC_Mode.h"
#include "debug_com.h"
#include "led.h"
Tim_State g_timxchy_cap_sta ={0};
uint16_t g_timxchy_cap_val = 0; /* 输入捕获值 */  

/* USER CODE END 0 */

TIM_HandleTypeDef GPT_IC_Handle;


uint32_t timESum = 0;
uint8_t t = 0;

void GTP_IC_text_init(void)
{

    UART_Init(115200);
    led_init();
    gpio_init();
    GTP_TimerICInit(GTP_TIMX_IC_INT,0x10000-1,84-1);
    printf("hello world!! \r\n");
}


void GTP_IC_text_while(void)
{

    while (1)
    {
         /*捕获成功*/
        if(1 == g_timxchy_cap_sta.finish)
        {
          /*溢出次数*/
          timESum = g_timxchy_cap_sta.data;
          /*溢出时间总和*/
          timESum *= 65536;
          /*高电平脉冲宽度时间*/
          timESum += g_timxchy_cap_val;
          /* 打印总的高电平时间 */
          printf("高电平溢出次数       HIGH.data:%d \r\n", g_timxchy_cap_sta.data); 
          printf("未溢出脉冲宽度时间   HIGH.value:%d us\r\n", g_timxchy_cap_val);
          printf("总高电平脉冲宽度时间 %d = value * 65536 + data \r\n", timESum); 
          /*准备下一次捕获*/ 
          g_timxchy_cap_sta.finish  = 0;
          g_timxchy_cap_sta.state  = 0;
          g_timxchy_cap_sta.data  = 0;

        }

          LED0_TOGGLE();
          HAL_Delay(100);
    }
}




void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
      
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM5 GPIO Configuration
    PA0-WKUP     ------> TIM5_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
	/*配置为输出的原因---复用给TIM5作为输入捕获*/
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    /*这里一定要下拉：PA0作为输入还是要上来拉要不然用不了*/
	/*GPIO_InitStruct.Pull 的配置 在输入场景下有效（如输入捕获）*/
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}



/* TIM5 init function */
void  GTP_TimerICInit(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  GPT_IC_Handle.Instance = TIM;
  GPT_IC_Handle.Init.Prescaler = psc;
  GPT_IC_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  GPT_IC_Handle.Init.Period = arr;
  GPT_IC_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&GPT_IC_Handle) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&GPT_IC_Handle, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&GPT_IC_Handle) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&GPT_IC_Handle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&GPT_IC_Handle, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */
  /* 启动更新中断 + 输入捕获中断【同时开启两个】 */
  __HAL_TIM_ENABLE_IT(&GPT_IC_Handle, TIM_IT_UPDATE); 
  HAL_TIM_IC_Start_IT(&GPT_IC_Handle, TIM_CHANNEL_1);
  /* USER CODE END TIM5_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{


  if(tim_baseHandle->Instance==GTP_TIMX_IC_INT)
  {
  /* USER CODE BEGIN TIM5_MspInit 0 */

  /* USER CODE END TIM5_MspInit 0 */
    /* TIM5 clock enable */
    GTP_TIMX_IC_INT_CLK_ENABLE();

    /* TIM5 interrupt Init */
    HAL_NVIC_SetPriority(GTP_TIMX_IC_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GTP_TIMX_IC_INT_IRQn);
  /* USER CODE BEGIN TIM5_MspInit 1 */

  /* USER CODE END TIM5_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==GTP_TIMX_IC_INT)
  {
  /* USER CODE BEGIN TIM5_MspDeInit 0 */

  /* USER CODE END TIM5_MspDeInit 0 */
    /* Peripheral clock disable */
    GTP_TIMX_IC_INT_CLK_ENABLE();

    /**TIM5 GPIO Configuration
    PA0-WKUP     ------> TIM5_CH1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

    /* TIM5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM5_IRQn);
  /* USER CODE BEGIN TIM5_MspDeInit 1 */

  /* USER CODE END TIM5_MspDeInit 1 */
  }
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void GTP_TIMX_IC_INT_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */

  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&GPT_IC_Handle);
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/*输入捕获中断*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  /*finish = 0：还没有捕获成功*/
  if (0 == g_timxchy_cap_sta.finish)
  {
    /*state = 1:表示已经捕获过上升沿-->这次触发为下降沿触发中断----计算脉冲捕获值*/
      if(1 == g_timxchy_cap_sta.state)
      {
        /*标记捕获成功*/
          g_timxchy_cap_sta.finish = 1;
        /*获取捕获值 */
          g_timxchy_cap_val = HAL_TIM_ReadCapturedValue(&GPT_IC_Handle, TIM_CHANNEL_1);
        /*复位输入捕获极性 */
        TIM_RESET_CAPTUREPOLARITY(&GPT_IC_Handle, TIM_CHANNEL_1);
        /*重新设置为上升沿--准备下一次捕获*/
        TIM_SET_CAPTUREPOLARITY(&GPT_IC_Handle, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
      }
    /*state = 0:表示没有捕获过上升沿-->这次是第一次上升沿触发中断--->等待第一次捕获上升沿*/
      else            
      {
        
        /*重置溢出次数值*/
        g_timxchy_cap_sta.data = 0;
        /*重置捕获值【准备开始】*/
        g_timxchy_cap_val = 0;
        /*记录第一次上升沿*/
        g_timxchy_cap_sta.state = 1;

        /*重新配置部分***********************************/
        /*关闭定时器5*/
        __HAL_TIM_DISABLE(&GPT_IC_Handle);
        /*定时器5 初始化 ：计数值 = 0*/
        __HAL_TIM_SET_COUNTER(&GPT_IC_Handle,0);
        /*复位输入捕获极性 */
        TIM_RESET_CAPTUREPOLARITY(&GPT_IC_Handle, TIM_CHANNEL_1);
        /*重新设置为下升沿--等待下降沿触发进入if*/
        TIM_SET_CAPTUREPOLARITY(&GPT_IC_Handle, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
         /*开始定时器5----开始计数*/
        __HAL_TIM_ENABLE(&GPT_IC_Handle);
        /*重新配置部分***********************************/    
      }
  }

    
}

/*溢出更新中断*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /*定时器公共服务函数---这里只处理TIM5溢出中断*/
  if(GTP_TIMX_IC_INT == htim->Instance)
  {
    /*定时器已经溢出了 还没捕获成功*/
    if(0 == g_timxchy_cap_sta.finish)
    {
      /*已经捕获到上升沿了*/
      if(1 == g_timxchy_cap_sta.state)
      {
        /*最大溢出次数---一直是高电平并且溢出次数到达上限*/
        if(0x3F == g_timxchy_cap_sta.data)
        {
          /*复位输入捕获【此时是等待下降沿捕获】极性 */
          TIM_RESET_CAPTUREPOLARITY(&GPT_IC_Handle, TIM_CHANNEL_1);
          
         /*就强制标记捕获完成，并配置定时器通道上升沿捕获*/
          g_timxchy_cap_sta.finish = 1;
          /*读取脉冲宽度时 =  为最大值*/
          g_timxchy_cap_val = 0XFFFF;
          
          /*设置输入捕获【此时是等待上升沿捕获】极性，重新开始捕获 */
          TIM_SET_CAPTUREPOLARITY(&GPT_IC_Handle, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        /*记录溢出次数*/
        else
        {
          g_timxchy_cap_sta.data++;
        }
      }
    }
  }
}
/* USER CODE END 1 */
