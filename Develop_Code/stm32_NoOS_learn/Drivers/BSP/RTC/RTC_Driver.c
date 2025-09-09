#include "RTC_Driver.h"

RTC_HandleTypeDef gRTC_HandleTypeStruct = {0};

static RTC_TimeTypeDef init_Time = {
    .Hours = 23,       // Set initial hour (12-hour format)
    .Minutes = 59,      // Set initial minute
    .Seconds = 52,      // Set initial second
    .TimeFormat = RTC_HOURFORMAT_24, // Set AM/PM format (PM in this case)
    .SubSeconds = 0,   // Subseconds (not used here)
    .SecondFraction = 0,// Second fraction (not used here)xq
    .DayLightSaving = 0,// Daylight saving time (not used here)
    .StoreOperation = 0 // Store operation (not used here)
};

static RTC_DateTypeDef init_Date = {
    .WeekDay = 1,      // Set initial weekday (1=Monday)
    .Month = 6,        // Set initial month (June)
    .Date = 13,         // Set initial date (13th)
    .Year = 25         // Set initial year (2025, represented as 25)
};

RTC_TimeTypeDef getTime = {0}; // Define a global variable to hold the RTC time structure
RTC_DateTypeDef getDate = {0}; // Define a global variable to hold the RTC date structure
uint32_t retry = 200; // Retry count for waiting for LSE to be ready 

// /**
//  * @brief  Initializes the RTC peripheral.
//  * @retval None 
//  */
// void RTC_Test_Init(void)
// {
//     // Initialize the RTC
//     uartInit(USART1, 115200); // Initialize UART for debugging
//     OLED_SSD1306_Init(); // Initialize the OLED display
//     RTC_Init();
    
//     /*关闭RTC和备份寄存器*/
//     //Reset_RtcAndBkpRegister();
    
// }
// void RTC_Test_While(void){
//     while (1)
//     {
//       _RTC_GetDate(&getDate); // Get the current date from the RTC
//       oLED_Show_pagemode_String(0, 0, "Date: ",6);
//       displayNumberOnOLED(0, 30, getDate.Year, 2,6); // Display the year
//       displayNumberOnOLED(0, 50, getDate.Month, 2, 6); // Display the month
//       displayNumberOnOLED(0, 70, getDate.Date, 2, 6); // Display the date
//       displayNumberOnOLED(0, 90, getDate.WeekDay, 1, 6); // Display the weekday

//       _RTC_GetTime(&getTime); // Get the current time from the RTC
//       oLED_Show_pagemode_String(1, 0, "Time: ",6);
//       displayNumberOnOLED(1, 30, getTime.Hours, 2, 6); // Display the hours
//       displayNumberOnOLED(1, 50, getTime.Minutes, 2, 6); // Display the minutes
//       displayNumberOnOLED(1, 70, getTime.Seconds, 2, 6); // Display the seconds
//       displayNumberOnOLED(1, 90, getTime.SubSeconds, 3, 6); // Display the seconds
//     }
// }



/**
 * @brief  Sets the initial time and date in the RTC.
 * *  This function initializes the RTC with a predefined time and date.
 * @retval None
 */
static void RTC_SetTimeAndDate_init(void)
{
    // Set the time in the RTC
    if (_RTC_SetTime(init_Time.Hours, init_Time.Minutes, init_Time.Seconds, init_Time.TimeFormat) != RTC_OK)
    {
        // Handle error
        Error_Handler();
    }

    // Set the date in the RTC
    if (_RTC_SetDate(init_Date.Year, init_Date.Month, init_Date.Date, init_Date.WeekDay) != RTC_OK)
    {
        // Handle error
        Error_Handler();
    }
}
void Reset_RtcAndBkpRegister(void)
{
    // 1. 禁用RTC时钟 
    __HAL_RCC_RTC_DISABLE();
    // 3. 使用库函数等待同步 
    if (HAL_RTC_WaitForSynchro(&gRTC_HandleTypeStruct) != HAL_OK) {
        Error_Handler(); // 同步失败处理 
    }
    
    // 2. 启用备份域访问权限 
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    
    // 3. 强制复位备份域 
    SET_BIT(RCC->BDCR, RCC_BDCR_BDRST);
    CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST);
    
    // 4. 可选：关闭备份域访问权限（增强安全性）
    HAL_PWR_DisableBkUpAccess();
}
/**
 * @brief  Writes a value to a backup register.
 * @param  bkp_reg: The backup register to write to (e.g., RTC_BKP_DR0).    
 * @param  value: The value to write to the backup register.
 * @retval The value written to the backup register.
 */
static uint32_t RTC_WriteBkpRegister(uint32_t bkp_reg, uint32_t value)
{
    //取消写保护
    HAL_PWR_EnableBkUpAccess(); // Enable access to the backup domain
    // Write the value to the backup register
    //写入备份寄存器值
    HAL_RTCEx_BKUPWrite(&gRTC_HandleTypeStruct, bkp_reg, value);
    return value; // Return the written value
}

