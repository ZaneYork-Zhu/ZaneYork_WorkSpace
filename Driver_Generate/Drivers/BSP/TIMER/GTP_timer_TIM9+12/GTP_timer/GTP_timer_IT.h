#ifndef  __GTP_TIMER_H_
#define  __GTP_TIMER_H_

#include "main.h"

extern TIM_HandleTypeDef tim_GTP_handle;

#define GTP_TIMX_POLLING_ENABLE                         0  /* 0: 禁止轮询模式, 1: 允许轮询模式 */
#define GTP_TIMX_INT_IT_ENABLE                          0  /* 0: 禁止中断模式, 1: 允许中断模式 */
#define GTP_TIMX_IC_ENABLE                              0  /* 0: 禁止输入捕获, 1: 允许输入捕获 */
#define GTP_TIMX_IC_IT_ENABLE                           0  /* 0: 禁止输入捕获中断, 1: 允许输入捕获中断 */


#define GTP_TIMX_OC_PWM_DC_HALF_ENABLE                  0  /* 0: 禁止输出比较PWM占空比为50%, 1: 允许输出比较PWM占空比为50% */
#define GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE               1  /* 0: 禁止输出比较PWM占空比为50%中断, 1: 允许输出比较PWM占空比为50%中断 */
#define GTP_TIMX_OC_PWM_DC_CHANGE_ENABLE                0  /* 0: 禁止输出比较PWM占空比可调, 1: 允许输出比较PWM占空比可调 */
#define GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE             0  /* 0: 禁止输出比较PWM占空比可调中断, 1: 允许输出比较PWM占空比可调中断 */
#define GTP_TIMX_OC_FORCE_ENABLE                        0  /* 0: 禁止输出比较强制, 1: 允许输出比较强制 */
#define GTP_TIMX_OC_FORCE_IT_ENABLE                     0  /* 0: 禁止输出比较强制中断, 1: 允许输出比较强制中断 */
#define GTP_TIMX_OC_OPM_ENABLE                          0  /* 0: 禁止单脉冲模式, 1: 允许单脉冲模式 */

#define GTP_TIMX_INT                       TIM9
#define GTP_TIMX_INT_IRQn                  TIM1_BRK_TIM9_IRQn
#define GTP_TIMX_INT_IRQHandler            TIM1_BRK_TIM9_IRQHandler
#define __GTP_TIMX_INT_CLK_ENABLE()        do{ __HAL_RCC_TIM9_CLK_ENABLE(); }while(0)  /* TIM9 时钟 */


// 对于IC的定义
#define GTP_CH1_Pin                          GPIO_PIN_5
#define GPT_CH1_GPIO_Port                    GPIOE
/*这里一定要下拉：PE5作为输入还是要上来拉要不然用不了*/
/*GPIO_InitStruct.Pull 的配置 在输入场景下有效（如输入捕获）*/
#define GPT_CH1_GPIO_MODE                    GPIO_MODE_AF_PP
#define GPT_CH1_IC_GPIO_PULL                 GPIO_PULLDOWN
#define GPT_CH1_OC_GPIO_PULL                 GPIO_NOPULL
#define GPT_CH1_GPIO_AF                      GPIO_AF3_TIM9
#define __GPT_CH1_GPIO_CLK_ENABLE()          do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0U)

static void GPIO_CH1_Init(uint8_t pinPull);
void GTP_IC_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE);
void GTP_OC_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE, uint8_t OCMode);
void GTP_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE);

void GTP_text(void);
#endif
