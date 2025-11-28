#ifndef __DRIVER_USART_H_
#define __DRIVER_USART_H_

#include <stdint.h>
#include <stdio.h>
#include "n32wb03x_usart.h"

#define USART_PRINTF_ENABLE         (0) 
#define USART_INTERRUPT_ENABLE      (0)
#define USART_POLLING_ENABLE        (0)
#define USART_DMA_POLLING_ENABLE    (1)

#define _USART1_COM_
//#define _USART2_COM_

#ifdef _USART1_COM_
    #define USARTx              USART1
    #define USARTx_CLK          RCC_APB2_PERIPH_USART1
    #define USARTx_GPIO         GPIOB
    #define USARTx_GPIO_CLK     RCC_APB2_PERIPH_GPIOB
    #define USARTx_RxPin        GPIO_PIN_7
    #define USARTx_TxPin        GPIO_PIN_6
    #define USARTx_Rx_GPIO_AF   GPIO_AF4_USART1
    #define USARTx_Tx_GPIO_AF   GPIO_AF4_USART1
    #define GPIO_APBxClkCmd     RCC_EnableAPB2PeriphClk
    #define USART_APBxClkCmd    RCC_EnableAPB2PeriphClk

    #define USARTx_IRQn         USART1_IRQn
    #define USARTx_IRQHandler   USART1_IRQHandler

    #define USARTy_DAT_Base         (USART1_BASE + 0x04)
    #define USARTy_Tx_DMA_Channel   DMA_CH1
    #define USARTy_Tx_DMA_FLAG      DMA_FLAG_TC1
    #define USARTy_Rx_DMA_Channel   DMA_CH2
    #define USARTy_Rx_DMA_FLAG      DMA_FLAG_TC2
    #define USARTy_Tx_DMA_REMAP     DMA_REMAP_USART1_TX
    #define USARTy_Rx_DMA_REMAP     DMA_REMAP_USART1_RX

#endif

#ifdef _USART2_COM_
    #define USARTx              USART2
    #define USARTx_CLK          RCC_APB1_PERIPH_USART2
    #define USARTx_GPIO         GPIOB
    #define USARTx_GPIO_CLK     RCC_APB2_PERIPH_GPIOB
    #define USARTx_RxPin        GPIO_PIN_5
    #define USARTx_TxPin        GPIO_PIN_4
    #define USARTx_Rx_GPIO_AF   GPIO_AF3_USART2
    #define USARTx_Tx_GPIO_AF   GPIO_AF3_USART2
    #define GPIO_APBxClkCmd     RCC_EnableAPB2PeriphClk
    #define USART_APBxClkCmd    RCC_EnableAPB1PeriphClk

    #define USARTx_IRQn         USART2_IRQn
    #define USARTx_IRQHandler   USART2_IRQHandler

    #define USARTz_DAT_Base         (USART2_BASE + 0x04)
    #define USARTz_Tx_DMA_Channel   DMA_CH3
    #define USARTz_Tx_DMA_FLAG      DMA_FLAG_TC3
    #define USARTz_Rx_DMA_Channel   DMA_CH4
    #define USARTz_Rx_DMA_FLAG      DMA_FLAG_TC4
    #define USARTz_Tx_DMA_REMAP     DMA_REMAP_USART2_TX
    #define USARTz_Rx_DMA_REMAP     DMA_REMAP_USART2_RX
#endif
void uartTest(void);
static void NAL_UART_MspInit(USART_Module *huart);
static void NAL_UART_NVICInit(void);
void uartInit(USART_Module *uart,uint32_t baudrate);
FlagStatus UartPollingFixedSelfTransceive(USART_Module *huart, 
                                         uint8_t *pRxData,
                                         uint8_t *pTxData,
                                         uint16_t Size, 
                                         uint32_t Timeout);
static void NAL_UART_DMA_PollingInit(   uint32_t periphAddr, 
                                        uint8_t *TxBuf,
                                        uint8_t *RxBuf, 
                                        uint16_t TxBufSize,
                                        uint16_t RxBufSize);

#endif







