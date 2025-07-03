#include "GTP_timer_IT.h"
#include "led.h"

TIM_HandleTypeDef g_tim_GTP_handle = {0};
TIM_IC_InitTypeDef g_sConfigIC = {0};
TIM_OC_InitTypeDef g_sConfigOC = {0};
TIM_SlaveConfigTypeDef g_sSlaveConfig = {0};

void GTP_text(void)
{
    LED_init();
#if GTP_TIMX_POLLING_ENABLE || GTP_TIMX_INT_IT_ENABLE
    GTP_TimerInit(GTP_TIMX_INT,10000,16800,0);
#endif
#if GTP_TIMX_IC_ENABLE || GTP_TIMX_IC_IT_ENABLE
    GPIO_CH1_Init(GPT_CH1_IC_GPIO_PULL);
    /* 1s溢出->更新事件*/
    GTP_IC_TimerInit(GTP_TIMX_INT,10000,16800,0);
#endif
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE || GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL);
    // 输出占空比为50%
    GTP_OC_TimerInit(GTP_TIMX_INT,10000,16800,0, TIM_OCMODE_TOGGLE); // 输出比较模式设置为翻转模式
#endif  
#if GTP_TIMX_OC_PWM_DC_CHANGE_ENABLE || GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL);
    // 输出占空比可调
    GTP_OC_TimerInit(GTP_TIMX_INT,10000,16800,0,TIM_OCMODE_PWM1); // 输出比较模式设置为PWM模式
    // 设置通道1的比较值为2000/10000 = 20%占空比
    __HAL_TIM_SET_COMPARE(&g_tim_GTP_handle, TIM_CHANNEL_1, 2000); // 设置通道1的比较值为2000
#endif  
#if GTP_TIMX_OC_FORCE_ENABLE || GTP_TIMX_OC_FORCE_IT_ENABLE
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL);
    GTP_OC_TimerInit(GTP_TIMX_INT,10000,16800,0, TIM_OCMODE_FORCED_ACTIVE); // 输出比较模式设置为强制激活模式
#endif
#if GTP_TIMX_OC_OPM_ENABLE || GTP_TIMX_OC_OPM_IT_ENABLE
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL);
    GTP_OC_TimerInit(GTP_TIMX_INT,10000,16800,0, TIM_OCMODE_PWM1); // 输出比较模式设置为PWM模式
#endif

    while (1)
    {
#if GTP_TIMX_POLLING_ENABLE
      while(__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE)){
        __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE);
        LED1_TOGGLE();
        printf("GTP_TIMX_INT Polling !!!\r\n");
      }
#endif
#if GTP_TIMX_IC_ENABLE
      /* 轮询输入捕获 */
      if(__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1)){
        /* 清除输入捕获标志位 */
        __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1);
        LED1_TOGGLE();
        printf("GTP_TIMX_IC Input Capture Polling !!!\r\n");
      }
#endif
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE  || GTP_TIMX_OC_FORCE_ENABLE  || GTP_TIMX_OC_OPM_ENABLE || GTP_TIMX_OC_PWM_DC_CHANGE_ENABLE
      /* 轮询输出比较 */
      if(__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1)){
        /* 清除输出比较标志位 */
        __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1);
        LED1_TOGGLE();
        printf("GTP_TIMX_OC Output Compare Polling !!!\r\n");
      }
#endif
#if GTP_TIMX_INT_IT_ENABLE || GTP_TIMX_IC_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE || GTP_TIMX_OC_FORCE_IT_ENABLE || GTP_TIMX_OC_OPM_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE
      HAL_Delay(1000);  /* 延时1秒 */
#endif
    }
}




