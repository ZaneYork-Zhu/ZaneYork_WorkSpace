

#include "_w25Qxx_flash_SPI.h"


#if _SOFTWARE_SPI_ENABLE
    #include "_software_SPI.h"
#endif

#if _HARDWARE_SPI_ENABLE
    #include "_hardware_SPI.h"
#endif

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

const uint8_t ArraryWrite[] = {   
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xf8, 0xf8, 0xff, 0xff, 0xff, 0x8f, 0x8f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                            0xff, 0xf8, 0xf8, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 
                            0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0e, 
                            0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xf8, 0xf8, 0x80, 0x80, 0x80, 0x80, 0x80, 
                            0x80, 0xf8, 0xf8, 0xf8, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                            0x06, 0x06, 0x06, 0x3e, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x3f, 0x3f, 0xff, 0xff, 0xff, 
                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};




void _W25Q128_textInit(void)
{
    uint32_t addr = 0x101200; 
    W25Q128_AddrInfo startAddress = W25Q128_AddrConvert(addr);// 非法输入测试 
    uint8_t *ArraryReadbuffer = (uint8_t *)malloc(W25Q128_PAGE_SIZE*(sizeof(uint8_t)));
    if (ArraryReadbuffer == NULL) { 
        // 内存分配失败 
        printf("Memory allocation failed!\n"); 
        return ; 
    } 
    FLASH_State state = FLASH_OK;
    uartInit(USART1,115200);
    _W25Q128_Init();
    _W25Q128_check();
    state = _W25Q128_eraseBlock(0);
    state = _W25Q128_onePageWrite(startAddress.block,startAddress.sector,startAddress.page,startAddress.pageOffset,(uint8_t *)ArraryWrite,W25Q128_PAGE_SIZE);
    state = _W25Q128_Read(startAddress.block,startAddress.sector,startAddress.page,startAddress.pageOffset,ArraryReadbuffer,W25Q128_PAGE_SIZE);
    if(state == FLASH_OK)
    {
        printf("Block: %u\r\n", startAddress.block);  
        printf("Sector: %u\r\n", startAddress.sector);  
        printf("Page: %u\r\n", startAddress.page);  
        printf("Offset: %u\r\n", startAddress.pageOffset);  
 
        for(uint32_t i = 0;i<W25Q128_PAGE_SIZE;i++)
        {
            printf("%#x\t",ArraryReadbuffer[i]);
        }
    }
    else
    {
        printf("text error \r\n");
    }
    free(ArraryReadbuffer);
}

void _W25Q128_Init(void)
{

#if _SOFTWARE_SPI_ENABLE
    /*初始化software SPI*/
    _software_SPI_GPIO_Init(MODE0);
#endif

#if _HARDWARE_SPI_ENABLE
    /*初始化hardware SPI*/
    FLASH_hardware_SPI_GPIO_Init(0,FLASH_HARDWARE_SPI,SPI_BAUDRATEPRESCALER_2);

#endif
}
/** 
 * @brief  W25Q128地址转换函数 
 * @param  addr: 32位物理地址(0x000000 - 0xFFFFFF) 
 * @retval 转换后的地址信息结构体 
 *         如果输入地址超出24位地址范围(0 - 0xFFFFFF)，返回全零的结构体 
 */ 
W25Q128_AddrInfo W25Q128_AddrConvert(uint32_t addr) { 
    W25Q128_AddrInfo info = {0}; // 初始化地址信息结构体为全零 
 
    // 检查输入地址是否超出24位地址范围 
    if (addr > 0xFFFFFF) { 
        printf("The input address is outside the address range.\r\n");
        // 若超出范围，直接返回全零的结构体 
        return info; 
    } 
 
    // 计算块号，使用宏定义的块大小 
    // 块号等于地址除以块大小 
    info.block  = addr / W25Q128_BLOCK_SIZE; 
    // 计算块内地址，即地址对块大小取余 
    uint32_t blockAddr = addr % W25Q128_BLOCK_SIZE; 
 
    // 计算扇区号，使用宏定义的扇区大小 
    // 扇区号等于块内地址除以扇区大小 
    info.sector  = blockAddr / W25Q128_SECTOR_SIZE; 
    // 计算扇区内地址，即块内地址对扇区大小取余 
    uint32_t sectorAddr = blockAddr % W25Q128_SECTOR_SIZE; 
 
    // 计算页号，使用宏定义的页大小 
    // 页号等于扇区内地址除以页大小 
    info.page  = sectorAddr / W25Q128_PAGE_SIZE; 
 
    // 计算页内偏移，即扇区内地址对页大小取余 
    info.pageOffset  = sectorAddr % W25Q128_PAGE_SIZE; 
 
    return info; 
} 

