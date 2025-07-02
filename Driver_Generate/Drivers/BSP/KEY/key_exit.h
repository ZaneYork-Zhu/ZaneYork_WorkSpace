#ifndef __KEY_EXIT_H_
#define __KEY_EXIT_H_


#include "main.h"

#define KEY2_Pin                            GPIO_PIN_2
#define KEY2_GPIO_Port                      GPIOE
#define KEY2_GPIO_Mode                      GPIO_MODE_IT_FALLING
#define KEY2_GPIO_Pull                      GPIO_PULLUP
#define __HAL_RCC_KEY2_CLK_ENABLE()         do {__HAL_RCC_GPIOE_CLK_ENABLE();} while(0)
#define KEY2_EXTI_IRQn                      EXTI2_IRQn
#define KEY2_EXTI_IRQHandler                EXTI2_IRQHandler

#define KEY1_Pin                            GPIO_PIN_3
#define KEY1_GPIO_Port                      GPIOE
#define KEY1_GPIO_Mode                      GPIO_MODE_IT_FALLING
#define KEY1_GPIO_Pull                      GPIO_PULLUP
#define __HAL_RCC_KEY1_CLK_ENABLE()         do {__HAL_RCC_GPIOE_CLK_ENABLE();} while(0)
#define KEY1_EXTI_IRQn                      EXTI3_IRQn
#define KEY1_EXTI_IRQHandler                EXTI3_IRQHandler

#define KEY0_Pin                            GPIO_PIN_4
#define KEY0_GPIO_Port                      GPIOE
#define KEY0_GPIO_Mode                      GPIO_MODE_IT_FALLING
#define KEY0_GPIO_Pull                      GPIO_PULLUP
#define __HAL_RCC_KEY0_CLK_ENABLE()         do {__HAL_RCC_GPIOE_CLK_ENABLE();} while(0)
#define KEY0_EXTI_IRQn                      EXTI4_IRQn
#define KEY0_EXTI_IRQHandler                EXTI4_IRQHandler

#define WKUP_Pin                            GPIO_PIN_0
#define WKUP_GPIO_Port                      GPIOA
#define WKUP_GPIO_Mode                      GPIO_MODE_IT_RISING
#define WKUP_GPIO_Pull                      GPIO_PULLDOWN
#define __HAL_RCC_WKUP_CLK_ENABLE()         do {__HAL_RCC_GPIOA_CLK_ENABLE();} while(0)
#define WKUP_EXTI_IRQn                      EXTI0_IRQn
#define WKUP_EXTI_IRQHandler                EXTI0_IRQHandler

#define WKUP_PreemptPriority               2
#define WKUP_SubPriority                   0

#define KEY0_PreemptPriority               3
#define KEY0_SubPriority                   2

#define KEY1_PreemptPriority               3
#define KEY1_SubPriority                   1

#define KEY2_PreemptPriority               3
#define KEY2_SubPriority                   0

#define KEY0        HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)     /* 读取KEY0引脚 */
#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)     /* 读取KEY2引脚 */
#define WK_UP       HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_Pin)     /* 读取WKUP引脚 */

void key_exit_init(void);

void keYExit_text_init(void);
void keYExit_text_while(void);

#endif
