
#include "_24Cxx_iic_driver.h"
#include "key.h"

#if IIC_HARDWARE_ENABLE
    #include "_hardware_IIC.h"
#endif

#if IIC_SOFTWARE_ENABLE
    #include "_software_IIC.h"
#endif


/************************************************ 全局配置（初始化时设置）************************* */
EEPROM_TYPE current_type = EEPROM_AT24C02;          // 当前使用的型号
HardwarePins hw_pins = {0, 0, 0};            // 假设A2/A1/A0接地
EEPROM_TYPE_ENDADRRESS MAX_ENDADRRESS = EEPROM_AT24C02_endAddress;          //EEPROM尾地址
/*****************************************操作EEPROM的一些测试函数****************/
#define TEXT_SIZE sizeof(eeprom_full_data)
static uint8_t eeprom_full_data[255] = {
    0xA3, 0x7E, 0x19, 0xF0, 0x42, 0x8C, 0xD5, 0x6B, 0x31, 0x9A, 0x24, 0xCE, 0xB7, 0x58, 0x0D, 0xE2,
    0x5F, 0x88, 0x13, 0x76, 0xAA, 0x34, 0xC9, 0x1B, 0x90, 0x67, 0x2D, 0xFB, 0x44, 0xE7, 0x52, 0x9D,
    0x28, 0x03, 0xBF, 0x6E, 0xD1, 0x7A, 0x85, 0x4C, 0x39, 0xE0, 0x15, 0x92, 0xFE, 0x63, 0x87, 0x2A,
    0xBD, 0x70, 0x0A, 0xE9, 0x54, 0x96, 0x3F, 0xC2, 0x79, 0x1D, 0xA8, 0x43, 0xDE, 0x21, 0xB6, 0x5C,
    0x8F, 0x32, 0xCD, 0x68, 0x17, 0xE4, 0x9B, 0x26, 0x50, 0xAF, 0x3C, 0x81, 0x6D, 0xBA, 0x07, 0x94,
    0xEB, 0x45, 0xD8, 0x72, 0x0F, 0x9C, 0x61, 0xBE, 0x35, 0xCA, 0x83, 0x1E, 0xF9, 0x4D, 0xA0, 0x57,
    0x8A, 0x27, 0xDC, 0x71, 0xEE, 0x3B, 0x84, 0x59, 0xC6, 0x12, 0xAD, 0x48, 0x95, 0x6A, 0xF7, 0x0C,
    0xB3, 0x5E, 0xE1, 0x2C, 0x97, 0x4A, 0x85, 0x78, 0xD3, 0x6F, 0x1A, 0xA5, 0x30, 0xCF, 0x62, 0xFD,
    0x16, 0x89, 0x74, 0xDB, 0x20, 0xBD, 0x46, 0x9B, 0x64, 0x29, 0xF2, 0x5D, 0x80, 0x37, 0xEA, 0x15,
    0x8E, 0x73, 0xCC, 0x41, 0xAE, 0x13, 0x68, 0xD5, 0x7A, 0x87, 0x3C, 0xE9, 0x52, 0x9F, 0x04, 0xB9,
    0x66, 0x2B, 0xD4, 0x79, 0x82, 0x1F, 0xEC, 0x51, 0xBE, 0x05, 0x98, 0x63, 0x3E, 0xC5, 0x18, 0xAB,
    0x74, 0xD9, 0x22, 0x8F, 0x5A, 0xA7, 0x0C, 0xF1, 0x4E, 0x93, 0x68, 0x35, 0xCA, 0x17, 0xBC, 0x61,
    0x8E, 0x23, 0xFC, 0x49, 0x96, 0x5D, 0xA2, 0x0F, 0xB8, 0x45, 0xDA, 0x77, 0x2C, 0x91, 0x6E, 0xE3,
    0x18, 0x85, 0x7A, 0xC7, 0x32, 0xAF, 0x44, 0x99, 0x66, 0x3B, 0xD0, 0x6D, 0x82, 0x1F, 0xE8, 0x55,
    0xAA, 0x37, 0xCC, 0x71, 0x8E, 0x23, 0xFC, 0x41, 0x9E, 0x53, 0xE0, 0x1D, 0xB6, 0x6B, 0x84, 0x79,
    0xC2, 0x5F, 0xA8, 0x15, 0xEA, 0x47, 0x9C, 0x61, 0x3E, 0xD3, 0x68, 0xB5, 0x0A, 0xFF, 0xFF};
