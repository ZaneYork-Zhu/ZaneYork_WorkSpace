#ifndef OLED_SOFTWARE_SPI_H_
#define OLED_SOFTWARE_SPI_H_

#include "main.h"

/** 
 * @brief OLED硬件连接信息 
 * OLED需要7个引脚进行连接，各引脚连接情况如下： 
 * CS  : PB7  - 片选信号 
 * SCK : PC6  - 时钟信号 
 * MOSI: PC7  - 主输出从输入信号 
 * RST : PG15 - 复位信号 
 * DC  : PD6  - 数据/命令选择信号 
 * 此外还需要连接电源和地 [POWER GND] 
 */ 
/**********************************GPIO section*******************************/
/*主机输出引脚部分*/
/*SPI CS引脚*/
#define software_MODEx_SPI_CS_Pin                                  GPIO_PIN_12
#define software_MODEx_SPI_CS_GPIO_Port                            GPIOB
#define __software_MODEx_SPI_CS_CLK_ENABLE()                       do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)

/*SPI SCK--D0引脚*/
#define software_MODEx_SPI_SCK_Pin                                 GPIO_PIN_13
#define software_MODEx_SPI_SCK_GPIO_Port                           GPIOB
#define __software_MODEx_SPI_SCK_CLK_ENABLE()                      do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*SPI MOSI--D1引脚*/
#define software_MODEx_SPI_MOSI_Pin                                GPIO_PIN_15
#define software_MODEx_SPI_MOSI_GPIO_Port                          GPIOB
#define __software_MODEx_SPI_MOSI_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*其他引脚部分*/
/*SPI RET引脚*/
#define software_MODEx_SPI_RET_Pin                                 GPIO_PIN_0
#define software_MODEx_SPI_RET_GPIO_Port                           GPIOG
#define __software_MODEx_SPI_RET_CLK_ENABLE()                      do{__HAL_RCC_GPIOG_CLK_ENABLE();}while(0U)

/*SPI DC引脚*/
#define software_MODEx_SPI_DC_Pin                                  GPIO_PIN_1
#define software_MODEx_SPI_DC_GPIO_Port                            GPIOG
#define __software_MODEx_SPI_DC_CLK_ENABLE()                       do{__HAL_RCC_GPIOG_CLK_ENABLE();}while(0U)

#define OLED_SPI_CS(x)        do{x? \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_CS_GPIO_Port, software_MODEx_SPI_CS_Pin, GPIO_PIN_SET):  \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_CS_GPIO_Port, software_MODEx_SPI_CS_Pin, GPIO_PIN_RESET);} while(0U)
#define OLED_SPI_SCK(x)       do{x? \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_SCK_GPIO_Port, software_MODEx_SPI_SCK_Pin, GPIO_PIN_SET):  \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_SCK_GPIO_Port, software_MODEx_SPI_SCK_Pin, GPIO_PIN_RESET);} while(0U)
#define OLED_SPI_MOSI(x)      do{x? \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_MOSI_GPIO_Port, software_MODEx_SPI_MOSI_Pin, GPIO_PIN_SET):  \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_MOSI_GPIO_Port, software_MODEx_SPI_MOSI_Pin, GPIO_PIN_RESET);} while(0U)
#define OLED_SPI_RET(x)       do{x? \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_RET_GPIO_Port, software_MODEx_SPI_RET_Pin, GPIO_PIN_SET):  \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_RET_GPIO_Port, software_MODEx_SPI_RET_Pin, GPIO_PIN_RESET);} while(0U)
#define OLED_SPI_DC(x)       do{x? \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_DC_GPIO_Port, software_MODEx_SPI_DC_Pin, GPIO_PIN_SET):  \
                                  HAL_GPIO_WritePin(software_MODEx_SPI_DC_GPIO_Port, software_MODEx_SPI_DC_Pin, GPIO_PIN_RESET);} while(0U) 

// SPI数据位传输时间，单位可能是微秒 
#define SPI_DATA_BIT_TIME    (200) 
// 每次SPI传输的位数 
#define SPI_SWAP_BIT         (8) 
 
/** 
 * @brief 定义SPI模式与CPOL/CPHA的映射关系 
 * SPI有4种工作模式，每种模式由时钟极性（CPOL）和时钟相位（CPHA）决定。 
 * 该二维数组存储了4种模式对应的CPOL和CPHA值。 
 */ 
static uint8_t SPI_MODE_MAP[4][2] = { 
    {0, 0}, // Mode 0: CPOL = 0, CPHA = 0 
    {0, 1}, // Mode 1: CPOL = 0, CPHA = 1 
    {1, 0}, // Mode 2: CPOL = 1, CPHA = 0 
    {1, 1}  // Mode 3: CPOL = 1, CPHA = 1 
}; 
 
/** 
 * @brief 软件SPI状态枚举 
 * 定义了软件SPI通信的两种状态：成功和失败。 
 */ 
typedef enum { 
    SOFTWARE_SPI_FINISH = 1,    // 应答成功 
    SOFTWARE_SPI_ERROR = 0      // 应答失败 
} SOFTWARE_SPI_State; 
 
/** 
 * @brief SPI模式枚举 
 * 定义了SPI的4种工作模式。 
 */ 
typedef enum { 
    MODE0 = 0, 
    MODE1, 
    MODE2, 
    MODE3 
} SPI_MODE; 
 
/** 
 * @brief SPI数据缓冲区联合体 
 * 该联合体用于方便地操作SPI传输的字节数据，可以按位或按字节进行访问。 
 */ 
typedef union { 
    struct { 
        uint8_t Bit0:1;  // 最低有效位 
        uint8_t Bit1:1; 
        uint8_t Bit2:1; 
        uint8_t Bit3:1; 
        uint8_t Bit4:1; 
        uint8_t Bit5:1; 
        uint8_t Bit6:1; 
        uint8_t Bit7:1;  // 最高有效位 
    } bit; 
    uint8_t Byte; 
} SPI_DataBuffer; 
 
// 函数声明 
// 软件SPI延时函数 
static void oled_software_SPI_delay(void); 
// 写入CS引脚电平 
static void oled_software_SPI_W_CS(uint8_t bitValue); 
// 写入SCK引脚电平 
static void oled_software_SPI_W_SCK(uint8_t bitValue); 
// 写入MOSI引脚电平 
static void oled_software_SPI_W_MOSI(uint8_t bitValue); 
// 写入RST引脚电平 
static void oled_software_SPI_W_RET(uint8_t bitValue); 
// 初始化RST引脚 
static void RET_init(void); 
// 初始化软件SPI的GPIO引脚 
void oled_software_SPI_GPIO_Init(uint8_t SPI_MODE); 
// 设置SPI工作模式 
void SPI_SET_MODE(uint8_t SPI_MODE); 
// 启动SPI传输 
void oled_spi_Start(void); 
// 停止SPI传输 
void oled_spi_Stop(void); 
// 写入DC引脚电平 
void oled_software_SPI_W_DC(uint8_t bitValue); 
// 软件SPI字节交换函数 
void oled_spiSoftwareSwapByte(uint8_t sendData); 
 
#endif 
