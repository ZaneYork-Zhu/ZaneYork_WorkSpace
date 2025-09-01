/**
 * @brief       Implements support for printf functions 
 * @principal   1. Disable the microcontroller's [Semihosting mode] --> Semihosting requires a debug emulator 
                2. Do not enable the [Use MicroLIB] option 
 * @function    1. Utilizes file I/O [__FILE__] as the transmission medium 
 * @param       None 
 * @retval      None 
 */

/******************************************************************************************/
#include "debug_com.h"

/*****************************************************************************************************/
 
 
/*串口接收总数据缓存区*/
uint8_t *g_tx_buffer = NULL;
/*串口发送总数据缓存区*/
uint8_t *g_rx_buffer = NULL;
/*串口单次收发数据临时缓冲区*/
uint8_t gFixedTempData[IRQorDMAlength] = {0}; 
uint8_t gVararginTempData[disFIXED_SIZE] = {0};

/*串口接收数据长度*/
UART_HandleTypeDef g_UART_HandleTypeStruct;
#if USART_DMA
    DMA_HandleTypeDef hdma_usart_rx;
    DMA_HandleTypeDef hdma_usart_tx;
#endif
  /**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 * @retval      无
 */
 
  
void uartInit(USART_TypeDef *uart,uint32_t baudrate)
{
    g_tx_buffer= (uint8_t *)malloc(TXBUFFERSIZE*sizeof(uint8_t));
    g_rx_buffer = (uint8_t *)malloc(RXBUFFERSIZE*sizeof(uint8_t));
    if(g_tx_buffer == NULL || g_rx_buffer == NULL){
        free(g_tx_buffer);
        free(g_rx_buffer);
    }
//     /* 选择UART1串口作为重映射口 */
     g_UART_HandleTypeStruct.Instance = uart;
     
     g_UART_HandleTypeStruct.Init.BaudRate = baudrate;                  /* 波特率 */
     
     g_UART_HandleTypeStruct.Init.WordLength =  UART_WORDLENGTH_8B;     /* 字长为8位数据格式 */
     
     g_UART_HandleTypeStruct.Init.StopBits =  UART_STOPBITS_1;          /* 一个停止位 */
     
     g_UART_HandleTypeStruct.Init.Parity =  UART_PARITY_NONE;           /* 无奇偶校验位 */
     
     g_UART_HandleTypeStruct.Init.HwFlowCtl = UART_HWCONTROL_NONE;      /* 无硬件流控 */
     
     g_UART_HandleTypeStruct.Init.Mode = UART_MODE_TX_RX;               /* 收发模式 */
     
     g_UART_HandleTypeStruct.Init.OverSampling = UART_OVERSAMPLING_16;

     g_UART_HandleTypeStruct.pRxBuffPtr = g_rx_buffer;
     
     g_UART_HandleTypeStruct.RxXferSize = sizeof(uint8_t);

     g_UART_HandleTypeStruct.RxXferCount = RXBUFFERSIZE;
     
     g_UART_HandleTypeStruct.TxXferSize = sizeof(uint8_t);
     
     g_UART_HandleTypeStruct.TxXferCount = TXBUFFERSIZE;
     
     g_UART_HandleTypeStruct.pTxBuffPtr = g_tx_buffer;
     
     HAL_UART_Init(&g_UART_HandleTypeStruct);                            /* HAL_UART_Init()会使能UART1 */

#if USART_RX_IRQ&&data_FIXED_ENABLE
{
    HAL_UART_Receive_IT(&g_UART_HandleTypeStruct,gFixedTempData,IRQorDMAlength);
}
#endif
#if USART_RX_IRQ&&data_FIXED_DISENABLE
    HAL_UARTEx_ReceiveToIdle_IT(&g_UART_HandleTypeStruct, gVararginTempData, disFIXED_SIZE);
#endif
#if USART_DMA&&data_FIXED_ENABLE
    HAL_UART_Receive_DMA(&g_UART_HandleTypeStruct, gFixedTempData, IRQorDMAlength); 
#endif
#if USART_DMA&&data_FIXED_DISENABLE
    HAL_UARTEx_ReceiveToIdle_DMA(&g_UART_HandleTypeStruct, gVararginTempData, disFIXED_SIZE);
#endif
    free(g_tx_buffer);
    free(g_rx_buffer);
}


 /**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // 定义GPIO初始化结构体变量，用于配置GPIO引脚 
    GPIO_InitTypeDef GPIO_InitTypeStruct;
     // 检查UART句柄指向的UART实例是否为DEBUG_COM 
    if(g_UART_HandleTypeStruct.Instance == DEBUG_COM)
    {
        // 使能DEBUG_COM_TX对应的GPIO时钟，为该GPIO引脚提供工作时钟
        DEBUG_COM_TX_GPIO_CLK_ENABLE();
        // 使能DEBUG_COM对应的UART时钟，为UART外设提供工作时钟 
        DEBUGCOM_UART_CLK_ENABLE();
    #if USART_DMA
        // 使能DMA2的时钟，为DMA2外设提供工作时钟
        DEBUG_DMA_CLK_ENABLE();
    #endif
    
    #if USART_TX_RX
     // 使能DEBUG_COM_RX对应的GPIO时钟，为该GPIO引脚提供工作时钟 
        DEBUG_COM_RX_GPIO_CLK_ENABLE();
        GPIO_InitTypeStruct.Pin = DEBUG_COM_RX_GPIO_PIN;
        GPIO_InitTypeStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitTypeStruct.Pull = GPIO_NOPULL;
        GPIO_InitTypeStruct.Alternate = DEBUG_COM_RX_GPIO_AF;
        HAL_GPIO_Init(DEBUG_COM_RX_GPIO_PORT, &GPIO_InitTypeStruct);
    #endif
        GPIO_InitTypeStruct.Pin = DEBUG_COM_TX_GPIO_PIN;
        GPIO_InitTypeStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitTypeStruct.Pull = GPIO_NOPULL;
        GPIO_InitTypeStruct.Alternate = DEBUG_COM_TX_GPIO_AF;
        HAL_GPIO_Init(DEBUG_COM_TX_GPIO_PORT, &GPIO_InitTypeStruct);

    #if USART_DMA
        // 初始化USART1_RX的DMA配置
        // 设置DMA端口
        hdma_usart_rx.Instance = DEBUG_COM_RX_DMA;
        // 设置DMA通道
        hdma_usart_rx.Init.Channel = DEBUG_COM_RX_CHANNEL;
        // 设置DMA传输方向为从外设到内存
        hdma_usart_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        // 禁止外设地址自增，因为UART接收数据时外设地址不变
        hdma_usart_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        // 使能内存地址自增，以便将接收到的数据依次存储到内存中 
        hdma_usart_rx.Init.MemInc = DMA_MINC_ENABLE;
        // 设置外设数据对齐方式为字节对齐
        hdma_usart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        // 设置内存数据对齐方式为字节对齐
        hdma_usart_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        // 设置DMA工作模式为正常模式 
        hdma_usart_rx.Init.Mode = DMA_NORMAL;
        // 设置DMA优先级为低
        hdma_usart_rx.Init.Priority = DMA_PRIORITY_LOW;
        // 禁用FIFO模式 
        hdma_usart_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        // 调用HAL库函数初始化DMA 
        HAL_DMA_Init(&hdma_usart_rx);

        /*将UART句柄的hdmarx成员与hdma_usart_rx链接起来，建立UART与DMA的关联*/
        __HAL_LINKDMA(huart,hdmarx,hdma_usart_rx);

        hdma_usart_tx.Instance = DEBUG_COM_TX_DMA;
        hdma_usart_tx.Init.Channel = DEBUG_COM_TX_CHANNEL;
        hdma_usart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart_tx.Init.Mode = DMA_NORMAL;
        hdma_usart_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        /*链接DMA通道句柄*/
        __HAL_LINKDMA(huart,hdmatx,hdma_usart_tx);
        HAL_DMA_Init(&hdma_usart_tx);

        /* DMA2_Stream2_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA_RX_IRQ, USART_DMARX_PreemptPriority, USART_DMARX_SubPriority);
        HAL_NVIC_EnableIRQ(DMA_RX_IRQ);
        /* DMA2_Stream7_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA_TX_IRQ, USART_DMATX_PreemptPriority, USART_DMATX_SubPriority);
        HAL_NVIC_EnableIRQ(DMA_TX_IRQ);

        HAL_NVIC_SetPriority(USART_UX_IRQn,USART_PreemptPriority,USART_SubPriority);
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);
    #endif
    #if USART_RX_IRQ    
        // 设置UART中断的抢占优先级为USART_PreemptPriority，子优先级为USART_SubPriority 
        HAL_NVIC_SetPriority(USART_UX_IRQn,USART_PreemptPriority,USART_SubPriority);
        // 使能UART的中断
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);
    #endif
    }
}

/** 
 * @brief  使用轮询方式进行UART固定长度的自收发操作 
 * @param  huart: UART句柄指针 
 * @param  pTxData: 待发送的数据缓冲区指针 
 * @param  pRxData: 用于接收数据的缓冲区指针 
 * @param  Size: 要发送和接收的数据长度 
 * @param  Timeout: 超时时间（以毫秒为单位） 
 * @retval HAL_StatusTypeDef: 操作结果状态码 
 */ 