uint8_t datatemp[TEXT_SIZE] = {0};
uint8_t key_EEPROM = 0;

void EEPROM_text_init(void)
{
    EEPROM_init();
    uartInit(USART1,115200);
    /* USER CODE BEGIN 2 */
    printf("EEPROM IIC Driver Test\r\n");
    key_init();
    EEPROM_check();
}
void EEPROM_text_while(void)
{
    while (1)
    {
        key_EEPROM = key_scan(0);
        if (key_EEPROM == KEY1_PRES)
        {
            if(1 == EEPROM_WriteBuffer(0x00,eeprom_full_data,TEXT_SIZE))
            {
                for(int t = 0;t<TEXT_SIZE;t++)
                {
                    printf("0x%x ",eeprom_full_data[t]);
                }
            printf("\r\n\r\n");
            }
        }

        if (key_EEPROM == KEY0_PRES) 
        {
            if(1 == EEPROM_ReadBuffer(0x00,datatemp,TEXT_SIZE))
            {
                for(int t = 0;t<TEXT_SIZE;t++)
                {
                    printf("0x%x ",datatemp[t]);
                }
            }
        }
    }
}
/*****************************************操作EEPROM的一些基本函数****************/
/**
 * @brief       初始化IIC接口
 * @param       无
 * @retval      无
 */
void EEPROM_init(void)
{
    #if IIC_HARDWARE_ENABLE
        I2C_Hardware_Init(I2C1,400000);
    #endif

    #if IIC_SOFTWARE_ENABLE
        I2C_Software_Init();
    #endif
}


/**
 * @brief 获取当前型号的配置参数
 * @param type EEPORM型号
 * 
 */
EEPROM_Config get_eeprom_config(EEPROM_TYPE type)
{
    static const EEPROM_Config config_table[] = {
        /*存储容量为1Kbit（即128字节），2^7=128*/
        /*设备基地址，内存寻址地址位数，页大小，是否需要发送2字节*/
        [EEPROM_AT24C01]  = {AT24C02_WRITE,  7,  8,  0},
        [EEPROM_AT24C02]  = {AT24C02_WRITE,  8,  8,  0},
        [EEPROM_AT24C04]  = {AT24C02_WRITE,  9,  16, 0},
        [EEPROM_AT24C08]  = {AT24C02_WRITE, 10,  16, 0},
        [EEPROM_AT24C16]  = {AT24C02_WRITE, 11,  16, 0},
        [EEPROM_AT24C32]  = {AT24C02_WRITE, 12,  32, 1},
        [EEPROM_AT24C64]  = {AT24C02_WRITE, 13,  64, 1}
    };
    return config_table[type];
}



/**
 * @brief 生成设备地址（含硬件引脚和内存地址处理）  发送的设备地址<<0xA0|rw_mode>>
 * 
 * 对于24C01/02, 其器件地址格式(8bit)为: 1 0 1 0 A2 A1 A0 R/W
 * 对于24C04, 其器件地址格式(8bit)为: 1 0 1 0 A2 A1 a8 R/W
 * 对于24C08, 其器件地址格式(8bit)为: 1 0 1 0 A2 a9 a8 R/W
 * 对于24C16, 其器件地址格式(8bit)为: 1 0 1 0 a10 a9 a8 R/W
 * R/W : 读/写控制位 0,表示写; 1,表示读;
 * A0/A1/A2 : 对应器件的1,2,3引脚(只有24C01/02/04/8有这些脚)
 * a8/a9/a10: 对应存储整列的高位地址, 11bit地址最多可以表示2048个位置, 可以寻址24C16及以内的型号
 * @param addr 目标内存地址
 * @param rw_mode 读/写模式（0=写，1=读）
 * @retval 设备地址
 */
