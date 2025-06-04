#ifndef __TIM_TPAD_H_
#define __TIM_TPAD_H_

#include "main.h"

#include "usart.h"


#define TAPD_KEY_Pin                GPIO_PIN_5
#define TAPD_KEY_GPIO_Port          GPIOA
#define TAPD_KEY_Alternate          GPIO_AF1_TIM2

/*按键的时钟使能*/
#define __HAL_RCC_TAPD_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) 

#define TIM_TPAD_INT                       TIM2
#define TIM_TPAD_INT_IRQn                  TIM2_IRQn
#define TIM_TPAD_INT_IRQHandler            TIM2_IRQHandler
#define TIM_TPAD_INT_CAP_CHY               TIM_CHANNEL_1  
#define TIM_TPAD_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM2 时钟使能 */

extern TIM_HandleTypeDef TIM_TPAD_Handle;

/* USER CODE BEGIN Private defines */
/*平均滤波时使用*/
#define INDEX 10
/* 触摸的门限值, 也就是必须大于 g_tpad_default_val + TPAD_GATE_VAL
 * 才认为是有效触摸, 改大 TPAD_GATE_VAL, 可以降低灵敏度, 反之, 则可以提高灵敏度
 * 根据实际需求, 选择合适的 TPAD_GATE_VAL 即可
 */
#define TPAD_GATE_VALUE     100                 /* 触摸的门限值, 也就是必须大于 g_tpad_default_val + TPAD_GATE_VAL, 才认为是有效触摸 */
#define TPAD_ARR_MAX_VAL    0xFFFFFFFF          /* 最大的ARR值, 一般设置为定时器的ARR最大值 */

#define DEFAULT_SAMPLE_TIMES 3 
#define CONTINUOUS_SAMPLE_TIMES 6

void TPAD_text_init(void);
void TPAD_text_while(void);
/* USER CODE END Private defines */

void TIM_TPAD_init(TIM_TypeDef* TIM,uint32_t arr, uint16_t psc);

/* USER CODE BEGIN Prototypes */
static void tpad_reset(void);
static uint16_t tpad_get_val(void);
static uint16_t tpad_get_maxval(uint8_t n);
uint8_t tpad_scan(uint8_t mode);
uint8_t tpad_init(uint16_t psc);
/* USER CODE END Prototypes */


#endif
