#include "PWR_Driver.h"
#include "debug_com.h"
#include "led.h"


// 定义一个字符串数组，用于验证停止模式下数据的保留情况 
uint8_t gTest[] = "Verify the retention of STOP pattern data............"; 


/*********************************** 睡眠模式实验程序 *******************************************/ 
 
// 初始化函数，用于设置串口、LED，并进入睡眠模式 
void PWR_SleepMode_interrupt_uartRX_Init_Test(void) 
{ 
    // 初始化USART1串口，波特率设置为115200 
    uartInit(USART1, 115200); 
 
    // 初始化LED的GPIO引脚 
    LED_init(); 
 
    // 提示信息：开机就进入睡眠模式 
    // LED1亮起，表示系统已准备好进入睡眠模式 
    LED1(1);        
 
    // 注意：当进入睡眠模式后，不会执行【PWR_SleepMode_Exti_Wakeup_Init_while】代码 
    // 这里使用WEAK_UP按键通过EXTI0外部中断来唤醒系统 
    // 只有当外部中断触发唤醒时，才会执行【PWR_SleepMode_Exti_Wakeup_Init_while】代码 
    PWR_Enter_Sleep_Mode(); // 进入睡眠模式 
} 
 
// 当退出睡眠模式后执行的循环函数 
void PWR_SleepMode_interrupt_uartRX_Init_while(void) 
{ 
    // 提示信息：如果退出睡眠模式 
    // 进入无限循环 
    while (1) 
    { 
        // LED1熄灭 
        LED1(0); 
 
        // 简单的延时循环，用于控制LED闪烁频率 
        for(uint32_t i = 0; i < 10000000; i++); 
 
        // LED1亮起 
        LED1(1); 
 
        // 简单的延时循环，用于控制LED闪烁频率 
        for(uint32_t i = 0; i < 10000000; i++); 
    } 
} 
 
// 进入睡眠模式的函数 
void PWR_Enter_Sleep_Mode(void) 
{ 
    // 注意：一定不能省略这句，因为MCU初始化会自动调用滴答定时器 
    // 暂停滴答时钟，防止通过滴答时钟中断唤醒系统 
    HAL_SuspendTick();  
 
    // 进入睡眠模式，主稳压器开启，使用WFI指令进入 
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
 
    // 注意：一定不能省略这句，因为MCU初始化会自动调用滴答定时器 
    // 恢复滴答时钟 
    HAL_ResumeTick();  
} 


/***********************************停止模式实验程序*******************************************/ 

// 定义停止模式状态标志，初始化为0 
uint8_t stop_Status = 0; 
 
/** 
 * @brief 初始化停止模式并通过外部中断（KEY0按键）唤醒的测试函数 
 * 
 * 此函数用于初始化串口、LED，配置停止模式和唤醒按键， 
 * 并使系统进入停止模式。 
 */ 
void PWR_StopMode_Exti_KEY0_Init_Test(void) 
{ 
    // 初始化USART1串口，波特率为115200 
    uartInit(USART1, 115200); 
    // 初始化LED的GPIO引脚 
    LED_init(); 
    // 初始化停止模式和外部中断（KEY0按键） 
    PWR_StopMode_Exti_KEY0_Init(); 
 
    // 开机提示：LED1亮起，表示系统已准备好进入停止模式 
    LED1(1);        
    
    /** 
     * 注意：当进入停止模式后，不会执行【PWR_StopMode_Exti_KEY0_Init_while】代码 
     *       这里使用KEY0按键通过外部中断（EXTI0）唤醒系统 
     *       只有当外部中断唤醒时，才会执行【PWR_StopMode_Exti_KEY0_Init_while】代码 
     */ 
    // 进入停止模式 
    PWR_Enter_Stop_Mode(); 
} 
 
/** 
 * @brief 停止模式被唤醒后的循环处理函数 
 * 
 * 当系统从停止模式退出后，此函数会进入一个无限循环， 
 * 使LED1交替闪烁，并在必要时重新配置系统时钟和验证数据保留情况。 
 */ 
