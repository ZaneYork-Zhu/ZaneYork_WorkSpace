/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32wb03x_it.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "n32wb03x_it.h"
#include "ns_log.h"
#include "app_lpuart.h"
#include "app_gpio.h"
#include "app_rtc.h"
#include "app_ble.h"
/** @addtogroup N32WB03X_StdPeriph_Template
 * @{
 */

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles PendSV_Handler exception.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 */
void SysTick_Handler(void)
{
}

/**
 * @brief  This function handles PPP interrupt request.
 */
/*void PPP_IRQHandler(void)
{
}*/



/**
 * @brief  LPUART interrupt handler
 * @param  
 * @return 
 * @note   
 */
void LPUART1_IRQHandler(void)
{
    uint8_t data; 
    while (LPUART_GetFlagStatus(LPUART1, LPUART_FLAG_FIFO_NE) != RESET)
    {
        /* Read one byte from the receive data register */
        data = LPUART_ReceiveData(LPUART1);
        app_lpuart_receive_data(&data,1);
    }
}

/**
 * @brief  EXTI4_12 interrupt handler
 * @param  
 * @return 
 * @note   
 */
void EXTI4_12_IRQHandler(void)
{
    /* EXTI_LINE10 as LPUART Wakeup */
    if ( EXTI_GetITStatus(EXTI_LINE10)!= RESET)
    {
        EXTI_ClrITPendBit(EXTI_LINE10);
    }
}


/**
 * @brief  External lines 1 interrupt. It will avtive rwip_schedule.
 */
void EXTI0_1_IRQHandler(void)
{
    if ( EXTI_GetITStatus(KEY1_INPUT_EXTI_LINE)!= RESET)
    {
        ke_msg_send_basic(APP_BUTTON_1_EVT,TASK_APP,TASK_APP);
        EXTI_ClrITPendBit(KEY1_INPUT_EXTI_LINE);
    }
}

/**
 * @brief  External lines 2 interrupt. It will avtive rwip_schedule.
 */
void EXTI2_3_IRQHandler(void)
{
    if ( EXTI_GetITStatus(KEY2_INPUT_EXTI_LINE)!= RESET)
    {
        ke_msg_send_basic(APP_BUTTON_2_EVT,TASK_APP,TASK_APP);
        EXTI_ClrITPendBit(KEY2_INPUT_EXTI_LINE);
    }
}

/**
 * @brief  This function handles RTC interrupt request.
 */

void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_INT_ALRA) != RESET)
    {
        RTC_ClrIntPendingBit(RTC_INT_ALRA);
        NS_LOG_INFO("%s->Alarm\r\n",__func__);
        RTC_TimeShow();
    }
    if (RESET != EXTI_GetITStatus(EXTI_LINE8))
    {
        EXTI_ClrITPendBit(EXTI_LINE8);
    }
    #ifndef SLEEP_LP_TIMER_ENABLE
    if (RTC_GetITStatus(RTC_INT_WUT) != RESET)
    {
        RTC_ClrIntPendingBit(RTC_INT_WUT);
        NS_LOG_INFO("%s->WakeUp\r\n",__func__);
    }
    if (RESET != EXTI_GetITStatus(EXTI_LINE9))
    {
        EXTI_ClrITPendBit(EXTI_LINE9);
    }
    #endif
}

/**
 * @}
 */
