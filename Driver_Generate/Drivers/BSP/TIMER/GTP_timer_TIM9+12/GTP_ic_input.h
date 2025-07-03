#ifndef __GTP_IC_INPUT_H_
#define __GTP_IC_INPUT_H_

#include "main.h"

#define GTP_TIMX_IC_INT                       TIM5
#define GTP_TIMX_IC_INT_IRQn                  TIM5_IRQn
#define GTP_TIMX_IC_INT_IRQHandler            TIM5_IRQHandler
#define GTP_TIMX_IC_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM5_CLK_ENABLE(); }while(0)  /* TIM6 时钟使能 */

extern TIM_HandleTypeDef GPT_IC_Handle;

/* USER CODE BEGIN Private defines */
typedef struct {
  uint8_t finish : 1;  // bit7:0,没有成功的捕获;1,成功捕获到一次
  uint8_t state  : 1;  // bit6:0,没有捕获到上升沿;1,已经捕获到上升沿了.
  uint8_t data   : 6;  // bit5~bit0：溢出次数 
} Tim_State;


extern Tim_State g_timxchy_cap_sta;
extern uint16_t g_timxchy_cap_val;


void gpio_init(void);
void  GTP_TimerICInit(TIM_TypeDef* TIM,uint16_t arr, uint16_t psc);

void GTP_IC_text_init(void);
void GTP_IC_text_while(void);
/* USER CODE END Private defines */

#endif

