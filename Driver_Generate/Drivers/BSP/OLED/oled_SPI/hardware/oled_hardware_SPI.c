// 包含OLED硬件SPI相关的头文件 
#include "oled_hardware_SPI.h" 
 
// 定义一个全局的SPI句柄结构体变量，并初始化为0 
SPI_HandleTypeDef gSPI_HandleTypeSturct = {0}; 
// 定义一个全局的DMA句柄结构体变量，并初始化为0
DMA_HandleTypeDef ghdma_SPI_tx = {0} ;


/** 
 * @brief  初始化OLED硬件SPI的GPIO和SPI参数 
 * @param  spiMode: SPI的工作模式 
 * @param  SPIx: SPI外设的基地址 
 * @param  baudPrescaler: SPI波特率预分频的值 
 */ 
void oled_hardware_SPI_GPIO_Init(uint8_t spiMode, SPI_TypeDef *SPIx, uint32_t baudPrescaler) 
{ 
    // 设置SPI基地址 
    gSPI_HandleTypeSturct.Instance = SPIx; 
    // 设置SPI的主从模式为主模式 
    gSPI_HandleTypeSturct.Init.Mode = SPI_MODE_MASTER; 
    // 设置SPI的数据方向为双线模式 
    gSPI_HandleTypeSturct.Init.Direction = SPI_DIRECTION_2LINES; 
    // 设置SPI的数据大小为8bit 
    gSPI_HandleTypeSturct.Init.DataSize = SPI_DATASIZE_8BIT; 
    // 根据spiMode设置SPI的空闲电平 
    gSPI_HandleTypeSturct.Init.CLKPolarity = spiHardwareMode[spiMode][0]; 
    // 根据spiMode设置SPI的采样位置 
    gSPI_HandleTypeSturct.Init.CLKPhase = spiHardwareMode[spiMode][1]; 
    // 设置SPI的NSS信号管理为软件管理 
    gSPI_HandleTypeSturct.Init.NSS = SPI_NSS_SOFT; 
    // 设置SPI波特率预分频的值 
    gSPI_HandleTypeSturct.Init.BaudRatePrescaler = baudPrescaler; 
    // 设置SPI的bit流方向为MSB（最高位优先） 
    gSPI_HandleTypeSturct.Init.FirstBit = SPI_FIRSTBIT_MSB; 
    // 关闭SPI的TI模式 
    gSPI_HandleTypeSturct.Init.TIMode = SPI_TIMODE_DISABLE; 
    // 关闭SPI的硬件CRC校验 
    gSPI_HandleTypeSturct.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; 
    // 设置SPI的CRC计算多项式为7 
    gSPI_HandleTypeSturct.Init.CRCPolynomial = (uint8_t)7; 
    // 调用HAL库函数初始化SPI 
    HAL_SPI_Init(&gSPI_HandleTypeSturct); 
 
    // 使能OLED硬件SPI的片选信号（拉高） 
    OLED_HARDWARE_SPI_CS(1); 
    // 复位OLED硬件SPI的RET引脚（拉低） 
    OLED_HARDWARE_SPI_RET(0); 
    // 延时一段时间，用于OLED的复位操作 
    for (uint16_t i = 0; i < 1000; i++) 
    { 
        for (uint16_t j = 0; j < 1000; j++) 
        { 
            // 空循环，用于延时 
        } 
    } 
    // 释放OLED硬件SPI的RET引脚（拉高） 
    OLED_HARDWARE_SPI_RET(1); 
} 
 