static uint8_t generate_device_address(uint16_t addr, uint8_t rw_mode) 
{
    /*获取型号配置*/
    const EEPROM_Config config = get_eeprom_config(current_type);
    /*获取设备地址*/
    uint8_t device_addr = config.base_addr; 

    // 根据型号嵌入地址位或硬件引脚
    switch ((uint32_t)current_type)
    {

        case EEPROM_AT24C01:
        case EEPROM_AT24C02:
            {
                // 分步计算每个引脚对应的位域 
                uint8_t a2_bits = (hw_pins.a2_pin & 0x01) << 3;
                uint8_t a1_bits = (hw_pins.a1_pin & 0x01) << 2;
                uint8_t a0_bits = (hw_pins.a0_pin & 0x01) << 1;
                
                device_addr |= a2_bits | a1_bits | a0_bits;
            }
            break;
            
        case EEPROM_AT24C04:    
            {
                // 硬件引脚配置位（A2/A1）
                uint8_t a2_bits = (hw_pins.a2_pin & 0x01) << 3;
                uint8_t a1_bits = (hw_pins.a1_pin & 0x01) << 2;
                // 内存地址第8位（替代A0引脚功能）
                uint8_t A8_bits = ((addr >> 8) & 0x01) << 1;

                device_addr |= a2_bits | a1_bits | A8_bits;

            }
            break;
        case EEPROM_AT24C08:     
            {
                uint8_t a2_bits = (hw_pins.a2_pin & 0x01) << 3;
                uint8_t A9_bits = ((addr >> 9) & 0x01) << 2;
                // 从目标内存地址中提取第8位 
                uint8_t A8_bits = ((addr >> 8) & 0x01) << 1;

                device_addr |= a2_bits | A9_bits | A8_bits;
            }
            break;

        case EEPROM_AT24C16: 
            {
                uint8_t A10_bits =((addr >> 10) & 0x01) << 3;
                uint8_t A9_bits = ((addr >> 9) & 0x01) << 2;
                // 从目标内存地址中提取第8位 
                uint8_t A8_bits = ((addr >> 8) & 0x01) << 1;

                device_addr |= A10_bits | A9_bits | A8_bits;
            }
            break;
    }
    return (device_addr|rw_mode);

}

/**
 * @brief       在24CXX指定地址读出一个数据
 * @param       readaddr: 待读取的地址位置
 * @retval      读到的数据
 */
uint8_t EEPROM_ReadSingleByte(const uint16_t readaddr)
{
    /*获取待读取的地址*/
    uint16_t _24cxx_readaddr = readaddr;
    /*获取型号配置*/
    const EEPROM_Config config = get_eeprom_config(current_type);
    /*获取设备地址长度 MemAddSize*/
    uint8_t device_addr_bit = config.addr_bits; 
    volatile int8_t device_MemAddSize;
    /*获取读取是否成功*/
    volatile HAL_StatusTypeDef Operation_state = HAL_OK;
     /*负责返回读取数据*/
    uint8_t _24cxx_receive_byte = {0};
#if IIC_SOFTWARE_ENABLE
    /*监控ACK应答情况*/
    IIC_State IIC_read_state = IIC_ACK;
#endif
    if(device_addr_bit < 16)
    {
        device_MemAddSize = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
        device_MemAddSize = I2C_MEMADD_SIZE_16BIT;
    }
    /*软件IIC实现*/
    #if IIC_SOFTWARE_ENABLE
    {
        /*主机写方向-------------主机写设备地址+内存地址*/
        IIC_start();
        {
            /*写设备地址*/
            IIC_send_byte(generate_device_address(_24cxx_readaddr,0));

            /*等待应答信号*/
            IIC_read_state = IIC_receive_ACK();
            //printf("current readACK[0] = %d\r\n ",IIC_read_state);

            /*写内存地址*/
            IIC_send_byte(_24cxx_readaddr);

            /*等待应答信号*/
            IIC_read_state = IIC_receive_ACK();
            //printf("current readACK[1] = %d\r\n ",IIC_read_state);

        }
        /*主机读方向-------------主机读地址的内容*/
        IIC_start();
        {
            /*读设备地址*/
            IIC_send_byte(generate_device_address(_24cxx_readaddr,1));

            /*等待应答信号*/
            IIC_read_state = IIC_receive_ACK();
            //printf("current readACK[2] = %d\r\n ",IIC_read_state);

            /*读取数据*/
            _24cxx_receive_byte = IIC_read_byte();
            

            /*主机发送非应答信号--表示结束*/
            IIC_send_ACK(IIC_nACK);   
        }
        /*结束传输信号*/
        IIC_stop();
    }

    if( IIC_read_state == IIC_ACK)
    {
        return _24cxx_receive_byte;
    }
    else
    {
        printf("\r\n The send data is fialed , please retransmit send  the data !!\r\n \r\n");
        return OP_FAILURE;
    }
    #endif
    /*硬件IIC实现*/
    #if IIC_HARDWARE_ENABLE
    {
        /*************************************轮询方式***************************************************************************/
        #if IIC_POLLING_ENABLE
            /*使用存储器读取函数*/
            Operation_state = HAL_I2C_Mem_Read(&hi2c1,generate_device_address(_24cxx_readaddr,1),_24cxx_readaddr,device_MemAddSize,&_24cxx_receive_byte,1,1000);
        #endif
        /*************************************IT方式***************************************************************************/
        #if IIC_IT_ENABLE

            Operation_state = HAL_I2C_Mem_Read_IT(&hi2c1,generate_device_address(_24cxx_readaddr,1),_24cxx_readaddr,device_MemAddSize,&_24cxx_receive_byte,1);
            
            /*等待IT标志位*/
            while (gIT_transfer_read_complete == 0);
            // 准备下一次传输
            gIT_transfer_read_complete = 0;
        #endif
        /*************************************DMA方式***************************************************************************/
        #if IIC_DMA_ENABLE

            Operation_state = HAL_I2C_Mem_Read_DMA(&hi2c1,generate_device_address(_24cxx_readaddr,1),_24cxx_readaddr,device_MemAddSize,&_24cxx_receive_byte,1);
            
            /*等待DMA标志位*/
            while (gDMA_transfer_read_complete == 0);
            // 准备下一次传输
            gDMA_transfer_read_complete = 0;
        #endif
    }
    if( Operation_state == HAL_OK)
    {
        return _24cxx_receive_byte;
    }
    else
    {
        printf("\r\n The send data is fialed , please retransmit send  the data !!\r\n \r\n");
        return OP_FAILURE;
    }
    #endif
}


