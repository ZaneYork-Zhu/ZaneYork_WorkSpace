#ifndef _HARDWARE_SPI_H_
#define _HARDWARE_SPI_H_

#include "main.h"

/**********************************GPIO部分*******************************/
/*主机输出引脚部分*/
/*SPI CS引脚*/
#define hardware_MODEx_SPI_CS_Pin                                  GPIO_PIN_14
#define hardware_MODEx_SPI_CS_GPIO_Port                            GPIOB
#define __hardware_MODEx_SPI_CS_CLK_ENABLE()                       do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*SPI SCK引脚*/
#define hardware_MODEx_SPI_SCK_Pin                                 GPIO_PIN_3
#define hardware_MODEx_SPI_SCK_GPIO_Port                           GPIOB
#define hardware_MODEx_SPI_SCK_GPIO_AF                             GPIO_AF5_SPI1
#define __hardware_MODEx_SPI_SCK_CLK_ENABLE()                      do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*SPI MOSI引脚*/
#define hardware_MODEx_SPI_MOSI_Pin                                GPIO_PIN_5
#define hardware_MODEx_SPI_MOSI_GPIO_Port                          GPIOB
#define hardware_MODEx_SPI_MOSI_GPIO_AF                            GPIO_AF5_SPI1
#define __hardware_MODEx_SPI_MOSI_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*主机输入引脚部分*/
/*SPI MISO引脚*/
#define hardware_MODEx_SPI_MISO_Pin                                GPIO_PIN_4
#define hardware_MODEx_SPI_MISO_GPIO_Port                          GPIOB
#define hardware_MODEx_SPI_MISO_GPIO_AF                            GPIO_AF5_SPI1
#define __hardware_MODEx_SPI_MISO_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)

#define _HARDWARE_SPI_CS(x)        do{x? \
                                        HAL_GPIO_WritePin(hardware_MODEx_SPI_CS_GPIO_Port, hardware_MODEx_SPI_CS_Pin, GPIO_PIN_SET):  \
                                        HAL_GPIO_WritePin(hardware_MODEx_SPI_CS_GPIO_Port, hardware_MODEx_SPI_CS_Pin, GPIO_PIN_RESET);} while(0U)


#define _HARDWARE_SPI                                              SPI1
#define _HARDWARE_HAL_RCC_SPI_CLK_ENABLE()                        do{__HAL_RCC_SPI1_CLK_ENABLE();}while(0U)

#define _HARDWARE_SPI_POLLING   (1)
#define _HARDWARE_SPI_DMA       (1)
#define _HARDWARE_SPI_IT        (1)

// 定义超时时间宏 
#define HARDWARE_SPI_SWAP_BYTE_TIMEOUT 1000

typedef enum
{
  HARDWARE_SPI_FINISH = 1,    //应答成功
  HARDWARE_SPI_ERROR = 0    //应答失败

}HARDWARE_SPI_State;

// 定义模式与CPOL/CPHA的映射关系 
static uint8_t spiHardwareMode[4][2] = { 
    {SPI_POLARITY_LOW, SPI_PHASE_1EDGE}, // Mode 0 
    {SPI_POLARITY_LOW, SPI_PHASE_2EDGE}, // Mode 1 
    {SPI_POLARITY_HIGH, SPI_PHASE_1EDGE}, // Mode 2 
    {SPI_POLARITY_HIGH, SPI_PHASE_2EDGE}  // Mode 3 
}; 


void spiHardwareInit(uint8_t spiMode,SPI_TypeDef *SPIx,uint32_t baudPrescaler);
HARDWARE_SPI_State spiHardwareSwapByte(uint8_t sendData,uint8_t *receiveData);


#endif
