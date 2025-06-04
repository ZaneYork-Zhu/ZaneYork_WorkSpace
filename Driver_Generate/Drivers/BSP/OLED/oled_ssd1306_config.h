#ifndef _SSD1306_CONFIG_H_
#define _SSD1306_CONFIG_H_


#include "stdint.h"

// 命令类型枚举 
typedef enum {
    SSD1306_CMD_SINGLE = 1,         // 单字节命令 
    SSD1306_CMD_DOUBLE,             // 双字节命令 
    SSD1306_CMD_TRIPLE              // 三字节命令
} SSD1306_CmdType;

// 命令结构体 
typedef struct {
    uint8_t cmd;                     // 命令表命令定义 
    SSD1306_CmdType type;            // 命令的结构-是几个字节类型
    uint8_t param1;                  // 参数（仅当type=DOUBLE时有效）
    uint8_t param2;                  // 参数（仅当type=TRIPLE时有效） 
} SSD1306_InitCmd;


//设备地址
#define OLED_ADRESS         0x78
//写指令模式
#define WRITE_COMMAND_MODE  0x00
//写数据模式
#define WRITE_DATA_MODE     0x40

#define PAGE_SIZE           (1*8)
//最大页数
#define PAGE_MAX            8
//最大行数
#define ROW_MAX             (8*8)
//最大列数
#define COL_MAX             128

/*************************display显示部分***********************************************************************************/
// 显示开关
#define OLED_CMD_DISPLAY_OFF          0xAE                                // 关闭显示（进入休眠模式，降低功耗）
#define OLED_CMD_DISPLAY_ON           0xAF                                // 开启显示（正常显示模式）

// 对比度调节（需后续跟随参数）
#define OLED_CMD_SET_CONTRAST         0x81                                // 设置对比度（需后跟 0x00~0xFF 参数，默认 0x7F）

// 显示模式
#define OLED_CMD_DISPLAY_NORMAL       0xA4                                // 正常显示模式（根据显存内容渲染）
#define OLED_CMD_DISPLAY_FORCE_ON     0xA5                                // 强制全屏点亮（忽略显存数据，用于测试）
#define OLED_CMD_COLOR_NORMAL         0xA6                                // 正常颜色模式（高电平点亮像素）
#define OLED_CMD_COLOR_INVERT         0xA7                                // 反色显示模式（低电平点亮像素）

// 起始行设置（范围 0x40~0x7F）
#define OLED_CMD_SET_START_LINE(line) (0x40 | (line))                     // 设置显示起始行（0~63，用于垂直滚动）

/*************************地址管理部分***********************************************************************************/

// 地址模式设置（需后续跟随参数）
#define OLED_CMD_SET_ADDRESS_MODE     0x20                                // 设置地址模式（需后跟 0x00/0x01/0x02）
#define OLED_ADDR_MODE_HORIZONTAL     0x00                                // 水平地址模式（自动换行，适合连续写入）
#define OLED_ADDR_MODE_VERTICAL       0x01                                // 垂直地址模式（列满后换列，适合图形刷新）
#define OLED_ADDR_MODE_PAGE           0x02                                // 页地址模式（需手动切换页，适合逐页操作）

// 页地址设置（范围 0xB0~0xB7）
#define OLED_CMD_SET_PAGE(page)     do{\
                                          const uint8_t cmd = 0xB0 | (page);\
                                          oled_write_byte(cmd, OLED_8080_CMD);} while(0U)


// 列地址低4位设置（范围 0x00~0x0F）
#define OLED_CMD_SET_COL_LOW(col)     (0x00 | ((col) & 0x0F))  // 设置列地址低4位（0~15）
 
// 列地址高4位设置（范围 0x10~0x1F）
#define OLED_CMD_SET_COL_HIGH(col)    (0x10 | (((col) >> 4) & 0x0F))  // 设置列地址高4位（0~7，对应0~127列）
 
// 完整列地址设置宏（0~127列）   最终列 = LOW(col)+HIGH(col)*16
#define OLED_CMD_SET_COLUMN(col)     do{\
                                        oled_write_byte(OLED_CMD_SET_COL_LOW(col), OLED_8080_CMD); \
                                        oled_write_byte(OLED_CMD_SET_COL_HIGH(col), OLED_8080_CMD);}while(0U)

// 列地址范围（需后续跟随起始列和结束列）
#define OLED_CMD_SET_COLUMN_RANGE     0x21                                // 设置列地址范围（需后跟起始列和结束列参数）

/*************************硬件配置部分***********************************************************************************/

// 电荷泵控制 
#define OLED_CMD_CHARGE_PUMP_ON       0x8D                                // 开启电荷泵控制指令（必须启用）
#define OLED_CMD_CHARGE_PUMP_ENABLE   0x14                                // 电荷泵使能参数（配合 0x8D 使用）

// COM引脚扫描方向 
#define OLED_CMD_SET_COM_PINS         0xDA                                // 设置 COM 引脚硬件布局（需后跟扫描方向参数）
#define OLED_COM_SCAN_SEQUENTIAL      0x02                                // COM 顺序扫描（常规屏幕布局）
#define OLED_COM_SCAN_ALTERNATE       0x12                                // COM 交替扫描（特殊屏幕布局）

// 多路复用比率（需后续跟随参数）设置垂直分辨率 = 实际行数-1
#define OLED_CMD_SET_MUX_RATIO        0xA8                                // 设置屏幕高度复用比率（如 0x3F 对应 64 行）

// 时钟分频（需后续跟随参数）设置刷新频率--设置为默认的最大刷新率0x80
#define OLED_CMD_SET_CLOCK_DIV        0xD5                                // 设置时钟分频（调节刷新率）

