#ifndef __TIM_OC_FORCE_MODE_H_
#define __TIM_OC_FORCE_MODE_H_

#include "main.h"   

#define TIMx_OC_CHx_Pin                           GPIO_PIN_0
#define TIMx_OC_CHx_GPIO_Port                     GPIOA
#define TIMx_OC_CHx_Mode                          GPIO_MODE_AF_PP
#define TIMx_OC_CHx_AF_FOR_TIM                    GPIO_AF1_TIM2
#define TIMx_OC_CHx_Pull                          GPIO_NOPULL
#define TIMx_OC_CHx_Speed                         GPIO_SPEED_FREQ_VERY_HIGH
#define __HAL_RCC_GPIO_OC_CHx_CLK_ENABLE()        do {__HAL_RCC_GPIOA_CLK_ENABLE();} while(0)



#define TIMx_OC_INSTANCE                          TIM2
#define TIMx_OC_INSTANCE_IRQn                     TIM2_IRQn
#define TIMx_OC_INSTANCE_IRQHandler               TIM2_IRQHandler
#define __TIMx_OC_INSTANCE_CLK_ENABLE()           do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM2 时钟使能 */

#define TIM_OC_DMA_Stream                         DMA1_Stream5
#define TIM_OC_DMA_Channel                        DMA_CHANNEL_3
#define TIM_OC_DMA_IRQn                           DMA1_Stream5_IRQn
#define TIM_OC_DMA_IRQHandler                     DMA1_Stream5_IRQHandler
#define TIM_OC_DMA_FLAG_TCIF                      DMA_FLAG_TCIF1_5
#define __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE()       do{ __HAL_RCC_DMA1_CLK_ENABLE(); }while(0)


#define TIM_OC_FORCE_MODE_ENABLE                    (1) // 0: 强制无效模式，1：强制有效模式

#define TIM_OC_MODE_POLLING_ENABLE                   (1) // 0: 轮询方式关闭 ,1: 轮询方式使能
#define TIM_OC_MODE_IT_ENABLE                        (0) // 0: 中断方式关闭 ,1: 中断方式使能
#define TIM_OC_MODE_DMA_ENABLE                       (0) // 0: DMA方式关闭 ,1: DMA方式使能

#define TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE           (1) // 0: 禁止直接模式 ,1: 允许直接模式使能
/*combine mode 和single mode 不能同时使能*/
#define TIM_COMBINE_MODE_ENABLE                      (0) // 0: 禁止组合模式 ,1: 允许组合模式使能
#define TIM_SINGLE_MODE_ENABLE                       (0) // 0: 禁止单通道模式 ,1: 允许单通道模式使能

#define TIM_DMA_NORMAL_ENABLE                        (0) // 0: DMA循环模式 ,1: DMA正常模式使能
#define TIM_DMA_BURST_ENABLE                         (1) // 0: DMA突发模式 ,1: DMA突发模式使能


#define TIM_IT_PreemptPriority                       (0)  // 中断优先级
#define TIM_IT_SubPriority                           (0)  // 中断子优先级
#define TIM_DMA_PreemptPriority                      (1)  // DMA中断优先级
#define TIM_DMA_SubPriority                          (0)  // DMA中断子优先级




typedef struct NEW_TIM_HandleTypeDef{
    TIM_HandleTypeDef HAL_Handle;
    uint32_t OCchannel;
} NEW_TIM_HandleTypeDef;

extern void TIM_Base_Init(void);
extern void TIM_OC_MODE_Test(void);
extern void TIM_OC_combine_Mode_Init(void);
extern void TIM_OC_DMAburst_Mode_Init(void);
extern void TIM_OC_Force_Mode_Init(NEW_TIM_HandleTypeDef *pBaseStr, TIM_OC_InitTypeDef *pOCInitStr);
static void LED_Display_init(void);
static void GPIO_OC_CHx_Init(void);
static void DMA_OC_CHx_Init(TIM_HandleTypeDef *htim);

#endif /* __TIM_OC_FORCE_MODE_H_ */