/**
 * @brief       在AT24CXX指定地址写入一个数据
 * @param       write_addr: 写入数据的目的地址
 * @param       write_data: 要写入的数据
 * @retval      写入是否成功
 */
OperationResult EEPROM_WriteSingleByte(const uint16_t write_addr, const uint8_t write_data)
{
    /*获取待写入的地址*/
    uint16_t _24cxx_writeaddr = write_addr;
    /*获取型号配置*/
    const EEPROM_Config config = get_eeprom_config(current_type);
    /*获取设备地址长度 MemAddSize*/
    uint8_t device_addr_bit = config.addr_bits; 
    volatile uint8_t device_MemAddSize;
    /*获取待写入的数据*/
    volatile uint8_t Mem_write_data =  write_data;
    /*获取写入是否成功*/
    volatile OperationResult Operation_state = OP_SUCCESS;
#if IIC_SOFTWARE_ENABLE
    /*监控ACK应答情况*/
    IIC_State IIC_write_state = IIC_ACK;
#endif
    if(device_addr_bit < 16)
    {
        device_MemAddSize = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
        device_MemAddSize = I2C_MEMADD_SIZE_16BIT;
    }
    #if IIC_SOFTWARE_ENABLE
    {
        /*主机写方向-------------主机写设备地址+内存地址*/
        IIC_start();
        {
            /*写设备地址*/
            IIC_send_byte(generate_device_address(_24cxx_writeaddr,0));
    
            /*等待应答信号*/
            IIC_write_state = IIC_receive_ACK();
            //printf("current writeACK[0] = %d\r\n ",IIC_write_state);

            /*写内存地址*/
            IIC_send_byte(_24cxx_writeaddr);
    
            /*等待应答信号*/
            IIC_write_state = IIC_receive_ACK();
            //printf("current writeACK[1] = %d\r\n ",IIC_write_state);
    
        }
        /* 因为写数据的时候,不需要进入接收模式了,所以这里不用重新发送起始信号了 */
        /*开始发送数据部分*/
        {
            IIC_send_byte(write_data);
            /*等待ACK*/
            IIC_write_state = IIC_receive_ACK();
            //printf("current writeACK[2] = %d\r\n ",IIC_write_state);
        } 
        /*结束传输*/
        IIC_stop();
        /*给EERROM等待写入时间*/
        HAL_Delay(10);

        if( IIC_write_state == IIC_ACK)
        {
            return OP_SUCCESS;
        }
        return OP_FAILURE;
    }
    #endif
    /*硬件IIC实现*/
    #if IIC_HARDWARE_ENABLE
    {
        /*************************************轮询方式***************************************************************************/
        #if IIC_POLLING_ENABLE
            /*使用存储器写入函数*/
            Operation_state =  HAL_I2C_Mem_Write(&hi2c1,generate_device_address(_24cxx_writeaddr,0),_24cxx_writeaddr,(uint8_t)device_MemAddSize,(uint8_t *)(&Mem_write_data),1,1000);
        #endif
        /*************************************IT方式***************************************************************************/
        #if IIC_IT_ENABLE

            /*改用IT方式*/
            Operation_state =  HAL_I2C_Mem_Write_IT(&hi2c1,generate_device_address(_24cxx_writeaddr,0),_24cxx_writeaddr,(uint8_t)device_MemAddSize,(uint8_t *)(&Mem_write_data),1);
            //延迟等待EEPROM写入周期结束
            HAL_Delay(5);
            // 等待 IT传输完成 
            while (gIT_transfer_read_complete == 0);
            // 准备下一次传输
            gIT_transfer_read_complete = 0;
        #endif
        /*************************************DMA方式***************************************************************************/
        #if IIC_DMA_ENABLE
            /*改用DMA方式*/
            Operation_state =  HAL_I2C_Mem_Write_DMA(&hi2c1,generate_device_address(_24cxx_writeaddr,0),_24cxx_writeaddr,(uint8_t)device_MemAddSize,(uint8_t *)(&Mem_write_data),1);
            //延迟等待EEPROM写入周期结束
            HAL_Delay(5);
            // 等待 DMA传输完成 
            while (gDMA_transfer_read_complete == 0);
            // 准备下一次传输
            gDMA_transfer_read_complete = 0;
        #endif
        if( Operation_state != OP_SUCCESS)
        {
            return OP_SUCCESS;
        }
    }
    return OP_FAILURE;
    #endif
}


