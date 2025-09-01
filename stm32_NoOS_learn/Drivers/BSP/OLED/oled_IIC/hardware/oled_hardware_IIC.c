#include "oled_hardware_IIC.h" 
 
// 定义全局的I2C句柄，初始化为零 
I2C_HandleTypeDef gI2C_HandleTypeStruct = {0}; 
// 定义全局的DMA句柄，初始化为零 
DMA_HandleTypeDef ghdma_IIC_tx = {0}; 
 
/** 
 * @brief 初始化硬件I2C 
 * @param I2Cx 要初始化的I2C外设实例 
 * @param clockSpeed I2C时钟速度 
 */ 
void I2C_Hardware_Init(I2C_TypeDef *I2Cx, uint32_t clockSpeed) { 
    // 设置I2C句柄的实例 
    gI2C_HandleTypeStruct.Instance = I2Cx; 
    // 配置SCL的时钟速度 
    gI2C_HandleTypeStruct.Init.ClockSpeed = clockSpeed; 
    // 设置时钟占空比为2:1（低电平时间:高电平时间） 
    gI2C_HandleTypeStruct.Init.DutyCycle = I2C_DUTYCYCLE_2; 
    // 主设备自身地址设为0（主模式通常无需设置地址） 
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
 
    // 初始化I2C 
    HAL_I2C_Init(&gI2C_HandleTypeStruct); 
} 
 
/** 
 * @brief 初始化I2C的底层硬件（引脚、时钟、DMA等） 
 * @param i2cHandle I2C句柄 
 */ 
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle) { 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
 
    if (i2cHandle->Instance == IICx) { 
        // 使能SCL和SDA引脚的时钟 
        __HAL_SCL_GPIO_CLK_ENABLE(); 
        __HAL_SDA_GPIO_CLK_ENABLE(); 
        // 使能I2C外设的时钟 
        __HAL_IICx_CLK_ENABLE(); 
 
        #if _HARDWARE_IIC_DMA 
            // 使能I2C DMA的时钟 
            __IIC_DMA_CLK_ENABLE(); 
        #endif 
 
        // 配置SCL引脚 
        GPIO_InitStruct.Pin = SCL_IIC_Pin; 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; 
        GPIO_InitStruct.Pull = GPIO_PULLUP; 
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        GPIO_InitStruct.Alternate = SCL_IIC_GPIO_AF; 
        HAL_GPIO_Init(SCL_IIC_GPIO_Port, &GPIO_InitStruct); 
 
        // 配置SDA引脚 
        GPIO_InitStruct.Pin = SDA_IIC_Pin; 
        GPIO_InitStruct.Alternate = SDA_IIC_GPIO_AF; 
        HAL_GPIO_Init(SDA_IIC_GPIO_Port, &GPIO_InitStruct); 
 
        #if _HARDWARE_IIC_DMA 
            // 初始化I2C TX的DMA 
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
 
            // 初始化DMA 
            HAL_DMA_Init(&ghdma_IIC_tx); 
            // 链接DMA通道到I2C句柄 
            __HAL_LINKDMA(i2cHandle, hdmatx, ghdma_IIC_tx); 
 
            // 配置NVIC中断 
            HAL_NVIC_SetPriority(IIC_DMA_TX_IRQ, IIC_DMA_TX_PreemptPriority, IIC_DMA_TX_SubPriority); 
            HAL_NVIC_EnableIRQ(IIC_DMA_TX_IRQ); 
            HAL_NVIC_SetPriority(IIC_EV_IRQ, IIC_EV_PreemptPriority, IIC_EV_SubPriority); 
            HAL_NVIC_EnableIRQ(IIC_EV_IRQ); 
            HAL_NVIC_SetPriority(IIC_ER_IRQ, IIC_ER_PreemptPriority, IIC_ER_SubPriority); 
            HAL_NVIC_EnableIRQ(IIC_ER_IRQ); 
        #endif 
 
        #if _HARDWARE_IIC_IT 
            // 配置I2C事件和错误中断 
            HAL_NVIC_SetPriority(IIC_EV_IRQ, IIC_EV_PreemptPriority, IIC_EV_SubPriority); 
            HAL_NVIC_EnableIRQ(IIC_EV_IRQ); 
            HAL_NVIC_SetPriority(IIC_ER_IRQ, IIC_ER_PreemptPriority, IIC_ER_SubPriority); 
            HAL_NVIC_EnableIRQ(IIC_ER_IRQ); 
        #endif 
    } 
} 
 
