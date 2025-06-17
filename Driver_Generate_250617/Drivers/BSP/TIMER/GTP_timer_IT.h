#ifndef  __GTP_TIMER_IT_H_
#define  __GTP_TIMER_IT_H_

#include "main.h"

extern TIM_HandleTypeDef tim_GTP_handle;

#define GTP_TIMX_INT                       TIM3
#define GTP_TIMX_INT_IRQn                  TIM3_IRQn
#define GTP_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTP_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM6  ±÷” πƒ‹ */

void  GTP_TimerItInit(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc);

void GTP_text_init(void);
void GTP_text_while(void);
#endif
