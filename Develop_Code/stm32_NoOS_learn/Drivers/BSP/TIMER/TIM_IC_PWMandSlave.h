#ifndef __TIM_IC_PWMandSLAVE_H_
#define __TIM_IC_PWMandSLAVE_H_

#include "main.h"

/* PA2 -> TIM9_CH1 */
#define TIMx_IC_CH1_Pin                           GPIO_PIN_2
#define TIMx_IC_CH1_GPIO_Port                     GPIOA
#define TIMx_IC_CH1_Mode                          GPIO_MODE_AF_PP
#define TIMx_IC_CH1_AF_FOR_TIM                    GPIO_AF3_TIM9
#define TIMx_IC_CH1_Pull                          GPIO_NOPULL
#define TIMx_IC_CH1_Speed                         GPIO_SPEED_FREQ_VERY_HIGH
#define __HAL_RCC_TIMx_IC_CH1_CLK_ENABLE()        do {__HAL_RCC_GPIOA_CLK_ENABLE();} while(0)


#define TIMx_IC_PWM_INSTANCE                      TIM9
#define TIMx_IC_PWM_INSTANCE_IRQn                 TIM1_BRK_TIM9_IRQn
#define TIMx_IC_PWM_INSTANCE_IRQHandler           TIM1_BRK_TIM9_IRQHandler
#define __TIMx_IC_PWM_CLK_ENABLE()                do{ __HAL_RCC_TIM9_CLK_ENABLE(); }while(0)  /* TIM9 时钟使能 */


typedef struct nTIM_IC_InitTypeDef
{
    TIM_IC_InitTypeDef IC_Init;  /*!< TIM Input Capture required parameters */
    uint32_t IC_Channel;          /*!< Specifies the TIM Input Capture channel.
                                       This parameter can be a value of @ref TIM_Input_Capture_Channel */
}nTIM_IC_InitTypeDef;

#define TIM_IT_UPDATE_PreemptPriority                       (1)  // 中断优先级
#define TIM_IT_UPDATE_SubPriority                           (0)  // 中断子优先级
#define TIM_IT_TRIGGER_PreemptPriority                      (1)  // 触发中断优先级
#define TIM_IT_TRIGGER_SubPriority                          (1)  // 触发中断子优先级

void TIM_IC_PWMandSlave_Test(void);
static HAL_StatusTypeDef TIM_IC_PWMandSlave_Init(   TIM_HandleTypeDef *pBaseStr,
                                                    nTIM_IC_InitTypeDef *pIC1Str,
                                                    nTIM_IC_InitTypeDef *pIC2Str,
                                                    TIM_SlaveConfigTypeDef *pSlaveStr);
static void GPIO_IC1_Init(void);                                                    
#endif /* __TIM_IC_PWMandSLAVE_H_ */

