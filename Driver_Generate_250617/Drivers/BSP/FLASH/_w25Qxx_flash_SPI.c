

#include "_w25Qxx_flash_SPI.h"
#include "debug_com.h"

#if _SOFTWARE_SPI_ENABLE
    #include "_software_SPI.h"
#endif

#if _HARDWARE_SPI_ENABLE
    #include "_hardware_SPI.h"
#endif

/*用于接收，发送不需要接收的地方*/

uint8_t temp = 0;

// 定义适配JEDEC_ID的芯片列表 
static const JEDEC_ID chipList[] = {
    {WINBOND_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_8Mbit}, // W25Q80芯片ID 
    {WINBOND_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_16Mbit}, // W25Q16芯片ID 
    {WINBOND_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_32Mbit}, // W25Q32芯片ID 
    {WINBOND_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_64Mbit}, // W25Q64芯片ID 
    {WINBOND_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_128Mbit}, // W25Q128芯片ID 
    {WINBOND_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_256Mbit}, // W25Q256芯片ID 
    {BY_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_64Mbit}, // BY25Q64芯片ID 
    {BY_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_128Mbit}, // BY25Q128芯片ID 
    {NM_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_64Mbit}, // NM25Q64芯片ID 
    {NM_MANU, SPI_NOR_FLASH_TYPE, CAPACITY_128Mbit}  // NM25Q128芯片ID 
};
void _W25Q128_textInit(void)
{
    uint8_t ArraryWrite[] = {0xA1,0xB2,0xC3,0xD4};
    uint8_t ArraryWrite1[] = {0xA5};
    uint8_t ArraryRead[4] = {0};
    uint8_t ArraryRead1[4] = {0};
    FLASH_State state = FLASH_OK;
    uartInit(USART1,115200);
    _W25Q128_Init();
    // _W25Q128_check();
    // state = _W25Q128_eraseBlock(1);
    // state = _W25Q128_eraseBlock(0);
    // state = _W25Q128_onePageWrite(0x0010000,ArraryWrite1,1);
    // state = _W25Q128_onePageWrite(0x0000000,ArraryWrite,4);
    state = _W25Q128_Read(0x0010000,ArraryRead,1);
    if(state == FLASH_OK)
    {
        for(uint8_t i = 0;i<1;i++)
        {
            printf("ArraryRead = %x\r\n",ArraryRead[i]);
        }
        state = _W25Q128_Read(0x0000000,ArraryRead1,4);
        for(uint8_t i = 0;i<4;i++)
        {
            printf("ArraryRead1 = %x\r\n",ArraryRead1[i]);
        }
    }
    else
    {
        printf("text error \r\n");
    }
}

void _W25Q128_Init(void)
{

#if _SOFTWARE_SPI_ENABLE
    /*初始化software SPI*/
    _software_SPI_GPIO_Init(MODE0);
#endif

#if _HARDWARE_SPI_ENABLE
    /*初始化hardware SPI*/
    FLASH_hardware_SPI_GPIO_Init(0,FLASH_HARDWARE_SPI,SPI_BAUDRATEPRESCALER_256);

#endif
}

void _W25Q128_check(void)
{
    JEDEC_ID gNorFlashDId = {0};
    /*匹配成功标志*/
    uint8_t isMatched = 0; 
    /*匹配设备ID*/
    _W25Q128_ReadID(&gNorFlashDId);
    
    // 轮询匹配设备ID 
    for (uint16_t i = 0; i < sizeof(chipList) / sizeof(chipList[0]); i++) 
    { 
        if (gNorFlashDId.manuFacturerId  == chipList[i].manuFacturerId && 
            gNorFlashDId.memoryType  == chipList[i].memoryType && 
            gNorFlashDId.capacityId  == chipList[i].capacityId) 
            { 
                isMatched = 1; 
                break; 
            }
    }
    if(isMatched)
    {
        printf("{ \r\n");
        printf("\t");
        printf("gNorFlashDId.manuFacturerId = %x \r\n",gNorFlashDId.manuFacturerId);
        printf("\t");
        printf("gNorFlashDId.memoryType = %x \r\n",gNorFlashDId.memoryType);
        printf("\t");
        printf("gNorFlashDId.capacityId = %x \r\n",gNorFlashDId.capacityId);
        printf("}");
        printf("\r\n");
    }
    else
    {
        // 匹配失败，进行错误处理 
        // 例如，可以通过串口输出错误信息 
        printf("Device ID matching failed!\r\n"); 
    }
}

