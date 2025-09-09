#include "keyExti.h"

void CAL_GpioKeyInit(void)
{
    /*调用芯片相关代码：注册GPIO中断*/
    /*对于ST芯片*/
    key_exit_init();
    
    /*对于RTOS: 创建任务*/

}
