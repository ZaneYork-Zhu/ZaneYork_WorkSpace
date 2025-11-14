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
 * @file ns_sleep.c
 * @author Nations Firmware Team
 * @version v1.0.4
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/** @addtogroup 
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "ns_sleep.h"
#include "ns_ble.h"
/* Private typedef -----------------------------------------------------------*/
enum ns_bb_status_t
{
    BB_ACTIVE = 0,
    BB_SLEEP,
    BB_WAKEUP_OSEN,
    BB_WAKEUP_EXIT,    
    BB_WAKEUP_USER,
    BB_WAKEUP_ERROR,   
//    BB_WAKEUP_RTC, //keep in sleep after lpt callback
};
/* Private define ------------------------------------------------------------*/
#ifndef NS_WAKEUP_CONFIG
#define NS_WAKEUP_CONFIG()   
#endif
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t ns_sleep_lock = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef SLEEP_LP_TIMER_ENABLE
/**
 * @brief  low power timer initialization.
 * @param  period_ms: 1 - 0x7fff 
 * @param  func: low power timer callback, not suggest run over 2ms
 * @return void
 * @note   
 */
void ns_sleep_lp_timer_config(uint16_t period_ms, IRQ_HANNDLE_FUN func)
{
    /* enable rtc clock*/  
    RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();
    RTC_EnableWakeUp(DISABLE);
    RTC_ConfigInt(RTC_INT_WUT, DISABLE);  
    /* set wakeup callback function*/
    extern IRQ_HANNDLE_FUN lpt_wakeup_hdl ;
    lpt_wakeup_hdl = func;
    if(lpt_wakeup_hdl == NULL)
    {
        //disable low power timer
        return;
    }
    /* init wakeup timer*/  
    if((RTC->INITSTS&RTC_INITSTS_INITSF) == 0)
    {
        RTC_InitType RTC_InitStructure;
        RTC_InitStructure.RTC_AsynchPrediv = 0x7D; // 7f:23768 , 7d:32k
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
        RTC_InitStructure.RTC_HourFormat   = RTC_24HOUR_FORMAT;
        RTC_Init(&RTC_InitStructure);
    }
    /* set wakeup timer*/
    RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV16); //32k/16 = 2K
    RTC_SetWakeUpCounter(period_ms*2); 
    
    /* Enable the RTC WakeUp Interrupt */  
    EXTI_InitType EXTI_InitStructure;
    EXTI_ClrITPendBit(EXTI_LINE9);
    EXTI_InitStructure.EXTI_Line                = EXTI_LINE9;
    EXTI_InitStructure.EXTI_Mode                = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger             = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd             = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel          = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority  = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd       = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_SetPriority(EXTI4_12_IRQn, 3); 

    
    RTC_ConfigInt(RTC_INT_WUT, ENABLE);
    /* Enable wakeup timer*/
    RTC_EnableWakeUp(ENABLE);
}

/**
 * @brief  active rwip(bb) in wakeup callback.
 * @param  
 * @return void
 * @note   
 */
void ns_sleep_lp_timer_wake_ble(void)
{
    g_sleep_status_flag = BB_WAKEUP_USER;
}
#endif

/**
 * @brief  Acquire a sleep lock, it will prevent the os enter sleep mode. 
 *         We should call ns_sleep_lock_release function when this lock can be release. 
 * @param  
 * @return 
 * @note   
 */
uint8_t ns_sleep_lock_acquire(void)
{
    if(ns_sleep_lock++ == 0)
    {
        //overflow
        return false;
    }
    return true;
}

/**
 * @brief  Release a sleep lock, if all the lock has been released, os will enter sleep mode 
 *         when run out of task.
 * @param  
 * @return 
 * @note   
 */
uint8_t ns_sleep_lock_release(void)
{
    if(ns_sleep_lock)
    {
        ns_sleep_lock--;
        return true;
    }
    return false;
}


/**
 * @brief  entry_sleep
 * @param  
 * @return 
 * @note   
 */
