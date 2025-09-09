#ifndef  __GTP_TIMER_H_
#define  __GTP_TIMER_H_

#include "main.h"

extern TIM_HandleTypeDef tim_GTP_handle;

#define GTP_TIMX_POLLING_ENABLE                         1  /* 0: 禁止轮询模式, 1: 允许轮询模式 */
#define GTP_TIMX_INT_IT_ENABLE                          0  /* 0: 禁止中断模式, 1: 允许中断模式 */

#define GTP_TIMX_INT                       TIM9
#define GTP_TIMX_INT_IRQn                  TIM1_BRK_TIM9_IRQn
#define GTP_TIMX_INT_IRQHandler            TIM1_BRK_TIM9_IRQHandler
#define __GTP_TIMX_INT_CLK_ENABLE()        do{ __HAL_RCC_TIM9_CLK_ENABLE(); }while(0)  /* TIM9 时钟 */


void GTP_TimerInit(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc,uint8_t setARPE);

void GTP_text(void);
#endif
