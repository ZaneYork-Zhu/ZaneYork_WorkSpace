// 包含OLED硬件SPI相关的头文件 
#include "_hardware_SPI.h" 
 
// 定义一个全局的SPI句柄结构体变量，并初始化为0 
SPI_HandleTypeDef gFlash_SPI_HandleTypeSturct = {0}; 
// 定义一个全局的DMA句柄结构体变量，并初始化为0
DMA_HandleTypeDef gFlash_hdma_SPI_tx = {0} ;
DMA_HandleTypeDef gFlash_hdma_SPI_rx = {0} ;


/** 
 * @brief  初始化FLASH硬件SPI的GPIO和SPI参数 
 * @param  spiMode: SPI的工作模式 
 * @param  SPIx: SPI外设的基地址 
 * @param  baudPrescaler: SPI波特率预分频的值 
 */ 
void FLASH_hardware_SPI_GPIO_Init(uint8_t spiMode, SPI_TypeDef *SPIx, uint32_t baudPrescaler) 
{ 
    // 设置SPI基地址 
    gFlash_SPI_HandleTypeSturct.Instance = SPIx; 
    // 设置SPI的主从模式为主模式 
    gFlash_SPI_HandleTypeSturct.Init.Mode = SPI_MODE_MASTER; 
    // 设置SPI的数据方向为双线模式 
    gFlash_SPI_HandleTypeSturct.Init.Direction = SPI_DIRECTION_2LINES; 
    // 设置SPI的数据大小为8bit 
    gFlash_SPI_HandleTypeSturct.Init.DataSize = SPI_DATASIZE_8BIT; 
    // 根据spiMode设置SPI的空闲电平 
    gFlash_SPI_HandleTypeSturct.Init.CLKPolarity = spiHardwareMode[spiMode][0]; 
    // 根据spiMode设置SPI的采样位置 
    gFlash_SPI_HandleTypeSturct.Init.CLKPhase = spiHardwareMode[spiMode][1]; 
    // 设置SPI的NSS信号管理为软件管理 
    gFlash_SPI_HandleTypeSturct.Init.NSS = SPI_NSS_SOFT; 
    // 设置SPI波特率预分频的值 
    gFlash_SPI_HandleTypeSturct.Init.BaudRatePrescaler = baudPrescaler; 
    // 设置SPI的bit流方向为MSB（最高位优先） 
    gFlash_SPI_HandleTypeSturct.Init.FirstBit = SPI_FIRSTBIT_MSB; 
    // 关闭SPI的TI模式 
    gFlash_SPI_HandleTypeSturct.Init.TIMode = SPI_TIMODE_DISABLE; 
    // 关闭SPI的硬件CRC校验 
    gFlash_SPI_HandleTypeSturct.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; 
    // 设置SPI的CRC计算多项式为7 
    gFlash_SPI_HandleTypeSturct.Init.CRCPolynomial = (uint8_t)7; 
    // 调用HAL库函数初始化SPI 
    HAL_SPI_Init(&gFlash_SPI_HandleTypeSturct); 
 
    // 复位OLED硬件SPI的片选信号（拉高） 
    FLASH_HARDWARE_SPI_CS(1); 
} 
 
/** 
 * @brief  SPI的底层硬件初始化函数，由HAL库调用 
 * @param  hspi: SPI句柄指针 
 */ 
