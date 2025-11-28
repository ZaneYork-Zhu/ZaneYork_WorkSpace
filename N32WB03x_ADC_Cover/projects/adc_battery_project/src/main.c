#include "main.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>
 
// 全局变量定义
uint16_t g_offect = 0;
uint8_t g_adclock = 0;
// 配置参数
#define ADC_DMA_BUF_SIZE        1200
#define ADC_DMA_BUF_NUMBER      10
#define ADC_CYCLE_NUMBER        10
 
#define ADC_16BIT_DROP          15 
#define ADC_COVER_BUFFER_SIZE   250
 
#define ADCDATAADDRESS          0x1004000
#define FLASHSECTORSIZE         4096
#define ADCDATASIZE             (ADC_DMA_BUF_SIZE*ADC_DMA_BUF_NUMBER*ADC_CYCLE_NUMBER)
#define ADC_READFLASHSIZE       (ADCDATASIZE/FLASHSECTORSIZE)

#define PRINTF_ADC_SINGLE_TIME      0
#define PRINTF_ADC_MEMCPY_TIME      0
#define PRINTF_ADC_FLASHWRITE_TIME  0


// 新增滤波相关宏定义
#define FILTER_WINDOW_SIZE          5     // 移动平均滤波窗口大小
#define MIN_FILTER_SAMPLES          3     // 最小有效采样点数



typedef enum {
    ADC_DATA_8BIT = 1,
    ADC_DATA_16BIT = 2
} AdcDataType;
uint32_t writeFlashAddr = ADCDATAADDRESS;  // Flash写入位置管理
uint32_t readFlashAddr = ADCDATAADDRESS;  // Flash写入位置管理
ReturnMsg adcDataToFlashWrite(uint32_t addressBase, uint8_t* p_data,uint32_t dataLen, uint8_t dataType,uint32_t* pNextAddr);
ReturnMsg adcDataToFlashRead(uint32_t addressBase, uint8_t* p_data, uint32_t dataLen, uint8_t dataType,uint32_t* pNextAddr);
ReturnMsg adcDataToFlashErase(uint32_t addressBase, uint32_t dataLen, uint8_t dataType);
 
DMA_InitType DMA_InitStructure = {0};
// Heap_Size       EQU     0x0006000 修改堆大小
int16_t *g_adcBuffer = NULL;  // 动态分配的缓冲区指针
int16_t flashZero1[12] = {0};
int16_t ADCBuf[ADC_DMA_BUF_SIZE];
int16_t flashZero2[12] = {0};
 
// 函数声明 
static int16_t ADC16bit_DMA_Get_AVG_Val(uint16_t Channel, uint32_t avg_num);
/*******************************************************************************
* @brief 移动平均滤波处理
* @param data 原始数据数组 
* @param start 起始索引
* @param end 结束索引
* @param windowSize 滤波窗口大小
* @return 滤波后的数据值
*******************************************************************************/
static int32_t movingAverageFilter(int16_t *data, uint32_t start, uint32_t end, uint8_t windowSize)
{
    int32_t sum = 0;
    uint8_t validCount = 0;
    
    // 计算实际窗口边界
    uint32_t windowStart = (start >= windowSize/2) ? (start - windowSize/2) : 0;
    uint32_t windowEnd = (end + windowSize/2 < ADC_DMA_BUF_SIZE) ? 
                         (end + windowSize/2) : (ADC_DMA_BUF_SIZE - 1);
    
    // 累加窗口内有效数据
    for (uint32_t i = windowStart; i <= windowEnd; i++) {
        if (data[i] != 0) {  // 跳过无效数据点
            sum += data[i];
            validCount++;
        }
    }
    
    // 返回平均值或原始值
    return (validCount >= MIN_FILTER_SAMPLES) ? (sum / validCount) : data[start];
}
 
