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
 * @file app_rtc.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "app_rtc.h"
#include "ns_log.h"
/** @addtogroup 
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define RCT_LOG_ENABLE
#ifdef RCT_LOG_ENABLE
#define log_info NS_LOG_INFO
#else
#define log_info(str, ...) 
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
ErrorStatus RTC_DateTimeRegulate(void);
ErrorStatus RTC_AlarmRegulate(void);
void EXTI9_RTCWKUP_Configuration(FunctionalState Cmd);
void EXTI8_RTCAlarm_Configuration(FunctionalState Cmd);
void WakeUpClockSelect(uint8_t WKUPClkSrcSel);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  app rtc init
 */
void RTC_Configuration(void)
{
    RTC_InitType RTC_InitStructure;
    /* Enable the RTC Clock */
    RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();    
    if((RTC->INITSTS&RTC_INITSTS_INITSF) == 0)
    {
        /* Disable RTC Wakeup and before  */
        RTC_EnableWakeUp(DISABLE);
        RTC_ConfigInt(RTC_INT_WUT, DISABLE);   
        /* Configure the RTC data register and RTC prescaler */
        RTC_InitStructure.RTC_AsynchPrediv = 0x7D; // 7f:23768 , 7d:32k
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
        RTC_InitStructure.RTC_HourFormat   = RTC_24HOUR_FORMAT;
        /* Check on RTC init */
        if (RTC_Init(&RTC_InitStructure) == ERROR)
        {
            log_info(">> !! RTC Prescaler Config failed. !! <<\r\n");
        }
        else{
            log_info(">> !! RTC Prescaler Config success. !! <<\r\n");
        }
        /* Config date and time by default value */
        RTC_DateTimeRegulate();
    }
    else{
        log_info(">> !! RTC has been inited. !! <<\r\n");
    }
    /* Show RTC date and time */
    RTC_DateShow();
    RTC_TimeShow();
    
    /* Low power timer has used RTC wakeup. */
    #ifndef SLEEP_LP_TIMER_ENABLE
    RTC_EnableWakeUp(DISABLE);
    RTC_ConfigInt(RTC_INT_WUT, DISABLE);   
    /* wake up clock select */
    WakeUpClockSelect(1);
    /* wake up timer value */
    RTC_SetWakeUpCounter( 1000 );//400->200ms ,1000->500ms
    
    EXTI9_RTCWKUP_Configuration(ENABLE);
    /* Enable the RTC Wakeup Interrupt */
    RTC_ConfigInt(RTC_INT_WUT, ENABLE);    
    RTC_EnableWakeUp(ENABLE);
    #endif
    
    RTC_AlarmRegulate();
    EXTI8_RTCAlarm_Configuration(ENABLE);
    RTC_AlarmShow();
}


/**
 * @brief  Wake up clock config.
 */
void WakeUpClockSelect(uint8_t WKUPClkSrcSel)
{
    /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
    if (WKUPClkSrcSel == 0x01)
        RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV16);
    else if (WKUPClkSrcSel == 0x02)
        RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV8);
    else if (WKUPClkSrcSel == 0x03)
        RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV4);
    else if (WKUPClkSrcSel == 0x04)
        RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV2);
    else if (WKUPClkSrcSel == 0x05)
        RTC_ConfigWakeUpClock(RTC_WKUPCLK_CK_SPRE_16BITS);
    else if (WKUPClkSrcSel == 0x06)
        RTC_ConfigWakeUpClock(RTC_WKUPCLK_CK_SPRE_17BITS);
}
/**
 * @brief  Config RTC wake up Interrupt.
 */
void EXTI9_RTCWKUP_Configuration(FunctionalState Cmd)
{
    EXTI_InitType EXTI_InitStructure;

    EXTI_ClrITPendBit(EXTI_LINE9);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE9;
#ifdef __TEST_SEVONPEND_WFE_NVIC_DIS__
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /* Enable the RTC WakeUp Interrupt */
    NVIC_EnableIRQ(RTC_IRQn);
}

/**
 * @brief  Config RTC Alarm Interrupt.
 */
void EXTI8_RTCAlarm_Configuration(FunctionalState Cmd)
{
    EXTI_InitType EXTI_InitStructure;

    EXTI_ClrITPendBit(EXTI_LINE8);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE8;
#ifdef __TEST_SEVONPEND_WFE_NVIC_DIS__
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /* Enable the RTC Interrupt */
    NVIC_EnableIRQ(RTC_IRQn);
}