/**
  * @brief  将块/扇区/页/偏移转换为24位物理地址 
  * @param  block: 块号 (0-255)
  * @param  sector: 块内扇区号 (0-15)
  * @param  page: 扇区内页号 (0-15)
  * @param  offset: 页内偏移 (0-255)
  * @retval 24位物理地址 (0x000000-0xFFFFFF)，输入非法时返回0xFFFFFF
  * @note   使用宏定义校验层级结构限制 [2]()[3]()
  */
uint32_t W25Q128_AddrReverseConvert(uint16_t block, uint8_t sector, uint8_t page, uint16_t pageStartOffset)
{
    // 参数有效性校验 [3]()[6]()
    if (block >= W25Q128_BLOCK_COUNT || 
        sector >= W25Q128_SECTORS_PER_BLOCK ||
        page >= W25Q128_PAGES_PER_SECTOR || 
        pageStartOffset >= W25Q128_PAGE_SIZE) 
    {
        printf("Return 0xFFFFFFFF when the input is illegal.\r\n");
        return 0xFFFFFFFF; // 错误标志 
    }
 
    /* 层级结构地址计算 [2]()[10]() */
    // 块地址 = 块号 × 块大小 (64KB)
    uint32_t blockAddr = block * W25Q128_BLOCK_SIZE;
    
    // 扇区地址 = 扇区号 × 扇区大小 (4KB)
    uint32_t sectorAddr = sector * W25Q128_SECTOR_SIZE;
    
    // 页地址 = 页号 × 页大小 (256B)
    uint32_t pageAddr = page * W25Q128_PAGE_SIZE;
 
    // 总地址合成（确保不超过24位地址空间）
    uint32_t physicalAddr = blockAddr + sectorAddr + pageAddr + pageStartOffset;
    
    return (physicalAddr <= 0xFFFFFF) ? physicalAddr : 0xFFFFFFFF;
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
        printf("JEDEC_ID gNorFlashDId is :\r\n");
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
    const uint8_t ReadIDcommand[] = {
        /*从 FLASH 芯片中发送读取ID号【指令】*/
        FlashJedecDeviceId,
        /*从 FLASH 芯片中接收<厂商>ID号【数据】*/
        Flash_DUMMY_BYTE,
        /*从 FLASH 芯片中接收内存类型*/
        Flash_DUMMY_BYTE,
        /*从 FLASH 芯片中接收容量标识*/ 
        Flash_DUMMY_BYTE
    };
    uint8_t receiverbuffer[4] = {0};
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
        for(uint8_t i = 0;i<(sizeof(ReadIDcommand)/sizeof(ReadIDcommand[0]));i++){
            // 调用硬件SPI交换字节的函数 
            spiState = flash_spiHardwareSwapByte(&ReadIDcommand[i],&receiverbuffer[i],1);
            //printf("spiState = %d \r\n", (uint8_t) spiState);
            if(spiState != HARDWARE_SPI_FINISH)
            {   
                /*恢复SPI空闲*/
                FLASH_HARDWARE_SPI_CS(1);
                printf("Device JEDEC_ID receive ERROR !\r\n");
                return FLASH_ERROR;
            }   
        }
        driverId->manuFacturerId = receiverbuffer[1];
        driverId->memoryType = receiverbuffer[2];
        driverId->capacityId = receiverbuffer[3];
    }
    /*恢复SPI空闲*/
    FLASH_HARDWARE_SPI_CS(1);

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
    uint8_t command = FlashWriteEnable;
    uint8_t dummy = 0;
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
        spiState = flash_spiHardwareSwapByte(&command,&dummy,1);
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
    uint8_t dummy = 0;
    uint32_t timeOut = WAIT_BUSY_TIME;
    uint8_t waitBusyData = {0};
    uint8_t commandbuffer[] = { FlashReadStatusReg1,Flash_DUMMY_BYTE};
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
        spiState = flash_spiHardwareSwapByte(commandbuffer,&dummy,1);
        // 获取当前BUSY的状态
        // 循环检查BUSY位，直到空闲或超时
        do{
            // 再次发送DUMMY字节读取最新状态
            spiState = flash_spiHardwareSwapByte(commandbuffer+1,&waitBusyData,1);
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
    uint8_t dummy = 0;
    const uint8_t addrbuffer[] ={ \
        /* 发送 bit23 ~ bit16 地址 */
        (uint8_t)(address>>16), \
        /* 发送 bit15 ~ bit8  地址 */ 
        (uint8_t)(address>>8), \
        /* 发送 bit7  ~ bit0  地址 */
        (uint8_t)(address)} ;
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
    for(uint8_t i = 0;i<(sizeof(addrbuffer)/sizeof(addrbuffer[0]));i++){

        spiState = flash_spiHardwareSwapByte(&addrbuffer[i],&dummy,1);
        //printf("addrbuffer = %x\r\n",addrbuffer[i]);
        if(spiState != HARDWARE_SPI_FINISH)
        {
            printf("Device sendAddress ERROR !\r\n");
            return FLASH_ERROR;
        } 
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
    uint8_t dummy = 0;
    if (eraseSectorAddr >= W25Q128_SECTOR_SIZE)
    { 
        return FLASH_ERROR; 
    }
    // 物理实际地址 saddr<<12    ====  1->0001 0000 0000 0000
    uint32_t eraseAddr = eraseSectorAddr * W25Q128_SECTOR_SIZE;
    FLASH_State flashState = FLASH_OK;
    uint8_t command = FlashSectorErase;

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
        spiState = flash_spiHardwareSwapByte(&command,&dummy,1);
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
    uint8_t dummy = 0;
    if (eraseBlockAddr >= W25Q128_BLOCK_COUNT)
    { 
        return FLASH_ERROR; 
    }
    // 物理实际地址 saddr<<16 1->0001 0000 0000 0000 0000
    uint32_t blockAddr = eraseBlockAddr * W25Q128_BLOCK_SIZE;
    uint8_t command = FlashBlockErase;
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
        spiState = flash_spiHardwareSwapByte(&command,&dummy,1);
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
    uint8_t dummy = 0;
    FLASH_State flashState = FLASH_OK;
    flashState = _W25Q128_writeEnable();                /* 写使能 */
    flashState =_W25Q128_waitBusy();                    /* 等待空闲 */
    uint8_t command = FlashChipErase;
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
        spiState = flash_spiHardwareSwapByte(&command,&dummy,1);  /* 发送读寄存器命令 */
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
 * @brief SPI在一页(0~65535)内写入少于256个字节的数据 
 * @note 在指定地址开始写入最大256字节的数据 
 * @param block             : 要写入的块号 
 * @param sector            : 要写入的扇区号 
 * @param page              : 要写入的页号 
 * @param pageStartOffset   : 页内开始写的位置（页偏移） 
 * @param sendDataArray     : 数据存储区 
 * @param sendDataLen       : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!! 
 * @retval FLASH_State:当前FLASH是否空闲 
 */
FLASH_State _W25Q128_onePageWrite(uint32_t block, uint32_t sector, uint32_t page, uint32_t pageStartOffset, uint8_t *sendDataArray, uint16_t sendDataLen)
{
    // 初始化FLASH状态为OK 
    FLASH_State flashState = FLASH_OK; 
    // 定义页编程指令 
    uint8_t command = FlashPageProgram; 
    // 定义一个占位字节 
    uint8_t dummy  = 0; 
    // 分配发送缓冲区内存 
    uint8_t *sendbuffer = (uint8_t *)malloc(sendDataLen * sizeof(uint8_t)); 
    // 分配接收缓冲区内存 
    uint8_t *receivebuffer = (uint8_t *)malloc(sendDataLen * sizeof(uint8_t)); 
 
    // 提前检查内存分配是否成功 
    if (sendbuffer == NULL || receivebuffer == NULL) { 
        printf("Memory allocation failed.\r\n"); 
        return FLASH_ERROR; 
    } 
 
    // 计算起始地址 
    uint32_t sendPageStartAddress = W25Q128_AddrReverseConvert(block,sector,page,pageStartOffset); 
 
    // 参数校验 
    if (block >= W25Q128_BLOCK_COUNT || sector >= W25Q128_SECTORS_PER_BLOCK || page >= W25Q128_PAGES_PER_SECTOR || pageStartOffset >= W25Q128_PAGE_SIZE) { 
        printf("Error: Address parameter out of range!\r\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        return FLASH_ERROR; 
    } 
 
    // 检查sendDataLen是否超过最大允许值256 
    if (sendDataLen > W25Q128_PAGE_SIZE) { 
        printf("Error: dataLen exceeds the maximum of 256 bytes.\r\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        return FLASH_ERROR; 
    } 

    // 将数据从 dataArray 复制到 sendbuffer 
    memcpy(sendbuffer, sendDataArray, sendDataLen); 
    // 计算该页的剩余字节数 
    uint32_t pageOffset = sendPageStartAddress % W25Q128_PAGE_SIZE; 
    uint32_t remainingBytesInPage = W25Q128_PAGE_SIZE - pageOffset;
    // 检查 dataLen 是否超过该页的剩余字节数 
    if (sendDataLen > remainingBytesInPage) { 
        printf("Error: dataLen exceeds the remaining bytes in the page.\n"); 
        free(sendbuffer); // 释放已分配的内存 
        free(receivebuffer);
        return FLASH_ERROR; 
    }  
    /*1.打开写使能*/
    flashState = _W25Q128_writeEnable();
    if (flashState != FLASH_OK) { 
        printf("Failed to enable write.\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        return FLASH_ERROR; 
    } 
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
     // 初始化SPI状态为完成 
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH; 
 
    // 2. 片选开始 
    FLASH_HARDWARE_SPI_CS(0); 
 
    // 发送页编程指令 
    spiState = flash_spiHardwareSwapByte(&command, &dummy, 1); 
    if (spiState != HARDWARE_SPI_FINISH) { 
        printf("Failed to send page program command.\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        FLASH_HARDWARE_SPI_CS(1); 
        return FLASH_ERROR; 
    } 
 
    // 发送页地址 
    flashState = sendAddress(sendPageStartAddress); 
    if (flashState != FLASH_OK) { 
        printf("Failed to send page address.\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        FLASH_HARDWARE_SPI_CS(1); 
        return FLASH_ERROR; 
    } 
 
    // 写入数据 
    spiState = flash_spiHardwareSwapByte(sendbuffer, receivebuffer, sendDataLen); 
    if (spiState != HARDWARE_SPI_FINISH) { 
        printf("Failed to write data.\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        FLASH_HARDWARE_SPI_CS(1); 
        return FLASH_ERROR; 
    } 
 
    // 片选结束 
    FLASH_HARDWARE_SPI_CS(1); 
 
    // 3. 等待flash传输完成 
    flashState = _W25Q128_waitBusy(); 
    if (spiState != HARDWARE_SPI_FINISH || flashState != FLASH_OK) { 
        printf("Device onePageWrite ERROR !\r\n"); 
        // 释放已分配的内存 
        free(sendbuffer); 
        free(receivebuffer); 
        return FLASH_ERROR; 
    } 
#endif 
 
    // 释放发送和接收缓冲区内存 
    free(sendbuffer); 
    free(receivebuffer); 
 
    return FLASH_OK; 
} 
/** 
 * @brief       从 W25Q128 FLASH 中读取数据 
 * @note        此函数用于在指定的块、扇区、页和页内偏移地址开始读取指定长度的数据。 
 * @param       block             : 起始块编号 
 * @param       sector            : 起始扇区编号 
 * @param       page              : 起始页编号 
 * @param       pageStartOffset   : 页内起始偏移量 
 * @param       readDataArray     : 用于存储读取数据的缓冲区 
 * @param       readDataLen       : 要读取的字节数，最大为 16,777,216 (16M = 2^24) 
 * @retval      FLASH_State: 当前 FLASH 的状态 
 */
FLASH_State _W25Q128_Read(uint32_t block, uint32_t sector, uint32_t page, uint32_t pageStartOffset, uint8_t *readDataArray, uint32_t readDataLen)
{ 
    // 定义一个临时变量用于存储无用字节 
    uint8_t dummy = 0; 
    // 初始化 FLASH 状态为正常 
    FLASH_State flashState = FLASH_OK; 
    // 定义读取数据的命令数组，包含读取命令和一个虚拟字节 
    uint8_t command[2] = {FlashReadData, Flash_DUMMY_BYTE}; 
    // 为读取的数据分配内存 
    uint8_t *readDataBuffer = (uint8_t *)malloc(readDataLen * sizeof(uint8_t)); 
 
    // 检查分配的内存是否成功，并且读取长度是否超过最大允许值 
    if (readDataLen > W25Q128_BLOCK_SIZE * W25Q128_BLOCK_COUNT || readDataBuffer == NULL) { 
        // 若不满足条件，返回错误状态 
        return FLASH_ERROR; 
    } 
    // 计算起始读取地址 
    uint32_t readPageStartAddress = W25Q128_AddrReverseConvert(block,sector,page,pageStartOffset); 
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
// 初始化硬件 SPI 状态为完成 
    HARDWARE_SPI_State spiState = HARDWARE_SPI_FINISH; 
 
    // 1. 使能片选信号，开始通信 
    FLASH_HARDWARE_SPI_CS(0); 
 
    // 2. 执行数据读取操作 
    // 2.1 发送标准读取数据命令 
    spiState = flash_spiHardwareSwapByte(&command[0], &dummy, 1); 
    if (spiState != HARDWARE_SPI_FINISH) 
    { 
        // 若发送命令失败，恢复片选信号，输出错误信息并返回错误状态 
        FLASH_HARDWARE_SPI_CS(1); 
        printf("Failed to send read command!\r\n"); 
        free(readDataBuffer); 
        return FLASH_ERROR; 
    } 
 
    // 2.2 发送待读取数据的起始地址 
    flashState = sendAddress(readPageStartAddress); 
    if (flashState != FLASH_OK) 
    { 
        // 若发送地址失败，恢复片选信号，输出错误信息并返回错误状态 
        FLASH_HARDWARE_SPI_CS(1); 
        printf("Failed to send read address!\r\n"); 
        free(readDataBuffer); 
        return FLASH_ERROR; 
    } 
 
    // 2.3 读取指定长度的数据 
    spiState = flash_spiHardwareSwapByte(&command[1], readDataBuffer, readDataLen); 
    if (spiState != HARDWARE_SPI_FINISH) 
    { 
        // 若读取数据失败，恢复片选信号，输出错误信息并返回错误状态 
        FLASH_HARDWARE_SPI_CS(1); 
        printf("Failed to read data!\r\n"); 
        free(readDataBuffer); 
        return FLASH_ERROR; 
    } 
 
    // 3. 禁用片选信号，结束通信 
    FLASH_HARDWARE_SPI_CS(1); 
 
    // 4. 将读取的数据从临时缓冲区复制到目标缓冲区 
    memcpy(readDataArray, readDataBuffer, readDataLen); 
 
    // 5. 释放临时缓冲区的内存 
    free(readDataBuffer); 
#endif 
 
    // 返回正常状态 
    return FLASH_OK; 
} 
