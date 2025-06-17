#include "oled_software_SPI.h" 
 
// 定义一个全局变量来存储当前的SPI_MODE 
static uint8_t current_SPI_MODE = 0; 
 
/** 
 * @brief 初始化OLED软件SPI的GPIO引脚 
 * @param SPI_MODE 要设置的SPI模式 
 * @details 该函数用于初始化OLED软件SPI所需的GPIO引脚，包括CS、SCK、MOSI、RST和DC引脚。 
 *          同时，设置SPI模式并停止SPI通信，进行复位引脚的初始化。 
 */ 
void oled_software_SPI_GPIO_Init(uint8_t SPI_MODE) 
{ 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
    { 
        // 使能各个GPIO引脚的时钟 
        __software_MODEx_SPI_CS_CLK_ENABLE(); 
        __software_MODEx_SPI_SCK_CLK_ENABLE(); 
        __software_MODEx_SPI_MOSI_CLK_ENABLE(); 
        __software_MODEx_SPI_RET_CLK_ENABLE(); 
        __software_MODEx_SPI_DC_CLK_ENABLE(); 
    } 
    /*brief: mode is OP */ 
    /*CS initialization*/ 
    // 配置CS引脚 
    GPIO_InitStruct.Pin = software_MODEx_SPI_CS_Pin; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出模式 
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 上拉电阻 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 高速 
    HAL_GPIO_Init(software_MODEx_SPI_CS_GPIO_Port, &GPIO_InitStruct); 
 
    /*SCK initialization*/ 
    // 配置SCK引脚 
    GPIO_InitStruct.Pin = software_MODEx_SPI_SCK_Pin; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    HAL_GPIO_Init(software_MODEx_SPI_SCK_GPIO_Port, &GPIO_InitStruct); 
 
    /*MOSI initialization*/ 
    // 配置MOSI引脚 
    GPIO_InitStruct.Pin = software_MODEx_SPI_MOSI_Pin; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    HAL_GPIO_Init(software_MODEx_SPI_MOSI_GPIO_Port, &GPIO_InitStruct); 
 
    /*RST initialization*/ 
    // 配置RST引脚 
    GPIO_InitStruct.Pin = software_MODEx_SPI_RET_Pin; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    HAL_GPIO_Init(software_MODEx_SPI_RET_GPIO_Port, &GPIO_InitStruct); 
 
    /*DC initialization*/ 
    // 配置DC引脚 
    GPIO_InitStruct.Pin = software_MODEx_SPI_DC_Pin; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    HAL_GPIO_Init(software_MODEx_SPI_DC_GPIO_Port, &GPIO_InitStruct); 
 
    // 设置SPI模式 
    SPI_SET_MODE(SPI_MODE); 
    // 停止SPI通信 
    oled_spi_Stop(); 
    // 初始化复位引脚 
    RET_init(); 
} 
 
/*****************************************************SPI的时序控制部分****************************************/ 
/** 
 * @brief 软件SPI延时函数 
 * @details 该函数通过一个简单的循环实现延时，用于控制SPI信号的时序。 
 */ 
static void oled_software_SPI_delay(void) 
{ 
    for(uint16_t time = 0; time < SPI_DATA_BIT_TIME; time++) {;} 
} 
 
/** 
 * @brief 向CS引脚写入数据并延时 
 * @param bitValue 要写入CS引脚的值 
 * @details 该函数将指定的值写入CS引脚，并调用延时函数进行时序控制。 
 */ 
static void oled_software_SPI_W_CS(uint8_t bitValue) 
{ 
    OLED_SPI_CS(bitValue); 
    oled_software_SPI_delay(); 
} 
 
/** 
 * @brief 向SCK引脚写入数据并延时 
 * @param bitValue 要写入SCK引脚的值 
 * @details 该函数将指定的值写入SCK引脚，并调用延时函数进行时序控制。 
 */ 
static void oled_software_SPI_W_SCK(uint8_t bitValue) 
{ 
    OLED_SPI_SCK(bitValue); 
    oled_software_SPI_delay(); 
} 
 
/** 
 * @brief 向MOSI引脚写入数据并延时 
 * @param bitValue 要写入MOSI引脚的值 
 * @details 该函数将指定的值写入MOSI引脚，并调用延时函数进行时序控制。 
 */ 
static void oled_software_SPI_W_MOSI(uint8_t bitValue) 
{ 
    OLED_SPI_MOSI(bitValue); 
    oled_software_SPI_delay(); 
} 
 
/** 
 * @brief 向RST引脚写入数据并延时 
 * @param bitValue 要写入RST引脚的值 
 * @details 该函数将指定的值写入RST引脚，并调用延时函数进行时序控制。 
 */ 
static void oled_software_SPI_W_RET(uint8_t bitValue) 
{ 
    OLED_SPI_RET(bitValue); 
    oled_software_SPI_delay(); 
} 
 
/** 
 * @brief 向DC引脚写入数据并延时 
 * @param bitValue 要写入DC引脚的值 
 * @details 该函数将指定的值写入DC引脚，并调用延时函数进行时序控制。 
 */ 
void oled_software_SPI_W_DC(uint8_t bitValue) 
{ 
    OLED_SPI_DC(bitValue); 
    oled_software_SPI_delay(); 
} 
 
/*****************************************************SPI的模式部分****************************************/ 
/** 
 * @brief 设置SPI模式 
 * @param SPI_MODE 要设置的SPI模式 
 * @details 该函数更新全局变量current_SPI_MODE，并根据SPI模式设置片选信号和SCK初始电平。 
 */ 
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
/** 
 * @brief 启动SPI通信 
 * @details 该函数将片选信号拉低，启动SPI通信。 
 */ 
void oled_spi_Start(void) 
{ 
    /*CS ->默认拉低片选*/ 
    oled_software_SPI_W_CS(0); 
} 
 
/** 
 * @brief 停止SPI通信 
 * @details 该函数将片选信号拉高，并将DC信号拉高，停止SPI通信。 
 */ 
void oled_spi_Stop(void) 
{ 
    /*CS ->默认拉高片选*/ 
    oled_software_SPI_W_CS(1); 
    oled_software_SPI_W_DC(1); 
} 
 
/** 
 * @brief 初始化复位引脚 
 * @details 该函数对OLED的复位引脚进行初始化，通过拉低再拉高复位引脚实现硬件复位。 
 */ 
static void RET_init(void) 
{ 
    /*RET引脚 OLED硬件自复位*/ 
    oled_software_SPI_W_RET(0); 
    oled_software_SPI_delay(); 
    oled_software_SPI_delay(); 
    oled_software_SPI_delay(); 
    oled_software_SPI_W_RET(1); 
} 
 
/** 
 * @brief 软件SPI发送一个字节数据 
 * @param sendData 要发送的字节数据 
 * @details 该函数模拟硬件SPI的双缓存机制，根据SPI模式（CPOL和CPHA）逐位发送数据。 
 */ 
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
        // 发送数据左移一位 
        byteSend.Byte <<= 1; 
    } 
} 
