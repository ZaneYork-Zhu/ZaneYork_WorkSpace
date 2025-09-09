#include "cal_time.h"
#include "kal_time.h"
TIME_T KAL_GetTime(void)
{
    /*对于裸机：注册中断*/
    return CAL_GetTime();
    /*对于RTOS: 创建任务*/

}

void KAL_Delay(TIME_T delay_ms)
{
    /*对于裸机：注册中断*/
    CAL_Delay(delay_ms);
    /*对于RTOS: 创建任务*/
    // vTaskDelay(pdMS_TO_TICKS(delay));
}

