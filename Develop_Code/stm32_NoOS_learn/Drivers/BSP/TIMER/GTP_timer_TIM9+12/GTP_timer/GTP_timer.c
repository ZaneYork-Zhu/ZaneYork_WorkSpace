#include "GTP_timer.h"
#include "led.h"
#include <stdio.h>

TIM_HandleTypeDef g_tim_GTP_handle = {0};
void GTP_text(void)
{
    LED_init();
    uartInit(USART1,115200); // 初始化串口1,波特率115200
    printf("GTP_TIMX_INT <<<<Update>>>> test start !!!\r\n");
    GTP_TimerInit(GTP_TIMX_INT,10000,16800,0);
    while (1)
    {
#if GTP_TIMX_POLLING_ENABLE
      // 轮询定时器更新事件
      while(__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE)){
        __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE);
        LED1_TOGGLE();
        printf("GTP_TIMX_INT Polling !!!\r\n");
      }
#endif
#if GTP_TIMX_INT_IT_ENABLE
      HAL_Delay(1000);  /* 延时1秒 */
#endif
    }
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
#if GTP_TIMX_POLLING_ENABLE
  HAL_TIM_Base_Start(&g_tim_GTP_handle); // 启动定时器基础功能
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
#if GTP_TIMX_INT_IT_ENABLE 
    HAL_NVIC_SetPriority(GTP_TIMX_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GTP_TIMX_INT_IRQn);
#endif /* GTP_TIMX_INT_IT_ENABLE */

  }
}

#if GTP_TIMX_INT_IT_ENABLE 
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

// 定时器溢出回调函数
// 该函数在定时器溢出事件发生时被调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == GTP_TIMX_INT) {
    LED0_TOGGLE(); // 切换LED0状态
    printf("GTP generate event !!\r\n");
  }
}
#endif /* GTP_TIMX_INT_IT_ENABLE */


