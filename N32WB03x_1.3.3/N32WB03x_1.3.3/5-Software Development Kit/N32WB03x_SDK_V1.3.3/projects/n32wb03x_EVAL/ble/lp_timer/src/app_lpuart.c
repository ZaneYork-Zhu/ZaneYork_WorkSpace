/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file app_lpuart.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
 
/* NOTE:
 Please use 32768HZ LSI(or LSE) to support 9600 baud. 
 Add "app_handler.lsc_cfg = BLE_LSC_LSI_32768HZ;" when functon call ns_ble_stack_init.
 Define LSI_TRIM_32768HZ as global macro.
 */

#include "n32wb03x.h"
#include "app_lpuart.h"
#include "ns_timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LPUARTy                 LPUART1
#define LPUARTy_CLK             RCC_LPUART1CLK
#define LPUARTy_GPIO            GPIOB
#define LPUARTy_GPIO_CLK        RCC_APB2_PERIPH_GPIOB
#define LPUARTy_RxPin           GPIO_PIN_11
#define LPUARTy_TxPin           GPIO_PIN_12
#define LPUARTy_Rx_GPIO_AF      GPIO_AF2_LPUART1
#define LPUARTy_Tx_GPIO_AF      GPIO_AF2_LPUART1
#define LPUARTy_IRQn            LPUART1_IRQn
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned char Rx_data[256] = {0};
unsigned char Rx_len = 0;
timer_hnd_t lpuart_timer_hdl = 0xff;
/* Private function prototypes -----------------------------------------------*/
static void GPIO_Configuration(void);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  LPUART demo code
 * @param  
 * @return 
 * @note   
 */
void app_lpuart_output_demo(void)
{
    static unsigned char tx_idx = 0;
    while(tx_idx != Rx_len)
    {
        app_lpuart_send_data(&Rx_data[tx_idx],1);
        tx_idx++;
    }
    lpuart_timer_hdl = 0xff;
}



/**
 * @brief  LPUART initialization
 * @param  
 * @return 
 * @note   
 */
void app_lpuart_init(void)
{
	LPUART_InitType LPUART_InitStructure;
	NVIC_InitType NVIC_InitStructure;
	EXTI_InitType EXTI_InitStructure;
    
    /* Specifies the LPUARTy clock source, LSE selected as LPUARTy clock */
    RCC_ConfigLpuartClk(LPUARTy_CLK, RCC_LPUARTCLK_SRC_LSI_LSE);
    /* Enable LPUARTy Clock */
    RCC_EnableLpuartClk(ENABLE);   
    
    /* Enable the LPUARTx Wakeup Interrupt through EXTI line 10 */
    EXTI_InitStructure.EXTI_Line	= EXTI_LINE10;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
    
	/* Enable the LPUART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = LPUARTy_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority           = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the LPUART Wakeup Interrupt through EXTI line 10 */
	NVIC_InitStructure.NVIC_IRQChannel                  = EXTI4_12_IRQn;
	NVIC_Init(&NVIC_InitStructure);
    
    /* Configure GPIO for LPUART */
    GPIO_Configuration();

    /* USARTx and USARTz configuration ------------------------------------------------------*/
    LPUART_InitStructure.BaudRate            = 9600; 
    LPUART_InitStructure.Parity              = LPUART_PE_NO;
	LPUART_InitStructure.RtsThreshold        = LPUART_RTSTH_FIFOHF;
    LPUART_InitStructure.HardwareFlowControl = LPUART_HFCTRL_NONE;
    LPUART_InitStructure.Mode                = LPUART_MODE_RX | LPUART_MODE_TX;

    /* Configure USARTx */
    LPUART_Init(LPUARTy, &LPUART_InitStructure);
	/* Flush LPUARTy RX FIFO */
	LPUART_FlushRxFifo(LPUARTy);
    
    LPUART_ConfigInt(LPUARTy, LPUART_INT_FIFO_NE, ENABLE);
    
    /* Set the Wake-Up Event: specify wake-up on start bit */
    LPUART_ConfigWakeUpMethod(LPUARTy, LPUART_WUSTP_STARTBIT );

    /* Enable MCU Wake-up by LPUARTx */
    LPUART_EnableWakeUpStop(LPUARTy, ENABLE);
}


/**
 * @brief  LPUART disable
 * @param  
 * @return 
 * @note   
 */
void app_lpuart_deinit(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_EnableLpuartClk(DISABLE); 
	        
    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);    
    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = LPUARTy_TxPin|LPUARTy_RxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF0;
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure); 
}

void app_lpuart_receive_data(uint8_t *p_data,uint16_t len)
{
    while(len--)
    {
        Rx_data[Rx_len++] = *p_data;
        p_data++;
    }
    
    #if 1 //enable output demo
    if(lpuart_timer_hdl == 0xff)
    {
        lpuart_timer_hdl = ns_timer_create(10,app_lpuart_output_demo);
    }
    else{
        ns_timer_modify(lpuart_timer_hdl,10);
    }
    #endif
}
/**
 * @brief  LPUART send data 
 * @param  
 * @return 
 * @note   
 */
void app_lpuart_send_data(const uint8_t *data,uint16_t len)
{
	uint16_t i = 0;
	for(i = 0; i < len;i++)
	{
		LPUART_SendData(LPUARTy,data[i]);
		while (LPUART_GetFlagStatus(LPUARTy, LPUART_FLAG_TXC) == RESET);
		/* must clear the TXC flag */
		LPUART_ClrFlag(LPUARTy, LPUART_FLAG_TXC); 
	}
}

/**
 * @brief  LPUART GPIO configuration
 * @param  
 * @return 
 * @note   
 */
static void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(LPUARTy_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);    

    /* Configure LPUARTy Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = LPUARTy_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = LPUARTy_Tx_GPIO_AF;
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure);

    /* Configure LPUARTy Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = LPUARTy_RxPin;  
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = LPUARTy_Rx_GPIO_AF;
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure);
}


/**
 * @}
 */
