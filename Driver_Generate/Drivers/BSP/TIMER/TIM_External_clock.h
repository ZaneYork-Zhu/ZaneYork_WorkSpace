#ifndef __TIM_EXTERNAL_CLOCK_H_
#define __TIM_EXTERNAL_CLOCK_H_

#include "main.h"

/* 外部输入通道GPIO配置*/
#define EXTERNAL_Pin                            GPIO_PIN_0
#define EXTERNAL_GPIO_Port                      GPIOA
#define EXTERNAL_GPIO_Mode                      GPIO_MODE_AF_PP
#define EXTERNAL_GPIO_AF_FOR_TIM                GPIO_AF1_TIM2
#define EXTERNAL_GPIO_Pull                      GPIO_PULLDOWN
#define EXTERNAL_GPIO_Speed                     GPIO_SPEED_FREQ_VERY_HIGH
#define __HAL_RCC_EXTERNAL_CLK_ENABLE()         do {__HAL_RCC_GPIOA_CLK_ENABLE();} while(0)


/* 外部时钟源配置 */
#define EXTERNAL_CLK_Polarity                   TIM_CLOCKPOLARITY_RISING
#define EXTERNAL_CLK_Prescaler                  TIM_CLOCKPRESCALER_DIV1
#define EXTERNAL_CLK_Filter                     0x00 // 外部时钟滤波



#define EXTERNAL_TIM                            TIM2
#define __HAL_RCC_EXTERNAL_TIM_CLK_ENABLE()     do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  
#define EXTERNAL_TIM_IRQn                       TIM2_IRQn
#define EXTERNAL_TIM_IRQHandler                 TIM2_IRQHandler

#define EXTERNAL_DMA_Stream                     DMA1_Stream1
#define EXTERNAL_DMA_Channel                    DMA_CHANNEL_3
#define EXTERNAL_DMA_IRQn                       DMA1_Stream1_IRQn
#define EXTERNAL_DMA_IRQHandler                 DMA1_Stream1_IRQHandler
#define EXTERNAL_DMA_FLAG_TCIF                  DMA_FLAG_TCIF1_5
#define __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE()     do{ __HAL_RCC_DMA1_CLK_ENABLE(); }while(0)



#define TIM_POLLING_ENABLE                  (0)  // 0: 禁止轮询模式, 1: 允许轮询模式
#define TIM_IT_ENABLE                       (0)  // 0: 禁止中断模式, 1: 允许中断模式
#define TIM_DMA_ENABLE                      (1)  // 0: 禁止DMA模式, 1: 允许DMA模式
#define TIM_DMA_NORMAL_ENABLE               (1)  // 0: 开启DMA的Cycle模式, 1: 开启DMA的Normal正常模式

#define TIM_IT_PreemptPriority              (1)  // 中断优先级
#define TIM_IT_SubPriority                  (0)  // 中断子优先级
#define TIM_DMA_PreemptPriority             (0)  // DMA中断优先级
#define TIM_DMA_SubPriority                 (0)  // DMA中断子优先级

#define TIM_EXTERNAL_MODE1_TI1ED_ENABLE     (0)  // 0: 禁止外部时钟TI1ED模式, 1: 允许外部时钟TI1ED模式
#define TIM_EXTERNAL_MODE1_TIxFPx_ENABLE    (0)  // 0: 禁止外部时钟TIxFPx模式, 1: 允许外部时钟TIxFPx模式
#define TIM_EXTERNAL_MODE1_ETRMODE1_ENABLE  (1)  // 0: 禁止外部时钟ETRMODE1模式, 1: 允许外部时钟ETRMODE1模式
#define TIM_EXTERNAL_MODE2_ETRMODE2_ENABLE  (0)  // 0: 禁止外部时钟ETRMODE2模式, 1: 允许外部时钟ETRMODE2模式

extern void TIM_External_Test(void);
extern void TIM_External_clock_Init(TIM_TypeDef* TimInstance, uint32_t arr, uint32_t psc,uint32_t setARPE ,uint32_t rep ,uint32_t countMode);
static void GPIO_External_Input_Init(void);

#endif // __TIM_EXTERNAL_CLOCK_H_
