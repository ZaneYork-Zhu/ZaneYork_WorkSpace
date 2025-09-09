#ifndef __TIM_TPAD_H_
#define __TIM_TPAD_H_

#include "main.h"

#include "usart.h"


#define TAPD_KEY_Pin                GPIO_PIN_5
#define TAPD_KEY_GPIO_Port          GPIOA
#define TAPD_KEY_Alternate          GPIO_AF1_TIM2

/*������ʱ��ʹ��*/
#define __HAL_RCC_TAPD_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) 

#define TIM_TPAD_INT                       TIM2
#define TIM_TPAD_INT_IRQn                  TIM2_IRQn
#define TIM_TPAD_INT_IRQHandler            TIM2_IRQHandler
#define TIM_TPAD_INT_CAP_CHY               TIM_CHANNEL_1  
#define TIM_TPAD_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM2 ʱ��ʹ�� */

extern TIM_HandleTypeDef TIM_TPAD_Handle;

/* USER CODE BEGIN Private defines */
/*ƽ���˲�ʱʹ��*/
#define INDEX 10
/* ����������ֵ, Ҳ���Ǳ������ g_tpad_default_val + TPAD_GATE_VAL
 * ����Ϊ����Ч����, �Ĵ� TPAD_GATE_VAL, ���Խ���������, ��֮, ��������������
 * ����ʵ������, ѡ����ʵ� TPAD_GATE_VAL ����
 */
#define TPAD_GATE_VALUE     100                 /* ����������ֵ, Ҳ���Ǳ������ g_tpad_default_val + TPAD_GATE_VAL, ����Ϊ����Ч���� */
#define TPAD_ARR_MAX_VAL    0xFFFFFFFF          /* ����ARRֵ, һ������Ϊ��ʱ����ARR���ֵ */

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
