#ifndef  _GTP_PULSE_COUNT_H_
#define  _GTP_PULSE_COUNT_H_


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

#define GTP_TIMX_PULSE                       TIM2
#define GTP_TIMX_PULSE_IRQn                  TIM2_IRQn
#define GTP_TIMX_PULSE_IRQHandler            TIM2_IRQHandler
#define GTP_TIMX_PULSE_CLK_ENABLE()          do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM6  ±÷” πƒ‹ */

void GTP_pulse_init(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc);
void GTP_PULSE_GPIO_Init(void);
void  GTP_pulse_text_init(void);
void  GTP_pulse_text_while(void);

uint8_t key_scan(uint8_t mode);
uint32_t TIM_cnt_chy_get_count(void);
void TIM_cnt_chy_restart(void);


#endif