static uint32_t RTC_ReadBkpRegister(uint32_t bkp_reg)
{
    // Read the backup register value
    //读取备份寄存器值
    return HAL_RTCEx_BKUPRead(&gRTC_HandleTypeStruct, bkp_reg);
}



// Function to set the time in the RTC
//设置RTC时间函数
RTC_State _RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second,uint8_t am_pm)
{
    RTC_TimeTypeDef RTC_TimeTypeSturct = {0};
    // Set the time structure
    //设置小时
    RTC_TimeTypeSturct.Hours = hour;
    //设置分钟
    RTC_TimeTypeSturct.Minutes = minute;
    //设置秒
    RTC_TimeTypeSturct.Seconds = second;
    //设置12小时制或24小时制
    RTC_TimeTypeSturct.TimeFormat = am_pm; // Set AM/PM format

    // Set the time in the RTC
    //使用HAL库函数设置RTC时间
    //注意：RTC格式为二进制格式
    if (HAL_RTC_SetTime(&gRTC_HandleTypeStruct, &RTC_TimeTypeSturct, RTC_FORMAT_BIN) != HAL_OK)
    {
        // Handle error
        printf("Error setting RTC time.\r\n");
        return RTC_ERROR; // Return error state
        //Error_Handler();
    }
    //printf("RTC Time Set: %02d:%02d:%02d\r\n", hour, minute, second);
    return RTC_OK; // Return success state
}

// Function to set the date in the RTC  
//设置RTC日期函数
RTC_State _RTC_SetDate(uint8_t year, uint8_t month, uint8_t day,uint8_t week)
{
    RTC_DateTypeDef RTC_DateTypeSturct = {0};

    // Set the date structure
    //设置年
    RTC_DateTypeSturct.Year = year;
    //设置月
    RTC_DateTypeSturct.Month = month;
    //设置日
    RTC_DateTypeSturct.Date = day;
    //设置星期
    RTC_DateTypeSturct.WeekDay = week; // Set the day of the week (1=Monday, 2=Tuesday, ..., 7=Sunday)
    // Set the date in the RTC
    //使用HAL库函数设置RTC日期
    //注意：RTC格式为二进制格式
    if (HAL_RTC_SetDate(&gRTC_HandleTypeStruct, &RTC_DateTypeSturct, RTC_FORMAT_BIN) != HAL_OK)
    {
        printf("Error setting RTC date.\r\n");
        return RTC_ERROR; // Return error state
        // Handle error
        //Error_Handler();
    }
    //printf("RTC Date Set: %02d/%02d/%02d, Weekday: %d\r\n", year, month, day, week);
    return RTC_OK; // Return success state
}


/**
 * @brief  Gets the current time from the RTC.
 * @param  RTC_TimeTypeSturct: Pointer to the RTC_TimeTypeDef structure to hold the current time.
 * @return RTC_OK if successful, RTC_ERROR otherwise.
 * This function retrieves the current time from the RTC and stores it in the provided structure.
 * It uses the HAL library function HAL_RTC_GetTime to read the time in binary format
 * and returns the status of the operation.
 * @retval RTC_State: The status of the RTC operation (RTC_OK or RTC_ERROR).
 */
RTC_State _RTC_GetTime(RTC_TimeTypeDef *RTC_TimeTypeSturct)
{
    // Get the current time from the RTC
    //使用HAL库函数获取RTC时间
    if (HAL_RTC_GetTime(&gRTC_HandleTypeStruct, RTC_TimeTypeSturct, RTC_FORMAT_BIN) != HAL_OK)
    {
        return RTC_ERROR; // Return error state
        // Handle error
        //Error_Handler();
    }
    return RTC_OK; // Return success state
}

// Function to get the current date from the RTC
//获取RTC当前日期函数
//注意：RTC格式为二进制格式
RTC_State _RTC_GetDate(RTC_DateTypeDef *RTC_DateTypeSturct)
{
    // Get the current date from the RTC
    //使用HAL库函数获取RTC日期
    if (HAL_RTC_GetDate(&gRTC_HandleTypeStruct, RTC_DateTypeSturct, RTC_FORMAT_BIN) != HAL_OK)
    {
        return RTC_ERROR; // Return error state
        // Handle error
        //Error_Handler();
    }
    return RTC_OK; // Return success state
}