/**
 * @brief   获取厂商ID和设备ID
 * @param   MID:厂商ID
 * @param   DID:设备ID
 * @retval  无
 */
FLASH_State _W25Q128_ReadID(JEDEC_ID *driverId)
{
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    /*片选开始*/
    spi_Start();
    {
        /*从 FLASH 芯片中发送读取ID号【指令】*/
        spiState = spiSoftwareSwapByte(FlashJedecDeviceId,NULL);
        /*从 FLASH 芯片中接收<厂商>ID号【数据】*/
        spiState = spiSoftwareSwapByte(Flash_DUMMY_BYTE,&driverId->manuFacturerId);
        /*从 FLASH 芯片中接收内存类型*/
        spiState = spiSoftwareSwapByte(Flash_DUMMY_BYTE,&driverId->memoryType);
        /*从 FLASH 芯片中接收容量标识*/
        spiState = spiSoftwareSwapByte(Flash_DUMMY_BYTE,&driverId->capacityId);
    }
    spi_Stop();
    if(spiState != SOFTWARE_SPI_FINISH)
    {   
        
        printf("Device JEDEC_ID receive ERROR !\r\n");
        return FLASH_ERROR;
    }  
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    /*拉低CS--片选SPI*/
    FLASH_HARDWARE_SPI_CS(0);
    {
        /*从 FLASH 芯片中发送读取ID号【指令】*/ 
        spiState = flash_spiHardwareSwapByte(FlashJedecDeviceId, NULL); // 传递地址 
        printf("Device spiState[1] = %d \r\n", (uint8_t) spiState); 
        /*从 FLASH 芯片中接收<厂商>ID号【数据】*/ 
        spiState = flash_spiHardwareSwapByte(Flash_DUMMY_BYTE, &driverId->manuFacturerId); // 传递地址 
        printf("Device spiState[2] = %d \r\n", (uint8_t) spiState); 
        /*从 FLASH 芯片中接收内存类型*/ 
        spiState = flash_spiHardwareSwapByte(Flash_DUMMY_BYTE, &driverId->memoryType); // 传递地址 
        printf("Device spiState[3] = %d \r\n", (uint8_t) spiState); 
        /*从 FLASH 芯片中接收容量标识*/ 
        spiState = flash_spiHardwareSwapByte(Flash_DUMMY_BYTE, &driverId->capacityId); // 传递地址 
        printf("Device spiState[4] = %d \r\n", (uint8_t) spiState); 
    }
    /*恢复SPI空闲*/
    FLASH_HARDWARE_SPI_CS(1);
    if(spiState != HARDWARE_SPI_FINISH)
    {   
        
        printf("Device JEDEC_ID receive ERROR !\r\n");
        return FLASH_ERROR;
    }  
#endif

    return FLASH_OK;
}

/**
 * @brief   实现写使能
 * @param   无
 * @retval  无
 */
