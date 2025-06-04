#ifndef __KEY_H_
#define __KEY_H_

#include "led.h"
#include "main.h"

#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOE
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define KEY0_Pin GPIO_PIN_4
#define KEY0_GPIO_Port GPIOE
#define WK_UP_Pin GPIO_PIN_0
#define WK_UP_GPIO_Port GPIOA

#define KEY0 HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)
#define KEY1 HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2 HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define WK_UP HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define WK_UP_PRES 4


void key_init(void);
uint8_t key_scan(uint8_t mode);

void key_text(void);

#endif