void entry_sleep(void)
{
    uint32_t reg_rt_0 = REG32(0x40011004);
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR; // PWR enable    
    REG32(0x40028030) |= 0x07;
    REG32(0x40011004)  = 0x00; 
    EXTI_PA11_Configuration();  
    g_sleep_status_flag = BB_SLEEP;
    while(PWR->CR1&0x20); //wait ble sleep
    while(1)
    {
        __set_PRIMASK(1);//disable irq
        volatile uint8_t *p_flag = &g_sleep_status_flag;
        if( (*p_flag) != BB_SLEEP )
        {
            __set_PRIMASK(0);//enable irq
            //wake up rwip
            break;
        }
        NS_LOG_DEINIT();  
        
        PWR->CR1  = 0x0A;
        SCB->SCR |= 0x04;
        __WFI(); 
        
        RCC->LSCTRL |= 1;   //rewrite LSCTRL
        RCC->CFG |=  RCC_HCLK_DIV2; //APB1 = HCLK/2, APB1 max is 32M
        RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR; // PWR enable
        RCC->APB2PCLKEN |= RCC_APB2_PERIPH_GPIOA|RCC_APB2_PERIPH_GPIOB|RCC_APB2_PERIPH_AFIO; 
        REG32(0x40011004) = reg_rt_0; 
        NS_WAKEUP_CONFIG();
        NS_LOG_INIT();
        
        #ifdef SLEEP_LP_TIMER_ENABLE 
        if( (EXTI->PEND)&(~EXTI_LINE9) )
        {
            //rtc extra wake up
            g_sleep_status_flag = BB_WAKEUP_EXIT;
        }
        __set_PRIMASK(0);//enable irq
        //wait IRQ handler
        uint8_t exit_wait = 0xff;
        while((EXTI->PEND&EXTI->IMASK) != 0)
        {
            exit_wait--;
            if(exit_wait == 0)
            {
                //timeout error, wake ble
                g_sleep_status_flag = BB_WAKEUP_ERROR;
                break;
            }
        }
        #else
        //wake up rwip in all case
        break;
        #endif
    }
    //wake ble with wakeup req
    PWR->CR2 |= 0x100; //1<<8 wake up ble
    while(!(PWR->CR1&0x20));//wait ble active
}

/**
 * @brief  entry Idle mode
 * @param  
 * @return 
 * @note   
 */
void entry_idle(void)
{
    EXTI_PA11_Configuration(); 
    PWR->CR1 &= 0xF0;
    SCB->SCR &= 0xFB;
    __WFI();
}

/**
 * @brief  sleep task function, usually run after rwip_schedule function in main loop.
 * @param  
 * @return 
 * @note   
 */
void ns_sleep(void)
{
    if(ns_sleep_lock != 0)
    {
        return;
    }
    GLOBAL_INT_DISABLE();
    app_sleep_prepare_proc();
    switch(rwip_sleep())
    {
        case RWIP_DEEP_SLEEP:
        {
            entry_sleep();
        }
        break;
        case RWIP_CPU_SLEEP:
        {
            entry_idle();
        }
        break;
        case RWIP_ACTIVE:
        default:
        {

        }
        break;
    }
    GLOBAL_INT_RESTORE();
    
    volatile uint16_t *p_prevent_sleep = &rwip_env.prevent_sleep;
    /* check ble out of sleep */
    if(((*p_prevent_sleep) & (RW_WAKE_UP_ONGOING|RW_DEEP_SLEEP) )) 
    {
        uint32_t wait_sleep = 12800; //2*800us
        while((*p_prevent_sleep) & (RW_WAKE_UP_ONGOING|RW_DEEP_SLEEP))
        {
            wait_sleep--;
            if(wait_sleep == 0)
            {
                break;
            }
        }
        rwip_time_t current_time = rwip_time_get();
        // check if 1ms timer is active and make sure ke timer active.
        if(rwip_env.timer_1ms_target.hs != RWIP_INVALID_TARGET_TIME)
        {
            int32_t duration = CLK_DIFF(current_time.hs, rwip_env.timer_1ms_target.hs);
            if(duration < 0 )
            {
                // Mark that 1ms timer is over
                ke_event_set(KE_EVENT_KE_TIMER);
            }
        }
    }
    
    app_sleep_resume_proc(); 

}

/**
 * @brief  User code beofre enter sleep mode
 * @param  
 * @return 
 * @note   
 */
__weak void app_sleep_prepare_proc(void)
{
    
}

/**
 * @brief  User code after out of sleep mode. This function run after interrupt
 *         handler function if any interrupt pending when sleep.
 * @param  
 * @return 
 * @note   
 */
__weak void app_sleep_resume_proc(void)
{
    
}

/**
 * @}
 */

