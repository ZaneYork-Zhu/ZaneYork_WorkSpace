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
#define EXTERNAL_CLK_Prescaler                  TIM_CLOCKPRESCALER_DIV2
#define EXTERNAL_CLK_Filter                     0x08 // 外部时钟滤波



#define EXTERNAL_TIM                            TIM2
#define __HAL_RCC_EXTERNAL_TIM_CLK_ENABLE()     do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  
#define EXTERNAL_TIM_IRQn                       TIM2_IRQn
#define EXTERNAL_TIM_IRQHandler                 TIM2_IRQHandler



#define TIM_POLLING_ENABLE                  (1)  // 0: 禁止轮询模式, 1: 允许轮询模式
#define TIM_EXTERNAL_MODE1_TI1ED_ENABLE     (0)  // 0: 禁止外部时钟TI1ED模式, 1: 允许外部时钟TI1ED模式
#define TIM_EXTERNAL_MODE1_TIxFPx_ENABLE    (0)  // 0: 禁止外部时钟TIxFPx模式, 1: 允许外部时钟TIxFPx模式
#define TIM_EXTERNAL_MODE1__ETRMODE1_ENABLE (1)  // 0: 禁止外部时钟ETRMODE1模式, 1: 允许外部时钟ETRMODE1模式

extern void TIM_External_Test(void);
extern void TIM_External_clock_Init(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc);
static void GPIO_External_Input_Init(void);

#endif // __TIM_EXTERNAL_CLOCK_H_