#if FLASH_HARDWARE_SPI_ENABLE
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) 
{ 
    // 如果使能了FLASH硬件SPI，并且当前初始化的SPI实例是FLASH硬件SPI 
    if (hspi->Instance == FLASH_HARDWARE_SPI) 
    { 
        // 定义一个GPIO初始化结构体变量，并初始化为0 
        GPIO_InitTypeDef GPIO_InitTypeStruct = {0}; 
        // 使能SPI相关GPIO的时钟 
        { 
            // 使能SPI片选（CS）引脚的时钟 
            __FLASH_HARDWARE_MODEx_SPI_CS_CLK_ENABLE(); 
            // 使能SPI时钟（SCK）引脚的时钟 
            __FLASH_HARDWARE_MODEx_SPI_SCK_CLK_ENABLE(); 
            // 使能SPI主输出从输入（MOSI）引脚的时钟 
            __FLASH_HARDWARE_MODEx_SPI_MOSI_CLK_ENABLE(); 
            // 使能SPI主输入从输出（MISO）引脚的时钟 
            __FLASH_HARDWARE_MODEx_SPI_MISO_CLK_ENABLE(); 
        } 
        // 使能SPI外设的时钟 
        __FLASH_HARDWARE_HAL_RCC_SPI_CLK_ENABLE();
    #if _HARDWARE_SPI_DMA 
            __FLASH_SPI_DMA_CLK_ENABLE();
    #endif
    
            // 初始化SPI片选（CS）引脚 
            GPIO_InitTypeStruct.Pin = FLASH_HARDWARE_MODEx_SPI_CS_Pin; 
            // 设置CS引脚的工作模式 
            GPIO_InitTypeStruct.Mode = FLASH_HARDWARE_MODEx_SPI_CS_GPIO_MODE; 
            // 上拉CS引脚，确保在空闲状态下有确定的电平 
            GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
            // 设置CS引脚的速度为非常高，以满足高速SPI通信需求 
            GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
            // 调用HAL库函数初始化CS引脚 
            HAL_GPIO_Init(FLASH_HARDWARE_MODEx_SPI_CS_GPIO_Port, &GPIO_InitTypeStruct); 
    
            // 初始化SPI时钟（SCK）引脚 
            GPIO_InitTypeStruct.Pin = FLASH_HARDWARE_MODEx_SPI_SCK_Pin; 
            // 设置SCK引脚为复用推挽输出模式，用于SPI时钟信号输出 
            GPIO_InitTypeStruct.Mode = FLASH_HARDWARE_MODEx_SPI_SCK_GPIO_MODE; 
            // 设置SCK引脚的速度为高，以适应SPI通信速率 
            GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
            // 上拉SCK引脚，确保信号稳定 
            GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
            // 设置SCK引脚的复用功能，使其与SPI功能对应 
            GPIO_InitTypeStruct.Alternate = FLASH_HARDWARE_MODEx_SPI_SCK_GPIO_AF; 
            // 调用HAL库函数初始化SCK引脚 
            HAL_GPIO_Init(FLASH_HARDWARE_MODEx_SPI_SCK_GPIO_Port, &GPIO_InitTypeStruct); 
    
            // 初始化SPI主输出从输入（MOSI）引脚 
            GPIO_InitTypeStruct.Pin = FLASH_HARDWARE_MODEx_SPI_MOSI_Pin; 
            // 设置MOSI引脚为复用推挽输出模式，用于主设备向从设备发送数据 
            GPIO_InitTypeStruct.Mode = FLASH_HARDWARE_MODEx_SPI_MOSI_GPIO_MODE; 
            // 设置MOSI引脚的速度为高，以满足高速数据传输要求 
            GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
            // 上拉MOSI引脚，保证信号质量 
            GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
            // 设置MOSI引脚的复用功能，使其与SPI功能对应 
            GPIO_InitTypeStruct.Alternate = FLASH_HARDWARE_MODEx_SPI_MOSI_GPIO_AF; 
            // 调用HAL库函数初始化MOSI引脚 
            HAL_GPIO_Init(FLASH_HARDWARE_MODEx_SPI_MOSI_GPIO_Port, &GPIO_InitTypeStruct); 

            // 初始化SPI主输出从输入（MISO）引脚 
            GPIO_InitTypeStruct.Pin = FLASH_HARDWARE_MODEx_SPI_MISO_Pin; 
            // 设置MISO引脚为复用推挽输出模式，用于主设备向从设备发送数据 
            GPIO_InitTypeStruct.Mode = FLASH_HARDWARE_MODEx_SPI_MISO_GPIO_MODE; 
            // 设置MISO引脚的速度为高，以满足高速数据传输要求 
            GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
            // 上拉MISO引脚，保证信号质量 
            GPIO_InitTypeStruct.Pull = GPIO_PULLUP; 
            // 设置MISO引脚的复用功能，使其与SPI功能对应 
            GPIO_InitTypeStruct.Alternate = FLASH_HARDWARE_MODEx_SPI_MISO_GPIO_AF; 
            // 调用HAL库函数初始化MISO引脚 
            HAL_GPIO_Init(FLASH_HARDWARE_MODEx_SPI_MISO_GPIO_Port, &GPIO_InitTypeStruct); 
    
        } 
    #if _HARDWARE_SPI_DMA 
        /* 
        * 以下代码是对 SPI 的发送 DMA（直接内存访问）进行初始化配置 
        * DMA 可以在不经过 CPU 干预的情况下，实现外设与内存之间的数据传输，提高系统效率 
        */ 
        // 使用DMA处理SPI的发送数据传输 
        gFlash_hdma_SPI_tx.Instance = FLASH_SPI_TX_DMA; 
        // 使用DMA处理SPI的发送数据传输的通道 
        gFlash_hdma_SPI_tx.Init.Channel = FLASH_SPI_TX_CHANNEL; 
        // 设置DMA数据传输的方向为从内存到外设 
        // 这意味着数据将从内存中读取，并发送到SPI外设
        gFlash_hdma_SPI_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; 
        // 禁用外设地址自增功能 
        // 因为SPI在传输数据时，其数据寄存器地址通常是固定的，不需要自动增加 
        gFlash_hdma_SPI_tx.Init.PeriphInc = DMA_PINC_DISABLE; 
        // 启用内存地址自增功能 
        // 这样在每次数据传输后，内存地址会自动增加，以便将接收到的数据依次存储到连续的内存空间 
        gFlash_hdma_SPI_tx.Init.MemInc = DMA_MINC_ENABLE; 
        // 设置外设数据对齐方式为字节对齐 
        // 表示从SPI外设读取的数据是以字节为单位进行传输的 
        gFlash_hdma_SPI_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; 
        // 设置内存数据对齐方式为字节对齐 
        // 意味着接收到的数据将以字节为单位存储到内存中 
        gFlash_hdma_SPI_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; 
        // 设置DMA工作模式为正常模式 
        // 在正常模式下，DMA传输完成一次后就会停止，需要软件重新触发才能再次进行传输 
        gFlash_hdma_SPI_tx.Init.Mode = DMA_NORMAL; 
        // 设置DMA传输的优先级为低 
        // 当有多个DMA请求时，低优先级的请求可能会在高优先级请求之后被处理 
        gFlash_hdma_SPI_tx.Init.Priority = DMA_PRIORITY_LOW; 
        // 禁用FIFO模式 
        // FIFO（First In First Out）模式可以用于缓存数据，这里不使用该模式，数据将直接传输 
        gFlash_hdma_SPI_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; 
        /*初始化DMA*/ 
        HAL_DMA_Init(&gFlash_hdma_SPI_tx); 
        /*链接DMA通道到SPI句柄的接收DMA通道（此处原代码可能有误，应是hdmatx更合理）*/ 
        __HAL_LINKDMA(hspi,hdmatx,gFlash_hdma_SPI_tx); 

            /* 
        * 以下代码是对 SPI 的发送 DMA（直接内存访问）进行初始化配置 
        * DMA 可以在不经过 CPU 干预的情况下，实现外设与内存之间的数据传输，提高系统效率 
        */ 
        // 使用DMA处理SPI的发送数据传输 
        gFlash_hdma_SPI_rx.Instance = FLASH_SPI_RX_DMA; 
        // 使用DMA处理SPI的发送数据传输的通道 
        gFlash_hdma_SPI_rx.Init.Channel = FLASH_SPI_RX_CHANNEL; 
        // 设置DMA数据传输的方向为从内存到外设 
        // 这意味着数据将从内存中读取，并发送到SPI外设
        gFlash_hdma_SPI_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; 
        // 禁用外设地址自增功能 
        // 因为SPI在传输数据时，其数据寄存器地址通常是固定的，不需要自动增加 
        gFlash_hdma_SPI_rx.Init.PeriphInc = DMA_PINC_DISABLE; 
        // 启用内存地址自增功能 
        // 这样在每次数据传输后，内存地址会自动增加，以便将接收到的数据依次存储到连续的内存空间 
        gFlash_hdma_SPI_rx.Init.MemInc = DMA_MINC_ENABLE; 
        // 设置外设数据对齐方式为字节对齐 
        // 表示从SPI外设读取的数据是以字节为单位进行传输的 
        gFlash_hdma_SPI_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; 
        // 设置内存数据对齐方式为字节对齐 
        // 意味着接收到的数据将以字节为单位存储到内存中 
        gFlash_hdma_SPI_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; 
        // 设置DMA工作模式为正常模式 
        // 在正常模式下，DMA传输完成一次后就会停止，需要软件重新触发才能再次进行传输 
        gFlash_hdma_SPI_rx.Init.Mode = DMA_NORMAL; 
        // 设置DMA传输的优先级为低 
        // 当有多个DMA请求时，低优先级的请求可能会在高优先级请求之后被处理 
        gFlash_hdma_SPI_rx.Init.Priority = DMA_PRIORITY_LOW; 
        // 禁用FIFO模式 
        // FIFO（First In First Out）模式可以用于缓存数据，这里不使用该模式，数据将直接传输 
        gFlash_hdma_SPI_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; 
        /*初始化DMA*/ 
        HAL_DMA_Init(&gFlash_hdma_SPI_rx); 
        /*链接DMA通道到SPI句柄的接收DMA通道（此处原代码可能有误，应是hdmatx更合理）*/ 
        __HAL_LINKDMA(hspi,hdmarx,gFlash_hdma_SPI_rx); 

        /* DMA1_Stream6_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(FLASH_SPI_DMA_TX_IRQ, FLASH_SPI_DMA_TX_PreemptPriority, FLASH_SPI_DMA_TX_SubPriority);
        HAL_NVIC_EnableIRQ(FLASH_SPI_DMA_TX_IRQ);
        HAL_NVIC_SetPriority(FLASH_SPI_DMA_RX_IRQ, FLASH_SPI_DMA_RX_PreemptPriority, FLASH_SPI_DMA_RX_SubPriority);
        HAL_NVIC_EnableIRQ(FLASH_SPI_DMA_RX_IRQ);
    #endif

    #if _HARDWARE_SPI_IT    
        /* SPI中断初始化 */ 
        HAL_NVIC_SetPriority(FLASH_SPI_IRQ, FLASH_SPI_IT_PreemptPriority, FLASH_SPI_IT_SubPriority); 
        HAL_NVIC_EnableIRQ(FLASH_SPI_IRQ); 

    #endif
    } 
    
