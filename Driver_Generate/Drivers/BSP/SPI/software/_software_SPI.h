#ifndef _SOFTWARE_SPI_H_
#define _SOFTWARE_SPI_H_

#include "main.h"


/**********************************GPIO部分*******************************/
/*主机输出引脚部分*/
/*SPI CS引脚*/
#define software_MODEx_SPI_CS_Pin                                  GPIO_PIN_14
#define software_MODEx_SPI_CS_GPIO_Port                            GPIOB
#define __software_MODEx_SPI_CS_CLK_ENABLE()                       do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)

/*SPI SCK引脚*/
#define software_MODEx_SPI_SCK_Pin                                 GPIO_PIN_3
#define software_MODEx_SPI_SCK_GPIO_Port                           GPIOB
#define __software_MODEx_SPI_SCK_CLK_ENABLE()                      do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*SPI MOSI引脚*/
#define software_MODEx_SPI_MOSI_Pin                                GPIO_PIN_5
#define software_MODEx_SPI_MOSI_GPIO_Port                          GPIOB
#define __software_MODEx_SPI_MOSI_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*主机输入引脚部分*/
/*SPI MISO引脚*/
#define software_MODEx_SPI_MISO_Pin                                GPIO_PIN_4
#define software_MODEx_SPI_MISO_GPIO_Port                          GPIOB
#define __software_MODEx_SPI_MISO_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)

#define _SPI_CS(x)        do{x? \
                            HAL_GPIO_WritePin(software_MODEx_SPI_CS_GPIO_Port, software_MODEx_SPI_CS_Pin, GPIO_PIN_SET):  \
                            HAL_GPIO_WritePin(software_MODEx_SPI_CS_GPIO_Port, software_MODEx_SPI_CS_Pin, GPIO_PIN_RESET);} while(0U)
#define _SPI_SCK(x)       do{x? \
                            HAL_GPIO_WritePin(software_MODEx_SPI_SCK_GPIO_Port, software_MODEx_SPI_SCK_Pin, GPIO_PIN_SET):  \
                            HAL_GPIO_WritePin(software_MODEx_SPI_SCK_GPIO_Port, software_MODEx_SPI_SCK_Pin, GPIO_PIN_RESET);} while(0U)

#define _SPI_MOSI(x)       do{x? \
                            HAL_GPIO_WritePin(software_MODEx_SPI_MOSI_GPIO_Port, software_MODEx_SPI_MOSI_Pin, GPIO_PIN_SET):  \
                            HAL_GPIO_WritePin(software_MODEx_SPI_MOSI_GPIO_Port, software_MODEx_SPI_MOSI_Pin, GPIO_PIN_RESET);} while(0U)

#define _SPI_MISO()        HAL_GPIO_ReadPin(software_MODEx_SPI_MISO_GPIO_Port, software_MODEx_SPI_MISO_Pin)  

#define SPI_DATA_BIT_TIME    (200)
#define SPI_SWAP_BIT         (8)


// 定义模式与CPOL/CPHA的映射关系 
static uint8_t SPI_MODE_MAP[4][2] = { 
    {0, 0}, // Mode 0 
    {0, 1}, // Mode 1 
    {1, 0}, // Mode 2 
    {1, 1}  // Mode 3 
}; 

typedef enum
{
  SOFTWARE_SPI_FINISH = 1,    //应答成功
  SOFTWARE_SPI_ERROR = 0    //应答失败

}SOFTWARE_SPI_State;

typedef enum{
    MODE0 = 0,
    MODE1,
    MODE2,
    MODE3

}SPI_MODE;



 /*SPI的联合体*/                                                                        
 typedef union {
    struct
    {
      uint8_t Bit0:1;  // LSB
      uint8_t Bit1:1;
      uint8_t Bit2:1;
      uint8_t Bit3:1;
      uint8_t Bit4:1;
      uint8_t Bit5:1;
      uint8_t Bit6:1;
      uint8_t Bit7:1;  // MSB
    }bit;
    uint8_t Byte;
  }SPI_DataBuffer;  


static void _software_SPI_delay(void);
static void _software_SPI_W_CS(uint8_t bitValue);
static void _software_SPI_W_SCK(uint8_t bitValue);
static void _software_SPI_W_MOSI(uint8_t bitValue);
static uint8_t _software_SPI_R_MISO(void);

void _software_SPI_GPIO_Init(uint8_t SPI_MODE);
void SPI_SET_MODE(uint8_t SPI_MODE);
void spi_Start(void);
void spi_Stop(void);
SOFTWARE_SPI_State spiSoftwareSwapByte(uint8_t sendData,uint8_t *receiveData);

#endif
