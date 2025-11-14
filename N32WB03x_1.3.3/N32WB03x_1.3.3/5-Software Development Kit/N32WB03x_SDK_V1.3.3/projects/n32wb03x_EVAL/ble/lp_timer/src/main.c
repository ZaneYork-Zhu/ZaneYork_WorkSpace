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
 * @file main.c
 * @author Nations Firmware Team
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/** @addtogroup 
 * @{
 */
 
/* Includes ------------------------------------------------------------------*/
#include "n32wb03x.h"
#include "rwip.h"
#include "ns_ble.h"
#include "ns_sleep.h"
#include "ns_delay.h"
#include "ns_log.h"
#include "app_ble.h"
#include "app_lpuart.h"
#include "app_gpio.h"
#include "app_rtc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEMO_STRING  "\r\n Nations device low power timer demo \r\n"
#define LPT_PERIOD_MS  100

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
 * @brief  Low power timer handler.
 * You can drive peripherals and run logic code, do not disable interrupt here.
 * You send message but not set up ke_timer or ns_timer here. The rwip_schedule not 
 * active yet, the message you sent will handle after the rwip_schedule wake-up. 
 * You can wake-up the rwip_schedule manully with function ns_sleep_lp_timer_wake_ble.
 */
void user_lpt_wakeup_cb(void)
{
    LedBlink(LED1_PORT,LED1_PIN);
    #if 0
    // active rwip_schedule if necessary     
    ns_sleep_lp_timer_wake_ble();
    #endif
}

/**
 * @brief  main function
 * @param   
 * @return 
 * @note   Note
 */
int main(void)
{
    //for hold the SWD before sleep
    delay_n_10us(200*1000);
 
    NS_LOG_INIT();
    app_ble_init();
    
    NS_LOG_INFO(DEMO_STRING);
    NS_LOG_INFO("advertising interval:%dms\r\n",CUSTOM_ADV_FAST_INTERVAL*5/8);
    NS_LOG_INFO("low power timer interval:%dms\r\n",LPT_PERIOD_MS);
    
    LedInit(LED1_PORT,LED1_PIN);
    LedInit(LED2_PORT,LED2_PIN);    
    app_key_configuration();
    app_lpuart_init();
    app_lpuart_send_data("lpuart_test\r\n",13);
    RTC_Configuration();    

    #ifdef SLEEP_LP_TIMER_ENABLE
    ns_sleep_lp_timer_config(LPT_PERIOD_MS,user_lpt_wakeup_cb);
    #endif

    while (1)
    {
        /*schedule all pending events*/
        rwip_schedule();
        ns_sleep();
    }
}

/**
 * @brief  user handle before enter sleep mode
 * @param  
 * @return 
 * @note   
 */
void app_sleep_prepare_proc(void)
{

}

/**
 * @brief  user handle after wake up from sleep mode
 * @param  
 * @return 
 * @note   
 */
void app_sleep_resume_proc(void)
{
    app_key_reinit_after_sleep();
}



/**
 * @}
 */

