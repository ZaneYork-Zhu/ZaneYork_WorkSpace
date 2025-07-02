// 条件编译指令，用于防止头文件重复包含。如果未定义 __OLED_H_，则执行下面的代码 
// __OLED_H_ 是一个预处理器宏，通常使用全大写字母和下划线命名，以避免与其他标识符冲突 
#ifndef __OLED_H_ 
// 定义 __OLED_H_ 宏，当再次包含该头文件时，由于 __OLED_H_ 已定义，将跳过#ifndef 和 #endif 之间的内容 
#define __OLED_H_ 
 
// 包含 "main.h" 头文件，该头文件可能包含了项目的一些通用定义、宏、结构体等信息 
#include "main.h" 
#include "display_device.h" 
#include "system_config.h"

 
// 错误处理函数，释放内存并返回失败 
#define HANDLE_ERROR()  do{ free(sendbuffer); return OP_FAILURE; }while(0U)
// 定义一个宏 hi2c1，将其替换为 gI2C_HandleTypeStruct 
// 宏是一种预处理器指令，用于在编译前进行文本替换 
#define hi2c1               gI2C_HandleTypeStruct 
 
// 定义宏 IIC_HARDWARE_ENABLE 并赋值为 1，表示启用 I2C 硬件模式 
#define IIC_HARDWARE_ENABLE (0) 
// 定义宏 IIC_SOFTWARE_ENABLE 并赋值为 0，表示禁用 I2C 软件模式 
#define IIC_SOFTWARE_ENABLE (0) 
 
// 定义宏 SPI_HARDWARE_ENABLE 并赋值为 0，表示禁用 SPI 硬件模式 
#define SPI_HARDWARE_ENABLE (1) 
// 定义宏 SPI_SOFTWARE_ENABLE 并赋值为 0，表示禁用 SPI 软件模式 
#define SPI_SOFTWARE_ENABLE (0) 
 
// 声明一个函数 OLED_SSD1306_Init_text，该函数用于初始化 OLED 显示文本 
// 函数没有返回值，使用 void 类型 
void OLED_SSD1306_Init_text(void); 
OperationResult OLED_UpdateToGRAM_ALL(void);
// 声明一个函数 HAL_OledDeviceInit，该函数用于初始化 OLED，返回操作结果（成功或失败） 
// 返回值类型为 OperationResult 枚举类型 
OperationResult HAL_OledDeviceInit(void); 
// 声明一个函数 OLED_UpdateToGRAM_ALL，用于将所有数据更新到 OLED 的显存中，返回操作结果 
OperationResult HAL_OledDeviceFlushGramForDisplay(DisplayDevice *ptDisplayDevice); 
// 声明一个函数 HAL_OledDeviceSetPixel，用于设置 OLED 显示设备的像素
OperationResult HAL_OledDeviceSetPixel(DisplayDevice *ptDisplayDevice, uint8_t x, uint8_t y, uint8_t isLit);


// 声明一个静态函数 OLED_Clear，用于清除 OLED 屏幕内容，返回操作结果 
// 静态函数只能在声明它的文件中使用 
OperationResult OLED_Clear(void); 
// 声明一个静态函数 OLED_SetCursor，用于设置 OLED 光标位置 
// 参数 col 表示列位置，page 表示页位置，返回操作结果 
static OperationResult OLED_SetCursor(uint8_t col, uint8_t page); 
// 声明一个静态函数 OLED_Single_Write，用于向 OLED 写入单个数据或命令 
// 参数 data 表示要写入的数据，mode 表示写入模式，返回操作结果 
static OperationResult OLED_Single_Write(uint8_t data, uint8_t mode); 
// 声明一个静态函数 OLED_WriteSingle_Data，用于向 OLED 写入单个数据，返回操作结果 
static OperationResult OLED_WriteSingle_Data(const uint8_t write_data); 
// 声明一个静态函数 OLED_WriteSingle_Command，用于向 OLED 写入单个命令，返回操作结果 
static OperationResult OLED_WriteSingle_Command(const uint8_t write_command); 
 
