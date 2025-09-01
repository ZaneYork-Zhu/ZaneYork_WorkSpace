#ifndef __TIM_SLAVE_MODE_H_
#define __TIM_SLAVE_MODE_H_

#include "main.h"


#define TIMx_SLAVE_CHx_Pin                           GPIO_PIN_0
#define TIMx_SLAVE_CHx_GPIO_Port                     GPIOA
#define TIMx_SLAVE_CHx_Mode                          GPIO_MODE_AF_PP
#define TIMx_SLAVE_CHx_AF_FOR_TIM                    GPIO_AF1_TIM2
#define TIMx_SLAVE_CHx_Pull                          GPIO_NOPULL
#define TIMx_SLAVE_CHx_Speed                         GPIO_SPEED_FREQ_VERY_HIGH
#define __HAL_RCC_TIMx_SLAVE_CHx_CLK_ENABLE()        do {__HAL_RCC_GPIOA_CLK_ENABLE();} while(0)



#define TIMx_SLAVE_INSTANCE                          TIM2
#define TIMx_SLAVE_INSTANCE_IRQn                     TIM2_IRQn
#define TIMx_SLAVE_INSTANCE_IRQHandler               TIM2_IRQHandler
#define __TIMx_SLAVE_INSTANCE_CLK_ENABLE()           do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM2 时钟使能 */

#define TIM_SLAVE_DMA_Stream                         DMA1_Stream5
#define TIM_SLAVE_DMA_Channel                        DMA_CHANNEL_3
#define TIM_SLAVE_DMA_IRQn                           DMA1_Stream5_IRQn
#define TIM_SLAVE_DMA_IRQHandler                     DMA1_Stream5_IRQHandler
#define TIM_SLAVE_DMA_FLAG_TCIF                      DMA_FLAG_TCIF1_5
#define __HAL_RCC_EXTERNAL_DMA_CLK_ENABLE()          do{ __HAL_RCC_DMA1_CLK_ENABLE(); }while(0)



#define TIM_POLLING_ENABLE                          (1) // 0: 禁止轮询模式使能 ,1: 允许轮询模式使能
#define TIM_IT_ENABLE                               (0) // 0: 禁止中断使能 ,1: 允许中断使能
#define TIM_DMA_ENABLE                              (0) // 0: 禁止DMA使能 ,1: 允许DMA使能

#define TIM_SLAVE_RESET_MODE                        (1) // 0: 禁止复位模式使能 ,1: 允许复位模式使能
#define TIM_SLAVE_GATED_MODE                        (0) // 0: 禁止门控模式使能 ,1: 允许门控模式使能
#define TIM_SLAVE_TRIGGER_MODE                      (0) // 0: 禁止触发模式使能 ,1: 允许触发模式使能
#define TIM_SLAVE_EXTERNAL_MODE                     (0) // 0: 禁止外部时钟+触发模式使能 ,1: 允许外部时钟+触发模式使能



#define TIM_CHANNEL_FOR_DIRECT_MODE_ENABLE           (1) // 0: 禁止直接模式使能 ,1: 允许直接模式使能
/*combine mode 和single mode 不能同时使能*/
#define TIM_COMBINE_MODE_ENABLE                      (0) // 0: 禁止组合模式使能 ,1: 允许组合模式使能
#define TIM_SINGLE_MODE_ENABLE                       (0) // 0: 禁止单通道模式使能 ,1: 允许单通道模式使能

#define TIM_DMA_NORMAL_ENABLE                        (0) // 0: DMA循环模式使能 ,1: DMA正常模式使能
#define TIM_DMA_BURST_ENABLE                         (1) // 0: DMA突发模式使能 ,1: DMA突发模式使能


#define TIM_IT_PreemptPriority                       (1)  // 中断优先级
#define TIM_IT_SubPriority                           (0)  // 中断子优先级
#define TIM_DMA_PreemptPriority                      (0)  // DMA中断优先级
#define TIM_DMA_SubPriority                          (0)  // DMA中断子优先级




typedef struct nTIM_IC_InitTypeDef
{
    TIM_IC_InitTypeDef IC_Init;  /*!< TIM Input Capture required parameters */
    uint32_t IC_Channel;          /*!< Specifies the TIM Input Capture channel.
                                       This parameter can be a value of @ref TIM_Input_Capture_Channel */
}nTIM_IC_InitTypeDef;

static void GPIO_IC_CHx_Init(void);
extern void TIM_Slave_Mode_Test(void);
static HAL_StatusTypeDef TIM_SlaveReset_Mode_Init(TIM_HandleTypeDef *pBaseStr, TIM_SlaveConfigTypeDef *pSlaveStr,nTIM_IC_InitTypeDef *pICStr);
#endif /* __TIM_SLAVE_MODE_H_ */

