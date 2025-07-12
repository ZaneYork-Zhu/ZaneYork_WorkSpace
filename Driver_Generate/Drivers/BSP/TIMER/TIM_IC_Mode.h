#ifndef __TIM_IC_MODE_H_
#define __TIM_IC_MODE_H_

#include "main.h"


#define TIMx_IC_CHx_Pin                           GPIO_PIN_0
#define TIMx_IC_CHx_GPIO_Port                     GPIOA
#define TIMx_IC_CHx_Mode                          GPIO_MODE_AF_PP
#define TIMx_IC_CHx_AF_FOR_TIM                    GPIO_AF1_TIM2
#define TIMx_IC_CHx_Pull                          GPIO_PULLDOWN
#define TIMx_IC_CHx_Speed                         GPIO_SPEED_FREQ_VERY_HIGH
#define __HAL_RCC_TIMx_IC_CHx_CLK_ENABLE()        do {__HAL_RCC_GPIOA_CLK_ENABLE();} while(0)



#define TIMx_IC_INSTANCE                          TIM2
#define TIMx_IC_INSTANCE_IRQn                     TIM2_IRQn
#define TIMx_IC_INSTANCE_IRQHandler               TIM2_IRQHandler
#define __TIMx_IC_INSTANCE_CLK_ENABLE()           do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM2 时钟使能 */

#define TIM_IC_DMA_Stream                         DMA1_Stream1
#define TIM_IC_DMA_Channel                        DMA_CHANNEL_3
#define TIM_IC_DMA_IRQn                           DMA1_Stream1_IRQn
#define TIM_IC_DMA_IRQHandler                     DMA1_Stream1_IRQHandler
#define TIM_IC_DMA_FLAG_TCIF                      DMA_FLAG_TCIF1_5
#define __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE()       do{ __HAL_RCC_DMA1_CLK_ENABLE(); }while(0)



#define TIM_IC_MODE_POLLING_ENABLE                   (0) // 0: 轮询方式使能 ,1: 轮询方式使能
#define TIM_IC_MODE_IT_ENABLE                        (1) // 0: 中断方式使能 ,1: 中断方式使能
#define TIM_IC_MODE_DMA_ENABLE                       (0) // 0: DMA方式使能 ,1: DMA方式使能

#define TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE           (1) // 0: 禁止直接模式使能 ,1: 允许直接模式使能


#define TIM_IT_PreemptPriority                       (1)  // 中断优先级
#define TIM_IT_SubPriority                           (0)  // 中断子优先级
#define TIM_DMA_PreemptPriority                      (0)  // DMA中断优先级
#define TIM_DMA_SubPriority                          (0)  // DMA中断子优先级




typedef struct NEW_TIM_HandleTypeDef{
    TIM_HandleTypeDef *HAL_Handle;
    uint32_t ICchannel;
} NEW_TIM_HandleTypeDef;


extern void TIM_IC_MODE_Test(void);
extern void TIM_IC_MODE_Init(NEW_TIM_HandleTypeDef *deviceInit, TIM_IC_InitTypeDef *ic_device);
static void GPIO_IC_CHx_Init(void);
static void DMA_IC_CHx_Init(TIM_HandleTypeDef *htim);
static uint32_t TIM_setEdge(NEW_TIM_HandleTypeDef devStruct , uint32_t edge);

#endif /* __TIM_IC_MODE_H_ */