// 声明一个静态函数 oled_show_pagemode_char_8x6，用于在页模式下显示 8x6 大小的字符 
// 参数 page 表示页位置，col 表示列位置，char_index 表示字符索引，返回操作结果 
static OperationResult oled_show_pagemode_char_8x6(uint8_t page, uint8_t col, uint8_t char_index); 
// 声明一个静态函数 oled_show_pagemode_char_16x8，用于在页模式下显示 16x8 大小的字符 
static OperationResult oled_show_pagemode_char_16x8(uint8_t page, uint8_t col, uint8_t char_index); 
// 声明一个静态函数 oled_show_pagemode_char_16x16，用于在页模式下显示 16x16 大小的字符 
static OperationResult oled_show_pagemode_char_16x16(uint8_t page, uint8_t col, uint8_t char_index); 
// 声明一个函数 oled_show_pagemode_char，用于在页模式下显示指定大小的字符 
// 参数 page 表示页位置，col 表示列位置，ch 表示要显示的字符，FontSize 表示字体大小，返回操作结果 
OperationResult oled_show_pagemode_char(uint8_t page, uint8_t col, uint8_t ch, uint8_t FontSize); 
// 声明一个函数 oLED_Show_pagemode_String，用于在页模式下显示字符串 
// 参数 page 表示页位置，col 表示列位置，string 表示要显示的字符串，FontSize 表示字体大小，返回操作结果 
OperationResult oLED_Show_pagemode_String(uint8_t page, uint8_t col, const uint8_t *string, uint8_t FontSize); 
 
// 声明一个函数 oLED_Show_PageMode_Image，用于在页模式下显示图像 
// 参数 page 表示页位置，col 表示列位置，image_width 表示图像宽度，image_height 表示图像高度，Img 表示图像数据，返回操作结果 
OperationResult oLED_Show_PageMode_Image(uint8_t page, uint8_t col, uint8_t image_width, uint8_t image_height, const uint8_t *Img); 
// 声明一个函数 oLED_Show_PageMode_Chinese，用于在页模式下显示中文字符串 
// 参数 page 表示页位置，col 表示列位置，pString_zh 表示中文字符串，返回操作结果 
OperationResult oLED_Show_PageMode_Chinese(uint8_t page, uint8_t col, const char *pString_zh); 
 
/**************************************************无page限制显示图形函数部分******************************************** */ 

// 声明一个函数 oLED_Clear_Area，用于清除指定区域的内容 
// 参数 row 表示行位置，col 表示列位置，clear_width 表示清除区域的宽度，clear_height 表示清除区域的高度，返回操作结果 
OperationResult oLED_Clear_Area(uint8_t row, uint8_t col, uint8_t clear_width, uint8_t clear_height); 
// 声明一个静态函数 OLED_Cont_Write，用于连续向 OLED 写入数据或命令 
// 参数 data 表示要写入的数据数组，mode 表示写入模式，Cont_len 表示连续写入的长度，返回操作结果 
static OperationResult OLED_Cont_Write(const uint8_t *data, uint8_t mode, uint8_t Cont_len); 
// 声明一个静态函数 OLED_WriteCont_Data，用于连续向 OLED 写入数据，返回操作结果 
static OperationResult OLED_WriteCont_Data(const uint8_t  *write_data, uint8_t Cont_len); 
// 声明一个静态函数 OLED_WriteCont_Data，用于连续向 OLED 写入数据，返回操作结果 
static OperationResult OLED_WriteCont_Command(const uint8_t  *write_data, uint8_t Cont_len); 
// 声明一个函数 OLED_Cont_SetCursor，用于连续设置 OLED 的光标位置
static OperationResult OLED_Cont_SetCursor(uint8_t row, uint8_t col) ;
// 声明一个函数 oLED_Show_offset_Image，用于在指定偏移位置显示图像 
// 参数 start_y 表示起始 y 坐标，start_x 表示起始 x 坐标，image_width 表示图像宽度，image_height 表示图像高度，Img 表示图像数据，返回操作结果 
OperationResult oLED_Show_offset_Image(uint8_t start_y, uint8_t start_x, uint8_t image_width, uint8_t image_height, const uint8_t *Img); 
// 声明一个函数 oLED_Show_offset_Chinese，用于在指定偏移位置显示中文字符串 
// 参数 page 表示页位置，col 表示列位置，pString_zh 表示中文字符串，返回操作结果 
OperationResult oLED_Show_offset_Chinese(uint8_t page, uint8_t col, const char *pString_zh); 
// 声明一个函数 oled_show_offset_char，用于在指定偏移位置显示字符
OperationResult oled_show_offset_char(uint8_t row, uint8_t col, uint8_t ch, uint8_t font_size);
// 声明一个函数 oLED_Show_offset_String，用于在指定偏移位置显示字符串
OperationResult oLED_Show_offset_String(uint8_t row, uint8_t col, const uint8_t *string, uint8_t FontSize);

// 声明一个函数 calculatePower，用于计算 10 的 n 次方
static uint32_t calculatePower(uint32_t base, uint32_t exponent);
void displayNumberOnOLED(uint16_t page, uint16_t col, uint32_t number, uint8_t digitCount, uint8_t fontSize);
 
// 结束 #ifndef 条件编译指令 
#endif 
