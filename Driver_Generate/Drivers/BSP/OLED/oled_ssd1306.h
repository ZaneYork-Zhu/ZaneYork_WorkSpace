#ifndef __OLED_H_
#define __OLED_H_


#include "main.h"

typedef enum {
    OP_SUCCESS = 1,  // 操作成功 
    OP_FAILURE = 0   // 操作失败 
} OperationResult;


#define hi2c1               gI2C_HandleTypeStruct

#define IIC_HARDWARE_ENABLE (1)
#define IIC_SOFTWARE_ENABLE (0)

#define SPI_HARDWARE_ENABLE (0)
#define SPI_SOFTWARE_ENABLE (0)


void OLED_SSD1306_Init_text(void);

OperationResult OLED_SSD1306_Init(void);
static OperationResult OLED_Clear(void);
static OperationResult OLED_SetCursor(uint8_t col,uint8_t page);
static OperationResult OLED_Single_Write(uint8_t data, uint8_t mode);
static OperationResult OLED_WriteSingle_Data(const uint8_t write_data);
static OperationResult OLED_WriteSingle_Command(const uint8_t write_command);

static OperationResult oled_show_pagemode_char_8x6(uint8_t page, uint8_t col, uint8_t char_index);
static OperationResult oled_show_pagemode_char_16x8(uint8_t page, uint8_t col, uint8_t char_index);
static OperationResult oled_show_pagemode_char_16x16(uint8_t page, uint8_t col, uint8_t char_index);
OperationResult oled_show_pagemode_char(uint8_t page, uint8_t col, uint8_t ch,uint8_t FontSize);
OperationResult oLED_Show_pagemode_String(uint8_t page, uint8_t col, const uint8_t *string, uint8_t FontSize);

OperationResult oLED_Show_PageMode_Image(uint8_t page, uint8_t col, uint8_t image_width,uint8_t image_height, const uint8_t *Img);
OperationResult oLED_Show_PageMode_Chinese(uint8_t page, uint8_t col,const char *pString_zh);

/**************************************************无page限制显示图形函数部分******************************************** */
OperationResult OLED_UpdateToGRAM_ALL(void);
OperationResult oLED_Clear_Area(uint8_t row, uint8_t col, uint8_t clear_width, uint8_t clear_height);
static OperationResult OLED_Cont_Write(const uint8_t *data, uint8_t mode, uint8_t Cont_len) ;
static OperationResult OLED_WriteCont_Data(const uint8_t  *write_data,uint8_t Cont_len);
static OperationResult OLED_WriteCont_Command(const uint8_t *write_command,uint8_t Cont_len);
OperationResult oLED_Show_offset_Image(uint8_t start_y, uint8_t start_x, uint8_t image_width, uint8_t image_height, const uint8_t *Img);
OperationResult oLED_Show_offset_Chinese(uint8_t page, uint8_t col,const char *pString_zh);
#endif
