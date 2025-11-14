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
#include "main.h"

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
 * @brief  This function handles DMA interrupt request defined in main.h .
 */
void DMA_IRQ_HANDLER(void)
{
}

/******************************************************************************/
/*                 N32WB03X Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32wb03x.s).                                                 */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 */
/*void PPP_IRQHandler(void)
{
}*/


void KEYSCAN_IRQHandler(void)
{
    uint32_t key_data[5] = {0};
    uint8_t  num;
    if(KEYSCAN_GetInterruptState() == SET)
    {
        KEYSCAN_ClearInterrupt();
    }
    
    if(EXTI_GetITStatus(EXTI_LINE13))
    {
        EXTI_ClrITPendBit(EXTI_LINE13);
    }
    
    KEYSCAN_ReadKeyData(key_data);
    #if 0
    log_info("REG data: ");
    log_info("0x%08x,", key_data[0]);
    log_info("0x%08x,", key_data[1]);
    log_info("0x%08x,", key_data[2]);
    log_info("0x%08x,", key_data[3]);
    log_info("0x%08x ", key_data[4]);
    log_info("\r\n");
    #endif
    
    for(num = 1; num < KEYSCAN_KEY_MAX; num++)
    {
        if(memcmp(key_data,&keyscan_map[num][0],(KEYSCAN_INFO_CHECK<<2)) == 0)
        {
            break;
        }
    }
    
    if(num != KEYSCAN_KEY_MAX)
    {
        #if 1
        extern uint16_t key_receive ;
        key_receive = num;
        #else
        log_info("Detect key %d Pressed!\r\n", num);
        #endif
    }
}
/**
 * @}
 */
