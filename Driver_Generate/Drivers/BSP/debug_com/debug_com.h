#ifndef __DEBUGCOM_H_
#define __DEBUGCOM_H_


/* 用串口1支持--->仿造Windows的窗口进行通讯 */
/* 包含C语言中的IO库 */
#include <stdio.h>
#include "main.h"
#include "usart.h"


#define DEBUG_COM_TX_GPIO_PORT              GPIOA
#define DEBUG_COM_TX_GPIO_PIN               GPIO_PIN_9
#define DEBUG_COM_TX_GPIO_AF                GPIO_AF7_USART1
#define DEBUG_COM_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 发送引脚时钟使能 */
#define DEBUG_COM_TX_DMA                    DMA2_Stream7
#define DEBUG_COM_TX_CHANNEL                DMA_CHANNEL_4

#define DEBUG_COM_RX_GPIO_PORT              GPIOA
#define DEBUG_COM_RX_GPIO_PIN               GPIO_PIN_10
#define DEBUG_COM_RX_GPIO_AF                GPIO_AF7_USART1
#define DEBUG_COM_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 接收引脚时钟使能 */
#define DEBUG_COM_RX_DMA                    DMA2_Stream2
#define DEBUG_COM_RX_CHANNEL                DMA_CHANNEL_4

#define DEBUG_COM                           USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define DEBUGCOM_UART_CLK_ENABLE()          do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 时钟使能 */
#define DEBUG_DMA_CLK_ENABLE()              do{__HAL_RCC_DMA2_CLK_ENABLE(); }while(0U)     /*DMA2 使能*/

#define DMA_RX_IRQ                          DMA2_Stream2_IRQn
#define DMA_USART_RX_IRQHandler             DMA2_Stream2_IRQHandler
#define DMA_TX_IRQ                          DMA2_Stream7_IRQn
#define DMA_USART_TX_IRQHandler             DMA2_Stream7_IRQHandler

#define USART_PreemptPriority               3
#define USART_SubPriority                   2

#define USART_DMARX_PreemptPriority         3
#define USART_DMARX_SubPriority             1

#define USART_DMATX_PreemptPriority         3
#define USART_DMATX_SubPriority             0


/*开启usart接收功能*/
#define USART_TX_RX 1
/*开启usart接收中断功能*/
#define USART_RX_IRQ 0
/*开启usart的DMA功能*/
#define USART_DMA    1
/*定长与非定长*/
#define data_FIXED_ENABLE 1
#define data_FIXED_DISENABLE 0

#define RXBUFFERSIZE    2000                       /* 接收缓存大小 */
#define TXBUFFERSIZE    2000                       /* 发送缓存大小 */
#define disFIXED_SIZE   1000                       /* 非定长数据最大大小 */
/*Note:IRQLen and DMALen需要小于RXBUFFERSIZE*/
#define IRQlength       5                           /* 定长IRQ发送大小 */
#define DMAlength       2                           /* 定长DMA发送大小 */

void uartInit(USART_TypeDef *uart,uint32_t baudrate);
HAL_StatusTypeDef UartPollingFixedSelfTransceive(UART_HandleTypeDef *huart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout) ;
#endif







