#include "oled_software_SPI.h"


// 定义一个全局变量来存储当前的SPI_MODE 
static uint8_t current_SPI_MODE = 0;

void oled_software_SPI_GPIO_Init(uint8_t SPI_MODE)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    {
        __software_MODEx_SPI_CS_CLK_ENABLE();
        __software_MODEx_SPI_SCK_CLK_ENABLE();
        __software_MODEx_SPI_MOSI_CLK_ENABLE();
        __software_MODEx_SPI_RST_CLK_ENABLE();
        __software_MODEx_SPI_DC_CLK_ENABLE();
    }
    /*brief: mode is OP */
    /*CS initialization*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_CS_GPIO_Port, &GPIO_InitStruct);
    /*SCK initialization*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_SCK_GPIO_Port, &GPIO_InitStruct);
    /*MOSI initialization*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_MOSI_GPIO_Port, &GPIO_InitStruct);
    /*RST initialization*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_RST_GPIO_Port, &GPIO_InitStruct);
    /*DC initialization*/
    GPIO_InitStruct.Pin = software_MODEx_SPI_DC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(software_MODEx_SPI_DC_GPIO_Port, &GPIO_InitStruct);

    /*选择SPI模式*/
    SPI_SET_MODE(SPI_MODE);

    oled_spi_Stop();
}
/*****************************************************SPI的时序控制部分****************************************/
static void oled_software_SPI_delay(void)
{
   for(uint16_t time = 0;time <SPI_DATA_BIT_TIME;time++){;}
}
static void oled_software_SPI_W_CS(uint8_t bitValue)
{
    OLED_SPI_CS(bitValue);
    oled_software_SPI_delay();
}

static void oled_software_SPI_W_SCK(uint8_t bitValue)
{
    OLED_SPI_SCK(bitValue);
    oled_software_SPI_delay();
}

static void oled_software_SPI_W_MOSI(uint8_t bitValue)
{
    OLED_SPI_MOSI(bitValue);
    oled_software_SPI_delay();
}

static void oled_software_SPI_W_RST(uint8_t bitValue)
{
    OLED_SPI_RST(bitValue);
    oled_software_SPI_delay();
}

static void oled_software_SPI_W_DC(uint8_t bitValue)
{
    OLED_SPI_DC(bitValue);
    oled_software_SPI_delay();
}

/*****************************************************SPI的模式部分****************************************/
void SPI_SET_MODE(uint8_t SPI_MODE) 
{ 
    // 更新全局变量
    current_SPI_MODE = SPI_MODE;
    // 统一设置片选信号为高电平 
    oled_software_SPI_W_CS(1); 
 
    // 获取CPOL和CPHA的值 
    uint8_t CPOL = SPI_MODE_MAP[SPI_MODE][0]; 
    uint8_t CPHA = SPI_MODE_MAP[SPI_MODE][1];
    (void)CPHA;  // 显式标记未使用变量  
 
    // 根据CPOL设置SCK初始电平 
    oled_software_SPI_W_SCK(CPOL);
}
/*****************************************************SPI的信号部分****************************************/
void oled_spi_Start(void)
{
    /*CS ->默认拉低片选*/
    oled_software_SPI_W_CS(0);
}

void oled_spi_Stop(void)
{
     /*CS ->默认拉高片选*/
    oled_software_SPI_W_CS(1);
    /*RST引脚->引脚重复复位一次：等待被使用*/
    oled_software_SPI_W_RST(0);
    oled_software_SPI_delay();
    oled_software_SPI_delay();
    oled_software_SPI_W_RST(1);
}

/*OLED只发送不接收*/
void oled_spiSoftwareSwapByte(uint8_t sendData)
{
    // 获取CPOL和CPHA的值 
    uint8_t CPOL = SPI_MODE_MAP[current_SPI_MODE][0]; 
    uint8_t CPHA = SPI_MODE_MAP[current_SPI_MODE][1]; 

    /*模拟硬件SPI的双缓存机制*/ 
    /*byteSend对应硬件SPI的发送缓存，通过MOSI逐位输出数据*/ 
    /*byteReceive对应硬件SPI的接收缓存，通过MISO逐位接收数据*/ 
    SPI_DataBuffer byteSend;
    byteSend.Byte = sendData;

    // 根据CPOL设置SCK初始电平 
    oled_software_SPI_W_SCK(CPOL); 
    for(uint8_t swapBit = 0; swapBit < SPI_SWAP_BIT; swapBit++) 
    { 
        if (CPHA == 0) 
        { 
            // CPHA = 0，在时钟的第一个边沿采样 
            // 发送最高位 
            oled_software_SPI_W_MOSI(byteSend.bit.Bit7);   

            // 产生时钟跳变 
            oled_software_SPI_W_SCK(!CPOL); 
            /*SCLK保持这段时间,主机移除数据 从机移入数据*/
            // 时钟恢复到初始状态 
            oled_software_SPI_W_SCK(CPOL); 
        } 
    } 
}
