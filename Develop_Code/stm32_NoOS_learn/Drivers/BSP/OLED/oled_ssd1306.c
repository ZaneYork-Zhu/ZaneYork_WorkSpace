#include "oled_ssd1306.h" 
#include "oled_ssd1306_config.h" 
#include "oled_ASCII_font.h" 
#include "oled_Image_Array.h" 
 
#include <string.h> 
#include <stdlib.h> 
 
// 根据配置选择不同的通信协议库 
#if IIC_SOFTWARE_ENABLE 
    #include "oled_software_IIC.h" 
#endif 
#if IIC_HARDWARE_ENABLE 
    #include "oled_hardware_IIC.h" 
#endif 
#if SPI_SOFTWARE_ENABLE 
    #include "oled_software_SPI.h" 
#endif 
#if SPI_HARDWARE_ENABLE 
    #include "oled_Hardware_SPI.h" 
#endif 
 
/** 
  * OLED显存数组 
  * 所有的显示函数，都只是对此显存数组进行读写 
  * 随后调用OLED_Update函数或OLED_UpdateArea函数 
  * 才会将显存数组的数据发送到OLED硬件，进行显示 
  */ 
static uint8_t gOLED_gram_buffer[PAGE_MAX][COL_MAX] = {0}; 
 
// 初始化OLED并显示测试图像的函数 
void OLED_SSD1306_Init_text(void) 
{ 
    uartInit(USART1,115200);

    // 调用OLED初始化函数 
    OLED_SSD1306_Init(); 
    // 定义一个测试图像数据数组 
    uint8_t Image[] = { 
        0xFF,0x01,0xE1,0x11,0x49,0x25,0x45,0x05,0x45,0x25,0x49,0x11,0xE1,0x01,0xFF, 
        0x7F,0x40,0x43,0x44,0x48,0x51,0x52,0x52,0x52,0x51,0x48,0x44,0x43,0x40,0x7F 
    }; 
    // 在指定位置显示测试图像 
    oLED_Show_offset_Image(0,0,15,15,Image); 
    oLED_Show_offset_Image(15,15,15,15,Image); 
    oLED_Show_offset_Image(30,30,15,15,Image); 
    // displayNumberOnOLED(0, 0, 12,2,6); // 显示数字123456789
    // // oled_show_offset_char(0,0,'A',16);
    // oLED_Show_offset_String(18,1,"adasfaf",6);
    OLED_UpdateToGRAM_ALL(); 
    // 注释掉的函数，用于按页模式显示图像 
    // oLED_Show_PageMode_Image(0,0,15,15,Image); 

} 
/** 
 * @brief 初始化SSD1306 OLED显示屏。 
 *        该函数首先根据配置初始化I2C软件或硬件，或SPI软件，然后遍历初始化命令序列， 
 *        依次发送每个命令及其参数到OLED显示屏，以完成对显示屏的各项参数设置，使其进入正常显示模式。 
 *        最后清屏，避免出现雪花屏。 
 * @param 无 
 * @retval OP_SUCCESS 初始化成功；OP_FAILURE 初始化失败。 
 */ 