/*******************************************************************************
* @brief DMA方式获取ADC平均值 
* @param Channel ADC通道号 
* @param avg_num 采样次数
* @return ADC平均值
*******************************************************************************/
static int16_t ADC16bit_DMA_Get_AVG_Val(uint16_t Channel, uint32_t avg_num)
{
    /* 2. 配置DMA */
    DMA_DeInit(DMA_CH1);//不能忽略 精度会降低
    DMA_InitStructure.PeriphAddr = (uint32_t)&ADC->DAT;
    DMA_InitStructure.MemAddr = (uint32_t)ADCBuf;
    DMA_InitStructure.Direction = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize = avg_num;
    DMA_InitStructure.PeriphInc = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_HALFWORD;
    DMA_InitStructure.MemDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.CircularMode = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem = DMA_M2M_DISABLE;
    DMA_Init(DMA_CH1, &DMA_InitStructure);
    
    DMA_RequestRemap(DMA_REMAP_ADC, DMA, DMA_CH1, ENABLE);
    DMA_EnableChannel(DMA_CH1, ENABLE);
 
    /* 4. 等待DMA完成 */
    while(DMA_GetFlagStatus(DMA_FLAG_TC1, DMA) == RESET){;}
      return 0;
}
/**
 * @brief  Main program
 */
