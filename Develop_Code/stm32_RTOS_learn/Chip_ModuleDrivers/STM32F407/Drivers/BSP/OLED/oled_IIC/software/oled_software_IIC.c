#include "oled_software_IIC.h"

void I2C_Software_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /*初始化GPIO时钟*/
    __HAL_SCL_GPIO_CLK_ENABLE();
    __HAL_SDA_GPIO_CLK_ENABLE();

    HAL_GPIO_WritePin(SCL_IIC_GPIO_Port, SCL_IIC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SCL_IIC_GPIO_Port, SDA_IIC_Pin, GPIO_PIN_SET);

    GPIO_InitStruct.Pin   = SCL_IIC_Pin;
    GPIO_InitStruct.Mode  = SCL_IIC_GPIO_MODE;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SCL_IIC_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = SDA_IIC_Pin;
    GPIO_InitStruct.Mode  = SDA_IIC_GPIO_MODE;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SDA_IIC_GPIO_Port, &GPIO_InitStruct);
    
    /*重置IIC总线*/
    IIC_stop();
}

/*************************************电平模拟函数*****************************************************/
/** 
* @brief 电平延时切换时间 
* 
* 该函数通过一个简单的for循环进行延时操作，循环次数为200次， 
* 用于实现电平切换时的短暂延时，确保电平稳定。 
* 
* @return 无 
*/ 
static void _iic_delay(void) 
{ 
    for(uint8_t time = 0; time < 200; time++) {;} 
} 

/** 
* @brief 向24C02芯片的SCL引脚发送指定电平 
* 
* 该函数将指定的电平值写入SCL引脚，并通过_iic_delay函数延时， 
* 以确保电平稳定。 
* 
* @param bitValue 要发送的电平值，类型为GPIO_PinState 
* @return 无 
*/
static void _iic_W_SCL(uint8_t bitValue)
{
    _IIC_SCL(bitValue);
    _iic_delay();
}

/** 
* @brief 向24C02芯片的SDA引脚发送指定电平 
* 
* 该函数将指定的电平值写入SDA引脚，并通过_24C02_delay函数延时
* 以确保电平稳定。 
* 
* @param bitValue 要发送的电平值，类型为GPIO_PinState 
* @return 无 
*/
static void _iic_W_SDA(uint8_t bitValue)
{
    _IIC_SDA(bitValue);
    _iic_delay();
}
/*接收电平*/

/** 
 * @brief 从24C02芯片的SDA引脚读取一个字节的数据 
 * 
 * 该函数通过读取SDA引脚的电平状态，并通过_24C02_delay函数延时 
 * 
 * @return uint8_t 读取到的字节数据 
 */ 
static uint8_t _iic_R_SDA(void) 
{ 
    // 定义一个I2CDataBuffer类型的变量，用于存储读取到的数据 
    I2C_DataBuffer receivedData = {0}; 
    // 读取SDA引脚的电平状态，并存储到联合体的Byte成员中 
    receivedData.bit.Bit0 = READ_SDA_PIN(); 
    // 延时确保数据稳定 
    _iic_delay();
    // 返回读取到的字节数据 
    return receivedData.Byte; 
} 
/*************************************信号状态函数*****************************************************/
/** 
* @brief 【主机】设备释放总线 
* 
* 该函数将SDA和SCL引脚都设置为高电平，使设备进入空闲状态，释放I2C总线。 
* 
* @return 无 
*/ 
void IIC_idle_state(void)
{
    /*变为空闲态*/
    _iic_W_SDA(1);
    _iic_W_SCL(1);
}

/*接收电平*/
/** 
* @brief 【主机】产生IIC起始信号 
* 
* 该函数先将设备设置为空闲态，然后拉低SDA引脚产生下降沿， 
* 再拉低SCL引脚，解除空闲态，从而产生I2C通信的起始信号。 
* 
* @param 无 
* @retval 无 
*/ 
void IIC_start(void)
{
    /*先变为空闲态*/
    IIC_idle_state();
    /*拉低SDA下降沿*/
    _iic_W_SDA(0);
    /*IIC解除空闲态*/
    _iic_W_SCL(0);
}

/**
 * @brief 【主机】产生IIC停止信号
 * @param  无
 * @retval 无
 */
void IIC_stop(void)
{
  
    _iic_W_SDA(0);
    /*一定要先释放SCL再释放SDA*/
    _iic_W_SCL(1);
    _iic_W_SDA(1);

}

/**
 * @brief 获取ACK信号
 * @param  注意：ACK的前后不需要释放总线，因为数据发送8位后
 *              已经在发送函数中释放了总线
 * @retval 无
 */
IIC_State IIC_receive_ACK(void)
{
    uint8_t wattime_ACK = 0;
    /*默认设置为ACK接收成功*/
    IIC_State ack_state = IIC_ACK;
    /*SCL高电平；监测ACK信号*/
    _iic_W_SCL(1);
    while (1 ==_iic_R_SDA())  /*等待应答*/
    {
        wattime_ACK++;
        if(250 < wattime_ACK)
        {
            //停止传输
            IIC_stop();
            /*接收失败------------直接跳出停止一起操作*/
            ack_state = IIC_nACK;
            break;
        }
    }
    
    /*SCL = 0 结束ACK检测*/
    _iic_W_SCL(0);
    return ack_state;
}

/**
 * @brief 发送ACK信号
 * @param  注意：ACK的前后不需要释放总线，因为数据发送8位后
 *              已经在发送函数中释放了总线
 * @retval 无
 */

void IIC_send_ACK(const IIC_State ACK_state)
{
    I2C_DataBuffer send_buffer_ACK = {0};

    send_buffer_ACK.Byte = ACK_state;

    /*主机发送应答信号*/
    _iic_W_SDA(send_buffer_ACK.bit.Bit0);

    _iic_W_SCL(1);
    /*SCL以低电平结尾*/
    _iic_W_SCL(0);

}

/*************************************数据操作函数*****************************************************/
/**
* @brief IIC 发送一个字节----写数据前【start信号已经钳住总线】
* @param data: 要发送的数据
*         SCL以低电平结尾
* @retval 无
*/
void IIC_send_byte(const uint8_t data)
{
    I2C_DataBuffer send_buffer = {0};
    send_buffer.Byte = data;
    /*主机占用SDA写数据*/
    {
        for(uint8_t send_bit = 0;send_bit < 8;send_bit++ )
        {
            _iic_W_SDA(send_buffer.bit.Bit7);
            _iic_W_SCL(1);
            /*SCL以低电平结尾*/
            _iic_W_SCL(0);
            send_buffer.Byte<<= 1;
        }
    }
    /*总线变为空闲态*/
    IIC_idle_state();
}

/**
* @brief IIC 读取一个字节----读数据前【主机要保证已经释放了总线，让从机写入】
* @retval 接收到的数据
*/
uint8_t IIC_read_byte(void)
{
    I2C_DataBuffer read_buffer = {0};
    /*总线变为空闲态-释放总线*/
    IIC_idle_state();
    /*从机占用SDA写数据【主机接收数据】*/
    {
        for(uint8_t read_bit = 0;read_bit < 8 ; read_bit++)
        {
            /*SCL高电平；主机获取数据*/
            _iic_W_SCL(1);
            read_buffer.bit.Bit0 = _iic_R_SDA();
            _iic_W_SCL(0);
            /*只移位7次 */
            if(read_bit<7)
            {
                read_buffer.Byte<<=1;
            }
        }
        return read_buffer.Byte;
    }
}



