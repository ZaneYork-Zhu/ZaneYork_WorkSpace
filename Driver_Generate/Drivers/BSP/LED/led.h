#ifndef __LED_H_
#define __LED_H_


#include "main.h"

#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOF

#define LED0(x) do{x? \
                        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET):  \
                        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);} while(0U)
  
#define LED1(x) do{x? \
                        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET):  \
                        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);} while(0U)
  
#define LED0_TOGGLE()       do{HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);}while(0U)
#define LED1_TOGGLE()       do{HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);}while(0U)

void led_init(void);
void led_text(void);
#endif
