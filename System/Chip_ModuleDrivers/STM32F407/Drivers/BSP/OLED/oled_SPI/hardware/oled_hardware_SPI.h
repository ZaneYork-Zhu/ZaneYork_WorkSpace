#ifndef _HARDWARE_SPI_H_
#define _HARDWARE_SPI_H_

#include "main.h"

#define OLED_HARDWARE_SPI_ENABLE    (1)

#if OLED_HARDWARE_SPI_ENABLE
        /** 
         * @note :使用的是SPI2
         * @brief OLED硬件连接信息 
         * OLED需要7个引脚进行连接，各引脚连接情况如下： 
         * CS  : PB7  - 片选信号 
         * SCK : PC6  - 时钟信号 
         * MOSI: PC7  - 主输出从输入信号 
         * RST : PG0  - 复位信号 
         * DC  : PG1  - 数据/命令选择信号 
         * 此外还需要连接电源和地 [POWER GND] 
         */ 
        /**********************************GPIO section*******************************/
        /*主机输出引脚部分*/
        /*SPI CS引脚*/
        #define OLED_HARDWARE_MODEx_SPI_CS_Pin                                  GPIO_PIN_12
        #define OLED_HARDWARE_MODEx_SPI_CS_GPIO_Port                            GPIOB
        #define OLED_HARDWARE_MODEx_SPI_CS_GPIO_MODE                            GPIO_MODE_OUTPUT_PP
        #define __OLED_HARDWARE_MODEx_SPI_CS_CLK_ENABLE()                       do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
        
        /*SPI SCK--D0引脚*/
        #define OLED_HARDWARE_MODEx_SPI_SCK_Pin                                 GPIO_PIN_13
        #define OLED_HARDWARE_MODEx_SPI_SCK_GPIO_Port                           GPIOB
        #define OLED_HARDWARE_MODEx_SPI_SCK_GPIO_AF                             GPIO_AF5_SPI2
        #define __OLED_HARDWARE_MODEx_SPI_SCK_CLK_ENABLE()                      do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
        /*SPI MOSI--D1引脚*/
        #define OLED_HARDWARE_MODEx_SPI_MOSI_Pin                                GPIO_PIN_15
        #define OLED_HARDWARE_MODEx_SPI_MOSI_GPIO_Port                          GPIOB
        #define OLED_HARDWARE_MODEx_SPI_MOSI_GPIO_AF                            GPIO_AF5_SPI2
        #define __OLED_HARDWARE_MODEx_SPI_MOSI_CLK_ENABLE()                     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
        /*其他引脚部分*/
        /*SPI RET引脚*/
        #define OLED_HARDWARE_MODEx_SPI_RET_Pin                                 GPIO_PIN_0
        #define OLED_HARDWARE_MODEx_SPI_RET_GPIO_Port                           GPIOG
        #define OLED_HARDWARE_MODEx_SPI_RET_GPIO_MODE                           GPIO_MODE_OUTPUT_PP
        #define __OLED_HARDWARE_MODEx_SPI_RET_CLK_ENABLE()                      do{__HAL_RCC_GPIOG_CLK_ENABLE();}while(0U)

        /*SPI DC引脚*/
        #define OLED_HARDWARE_MODEx_SPI_DC_Pin                                  GPIO_PIN_1
        #define OLED_HARDWARE_MODEx_SPI_DC_GPIO_Port                            GPIOG
        #define OLED_HARDWARE_MODEx_SPI_DC_GPIO_MODE                            GPIO_MODE_OUTPUT_PP
        #define __OLED_HARDWARE_MODEx_SPI_DC_CLK_ENABLE()                       do{__HAL_RCC_GPIOG_CLK_ENABLE();}while(0U)
        
        /*SPI 片选信号处理*/
        #define OLED_HARDWARE_SPI_CS(x)          do{x? \
                                                        HAL_GPIO_WritePin(OLED_HARDWARE_MODEx_SPI_CS_GPIO_Port, OLED_HARDWARE_MODEx_SPI_CS_Pin, GPIO_PIN_SET):  \
                                                        HAL_GPIO_WritePin(OLED_HARDWARE_MODEx_SPI_CS_GPIO_Port, OLED_HARDWARE_MODEx_SPI_CS_Pin, GPIO_PIN_RESET);} while(0U)
        #define OLED_HARDWARE_SPI_RET(x)         do{x? \
                                                        HAL_GPIO_WritePin(OLED_HARDWARE_MODEx_SPI_RET_GPIO_Port, OLED_HARDWARE_MODEx_SPI_RET_Pin, GPIO_PIN_SET):  \
                                                        HAL_GPIO_WritePin(OLED_HARDWARE_MODEx_SPI_RET_GPIO_Port, OLED_HARDWARE_MODEx_SPI_RET_Pin, GPIO_PIN_RESET);} while(0U)                
        #define OLED_HARDWARE_SPI_DC(x)          do{x? \
                                                        HAL_GPIO_WritePin(OLED_HARDWARE_MODEx_SPI_DC_GPIO_Port, OLED_HARDWARE_MODEx_SPI_DC_Pin, GPIO_PIN_SET):  \
                                                        HAL_GPIO_WritePin(OLED_HARDWARE_MODEx_SPI_DC_GPIO_Port, OLED_HARDWARE_MODEx_SPI_DC_Pin, GPIO_PIN_RESET);} while(0U) 
                                         

        #define OLED_HARDWARE_SPI                                                SPI2
        #define __OLED_HARDWARE_HAL_RCC_SPI_CLK_ENABLE()                       do{__HAL_RCC_SPI2_CLK_ENABLE();}while(0U)

        // /*SPI+DMA*/
        #define SPI_TX_DMA                      DMA1_Stream4
        #define SPI_TX_CHANNEL                  DMA_CHANNEL_0          
        /*SPI+IT*/
        #define SPI_IRQ                         SPI2_IRQn
        #define SPI_IRQHandler                  SPI2_IRQHandler

        #define SPI_DMA_CLK_ENABLE()            do{__HAL_RCC_DMA1_CLK_ENABLE();}while(0U)
        #define SPI_DMA_TX_IRQ                  DMA1_Stream4_IRQn
        #define SPI_DMA_TX_IRQHandler           DMA1_Stream4_IRQHandler


        #define SPI_DMA_TX_PreemptPriority      3
        #define SPI_DMA_TX_SubPriority          0

        #define SPI_PreemptPriority             2
        #define SPI_SubPriority                 1
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


void oled_hardware_SPI_GPIO_Init(uint8_t spiMode,SPI_TypeDef *SPIx,uint32_t baudPrescaler);
HARDWARE_SPI_State oled_spiHardwareSwapByte(uint8_t *sendData,uint16_t dataLen);


#endif