/**
* @brief 检查 AT24CXX 是否正常
* @note 检测原理: 在器件的末地址写如 0X55, 然后再读取, 如果读取值为 0X55
* 则表示检测正常. 否则,则表示检测失败.
* @param 无
* @retval 检测结果
* 1: 检测成功
* 0: 检测失败
*/
OperationResult EEPROM_check(void)
{
    uint8_t temp;
    
    temp = EEPROM_ReadSingleByte(EEPROM_AT24C02_endAddress);

    if( 0x55 == temp)   /* 读取数据正常 */
    {
        printf("Device has been detected senddata = %x\r\n",temp);
        printf("Driver success\r\n");
        
        return OP_SUCCESS;
    }
    else                 /* 排除第一次初始化的情况 */
    {
        EEPROM_WriteSingleByte(EEPROM_AT24C02_endAddress,EEPROM_TEXT_DATA); /* 先写入数据 */
        temp = EEPROM_ReadSingleByte(EEPROM_AT24C02_endAddress);/* 再读取数据 */
        if(0x55 == temp)
        {
            printf("Device has been detected senddata = %x\r\n",temp);
            return OP_SUCCESS;
        }
    }
    printf("Driver failure\r\n");
    return OP_FAILURE;
}



/*****************************************EEPROM扩展函数*************************************/
/**
* @brief 从EEPROM指定地址读取多个数据[自动翻页]
* @param start_addr : 起始读取地址（范围根据芯片容量，如24C02为0~255）
* @param[out] buffer : 存储读取数据的缓冲区指针
* @param length : 待读取数据的字节数
* @retval EEPROM_Status : 操作状态（成功/失败）
*/
OperationResult EEPROM_ReadBuffer(uint16_t start_addr, uint8_t *read_buffer, uint16_t length)
{   
    /*---- 地址相关参数初始化 ----*/ 
    //用于跟踪当前读取的位置，在读取过程中会不断更新
    uint16_t current_address = start_addr; // 当前读取地址游标 
    uint16_t bytes_remaining = length;     // 剩余待读取字节数 
    //用于生成 I2C 设备的地址，在整个读取过程中保持不变
    volatile uint16_t _24cxx_readaddr = start_addr; // 待读取的地址 
    const uint16_t end_address = start_addr + length; // 尾地址 
    
    /*---- 获取EEPROM配置参数 ----*/ 
    const EEPROM_Config config = get_eeprom_config(current_type); // 根据型号获取配置 
    const uint8_t device_addr_bit = config.addr_bits;  // 设备地址长度 
    volatile uint8_t device_MemAddSize; 
    const uint8_t page_size = config.page_size;  // 设备物理页大小（字节） 
 
    /*---- 操作状态初始化 ----*/ 
    HAL_StatusTypeDef Operation_state = HAL_OK; // 读取操作状态，初始化为成功    
#if IIC_SOFTWARE_ENABLE
    /*监控ACK应答情况*/
    IIC_State IIC_read_state = IIC_ACK;
#endif
    if(device_addr_bit < 16)
    {
        device_MemAddSize = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
        device_MemAddSize = I2C_MEMADD_SIZE_16BIT;
    }
    /* 越界保护检查 */
    if (end_address > EEPROM_AT24C02_endAddress)  // 使用预定义的设备最大地址常量 
    { 
        printf("Address out of range!\r\n"); 
        return OP_FAILURE; 
    }
    /*自动翻页读取数据*/
    while (bytes_remaining > 0 && Operation_state == HAL_OK)
    {
        /* 计算当前页编号：将当前地址除以页大小（向下取整） */
        uint16_t current_page = current_address / page_size;

        /* 计算下一页起始地址：(当前页号+1)*页大小（即当前页的结束地址+1） */
        uint16_t next_page_start_adrress = (current_page + 1) * page_size;

        /* 计算当前页剩余可读字节数：下一页起始地址 - 当前地址 */
        uint16_t bytes_in_page = next_page_start_adrress - current_address;

        /* 确定本次读取长度（不超过剩余数据量） */
        uint16_t current_page_to_read = (bytes_in_page < bytes_remaining) 
                                    ? bytes_in_page 
                                    : bytes_remaining;
    /*软件IIC实现*/
    #if IIC_SOFTWARE_ENABLE    
    {
        if(IIC_read_state == IIC_ACK)
        {
            /* 步骤1：发送设备地址（写模式）+ 内存地址 */
            IIC_start();
            {
                // 2. 发送设备地址（写模式）+ 存储地址
                /*写设备地址*/
                IIC_send_byte(generate_device_address(current_address,0));
        
                /*等待应答信号*/
                IIC_read_state = IIC_receive_ACK();
                //printf("current readbuffer_ACK[0] = %d\r\n ",IIC_receive_state);
        
                /*写内存地址*/
                IIC_send_byte(current_address);
        
                /*等待应答信号*/
                IIC_read_state = IIC_receive_ACK();
                //printf("current readbuffer_ACK[1] = %d\r\n ",IIC_receive_state);
        
            }
            /* 步骤2：重新发送起始信号 + 设备地址（读模式） */
            IIC_start();
            {
                /*读设备地址*/
                IIC_send_byte(generate_device_address(current_address,1));

                /*等待应答信号*/
                IIC_read_state = IIC_receive_ACK();
                //printf("current readbuffer_ACK[2] = %d\r\n ",IIC_receive_state);

            }
            /* 步骤3：连续读取数据 */
            for(int i = 0; i < current_page_to_read;i++)
            {
                read_buffer[i] = IIC_read_byte();

                if(i < current_page_to_read - 1)
                {
                    IIC_send_ACK(IIC_ACK);
                }
                /*最后一次发送NACK不应答*/
                else
                {
                    IIC_send_ACK(IIC_nACK);
                }
            }

            /*结束传输信号*/
            IIC_stop();
            /* 更新地址和剩余数据量 */
            current_address += current_page_to_read;
            read_buffer  += current_page_to_read;
            bytes_remaining -= current_page_to_read;
    
            /* 跨页时需等待EEPROM内部处理（仅当连续读取时） */
            if (bytes_remaining > 0) HAL_Delay(5);
        }
        else
        {
            /*读取失败*/
            printf("\r\n The read data operation  is fialed  ,error!!\r\n \r\n");
            return OP_FAILURE;
        }
    }
    #endif                          
    /*硬件IIC实现*/
    #if IIC_HARDWARE_ENABLE
    {
        /*************************************轮询方式***************************************************************************/
        #if IIC_POLLING_ENABLE
            Operation_state = HAL_I2C_Mem_Read(&hi2c1,generate_device_address(_24cxx_readaddr,1),current_address,device_MemAddSize,read_buffer,current_page_to_read,1000);
            if(Operation_state == HAL_OK) 
            {
                current_address += current_page_to_read;     // 更新地址游标 
                read_buffer  += current_page_to_read;        // 更新剩余字节数 
                bytes_remaining -= current_page_to_read;     // 更新数据指针 
            }
        #endif
        /*************************************中断***************************************************************************/
        #if IIC_IT_ENABLE
            Operation_state = HAL_I2C_Mem_Read_IT(&hi2c1,generate_device_address(_24cxx_readaddr,1),current_address,device_MemAddSize,read_buffer,current_page_to_read);
            
            /*等待执行完中断回调函数 */
            while(gIT_transfer_read_complete == 0);
            
            if(Operation_state == HAL_OK&&gIT_transfer_read_complete) 
            {
                gIT_transfer_read_complete = 0;
                current_address += current_page_to_read;     // 更新地址游标 
                read_buffer  += current_page_to_read;        // 更新剩余字节数 
                bytes_remaining -= current_page_to_read;     // 更新数据指针 
            }
        #endif
        /*************************************DMA方式***************************************************************************/
        #if IIC_DMA_ENABLE
            Operation_state = HAL_I2C_Mem_Read_DMA(&hi2c1,generate_device_address(_24cxx_readaddr,1),current_address,device_MemAddSize,read_buffer,current_page_to_read);
            
            /*等待执行完中断回调函数 */
            while(gDMA_transfer_read_complete == 0);
            
            if(Operation_state == HAL_OK&&gDMA_transfer_read_complete) 
            {
                gDMA_transfer_read_complete = 0;
                current_address += current_page_to_read;     // 更新地址游标 
                read_buffer  += current_page_to_read;        // 更新剩余字节数 
                bytes_remaining -= current_page_to_read;     // 更新数据指针 
            }
        #endif
        if(Operation_state != HAL_OK)
        {
        /*读取失败*/
            printf("\r\n The read data operation  is fialed  ,error!!\r\n \r\n");
            return OP_FAILURE;
        }
    }
    #endif
    }
    return OP_SUCCESS;
}
/**
 * @brief       在AT24CXX指定地址写入指定个数的数据【并且自动翻页】
 * @param       start_addr: 写入数据的目的地址
 * @param       write_buffer: 数据数组首地址
 * @param       length:写入长度
 * @retval      写入是否成功
 */