void PWR_StopMode_Exti_KEY0_Init_while(void) 
{ 
    // 如果退出停止模式 
    // 用户代码开始，进入无限循环 
    while (1) 
    { 
        // LED1熄灭 
        LED1(0); 
        // 简单的延时循环 
        for(uint32_t i = 0; i < 10000000; i++); 
        // LED1亮起 
        LED1(1); 
        // 简单的延时循环 
        for(uint32_t i = 0; i < 10000000; i++); 
 
        if(stop_Status != 0) 
        { 
            /** 
             * 注意：停止模式后，主时钟会复位为8MHz（未倍频时钟）， 
             *       这会导致LED闪烁变慢。这里重新倍频设置时钟 
             */ 
            // 重新配置系统时钟 
            SystemClock_Config(); 
 
            /** 
             * 如果停止模式后，能正确显示gTest的值， 
             * 那么说明停止模式不会丧失数据保存功能 
             */ 
            printf("data save is : %s \r\n", gTest); 
            // 清除停止模式状态标志 
            stop_Status = 0; 
        } 
    } 
} 
 
/** 
 * @brief 进入停止模式的函数 
 * 
 * 此函数用于进入停止模式，在进入前暂停滴答时钟， 
 * 进入后恢复滴答时钟，防止滴答时钟中断唤醒系统。 
 */ 
void PWR_Enter_Stop_Mode(void) 
{ 
    /** 
     * 注意：一定不能省略这句，因为MCU初始化会自动调用滴答定时器 
     * 暂停滴答时钟，防止通过滴答时钟中断唤醒系统 
     */ 
    HAL_SuspendTick(); 
    // 进入停止模式，主稳压器开启，使用WFI指令进入 
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
 
    /** 
     * 注意：一定不能省略这句，因为MCU初始化会自动调用滴答定时器 
     * 恢复滴答时钟 
     */ 
    HAL_ResumeTick(); 
} 
 
/** 
 * @brief 初始化停止模式和外部中断（KEY0按键）的函数 
 * 
 * 此函数用于配置唤醒按键的GPIO引脚和外部中断。 
 */ 
void PWR_StopMode_Exti_KEY0_Init(void) 
{ 
    // 定义GPIO初始化结构体 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
    // 使能唤醒GPIO端口的时钟 
    __PWR_KEY0_GPIO_CLK_ENABLE(); 
    // 设置唤醒引脚 
    GPIO_InitStruct.Pin = PWR_KEY0_PIN; 
    // 设置唤醒引脚的工作模式 
    GPIO_InitStruct.Mode = PWR_KEY0_GPIO_MODE; 
    // 设置唤醒引脚的上拉/下拉配置 
    GPIO_InitStruct.Pull = PWR_KEY0_GPIO_PULL; 
    // 初始化唤醒GPIO引脚 
    HAL_GPIO_Init(PWR_KEY0_GPIO_PORT, &GPIO_InitStruct); 
 
    // 设置外部中断（KEY0）的优先级 
    HAL_NVIC_SetPriority(PWR_KEY0_EXTI_IRQn, PWR_KEY0_EXTI_PreemptPriority, PWR_KEY0_EXTI_SubPriority); 
    // 使能外部中断（KEY0） 
    HAL_NVIC_EnableIRQ(PWR_KEY0_EXTI_IRQn); 
} 
 
/** 
 * @brief 外部中断（KEY0）处理函数 
 * 
 * 此函数用于处理唤醒中断，调用HAL库的GPIO外部中断处理函数清除中断标志。 
 */ 
void PWR_KEY0_EXTI_IRQHandler(void) 
{ 
    // 调用HAL库的GPIO外部中断处理函数清除中断标志 
    HAL_GPIO_EXTI_IRQHandler(PWR_KEY0_PIN); 
} 
 
/** 
 * @brief GPIO外部中断回调函数 
 * 
 * 当外部中断触发时，此函数会被调用，检查是否为唤醒引脚的中断， 
 * 并进行相应的处理，如切换LED状态和设置停止模式状态标志。 
 */ 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{ 
    // 检查是否为唤醒引脚的中断 
    if (GPIO_Pin == PWR_KEY0_PIN) 
    { 
        // 切换LED0状态，以指示唤醒事件 
        LED0_TOGGLE(); 
        // 设置停止模式状态标志 
        stop_Status = 1; 
    } 
} 

