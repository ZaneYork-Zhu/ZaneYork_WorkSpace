#ifndef _Hardware_IIC_H_
#define _Hardware_IIC_H_

#include "main.h"
#include "usart.h"
/*IIC_SCL引脚初始化*/
#define _flash_I2C_24Cxx 0
#define _oled_I2C_SSD1306 1
#if _flash_24Cxx
    #define SCL_IIC_Pin                     GPIO_PIN_8
    #define SCL_IIC_GPIO_Port               GPIOB
    #define SCL_IIC_GPIO_AF                 GPIO_AF4_I2C1
    #define __HAL_SCL_GPIO_CLK_ENABLE()     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
    /*IIC_SDA引脚初始化*/
    #define SDA_IIC_Pin                     GPIO_PIN_9
    #define SDA_IIC_GPIO_Port               GPIOB
    #define SDA_IIC_GPIO_AF                 GPIO_AF4_I2C1
    #define __HAL_SDA_GPIO_CLK_ENABLE()     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
    /*IIC初始化*/
    #define IICx                            I2C1
    #define __HAL_IICx_CLK_ENABLE()         do{__HAL_RCC_I2C1_CLK_ENABLE();}while(0U) 
    /*IIC+DMA*/
    #define IIC_RX_DMA                      DMA1_Stream0
    #define IIC_RX_CHANNEL                  DMA_CHANNEL_1 
    #define IIC_TX_DMA                      DMA1_Stream6
    #define IIC_TX_CHANNEL                  DMA_CHANNEL_1          
    /*IIC+IT*/
    #define IIC_EV_IRQ                      I2C1_EV_IRQn
    #define IIC_EV_IRQHandler               I2C1_EV_IRQHandler
    #define IIC_ER_IRQ                      I2C1_ER_IRQn
    #define IIC_ER_IRQHandler               I2C1_ER_IRQHandler

    #define IIC_DMA_CLK_ENABLE()            do{__HAL_RCC_DMA1_CLK_ENABLE();}while(0U)
    #define IIC_DMA_RX_IRQ                  DMA1_Stream0_IRQn
    #define IIC_DMA_RX_IRQHandler           DMA1_Stream0_IRQHandler
    #define IIC_DMA_TX_IRQ                  DMA1_Stream6_IRQn
    #define IIC_DMA_TX_IRQHandler           DMA1_Stream6_IRQHandler
#endif

#if _oled_I2C_SSD1306
    #define SCL_IIC_Pin                     GPIO_PIN_10
    #define SCL_IIC_GPIO_Port               GPIOB
    #define SCL_IIC_GPIO_AF                 GPIO_AF4_I2C2
    #define __HAL_SCL_GPIO_CLK_ENABLE()     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
    /*IIC_SDA引脚初始化*/
    #define SDA_IIC_Pin                     GPIO_PIN_11
    #define SDA_IIC_GPIO_Port               GPIOB
    #define SDA_IIC_GPIO_AF                 GPIO_AF4_I2C2
    #define __HAL_SDA_GPIO_CLK_ENABLE()     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
    /*IIC初始化*/
    #define IICx                            I2C2
    #define __HAL_IICx_CLK_ENABLE()         do{__HAL_RCC_I2C2_CLK_ENABLE();}while(0U) 
    // /*IIC+DMA*/
    #define IIC_RX_DMA                      DMA1_Stream2
    #define IIC_RX_CHANNEL                  DMA_CHANNEL_7 
    #define IIC_TX_DMA                      DMA1_Stream7
    #define IIC_TX_CHANNEL                  DMA_CHANNEL_7          
    /*IIC+IT*/
    #define IIC_EV_IRQ                      I2C2_EV_IRQn
    #define IIC_EV_IRQHandler               I2C2_EV_IRQHandler
    #define IIC_ER_IRQ                      I2C2_ER_IRQn
    #define IIC_ER_IRQHandler               I2C2_ER_IRQHandler

    #define IIC_DMA_CLK_ENABLE()            do{__HAL_RCC_DMA1_CLK_ENABLE();}while(0U)
    #define IIC_DMA_RX_IRQ                  DMA1_Stream2_IRQn
    #define IIC_DMA_RX_IRQHandler           DMA1_Stream2_IRQHandler
    #define IIC_DMA_TX_IRQ                  DMA1_Stream7_IRQn
    #define IIC_DMA_TX_IRQHandler           DMA1_Stream7_IRQHandler
#endif

#define IIC_DMA_RX_PreemptPriority      3
#define IIC_DMA_RX_SubPriority          1
#define IIC_DMA_TX_PreemptPriority      3
#define IIC_DMA_TX_SubPriority          0

#define IIC_EV_PreemptPriority          2
#define IIC_EV_SubPriority              1
#define IIC_ER_PreemptPriority          2
#define IIC_ER_SubPriority              0

#define IIC_POLLING_ENABLE                      (0)
#define IIC_IT_ENABLE                           (0)
#define IIC_DMA_ENABLE                          (1)
 

extern I2C_HandleTypeDef gI2C_HandleTypeStruct;
extern DMA_HandleTypeDef ghdma_IIC_rx;
extern DMA_HandleTypeDef ghdma_IIC_tx;


// 全局变量用于标志 中断 读取传输完成 
extern volatile uint8_t gIT_transfer_read_complete;
// 全局变量用于标志 中断 写入传输完成 
extern volatile uint8_t gIT_transfer_write_complete;

// 全局变量用于标志 中断 读取传输完成 
extern volatile uint8_t gDMA_transfer_read_complete;

// 全局变量用于标志 中断 写入传输完成 
extern volatile uint8_t gDMA_transfer_write_complete;

void I2C_Hardware_Init(I2C_TypeDef *I2Cx,uint32_t clockSpeed);


#endif
