#ifndef __LED_H_ 
#define __LED_H_ 
 
#include "main.h" 
 
// 定义LED的相关宏，便于后续代码复用和维护 
// 对于LED0的定义 
#define LED0_Pin                GPIO_PIN_9 
#define LED0_GPIO_Port          GPIOF 
#define LED0_GPIO_MODE          GPIO_MODE_OUTPUT_PP 
#define LED0_GPIO_PULL          GPIO_NOPULL 
// 使能LED0对应的GPIO时钟，使用do-while(0)结构确保宏作为语句使用时的正确性 
#define __LED0_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOF_CLK_ENABLE(); } while(0U) 
 
// 对于LED1的定义 
#define LED1_Pin                GPIO_PIN_10 
#define LED1_GPIO_Port          GPIOF 
#define LED1_GPIO_MODE          GPIO_MODE_OUTPUT_PP 
#define LED1_GPIO_PULL          GPIO_NOPULL 
// 使能LED1对应的GPIO时钟，使用do-while(0)结构确保宏作为语句使用时的正确性 
#define __LED1_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOF_CLK_ENABLE(); } while(0U) 
 

// 翻转LED0状态的宏，使用do-while(0)结构确保宏作为语句使用时的正确性 
#define LED0_TOGGLE() do { HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); } while(0U) 
// 翻转LED1状态的宏，使用do-while(0)结构确保宏作为语句使用时的正确性 
#define LED1_TOGGLE() do { HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); } while(0U)

// 控制LED亮灭的宏，当x为真时点亮LED，为假时熄灭LED 
// 使用do-while(0)结构确保宏作为语句使用时的正确性
#define LED0(x) do{x? \
                        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET):  \
                        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);} while(0U)
  
#define LED1(x) do{x? \
                        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET):  \
                        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);} while(0U)
  

void LED_init(void);
void LED_text(void);
#endif
