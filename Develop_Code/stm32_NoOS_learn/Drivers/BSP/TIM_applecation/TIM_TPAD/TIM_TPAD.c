#include "TIM_TPAD.h"
#include "led.h"
#include "debug_com.h"


uint32_t threshold_cap_val = 0;     /* [TPAD未按下]充电阈值时间 */
uint32_t TPAD_cap_val = 0;          /* [TPAD按下]充电时间 */

// 中断回调函数中使用的全局变量 
volatile uint8_t capture_flag = 0;

/************************************************************主函数部分********************************************* */
// 循环计数变量 
uint8_t t = 0;

/** 
 * @brief 触摸按键测试初始化函数 
 *        此函数用于初始化串口、触摸按键、LED灯等外设 
 * @param 无 
 * @retval 无 
 */ 
void TPAD_text_init(void)
{
    // 初始化串口通信，波特率为115200 
    UART_Init(115200); 
    // 打印提示信息，表示TIM开始运行 
    printf("TIM开始运行\r\n"); 
    // 初始化触摸按键，预分频系数为8 
    tpad_init(8); 
    // 初始化LED灯 
    led_init(); 
}
/** 
 * @brief 触摸按键测试循环函数 
 *        此函数包含一个无限循环，用于不断扫描触摸按键并控制LED灯的状态 
 * @param 无 
 * @retval 无 
 */ 
void TPAD_text_while(void) 
{ 
    while (1) 
    { 
        /* USER CODE END WHILE */ 
        // 扫描触摸按键，如果成功捕获到一次上升沿 
        if (tpad_scan(0))                    
        { 
            // 翻转LED1的状态 
            LED1_TOGGLE();                  
        } 
 
        // 循环计数变量加1 
        t++; 
        // 当循环计数达到15时 
        if (t == 15) 
        { 
            // 循环计数变量归零 
            t = 0; 
            // 翻转LED0的状态 
            LED0_TOGGLE();                  
        } 
 
        // 延时10ms 
        HAL_Delay(10); 
        /* USER CODE BEGIN 3 */ 
    } 
} 
/************************************************************输入捕获初始化部分********************************************* */


// 定时器句柄 
TIM_HandleTypeDef TIM_TPAD_Handle; 
 
/** 
 * @brief TIM2初始化函数 
 *        此函数用于初始化TIM2定时器，配置时钟源、输入捕获等参数 
 * @param TIM: 定时器实例 
 * @param arr: 自动重装载值 
 * @param psc: 预分频系数 
 * @retval 无 
 */ 