OperationResult EEPROM_WriteBuffer(uint16_t start_addr,uint8_t *write_buffer, uint16_t length)
{
    /*---- 地址相关参数初始化 ----*/ 
    //用于跟踪当前写入的位置，在写入过程中会不断更新
    uint16_t current_address = start_addr; // 当前写入地址游标 
    uint16_t bytes_remaining = length;     // 剩余待写入字节数
    //用于生成 I2C 设备的地址，在整个写入过程中保持不变
    volatile uint16_t _24cxx_writeaddr = start_addr; // 待写入的地址 
    const uint16_t end_address = start_addr + length; // 尾地址 
    
    /*---- 获取EEPROM配置参数 ----*/ 
    const EEPROM_Config config = get_eeprom_config(current_type); // 根据型号获取配置 
    const uint8_t device_addr_bit = config.addr_bits;  // 设备地址长度 
    volatile uint8_t device_MemAddSize; 
    const uint8_t page_size = config.page_size;  // 设备物理页大小（字节） 
    uint8_t *current_ptr = write_buffer;  // 数据缓冲区指针 

    /*---- 操作状态初始化 ----*/ 
    HAL_StatusTypeDef Operation_state = HAL_OK; // 写入操作状态，初始化为成功 
#if IIC_SOFTWARE_ENABLE
    IIC_State IIC_write_state = IIC_ACK;  // IIC总线状态跟踪
#endif
    /* 越界保护检查 */
    if (end_address > EEPROM_AT24C02_endAddress)  // 使用预定义的设备最大地址常量 
    { 
        printf("Address out of range!\r\n"); 
        return OP_FAILURE; 
    }

    if(device_addr_bit < 16)
    {
        device_MemAddSize = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
        device_MemAddSize = I2C_MEMADD_SIZE_16BIT;
    }

    while(bytes_remaining >0 && Operation_state == HAL_OK)
    {
        // 计算当前地址在页内的偏移量（0~page_size-1）
        uint16_t page_offset = current_address % page_size;
        
        // 计算当前页剩余可用空间（从当前地址到页尾的字节数）
        uint16_t current_page_remaining = page_size - page_offset;
        
        // 确定本次写入的字节数：取「当前页剩余空间」与「剩余待写入数据量」的较小值 
        uint16_t current_page_to_write = (current_page_remaining < bytes_remaining) 
                                         ? current_page_remaining 
                                        : bytes_remaining;
    /*软件IIC实现*/
    #if IIC_SOFTWARE_ENABLE
    {
        if(IIC_write_state == HAL_OK)
        {
            /*主机写方向-------------主机写设备地址+内存地址*/
            IIC_start();
            {
                /*写设备地址*/
                IIC_send_byte(generate_device_address(current_address,0));

                /*等待应答信号*/
                IIC_write_state = IIC_receive_ACK();

                /*写内存地址*/
                IIC_send_byte(current_address);

                /*等待应答信号*/
                IIC_write_state = IIC_receive_ACK();
                //printf("写入地址是否正确 = %d\r\n",IIC_write_state);

            }

            /* 因为写数据的时候,不需要进入接收模式了,所以这里不用重新发送起始信号了 */
            /*开始发送数据部分*/
            {
                for (int i = 0; i < current_page_to_write; i++)
                {
                    IIC_send_byte(*current_ptr++);

                    /*等待应答信号*/
                    IIC_write_state = IIC_receive_ACK();
                }  
            }
            /* 停止本次页写入 */
            IIC_stop();
            /* 状态更新与延时 */
            if(IIC_write_state == IIC_ACK) 
            {
                current_address += current_page_to_write;     // 更新地址游标 
                bytes_remaining -= current_page_to_write;    // 更新剩余字节数
                /*因为软件IIC中，current_ptr一直在移动*/
                /*硬件IIC中，current_ptr要手动重新更新为新一页起始地址*/
                //current_ptr += current_page_to_write;         // 更新数据指针  
                HAL_Delay(5);       // 等待内部写周期完成（典型值5ms）
            }
        }
        else
        {
            printf("The write data is fialed , please retransmit write  the data !!\r\n ");
            return OP_FAILURE;
        }
    }
    #endif
    /*硬件IIC实现*/
    #if IIC_HARDWARE_ENABLE
    {
        /*************************************轮询方式***************************************************************************/
        #if IIC_POLLING_ENABLE
            /*使用存储器写入函数*/
            Operation_state =  HAL_I2C_Mem_Write(&hi2c1,generate_device_address(_24cxx_writeaddr,0),current_address,device_MemAddSize,current_ptr,current_page_to_write,1000);
            if(Operation_state == HAL_OK) {
                current_address += current_page_to_write;     // 更新地址游标 
                bytes_remaining -= current_page_to_write;     // 更新剩余字节数 
                current_ptr += current_page_to_write;         // 更新数据指针 
                HAL_Delay(5);       // 等待内部写周期完成（典型值5ms））
            }
        #endif
        /*************************************中断实现***************************************************************************/
        #if IIC_IT_ENABLE
            Operation_state =  HAL_I2C_Mem_Write_IT(&hi2c1,generate_device_address(_24cxx_writeaddr,0),current_address,device_MemAddSize,current_ptr,current_page_to_write);
            
            /*等待从机发送ACK */
            while(gIT_transfer_write_complete ==0){};
            
            if ((Operation_state == HAL_OK)&&(gIT_transfer_write_complete == 1)) 
            {   
                gIT_transfer_write_complete = 0;
                current_address += current_page_to_write;     // 更新地址游标 
                bytes_remaining -= current_page_to_write;     // 更新剩余字节数 
                current_ptr += current_page_to_write;         // 更新数据指针 
                HAL_Delay(5);       // 等待内部写周期完成（典型值5ms））
            } 
        #endif
        /*************************************DMA方式***************************************************************************/
        #if IIC_DMA_ENABLE
            //改为用DMA方式
            Operation_state =  HAL_I2C_Mem_Write_DMA(&hi2c1,generate_device_address(_24cxx_writeaddr,0),current_address,device_MemAddSize,current_ptr,current_page_to_write);
            
            /*等待从机发送ACK */
            while( gDMA_transfer_write_complete ==0){};
                
            if ((Operation_state == HAL_OK)&&(gDMA_transfer_write_complete == 1))
            {   
                gDMA_transfer_write_complete = 0;
                current_address += current_page_to_write;     // 更新地址游标 
                bytes_remaining -= current_page_to_write;     // 更新剩余字节数 
                current_ptr += current_page_to_write;         // 更新数据指针 
                HAL_Delay(5);       // 等待内部写周期完成（典型值5ms））
            } 
        #endif
        if(Operation_state != HAL_OK)
        {
        /*读取失败*/
            printf("\r\n The read data operation  is fialed  ,error!!\r\n \r\n");
            return OP_FAILURE;
        }
    }
    #endif
    }
    return OP_SUCCESS;
}
