/** 
 * @file GTP_timer_OC_PwmChange.c 
 * @brief 该文件实现了GTP定时器输出比较通道1的PWM占空比控制功能。 
 *        主要功能包括初始化定时器、GPIO引脚，设置PWM占空比，以及处理定时器相关的中断事件。 
 *        通过定时器输出比较模式PWM1实现占空比可调的PWM信号输出，并利用LED灯状态变化和串口打印信息进行状态反馈。 
 * @author GGBoy  
 * @date  2025年7月4日15:32:58
 * @version V1.0
 */ 
 
#include "GTP_timer_OC_PwmChange.h" 
#include "led.h" 
#include <stdio.h> 
 
// 用于记录PWM占空比 
uint16_t pwmDuty = 0; 
 
// 定时器句柄，用于管理定时器的各种参数和状态 
TIM_HandleTypeDef g_tim_GTP_handle = {0}; 
 
// 定时器输出比较初始化结构体，用于配置输出比较通道的参数 
TIM_OC_InitTypeDef g_sConfigOC = {0}; 
 
/** 
 * @note 
 * 1. PWM周期计算公式：PWM周期 = (ARR + 1) * (PSC + 1) / Ftimer 
 *    其中，ARR为自动重装载寄存器值，PSC为预分频系数，Ftimer为定时器时钟频率。 
 * 2. PWM占空比计算公式：PWM占空比 = Pulse / (ARR + 1) 
 *    其中，Pulse为比较值。 
 * 3. 注意：TIM9/TIM12的时钟是168MHz 
 * @brief 
 * 1. text的周期为100ms，计算方式为 1000 * 16800 / (168 * 10^6) s 
 * 2. 输出比较模式设置为PWM1模式 
 * 3. 输出占空比可调 
 */ 
void GTP_text(void) 
{ 
    // 初始化LED灯 
    LED_init(); 
 
    // 初始化GPIO引脚，作为定时器输出比较通道1的输出引脚 
    GPIO_CH1_Init(GPT_CH1_OC_GPIO_PULL); 
 
    // 输出占空比可调 
    // 初始化定时器输出比较模式为PWM模式，设置自动重装载寄存器值、预分频系数等参数 
    GTP_OC_PwmChange_TimerInit(GTP_TIMX_INT, 1000, 16800, 0, TIM_OCMODE_PWM1); 
 
    // 打印定时器的周期信息 
    printf("g_tim_GTP_handle.period  = %d\r\n", g_tim_GTP_handle.Init.Period); 
    // 提示默认的PWM占空比为50% 
    printf("Default OC PWM duty is : arr/2 !!!!\r\n"); 
 
    while (1) 
    { 
#if GTP_TIMX_OC_PWM_DC_CHANGE_ENABLE 
        /* 轮询输出比较 */ 
        // 检查输出比较标志位是否置位 
        if (__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1)) 
        { 
            // 清除输出比较标志位 
            __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_CC1); 
            // 切换LED1的状态 
            LED1_TOGGLE(); 
            // 打印输出比较轮询信息 
            printf("GTP_TIMX_OC Output Compare Polling !!!\r\n"); 
        } 
        // 检查更新事件标志位是否置位 
        else if (__HAL_TIM_GET_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE)) 
        { 
            // 清除更新事件标志位 
            __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE); 
            // 切换LED0的状态 
            LED0_TOGGLE(); 
            // 打印更新事件轮询信息 
            printf("GTP_TIMX_OC Update Event Polling !!!\r\n"); 
        } 
        { 
            /* code */ 
        } 
#endif 
 
#if GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE 
        // 延时1秒 
        HAL_Delay(1000);  
#endif 
    } 
} 
 
/** 
 * @brief  设置GTP定时器输出比较通道1的PWM占空比 
 * @param  pulse 占空比对应的脉冲宽度 
 * @note   该函数用于设置GTP定时器输出比较通道1的PWM占空比。 
 *         通过修改通道1的比较值来实现PWM占空比的变化。 
 */ 
