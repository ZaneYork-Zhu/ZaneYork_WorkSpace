#ifndef     RE__RTC_H__
#define     RE__RTC_H__

#include "main.h"
/* USER CODE BEGIN Includes */

typedef enum {
    RTC_OK = HAL_OK,       /*!< RTC operation successful */
    RTC_ERROR = HAL_ERROR,    /*!< RTC operation failed */
}RTC_State;


extern RTC_HandleTypeDef gRTC_HandleTypeStruct;

// Function to set the time in the RTC
//设置RTC时间函数
RTC_State _RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second,uint8_t am_pm);
//设置RTC日期函数
RTC_State _RTC_SetDate(uint8_t year, uint8_t month, uint8_t day,uint8_t week);
//获取RTC当前时间函数
RTC_State _RTC_GetTime(RTC_TimeTypeDef *RTC_TimeTypeSturct);
//获取RTC当前日期函数
RTC_State _RTC_GetDate(RTC_DateTypeDef *RTC_DateTypeSturct);
//初始化RTC函数
void RTC_Test_Init(void);
void RTC_Test_While(void);
void RTC_Init(void);
void Reset_RtcAndBkpRegister(void);
static void RTC_SetTimeAndDate_init(void);
static uint32_t RTC_WriteBkpRegister(uint32_t bkp_reg, uint32_t value);
static uint32_t RTC_ReadBkpRegister(uint32_t bkp_reg);

#endif /* __RTC_H__ */