#if _HARDWARE_IIC_DMA 
/** 
 * @brief 处理DMA1 stream6全局中断 
 */ 
void IIC_DMA_TX_IRQHandler(void) { 
    HAL_DMA_IRQHandler(&ghdma_IIC_tx); 
} 
 
/** 
 * @brief 处理I2C1事件中断 
 */ 
void IIC_EV_IRQHandler(void) { 
    HAL_I2C_EV_IRQHandler(&gI2C_HandleTypeStruct); 
} 
 
/** 
 * @brief 处理I2C1错误中断 
 */ 
void IIC_ER_IRQHandler(void) { 
    HAL_I2C_ER_IRQHandler(&gI2C_HandleTypeStruct); 
} 
#endif 
 
#if _HARDWARE_IIC_IT 
/** 
 * @brief 处理I2C1事件中断 
 */ 
void IIC_EV_IRQHandler(void) { 
    HAL_I2C_EV_IRQHandler(&gI2C_HandleTypeStruct); 
} 
 
/** 
 * @brief 处理I2C1错误中断 
 */ 
void IIC_ER_IRQHandler(void) { 
    HAL_I2C_ER_IRQHandler(&gI2C_HandleTypeStruct); 
} 
#endif 
 
/** 
 * @brief 通过硬件I2C交换字节数据 
 * @param devAddress 从设备地址 
 * @param sendData 要发送的数据指针 
 * @param dataLen 要发送的数据长度 
 * @return HAL状态码 
 */ 
HAL_StatusTypeDef oled_IIC_HardwareSwapByte(uint16_t devAddress, uint8_t *sendData, uint16_t dataLen) { 
    // 定义一个HAL状态结构体变量，并初始化为HAL_OK 
    HAL_StatusTypeDef HAL_Status = HAL_OK; 
 
    // 检查I2C句柄是否有效 
    if (&gI2C_HandleTypeStruct == NULL) { 
        // 若I2C句柄为空，打印错误信息 
        printf("Error: I2C handle is NULL.\r\n"); 
        // 返回IIC错误状态 
        return HAL_ERROR; 
    } 
 
    #if _HARDWARE_IIC_POLLING 
        // 如果使用轮询方式实现IIC通信 
        HAL_Status = HAL_I2C_Master_Transmit(&gI2C_HandleTypeStruct, devAddress, sendData, dataLen, 1000); 
        if (HAL_Status == HAL_OK) { 
            // 若IIC发送成功，将操作结果设置为成功 
            return HAL_OK; 
        } else { 
            // 若IIC发送失败，打印错误信息 
            printf("I2C hardware transmit error \r\n"); 
            return HAL_ERROR; 
        } 
    #endif 
 
    #if _HARDWARE_IIC_DMA 
        // 如果使用DMA方式实现IIC通信 
        HAL_Status = HAL_I2C_Master_Transmit_DMA(&gI2C_HandleTypeStruct, devAddress, sendData, dataLen); 
        if (HAL_Status == HAL_OK) { 
            // 等待DMA传输完成 
            while (HAL_I2C_GetState(&gI2C_HandleTypeStruct) != HAL_I2C_STATE_READY) { 
                // 可以添加超时机制，避免死循环 
                // 例如：使用定时器记录时间，超时则跳出循环 
            } 
            return HAL_OK; 
        } else { 
            // 若IIC发送失败，打印错误信息 
            printf("I2C hardware transmit error \r\n"); 
            return HAL_ERROR; 
        } 
    #endif 
 
    #if _HARDWARE_IIC_IT 
        // 如果使用中断方式实现IIC通信 
        HAL_Status = HAL_I2C_Master_Transmit_IT(&gI2C_HandleTypeStruct, devAddress, sendData, dataLen); 
        if (HAL_Status == HAL_OK) { 
            // 等待中断传输完成 
            while (HAL_I2C_GetState(&gI2C_HandleTypeStruct) != HAL_I2C_STATE_READY) { 
                // 可以添加超时机制，避免死循环 
                // 例如：使用定时器记录时间，超时则跳出循环 
            } 
            return HAL_OK; 
        } else { 
            // 若IIC发送失败，打印错误信息 
            printf("I2C hardware transmit error \r\n"); 
            return HAL_ERROR; 
        } 
    #endif 
} 
