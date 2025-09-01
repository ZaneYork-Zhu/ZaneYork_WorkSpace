#ifndef   __24Cxx_H_
#define   __24Cxx_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include "main.h"

#define hi2c1               gI2C_HandleTypeStruct
#define EEPROM_TEXT_DATA    0x55 

#define IIC_SOFTWARE_ENABLE                     (1)
#define IIC_HARDWARE_ENABLE                     (0)

typedef enum {
    OP_SUCCESS = 1,  // 操作成功 
    OP_FAILURE = 0   // 操作失败 
} OperationResult;

/* 硬件引脚配置（根据实际接线修改） */
typedef struct {
    uint8_t a2_pin : 1;  // A2引脚电平（0/1）
    uint8_t a1_pin : 1;  // A1引脚电平
    uint8_t a0_pin : 1;  // A0引脚电平
} HardwarePins;


/* 型号枚举（明确支持的所有设备类型） */
typedef enum
{
    EEPROM_AT24C01  = 1,
    EEPROM_AT24C02,  
    EEPROM_AT24C04,
    EEPROM_AT24C08,
    EEPROM_AT24C16,
    EEPROM_AT24C32,
    EEPROM_AT24C64,
    EEPROM_AT24C128,
    EEPROM_AT24C256,
}EEPROM_TYPE;


/* 型号枚举（明确支持的所有设备类型） */
typedef enum
{
    EEPROM_AT24C01_endAddress  = (0x80 - 1),
    EEPROM_AT24C02_endAddress  = (0x100 - 1),  
    EEPROM_AT24C04_endAddress  = (0x200 - 1),
    EEPROM_AT24C08_endAddress  = (0x400 - 1),
    EEPROM_AT24C16_endAddress  = (0x800 - 1),
    EEPROM_AT24C32_endAddress  = (0x1000 - 1),
    EEPROM_AT24C64_endAddress  = (0x2000 - 1),
    EEPROM_AT24C128_endAddress = (0x4000 - 1),
    EEPROM_AT24C256_endAddress = (0x8000 - 1)
}EEPROM_TYPE_ENDADRRESS;


/* 地址生成规则结构体 */
typedef struct {

    uint8_t base_addr;    // 基础设备地址（如0xA0）

    /*存储容量为1Kbit（即128字节），2^7=128*/
    uint8_t addr_bits;    // 内存地址寻址总位数（如AT24C01=7位）

    uint8_t page_size;     // 分页大小（如AT24C01=8字节）
    uint8_t use_split_addr;  // 是否需要分两次发送内存地址（AT24C32及以上需true）
} EEPROM_Config;


typedef enum {
    AT24C02_WRITE = 0xA0,  // 写操作地址（7位地址左移1位 + 写方向位0）
    AT24C02_READ  = 0xA1   // 读操作地址（7位地址左移1位 + 读方向位1）
} I2C_DEVICEADDRES;

void EEPROM_text_init(void);
void EEPROM_text_while(void);

/*初始化EEPROM的GPIO*/
void EEPROM_init(void);

/*生成设备地址（含硬件引脚和内存地址处理）*/
static uint8_t generate_device_address(uint16_t addr, uint8_t rw_mode);
/*读取1字节数据*/
uint8_t EEPROM_ReadSingleByte(const uint16_t readaddr);
/*读取指定字节数据*/
OperationResult EEPROM_ReadBuffer(uint16_t start_addr,uint8_t *buffer, uint16_t length);
/*写入1字节数据*/
OperationResult EEPROM_WriteSingleByte(const uint16_t write_addr, const uint8_t write_data);
/*写入多字节数据*/
OperationResult EEPROM_WriteBuffer(uint16_t start_addr,uint8_t *write_buffer, uint16_t length);

OperationResult EEPROM_check(void);

#ifdef __cplusplus
}
#endif


#endif