HAL_StatusTypeDef UartPollingFixedSelfTransceive(UART_HandleTypeDef *huart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout) 
{ 
    HAL_StatusTypeDef status; 
 
    // 发送数据 
    status = HAL_UART_Transmit(huart, pTxData, Size, Timeout); 
    if (status != HAL_OK) { 
        return status; 
    } 
 
    // 接收数据 
    status = HAL_UART_Receive(huart, pRxData, Size, Timeout); 
    if (status != HAL_OK) { 
        return status; 
    } 
 
    return HAL_OK; 
} 
/**
 * @brief       串口定长中断自收发函数 
 *              注意：USART_TX_RX = 1
 * @param       IRQlength 收发数据固定长度（单位：字节）
 * @retval      HAL_Status
 */

#if USART_RX_IRQ&&data_FIXED_ENABLE
    /*中断服务函数*/
    void USART_UX_IRQHandler(void)
    {
        /*调用 HAL 库中断处理公用函数*/
        HAL_UART_IRQHandler(&g_UART_HandleTypeStruct);
    }
    /*中断回调函数*/
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        if(g_UART_HandleTypeStruct.Instance == DEBUG_COM)
        {
            HAL_UART_Transmit(&g_UART_HandleTypeStruct, gFixedTempData, IRQorDMAlength, HAL_MAX_DELAY);
            HAL_UART_Receive_IT(&g_UART_HandleTypeStruct, gFixedTempData, IRQorDMAlength); // 继续接收
            
        }
    }