void set_GTP_OC_PwmChange(uint16_t pulse) 
{ 
    // 设置通道1的比较值为pulse 
    __HAL_TIM_SET_COMPARE(&g_tim_GTP_handle, TIM_CHANNEL_1, pulse); 
} 
 
/** 
 * @brief  初始化GPIO引脚 ---> 作为OC输出通道 
 * @param  pinPull 上拉/下拉配置 
 * @note   该函数用于初始化GTP定时器输出比较通道1的GPIO引脚。 
 *         设置引脚模式、上拉/下拉配置、速度和复用功能。 
 */ 
static void GPIO_CH1_Init(uint8_t pinPull) 
{ 
    // GPIO初始化结构体，用于配置GPIO引脚的参数 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
 
    // 使能GPIO时钟 
    __GPT_CH1_GPIO_CLK_ENABLE(); 
 
    // 设置引脚 
    GPIO_InitStruct.Pin = GTP_CH1_Pin; 
 
    // 设置模式 
    GPIO_InitStruct.Mode = GPT_CH1_GPIO_MODE; 
 
    // 设置上拉/下拉 
    GPIO_InitStruct.Pull = pinPull; 
 
    // 设置速度 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
 
    // 设置复用功能 
    GPIO_InitStruct.Alternate = GPT_CH1_GPIO_AF; 
 
    // 初始化GPIO 
    HAL_GPIO_Init(GPT_CH1_GPIO_Port, &GPIO_InitStruct); 
} 
 
/** 
 * @brief  OCMode->TIM_OCMODE_PWM1  + Pulse[修改]实现PWM的输出 
 * @param  TimInstance 定时器基地址 
 * @param  arr 自动重装载寄存器值 
 * @param  psc 预分频系数 
 * @param  setARPE 自动重载预装载使能 
 * @note   该函数用于初始化定时器输出比较模式为PWM1，并设置自动重装载寄存器和预分频系数。 
 *         <与PwmTOGGLE模式对比>->可以动态修改占空比 
 * @note   主要是修改Pluse的值来改变PWM的占空比。 
 *         __HAL_TIM_SET_COMPARE();可以动态设置占空比 
 */ 
void GTP_OC_PwmChange_TimerInit(TIM_TypeDef* TimInstance, uint16_t arr, uint16_t psc, uint8_t setARPE, uint8_t OCMode) 
{ 
    // 设置定时器实例 
    g_tim_GTP_handle.Instance = TimInstance; 
 
    // 设置预分频系数 
    g_tim_GTP_handle.Init.Prescaler = psc - 1; 
 
    // 设置计数模式为向上计数 
    g_tim_GTP_handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
 
    // 设置自动重装载寄存器值 
    g_tim_GTP_handle.Init.Period = arr - 1; 
 
    // 设置时钟分频系数 
    g_tim_GTP_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
 
    if (setARPE) 
    { 
        // 使能自动重载预装载 
        g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 
    } 
    else 
    { 
        // 禁止自动重载预装载 
        g_tim_GTP_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; 
    } 
 
    /* 初始化定时器 */ 
    if (HAL_TIM_Base_Init(&g_tim_GTP_handle) != HAL_OK) 
    { 
        // 定时器初始化失败，调用错误处理函数 
        Error_Handler(); 
    } 
 
    // 设置输出比较模式为PWM1模式 
    g_sConfigOC.OCMode = OCMode;  
 
    // 设置初始占空比为50% 
    g_sConfigOC.Pulse = arr / 2;           
 
    // 设置输出极性为高电平有效 
    g_sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; 
 
    // 禁用快速模式 
    g_sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; 
 
    /* 初始化输出比较 */ 
    if (HAL_TIM_OC_Init(&g_tim_GTP_handle) != HAL_OK) 
    { 
        // 输出比较初始化失败，调用错误处理函数 
        Error_Handler(); 
    } 
 
    /* 配置输出比较通道 */ 
    if (HAL_TIM_OC_ConfigChannel(&g_tim_GTP_handle, &g_sConfigOC, TIM_CHANNEL_1) != HAL_OK) 
    { 
        // 输出比较通道配置失败，调用错误处理函数 
        Error_Handler(); 
    } 
 
    // TIM9/TIM12只能向上计数，所以不需要设置计数 
#if GTP_TIMX_OC_PWM_DC_CHANGE_ENABLE 
    // 启动输出比较通道 
    HAL_TIM_OC_Start(&g_tim_GTP_handle, TIM_CHANNEL_1); 
    // 启动定时器基础功能 
    HAL_TIM_Base_Start(&g_tim_GTP_handle); 
#endif 
 
#if GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE 
    // 启动输出比较中断 
    HAL_TIM_OC_Start_IT(&g_tim_GTP_handle, TIM_CHANNEL_1); 
    // 启动定时器基础功能中断 
    HAL_TIM_Base_Start_IT(&g_tim_GTP_handle); 
#endif 
} 
 
