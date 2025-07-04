/** 
 * @file GTP_timer_OC_PwmToggle.c 
 * @brief 该文件主要实现了定时器输出比较的PWM翻转模式功能。通过配置定时器的输出比较通道， 
 *        实现PWM信号的输出，并且可以动态调整PWM的翻转周期。同时，文件中包含了LED控制、 
 *        GPIO初始化、定时器初始化以及中断处理等功能，可用于实际的硬件控制场景。 
 * @version 0.1 
 * @date 2025年7月4日16:30:00
 */ 
 
#include "GTP_timer_OC_PwmToggle.h" 
#include "led.h" 
#include <stdio.h> 
 
// 翻转周期 
uint16_t pwmToggleCycle = 0; 
// 每次增加的步长 
uint16_t incrementStep = 100; 
 
// 定时器句柄 
TIM_HandleTypeDef g_tim_GTP_handle = {0}; 
// 输出比较配置结构体 
TIM_OC_InitTypeDef g_sConfigOC = {0}; 
 
/** 
 * @brief 主测试函数，包含LED和GPIO初始化，定时器配置，以及循环处理PWM翻转周期的更新。 
 * @retval None 
 */ 
void GTP_text(void) 
{ 
    // 初始化LED 
    LED_init(); 
    // 初始化GPIO引脚，用于定时器输出比较通道1 
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL); 
 
    // 初始化定时器输出比较通道为PWM翻转模式 
    // 输出比较模式设置为翻转模式，当CNT = CCR时，翻转一次电平 
    GTP_OC_PwmToggle_TimerInit(GTP_TIMX_INT, 10000, 16800, 0, TIM_OCMODE_TOGGLE); 
 
    while (1) 
    { 
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE 
        // 检查输出比较通道1的更新事件标志位 
        while (__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1)) 
        { 
            // 清除输出比较通道1的更新事件标志位 
            __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1); 
 
            // 翻转LED0状态 
            LED0_TOGGLE(); 
 
            // 打印更新事件提示信息 
            printf("GTP_TIMX_OC Update Event Polling !!!\r\n"); 
 
            // 每次更新事件发生时，增加翻转周期 
            pwmToggleCycle += incrementStep; 
 
            // 如果翻转周期超过最大值，则重置为0 
            if (pwmToggleCycle > ARR_MAX_VALUE) 
            { 
                pwmToggleCycle = 0; 
            } 
 
            // 设置输出比较翻转模式的自动重装载寄存器值 
            __Set_GTP_OC_PwmToggle(pwmToggleCycle); 
 
            // 打印当前翻转周期 
            printf("pwmToggleCycle = %d\r\n", pwmToggleCycle); 
        } 
#endif 
 
#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE 
        // 延时1秒 
        HAL_Delay(1000);  
#endif 
    } 
} 
 
/** 
 * @brief 设置输出比较翻转模式的自动重装载寄存器值。 
 * @param Period 自动重装载寄存器值 
 * @retval None 
 */ 
void __Set_GTP_OC_PwmToggle(uint16_t Period) 
{ 
    // 设置自动重装载寄存器值 
    g_tim_GTP_handle.Init.Period = Period - 1; 
 
    // 设置通道1的比较值为pulse，CCR的值为ARR的一半 
    __HAL_TIM_SET_COMPARE(&g_tim_GTP_handle, TIM_CHANNEL_1, (Period - 1) / 2); 
 
    // 初始化定时器输出比较 
    if (HAL_TIM_OC_Init(&g_tim_GTP_handle) != HAL_OK) 
    { 
        // 初始化失败，调用错误处理函数 
        Error_Handler(); 
    } 
} 
 
/** 
 * @brief 初始化GPIO引脚，用于定时器输出比较通道1。 
 * @param pinPull GPIO引脚电平状态 
 * @retval None 
 */ 
static void GPIO_CH1_Init(uint8_t pinPull) 
{ 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
 
    // 使能GPIO时钟 
    __GPT_CH1_GPIO_CLK_ENABLE(); 
 
    // 设置GPIO引脚 
    GPIO_InitStruct.Pin = GTP_CH1_Pin; 
    // 设置GPIO模式 
    GPIO_InitStruct.Mode = GPT_CH1_GPIO_MODE; 
    // 设置GPIO上拉/下拉状态 
    GPIO_InitStruct.Pull = pinPull; 
    // 设置GPIO速度 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    // 设置GPIO复用功能 
    GPIO_InitStruct.Alternate = GPT_CH1_GPIO_AF; 
 
    // 初始化GPIO 
    HAL_GPIO_Init(GPT_CH1_GPIO_Port, &GPIO_InitStruct); 
} 
 
/** 
 * @brief 初始化定时器输出比较通道为PWM翻转模式。 
 * @param TimInstance 定时器基地址 
 * @param arr 自动重装载寄存器值 
 * @param psc 预分频系数 
 * @param setARPE 自动重载预装载使能 
 * @param setOCMode 输出比较模式 
 * @note 该函数会初始化定时器的输出比较通道1为翻转模式，并启动定时器和输出比较通道1。 
 *       当CCR = ARR时，PWM输出电压翻转1次，占空比为高电平ARR，低电平ARR。 
 *       例如：ARR = 10000，psc = 16800，则PWM周期为2s，高电平1s，低电平1s。 
 *       该模式下，g_sConfigOC.pluse 无效。 
 */ 