#endif
/**
 * @brief       串口非定长中断自收发函数 
 *              注意：USART_TX_RX = 1
 * @param       IRQlength 收发数据固定长度（单位：字节）
 * @retval      HAL_Status
 */

 #if USART_RX_IRQ&&data_FIXED_DISENABLE
 /*中断服务函数*/
 void USART_UX_IRQHandler(void)
 {
     /*调用 HAL 库中断处理公用函数*/
     HAL_UART_IRQHandler(&g_UART_HandleTypeStruct);
 }
 /*中断回调函数*/
 void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
 {
     if(g_UART_HandleTypeStruct.Instance == DEBUG_COM)
     {
         HAL_UART_Transmit(&g_UART_HandleTypeStruct, gVararginTempData, Size, HAL_MAX_DELAY);
         HAL_UARTEx_ReceiveToIdle_IT(&g_UART_HandleTypeStruct, gVararginTempData, disFIXED_SIZE); // 继续接收
     }
 }
#endif

/**
 * @brief       串口DMA自收发函数 
 * @param       DMAlength 收发数据固定长度（单位：字节）
 *              注意:要开3个中断函数
 * @retval      HAL_Status
 */
#if USART_DMA
    /*DMA接收中断函数*/
    void DMA_USART_RX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&hdma_usart_rx);
    }

    /*DMA发送中断函数*/
    void DMA_USART_TX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&hdma_usart_tx);
    }
    #if data_FIXED_ENABLE
        /*串口中断服务函数*/
        void USART_UX_IRQHandler(void)
        {
            /*调用 HAL 库中断处理公用函数*/
            HAL_UART_IRQHandler(&g_UART_HandleTypeStruct);
        }

        /*中断回调函数*/
        void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
        {
            if(g_UART_HandleTypeStruct.Instance == DEBUG_COM)
            {
                if(HAL_UART_Transmit_DMA(&g_UART_HandleTypeStruct, gFixedTempData, IRQorDMAlength) == HAL_OK)
                {
                    HAL_UART_Receive_DMA(&g_UART_HandleTypeStruct, gFixedTempData, IRQorDMAlength); // 继续接收
                }
            }
        }
    #endif
    #if data_FIXED_DISENABLE
        /*串口中断服务函数*/
        void USART_UX_IRQHandler(void)
        {
            /*调用 HAL 库中断处理公用函数*/
            HAL_UART_IRQHandler(&g_UART_HandleTypeStruct);
        }

        /*中断回调函数*/
        void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
        {
            if(g_UART_HandleTypeStruct.Instance == DEBUG_COM)
            {
                if(HAL_UART_Transmit_DMA(&g_UART_HandleTypeStruct, gVararginTempData, Size) == HAL_OK)
                {
                    HAL_UARTEx_ReceiveToIdle_DMA(&g_UART_HandleTypeStruct, gVararginTempData, disFIXED_SIZE); // 继续接收
                }
            }
        }
    #endif