void GTP_OC_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE, uint8_t OCMode)
{
  // 初始化输出比较
  g_tim_GTP_handle.Instance = TimInstance;
  g_tim_GTP_handle.Init.Prescaler = psc-1; // 预分频系数
  g_tim_GTP_handle.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数
  g_tim_GTP_handle.Init.Period = arr-1; // 自动重装载寄存器
  g_tim_GTP_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频系数
  if(setARPE){g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* 自动重载预装载使能 */}
  else{g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* 自动重载预装载禁止 */ }

  /* 初始化定时器 */
  if(HAL_TIM_Base_Init(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
  
  g_sConfigOC.OCMode = OCMode; // 输出比较模式
  //强制输出模式下->pulse值无效
  g_sConfigOC.Pulse = arr/2; // 设置脉冲宽度为自动重装载值的一半
  g_sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出极性为高电平有效
  g_sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; // 快速模式禁用
#if GTP_TIMX_OC_OPM_ENABLE
  // 初始化单脉冲模式
  if(HAL_TIM_OnePulse_Init(&g_tim_GTP_handle, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  g_sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER; // 从模式选择为触发模式
  g_sSlaveConfig.InputTrigger = TIM_TS_ITR0; //触发源为内部触发器0
  g_sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING; // 触发极性为上升沿
  g_sSlaveConfig.TriggerFilter = 0; // 触发滤波器为0
  if (HAL_TIM_SlaveConfigSynchro(&g_tim_GTP_handle, &g_sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
#else
  /* 初始化输出比较 */
  if (HAL_TIM_OC_Init(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
#endif
  if (HAL_TIM_OC_ConfigChannel(&g_tim_GTP_handle, &g_sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  // TIM9/TIM12只能向上计数,所以不需要设置计数
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE || GTP_TIMX_OC_FORCE_ENABLE || GTP_TIMX_OC_OPM_ENABLE || GTP_TIMX_OC_PWM_DC_CHANGE_ENABLE 
  // 启动输出比较通道
  HAL_TIM_OC_Start(&g_tim_GTP_handle, TIM_CHANNEL_1);
#endif

#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE || GTP_TIMX_OC_FORCE_IT_ENABLE || GTP_TIMX_OC_OPM_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE 
  // 启动输出比较中断
  HAL_TIM_OC_Start_IT(&g_tim_GTP_handle, TIM_CHANNEL_1);
#endif
}

void GTP_IC_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE)
{
  // 初始化输入捕获
  g_tim_GTP_handle.Instance = TimInstance;
  g_tim_GTP_handle.Init.Prescaler = psc-1; // 预分频系数
  /*通用定时器TIM9/TIM12只能递增计数*/
  g_tim_GTP_handle.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数
  g_tim_GTP_handle.Init.Period = arr-1; // 自动重装载寄存器
  /*TIM_CLOCKDIVISION_DIV1->不分频*/
  g_tim_GTP_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频系数
  if(setARPE){g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* 自动重载预装载使能 */}
  else{g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* 自动重载预装载禁止 */ }
  /* 初始化定时器 */
  if(HAL_TIM_Base_Init(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
  /* 初始化输入捕获 */
  if (HAL_TIM_IC_Init(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
  /*捕获上升沿信号*/
  g_sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; // 输入捕获极性为上升沿
  g_sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; // 输入捕获选择为直接输入
  g_sConfigIC.ICPrescaler = TIM_ICPSC_DIV1; // 输入捕获预分频系数为1
  g_sConfigIC.ICFilter = 0; // 输入捕获滤波器为0
  if (HAL_TIM_IC_ConfigChannel(&g_tim_GTP_handle, &g_sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  // TIM9/TIM12只能向上计数,所以不需要设置计数
#if GTP_TIMX_IC_ENABLE
  // 启动输入捕获通道
  HAL_TIM_IC_Start(&g_tim_GTP_handle, TIM_CHANNEL_1);
#endif
#if GTP_TIMX_IC_IT_ENABLE
  // 启动输入捕获中断
  HAL_TIM_IC_Start_IT(&g_tim_GTP_handle, TIM_CHANNEL_1);
#endif
}

/**
 * @brief  通用定时器TIM9/TIM12定时中断初始化函数
 * @note   stm32F407->TIM9/TIM12是通用定时器
 *         通用定时器的时钟来自APB2,当PPRE2 ≥ 2分频的时候
 *         通用定时器的时钟为APB2时钟的2倍, 而APB2为84M, 所以定时器时钟 = 168Mhz
 *         定时器溢出时间计算方法: Tout = (arr*psc) / Ft us.
 *         Ft=定时器工作频率,单位:Mhz
 * @param  TimInstance: 定时器实例
 * @param  arr: 自动重装值
 * @param  psc: 预分频系数
 * @retval 无
 */

void  GTP_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE)
{


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  // 判断TIM是否为TIM9或TIM12
  g_tim_GTP_handle.Instance = TimInstance;
  // PSC->预分频系数->Note：TIM9/TIM12的时钟是168MHz
  g_tim_GTP_handle.Init.Prescaler = psc-1;
  // TIM9/TIM12->只能向上计数
  g_tim_GTP_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  // ARR->自动重装载寄存器
  // TIM9/TIM12->ARR的值范围为0~65535
  g_tim_GTP_handle.Init.Period = arr-1;
  // TIM9/TIM12->预装载寄存器ARPE = 0
  if (setARPE) {g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;}
  else {g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;}

  if (HAL_TIM_Base_Init(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&g_tim_GTP_handle, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&g_tim_GTP_handle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
#if GTP_TIMX_POLLING_ENABLE
  if (HAL_TIM_Base_Start(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
#endif
#if GTP_TIMX_INT_IT_ENABLE
  HAL_TIM_Base_Start_IT(&g_tim_GTP_handle);
#endif

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if((tim_baseHandle->Instance==TIM9) || (tim_baseHandle->Instance==TIM12) != 0)
  {

    // TIM9 clock enable
    __GTP_TIMX_INT_CLK_ENABLE();
#if GTP_TIMX_INT_IT_ENABLE || GTP_TIMX_IC_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE || GTP_TIMX_OC_FORCE_IT_ENABLE || GTP_TIMX_OC_OPM_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE
    /* TIM9 interrupt Init */
    HAL_NVIC_SetPriority(GTP_TIMX_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GTP_TIMX_INT_IRQn);
#endif /* GTP_TIMX_INT_IT_ENABLE */

  }
}

#if GTP_TIMX_INT_IT_ENABLE || GTP_TIMX_IC_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE || GTP_TIMX_OC_FORCE_IT_ENABLE || GTP_TIMX_OC_OPM_IT_ENABLE || GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE
/**
  * @brief This function handles TIM3 global interrupt.
  */
void GTP_TIMX_INT_IRQHandler(void)
{
  /*服务函数会自动清除IT*/
  HAL_TIM_IRQHandler(&g_tim_GTP_handle);
  /*这里手动再清理一次*/
  __HAL_TIM_CLEAR_IT(&g_tim_GTP_handle, TIM_IT_UPDATE); // 清除TIM9/TIM12的更新中断标志位


}

#endif /* GTP_TIMX_INT_IT_ENABLE */

/*******************************************************公共部分******************************************* */
static void GPIO_CH1_Init(uint8_t pinPull)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __GPT_CH1_GPIO_CLK_ENABLE(); // 使能GPIO时钟
  GPIO_InitStruct.Pin = GTP_CH1_Pin; // 设置引脚
  GPIO_InitStruct.Mode = GPT_CH1_GPIO_MODE; // 设置模式
  GPIO_InitStruct.Pull = pinPull; // 设置上拉/下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 设置速度
  GPIO_InitStruct.Alternate = GPT_CH1_GPIO_AF; // 设置复用功能
  HAL_GPIO_Init(GPT_CH1_GPIO_Port, &GPIO_InitStruct); // 初始化GPIO
}
// 定时器溢出回调函数
// 该函数在定时器溢出事件发生时被调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == GTP_TIMX_INT) {
    LED0_TOGGLE(); // 切换LED0状态
    printf("GTP generate event !!\r\n");
  }
}
// 输入捕获回调函数
// 该函数在输入捕获事件发生时被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == GTP_TIMX_INT) {
    LED1_TOGGLE(); // 切换LED1状态
    printf("GTP_TIMX_INT IC !!!!\r\n");
  }
}
// 输出比较回调函数
// 该函数在输出比较事件发生时被调用
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == GTP_TIMX_INT) {
    LED1_TOGGLE(); // 切换LED1状态
    printf("GTP_TIMX_INT OC !!!!\r\n");
  }
}
