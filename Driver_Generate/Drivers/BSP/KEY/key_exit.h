#ifndef __KEY_EXIT_H_
#define __KEY_EXIT_H_


#include "main.h"

#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOE
#define KEY2_EXTI_IRQn EXTI2_IRQn
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define KEY1_EXTI_IRQn EXTI3_IRQn
#define KEY0_Pin GPIO_PIN_4
#define KEY0_GPIO_Port GPIOE
#define KEY0_EXTI_IRQn EXTI4_IRQn
#define WKUP_Pin GPIO_PIN_0
#define WKUP_GPIO_Port GPIOA
#define WKUP_EXTI_IRQn EXTI0_IRQn

#define KEY0        HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)     /* 读取KEY0引脚 */
#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)     /* 读取KEY2引脚 */
#define WK_UP       HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_Pin)     /* 读取WKUP引脚 */

void key_exit_init(void);

void keYExit_text_init(void);
void keYExit_text_while(void);

#endif
