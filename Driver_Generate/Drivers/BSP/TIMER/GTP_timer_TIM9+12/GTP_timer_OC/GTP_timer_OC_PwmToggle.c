#include "GTP_timer_OC_PwmToggle.h"
#include "led.h"
#include <stdio.h>

TIM_HandleTypeDef g_tim_GTP_handle = {0};
TIM_OC_InitTypeDef g_sConfigOC = {0};

void GTP_text(void)
{
    LED_init();
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL);
    //高电平时间 ARR = CCR时 翻转一次电平
    GTP_OC_TimerInit(GTP_TIMX_INT,10000,16800,0, TIM_OCMODE_TOGGLE); // 输出比较模式设置为翻转模式

    while (1)
    {
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE
      /* 轮询输出比较 */
      if(__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1)){
        /* 清除输出比较标志位 */
        __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1);
        LED1_TOGGLE();
        printf("GTP_TIMX_OC Output Compare Polling !!!\r\n");
      }
      else if (__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE)){
        /* 清除更新事件标志位 */
        __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE);
        LED0_TOGGLE();
        printf("GTP_TIMX_OC Update Event Polling !!!\r\n");
      }
#endif
#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE
      HAL_Delay(1000);  /* 延时1秒 */
#endif
    }
}


/**
 * @brief  GPIO_CH1_Init 初始化GPIO引脚
 * @param  pinPull: GPIO引脚电平状态
 * @retval None
 */
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

/**
 * @brief  OCMode->TIM_OCMODE_TOGGLE 实现PWM的输出
 * @param  TimInstance 定时器基地址
 * @param  arr 自动重装载寄存器值
 * @param  psc 预分频系数
 * @param  setARPE 自动重载预装载使能
 * @note 该函数会初始化定时器的输出比较通道1为翻转模式,并启动定时器和输出比较通道1.
 *       CCR = ARR ->PWM输出电压翻转1次
 *       占空比为：高电平ARR 低电平ARR
 *       例如：ARR=10000, psc=16800, 则PWM周期：2s, 高电平1s, 低电平1s
 * @note 该模式下：g_sConfigOC.pluse 无效
 */
void GTP_OC_PwmToggle_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE)
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
  
  g_sConfigOC.OCMode = TIM_OCMODE_TOGGLE; // 输出比较模式
  //强制输出模式下->pulse值无效
  g_sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出极性为高电平有效
  g_sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; // 快速模式禁用
  /* 初始化输出比较 */
  if (HAL_TIM_OC_Init(&g_tim_GTP_handle) != HAL_OK)
  {
    Error_Handler();
  }
  /* 配置输出PWM的通道 */
  if (HAL_TIM_OC_ConfigChannel(&g_tim_GTP_handle, &g_sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE
  // 启动输出比较通道
  HAL_TIM_OC_Start(&g_tim_GTP_handle, TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&g_tim_GTP_handle); // 启动定时器基础功能
#endif

#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE
  // 启动输出比较中断
  HAL_TIM_OC_Start_IT(&g_tim_GTP_handle, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&g_tim_GTP_handle); // 启动定时器基础功能中断
#endif
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if((tim_baseHandle->Instance==TIM9) || (tim_baseHandle->Instance==TIM12) != 0)
  {

    // TIM9 clock enable
    __GTP_TIMX_INT_CLK_ENABLE();
#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE
    /* TIM9 interrupt Init */
    HAL_NVIC_SetPriority(GTP_TIMX_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GTP_TIMX_INT_IRQn);
#endif /* GTP_TIMX_INT_IT_ENABLE */

  }
}

#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE
/**
  * @brief This function handles TIM3 global interrupt.
  */
void GTP_TIMX_INT_IRQHandler(void)
{
  /*服务函数会自动清除IT*/
  HAL_TIM_IRQHandler(&g_tim_GTP_handle);
  /*这里手动再清理一次*/
  __HAL_TIM_CLEAR_IT(&g_tim_GTP_handle, TIM_IT_UPDATE); // 清除TIM9/TIM12的更新中断标志位
  __HAL_TIM_CLEAR_IT(&g_tim_GTP_handle, TIM_IT_CC1); // 清除TIM9/TIM12的通道1比较中断标志位
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

// 输出比较回调函数
// 该函数在输出比较事件发生时被调用
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == GTP_TIMX_INT) {
    LED1_TOGGLE(); // 切换LED1状态
    printf("GTP_TIMX_INT OC !!!!\r\n");
  }
}
#endif /* GTP_TIMX_INT_IT_ENABLE */


