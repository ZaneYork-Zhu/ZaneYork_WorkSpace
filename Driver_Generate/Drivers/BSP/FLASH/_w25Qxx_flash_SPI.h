#ifndef _FLASH_SPI_H_
#define _FLASH_SPI_H_

#include "main.h"


#define _HARDWARE_SPI_ENABLE            (0)
#define _SOFTWARE_SPI_ENABLE            (1)


typedef struct 
{
    uint8_t manuFacturerId;   // 制造商ID（如0xEF对应Winbond）
    uint8_t memoryType;       // 内存类型（如0x40表示SPI NOR Flash）
    uint8_t capacityId;       // 容量标识（如0x18对应128Mbit）

}__attribute__((packed)) JEDEC_ID;  // 使用紧凑对齐避免填充字节 ;


/* FLASH芯片列表 */
#define WINBOND_MANU   0xEF  // 华邦（Winbond）制造商ID 
#define BY_MANU        0x68  // BY厂商ID 
#define NM_MANU        0x52  // NM厂商ID 
 
// 假设所有芯片均为SPI NOR Flash（内存类型0x40）
#define SPI_NOR_FLASH_TYPE  0x40 
/*容量大小*/
#define CAPACITY_8Mbit    0x14  // 8Mbit 
#define CAPACITY_16Mbit   0x15  // 16Mbit 
#define CAPACITY_32Mbit   0x16  // 32Mbit 
#define CAPACITY_64Mbit   0x17  // 64Mbit 
#define CAPACITY_128Mbit  0x18  // 128Mbit 
#define CAPACITY_256Mbit  0x19  // 256Mbit 



 
/* 指令表 */
#define FlashWriteEnable           0x06    // 使能写入操作 
#define FlashWriteDisable          0x04    // 禁用写入操作 
#define FlashReadStatusReg1        0x05    // 读取状态寄存器1 
#define FlashReadStatusReg2        0x35    // 读取状态寄存器2 
#define FlashReadStatusReg3        0x15    // 读取状态寄存器3 
#define FlashWriteStatusReg1       0x01    // 写入状态寄存器1 
#define FlashWriteStatusReg2       0x31    // 写入状态寄存器2 
#define FlashWriteStatusReg3       0x11    // 写入状态寄存器3 
#define FlashReadData              0x03    // 标准数据读取 
#define FlashFastReadData          0x0B    // 快速数据读取 
#define FlashFastReadDual          0x3B    // 双输出快速读取 
#define FlashFastReadQuad          0xEB    // 四输出快速读取 
#define FlashPageProgram           0x02    // 页编程指令 
#define FlashPageProgramQuad       0x32    // 四线页编程 
#define FlashBlockErase            0xD8    // 块擦除指令 
#define FlashSectorErase           0x20    // 扇区擦除指令 
#define FlashChipErase             0xC7    // 全片擦除指令 
#define FlashPowerDown             0xB9    // 进入掉电模式 
#define FlashReleasePowerDown      0xAB    // 退出掉电模式 
#define FlashDeviceId              0xAB    // 获取设备ID 
#define FlashManufactDeviceId      0x90    // 获取制造商ID 
/*发送 0x9F 后，FLASH 会返回 3 字节数据*/
/**
 *  第1字节：制造商 ID（如华邦 Winbond 为 0xEF，兆易创新 GD 为 0xC8）。
 *  第2字节：设备类型（如电压、接口类型等）。
 *  第3字节：容量代码（如 0x17 代表 128M-bit）
 */
#define FlashJedecDeviceId         0x9F    // 获取JEDEC ID 
#define FlashEnable4ByteAddr       0xB7    // 启用4字节地址 
#define FlashExit4ByteAddr         0xE9    // 退出4字节地址 
#define FlashSetReadParam          0xC0    // 设置读取参数 
#define FlashEnterQPIMode          0x38    // 进入QPI模式 
#define FlashExitQPIMode           0xFF    // 退出QPI模式 

#define Flash_DUMMY_BYTE           0xFF


// 物理存储单元大小 
#define W25Q128_PAGE_SIZE       256    // 256B (页大小)
#define W25Q128_SECTOR_SIZE     4096   // 4KB  (扇区大小)
#define W25Q128_BLOCK_SIZE      65536  // 64KB (块大小)
 
// 存储单元总数 
#define W25Q128_PAGE_COUNT      65536  // 总页数 (16MB / 256B)
#define W25Q128_SECTOR_COUNT    4096   // 总扇区数 (16MB / 4KB)
#define W25Q128_BLOCK_COUNT     256    // 总块数 (16MB / 64KB)

#define WAIT_BUSY_TIME          1000000

/*flash状态寄存器的状态*/
typedef enum{
    FLASH_OK = 0,
    FLASH_BUSY,
    FLASH_ERROR
}FLASH_State;

void _W25Q128_Init(void);
FLASH_State _W25Q128_ReadID(JEDEC_ID *driverId);
static FLASH_State _W25Q128_writeEnable(void);
static FLASH_State _W25Q128_waitBusy(void);
static FLASH_State sendAddress(uint32_t address);

FLASH_State _W25Q128_onePageWrite(uint32_t pageStartAddress,uint8_t *dataArray,uint8_t dataLen);
FLASH_State _W25Q128_eraseSector(uint16_t saddr);
FLASH_State _W25Q128_eraseBlock(uint16_t eraseBlockAddr);
FLASH_State _W25Q128_eraseChip(void);
FLASH_State _W25Q128_Read(uint32_t StartAddress,uint8_t *dataArray,uint32_t dataLen);

void _W25Q128_textInit(void);
void _W25Q128_check(void);

#endif