/** 
 * @brief  SPI的底层硬件初始化函数，由HAL库调用 
 * @param  hspi: SPI句柄指针 
 */ 
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) 
{ 
#if OLED_HARDWARE_SPI_ENABLE 
    // 如果使能了OLED硬件SPI，并且当前初始化的SPI实例是OLED硬件SPI 
    if (hspi->Instance == OLED_HARDWARE_SPI) 
    { 
        // 定义一个GPIO初始化结构体变量，并初始化为0 
        GPIO_InitTypeDef GPIO_InitTypeStruct = {{0}}; 
        // 使能SPI相关GPIO的时钟 
        { 
            // 使能SPI片选（CS）引脚的时钟 
            __OLED_HARDWARE_MODEx_SPI_CS_CLK_ENABLE(); 
            // 使能SPI时钟（SCK）引脚的时钟 
            __OLED_HARDWARE_MODEx_SPI_SCK_CLK_ENABLE(); 
            // 使能SPI主输出从输入（MOSI）引脚的时钟 
            __OLED_HARDWARE_MODEx_SPI_MOSI_CLK_ENABLE(); 
            // 使能SPI复位（RET）引脚的时钟 
            __OLED_HARDWARE_MODEx_SPI_RET_CLK_ENABLE(); 
            // 使能SPI数据/命令选择（DC）引脚的时钟 
            __OLED_HARDWARE_MODEx_SPI_DC_CLK_ENABLE(); 
        } 
        // 使能SPI外设的时钟 
        __OLED_HARDWARE_HAL_RCC_SPI_CLK_ENABLE();
#if _HARDWARE_SPI_DMA 
        SPI_DMA_CLK_ENABLE();
#endif
 
        // 初始化SPI片选（CS）引脚 
        GPIO_InitTypeStruct.Pin = OLED_HARDWARE_MODEx_SPI_CS_Pin; 
        // 设置CS引脚的工作模式 
        GPIO_InitTypeStruct.Mode = OLED_HARDWARE_MODEx_SPI_CS_GPIO_MODE; 
        // 上拉CS引脚，确保在空闲状态下有确定的电平 
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
        // 设置CS引脚的速度为非常高，以满足高速SPI通信需求 
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // 调用HAL库函数初始化CS引脚 
        HAL_GPIO_Init(OLED_HARDWARE_MODEx_SPI_CS_GPIO_Port, &GPIO_InitTypeStruct); 
 
        // 初始化SPI时钟（SCK）引脚 
        GPIO_InitTypeStruct.Pin = OLED_HARDWARE_MODEx_SPI_SCK_Pin; 
        // 设置SCK引脚为复用推挽输出模式，用于SPI时钟信号输出 
        GPIO_InitTypeStruct.Mode = GPIO_MODE_AF_PP; 
        // 设置SCK引脚的速度为高，以适应SPI通信速率 
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
        // 上拉SCK引脚，确保信号稳定 
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
        // 设置SCK引脚的复用功能，使其与SPI功能对应 
        GPIO_InitTypeStruct.Alternate = OLED_HARDWARE_MODEx_SPI_SCK_GPIO_AF; 
        // 调用HAL库函数初始化SCK引脚 
        HAL_GPIO_Init(OLED_HARDWARE_MODEx_SPI_SCK_GPIO_Port, &GPIO_InitTypeStruct); 
 
        // 初始化SPI主输出从输入（MOSI）引脚 
        GPIO_InitTypeStruct.Pin = OLED_HARDWARE_MODEx_SPI_MOSI_Pin; 
        // 设置MOSI引脚为复用推挽输出模式，用于主设备向从设备发送数据 
        GPIO_InitTypeStruct.Mode = GPIO_MODE_AF_PP; 
        // 设置MOSI引脚的速度为高，以满足高速数据传输要求 
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
        // 上拉MOSI引脚，保证信号质量 
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
        // 设置MOSI引脚的复用功能，使其与SPI功能对应 
        GPIO_InitTypeStruct.Alternate = OLED_HARDWARE_MODEx_SPI_MOSI_GPIO_AF; 
        // 调用HAL库函数初始化MOSI引脚 
        HAL_GPIO_Init(OLED_HARDWARE_MODEx_SPI_MOSI_GPIO_Port, &GPIO_InitTypeStruct); 
 
        // 初始化SPI复位（RET）引脚 
        GPIO_InitTypeStruct.Pin = OLED_HARDWARE_MODEx_SPI_RET_Pin; 
        // 设置RET引脚的工作模式 
        GPIO_InitTypeStruct.Mode = OLED_HARDWARE_MODEx_SPI_RET_GPIO_MODE; 
        // 上拉RET引脚，确保复位信号稳定 
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
        // 设置RET引脚的速度为非常高，以快速响应复位操作 
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // 调用HAL库函数初始化RET引脚 
        HAL_GPIO_Init(OLED_HARDWARE_MODEx_SPI_RET_GPIO_Port, &GPIO_InitTypeStruct); 
 
        // 初始化SPI数据/命令选择（DC）引脚 
        GPIO_InitTypeStruct.Pin = OLED_HARDWARE_MODEx_SPI_DC_Pin; 
        // 设置DC引脚的工作模式 
        GPIO_InitTypeStruct.Mode = OLED_HARDWARE_MODEx_SPI_DC_GPIO_MODE; 
        // 上拉DC引脚，确保信号稳定 
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
        // 设置DC引脚的速度为非常高，以快速切换数据/命令模式 
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // 调用HAL库函数初始化DC引脚 
        HAL_GPIO_Init(OLED_HARDWARE_MODEx_SPI_DC_GPIO_Port, &GPIO_InitTypeStruct); 
    } 
#endif 
 
#if _HARDWARE_SPI_DMA 
    /* 
    * 以下代码是对 SPI 的发送 DMA（直接内存访问）进行初始化配置 
    * DMA 可以在不经过 CPU 干预的情况下，实现外设与内存之间的数据传输，提高系统效率 
    */ 
    // 使用DMA处理SPI的发送数据传输 
    ghdma_SPI_tx.Instance = SPI_TX_DMA; 
    // 使用DMA处理SPI的发送数据传输的通道 
    ghdma_SPI_tx.Init.Channel = SPI_TX_CHANNEL; 
    // 设置DMA数据传输的方向为从内存到外设 
    // 这意味着数据将从内存中读取，并发送到SPI外设
    ghdma_SPI_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; 
    // 禁用外设地址自增功能 
    // 因为SPI在传输数据时，其数据寄存器地址通常是固定的，不需要自动增加 
    ghdma_SPI_tx.Init.PeriphInc = DMA_PINC_DISABLE; 
    // 启用内存地址自增功能 
    // 这样在每次数据传输后，内存地址会自动增加，以便将接收到的数据依次存储到连续的内存空间 
    ghdma_SPI_tx.Init.MemInc = DMA_MINC_ENABLE; 
    // 设置外设数据对齐方式为字节对齐 
    // 表示从SPI外设读取的数据是以字节为单位进行传输的 
    ghdma_SPI_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; 
    // 设置内存数据对齐方式为字节对齐 
    // 意味着接收到的数据将以字节为单位存储到内存中 
    ghdma_SPI_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; 
    // 设置DMA工作模式为正常模式 
    // 在正常模式下，DMA传输完成一次后就会停止，需要软件重新触发才能再次进行传输 
    ghdma_SPI_tx.Init.Mode = DMA_NORMAL; 
    // 设置DMA传输的优先级为低 
    // 当有多个DMA请求时，低优先级的请求可能会在高优先级请求之后被处理 
    ghdma_SPI_tx.Init.Priority = DMA_PRIORITY_LOW; 
    // 禁用FIFO模式 
    // FIFO（First In First Out）模式可以用于缓存数据，这里不使用该模式，数据将直接传输 
    ghdma_SPI_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; 
    /*初始化DMA*/ 
    HAL_DMA_Init(&ghdma_SPI_tx); 
    /*链接DMA通道到SPI句柄的接收DMA通道（此处原代码可能有误，应是hdmatx更合理）*/ 
    __HAL_LINKDMA(hspi,hdmatx,ghdma_SPI_tx); 

    /* DMA1_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SPI_DMA_TX_IRQ, SPI_DMA_TX_PreemptPriority, SPI_DMA_TX_SubPriority);
    HAL_NVIC_EnableIRQ(SPI_DMA_TX_IRQ);
#endif

#if _HARDWARE_SPI_IT    
    /* SPI中断初始化 */ 
    HAL_NVIC_SetPriority(SPI_IRQ, SPI_PreemptPriority, SPI_SubPriority); 
    HAL_NVIC_EnableIRQ(SPI_IRQ); 

#endif
} 
 