int main(void)
{
    log_init();
    log_info("\nthis is 16bit adc read Demo. \n");
    log_info("Please make sure J15 and J16 connect the IO to pin on board!\n");
    log_info("CH2 is PB9 which is support 0-1V.\n");
    log_info("CH3 is PB8 which is support 0-3.6V.\n");
    log_info("ADC16Bit offset:%d\n",g_offect);
    Qflash_Init();
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC | RCC_AHB_PERIPH_DMA, ENABLE);
    RCC_ConfigAdcClk(RCC_ADCCLK_SRC_AUDIOPLL);
    RCC_Enable_ADC_CLK_SRC_AUDIOPLL(ENABLE);
    /* 3. 配置ADC并启动转换 */
    ADC_EnableBypassFilter(ADC, DISABLE);
    //开启连续模式
    ADC_ConfigContinuousMode(ADC, ENABLE);
    //过采样开启
    ADC_SetOverSampleCounter(ADC,15);
    ADC_ConfigChannel(ADC, ADC_CTRL_CH_3);  // 关键：使用传入的通道参数 
    ADC_EnableDMA(ADC, ENABLE);
    ADC_Enable(ADC, ENABLE);
    // 修改点2：动态分配内存
    size_t bufferSize = ADC_DMA_BUF_SIZE * ADC_DMA_BUF_NUMBER * sizeof(int16_t);
    g_adcBuffer = (int16_t*)malloc(bufferSize);
    
    if(g_adcBuffer == NULL) {
        log_error("Memory allocation failed for g_adcBuffer!");
        while(1); // 内存分配失败时挂起系统
    }
    memset(g_adcBuffer, 0, bufferSize); // 初始化内存
    while (1)
    {
/*zuse dayin enable*/
        if(!g_adclock){
            adcDataToFlashErase(ADCDATAADDRESS,ADCDATASIZE,ADC_DATA_16BIT);
            #if PRINTF_ADC_SINGLE_TIME
            printf("[time] qflash write[ADC_CYCLE_NUMBER] adc_dma start \r\n");
            #endif
            for(uint8_t i = 0;i<ADC_CYCLE_NUMBER;i++){
                #if PRINTF_ADC_MEMCPY_TIME
                printf("[time] memcpy[ADC_DMA_BUF_NUMBER] adc_dma Start\r\n");
                #endif
                for(uint8_t j = 0;j<ADC_DMA_BUF_NUMBER;j++){
                    #if PRINTF_ADC_FLASHWRITE_TIME
                    printf("[time] single copy[ADC_DMA_BUF_SIZE] adc_dma Start\r\n");
                    #endif
                    int16_t adc_val = ADC16bit_DMA_Get_AVG_Val(ADC_CTRL_CH_3,ADC_DMA_BUF_SIZE);
                    #if PRINTF_ADC_FLASHWRITE_TIME
                    printf("[time] single copy[ADC_DMA_BUF_SIZE] adc_dma Stop\r\n");
                    #endif
                    memcpy(g_adcBuffer + ADC_DMA_BUF_SIZE*j, ADCBuf, ADC_DMA_BUF_SIZE*2);
                }
                #if PRINTF_ADC_MEMCPY_TIME
                printf("[time] memcpy[ADC_DMA_BUF_NUMBER]  adc_dma Stop\r\n");
                #endif
                Qflash_Write(writeFlashAddr + i*ADC_DMA_BUF_NUMBER*ADC_DMA_BUF_SIZE,
                                                (uint8_t *)g_adcBuffer, ADC_DMA_BUF_SIZE*ADC_DMA_BUF_NUMBER*2);
            }
            #if PRINTF_ADC_SINGLE_TIME
            printf("[time] qflash write[ADC_CYCLE_NUMBER] adc_dma stop \r\n");
            #endif
            free(g_adcBuffer);
            g_adcBuffer = NULL;
            g_adclock = 1;
            printf("\r\n*******************\r\n");
            for(uint8_t i = 0;i<ADC_DMA_BUF_NUMBER*ADC_CYCLE_NUMBER;i++){
                printf("\r\n %d \r\n",i);
                Qflash_Read(readFlashAddr + i*ADC_DMA_BUF_SIZE,
                                                (uint8_t *)ADCBuf,ADC_DMA_BUF_SIZE*2);
                uint32_t startIdx = ADC_16BIT_DROP;  // 跳过第一个块头部5个点
                while (startIdx < ADC_DMA_BUF_SIZE)
                {
                     // 1. 计算当前块边界
                    uint32_t blockEnd = startIdx + ADC_COVER_BUFFER_SIZE;
                    if (blockEnd > ADC_DMA_BUF_SIZE) {
                        blockEnd = ADC_DMA_BUF_SIZE;
                    }
 
                    // 2. 计算有效数据结束位置（跳过尾部无效数据）
                    uint32_t outputEnd = blockEnd;
                    if ((blockEnd - startIdx) > ADC_16BIT_DROP) {
                        outputEnd = blockEnd - ADC_16BIT_DROP;
                    } else {
                        outputEnd = startIdx;
                    }
 
                    // 3. 滤波处理并输出有效数据
                    for (uint32_t j = startIdx; j < outputEnd; j++) {
                        if(ADCBuf[j] != 0) {
                            // 应用移动平均滤波
                            int32_t filteredValue = movingAverageFilter(ADCBuf, j, outputEnd, FILTER_WINDOW_SIZE);
                            uint32_t adcVoltage_mV = ADC16Bit_ConverValueToVoltage(filteredValue,ADC_CTRL_CH_3,g_offect);
                            printf("%d\r\n",adcVoltage_mV);
                        }
                    }
                    
                    // 4. 定位下一块起始位置（跳过下一块头部5个点）
                    startIdx = blockEnd + ADC_16BIT_DROP;  // 跳至下一块有效数据起点 
                    
                    // 5. 边界保护（防止越界）
                    if (startIdx > ADC_DMA_BUF_SIZE) {
                        startIdx = ADC_DMA_BUF_SIZE;  // 终止循环 
                    }
                }
            }
            adcDataToFlashErase(ADCDATAADDRESS,ADCDATASIZE,ADC_DATA_16BIT);
        }
    }
}


// ================== Flash操作模块 ==================
/*******************************************************************************
 * @brief 写入ADC数据到Flash
 * @param addressBase 起始地址
 * @param p_data 数据指针
 * @param dataLen 数据长度
 * @param dataType 数据类型
 * @return 操作状态
 *******************************************************************************/
