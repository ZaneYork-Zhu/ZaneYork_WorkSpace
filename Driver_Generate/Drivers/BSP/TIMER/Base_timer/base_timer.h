#ifndef __BASE_TIMER_H_
#define __BASE_TIMER_H_

#include "main.h"


extern TIM_HandleTypeDef gTim_base_handle;

#define BASE_TIMX_INT                       TIM6
#define BASE_TIMX_INT_IRQn                  TIM6_DAC_IRQn
#define BASE_TIMX_INT_IRQHandler            TIM6_DAC_IRQHandler
#define __BASE_TIMX_INT_CLK_ENABLE()        do{ __HAL_RCC_TIM6_CLK_ENABLE(); }while(0)  /* TIM6时钟配置 */

#define BASE_TIMER_POLL_ENABLE (1)

void  baseTimer_SetARPE_Init(TIM_TypeDef* TimInstance,uint16_t arr, uint16_t psc ,uint8_t setARPE);

void BaseTimer_Test_Init(void);
void BaseTimer_Test_While(void);

#endif