/** 
 * @brief 定时器底层初始化函数，用于初始化定时器的时钟和中断 
 * @param tim_baseHandle 定时器句柄 
 */ 
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle) 
{ 
    if ((tim_baseHandle->Instance == TIM9) || (tim_baseHandle->Instance == TIM12)) 
    { 
        // 使能定时器时钟 
        __GTP_TIMX_INT_CLK_ENABLE(); 
 
#if GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE 
        // 设置定时器中断优先级 
        HAL_NVIC_SetPriority(GTP_TIMX_INT_IRQn, GTP_TIMX_OC_PWM_DC_CHANGE_PreemptPriority, GTP_TIMX_OC_PWM_DC_CHANGE_SubPriority); 
        // 使能定时器中断 
        HAL_NVIC_EnableIRQ(GTP_TIMX_INT_IRQn); 
#endif 
    } 
} 
 
#if GTP_TIMX_OC_PWM_IT_DC_CHANGE_ENABLE 
/** 
 * @brief 定时器中断处理函数，处理定时器的中断事件 
 */ 
void GTP_TIMX_INT_IRQHandler(void) 
{ 
    // 调用HAL库的中断处理函数，会自动清除中断标志位 
    HAL_TIM_IRQHandler(&g_tim_GTP_handle); 
 
    // 手动再清理一次更新中断标志位 
    __HAL_TIM_CLEAR_IT(&g_tim_GTP_handle, TIM_IT_UPDATE); 
    __HAL_TIM_CLEAR_FLAG(&g_tim_GTP_handle, TIM_FLAG_UPDATE); 
} 
 
/** 
 * @brief 定时器溢出回调函数，在定时器溢出事件发生时被调用 
 * @param htim 定时器句柄 
 */ 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
    if (htim->Instance == GTP_TIMX_INT) 
    { 
        // 切换LED0的状态 
        LED0_TOGGLE(); 
        // 打印定时器溢出事件信息 
        printf("GTP generate event !!\r\n"); 
        // 占空比递增 
        pwmDuty++; 
        // 打印当前占空比信息 
        printf("pwmDuty: %d / %d\r\n", pwmDuty, g_tim_GTP_handle.Init.Period + 1); 
        if (pwmDuty > g_tim_GTP_handle.Init.Period) 
        { 
            // 占空比达到最大值，重置占空比 
            pwmDuty = 0; 
            // 设置占空比为0% 
            set_GTP_OC_PwmChange(0); 
        } 
    } 
} 
 
/** 
 * @brief 输出比较回调函数，在输出比较事件发生时被调用 
 * @param htim 定时器句柄 
 */ 
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
    if (htim->Instance == GTP_TIMX_INT) 
    { 
        // 设置新的占空比 
        set_GTP_OC_PwmChange(pwmDuty); 
    } 
} 
#endif 