void TIM_TPAD_init(TIM_TypeDef* TIM, uint32_t arr, uint16_t psc) 
{ 
    /* USER CODE BEGIN TIM2_Init 0 */ 
 
    /* USER CODE END TIM2_Init 0 */ 
 
    // 定时器时钟配置结构体 
    TIM_ClockConfigTypeDef sClockSourceConfig = {0}; 
    // 定时器主模式配置结构体 
    TIM_MasterConfigTypeDef sMasterConfig = {0}; 
    // 定时器输入捕获配置结构体 
    TIM_IC_InitTypeDef sConfigIC = {0}; 
 
    /* USER CODE BEGIN TIM2_Init 1 */ 
 
    /* USER CODE END TIM2_Init 1 */ 
    // 指定定时器实例 
    TIM_TPAD_Handle.Instance = TIM; 
    // 配置预分频系数 
    TIM_TPAD_Handle.Init.Prescaler = psc; 
    // 配置计数模式为向上计数 
    TIM_TPAD_Handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
    // 配置自动重装载值 
    TIM_TPAD_Handle.Init.Period = arr; 
    // 配置时钟分频为1 
    TIM_TPAD_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
    // 使能自动重装载预加载 
    TIM_TPAD_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 
    // 初始化定时器基本功能，如果失败则调用错误处理函数 
    if (HAL_TIM_Base_Init(&TIM_TPAD_Handle) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // 配置定时器时钟源为内部时钟 
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; 
    // 应用时钟源配置，如果失败则调用错误处理函数 
    if (HAL_TIM_ConfigClockSource(&TIM_TPAD_Handle, &sClockSourceConfig) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // 初始化定时器输入捕获功能，如果失败则调用错误处理函数 
    if (HAL_TIM_IC_Init(&TIM_TPAD_Handle) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // 配置主输出触发为复位 
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; 
    // 禁用主从模式 
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE; 
    // 应用主模式配置，如果失败则调用错误处理函数 
    if (HAL_TIMEx_MasterConfigSynchronization(&TIM_TPAD_Handle, &sMasterConfig) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // 配置输入捕获极性为上升沿触发 
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; 
    // 配置输入捕获选择为直接输入 
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; 
    // 配置输入捕获预分频为1 
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1; 
    // 配置输入捕获滤波器为0 
    sConfigIC.ICFilter = 0; 
    // 配置定时器通道1的输入捕获参数，如果失败则调用错误处理函数 
    if (HAL_TIM_IC_ConfigChannel(&TIM_TPAD_Handle, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
} 
 
/** 
 * @brief TIM2底层硬件初始化函数 
 *        此函数用于使能定时器时钟、配置GPIO引脚、初始化中断等 
 * @param tim_baseHandle: 定时器句柄 
 * @retval 无 
 */ 
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle) 
{ 
    // GPIO初始化结构体 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
    // 判断定时器实例是否为TIM_TPAD_INT 
    if(tim_baseHandle->Instance == TIM_TPAD_INT) 
    { 
        /* USER CODE BEGIN TIM2_MspInit 0 */ 
 
        /* USER CODE END TIM2_MspInit 0 */ 
        // 使能TIM2时钟 
        TIM_TPAD_INT_CLK_ENABLE(); 
 
        // 使能触摸按键相关时钟 
        __HAL_RCC_TAPD_CLK_ENABLE(); 
        /**TIM2 GPIO Configuration 
        PA5     ------> TIM2_CH1 
        */ 
        // 配置GPIO引脚 
        GPIO_InitStruct.Pin = TAPD_KEY_Pin; 
        // 配置GPIO模式为复用推挽输出 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; 
        // 不使用上下拉电阻 
        GPIO_InitStruct.Pull = GPIO_NOPULL; 
        // 配置GPIO速度为非常高速 
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // 配置GPIO复用功能 
        GPIO_InitStruct.Alternate = TAPD_KEY_Alternate; 
        // 初始化GPIO引脚 
        HAL_GPIO_Init(TAPD_KEY_GPIO_Port, &GPIO_InitStruct); 
 
        // 初始化TIM2中断 
        HAL_NVIC_SetPriority(TIM_TPAD_INT_IRQn, 0, 0); 
        // 使能TIM2中断 
        HAL_NVIC_EnableIRQ(TIM_TPAD_INT_IRQn); 
        /* USER CODE BEGIN TIM2_MspInit 1 */ 
 
        /* USER CODE END TIM2_MspInit 1 */ 
    } 
} 
 
/** 
 * @brief TIM2底层硬件反初始化函数 
 *        此函数用于禁用定时器时钟、反初始化GPIO引脚、禁用中断等 
 * @param tim_baseHandle: 定时器句柄 
 * @retval 无 
 */ 
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle) 
{ 
    // 判断定时器实例是否为TIM_TPAD_INT 
    if(tim_baseHandle->Instance == TIM_TPAD_INT) 
    { 
        /* USER CODE BEGIN TIM2_MspDeInit 0 */ 
 
        /* USER CODE END TIM2_MspDeInit 0 */ 
        // 禁用TIM2时钟 
        TIM_TPAD_INT_CLK_ENABLE(); 
 
        /**TIM2 GPIO Configuration 
        PA5     ------> TIM2_CH1 
        */ 
        // 反初始化GPIO引脚 
        HAL_GPIO_DeInit(TAPD_KEY_GPIO_Port, TAPD_KEY_Pin); 
 
        // 禁用TIM2中断 
        HAL_NVIC_DisableIRQ(TIM_TPAD_INT_IRQn); 
        /* USER CODE BEGIN TIM2_MspDeInit 1 */ 
 
        /* USER CODE END TIM2_MspDeInit 1 */ 
    } 
} 

/** 
 * @brief TIM2全局中断处理函数 
 *        此函数调用HAL库的中断处理函数，并清除中断标志位 
 * @param 无 
 * @retval 无 
 */ 
void TIM_TPAD_INT_IRQHandler(void) 
{ 
    // 调用HAL库的中断处理函数 
    HAL_TIM_IRQHandler(&TIM_TPAD_Handle); 
    // 清除标志位（HAL库可能已处理，建议显式清除） 
    __HAL_TIM_CLEAR_FLAG(&TIM_TPAD_Handle, TIM_FLAG_CC1); 
    /* USER CODE END TIM2_IRQn 1 */ 
} 
 
/* USER CODE BEGIN 1 */ 
/** 
 * @brief 捕获上升沿回调函数 
 *        当检测到上升沿时，设置捕获标志位并停止输入捕获中断 
 * @param htim: 定时器句柄 
 * @retval 无 
 */ 
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) 
{ 
    // 判断是否为TIM_TPAD_Handle定时器 
    if (htim == &TIM_TPAD_Handle) 
    { 
        // 设置捕获标志位 
        capture_flag = 1; 
        // 停止输入捕获中断 
        HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    } 
} 
 
/** 
 * @brief 定时器溢出回调函数 
 *        当定时器溢出时，翻转LED1的状态并设置阈值为最大值 
 * @param htim: 定时器句柄 
 * @retval 无 
 */ 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
    // 判断是否为TIM_TPAD_INT定时器 
    if(TIM_TPAD_INT == htim->Instance) 
    { 
        // 翻转LED1的状态 
        LED1_TOGGLE(); 
        // 设置阈值为最大值 
        threshold_cap_val = TPAD_ARR_MAX_VAL; 
        printf("电平读取失败 \r\n");
    } 
} 
/****************************************************引脚模式翻转部分****************************************************************************** */

/** 
 * @brief 复位引脚---输出为低电平 
 *        此函数用于复位引脚，将其设置为推挽输出并输出低电平，然后重置定时器并将引脚切换为复用推挽输出 
 * @param 无 
 * @retval 无 
 */ 
static void tpad_reset(void) 
{ 
    // GPIO初始化结构体 
    GPIO_InitTypeDef GPIO_InitStruct_reset = {0}; 
 
    // 引脚切换为【普通】推挽输出--输出低电平 
    { 
        // 配置GPIO引脚 
        GPIO_InitStruct_reset.Pin = TAPD_KEY_Pin; 
        // 配置GPIO模式为推挽输出 
        GPIO_InitStruct_reset.Mode = GPIO_MODE_OUTPUT_PP; 
        // 配置GPIO上下拉为下拉 
        GPIO_InitStruct_reset.Pull = GPIO_PULLDOWN; 
        // 配置GPIO速度为非常高速 
        GPIO_InitStruct_reset.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // 初始化GPIO引脚 
        HAL_GPIO_Init(TAPD_KEY_GPIO_Port, &GPIO_InitStruct_reset); 
        // 写入低电平 
        HAL_GPIO_WritePin(TAPD_KEY_GPIO_Port, TAPD_KEY_Pin, GPIO_PIN_RESET); 
        
        // 延时防抖动 
        for(int time = 0; time < 2000; time++) {;} 
    } 
 
    // 重置定时器 
    { 
        // 清除定时器的中断标志位 
        __HAL_TIM_CLEAR_FLAG(&TIM_TPAD_Handle, TIM_FLAG_CC1|TIM_FLAG_UPDATE); 
        // 停止捕获 
        HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_CHANNEL_1); 
        // CNT归零 
        __HAL_TIM_SET_COUNTER(&TIM_TPAD_Handle, 0); 
        // 重启捕获 
        HAL_TIM_IC_Start_IT(&TIM_TPAD_Handle, TIM_CHANNEL_1); 
    } 
    // 引脚切换为【复用】推挽输出--初始化为输入捕获检测 
    { 
        // 配置GPIO引脚 
        GPIO_InitStruct_reset.Pin = TAPD_KEY_Pin; 
        // 配置GPIO模式为复用推挽输出 
        GPIO_InitStruct_reset.Mode = GPIO_MODE_AF_PP; 
        // 不使用上下拉电阻 
        GPIO_InitStruct_reset.Pull = GPIO_NOPULL; 
        // 配置GPIO速度为高速 
        GPIO_InitStruct_reset.Speed = GPIO_SPEED_FREQ_HIGH; 
        // 配置GPIO复用功能 
        GPIO_InitStruct_reset.Alternate = TAPD_KEY_Alternate; 
        // 初始化GPIO引脚 
        HAL_GPIO_Init(TAPD_KEY_GPIO_Port, &GPIO_InitStruct_reset); 
    } 
    // 清除捕获标志位 
    capture_flag = 0; 
} 
/****************************************************功能函数部分****************************************************************************** */
/** 
 * @brief 1.初始化触摸按键 
 *        2.平均滤波获取阈值 
 * @param psc: 预分频系数 
 * @retval 1:初始化成功；0:初始化失败； 
 */ 
/*冒泡排序*/ 
uint8_t tpad_init(uint16_t psc) 
{ 
    // 初始化TIM_TPAD_INT定时器 
    TIM_TPAD_init(TIM_TPAD_INT, 0x100000000 - 1, psc - 1); 
    // 临时变量 
    uint32_t temp = 0; 
    // 存储捕获值的数组 
    uint32_t current_buf[10] = {0}; 
    // 循环10次获取捕获值 
    for(int i = 0; i < 10; i++) 
    { 
        current_buf[i] = tpad_get_val(); 
    } 
 
    // 冒泡排序 
    for(int i = 0; i < 9; i++) 
    { 
        for(int j = i + 1; j < 10; j++) 
        { 
            if(current_buf[i] > current_buf[j]) 
            { 
                temp = current_buf[i]; 
                current_buf[i] = current_buf[j]; 
                current_buf[j] = temp; 
            } 
        } 
    } 
  
    temp = 0; 
    // 取中间6个数的平均值 
    for(int i = 2; i < 8; i++) 
    { 
        temp += current_buf[i]; 
    } 
    // 计算阈值 
    threshold_cap_val = temp / 6; 
 
    // 打印未按下时的充电时间 
    printf("未按下时充电时间 = %d\r\n", threshold_cap_val); 
  
    // 判断阈值是否超过最大值的一半，如果超过则初始化失败 
    if(threshold_cap_val > TPAD_ARR_MAX_VAL / 2) 
    { 
        return 0; 
    } 
    return 1; 
} 
 
/** 
 * @brief 得到定时器捕获值 
 *   @note 如果超时, 则直接返回定时器的计数值 
 *         我们定义超时时间为: TPAD_ARR_MAX_VAL - 500 
 * @param 无 
 * @retval 捕获值/计数值（超时的情况下返回） 
 */ 
static uint16_t tpad_get_val(void) 
{ 
    // 捕获的值 
    uint32_t  capture_value = 0; 
    // 复位引脚 
    tpad_reset(); 
 
    // 开启定时器输入捕获中断 
    HAL_TIM_IC_Start_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    { 
        // 超时计数 
        uint32_t timeout = 0; 
        // 等待捕获标志位被设置 
        while (!capture_flag) 
        { 
            // 判断是否超时 
            if (timeout++ > (TPAD_ARR_MAX_VAL - 500)) 
            { 
                // 停止定时器输入捕获中断 
                HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
                // 返回计数器当前值 
                return __HAL_TIM_GET_COUNTER(&TIM_TPAD_Handle); 
            } 
            // 延时1ms 
            HAL_Delay(1); 
        } 
    } 
    // 关闭定时器输入捕获中断 
    HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    // 清除捕获标志位 
    capture_flag = 0; 
    // 读取输入捕获的当前值 
    capture_value = HAL_TIM_ReadCapturedValue(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    return (uint16_t)capture_value; 
} 
/**
* @brief 读取 n 次, 取最大值[获取按下TPAD时的充电时间]
* @param n ：连续获取的次数
* @retval n 次读数里面读到的最大读数值
*/
static uint16_t tpad_get_maxval(uint8_t n)
{
  uint16_t TPAD_temp = 0;
  uint16_t TPAD_maxval = 0;
  while (n--)
  {
    tpad_reset();
    TPAD_temp = tpad_get_val();
    //printf("按下的值[%d] = %d\r\n",n,current_cnt_buffer[n]);
    if(TPAD_temp > TPAD_maxval )
    {
      
      TPAD_maxval = TPAD_temp;
    }
  }
  return TPAD_maxval;
}
/**
 * @brief 扫描触摸按键-------------比较current充电值和threshold充电值
 * @param mode:扫描模式
 *        cnt：扫描按键按下次数
 *  @arg  0:不支持连续按
 *  @arg  1:支持连续按
 * @retval 0：没按下；1：按下
 */
uint8_t tpad_scan(uint8_t mode)
{
  /*static保证变量不被释放*/
  static uint8_t PTAD_state = 0;   /*0 可以开始检测； !0 ：不能开始检测*/
  uint8_t scan_result = 0;     /*返回值 是否按下*/
  uint8_t TPAD_sample_cnt = DEFAULT_SAMPLE_TIMES;/*默认采样次数 = 3次*/

  /*连续按模式下，采样次数*/
  if(mode)                    
  {
    TPAD_sample_cnt = CONTINUOUS_SAMPLE_TIMES;
    PTAD_state = 0;
  }

  /*获取当前充电值*/
  TPAD_cap_val = tpad_get_maxval(TPAD_sample_cnt);
  //printf("当前值 = %d\r\n",TPAD_cap_val);
  /*大于threshold+最低预测增加充电时间--------->有效按下*/
  if(TPAD_cap_val>(threshold_cap_val+TPAD_GATE_VALUE))
  {
    /*测试手指是否移走，重新按下*/
    if(0 == PTAD_state)
    {
      printf("TPAD按下时时间 = %d\r\n",TPAD_cap_val);
      scan_result = 1;
    }
    /*mode = 0；--->扫描1次-采样3次【1次按下+2次未按下】-->keyen = 0才能再次进行扫描按下*/
    PTAD_state = DEFAULT_SAMPLE_TIMES;
    /*********************** */
  }
  if(PTAD_state>0) 
  {
    PTAD_state--;
  }
  
  return scan_result;

}  