// Function to initialize the RTC
//初始化RTC函数
void RTC_Init(void)
{
    //存储备份寄存器的初始值
    // Initialize the RTC peripheral
    uint32_t BkpInitValue = 0;
    {
        __HAL_RCC_RTC_ENABLE();     /* RTC使能 */
        HAL_PWR_EnableBkUpAccess(); /* 取消备份区域写保护 */
        __HAL_RCC_PWR_CLK_ENABLE(); /* 使能电源时钟PWR */
    }
    // Initialize the RTC handle structure
    gRTC_HandleTypeStruct.Instance = RTC;
    // Set the RTC parameters
    // Set the hour format to 24-hour
    gRTC_HandleTypeStruct.Init.HourFormat = RTC_HOURFORMAT_24;
    // 127*255 = 32767, which is the maximum value for the RTC prescaler
    {
        // Set the asynchronous prescaler value
        gRTC_HandleTypeStruct.Init.AsynchPrediv = 127;
        // Set the synchronous prescaler value
        gRTC_HandleTypeStruct.Init.SynchPrediv = 255;
    }
    // Set the output to disable
    gRTC_HandleTypeStruct.Init.OutPut = RTC_OUTPUT_DISABLE;
    // Set the output polarity to high
    gRTC_HandleTypeStruct.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    // Set the output type to open-drain
    gRTC_HandleTypeStruct.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    /*Note:这句话一定要在RTC_Init前使用--->因为RTC_Init会调用MSP写入数值 */
    BkpInitValue = RTC_ReadBkpRegister(RTC_BKP_DR0); // Read the initial value of backup register 0
    // Initialize the RTC
    if (HAL_RTC_Init(&gRTC_HandleTypeStruct) != HAL_OK)
    {
        // Initialization failed, handle the error
        Error_Handler();
    }

    if (BkpInitValue != 0x5050 && BkpInitValue != 0x5051) // Check if the backup register is not initialized
    {
        // If not initialized, set the time and date
        RTC_SetTimeAndDate_init();
    }
    else
    {
        // Backup register is already initialized, no need to set time and date
        printf("RTC already initialized.\r\n");
    }
    
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{
    // Initialize the RTC MSP (MCU Support Package)
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    // Initialize the RCC (Reset and Clock Control) structure
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    if(rtcHandle->Instance==RTC)
    {
        //使能RTC时钟
        __HAL_RCC_RTC_ENABLE(); // Enable the RTC clock
        //使能电源时钟PWR和BKP时钟[现在已经合并到RTC时钟中]
        __HAL_RCC_PWR_CLK_ENABLE(); // Enable the PWR clock
        //取消备份域写保护
        HAL_PWR_EnableBkUpAccess(); // Allow access to the backup domain
        //尝试开启LSE时钟源--看看外部时钟源是否正常
        {
            // Configure the RTC clock source
            RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; // Use LSE as the oscillator type
            RCC_OscInitStruct.LSEState = RCC_LSE_ON; // Enable LSE
            RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // No PLL used
            if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
            {
                Error_Handler(); // Handle error if oscillator configuration fails
            }
            while(retry-- && __HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET) // Wait until LSE is ready
            {
               printf("Waiting for LSE to be ready...\r\n");
            }
        }
        /* 如果LSE未准备就绪，则使用LSI作为后备 */
        if(retry == 0){
            /*开启LSI时钟源*/
            {
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;  // Use LSI as a fallback if LSE is not ready
                RCC_OscInitStruct.LSIState = RCC_LSI_ON; // Enable LSI as a fallback
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // No PLL used
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    Error_Handler(); // Handle error if oscillator configuration fails
                }
            }
            /* 配置RTC时钟源为LSI */
            {
                PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; // Select the peripheral clock for RTC
                PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI; // Select LSI as the RTC clock source
                if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
                {
                    Error_Handler(); // Handle error if peripheral clock configuration fails
                }
            }
            //使能RTC时钟
            __HAL_RCC_RTC_ENABLE(); // Enable the RTC clock
            //检测备份寄存器是否已初始化
            //如果未初始化，则写入一个魔术数字到备份寄存器0
            RTC_WriteBkpRegister(RTC_BKP_DR0, 0x5051); // Write a magic number to backup register 0 to indicate initialization
        }
        else{
            /*开启LSE时钟源*/
            {
                RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
                RCC_OscInitStruct.LSEState = RCC_LSE_ON; // Enable LSE
                RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // No PLL used
                if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                {
                    Error_Handler(); // Handle error if oscillator configuration fails
                }
            }
            /* 配置RTC时钟源为LSE */
            {
                PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; // Select the peripheral clock for RTC
                PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; // Select LSE as the RTC clock source
                if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
                {
                    Error_Handler(); // Handle error if peripheral clock configuration fails
                }
            }
            //使能RTC时钟
            __HAL_RCC_RTC_ENABLE(); // Enable the RTC clock
            //检测备份寄存器是否已初始化
            //如果未初始化，则写入一个魔术数字到备份寄存器0
            RTC_WriteBkpRegister(RTC_BKP_DR0, 0x5050); // Write a magic number to backup register 0 to indicate initialization
        }

        
    }
}