/***********************************待机模式*******************************************/ 
 
/** 
 * @brief 初始化函数，用于设置串口、LED，并进入待机模式 
 * 
 * 此函数会初始化USART1串口和LED的GPIO引脚，配置唤醒引脚， 
 * 并最终使系统进入待机模式。进入待机模式后，后续代码不会执行。 
 */ 
void PWR_StandbyMode_WAKEUP_Init_Test(void) 
{ 
    // 初始化USART1串口，波特率设置为115200 
    uartInit(USART1, 115200); 
 
    // 初始化LED的GPIO引脚 
    LED_init(); 
 
    // 初始化唤醒引脚相关配置 
    PWR_WAKEUP_Init(); 
 
    // 切换LED0的状态，可用于指示系统进入初始化流程 
    LED0_TOGGLE(); 
    PWR_Enter_Standby(); // 进入待机模式 
} 
 
/** 
 * @note : 因为Standby相当于是关闭并且复位，调用PWR_Enter_Standby();进入待机模式后， 
 * 其后的内容都不会被执行，所以while循环也不会执行。 
 * 
 * @brief 当退出待机模式后执行的循环函数 
 * 
 * 系统从待机模式被唤醒后，会进入此无限循环，通过控制LED1闪烁来提示系统已唤醒。 
 */ 
void PWR_StandbyMode_WAKEUP_Init_while(void) 
{ 
    // 提示信息：如果退出待机模式，进入无限循环 
    while (1) 
    { 
        // LED1熄灭 
        LED1(0); 
 
        // 简单的延时循环，用于控制LED闪烁频率 
        for(uint32_t i = 0; i < 10000000; i++); 
 
        // LED1亮起 
        LED1(1); 
 
        // 简单的延时循环，用于控制LED闪烁频率 
        for(uint32_t i = 0; i < 10000000; i++); 
    } 
} 
 
/** 
 * @brief 初始化唤醒引脚相关配置 
 * 
 * 此函数会使能唤醒GPIO端口的时钟，配置唤醒引脚的工作模式和上拉/下拉状态。 
 */ 
void PWR_WAKEUP_Init(void) 
{ 
    // 定义GPIO初始化结构体，并初始化为0 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
 
    // 使能唤醒GPIO端口的时钟 
    __PWR_WAKEUP_GPIO_CLK_ENABLE(); 
 
    // 设置唤醒引脚 
    GPIO_InitStruct.Pin = PWR_WAKEUP_PIN; 
 
    // 设置唤醒引脚的工作模式 
    GPIO_InitStruct.Mode = PWR_WAKEUP_GPIO_MODE; 
 
    // 设置唤醒引脚的上拉/下拉配置 
    GPIO_InitStruct.Pull = PWR_WAKEUP_GPIO_PULL; 
 
    // 初始化唤醒GPIO引脚 
    HAL_GPIO_Init(PWR_WAKEUP_GPIO_PORT, &GPIO_InitStruct); 
} 
 
/** 
 * @brief 使系统进入待机模式 
 * 
 * 此函数会等待WKUP_KD引脚释放，使能电源时钟， 
 * 使能WAKE_UP引脚的唤醒功能，清除唤醒标志，最终使系统进入待机模式。 
 */ 
void PWR_Enter_Standby(void) 
{ 
    // 等待WKUP_KD引脚释放 
    while(WKUP_KD); 
 
    // 使能电源时钟 
    __HAL_RCC_PWR_CLK_ENABLE(); 
 
    /** 
     * @note: 使能WAKE_UP引脚的唤醒功能 
     * @brief: 这里WAKE_UP引脚不需要自己配置，HAL库自动配置 
     */ 
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); 
 
    // 将唤醒标志清零 (Clear wakeup flag) 
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); 
 
    // 进入待机模式 
    HAL_PWR_EnterSTANDBYMode(); 
} 
