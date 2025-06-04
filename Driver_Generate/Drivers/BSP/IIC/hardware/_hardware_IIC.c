 #include "_hardware_IIC.h"

I2C_HandleTypeDef gI2C_HandleTypeStruct = {0};
DMA_HandleTypeDef ghdma_IIC_rx = {0} ;
DMA_HandleTypeDef ghdma_IIC_tx = {0} ;

/**
 * @note  这里的四个全局变量一定要volatile
 *        因为是在中断中改变，不加volatile会读缓冲值，不是中断值
 * 
 */
// 全局变量用于标志 中断 读取传输完成 
volatile uint8_t gIT_transfer_read_complete = 0;
// 全局变量用于标志 中断 写入传输完成 
volatile uint8_t gIT_transfer_write_complete = 0;

/*注意DMA情况下 一定切换为单次读取normal 写几页重新开启几次dma*/
// 全局变量用于标志 中断 读取传输完成 
volatile uint8_t gDMA_transfer_read_complete = 0;

// 全局变量用于标志 中断 写入传输完成 
volatile uint8_t gDMA_transfer_write_complete = 0;

void I2C_Hardware_Init(I2C_TypeDef *I2Cx,uint32_t clockSpeed)
{
    

    gI2C_HandleTypeStruct.Instance = I2Cx;
    /*SCL的时钟配置*/
    gI2C_HandleTypeStruct.Init.ClockSpeed = clockSpeed;
    // 设置时钟占空比为2:1（低电平时间:高电平时间）
    gI2C_HandleTypeStruct.Init.DutyCycle = I2C_DUTYCYCLE_2;
    // 主设备自身地址设为0（主模式通常无需设置地址
    gI2C_HandleTypeStruct.Init.OwnAddress1 = 0;
    // 使用7位地址模式
    gI2C_HandleTypeStruct.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    // 禁用双地址模式
    gI2C_HandleTypeStruct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    // 第二从地址设为0（双地址模式禁用时此参数无效）
    gI2C_HandleTypeStruct.Init.OwnAddress2 = 0;
    // 禁用通用广播呼叫模式（避免响应广播地址）
    gI2C_HandleTypeStruct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    // 允许SCL时钟拉伸（从设备需要缓冲时间时可启用）
    gI2C_HandleTypeStruct.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    /*初始化IIC*/
    HAL_I2C_Init(&gI2C_HandleTypeStruct);
}
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(i2cHandle->Instance == IICx)
    {
        /*初始化引脚时钟*/
        __HAL_SCL_GPIO_CLK_ENABLE();
        __HAL_SDA_GPIO_CLK_ENABLE();
        /*初始化IIC时钟*/
        __HAL_IICx_CLK_ENABLE();
    #if IIC_DMA_ENABLE
        /*I2C_DMA时钟*/
        IIC_DMA_CLK_ENABLE();
    #endif
        GPIO_InitStruct.Pin = SCL_IIC_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = SCL_IIC_GPIO_AF;
        HAL_GPIO_Init(SCL_IIC_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = SDA_IIC_Pin;
        GPIO_InitStruct.Alternate = SDA_IIC_GPIO_AF;
        HAL_GPIO_Init(SDA_IIC_GPIO_Port, &GPIO_InitStruct);
    #if IIC_DMA_ENABLE

        /* 
        * 以下代码是对 I2C 的接收 DMA（直接内存访问）进行初始化配置 
        * DMA 可以在不经过 CPU 干预的情况下，实现外设与内存之间的数据传输，提高系统效率 
        */ 
        
        // 使用DMA处理I2C的接收数据传输 
        ghdma_IIC_rx.Instance = IIC_RX_DMA;
        // 使用DMA处理I2C的接收数据传输的通道
        ghdma_IIC_rx.Init.Channel = IIC_RX_CHANNEL;
        // 设置DMA数据传输的方向为从外设到内存 
        ghdma_IIC_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        // 禁用外设地址自增功能 
        // 因为I2C1在接收数据时，其数据寄存器地址通常是固定的，不需要自动增加 
        ghdma_IIC_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        // 启用内存地址自增功能 
        // 这样在每次数据传输后，内存地址会自动增加，以便将接收到的数据依次存储到连续的内存空间
        ghdma_IIC_rx.Init.MemInc = DMA_MINC_ENABLE;
        // 设置外设数据对齐方式为字节对齐 
        // 表示从I2C外设读取的数据是以字节为单位进行传输的 
        ghdma_IIC_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        // 设置内存数据对齐方式为字节对齐 
        // 意味着接收到的数据将以字节为单位存储到内存中 
        ghdma_IIC_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        // 设置DMA工作模式为正常模式 
        // 在正常模式下，DMA传输完成一次后就会停止，需要软件重新触发才能再次进行传输 
        ghdma_IIC_rx.Init.Mode = DMA_NORMAL;
        // 设置DMA传输的优先级为低 
        // 当有多个DMA请求时，低优先级的请求可能会在高优先级请求之后被处理 
        ghdma_IIC_rx.Init.Priority = DMA_PRIORITY_LOW;
        // 禁用FIFO模式 
        // FIFO（First In First Out）模式可以用于缓存数据，这里不使用该模式，数据将直接传输 
        ghdma_IIC_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        /*初始化DMA*/
        HAL_DMA_Init(&ghdma_IIC_rx);
        /*链接DMA通道*/
        __HAL_LINKDMA(i2cHandle,hdmarx,ghdma_IIC_rx);

        /* I2C_TX Init */
        ghdma_IIC_tx.Instance = IIC_TX_DMA;
        ghdma_IIC_tx.Init.Channel = IIC_TX_CHANNEL;
        // 设置DMA数据传输的方向为从内存到外设 
        ghdma_IIC_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        ghdma_IIC_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        ghdma_IIC_tx.Init.MemInc = DMA_MINC_ENABLE;
        ghdma_IIC_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        ghdma_IIC_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        ghdma_IIC_tx.Init.Mode = DMA_NORMAL;
        ghdma_IIC_tx.Init.Priority = DMA_PRIORITY_LOW;
        ghdma_IIC_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        /*初始化DMA*/
        HAL_DMA_Init(&ghdma_IIC_tx);
        /*链接DMA通道*/
        __HAL_LINKDMA(i2cHandle,hdmatx,ghdma_IIC_tx);

        /*NVIC部分*/
        {
            HAL_NVIC_SetPriority(IIC_DMA_RX_IRQ, IIC_DMA_RX_PreemptPriority, IIC_DMA_RX_SubPriority);
            HAL_NVIC_EnableIRQ(IIC_DMA_RX_IRQ);
            /* DMA1_Stream6_IRQn interrupt configuration */
            HAL_NVIC_SetPriority(IIC_DMA_TX_IRQ, IIC_DMA_TX_PreemptPriority, IIC_DMA_TX_SubPriority);
            HAL_NVIC_EnableIRQ(IIC_DMA_TX_IRQ);

            /* I2C1 interrupt Init */
            HAL_NVIC_SetPriority(IIC_EV_IRQ, IIC_EV_PreemptPriority, IIC_EV_SubPriority);
            HAL_NVIC_EnableIRQ(IIC_EV_IRQ);

            HAL_NVIC_SetPriority(IIC_ER_IRQ, IIC_ER_PreemptPriority, IIC_ER_SubPriority);
            HAL_NVIC_EnableIRQ(IIC_ER_IRQ);
        }


    #endif
    #if IIC_IT_ENABLE    
        /* I2C1 interrupt Init */
        HAL_NVIC_SetPriority(IIC_EV_IRQ, IIC_EV_PreemptPriority, IIC_EV_SubPriority);
        HAL_NVIC_EnableIRQ(IIC_EV_IRQ);

        HAL_NVIC_SetPriority(IIC_ER_IRQ, IIC_ER_PreemptPriority, IIC_ER_SubPriority);
        HAL_NVIC_EnableIRQ(IIC_ER_IRQ);
    #endif
    }
}

