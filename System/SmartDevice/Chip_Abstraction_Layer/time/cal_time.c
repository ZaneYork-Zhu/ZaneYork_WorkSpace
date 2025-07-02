#include "main.h"
#include "input_system.h"
#include "cal_time.h"
TIME_T CAL_GetTime(void)
{
    /*对于裸机：注册中断*/
    return HAL_GetTick();
    /*对于RTOS: 创建任务*/

}
#
void CAL_Delay(TIME_T delay_ms)
{
    /*对于裸机：注册中断*/
    HAL_Delay(delay_ms);
    /*对于RTOS: 创建任务*/
    // vTaskDelay(pdMS_TO_TICKS(delay_ms));
}