static FLASH_State _W25Q128_writeEnable(void)
{
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    /*片选开始*/
    spi_Start();
    {
        // 使能写入操作 
        spiState = spiSoftwareSwapByte(FlashWriteEnable,NULL);
    }
    spi_Stop();
    if(spiState != SOFTWARE_SPI_FINISH)
    {
        printf("Device writeEnable ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    /*拉低CS*/
    FLASH_HARDWARE_SPI_CS(0);
    {
        // 使能写入操作 
        spiState = flash_spiHardwareSwapByte(FlashWriteEnable,NULL);
    }
    /*恢复SPI空闲*/
    FLASH_HARDWARE_SPI_CS(1);
    if(spiState != HARDWARE_SPI_FINISH)
    {
        printf("Device writeEnable ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif
    return FLASH_OK;  
    
}

/**
 * @brief       等待空闲BUSY位
 * @param       无
 * @retval      FLASH_State:当前FLASH是否空闲
 */
static FLASH_State _W25Q128_waitBusy(void)
{
    uint32_t timeOut = WAIT_BUSY_TIME;
    uint8_t waitBusyData = {0};
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    /*片选开始*/
    spi_Start();
    {
        // 读取状态寄存器1 
        spiState = spiSoftwareSwapByte(FlashReadStatusReg1,NULL);
        // 获取当前BUSY的状态
        do{
            // 再次发送DUMMY字节读取最新状态
            spiState = spiSoftwareSwapByte(Flash_DUMMY_BYTE,&waitBusyData);
            if (spiState != SOFTWARE_SPI_FINISH)
            {
                spi_Stop();
                return FLASH_ERROR;
            }
            timeOut--;
        }while ((waitBusyData & 0x01) == 0x01 && timeOut > 0);
    }
    spi_Stop();
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    FLASH_HARDWARE_SPI_CS(0);
    {
        // 读取状态寄存器1 
        spiState = flash_spiHardwareSwapByte(FlashReadStatusReg1,NULL);
        // 获取当前BUSY的状态
        // 循环检查BUSY位，直到空闲或超时
        do{
            // 再次发送DUMMY字节读取最新状态
            spiState = flash_spiHardwareSwapByte(Flash_DUMMY_BYTE,&waitBusyData);
            if (spiState != HARDWARE_SPI_FINISH)
            {
                FLASH_HARDWARE_SPI_CS(1);
                return FLASH_ERROR;
            }
            timeOut--;
        }while ((waitBusyData & 0x01) == 0x01 && timeOut > 0);
    }
    FLASH_HARDWARE_SPI_CS(1);
#endif
    if (timeOut == 0) 
    {
        return FLASH_ERROR;  // 超时处理 
    }
    return FLASH_OK;
}
/**
 * @brief   发送地址函数
 * @param   无
 * @retval  无
 */
static FLASH_State sendAddress(uint32_t address) 
{ 

#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;  
    /* 发送 bit23 ~ bit16 地址 */
    spiState = spiSoftwareSwapByte((uint8_t)(address>> 16),NULL);
    /* 发送 bit15 ~ bit8  地址 */ 
    spiState = spiSoftwareSwapByte((uint8_t)(address >> 8),NULL); 
    /* 发送 bit7  ~ bit0  地址 */
    spiState = spiSoftwareSwapByte((uint8_t)(address),NULL); 
    if(spiState != SOFTWARE_SPI_FINISH)
    {
        printf("Device sendAddress ERROR !\r\n");
        return FLASH_ERROR;
    } 
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    /* 发送 bit23 ~ bit16 地址 */
    spiState = flash_spiHardwareSwapByte((uint8_t)(address>> 16),NULL);
    /* 发送 bit15 ~ bit8  地址 */ 
    spiState = flash_spiHardwareSwapByte((uint8_t)(address >> 8),NULL); 
    /* 发送 bit7  ~ bit0  地址 */
    spiState = flash_spiHardwareSwapByte((uint8_t)(address),NULL);    
    if(spiState != HARDWARE_SPI_FINISH)
    {
        printf("Device sendAddress ERROR !\r\n");
        return FLASH_ERROR;
    } 
#endif
    return FLASH_OK;      
}

/**
 * @brief       SPI在一页(0~65535)内写入少于256个字节的数据
 *   @note      在指定地址开始写入最大256字节的数据
 * @param       dataArray           : 数据存储区
 * @param       pageStartAddress    : 开始写入的地址(最大32bit)
 * @param       dataLen             : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @retval      FLASH_State:当前FLASH是否空闲
 */
FLASH_State _W25Q128_onePageWrite(uint32_t pageStartAddress,uint8_t *dataArray,uint8_t dataLen)
{
    FLASH_State flashState = FLASH_OK;
    // 检查 dataLen 是否超过最大允许值 256 
    if (dataLen > W25Q128_PAGE_SIZE) { 
        printf("Error: dataLen exceeds the maximum of 256 bytes.\n"); 
        return FLASH_ERROR; 
    }

    // 计算该页的剩余字节数 
    uint32_t pageOffset = pageStartAddress % W25Q128_PAGE_SIZE; 
    uint32_t remainingBytesInPage = W25Q128_PAGE_SIZE - pageOffset;
    // 检查 dataLen 是否超过该页的剩余字节数 
    if (dataLen > remainingBytesInPage) { 
        printf("Error: dataLen exceeds the remaining bytes in the page.\n"); 
        return FLASH_ERROR; 
    }  

    /*1.打开写使能*/
    flashState = _W25Q128_writeEnable();
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    /*2.片选开始*/
    spi_Start();
    {
        // 1·页编程指令 
        spiState = spiSoftwareSwapByte(FlashPageProgram,NULL);
        // 2·发送页地址
        flashState = sendAddress(pageStartAddress);
        // 3·写入数据
        for(uint16_t len = 0;len<dataLen + 1;len++)
        {
            spiState = spiSoftwareSwapByte(dataArray[len],NULL);
        }
    }
    spi_Stop();
    /*3.等待flash传输完成*/
    flashState = _W25Q128_waitBusy();
    if(spiState != SOFTWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device onePageWrite ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif
#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    /*2.片选开始*/
    FLASH_HARDWARE_SPI_CS(0);
    {
        // 1·页编程指令 
        spiState = flash_spiHardwareSwapByte(FlashPageProgram,NULL);
        // 2·发送页地址
        flashState = sendAddress(pageStartAddress);
        // 3·写入数据
        for(uint16_t len = 0;len<dataLen;len++)
        {
            spiState = flash_spiHardwareSwapByte(dataArray[len],NULL);
        }
    }
    FLASH_HARDWARE_SPI_CS(1);
    /*3.等待flash传输完成*/
    flashState = _W25Q128_waitBusy();
    if(spiState != HARDWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device onePageWrite ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

    return FLASH_OK; 
}
/**
 * @brief       擦除一个扇区
 * @note        注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 * @param       eraseSectorAddr : 扇区地址 根据实际容量设置
 *              saddr = 0 → 物理地址 0x0000 0000
 *              saddr = 1 → 物理地址 0x0000 1000（4KB 处）
 *              saddr = 4095 → 物理地址 0x00FF F000（最后一个扇区）（4096*（4096-1））
 * 
 * @retval      FLASH_State:当前FLASH是否空闲
 */
FLASH_State _W25Q128_eraseSector(uint16_t eraseSectorAddr)
{

    if (eraseSectorAddr >= W25Q128_SECTOR_COUNT)
    { 
        return FLASH_ERROR; 
    }
    // 物理实际地址 saddr<<12    ====  1->0001 0000 0000 0000
    uint32_t eraseAddr = eraseSectorAddr * W25Q128_SECTOR_SIZE;
    FLASH_State flashState = FLASH_OK;

    flashState = _W25Q128_writeEnable();
    flashState = _W25Q128_waitBusy();
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    /*片选开始*/
    spi_Start();
    {
        // 1.扇区擦除指令
        spiState = spiSoftwareSwapByte(FlashSectorErase,NULL);
        // 2·发送物理实际地址
        flashState = sendAddress(eraseAddr);
    }
    spi_Stop();
    /*3.等待flash擦除完成*/
    flashState = _W25Q128_waitBusy();
    if(spiState != SOFTWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device eraseSector ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    /*片选开始*/
    FLASH_HARDWARE_SPI_CS(0);
    {
        // 1.扇区擦除指令
        spiState = flash_spiHardwareSwapByte(FlashSectorErase,NULL);
        // 2·发送物理实际地址
        flashState = sendAddress(eraseAddr);
    }
    FLASH_HARDWARE_SPI_CS(1);
    /*3.等待flash擦除完成*/
    flashState = _W25Q128_waitBusy();
    if(spiState != HARDWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device eraseSector ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

    return FLASH_OK; 
}
/**
 * @brief       擦除一个块区
 * @note        注意,这里是块区地址,不是字节地址!!
 * @param       eraseBlockAddr : 块区地址 根据实际容量设置
 *              baddr = 0 → 物理地址 0x0000 0000
 *              baddr = 1 → 物理地址 0x0001 0000（64KB 处）
 *              baddr = 256 → 物理地址 0x00ff 0000（最后一个块）
 * 
 * @retval      FLASH_State:当前FLASH是否空闲
 */
FLASH_State _W25Q128_eraseBlock(uint16_t eraseBlockAddr)
{
    
    if (eraseBlockAddr >= W25Q128_BLOCK_COUNT)
    { 
        return FLASH_ERROR; 
    }
    // 物理实际地址 saddr<<16 1->0001 0000 0000 0000 0000
    uint32_t blockAddr = eraseBlockAddr * W25Q128_BLOCK_SIZE;
    FLASH_State flashState = FLASH_OK;
    flashState = _W25Q128_writeEnable();
    flashState = _W25Q128_waitBusy();
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    /*片选开始*/
    spi_Start();
    {
        // 1.扇区擦除指令
        spiState = spiSoftwareSwapByte(FlashBlockErase,NULL);
        // 2·发送物理实际地址
        flashState = sendAddress(blockAddr);
    }
    spi_Stop();
    /*3.等待flash擦除完成*/
    flashState = _W25Q128_waitBusy();
    if(spiState != SOFTWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device eraseSector ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;
    /*片选开始*/
    FLASH_HARDWARE_SPI_CS(0);
    {
        // 1.扇区擦除指令
        spiState = flash_spiHardwareSwapByte(FlashBlockErase,NULL);
        // 2·发送物理实际地址
        flashState = sendAddress(blockAddr);
    }
    FLASH_HARDWARE_SPI_CS(1);
    /*3.等待flash擦除完成*/
    flashState = _W25Q128_waitBusy();
    if(spiState != HARDWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device eraseBlock ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

    return FLASH_OK; 
}
/**
 * @brief       擦除整个芯片
 *   @note      等待时间超长...
 * @param       无
 * @retval      无
 */
FLASH_State _W25Q128_eraseChip(void)
{
    FLASH_State flashState = FLASH_OK;
    flashState = _W25Q128_writeEnable();                /* 写使能 */
    flashState =_W25Q128_waitBusy();                    /* 等待空闲 */
#if _SOFTWARE_SPI_ENABLE    
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;
    spi_Start();
    {
        spiState = spiSoftwareSwapByte(FlashChipErase,NULL);  /* 发送读寄存器命令 */
    } 
    spi_Stop();
    flashState = _W25Q128_waitBusy();
    if(spiState != SOFTWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device eraseBlock ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH;  
    FLASH_HARDWARE_SPI_CS(0);
    {
        spiState = flash_spiHardwareSwapByte(FlashChipErase,NULL);  /* 发送读寄存器命令 */
    } 
    FLASH_HARDWARE_SPI_CS(1);
    flashState = _W25Q128_waitBusy();
    if(spiState != HARDWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device eraseBlock ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif
    return FLASH_OK; 
}

/**
 * @brief       读取FLASH 
 * @note        在指定地址开始读取指定长度的数据
 * @param       dataArray           : 数据存储区
 * @param       pageStartAddress    : 开始读取的地址(最大32bit)
 * @param       dataLen             : 要读取的字节数(最大16,777,216)
 *              最大只能16M = 2^24
 * @retval      FLASH_State:当前FLASH是否空闲
 */
FLASH_State _W25Q128_Read(uint32_t StartAddress,uint8_t *dataArray,uint32_t dataLen)
{ 
    FLASH_State flashState = FLASH_OK;
    uint8_t *tempData = dataArray;
    if(dataLen>W25Q128_BLOCK_SIZE*W25Q128_BLOCK_COUNT)
    {
        return FLASH_ERROR; 
    }
#if _SOFTWARE_SPI_ENABLE
    SOFTWARE_SPI_State spiState = SOFTWARE_SPI_FINISH;  
    /*1.片选开始*/
    spi_Start();
    {
        // 1·标准数据读取
        spiState = spiSoftwareSwapByte(FlashReadData,NULL);
        // 2·发送待读取地址
        flashState = sendAddress(StartAddress);
        // 3·读取数据
        for(uint32_t len = 0;len<dataLen;len++)
        {
            spiSoftwareSwapByte(Flash_DUMMY_BYTE,tempData++);
        }
    }
    spi_Stop();
    if(spiState != SOFTWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device ReadLen ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif

#if _HARDWARE_SPI_ENABLE
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH; 
    /*1.片选开始*/
    FLASH_HARDWARE_SPI_CS(0);
    {
        // 1·标准数据读取
        spiState = flash_spiHardwareSwapByte(FlashReadData,NULL);
        // 2·发送待读取地址
        flashState = sendAddress(StartAddress);
        // 3·读取数据
        for(uint32_t len = 0;len<dataLen;len++)
        {
            spiState = flash_spiHardwareSwapByte(Flash_DUMMY_BYTE,tempData++);
        }
    }
    FLASH_HARDWARE_SPI_CS(1);
    if(spiState != HARDWARE_SPI_FINISH || flashState != FLASH_OK)
    {
        printf("Device ReadLen ERROR !\r\n");
        return FLASH_ERROR;
    }
#endif
    return FLASH_OK; 
}