#if _HARDWARE_SPI_DMA

    /**
     * @brief This function handles DMA1 stream0 global interrupt.
     */
    void SPI_DMA_TX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&ghdma_SPI_tx);
    }

 #endif

 #if _HARDWARE_SPI_IT
     /**
     * @brief This function handles SPI interrupt.
     */
    void SPI_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&gSPI_HandleTypeSturct);
    }
    
 #endif

/** 
 * @brief  SPI主从设备进行字节交换操作 
 * @param  sendData: 发送的数据指针 
 * @param  dataLen: 发送的数据长度 
 * @return HARDWARE_SPI_State: SPI操作的状态 
 */ 
HARDWARE_SPI_State oled_spiHardwareSwapByte(uint8_t *sendData, uint16_t dataLen) 
{ 
    // 定义一个HAL状态结构体变量，并初始化为HAL_OK 
    HAL_StatusTypeDef HAL_StatusTypeStruct = HAL_OK; 
 
    // 检查SPI句柄是否有效 
    if (&gSPI_HandleTypeSturct == NULL) 
    { 
        // 若SPI句柄为空，打印错误信息 
        printf("Error: SPI handle is NULL.\r\n"); 
        // 返回SPI错误状态 
        return HARDWARE_SPI_ERROR; 
    } 
 
#if _HARDWARE_SPI_POLLING 
    // 如果使用轮询方式实现SPI通信 
    // 调用HAL库的SPI发送函数进行数据传输 
    HAL_StatusTypeStruct = HAL_SPI_Transmit(&gSPI_HandleTypeSturct, sendData, dataLen, HARDWARE_SPI_SWAP_BYTE_TIMEOUT); 
#endif 

#if _HARDWARE_SPI_DMA
    // 如果使用轮询方式实现SPI通信 
    // 调用HAL库的SPI发送函数进行数据传输 
    HAL_StatusTypeStruct = HAL_SPI_Transmit_DMA(&gSPI_HandleTypeSturct, sendData, dataLen);
    while(HAL_SPI_GetState(&gSPI_HandleTypeSturct) != HAL_SPI_STATE_READY){/* 等待DMA传输完成 */ };
#endif

#if _HARDWARE_SPI_IT
    // 如果使用轮询方式实现SPI通信 
    // 调用HAL库的SPI发送函数进行数据传输 
    HAL_StatusTypeStruct = HAL_SPI_Transmit_IT(&gSPI_HandleTypeSturct, sendData, dataLen);
    while(HAL_SPI_GetState(&gSPI_HandleTypeSturct) != HAL_SPI_STATE_READY){/* 等待DMA传输完成 */ };

#endif
 
    // 如果SPI传输成功 
    if (HAL_StatusTypeStruct == HAL_OK) 
    { 
        // 返回SPI操作完成状态 
        return HARDWARE_SPI_FINISH; 
    } 
    else 
    { 
        // 若SPI传输失败，打印错误信息 
        printf("SPI transmit receive failed with status: %d \r\n", HAL_StatusTypeStruct); 
        // 返回SPI错误状态 
        return HARDWARE_SPI_ERROR; 
    } 
} 
