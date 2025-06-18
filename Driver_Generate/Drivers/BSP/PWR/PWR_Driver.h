#ifndef __PWR_DRIVER_H__
#define __PWR_DRIVER_H__

#include "main.h"

#define PWR_STOP_MODE_ENABLE        (1)
#define PWR_WAKEUP_MODE_ENABLE      (1)
#define PWR_USATY_ENABLE       USART_RX_IRQ

#if PWR_STOP_MODE_ENABLE

    #define PWR_KEY0_PIN                                          GPIO_PIN_4
    #define PWR_KEY0_GPIO_PORT                                    GPIOE
    #define __PWR_KEY0_GPIO_CLK_ENABLE()                          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)
    #define PWR_KEY0_GPIO_MODE                                    GPIO_MODE_IT_FALLING
    #define PWR_KEY0_GPIO_PULL                                    GPIO_PULLUP
    #define PWR_KEY0_EXTI_IRQn                                    EXTI4_IRQn
    #define PWR_KEY0_EXTI_IRQHandler                              EXTI4_IRQHandler
    #define PWR_KEY0_EXTI_PreemptPriority                         3
    #define PWR_KEY0_EXTI_SubPriority                             0


#endif

#if PWR_WAKEUP_MODE_ENABLE

    #define PWR_WAKEUP_PIN                                          GPIO_PIN_0
    #define PWR_WAKEUP_GPIO_PORT                                    GPIOA
    #define __PWR_WAKEUP_GPIO_CLK_ENABLE()                          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
    #define PWR_WAKEUP_GPIO_MODE                                    GPIO_MODE_IT_RISING
    #define PWR_WAKEUP_GPIO_PULL                                    GPIO_PULLDOWN
    #define WKUP_KD                                                 HAL_GPIO_ReadPin(PWR_WAKEUP_GPIO_PORT, PWR_WAKEUP_PIN)  /* 读取WK_UP按键的状态 */

#endif

void PWR_Enter_Stop_Mode(void);
void PWR_Enter_Sleep_Mode(void);
void PWR_Enter_Standby(void);

void PWR_StopMode_Exti_KEY0_Init(void);
void PWR_StopMode_Exti_KEY0_Init_Test(void);
void PWR_StopMode_Exti_KEY0_Init_while(void);

void PWR_SleepMode_interrupt_uartRX_Init_Test(void);
void PWR_SleepMode_interrupt_uartRX_Init_while(void);

void PWR_WAKEUP_Init(void);
void PWR_StandbyMode_WAKEUP_Init_Test(void);
void PWR_StandbyMode_WAKEUP_Init_while(void);



#endif