/**
 * @brief  Display the current Date on the Hyperterminal.
 */
void RTC_DateShow(void)
{
    RTC_DateType RTC_DateStructure;

    /* Get the current Date */
    RTC_GetDate(RTC_FORMAT_BIN, &RTC_DateStructure);
    log_info("The current date (WeekDay-Date-Month-Year) is : %0.2d-%0.2d-%0.2d-%0.2d\r\n",
             RTC_DateStructure.WeekDay,
             RTC_DateStructure.Date,
             RTC_DateStructure.Month,
             RTC_DateStructure.Year);
}

/**
 * @brief  Display the current time on the Hyperterminal.
 */
void RTC_TimeShow(void)
{
    RTC_TimeType RTC_TimeStructure;

    /* Get the current Time and Date */
    RTC_GetTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
    log_info("The current time (Hour-Minute-Second) is : %0.2d:%0.2d:%0.2d \r\n",
             RTC_TimeStructure.Hours,
             RTC_TimeStructure.Minutes,
             RTC_TimeStructure.Seconds);
    /* Unfreeze the RTC DAT Register */
    (void)RTC->DATE;
}

/**
 * @brief  Display the current alarm time on the Hyperterminal.
 */
void RTC_AlarmShow(void)
{
    RTC_AlarmType RTC_AlarmStructure;
    /* Get the current Alarm */
    RTC_GetAlarm(RTC_FORMAT_BIN, RTC_A_ALARM, &RTC_AlarmStructure);
    printf("The current alarm is :  %0.2d:%0.2d:%0.2d \r\n",
           RTC_AlarmStructure.AlarmTime.Hours,
           RTC_AlarmStructure.AlarmTime.Minutes,
           RTC_AlarmStructure.AlarmTime.Seconds);
}
/**
 * @brief  RTC date and time regulate with the default value.
 */
ErrorStatus RTC_DateTimeRegulate(void)
{
    RTC_TimeType RTC_TimeDefault;
    RTC_DateType RTC_DateDefault;
    // Date
    RTC_DateDefault.WeekDay = 3;
    RTC_DateDefault.Date    = 20;
    RTC_DateDefault.Month   = 11;
    RTC_DateDefault.Year    = 19;
    // Time
    RTC_TimeDefault.H12     = RTC_AM_H12;
    RTC_TimeDefault.Hours   = 4;
    RTC_TimeDefault.Minutes = 5;
    RTC_TimeDefault.Seconds = 1;    
    
    
    /* Configure the RTC date register */
    if (RTC_SetDate(RTC_FORMAT_BIN, &RTC_DateDefault) == ERROR)
    {
        log_info(">> !! RTC Set Date failed. !! <<\r\n");
        return ERROR;
    }

    /* Configure the RTC time register */
    if (RTC_ConfigTime(RTC_FORMAT_BIN, &RTC_TimeDefault) == ERROR)
    {
        log_info(">> !! RTC Set Time failed. !! <<\r\n");
        return ERROR;
    }
    return SUCCESS;
    
}



/**
 * @brief  RTC alarm regulate with the default value.
 */
ErrorStatus RTC_AlarmRegulate(void)
{
    RTC_AlarmType RTC_AlarmDefault;
    /* Disable the AlarmA */
    RTC_EnableAlarm(RTC_A_ALARM, DISABLE);
    /* Set the Alarm A */
    RTC_AlarmDefault.AlarmTime.H12     = RTC_AM_H12;
    RTC_AlarmDefault.AlarmTime.Hours   = 4;
    RTC_AlarmDefault.AlarmTime.Minutes = 23;
    RTC_AlarmDefault.AlarmTime.Seconds = 05;
    RTC_AlarmDefault.DateWeekMode      = RTC_ALARM_SEL_WEEKDAY_DATE;
    RTC_AlarmDefault.DateWeekValue     = 0x31;
    RTC_AlarmDefault.AlarmMask       = RTC_ALARMMASK_WEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;

    /* Configure the RTC Alarm A register */
    RTC_SetAlarm(RTC_FORMAT_BIN, RTC_A_ALARM, &RTC_AlarmDefault);
    
    /* Enable the RTC Alarm A Interrupt */
    RTC_ConfigInt(RTC_INT_ALRA, ENABLE);
    /* Enable the alarm   */
    RTC_EnableAlarm(RTC_A_ALARM, ENABLE);
   
    return SUCCESS;
}

/**
 * @}
 */
