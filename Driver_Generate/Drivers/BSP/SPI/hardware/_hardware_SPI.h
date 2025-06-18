#ifndef _FLASH_HARDWARE_SPI_H_
#define _FLASH_HARDWARE_SPI_H_

#include "main.h"

#define FLASH_HARDWARE_SPI_ENABLE    (1)

#if FLASH_HARDWARE_SPI_ENABLE
        /** 
         * @brief FLASH硬件连接信息 
         * FLASH需要4个引脚进行连接，各引脚连接情况如下： 
         * CS  : PB14 - 片选信号 
         * SCK : PP3  - 时钟信号 
         * MOSI: PB5  - 主输出从输入信号 
         * MISO: PB4  - 主输入从输出信号 
         * 此外还需要连接电源和地 [POWER GND] 
         */ 
        /**********************************GPIO section*******************************/
        /*主机输出引脚部分*/
        /*SPI CS引脚*/
        #define FLASH_HARDWARE_MODEx_SPI_CS_Pin                                  GPIO_PIN_14
        #define FLASH_HARDWARE_MODEx_SPI_CS_GPIO_Port                            GPIOB
        #define FLASH_HARDWARE_MODEx_SPI_CS_GPIO_MODE                            GPIO_MODE_OUTPUT_PP
        #define __FLASH_HARDWARE_MODEx_SPI_CS_CLK_ENABLE()                       do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
        
        /*SPI SCK*/
        #define FLASH_HARDWARE_MODEx_SPI_SCK_Pin                                 GPIO_PIN_3
        #define FLASH_HARDWARE_MODEx_SPI_SCK_GPIO_Port                           GPIOB
        #define FLASH_HARDWARE_MODEx_SPI_SCK_GPIO_AF                             GPIO_AF5_SPI1
        #define FLASH_HARDWARE_MODEx_SPI_SCK_GPIO_MODE                           GPIO_MODE_AF_PP
        #define __FLASH_HARDWARE_MODEx_SPI_SCK_CLK_ENABLE()                      do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
        /*SPI MOSI*/
        #define FLASH_HARDWARE_MODEx_SPI_MOSI_Pin                                GPIO_PIN_5
        #define FLASH_HARDWARE_MODEx_SPI_MOSI_GPIO_Port                          GPIOB
        #define FLASH_HARDWARE_MODEx_SPI_MOSI_GPIO_AF                            GPIO_AF5_SPI1
        #define FLASH_HARDWARE_MODEx_SPI_MOSI_GPIO_MODE                           GPIO_MODE_AF_PP
        #define __FLASH_HARDWARE_MODEx_SPI_MOSI_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
        /*其他引脚部分*/
        /*SPI MISO*/
        #define FLASH_HARDWARE_MODEx_SPI_MISO_Pin                                GPIO_PIN_4
        #define FLASH_HARDWARE_MODEx_SPI_MISO_GPIO_Port                          GPIOB
        #define FLASH_HARDWARE_MODEx_SPI_MISO_GPIO_AF                            GPIO_AF5_SPI1
        #define FLASH_HARDWARE_MODEx_SPI_MISO_GPIO_MODE                          GPIO_MODE_AF_PP
        #define __FLASH_HARDWARE_MODEx_SPI_MISO_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)

        
        /*SPI 片选信号处理*/
        #define FLASH_HARDWARE_SPI_CS(x)        do{x? \
                                                        HAL_GPIO_WritePin(FLASH_HARDWARE_MODEx_SPI_CS_GPIO_Port, FLASH_HARDWARE_MODEx_SPI_CS_Pin, GPIO_PIN_SET):  \
                                                        HAL_GPIO_WritePin(FLASH_HARDWARE_MODEx_SPI_CS_GPIO_Port, FLASH_HARDWARE_MODEx_SPI_CS_Pin, GPIO_PIN_RESET);} while(0U)
                                         

        #define FLASH_HARDWARE_SPI                                                SPI1
        #define __FLASH_HARDWARE_HAL_RCC_SPI_CLK_ENABLE()                         do{__HAL_RCC_SPI1_CLK_ENABLE();}while(0U)
        
        // /*SPI+DMA*/
        #define FLASH_SPI_TX_DMA                              DMA2_Stream3
        #define FLASH_SPI_TX_CHANNEL                          DMA_CHANNEL_3       
        #define FLASH_SPI_RX_DMA                              DMA2_Stream0
        #define FLASH_SPI_RX_CHANNEL                          DMA_CHANNEL_3           
        /*SPI+IT*/
        #define FLASH_SPI_IRQ                                 SPI1_IRQn
        #define FLASH_SPI_IRQHandler                          SPI1_IRQHandler

        #define __FLASH_SPI_DMA_CLK_ENABLE()                  do{__HAL_RCC_DMA2_CLK_ENABLE();}while(0U)
        #define FLASH_SPI_DMA_TX_IRQ                          DMA2_Stream3_IRQn
        #define FLASH_SPI_DMA_TX_IRQHandler                   DMA2_Stream3_IRQHandler
        #define FLASH_SPI_DMA_RX_IRQ                          DMA2_Stream0_IRQn
        #define FLASH_SPI_DMA_RX_IRQHandler                   DMA2_Stream0_IRQHandler

        #define FLASH_SPI_DMA_TX_PreemptPriority      3
        #define FLASH_SPI_DMA_TX_SubPriority          1
        #define FLASH_SPI_DMA_RX_PreemptPriority      3
        #define FLASH_SPI_DMA_RX_SubPriority          0


        #define FLASH_SPI_IT_PreemptPriority          2
        #define FLASH_SPI_IT_SubPriority              1
#endif



#define _HARDWARE_SPI_POLLING   (0)
#define _HARDWARE_SPI_DMA       (0)
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


void FLASH_hardware_SPI_GPIO_Init(uint8_t spiMode, SPI_TypeDef *SPIx, uint32_t baudPrescaler);

HARDWARE_SPI_State flash_spiHardwareSwapByte(const uint8_t *sendData, uint8_t *receiveData,uint64_t sizelen) ;


#endif
