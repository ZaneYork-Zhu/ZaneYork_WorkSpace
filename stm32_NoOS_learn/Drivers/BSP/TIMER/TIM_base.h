#ifndef __TIM_BASE_H_
#define __TIM_BASE_H_

#include "main.h"

extern TIM_HandleTypeDef g_tim_base_handle;
extern DMA_HandleTypeDef g_Hdma_UpdateEvent_handle; // DMA句柄

#define BASE_TIM                            TIM1
#define BASE_TIM_IRQn                       TIM1_UP_TIM10_IRQn
#define BASE_TIM_IRQHandler                 TIM1_UP_TIM10_IRQHandler
#define __BASE_TIM_CLK_ENABLE()             do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)


#define TIM_POLLING_ENABLE                  (0)  // 0: 禁止轮询模式, 1: 允许轮询模式
#define TIM_IT_ENABLE                       (0)  // 0: 禁止中断模式, 1: 允许中断模式
#define TIM_DMA_ENABLE                      (1)  // 0: 禁止DMA模式, 1: 允许DMA模式
#define TIM_DMA_NORMAL_ENABLE               (0)  // 0: 禁止DMA正常模式, 1: 允许DMA正常模式


#define TIM_IT_PreemptPriority              (0)  // 中断优先级
#define TIM_IT_SubPriority                  (0)  // 中断子优先级
#define TIM_DMA_PreemptPriority             (1)  // DMA中断优先级   
#define TIM_DMA_SubPriority                 (1)  // DMA中断子优先级

#define REPETITION_COUNTER_ENABLE            (0)  // 0: 禁止重复计数器, 1: 允许重复计数器
#define REPETITION_COUNTER_VALUE             (0)  // 重复计数器值


extern void Tim_base_Test(void);
extern void Set_TIM_Period(uint32_t period); 
extern void Tim_base_Init(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc, uint8_t setARPE);

#endif // __TIM_BASE_H_