OperationResult OLED_SSD1306_Init(void) 
{ 
    // 根据配置初始化不同的通信协议 
#if IIC_SOFTWARE_ENABLE 
    I2C_Software_Init(); 
#endif 
#if IIC_HARDWARE_ENABLE 
    I2C_Hardware_Init(IICx,400000); 
#endif 
#if SPI_SOFTWARE_ENABLE 
    // SSD1306使用SPI的模式0 
    oled_software_SPI_GPIO_Init(0); 
#endif
#if SPI_HARDWARE_ENABLE 
    // SSD1306使用SPI的模式0 
    oled_hardware_SPI_GPIO_Init(0,OLED_HARDWARE_SPI,SPI_BAUDRATEPRESCALER_2); 
#endif  
    // 初始化操作结果为成功 
    OperationResult Operation_state = OP_SUCCESS; 
    // 遍历初始化命令序列 
    for (uint8_t i = 0; i < sizeof(initCommands) / sizeof(initCommands[0]); i++) 
    { 
        // 获取当前命令 
        const SSD1306_InitCmd* currentCommand = &initCommands[i]; 
        // 写入命令 
        Operation_state = OLED_WriteSingle_Command(currentCommand->cmd); 
 
        // 根据命令类型写入参数 
        if (currentCommand->type == SSD1306_CMD_DOUBLE) 
        { 
            // 双字节命令，写入参数 1 
            Operation_state = OLED_WriteSingle_Command(currentCommand->param1); 
        } 
        else if (currentCommand->type == SSD1306_CMD_TRIPLE) 
        { 
            // 三字节命令，写入参数 1 和参数 2 
            Operation_state = OLED_WriteSingle_Command(currentCommand->param1); 
            Operation_state = OLED_WriteSingle_Command(currentCommand->param2); 
        } 
        // 如果操作失败，直接返回失败结果 
        if (Operation_state != OP_SUCCESS) 
        { 
            return OP_FAILURE; 
        }    
    } 
    // 不清屏oled默认是随机值，输出为雪花屏，所以清屏 
    // 清屏OLED 
    Operation_state = OLED_Clear(); 
    if (Operation_state != OP_SUCCESS) 
    { 
        return OP_FAILURE; 
    }    
    // 所有操作成功，返回成功结果 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief 通过I2C接口向OLED发送数据或命令。 
 *        该函数使用I2C协议与OLED进行通信，发送设备地址、模式（数据或命令）和具体的数据， 
 *        并根据I2C的应答状态判断发送是否成功。 
 * @param data 要发送的数据或命令。 
 * @param mode 发送模式，指定是数据还是命令。 
 * @retval OP_SUCCESS 发送成功；OP_FAILURE 发送失败。 
 */ 
static OperationResult OLED_Single_Write(uint8_t data, uint8_t mode) 
{ 
    // 定义发送缓冲区 
    volatile uint8_t sendbuffer[2] = {0}; 
    // 将模式存入缓冲区 
    sendbuffer[0] = mode; 
    // 将数据存入缓冲区 
    sendbuffer[1] = data; 
 
    // 根据配置选择不同的通信协议进行数据发送 
#if IIC_SOFTWARE_ENABLE 
    IIC_State IIC_write_state = IIC_ACK; 
 
    IIC_start(); 
    { 
        // 发送OLED设备地址 
        IIC_send_byte(OLED_ADRESS); 
        // 接收I2C应答信号 
        IIC_write_state = IIC_receive_ACK(); 
 
        if (IIC_write_state != IIC_ACK) { 
            IIC_stop(); 
            printf("IIC send device address failed!\r\n");
            return OP_FAILURE; 
        } 
 
        // 发送模式 
        IIC_send_byte(mode); 
        // 接收I2C应答信号 
        IIC_write_state = IIC_receive_ACK(); 
 
        if (IIC_write_state != IIC_ACK) { 
            printf("IIC send mode failed!\r\n");
            IIC_stop(); 
            return OP_FAILURE; 
        } 
    } 
 
    // 发送数据 
    IIC_send_byte(data); 
    // 接收I2C应答信号 
    IIC_write_state = IIC_receive_ACK(); 
 
    IIC_stop(); 
 
    // 如果收到应答信号，操作结果设置为成功 
    if (IIC_write_state == IIC_ACK) 
    { 
        return OP_SUCCESS; 
    } 
#endif 
#if IIC_HARDWARE_ENABLE 
    // 使用硬件I2C发送数据 
    HAL_StatusTypeDef hal_status = oled_IIC_HardwareSwapByte(OLED_ADRESS, (uint8_t * ) sendbuffer, 2); 
    // 如果硬件发送成功，返回成功结果 
    if (hal_status == HAL_OK) { 
        return OP_SUCCESS; 
    } 
#endif 
#if SPI_SOFTWARE_ENABLE 
    // 启动SPI通信 
    oled_spi_Start(); 
    { 
        // 发送写命令/数据模式 
        oled_software_SPI_W_DC(mode); 
        // 交换数据 
        oled_spiSoftwareSwapByte(data); 
    } 
    // 停止SPI通信 
    oled_spi_Stop(); 
    // 假设SPI软件通信默认成功，可根据实际情况修改 
    return OP_SUCCESS; 
#endif 
 
#if SPI_HARDWARE_ENABLE 
    // 启动SPI通信 
    OLED_HARDWARE_SPI_CS(0); 
    // 发送写命令/数据模式 
    OLED_HARDWARE_SPI_DC(mode); 
    // 交换数据 
    HARDWARE_SPI_State SPI_status = oled_spiHardwareSwapByte(&data, 1); 
    // 停止SPI通信 
    OLED_HARDWARE_SPI_CS(1); 
    OLED_HARDWARE_SPI_DC(1); 
    // 假设SPI硬件通信默认成功，可根据实际情况修改 
    if (SPI_status == HARDWARE_SPI_FINISH) { 
        return OP_SUCCESS; 
    } 
#endif 
 
    // 如果没有任何通信协议启用，返回失败 
    return OP_FAILURE; 
} 
 
/** 
 * @brief 向OLED发送单个数据。 
 *        调用OLED_Write函数，以WRITE_DATA_MODE模式发送数据。 
 * @param write_data 要发送的数据。 
 * @retval OP_SUCCESS 发送成功；OP_FAILURE 发送失败。 
 */  
static OperationResult OLED_WriteSingle_Data(const uint8_t write_data) 
{ 
    // 以数据模式调用单个写入函数 
    return OLED_Single_Write(write_data, WRITE_DATA_MODE); 
} 
 
/** 
* @brief 向OLED发送单个命令。 
*        调用OLED_Write函数，以WRITE_COMMAND_MODE模式发送命令。 
* @param write_command 要发送的命令。 
* @retval OP_SUCCESS 发送成功；OP_FAILURE 发送失败。 
*/  
static OperationResult OLED_WriteSingle_Command(const uint8_t write_command) 
{ 
   // 以命令模式调用单个写入函数 
   return OLED_Single_Write(write_command, WRITE_COMMAND_MODE); 
} 
 
/** 
 * @brief 设置OLED光标位置。 
 *        该函数根据传入的页面和列位置，通过发送相应的命令来设置OLED的光标位置。 
 * @param page 要设置的页面，范围从0到PAGE_MAX - 1。 
 * @param col 要设置的列位置，范围从0到COL_MAX - 1。 
 * @retval OP_SUCCESS 设置成功；OP_FAILURE 设置失败。 
 */ 
static OperationResult OLED_SetCursor(uint8_t page, uint8_t col) 
{ 
    // 初始化操作结果为成功 
    OperationResult Operation_state = OP_SUCCESS; 
    // 检查页面是否越界 
    if(page >= PAGE_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 设置纵坐标col X7~X0 
    { 
        // 写入col的低四位0x00|(X3~X0)【改变这里的0x00的最后4位】 
        Operation_state = OLED_WriteSingle_Command(0x00 | (col & 0x0F)); 
        // 写入col的高四位0x10|(X7~X4)【改变这里的0x10的最后4位】->所以要把X7~X4移到低四位 
        Operation_state = OLED_WriteSingle_Command(0x10 | ((col & 0xF0) >> 4)); 
    } 
 
    // 设置横坐标page ：page +1->移动8个bit 
    {   
        // 写入page的低四位0x00|(X2~X0)【改变这里的0xB0的最后3位】 
        Operation_state = OLED_WriteSingle_Command(0xB0 | (page & 0x07)); 
    } 
    // 如果操作失败，返回失败结果 
    if (Operation_state != OP_SUCCESS) 
    { 
        return OP_FAILURE; 
    } 
    // 所有操作成功，返回成功结果 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief oled清屏 
 *        该函数遍历所有页面和列，将每个位置的数据设置为0x00，实现清屏操作。 
 * @retval OP_SUCCESS 发送成功；OP_FAILURE 发送失败。 
 */ 
static OperationResult OLED_Clear(void) 
{ 
    // 初始化操作状态为成功 
    OperationResult operationState = OP_SUCCESS; 
 
    // 遍历所有页面 
    for (uint8_t currentPage = 0; currentPage < PAGE_MAX; currentPage++) 
    { 
        // 切换到当前页面的起始列 
        operationState = OLED_SetCursor(currentPage, 0); 
        // 如果设置光标位置失败，返回失败结果 
        if (operationState != OP_SUCCESS) 
        { 
            return OP_FAILURE; 
        } 
 
        // 遍历当前页面的所有列 
        for (uint8_t currentColumn = 0; currentColumn < COL_MAX; currentColumn++) 
        { 
            // 向当前位置写入 0x00 进行清屏 
            operationState = OLED_WriteSingle_Data(0x00); 
            // 如果写入数据失败，返回失败结果 
            if (operationState != OP_SUCCESS) 
            { 
                return OP_FAILURE; 
            } 
        } 
    } 
 
    // 所有操作成功，返回成功结果 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief 在指定位置显示图形 
 *        该函数根据传入的页面、列位置、图像宽度和高度，以及图像数据指针， 
 *        将图像数据按页模式显示在OLED上。 
 * @param page 显示页面，范围从0到PAGE_MAX - 1。 
 * @param col 显示列位置，范围从0到COL_MAX - 1。 
 * @param image_width 图像宽度，范围从0到COL_MAX - col。 
 * @param image_height 图像高度，范围从0到ROW_MAX * PAGE_SIZE - page * PAGE_SIZE。 
 * @param Img 要显示的图像数据指针。 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */ 
OperationResult oLED_Show_PageMode_Image(uint8_t page, uint8_t col, uint8_t image_width, uint8_t image_height, const uint8_t *Img) 
{ 
    // OLED按页进行写入->一页8行 
    // 向上取整计算图像所需页数 
    uint8_t image_page = (image_height + PAGE_MAX - 1) / PAGE_MAX; 
    // 初始化操作结果为成功 
    OperationResult operation_state = OP_SUCCESS;  
    // 进行越界判断 
    // 检查列位置和图像宽度是否超出最大宽度 
    if (col + image_width >= COL_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 检查页面和图像高度转换后的页数是否超出最大页数 
    if (page + image_page >= PAGE_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 遍历图像所需的每一页 
    for (uint8_t height = 0; height < image_page; height++) 
    { 
        // 设置光标位置 
        operation_state = OLED_SetCursor(page + height, col); 
        // 如果设置光标位置失败，返回失败结果 
        if (operation_state != OP_SUCCESS) 
        { 
            return OP_FAILURE; 
        } 
 
        // 打印图像一行 
        for (uint8_t width = 0; width < image_width; width++) 
        { 
            // 一次偏移一整行 
            operation_state = OLED_WriteSingle_Data(Img[image_width * height + width]); 
            // 如果写入数据失败，返回失败结果 
            if (operation_state != OP_SUCCESS) 
            { 
                return OP_FAILURE; 
            } 
        } 
    } 
 
    // 所有操作成功，返回成功结果 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief 在 OLED 上显示 8x6 字体的字符 
 *        该函数根据传入的页面、列位置和字符索引，从字库数组中获取字符数据， 
 *        并将其显示在OLED上。 
 * @param page 显示页面，范围从0到PAGE_MAX - 1。 
 * @param col 显示列位置，范围从0到COL_MAX - OLED_FONT_SIZE_8X6。 
 * @param char_index 字符在字库数组中的索引，范围从0到字符数量 - 1。 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */ 
static OperationResult oled_show_pagemode_char_8x6(uint8_t page, uint8_t col, uint8_t char_index)
{ 
    // 进行越界判断 
    // 检查列位置和图像宽度是否超出最大宽度 
    if (col + OLED_FONT_SIZE_8X6 >= COL_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 检查页面和图像高度转换后的页数是否超出最大页数 
    if (page + 1 >= PAGE_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 设置 OLED 光标到指定的页面和列位置 
    OLED_SetCursor(page, col); 
    // Note : 必须从上到下按照顺序索引方式 
    // 遍历字符的每一列 
    for (uint8_t col_index = 0; col_index < OLED_FONT_SIZE_8X6; col_index++) 
    { 
        // 从字库数组中获取当前列的数据 
        uint8_t data = OLED_F8x6[char_index][col_index]; 
        // 向 OLED 写入当前列的数据 
        OLED_WriteSingle_Data(data); 
    } 
 
    // 所有操作成功，返回成功结果 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief 在 OLED 上显示 16x8 字体的字符 
 *        该函数根据传入的页面、列位置和字符索引，从字库数组中获取字符数据， 
 *        分两页显示在OLED上。 
 * @param page 显示页面，范围从0到PAGE_MAX - 2。 
 * @param col 显示列位置，范围从0到COL_MAX - OLED_FONT_SIZE_16X8。 
 * @param char_index 字符在字库数组中的索引，范围从0到字符数量 - 1。 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */ 
static OperationResult oled_show_pagemode_char_16x8(uint8_t page, uint8_t col, uint8_t char_index) 
{ 
    // 进行越界判断 
    // 检查列位置和图像宽度是否超出最大宽度 
    if (col + OLED_FONT_SIZE_16X8 >= COL_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 检查页面和图像高度转换后的页数是否超出最大页数 
    if (page + 2 >= PAGE_MAX) 
    { 
        return OP_FAILURE; 
    } 
    // 循环两次，分别处理第一页和第二页 
    for (uint8_t page_offset = 0; page_offset < 2; page_offset++) 
    { 
        // 设置光标位置 
        OLED_SetCursor(page + page_offset, col); 
        // 循环写入每一列的数据 
        for (uint8_t col_index = 0; col_index < OLED_FONT_SIZE_16X8; col_index++) { 
            // 计算字库数组的索引 
            uint8_t data = OLED_F16x8[char_index][col_index + page_offset * OLED_FONT_SIZE_16X8]; 
            // 写入数据 
            OLED_WriteSingle_Data(data); 
        } 
    } 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief 在 OLED 上显示 16x16 字体的字符 
 * @param page 显示页面 
 * @param col 显示列位置 
 * @param char_index 字符在字库数组中的索引 
 * @return 操作结果，成功返回 OP_SUCCESS 
 */ 
static OperationResult oled_show_pagemode_char_16x16(uint8_t page, uint8_t col, uint8_t char_index) { 
    // 进行越界判断 
    // 检查列位置和图像宽度是否超出最大宽度 
    if (col + OLED_FONT_SIZE_16X16 >= COL_MAX) { 
        return OP_FAILURE; 
    } 
    // 检查页面和图像高度转换后的页数是否超出最大页数 
    if (page + 2 >= PAGE_MAX) { 
        return OP_FAILURE; 
    }
    // 循环两次，分别处理第一页和第二页 
    for (uint8_t page_offset = 0; page_offset < 2; page_offset++) { 
        // 设置光标位置 
        OLED_SetCursor(page + page_offset, col); 
        // 循环写入每一列的数据 
        for (uint8_t col_index = 0; col_index < OLED_FONT_SIZE_16X16; col_index++) { 
            // 计算字库数组的索引 
            uint8_t data = OLED_F16x16[char_index][col_index + page_offset * OLED_FONT_SIZE_16X16]; 
            // 写入数据 
            OLED_WriteSingle_Data(data); 
        } 
    } 
    return OP_SUCCESS; 
} 
 
/** 
 * @brief 在指定位置显示指定字体大小的字符 
 * @param page 显示页面 
 * @param col 显示列位置 
 * @param ch 要显示的字符 
 * @param font_size 字体大小 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */ 
OperationResult oled_show_pagemode_char(uint8_t page, uint8_t col, uint8_t ch, uint8_t font_size) { 
    // 检查输入的字符是否为可打印的 ASCII 字符 
    if (ch < ASCII_OFFSET) { 
        // 如果字符小于 ASCII 可打印字符的起始值，返回操作失败 
        return OP_FAILURE; 
    } 
 
    // 计算字符在字库数组中的索引 
    uint8_t char_index = ch - ASCII_OFFSET; 
 
    switch (font_size) { 
        case OLED_FONT_SIZE_8X6: 
            return oled_show_pagemode_char_8x6(page, col, char_index); 
        case OLED_FONT_SIZE_16X8: 
            return oled_show_pagemode_char_16x8(page, col, char_index); 
        case OLED_FONT_SIZE_16X16: 
            return oled_show_pagemode_char_16x16(page, col, char_index); 
        default: 
            // 输入其他的字体大小 -- 默认输出 8x6 
            return oled_show_pagemode_char_8x6(page, col, char_index); 
    } 
} 




/** 
 * @brief 在指定位置显示 font大小的字符串 
 * @param page 显示页面 
 * @param col 显示列位置 
 * @param ch 要显示的字符 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */ 
OperationResult oLED_Show_pagemode_String(uint8_t page, uint8_t col, const uint8_t *string, uint8_t FontSize) 
{
    OperationResult Operation_state = OP_SUCCESS;  
    for(uint8_t strindex = 0;string[strindex] !='\0';strindex++)
    {
        Operation_state = oled_show_pagemode_char(page,col+(strindex*FontSize),string[strindex],FontSize);
        if(Operation_state != OP_SUCCESS)
            return OP_FAILURE;
    }
    return OP_SUCCESS;
}
/** 
 * @brief 在指定位置显示中文字符串 
 * @param page 显示页面 
 * @param col 显示列位置 
 * @param p_string_zh 要显示的中文字符串数据指针 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 * @note  UTF-8下执行：1个中文字符->4byte 
 */ 
OperationResult oLED_Show_PageMode_Chinese(uint8_t page, uint8_t col,const char *pString_zh){

    OperationResult operation_state = OP_SUCCESS;
    // 存储一个中文字符的ASCII码，UTF-8下一个中文字符占3个字节 
    uint8_t single_zh_font_array[UTF8_ZH_FONT_SIZE] = {0}; 
    // single_zh_font_array数组的索引 
    uint8_t single_zh_font_index = 0; 
    // 字库数组的索引 
    uint8_t str_index = 0; 
    // 当前列偏移量 
    uint8_t col_offset = col; 
    /*for循环4次：每次存储1个中文字符->4byte[每次对比4个byte：即一个中文字符进行匹配]*/
    for(uint8_t zh_temp_ascii_index = 0;pString_zh[zh_temp_ascii_index]!='\0';zh_temp_ascii_index++){
        //将当前字符存入single_zh_font_array数组
        single_zh_font_array[single_zh_font_index] = pString_zh[zh_temp_ascii_index];
        
        //printf("%c",single_zh_font_array[single_zh_font_index]);
        single_zh_font_index++;
        //printf("%d",single_zh_font_index);
        // 当single_zh_font_array数组存满一个中文字符（4个字节）时 
        if(single_zh_font_index >= UTF8_ZH_FONT_SIZE){
            // 重置single_zh_font_array数组的索引 
            single_zh_font_index = 0;
            //printf("%s",single_zh_font_array);

            /*遍历字库：中文字符与字库的zh_font进行每行匹配--到字库尾部停止*/
            for(str_index = 0;strcmp((const char*)OLED_ZH_F16x16[str_index].zh_font,"") != 0;str_index++){
                /*匹配中文字符:如果匹配成功，停止遍历字库*/
                if(strcmp((const char*)OLED_ZH_F16x16[str_index].zh_font,(const char*)single_zh_font_array) == 0){
                    break;
                }
            }
            // 显示匹配的中文字符 
            operation_state = oLED_Show_PageMode_Image(page, col_offset, OLED_FONT_SIZE_16X16, OLED_FONT_SIZE_16X16, OLED_ZH_F16x16[str_index].Data);
            if (operation_state != OP_SUCCESS) { 
                return OP_FAILURE; 
            }
            // 每次显示完一个中文字符，列位置偏移一个字符的宽度 
            col_offset += OLED_FONT_SIZE_16X16;   

        }
    }
    return OP_SUCCESS;
}

/**************************************************无page限制显示图形函数部分******************************************** */ 
/** 
 * @note: 利用 IIC 的连写特性 
 * @brief 通过 I2C 接口向 OLED 发送数据或命令。 
 *        该函数使用 I2C 协议与 OLED 进行通信，发送设备地址、模式（数据或命令）和具体的数据， 
 *        并根据 I2C 的应答状态判断发送是否成功。 
 * @param data 要发送的数据或命令的指针。 
 * @param mode 发送模式，指定是数据还是命令。 
 * @param Cont_len 要发送的数据长度。 
 * @retval OP_SUCCESS 发送成功；OP_FAILURE 发送失败。 
 */ 
static OperationResult OLED_Cont_Write(const uint8_t *data, uint8_t mode, uint8_t Cont_len) { 
    // 初始化操作结果为失败 
    volatile OperationResult Operation_state = OP_FAILURE; 
    
    // 检查传入的指针是否为空 
    if (data == NULL || Cont_len > COL_MAX) { 
        // 若指针为空，直接返回失败 
        return OP_FAILURE; 
    } 
 
    // 为发送缓冲区分配内存，长度为数据长度加 1（用于存储模式） 
    uint8_t *sendbuffer = (uint8_t *)malloc(Cont_len + 1); 
    if (sendbuffer == NULL) { 
        // 内存分配失败，返回失败 
        return OP_FAILURE; 
    } 
 
    // 将模式存储在发送缓冲区的第一个位置 
    sendbuffer[0] = mode; 
    // 复制数据到发送缓冲区 
    for (uint8_t i = 0; i < Cont_len; i++) { 
        sendbuffer[i + 1] = data[i]; 
    } 
 

        
    #if IIC_SOFTWARE_ENABLE 
        // 初始化 I2C 写状态为 ACK 
        IIC_State IIC_write_state = IIC_ACK; 
 
        // 发起 I2C 起始信号 
        IIC_start(); 
        // 发送 OLED 设备地址 
        IIC_send_byte(OLED_ADRESS); 
        // 接收 I2C 应答信号 
        IIC_write_state = IIC_receive_ACK(); 
        if (IIC_write_state != IIC_ACK) { 
            // 若未收到 ACK，停止 I2C 通信并返回失败 
            IIC_stop(); 
            HANDLE_ERROR(); 
        } 
 
        // 循环发送数据 
        for (uint8_t cont = 0; cont < Cont_len + 1; cont++) { 
            // 发送一个字节的数据 
            IIC_send_byte(sendbuffer[cont]); 
            // 接收 I2C 应答信号 
            IIC_write_state = IIC_receive_ACK(); 
            if (IIC_write_state != IIC_ACK) { 
                // 若未收到 ACK，停止 I2C 通信并返回失败 
                IIC_stop(); 
                HANDLE_ERROR(); 
            } 
        } 
        // 发送 I2C 停止信号 
        IIC_stop(); 
        // 若发送成功，将操作结果设置为成功 
        Operation_state = OP_SUCCESS; 
    #endif 
 
    #if IIC_HARDWARE_ENABLE 
        // 使用硬件I2C发送数据 
        HAL_StatusTypeDef hal_status = oled_IIC_HardwareSwapByte(OLED_ADRESS, (uint8_t * ) sendbuffer, Cont_len + 1); 
        // 如果硬件发送成功，返回成功结果 
        if (hal_status == HAL_OK) { 
            Operation_state = OP_SUCCESS; 
        } else { 
            // 打印错误信息 
            printf("error \r\n"); 
            Operation_state = OP_FAILURE; 
        } 
    #endif 
 
    #if SPI_SOFTWARE_ENABLE 
        // 启动 SPI 通信 
        oled_spi_Start(); 
        // 先发送写命令/数据模式 
        OLED_SPI_DC(mode); 
        // 循环发送数据 
        for (uint8_t i = 0; i < Cont_len; i++) { 
            oled_spiSoftwareSwapByte(data[i]); 
        } 
        // 停止 SPI 通信 
        oled_spi_Stop(); 
        // 假设 SPI 发送成功，将操作结果设置为成功 
        Operation_state = OP_SUCCESS; 
    #endif 
 
    #if SPI_HARDWARE_ENABLE 
        // 启动 SPI 通信 
        OLED_HARDWARE_SPI_CS(0); 
            OLED_HARDWARE_SPI_DC(mode); 
            oled_spiHardwareSwapByte(sendbuffer, Cont_len + 1); 
            OLED_HARDWARE_SPI_DC(1); 
        // 停止 SPI 通信 
        OLED_HARDWARE_SPI_CS(1); 
        // 假设 SPI 发送成功，将操作结果设置为成功 
        Operation_state = OP_SUCCESS; 
    #endif 
 
    // 释放发送缓冲区的内存 
    free(sendbuffer); 
 
    return Operation_state; 
} 
/** 
 * @brief 向 OLED 发送数据 
 * @param write_data 要发送的数据指针 
 * @param Cont_len 要发送的数据长度 
 * @retval OP_SUCCESS 发送成功；OP_FAILURE 发送失败 
 */ 
static OperationResult OLED_WriteCont_Data(const uint8_t *write_data,uint8_t Cont_len){
    return OLED_Cont_Write(write_data, WRITE_DATA_MODE,Cont_len); 
}

static OperationResult OLED_WriteCont_Command(const uint8_t  *write_data, uint8_t Cont_len){
    return OLED_Cont_Write(write_data, WRITE_COMMAND_MODE,Cont_len);
}

/** 
 * @brief 设置OLED光标位置。 
 *        该函数根据传入的页面和列位置，通过发送相应的命令来设置OLED的光标位置。 
 * @param page 要设置的页面，范围从0到PAGE_MAX - 1。 
 * @param col 要设置的列位置，范围从0到COL_MAX - 1。 
 * @retval OP_SUCCESS 设置成功；OP_FAILURE 设置失败。 
 */ 
static OperationResult OLED_Cont_SetCursor(uint8_t page, uint8_t col) 
{ 
    uint8_t command_offset[3] ={
        (0x00 | (col & 0x0F)), // 用于设置列的低四位
        (0x10 | ((col & 0xF0) >> 4)), // 用于设置列的高四位
        (0xB0 | ((page) & 0x07))  // 用于设置页的低四位
    }; 
    // 初始化操作结果为成功 
    OperationResult Operation_state = OP_SUCCESS; 
    // 检查页面是否越界 
    if(page >= PAGE_MAX ||  col >= COL_MAX)
    { 
        
        // 如果行或列越界，返回失败结果
        return OP_FAILURE; 
    }

    Operation_state = OLED_WriteCont_Command(command_offset,3); 

    // 如果操作失败，返回失败结果 
    if (Operation_state != OP_SUCCESS) 
    { 
        printf("OLED_Cont_SetCursor failed!\r\n");
        return OP_FAILURE; 
    } 
    // 所有操作成功，返回成功结果 
    return OP_SUCCESS; 
} 
/**
  * 函    数：将OLED显存数组更新到OLED屏幕
  * 参    数：无
  * 返 回 值：无
  * 说    明：所有的显示函数，都只是对OLED显存数组进行读写
  *           随后调用OLED_Update函数或OLED_UpdateArea函数
  *           才会将显存数组的数据发送到OLED硬件，进行显示
  *           故调用显示函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
OperationResult OLED_UpdateToGRAM_ALL(void)
{

    OperationResult operation_state = OP_SUCCESS;
	/*遍历每一页*/
	for (uint8_t page_index = 0; page_index < PAGE_MAX; page_index++){
        /*设置光标位置为每一页的第一列*/
		operation_state = OLED_Cont_SetCursor(page_index, 0);
        operation_state = OLED_WriteCont_Data((const uint8_t*)gOLED_gram_buffer[page_index],COL_MAX);
        //printf("page_index:%d\r\n",page_index);
        // 如果设置光标位置失败，返回失败结果
        if(operation_state !=OP_SUCCESS)
        {
            return OP_FAILURE;
        }
    }
    return OP_SUCCESS;
}

/** 
 * @brief 清除OLED屏幕指定区域的内容 
 * 
 * 该函数用于清除OLED屏幕上指定区域的内容，通过将该区域对应的显存缓冲区的位清零来实现。 
 * 
 * @param row 要清除区域的起始行位置 
 * @param col 要清除区域的起始列位置 
 * @param clear_width 要清除区域的宽度（列数） 
 * @param clear_height 要清除区域的高度（行数） 
 * @return OperationResult 操作结果，OP_SUCCESS表示操作成功，OP_FAILURE表示操作失败 
 */ 
OperationResult oLED_Clear_Area(uint8_t row, uint8_t col, uint8_t clear_width, uint8_t clear_height) 
{ 
    // 具体到页内的那个像素点 
    uint8_t page_mark = 0; 
    // 存储坐标转换到page 
    uint8_t page_index = 0; 
 
    // 进行越界判断 
    // 检查列位置和图像宽度是否超出最大宽度 
    if (col + clear_width > COL_MAX) { 
        return OP_FAILURE; 
    } 
    // 检查行位置和图像高度是否超出最大行数 
    if (row + clear_height > ROW_MAX) { 
        return OP_FAILURE; 
    } 
 
    // 遍历要清除的区域 
    for (uint8_t row_index = row; row_index < row + clear_height; row_index++) { 
        // 页数向上取整 
        page_index = (row_index + PAGE_SIZE - 1) / PAGE_SIZE; 
        page_mark = row_index % PAGE_SIZE; 
 
        // 清除该点对应的显存位 
        for (uint8_t col_index = col; col_index < col + clear_width; col_index++) { 
            gOLED_gram_buffer[page_index][col_index] &= ~(1<< page_mark); 
        } 
    } 
 
    return OP_SUCCESS; 
} 

/** 
 * @brief 在指定位置显示图形 
 * @param start_x 显示列位置，使用绝对坐标 (0, 0) 作为参考 
 * @param image_width 图像宽度，用于确定图像在列方向的像素数量 
 * @param image_height 图像高度，用于确定图像在行方向的像素数量，结合 PAGE_SIZE 计算图像所占的页数 
 * @param Img 要显示的图像数据指针，指向存储图像数据的数组，数组大小由图像宽度和高度决定 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */ 
OperationResult oLED_Show_offset_Image(uint8_t start_y, uint8_t start_x, uint8_t image_width, uint8_t image_height, const uint8_t *Img) { 
    // 步骤 1: 坐标转换 
    // 计算图像起始的页号。在 OLED 显示中，通常将显示区域按页划分，每页包含 PAGE_SIZE 个像素行。 
    // 这里通过 start_y 除以 PAGE_SIZE 得到起始页号，因为 start_y 是像素行的起始位置。 
    uint8_t start_page = start_y / PAGE_SIZE; 
    // 计算起始位置相对于起始页的偏移量。start_y 对 PAGE_SIZE 取余得到在起始页内的像素行偏移。 
    uint8_t shift_offset = start_y % PAGE_SIZE; 
    // 计算图像结束的页号。通过 (start_y + image_height - 1) 计算图像结束的像素行位置，再除以 PAGE_SIZE 得到结束页号。 
    uint8_t end_page = (start_y + image_height - 1) / PAGE_SIZE; 
    // 计算图像结束的列号。通过 start_x 加上图像宽度得到结束列号。 
    uint8_t end_col = start_x + image_width; 
 
    // 步骤 2: 越界检查（含负坐标裁剪） 
    // 检查图像是否超出显示区域的边界。如果结束列号超过最大列数 COL_MAX， 
    // 或者结束页号超过最大行数 ROW_MAX，或者起始列号超过最大列数， 
    // 或者起始行号超过最大行数乘以 PAGE_SIZE，说明图像会超出显示区域，返回 OP_FAILURE。 
    if (end_col > COL_MAX || end_page >= ROW_MAX || start_x >= COL_MAX || start_y >= ROW_MAX * PAGE_SIZE) 
        return OP_FAILURE; 
 
    // 步骤 3: 清屏区域 
    // 调用 oLED_Clear_Area 函数清除指定区域，该区域由起始位置、图像宽度和高度确定。 
    // 保存清除操作的结果到 state 变量。 
    OperationResult state = oLED_Clear_Area(start_y, start_x, image_width, image_height); 
    // 如果清除操作失败，直接返回该结果。 
    if (state != OP_SUCCESS) return state; 
 
    // 步骤 4: 逐页逐列写入 
    // 外层循环遍历从起始页到结束页的每一页。 
    for (uint8_t page_index = start_page; page_index < end_page + 1; page_index++) { 
        // 内层循环遍历从起始列到结束列的每一列。 
        for (uint8_t col_index = start_x; col_index < end_col; col_index++) { 
            // 计算当前【单个像素】在图像数据中的索引。 
            // 通过 (page_index - start_page) 得到当前页相对于起始页的偏移，乘以图像宽度得到【页偏移的像素数】， 
            // 再加上 (col_index - start_x) 得到列偏移的像素数，从而确定在【图像数据中的索引】。 
            uint16_t source_byte_index = (page_index - start_page) * image_width + (col_index - start_x); 
 
            // 越界检查：判断计算得到的图像数据索引是否超出了图像数据的有效范围 
            // image_width * ((image_height + PAGE_SIZE - 1) / PAGE_SIZE) 计算的是图像数据的总字节数 
            // 这里先计算 (image_height + PAGE_SIZE - 1) / PAGE_SIZE 得到图像所占的总页数 
            // 再乘以 image_width 得到总字节数 
            // 如果 source_byte_index 大于或等于这个总字节数，说明索引超出了图像数据的范围 
            if (source_byte_index >= image_width * ((image_height + PAGE_SIZE - 1) / PAGE_SIZE)) { 
                // 若索引越界，跳过当前像素的写入操作，继续处理下一个像素 
                // 这样可以避免访问无效的内存地址，保证程序的稳定性 
                continue; 
            } 
 
            // 超出边界，则跳过显示 
            if (col_index >= COL_MAX - 1) break; 
            if (page_index >= ROW_MAX - 1) return OP_FAILURE; 
 
            // 当前页写入 
            // 将图像数据左移 shift_offset 位，然后与当前页对应的 OLED 显存缓冲区的位置进行按位或操作， 
            // 实现将图像数据写入当前页。 
            gOLED_gram_buffer[page_index][col_index] |= Img[source_byte_index] << shift_offset; 
 
            // 跨页写入（需验证下一页有效性） 
            // 如果 shift_offset 大于 0，说明图像数据可能会跨页显示。 
            // 同时检查下一页是否有效（即 page_index + 1 小于 ROW_MAX）。 
            if (shift_offset > 0 && page_index + 1 < ROW_MAX) { 
                // 超出边界，则跳过显示 
                if (page_index + 1 >= ROW_MAX - 1) continue; 
                // 将图像数据右移 (PAGE_SIZE - shift_offset) 位，然后与下一页对应的 OLED 显存缓冲区的位置进行按位或操作， 
                // 实现跨页写入。 
                gOLED_gram_buffer[page_index + 1][col_index] |= Img[source_byte_index] >> (PAGE_SIZE - shift_offset); 
            } 
        } 
    } 
    // 所有操作成功完成，返回 OP_SUCCESS。 
    return OP_SUCCESS; 
} 


/** 
 * @brief 在指定位置显示中文字符串 
 * @param row 显示页面 
 * @param col 显示列位置 
 * @param p_string_zh 要显示的中文字符串数据指针 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 * @note  UTF-8下执行：1个中文字符->4byte 
 */ 
OperationResult oLED_Show_offset_Chinese(uint8_t row, uint8_t col,const char *pString_zh){
    OperationResult operation_state = OP_SUCCESS;
    // 存储一个中文字符的ASCII码，UTF-8下一个中文字符占3个字节 
    uint8_t single_zh_font_array[UTF8_ZH_FONT_SIZE] = {0}; 
    // single_zh_font_array数组的索引 
    uint8_t single_zh_font_index = 0; 
    // 字库数组的索引 
    uint8_t str_index = 0; 
    // 当前列偏移量 
    uint8_t col_offset = col; 
    /*for循环4次：每次存储1个中文字符->4byte[每次对比4个byte：即一个中文字符进行匹配]*/
    for(uint8_t zh_temp_ascii_index = 0;pString_zh[zh_temp_ascii_index]!='\0';zh_temp_ascii_index++){
        //将当前字符存入single_zh_font_array数组
        single_zh_font_array[single_zh_font_index] = pString_zh[zh_temp_ascii_index];
        
        //printf("%c",single_zh_font_array[single_zh_font_index]);
        single_zh_font_index++;
        //printf("%d",single_zh_font_index);
        // 当single_zh_font_array数组存满一个中文字符（4个字节）时 
        if(single_zh_font_index >= UTF8_ZH_FONT_SIZE){
            // 重置single_zh_font_array数组的索引 
            single_zh_font_index = 0;
            //printf("%s",single_zh_font_array);

            /*遍历字库：中文字符与字库的zh_font进行每行匹配--到字库尾部停止*/
            for(str_index = 0;strcmp((const char*)OLED_ZH_F16x16[str_index].zh_font,"") != 0;str_index++){
                /*匹配中文字符:如果匹配成功，停止遍历字库*/
                if(strcmp((const char*)OLED_ZH_F16x16[str_index].zh_font,(const char*)single_zh_font_array) == 0){
                    break;
                }
            }
            // 显示匹配的中文字符 
            operation_state = oLED_Show_offset_Image(row, col_offset, OLED_FONT_SIZE_16X16, OLED_FONT_SIZE_16X16, OLED_ZH_F16x16[str_index].Data);
            if (operation_state != OP_SUCCESS) { 
                return OP_FAILURE; 
            }
            // 每次显示完一个中文字符，列位置偏移一个字符的宽度 
            col_offset += OLED_FONT_SIZE_16X16;   

        }
    }
    return OP_SUCCESS;
}

/** 
 * @brief 在指定位置显示字符 
 * @param row 显示页面 
 * @param col 显示列位置 
 * @param ch 要显示的字符 
 * @param font_size 字体大小 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */
OperationResult oled_show_offset_char(uint8_t row, uint8_t col, uint8_t ch, uint8_t font_size) { 
    // 检查输入的字符是否为可打印的 ASCII 字符 
    if (ch < ASCII_OFFSET) { 
        // 如果字符小于 ASCII 可打印字符的起始值，返回操作失败 
        return OP_FAILURE; 
    } 
 
    // 计算字符在字库数组中的索引 
    uint8_t char_index = ch - ASCII_OFFSET; 
 
    switch (font_size) { 
        case OLED_FONT_SIZE_8X6: 
            return oLED_Show_offset_Image(row, col, OLED_FONT_SIZE_8X6, OLED_FONT_SIZE_8X6, OLED_F8x6[char_index]); 
        case OLED_FONT_SIZE_16X8: 
            return oLED_Show_offset_Image(row, col, OLED_FONT_SIZE_16X8, OLED_FONT_SIZE_16X8*2, OLED_F16x8[char_index]); 
        case OLED_FONT_SIZE_16X16: 
            return oLED_Show_offset_Image(row, col, OLED_FONT_SIZE_16X16, OLED_FONT_SIZE_16X16, OLED_F16x16[char_index]); 
        default: 
            // 输入其他的字体大小 -- 默认输出 8x6 
            return oLED_Show_offset_Image(row, col, OLED_FONT_SIZE_8X6, OLED_FONT_SIZE_8X6, OLED_F8x6[char_index]); 
    } 
} 




/** 
 * @brief 在指定位置显示指定字体大小的字符串 
 * @param row 显示页面 
 * @param col 显示列位置 
 * @param string 要显示的字符串 
 * @param FontSize 字体大小 
 * @return 操作结果，成功返回 OP_SUCCESS，失败返回 OP_FAILURE 
 */
OperationResult oLED_Show_offset_String(uint8_t row, uint8_t col, const uint8_t *string, uint8_t FontSize) {
    OperationResult Operation_state = OP_SUCCESS;  
    for(uint8_t strindex = 0;string[strindex] !='\0';strindex++)
    {
        Operation_state = oled_show_offset_char(row,col+(strindex*FontSize),string[strindex],FontSize);
        if(Operation_state != OP_SUCCESS)
            return OP_FAILURE;
    }
    return OP_SUCCESS;
}

/*****************************************************工具函数仅供内部部分函数使用*********************************************/

/** 
 * @brief 计算 base 的 exponent 次方 
 * @param base 底数 
 * @param exponent 指数 
 * @return 计算结果 
 * @note 该函数用于计算整数的幂，适用于正整数的指数 
 */
static uint32_t calculatePower(uint32_t base, uint32_t exponent) { 
    // 初始化结果为 1，因为任何数的 0 次方都为 1 
    uint32_t result = 1; 
    // 循环 exponent 次，进行累乘操作 
    while (exponent--) { 
        // 每次将底数 base 累乘到结果上 
        result *= base; 
    } 
    return result; 
} 

/** 
 * @brief 在 OLED 上显示数字 
 * @param page 显示页面 
 * @param col 显示列位置 
 * @param number 要显示的数字 
 * @param digitCount 数字的位数 
 * @param fontSize 字体大小 
 * @note 该函数将数字转换为字符并在 OLED 上显示，适用于显示整数。 
 */
void displayNumberOnOLED(uint16_t page, uint16_t col, uint32_t number, uint8_t digitCount, uint8_t fontSize) { 
    // 遍历数字的每一位 
    for (uint8_t i = 0; i < digitCount; i++) { 
        // 提取数字的第 i 位 
        // number / calculatePower(10, digitCount - i - 1) % 10 用于十进制提取数字的每一位 
        // + '0' 可将数字转换为字符格式 
        uint8_t digit = number / calculatePower(10, digitCount - i - 1) % 10 + '0'; 
        // 在 OLED 上显示该字符
        //注意：这里一定要用page函数，直接写入GRAM中，否则会有延迟
        oled_show_pagemode_char(page,col+ i * fontSize, digit, fontSize); 
    } 
}