#endif



/* USER CODE BEGIN 1 */
/*******************************************************************************************/
/* 加入以下代码, 支持printf函数, 需要选择use MicroLIB */
 
 
 //#if 1                                /* 确保任何条件下，都会编译这段代码*/
 #if defined(__ARMCC_VERSION)  
 
    #if (__ARMCC_VERSION >= 6010050)  
        // AC6 编译器相关的代码  
        __asm(".global __use_no_semihosting\n\t");  
        __asm(".global __ARM_use_no_argv \n\t");  
  
    #elif (__ARMCC_VERSION >= 5060020)  
        // AC5.06 或更高版本的 AC5 编译器相关的代码  
        #pragma import(__use_no_semihosting)  
 
    #endif

//struct __FILE
//{
//    int handle;
//    /* Whatever you require here. If the only file you are using is */
//    /* standard output using printf() for debugging, no file handling */
//    /* is required. */
//};        
/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;
FILE __stdin;
/*******************************实现printf支持***********************************************************/
/**
 * @brief       实现printf支持
 * @local       [stdio.h]的673行
 * @param       1.ch:字符写入值
 *              2.stream:重定向的文件流
 *              发送hello--->对应发送5次
 * @retval      成功-->返回ch||失败-->返回EOF
 */
 
 /*轮询实现太慢-要操作2次缓冲区*/
/////重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
//int fputc(int ch, FILE *stream)
//{
//    uint8_t data = (uint8_t)ch; 
//    uint8_t dummy; 
//    HAL_StatusTypeDef status = UartPollingFixedSelfTransceive(&g_UART_HandleTypeStruct, &data, &dummy, 1, 100); 
//    if (status == HAL_OK) 
//    { 
//        return ch; 
//    } 
//    else 
//    { 
//        return EOF; 
//    } 
//}


// 重定向fputc 
int fputc(int ch, FILE *stream)
{
    // 每次只发送一个字节数据到串口 DEBUG_USART 
    while (__HAL_UART_GET_FLAG(&g_UART_HandleTypeStruct, UART_FLAG_TXE) != SET); 
    HAL_UART_Transmit(&g_UART_HandleTypeStruct, (uint8_t *)&ch, 1, 1000); 
    return ch; 
}

int fgetc(FILE *stream)
{
    uint8_t data;
    // 等待接收数据就绪
    while (__HAL_UART_GET_FLAG(&g_UART_HandleTypeStruct, UART_FLAG_RXNE) != SET);
    HAL_UART_Receive(&g_UART_HandleTypeStruct, &data, 1, 1000);
    return (int)data; // 返回接收到的字符
}

#endif
/* USER CODE END 1 */
