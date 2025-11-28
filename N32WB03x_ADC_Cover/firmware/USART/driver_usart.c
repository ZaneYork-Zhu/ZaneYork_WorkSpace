/**
 * @brief       Implements support for printf functions 
 * @principal   1. Disable the microcontroller's [Semihosting mode] --> Semihosting requires a debug emulator 
                2. Do not enable the [Use MicroLIB] option 
 * @function    1. Utilizes file I/O [__FILE__] as the transmission medium 
 * @param       None 
 * @retval      None 
 */

/******************************************************************************************/
#include "driver_usart.h"
#include "n32wb03x.h"
USART_InitType g_uSARTInitStructure;
/*****************************************************************************************************/
#define USART_BUF_SIZE 64  // 扩大缓冲区防止溢出
 
__IO uint8_t g_TxBuf[USART_BUF_SIZE] = {0xDA, 0x2F, 0x84, 0xBD, 0x5B, 0xCB, 0x4C, 0x0E,  
                                        0x9B, 0xE6, 0x63, 0x08, 0xF4, 0xA7, 0x27, 0x79,  
                                        0x3F, 0xB1, 0xFA, 0x15, 0x6C, 0xC4, 0x53, 0x2D,  
                                        0x0C, 0xEE, 0x47, 0x98, 0xE1, 0x5D, 0xB4, 0x1D,   
                                        0x8D, 0x38, 0xD9, 0x67, 0xC2, 0x43, 0xF6, 0x12,  
                                        0x4F, 0xA2, 0xEA, 0x05, 0x80, 0xD3, 0x25, 0x95,  
                                        0xBF, 0x18, 0x6E, 0xFD, 0x2A, 0xAD, 0x56, 0xC8,  
                                        0x21, 0x91, 0xD0, 0x48, 0x9F, 0xE3, 0x3D, 0xB8};
__IO uint8_t g_RxBuf[USART_BUF_SIZE];
//头指针和尾指针
__IO uint16_t g_TxHead = 0, g_TxTail = 0;  // 发送缓冲区指针 
__IO uint8_t g_TxBusy = false;  // 发送状态标志
/*****************************************************************************************************/

void uartTest(void)
{
    uartInit(USARTx, 115200);
    #if USART_PRINTF_ENABLE
    printf("test usart printf function\r\n");
    #endif
    while(1){
        #if USART_POLLING_ENABLE
        UartPollingFixedSelfTransceive(USARTx,(uint8_t *)g_TxBuf,(uint8_t *)g_TxBuf,USART_BUF_SIZE,0);
        #endif
    }
}

/**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 * @retval      无
 */
void uartInit(USART_Module *uart,uint32_t baudrate)
{
    /* Enable USARTx Clock */
    USART_APBxClkCmd(USARTx_CLK, ENABLE);
    /* USARTy and USARTz configuration ------------------------------------------------------*/
    g_uSARTInitStructure.BaudRate            = baudrate;
    g_uSARTInitStructure.WordLength          = USART_WL_8B;
    g_uSARTInitStructure.StopBits            = USART_STPB_1;
    g_uSARTInitStructure.Parity              = USART_PE_NO;
    g_uSARTInitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    g_uSARTInitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
    /* Configure USARTx */
    USART_Init(uart, &g_uSARTInitStructure);
    /* Enable the USARTx */
    USART_Enable(uart, ENABLE);
    NAL_UART_MspInit(uart);
}

 /**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
static void NAL_UART_MspInit(USART_Module *huart)
{
    GPIO_InitType GPIO_InitStructure;
    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);  
    /* Enable GPIO clock */
    GPIO_APBxClkCmd(USARTx_GPIO_CLK, ENABLE);
    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = USARTx_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = USARTx_Tx_GPIO_AF;
    GPIO_InitPeripheral(USARTx_GPIO, &GPIO_InitStructure);   

    /* Configure USARTx Rx as alternate function push-pull */
    GPIO_InitStructure.Pin            = USARTx_RxPin;
    GPIO_InitStructure.GPIO_Alternate = USARTx_Rx_GPIO_AF;
    GPIO_InitPeripheral(USARTx_GPIO, &GPIO_InitStructure);
    #if USART_INTERRUPT_ENABLE
    //开启中断
    NAL_UART_NVICInit();
    USART_ConfigInt(USARTx, USART_INT_RXDNE, ENABLE);
    USART_ConfigInt(USARTx, USART_INT_TXDE, DISABLE);  // 初始禁用
    #endif /* USART_INTERRUPT_ENABLE */
    #if USART_DMA_POLLING_ENABLE
    //DMA初始化
    NAL_UART_DMA_PollingInit(USARTy_DAT_Base, (uint8_t *)g_TxBuf,(uint8_t *)g_RxBuf, USART_BUF_SIZE,USART_BUF_SIZE);
    #endif /* USART_DMA_POLLING_ENABLE */
}

