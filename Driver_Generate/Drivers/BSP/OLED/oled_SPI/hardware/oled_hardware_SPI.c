#include "oled_hardware_SPI.h"
#include "debug_com.h"
SPI_HandleTypeDef gSPI_HandleTypeSturct = {0};

void spiHardwareInit(uint8_t spiMode,SPI_TypeDef *SPIx,uint32_t baudPrescaler)
{
    
    /*SPI基地址*/
    gSPI_HandleTypeSturct.Instance =SPIx;
    /*SPI的主从模式:主模式*/
    gSPI_HandleTypeSturct.Init.Mode = SPI_MODE_MASTER;
    /*SPI的数据方向：SPI双线模式*/
    gSPI_HandleTypeSturct.Init.Direction = SPI_DIRECTION_2LINES;
    /*SPI的数据大小：8bit*/
    gSPI_HandleTypeSturct.Init.DataSize = SPI_DATASIZE_8BIT;
    /*SPI的空闲电平*/
    gSPI_HandleTypeSturct.Init.CLKPolarity = spiHardwareMode[spiMode][0] ;
    /*SPI的采样位置*/
    gSPI_HandleTypeSturct.Init.CLKPhase = spiHardwareMode[spiMode][1];
    /*SPI的NSS信号管理*/
    gSPI_HandleTypeSturct.Init.NSS = SPI_NSS_SOFT;
    /*SPI波特率预分频的值:波特率预分频值*/
    gSPI_HandleTypeSturct.Init.BaudRatePrescaler = baudPrescaler;  
    /*SPI的bit流方向:MSB*/  
    gSPI_HandleTypeSturct.Init.FirstBit = SPI_FIRSTBIT_MSB;
    /*SPI的TI模式:关闭*/  
    gSPI_HandleTypeSturct.Init.TIMode = SPI_TIMODE_DISABLE;
    /*SPI的硬件CRC校验：关闭*/
    gSPI_HandleTypeSturct.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    /*SPI的CRC计算多项式：7*/
    gSPI_HandleTypeSturct.Init.CRCPolynomial = (uint8_t)7;
    /*初始化SPI*/
    HAL_SPI_Init(&gSPI_HandleTypeSturct);
}

/*SPI的基本初始化部分*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    
    if(hspi->Instance == _HARDWARE_SPI)
    {
        GPIO_InitTypeDef GPIO_InitTypeStruct = {0};
        /*GPIO RCC初始化*/
        {
            //CS_CLK
            __hardware_MODEx_SPI_CS_CLK_ENABLE();
            //SCK_CLK
            __hardware_MODEx_SPI_SCK_CLK_ENABLE();
            //MOSI_CLK
            __hardware_MODEx_SPI_MOSI_CLK_ENABLE();
            //MISO_CLK 
            __hardware_MODEx_SPI_MISO_CLK_ENABLE();
        }
        /*SPI RCC初始化*/
        _HARDWARE_HAL_RCC_SPI_CLK_ENABLE();

        /*CS GPIO初始化*/
        GPIO_InitTypeStruct.Pin = hardware_MODEx_SPI_CS_Pin;
        /*软件模拟SPI--->设置为推挽输出*/
        GPIO_InitTypeStruct.Mode =  GPIO_MODE_OUTPUT_PP;
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(hardware_MODEx_SPI_CS_GPIO_Port,&GPIO_InitTypeStruct);

        /*SCK GPIO初始化*/
        GPIO_InitTypeStruct.Pin = hardware_MODEx_SPI_SCK_Pin;
        GPIO_InitTypeStruct.Mode =  GPIO_MODE_AF_PP;
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP;
        GPIO_InitTypeStruct.Alternate = hardware_MODEx_SPI_SCK_GPIO_AF;
        HAL_GPIO_Init(hardware_MODEx_SPI_SCK_GPIO_Port,&GPIO_InitTypeStruct);

        /*MOSI GPIO初始化*/
        GPIO_InitTypeStruct.Pin = hardware_MODEx_SPI_MOSI_Pin;
        GPIO_InitTypeStruct.Mode =  GPIO_MODE_AF_PP;
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP;
        GPIO_InitTypeStruct.Alternate = hardware_MODEx_SPI_MOSI_GPIO_AF;
        HAL_GPIO_Init(hardware_MODEx_SPI_MOSI_GPIO_Port,&GPIO_InitTypeStruct);

        /*MISO GPIO初始化*/
        GPIO_InitTypeStruct.Pin = hardware_MODEx_SPI_MISO_Pin;
        GPIO_InitTypeStruct.Mode =  GPIO_MODE_AF_PP;
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitTypeStruct.Pull = GPIO_PULLUP;
        GPIO_InitTypeStruct.Alternate = hardware_MODEx_SPI_MISO_GPIO_AF;
        HAL_GPIO_Init(hardware_MODEx_SPI_MISO_GPIO_Port,&GPIO_InitTypeStruct);
    }
}

/**
 * @brief  SPI的主从设备进行交换-->交换大小1byte = 8Bit
 * @param   sendData:发送端【发送的】数据
 * @param   receiveData:发送端【接收的】数据
 * 
 */
HARDWARE_SPI_State spiHardwareSwapByte(uint8_t sendData,uint8_t *receiveData)
{
    HAL_StatusTypeDef HAL_StatusTypeStruct = HAL_OK;
    // 检查传入的接收数据指针是否有效 
    if (receiveData == NULL) { 
        uint8_t tempReceiveData;  // 临时变量用于接收数据 
        receiveData = &tempReceiveData; 
    } 
 
    // 检查 SPI 句柄是否有效 
    if (&gSPI_HandleTypeSturct == NULL) { 
        printf("Error: SPI handle is NULL.\r\n"); 
        return HARDWARE_SPI_ERROR; 
    } 
#if _HARDWARE_SPI_POLLING

    //Implementing the function through polling method
    HAL_StatusTypeStruct = HAL_SPI_TransmitReceive(&gSPI_HandleTypeSturct,&sendData,receiveData,1,HARDWARE_SPI_SWAP_BYTE_TIMEOUT);
#endif
    if(HAL_StatusTypeStruct == HAL_OK)
    {
        return HARDWARE_SPI_FINISH;
    }
    else
    {
        printf("SPI transmit receive failed with status: %d \r\n", HAL_StatusTypeStruct); 
        return HARDWARE_SPI_ERROR;
    }
}
