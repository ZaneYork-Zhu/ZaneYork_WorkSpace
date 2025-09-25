#ifndef __TIM_SYNC_RESET_H_
#define __TIM_SYNC_RESET_H_

#include "main.h"

#define TIMx_MASTER_INSTANCE                         TIM1
#define TIMx_MASTER_INSTANCE_IRQn                    TIM1_UP_TIM10_IRQn
#define TIMx_MASTER_INSTANCE_IRQHandler              TIM1_UP_TIM10_IRQHandler
#define __TIMx_MASTER_INSTANCE_CLK_ENABLE()          do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 时钟使能 */


#define TIMx_SLAVE_INSTANCE                          TIM3
#define TIMx_SLAVE_INSTANCE_IRQn                     TIM3_IRQn
#define TIMx_SLAVE_INSTANCE_IRQHandler               TIM3_IRQHandler
#define __TIMx_SLAVE_INSTANCE_CLK_ENABLE()           do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 时钟使能 */


#define TIM_MASTER_IT_PreemptPriority                      (1)  // 中断优先级
#define TIM_MASTER_IT_SubPriority                          (0)  // 中断子优先级
#define TIM_SLAVE_IT_PreemptPriority                       (0)  // 中断优先级
#define TIM_SLAVE_IT_SubPriority                           (0)  // 中断子优先级


HAL_StatusTypeDef TIM_Syn_Init( TIM_HandleTypeDef *pBaseMasterStr, 
                                TIM_MasterConfigTypeDef *pMasterStr,
                                TIM_HandleTypeDef *pBaseSlaveStr, 
                                TIM_SlaveConfigTypeDef *pSlaveStr);
void TIM_Syn_Test(void);
#endif /* __TIM_SYNC_RESET_H_ */

