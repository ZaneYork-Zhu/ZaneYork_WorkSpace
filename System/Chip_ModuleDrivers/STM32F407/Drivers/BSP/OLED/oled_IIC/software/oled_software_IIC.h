
#ifndef OLED_Software_IIC_H_
#define OLED_Software_IIC_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
//oled_iic   i2c2 PB10  SCL PB11 SDA 
//240C02     i2c1 PB8 SCL PB9 SDA
/*IIC_SCL引脚初始化*/
#define SCL_IIC_Pin                     GPIO_PIN_10
#define SCL_IIC_GPIO_Port               GPIOB
#define SCL_IIC_GPIO_MODE               GPIO_MODE_OUTPUT_OD
#define __HAL_SCL_GPIO_CLK_ENABLE()     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/*IIC_SDA引脚初始化*/
#define SDA_IIC_Pin                     GPIO_PIN_11
#define SDA_IIC_GPIO_Port               GPIOB
#define SDA_IIC_GPIO_MODE               GPIO_MODE_OUTPUT_OD
#define __HAL_SDA_GPIO_CLK_ENABLE()     do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0U)
/* USER CODE END Includes */
    
/****************************************SPI设置**********************************************************/

/*EEPROM设置*/
#define _IIC_SCL(x)       do{x? \
                              HAL_GPIO_WritePin(SCL_IIC_GPIO_Port, SCL_IIC_Pin, GPIO_PIN_SET):  \
                              HAL_GPIO_WritePin(SCL_IIC_GPIO_Port, SCL_IIC_Pin, GPIO_PIN_RESET);} while(0U)
#define _IIC_SDA(x)       do{x? \
                              HAL_GPIO_WritePin(SDA_IIC_GPIO_Port, SDA_IIC_Pin, GPIO_PIN_SET):  \
                              HAL_GPIO_WritePin(SDA_IIC_GPIO_Port, SDA_IIC_Pin, GPIO_PIN_RESET);} while(0U)

// 宏定义：读取SDA引脚的电平状态
#define READ_SDA_PIN()    HAL_GPIO_ReadPin(SDA_IIC_GPIO_Port, SDA_IIC_Pin) 


typedef enum
{
  IIC_ACK = 0,    //应答成功
  IIC_nACK = 1    //应答失败

}IIC_State;



 /*数据位域*/                                                                        
 typedef union {
    struct
    {
      uint8_t Bit0:1;  // LSB
      uint8_t Bit1:1;
      uint8_t Bit2:1;
      uint8_t Bit3:1;
      uint8_t Bit4:1;
      uint8_t Bit5:1;
      uint8_t Bit6:1;
      uint8_t Bit7:1;  // MSB
    }bit;
    uint8_t Byte;
  }I2C_DataBuffer;  
    

static void _iic_delay(void);
static void _iic_W_SCL(uint8_t bitValue);
static void _iic_W_SDA(uint8_t bitValue);
static uint8_t _iic_R_SDA(void);

void I2C_Software_Init(void);
void IIC_idle_state(void);
void IIC_start(void);
void IIC_stop(void);
IIC_State IIC_receive_ACK(void);
void IIC_send_ACK(const IIC_State ACK_state);

void IIC_send_byte(const uint8_t data);
uint8_t IIC_read_byte(void);

#ifdef __cplusplus
}
#endif
#endif /*__ Software_iic_H__ */
    