#if _HARDWARE_SPI_DMA

    /**
     * @brief This function handles DMA1 stream0 global interrupt.
     */
    void FLASH_SPI_DMA_TX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&gFlash_hdma_SPI_tx);
    }

    void FLASH_SPI_DMA_RX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&gFlash_hdma_SPI_rx);
    }

 #endif

 #if _HARDWARE_SPI_IT
     /**
     * @brief This function handles SPI interrupt.
     */
    void FLASH_SPI_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&gFlash_SPI_HandleTypeSturct);
    }
    
 #endif

#endif

/** 
 * @brief  实现SPI主从设备之间的字节交换操作。 
 *         该函数可以根据不同的配置（轮询、DMA、中断）完成数据的发送和接收。 
 * 
 * @param  sendData             : 待发送的字节数组指针。 
 * @param  receiveData          : 用于存储接收到的数据的指针。  
 * @param  sizelen              : 要交换的字节总数。 
 * @return HARDWARE_SPI_State   : SPI操作的状态，可能是HARDWARE_SPI_FINISH（操作完成） 
 *                                  或HARDWARE_SPI_ERROR（操作出错）。 
 */ 
HARDWARE_SPI_State flash_spiHardwareSwapByte(const uint8_t *sendData, uint8_t *receiveData, uint64_t sizelen) 
{ 
    // 定义一个HAL状态结构体变量，并初始化为HAL_OK 
    HAL_StatusTypeDef halStatus = HAL_OK; 
 
    // 检查SPI句柄是否有效 
    if (gFlash_SPI_HandleTypeSturct.Instance == NULL) 
    { 
        // 打印错误信息 
        printf("Error: SPI handle is invalid.\r\n"); 
        return HARDWARE_SPI_ERROR; 
    } 
 
    // 剩余待交换的字节数 
    uint64_t remainingBytes = sizelen; 
    // 当前处理的字节偏移量 
    uint64_t offset = 0; 
 
    // 循环处理所有待交换的字节 
    while (remainingBytes > 0) 
    { 
        // 计算当前一次传输的字节数，避免超过UINT16_MAX 
        uint16_t currentSize = (remainingBytes > UINT16_MAX) ? UINT16_MAX : (uint16_t)remainingBytes; 
 
        // 根据不同的SPI通信方式进行数据传输 
        #if _HARDWARE_SPI_POLLING 
            // 使用轮询方式实现SPI通信 
            // 调用HAL库的SPI发送接收函数进行数据传输 
            halStatus = HAL_SPI_TransmitReceive(&gFlash_SPI_HandleTypeSturct, 
                                                sendData + offset, 
                                                receiveData + offset, 
                                                currentSize, 
                                                HARDWARE_SPI_SWAP_BYTE_TIMEOUT); 
        #endif 
 
        #if _HARDWARE_SPI_DMA 
            // 使用DMA方式实现SPI通信 
            // 调用HAL库的SPI DMA发送接收函数进行数据传输 
            halStatus = HAL_SPI_TransmitReceive_DMA(&gFlash_SPI_HandleTypeSturct, 
                                                    sendData + offset, 
                                                    receiveData + offset, 
                                                    currentSize); 
            // 等待DMA传输完成 
            while (HAL_SPI_GetState(&gFlash_SPI_HandleTypeSturct) != HAL_SPI_STATE_READY) { 
                // 空循环，等待DMA传输完成 
            } 
        #endif 
 
        #if _HARDWARE_SPI_IT 
            // 使用中断方式实现SPI通信 
            // 调用HAL库的SPI中断发送接收函数进行数据传输 
            halStatus = HAL_SPI_TransmitReceive_IT(&gFlash_SPI_HandleTypeSturct, 
                                                   sendData + offset, 
                                                   receiveData + offset, 
                                                   currentSize); 
            // 等待数据传输完成 
            while (HAL_SPI_GetState(&gFlash_SPI_HandleTypeSturct) != HAL_SPI_STATE_READY) { 
                // 空循环，等待数据传输完成 
            } 
        #endif  
 
        // 检查传输状态 
        if (halStatus != HAL_OK) 
        { 
            // 打印错误信息 
            printf("Error: SPI transmission failed.\r\n"); 
            return HARDWARE_SPI_ERROR; 
        } 
 
        // 更新剩余待交换的字节数和偏移量 
        remainingBytes -= currentSize; 
        offset += currentSize; 
    } 
 
    // 返回SPI操作完成状态 
    return HARDWARE_SPI_FINISH; 
} 
