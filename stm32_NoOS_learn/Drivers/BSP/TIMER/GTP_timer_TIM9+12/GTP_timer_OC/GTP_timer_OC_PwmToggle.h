#ifndef  __TIMER_OC_PWM_TOGGLE_H_
#define  __TIMER_OC_PWM_TOGGLE_H_

#include "main.h"


#define TIMX_OC_PWM_DC_HALF_ENABLE                  0  /* 0: 禁止输出比较PWM占空比为50%, 1: 允许输出比较PWM占空比为50% */
#define TIMX_OC_PWM_IT_DC_HALF_ENABLE               1  /* 0: 禁止输出比较PWM占空比为50%中断, 1: 允许输出比较PWM占空比为50%中断 */

#define TIMX_INT                       TIM1
#define TIMX_INT_IRQn                  TIM1_BRK_TIM9_IRQn
#define TIMX_INT_IRQHandler            TIM1_BRK_TIM9_IRQHandler
#define __TIMX_INT_CLK_ENABLE()        do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 时钟 */


// 对于IC的定义
#define TIM_CH1_Pin                          GPIO_PIN_5
#define TIM_CH1_GPIO_Port                    GPIOE
#define TIM_CH1_GPIO_MODE                    GPIO_MODE_AF_PP
#define TIM_CH1_OC_GPIO_PULL                 GPIO_NOPULL
#define TIM_CH1_GPIO_AF                      GPIO_AF1_TIM1
#define __TIM_CH1_GPIO_CLK_ENABLE()          do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0U)

#define ARR_MAX_VALUE                        0x5000 // ARR的最大值

extern void GTP_text(void);
static void GPIO_CH1_Init(uint8_t pinPull);
extern void __Set_GTP_OC_PwmToggle(uint16_t Period);
extern void GTP_OC_PwmToggle_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE,uint8_t setOCMode);


#endif // __TIMER_OC_PWM_TOGGLE_H_