void GTP_OC_PwmToggle_TimerInit(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc, uint8_t setARPE, uint8_t setOCMode) 
{ 
    // 初始化定时器句柄 
    g_tim_GTP_handle.Instance = TimInstance; 
    // 设置预分频系数 
    g_tim_GTP_handle.Init.Prescaler = psc - 1; 
    // 设置计数模式为向上计数 
    g_tim_GTP_handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
    // 设置自动重装载寄存器值 
    g_tim_GTP_handle.Init.Period = arr - 1; 
    // 设置时钟分频系数 
    g_tim_GTP_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
 
    // 根据setARPE的值，使能或禁止自动重载预装载 
    if (setARPE) 
    { 
        g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 
    } 
    else 
    { 
        g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; 
    } 
 
    // 初始化定时器基础功能 
    if (HAL_TIM_Base_Init(&g_tim_GTP_handle) != HAL_OK) 
    { 
        // 初始化失败，调用错误处理函数 
        Error_Handler(); 
    } 
 
    // 设置输出比较模式 
    g_sConfigOC.OCMode = setOCMode; 
    // 设置输出极性为高电平有效 
    g_sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; 
    // 禁用快速模式 
    g_sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; 
 
    // 初始化输出比较 
    if (HAL_TIM_OC_Init(&g_tim_GTP_handle) != HAL_OK) 
    { 
        // 初始化失败，调用错误处理函数 
        Error_Handler(); 
    } 
 
    // 配置输出PWM的通道 
    if (HAL_TIM_OC_ConfigChannel(&g_tim_GTP_handle, &g_sConfigOC, TIM_CHANNEL_1) != HAL_OK) 
    { 
        // 配置失败，调用错误处理函数 
        Error_Handler(); 
    } 
 
#if GTP_TIMX_OC_PWM_DC_HALF_ENABLE 
    // 启动输出比较通道 
    HAL_TIM_OC_Start(&g_tim_GTP_handle, TIM_CHANNEL_1); 
    // 启动定时器基础功能 
    HAL_TIM_Base_Start(&g_tim_GTP_handle); 
#endif 
 
#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE 
    // 启动输出比较中断 
    HAL_TIM_OC_Start_IT(&g_tim_GTP_handle, TIM_CHANNEL_1); 
#endif 
} 
 
/** 
 * @brief 定时器底层硬件初始化函数，用于使能定时器时钟和配置中断。 
 * @param tim_baseHandle 定时器句柄 
 * @retval None 
 */ 
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle) 
{ 
    if ((tim_baseHandle->Instance == TIM9) || (tim_baseHandle->Instance == TIM12)) 
    { 
        // 使能定时器时钟 
        __GTP_TIMX_INT_CLK_ENABLE(); 
 
#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE 
        // 配置定时器中断优先级 
        HAL_NVIC_SetPriority(GTP_TIMX_INT_IRQn, 0, 0); 
        // 使能定时器中断 
        HAL_NVIC_EnableIRQ(GTP_TIMX_INT_IRQn); 
#endif 
    } 
} 
 
#if GTP_TIMX_OC_PWM_IT_DC_HALF_ENABLE 
/** 
 * @brief 定时器中断处理函数，处理定时器输出比较通道1的中断事件。 
 * @retval None 
 */ 
void GTP_TIMX_INT_IRQHandler(void) 
{ 
    // 调用HAL库的中断处理函数 
    HAL_TIM_IRQHandler(&g_tim_GTP_handle); 
 
    // 手动清除TIM9/TIM12的输出比较通道1中断标志位 
    __HAL_TIM_CLEAR_IT(&g_tim_GTP_handle, TIM_IT_CC1); 
} 
 
/** 
 * @brief 定时器输出比较延时回调函数，在定时器输出比较事件发生时被调用。 
 * @param htim 定时器句柄 
 * @retval None 
 * @note 该函数主要用于处理定时器输出比较事件，例如翻转LED状态以及更新翻转周期。 
 *       当定时器输出比较事件发生时，即CNT等于CCR时，会调用此回调函数。 
 */ 
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
    if (htim->Instance == GTP_TIMX_INT) 
    { 
        // 翻转LED0状态 
        LED0_TOGGLE(); 
 
        // 打印事件提示信息 
        printf("GTP generate event !!\r\n"); 
 
        // 增加翻转周期 
        pwmToggleCycle += incrementStep; 
 
        // 设置输出比较翻转模式的自动重装载寄存器值 
        __Set_GTP_OC_PwmToggle(pwmToggleCycle); 
 
        // 如果翻转周期超过最大值，则重置为0 
        if (pwmToggleCycle > ARR_MAX_VALUE) 
        { 
            pwmToggleCycle = 0; 
        } 
 
        // 打印当前翻转周期 
        printf("pwmToggleCycle = %d\r\n", pwmToggleCycle); 
    } 
} 
#endif 