#if IIC_DMA_ENABLE

    /**
     * @brief This function handles DMA1 stream0 global interrupt.
     */
    void IIC_DMA_RX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&ghdma_IIC_rx);
    }

    /**
     * @brief This function handles DMA1 stream6 global interrupt.
     */
    void IIC_DMA_TX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&ghdma_IIC_tx);
    }

    /**
     * @brief This function handles I2C1 event interrupt.
     */
    void IIC_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&gI2C_HandleTypeStruct);
    }

    /**
     * @brief This function handles I2C1 error interrupt.
     */
    void IIC_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&gI2C_HandleTypeStruct);
    }

    // DMA 接收完成回调函数 
    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) 
    { 
        if (hi2c == &gI2C_HandleTypeStruct) 
        { 
            gDMA_transfer_read_complete = 1; 
        } 
    } 

    // DMA 发送完成回调函数 
    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) 
    { 
        if (hi2c == &gI2C_HandleTypeStruct) 
        { 
            gDMA_transfer_write_complete = 1;
        } 
    } 
 #endif

 #if IIC_IT_ENABLE
     /**
     * @brief This function handles I2C1 event interrupt.
     */
    void IIC_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&gI2C_HandleTypeStruct);
    }

    /**
     * @brief This function handles I2C1 error interrupt.
     */
    void IIC_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&gI2C_HandleTypeStruct);
    }
    // IT 接收完成回调函数 
    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) 
    { 
        if (hi2c == &gI2C_HandleTypeStruct) 
        { 
            gIT_transfer_read_complete = 1; 
        } 
    } 

    // IT 发送完成回调函数 
    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) 
    { 
        if (hi2c == &gI2C_HandleTypeStruct) 
        { 
            gIT_transfer_write_complete = 1;
        } 
    } 
 
 #endif