//垂直显示偏移量，即调整显示内容的起始行位置
#define OLED_CMD_SET_DISPLAY_OFFSET    0xD3                               // 设置垂直显示偏移量（需后跟偏移参数）

// 设置预充电周期指令（需后跟参数）
#define OLED_CMD_SET_PRECHARGE_PERIOD  0xD9                               // Phase1 和 Phase2 的取值需在 0x0~0xF 之间（对应 1~16 DCLK 周期） 

// 参数示例（默认配置）默认：0x22（Phase1=2 DCLK，Phase2=2 DCLK）
#define OLED_PRECHARGE_PHASE1_DEFAULT  0x02                               // Phase1=2 DCLK 低功耗模式：Phase1=1, Phase2=1
#define OLED_PRECHARGE_PHASE2_DEFAULT  0x02                               // Phase2=2 DCLK 高刷新率：Phase1=15, Phase2=1

// 设置 VCOMH 电压倍率（需后跟参数）
#define OLED_CMD_SET_VCOMH             0xDB  
#define OLED_VCOMH_083_VCC             0x30                              // 0.83 × VCC 0x30（二进制 011）：0.83 × VCC（最高亮度，可能缩短屏幕寿命）
/*************************滚屏功能部分***********************************************************************************/
// 水平滚动方向 
#define OLED_CMD_SCROLL_RIGHT         0x26                                // 开启水平向右滚动（需设置页范围和速度）
#define OLED_CMD_SCROLL_LEFT          0x27                                // 开启水平向左滚动 
 
// 垂直+水平滚动 
#define OLED_CMD_SCROLL_VH_RIGHT      0x29                                // 开启垂直+水平向右滚动（需设置偏移量）
#define OLED_CMD_SCROLL_VH_LEFT       0x2A                                // 开启垂直+水平向左滚动 
 
// 滚动开关 
#define OLED_CMD_SCROLL_OFF           0x2E                                // 关闭滚动功能 
#define OLED_CMD_SCROLL_ON            0x2F                                // 启用已配置的滚动效果

/*************************显示方向部分***********************************************************************************/
// 列地址方向 
#define OLED_CMD_SEGMENT_REMAP_NORMAL  0xA0                               // 列地址从左到右（默认方向）
#define OLED_CMD_SEGMENT_REMAP_REVERSE 0xA1                               // 列地址从右到左（水平镜像）
 
// 行扫描方向 
#define OLED_CMD_COM_SCAN_NORMAL       0xC0                               // 行扫描方向从上到下（默认）
#define OLED_CMD_COM_SCAN_REVERSE      0xC8                               // 行扫描方向从下到上（垂直镜像）

/***********************************************************************************************************************/
// 初始化命令序列 
const static SSD1306_InitCmd initCommands[] = { 
    // 关闭显示，进入休眠模式，降低功耗 
    {OLED_CMD_DISPLAY_OFF, SSD1306_CMD_SINGLE, 0, 0}, 
    // 设置时钟分频，调节刷新率，此处设置为默认的最大刷新率 
    {OLED_CMD_SET_CLOCK_DIV, SSD1306_CMD_DOUBLE, 0x80, 0}, 
    // 设置屏幕高度复用比率，0x3F 对应 64 行 
    {OLED_CMD_SET_MUX_RATIO, SSD1306_CMD_DOUBLE, 0x3F, 0}, 
    // 设置垂直显示偏移量，此处偏移量为 0 
    {OLED_CMD_SET_DISPLAY_OFFSET, SSD1306_CMD_DOUBLE, 0x00, 0}, 
    // 设置显示起始行，此处从第 0 行开始 
    {OLED_CMD_SET_START_LINE(0), SSD1306_CMD_SINGLE, 0, 0}, 
    // 设置列地址从右到左，实现水平镜像 
    {OLED_CMD_SEGMENT_REMAP_REVERSE, SSD1306_CMD_SINGLE, 0, 0}, 
    // 设置行扫描方向从下到上，实现垂直镜像 
    {OLED_CMD_COM_SCAN_REVERSE, SSD1306_CMD_SINGLE, 0, 0}, 
    // 设置 COM 引脚硬件布局为交替扫描 
    {OLED_CMD_SET_COM_PINS, SSD1306_CMD_DOUBLE, 0x12, 0}, 
    // 设置对比度，此处对比度为 0xCF 
    {OLED_CMD_SET_CONTRAST, SSD1306_CMD_DOUBLE, 0xCF, 0}, 
    // 设置预充电周期，Phase1 和 Phase2 取值为 0xF1 
    {OLED_CMD_SET_PRECHARGE_PERIOD, SSD1306_CMD_DOUBLE, 0xF1, 0}, 
    // 设置 VCOMH 电压倍率为 0.83 × VCC 
    {OLED_CMD_SET_VCOMH, SSD1306_CMD_DOUBLE, 0x30, 0}, 
    // 设置正常显示模式，根据显存内容渲染 
    {OLED_CMD_DISPLAY_NORMAL, SSD1306_CMD_SINGLE, 0, 0}, 
    // 设置正常颜色模式，高电平点亮像素 
    {OLED_CMD_COLOR_NORMAL, SSD1306_CMD_SINGLE, 0, 0}, 
    // 开启电荷泵控制，使能参数为 0x14 
    {OLED_CMD_CHARGE_PUMP_ON, SSD1306_CMD_DOUBLE, 0x14, 0}, 
    // 开启显示，进入正常显示模式 
    {OLED_CMD_DISPLAY_ON, SSD1306_CMD_SINGLE, 0, 0} 
}; 

#endif
