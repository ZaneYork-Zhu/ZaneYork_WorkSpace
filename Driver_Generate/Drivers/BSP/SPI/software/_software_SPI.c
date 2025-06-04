#include "_software_SPI.h"


// 定义一个全局变量来存储当前的SPI_MODE 
static uint8_t current_SPI_MODE = 0;

void _software_SPI_GPIO_Init(uint8_t SPI_MODE)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __software_MODEx_SPI_CS_CLK_ENABLE();
    __software_MODEx_SPI_SCK_CLK_ENABLE();
    __software_MODEx_SPI_MISO_CLK_ENABLE();
    __software_MODEx_SPI_MOSI_CLK_ENABLE();

    /*Configure GPIO pins : software_MODE0_SPI1_CS_Pin software_MODE0_SPI1_SCK_Pin software_MODE0_SPI1_MISO_Pin software_MODE0_SPI1_MOSI_Pin */
    /*软件模拟SPI--->设置为推挽输出*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_CS_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = software_MODEx_SPI_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_SCK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = software_MODEx_SPI_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_MOSI_GPIO_Port, &GPIO_InitStruct);

    /*软件模拟SPI--->MISO设置为输入*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_MISO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_MISO_GPIO_Port, &GPIO_InitStruct);

    /*选择SPI模式*/
    SPI_SET_MODE(SPI_MODE);
    /*清空缓存区*/
    spiSoftwareSwapByte(0xFF,NULL);
}
/*****************************************************SPI的时序控制部分****************************************/
static void _software_SPI_delay(void)
{
   for(uint16_t time = 0;time <SPI_DATA_BIT_TIME;time++){;}
}


static void _software_SPI_W_CS(uint8_t bitValue)
{
    _SPI_CS(bitValue);
    _software_SPI_delay();
}

static void _software_SPI_W_SCK(uint8_t bitValue)
{
    _SPI_SCK(bitValue);
    _software_SPI_delay();
}

static void _software_SPI_W_MOSI(uint8_t bitValue)
{
    _SPI_MOSI(bitValue);
    _software_SPI_delay();
}

static uint8_t _software_SPI_R_MISO(void)
{
    uint8_t bitValue = _SPI_MISO();
    _software_SPI_delay();
    return bitValue;
}
/*****************************************************SPI的模式部分****************************************/
void SPI_SET_MODE(uint8_t SPI_MODE) 
{ 
    // 更新全局变量
    current_SPI_MODE = SPI_MODE;
    // 统一设置片选信号为高电平 
    _software_SPI_W_CS(1); 
 
    // 获取CPOL和CPHA的值 
    uint8_t CPOL = SPI_MODE_MAP[SPI_MODE][0]; 
    uint8_t CPHA = SPI_MODE_MAP[SPI_MODE][1];
    (void)CPHA;  // 显式标记未使用变量  
 
    // 根据CPOL设置SCK初始电平 
    _software_SPI_W_SCK(CPOL);
}
/*****************************************************SPI的信号部分****************************************/
void spi_Start(void)
{
    _software_SPI_W_CS(0);
}

void spi_Stop(void)
{
    _software_SPI_W_CS(1);
}

SOFTWARE_SPI_State spiSoftwareSwapByte(uint8_t sendData,uint8_t *receiveData)
{
    // 获取CPOL和CPHA的值 
    uint8_t CPOL = SPI_MODE_MAP[current_SPI_MODE][0]; 
    uint8_t CPHA = SPI_MODE_MAP[current_SPI_MODE][1]; 

    /*模拟硬件SPI的双缓存机制*/ 
    /*byteSend对应硬件SPI的发送缓存，通过MOSI逐位输出数据*/ 
    /*byteReceive对应硬件SPI的接收缓存，通过MISO逐位接收数据*/ 
    SPI_DataBuffer byteReceive, byteSend; 
    byteSend.Byte = sendData; 

    // 根据CPOL设置SCK初始电平 
    _software_SPI_W_SCK(CPOL); 
    for(uint8_t swapBit = 0; swapBit < SPI_SWAP_BIT; swapBit++) 
    { 
        if (CPHA == 0) 
        { 
            // CPHA = 0，在时钟的第一个边沿采样 
            // 发送最高位 
            _software_SPI_W_MOSI(byteSend.bit.Bit7);   

            // 产生时钟跳变 
            _software_SPI_W_SCK(!CPOL); 

            // 先左移最低位，接收数据到最低位 
            byteReceive.Byte <<= 1; 
            // 读取从机返回的位 
            byteReceive.bit.Bit0 = _software_SPI_R_MISO(); 

            // 时钟恢复到初始状态 
            _software_SPI_W_SCK(CPOL); 
        } 
        else 
        { 
            /*移动半个周期的相位*/
            // CPHA = 1，在时钟的第二个边沿采样 
            // 时钟跳变到相反状态 
            _software_SPI_W_SCK(!CPOL); 

            // 发送最高位 
            _software_SPI_W_MOSI(byteSend.bit.Bit7);   

            // 时钟恢复到初始状态 
            _software_SPI_W_SCK(CPOL); 

            // 先左移最低位，接收数据到最低位 
            byteReceive.Byte <<= 1; 
            // 读取从机返回的位 
            byteReceive.bit.Bit0 = _software_SPI_R_MISO(); 
        }
        //仅在接收指针有效时存储数据
        if (receiveData != NULL) 
        { 
            // 将接收到的数据复制到传入的指针所指向的地址 
            *receiveData = byteReceive.Byte; 
        }
        // 发送数据左移一位 
        byteSend.Byte <<= 1; 
    } 
    return SOFTWARE_SPI_FINISH; 
}