static void NAL_UART_DMA_PollingInit(   uint32_t periphAddr, 
                                        uint8_t *TxBuf,
                                        uint8_t *RxBuf, 
                                        uint16_t TxBufSize,
                                        uint16_t RxBufSize)
{
    /* DMA clock enable */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA, ENABLE);
    DMA_InitType DMA_InitStructure = {0};
    /* USARTy TX DMA Channel (triggered by USARTy Tx event) Config */
    DMA_DeInit(USARTy_Tx_DMA_Channel);
    DMA_InitStructure.PeriphAddr     = USARTy_DAT_Base;
    DMA_InitStructure.MemAddr        = (uint32_t)TxBuf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = TxBufSize;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(USARTy_Tx_DMA_REMAP, DMA, USARTy_Tx_DMA_Channel, ENABLE);   
    
    /* USARTy RX DMA Channel (triggered by USARTy Rx event) Config */
    DMA_DeInit(USARTy_Rx_DMA_Channel);
    DMA_InitStructure.PeriphAddr = USARTy_DAT_Base;
    DMA_InitStructure.MemAddr    = (uint32_t)RxBuf;
    DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize    = RxBufSize;
    DMA_Init(USARTy_Rx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(USARTy_Rx_DMA_REMAP, DMA, USARTy_Rx_DMA_Channel, ENABLE);

    /* Enable USARTy DMA Rx and TX request */
    USART_EnableDMA(USARTx, USART_DMAREQ_RX | USART_DMAREQ_TX, ENABLE);
    /* Enable USARTy TX DMA Channel */
    DMA_EnableChannel(USARTy_Tx_DMA_Channel, ENABLE);
    /* Enable USARTy RX DMA Channel */
    DMA_EnableChannel(USARTy_Rx_DMA_Channel, ENABLE);
}



/**
 * @brief  This function handles USART interrupt request.
 * @param  None
 * @retval None
 */

static void NAL_UART_NVICInit(void)
{
    NVIC_InitType NVIC_InitStructure;
    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USARTx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority           = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USARTx_IRQHandler(void) {
    // 接收中断处理 
    if (USART_GetIntStatus(USARTx, USART_INT_RXDNE) != RESET) {
        USART_ClrIntPendingBit(USARTx, USART_INT_RXDNE);
        uint8_t data = USART_ReceiveData(USARTx);
        
        // 存入待发送缓冲区
        uint16_t next = (g_TxHead + 1) % USART_BUF_SIZE;
        if (next != g_TxTail) {
            g_TxBuf[g_TxHead] = data;   // 存入待发送数据 
            g_TxHead = next;
        }
        
        // 若发送空闲则启动发送 
        if (!g_TxBusy) {
            g_TxBusy = true;
            USART_ConfigInt(USARTx, USART_INT_TXDE, ENABLE);  // 使能发送中断
        }
    }
 
    // 发送中断处理 
    if (USART_GetIntStatus(USARTx, USART_INT_TXDE) != RESET) {
        if (g_TxTail != g_TxHead) {
            USART_SendData(USARTx, g_TxBuf[g_TxTail]);
            g_TxTail = (g_TxTail + 1) % USART_BUF_SIZE;
        } else {
            USART_ConfigInt(USARTx, USART_INT_TXDE, DISABLE);  // 缓冲区空则关闭中断 
            g_TxBusy = false;
        }
    }
}


/**
 * @brief  UART轮询自收发函数
 * @param  huart: UART句柄指针 
 * @param  pRxData: 接收数据缓冲区
 * @param  pTxData: 发送数据缓冲区（将被接收数据覆盖）
 * @param  Size: 数据长度
 * @param  Timeout: 超时时间（毫秒）
 * @retval FlagStatus: 操作结果状态码 
 */
FlagStatus UartPollingFixedSelfTransceive(USART_Module *huart, 
                                         uint8_t *pRxData,
                                         uint8_t *pTxData,
                                         uint16_t Size, 
                                         uint32_t Timeout)
{
    uint16_t txCount = 0;               // 发送数据计数器 
    uint8_t txActive = false;           // 发送状态标志
 
    //环形缓冲区控制变量
    uint16_t txHead = 0;                // 发送缓冲区头指针
    uint16_t txTail = 0;                // 发送缓冲区尾指针
 
    while (txCount < Size)              // 必须发送完所有数据 
    {
        /*------ 接收处理（RXDNE）------*/
        if (USART_GetFlagStatus(huart, USART_FLAG_RXDNE)){
            // 读取接收数据寄存器 
            uint8_t data = USART_ReceiveData(huart);
            
            // 存入发送缓冲区
            pTxData[txHead] = data;  // 同步准备发送数据 
            
            // 更新发送指针（模拟环形缓冲区）
            txHead = (txHead + 1) % Size;
            txCount++;
            
            // 激活发送流程（对应中断使能逻辑）
            if (!txActive) txActive = true; 
        }
        
        /*------ 发送处理（TXDE）------*/
        if (txActive && USART_GetFlagStatus(huart, USART_FLAG_TXDE)){
            if (txTail != txHead)  // 缓冲区有数据待发送 
            {
                USART_SendData(huart, pTxData[txTail]);
                txTail = (txTail + 1) % Size;
                txCount++;
            }
            else  // 缓冲区已空
            {
                txActive = false;  // 模拟关闭发送中断 
            }
        }
        
    }
    return SET;  // 成功完成 
}


/**
 * @brief  Retarget the C library printf function to the USART
 * @param  ch: The character to transmit
 * @param  f: pointer to a FILE structure that identifies the stream
 * @retval The character written. 
 */
int fputc(int ch, FILE* f)
{
    USART_SendData(USARTx, (uint8_t)ch);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);

    return (ch);
}

/**
 * @brief Assert failed function by user.
 * @param file The name of the call that failed.
 * @param line The source line number of the call that failed.
 */
#ifdef USE_FULL_ASSERT
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    while (1)
    {
    }
}
#endif // USE_FULL_ASSERT
