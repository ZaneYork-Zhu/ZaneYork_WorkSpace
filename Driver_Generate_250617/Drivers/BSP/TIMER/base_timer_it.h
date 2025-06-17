#ifndef __BASE_TIMER_IT_H_
#define __BASE_TIMER_IT_H_

#include "main.h"


extern TIM_HandleTypeDef tim_base_handle;

#define BASE_TIMX_INT                       TIM6
#define BASE_TIMX_INT_IRQn                  TIM6_DAC_IRQn
#define BASE_TIMX_INT_IRQHandler            TIM6_DAC_IRQHandler
#define BASE_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM6_CLK_ENABLE(); }while(0)  /* TIM6  ±÷” πƒ‹ */

void  baseTimerItInit(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc);

void base_text_init(void);
void base_text_while(void);
#endif