ReturnMsg adcDataToFlashWrite(uint32_t addressBase, uint8_t* p_data, uint32_t dataLen, uint8_t dataType,uint32_t* pNextAddr)
{
    /* 参数检查 */
    if (!p_data || !dataLen) {
        printf("[ERROR] Invalid input\r\n");
        return FlashWriteRegFailed;
    }
    if (dataType != ADC_DATA_8BIT && dataType != ADC_DATA_16BIT) {
        printf("[ERROR] Invalid dataType: %d\r\n", dataType);
        return FlashWriteRegFailed;
    }
 
    /* 计算数据尺寸 */
    uint32_t elementSize = (dataType == ADC_DATA_8BIT) ? 
                          sizeof(uint8_t) : sizeof(uint16_t);
    uint32_t dataSize = dataLen * elementSize;
 
    
    /* 执行Flash写入 */
    ReturnMsg flashStatus = (ReturnMsg)Qflash_Write(addressBase, p_data, dataSize);
    if(flashStatus != FlashOperationSuccess) {
        printf("[ERROR] Flash write failed: 0x%X\r\n", addressBase);
        return flashStatus;
    }
    if(pNextAddr) *pNextAddr = addressBase + dataSize;  // 更新写入位置
    return FlashOperationSuccess; 
}
/*******************************************************************************
 * @brief 写入ADC数据到Flash
 * @param addressBase 起始地址
 * @param p_data 数据指针
 * @param dataLen 数据长度
 * @param dataType 数据类型
 * @return 操作状态
 *******************************************************************************/
ReturnMsg adcDataToFlashRead(uint32_t addressBase, uint8_t* p_data, uint32_t dataLen, uint8_t dataType,uint32_t* pNextAddr)
{
    /* 参数检查 */
    if (!p_data || !dataLen) {
        printf("[ERROR] Invalid input\r\n");
        return FlashWriteRegFailed;
    }
    if (dataType != ADC_DATA_8BIT && dataType != ADC_DATA_16BIT) {
        printf("[ERROR] Invalid dataType: %d\r\n", dataType);
        return FlashWriteRegFailed;
    }
 
    /* 计算数据尺寸 */
    uint32_t elementSize = (dataType == ADC_DATA_8BIT) ? 
                          sizeof(uint8_t) : sizeof(uint16_t);
    uint32_t dataSize = dataLen * elementSize;
 
    /* 执行Flash读取 */
    ReturnMsg flashStatus = (ReturnMsg)Qflash_Read(addressBase, p_data, dataSize);
    if(flashStatus != FlashOperationSuccess) {
        printf("[ERROR] Flash read failed: 0x%X\r\n", addressBase);
        return flashStatus;
    }
    if(pNextAddr) *pNextAddr = addressBase + dataSize;  // 更新读取位置
    return FlashOperationSuccess; 
}
/*******************************************************************************
 * @brief 擦除Flash存储区域
 * @param addressBase 起始地址
 * @param dataLen 数据长度 
 * @param dataType 数据类型 
 * @return 操作状态
 *******************************************************************************/
ReturnMsg adcDataToFlashErase(uint32_t addressBase, uint32_t dataLen, uint8_t dataType)
{
    /* 计算数据尺寸 */
    uint32_t elementSize = (dataType == ADC_DATA_8BIT) ? 
                          sizeof(uint8_t) : sizeof(uint16_t);
    uint32_t dataSize = dataLen * elementSize;
    
    /* 计算需要擦除的扇区数量 */
    uint32_t eraseSectorNum = (dataSize + FLASHSECTORSIZE - 1) / FLASHSECTORSIZE;   
    
    for(uint32_t i = 0; i < eraseSectorNum; i++) {
        uint32_t currentAddr = addressBase + (i * FLASHSECTORSIZE);
        ReturnMsg flashStatus = (ReturnMsg)Qflash_Erase_Sector(currentAddr);
        
        if(flashStatus != FlashOperationSuccess) {
            printf("[ERROR] Flash erase failed: 0x%08X\r\n", currentAddr);
            return flashStatus;
        }
    }
    return FlashOperationSuccess;
}
#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param file pointer to the source file name
 * @param line assert_param error line source number
 */
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    while (1)
    {
    }
}

#endif



