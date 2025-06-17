#include "key.h" 
#include "led.h"
 
// 按键初始化函数 
// 此函数用于初始化按键对应的GPIO引脚 
// 配置按键的触发模式（下降沿或上升沿触发）和上拉/下拉电阻 
void Key_Init(void) { 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
 
    // 使能KEY0、KEY1、KEY2和WK_UP对应的GPIO时钟
    __KEY2_GPIO_CLK_ENABLE(); 
    __KEY1_GPIO_CLK_ENABLE(); 
    __KEY0_GPIO_CLK_ENABLE();
    __WK_UP_GPIO_CLK_ENABLE();
 
    // 配置KEY0、KEY1、KEY2引脚 
    // 这三个按键为下降沿触发，上拉电阻 
    {
        GPIO_InitStruct.Pin = KEY2_Pin; 
        GPIO_InitStruct.Mode = KEY2_GPIO_Mode; 
        GPIO_InitStruct.Pull = KEY2_GPIO_Pull; 
        HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct); 

        GPIO_InitStruct.Pin = KEY1_Pin; 
        GPIO_InitStruct.Mode = KEY1_GPIO_Mode; 
        GPIO_InitStruct.Pull = KEY1_GPIO_Pull; 
        HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct); 

        GPIO_InitStruct.Pin = KEY0_Pin; 
        GPIO_InitStruct.Mode = KEY0_GPIO_Mode; 
        GPIO_InitStruct.Pull = KEY0_GPIO_Pull; 
        HAL_GPIO_Init(KEY0_GPIO_Port, &GPIO_InitStruct); 
    }
 
    // 配置WK_UP引脚 
    // WK_UP按键为上升沿触发，下拉电阻 
    GPIO_InitStruct.Pin = WK_UP_Pin; 
    GPIO_InitStruct.Mode = WK_UP_GPIO_Mode; 
    GPIO_InitStruct.Pull = WK_UP_GPIO_Pull; 
    HAL_GPIO_Init(WK_UP_GPIO_Port, &GPIO_InitStruct); 
} 
 
// 键值获取函数 
// mode为1时不检查按键是否松开，为0时检查 
// 返回值为按下的按键对应的键值 
uint8_t Key_Scan(uint8_t mode) { 
    // 按键松开标志，静态变量，用于记录按键状态 
    static uint8_t key_up = 1; 
    // 键值 
    uint8_t keyval = 0; 
 
    // 如果mode为1，不检查按键是否松开，将key_up置为1 
    if (mode) { 
        key_up = 1; 
    } 
 
    // 检查是否有按键按下 
    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1)) { 
        // 消抖处理，延时10ms 
        HAL_Delay(10); 
        // 按键已按下，将key_up置为0 
        key_up = 0; 
 
        // 判断具体是哪个按键按下 
        if (KEY0 == 0)        { keyval = KEY0_PRES; } 
        else if (KEY1 == 0)   { keyval = KEY1_PRES; } 
        else if (KEY2 == 0)   { keyval = KEY2_PRES; } 
        else if (WK_UP == 1)  { keyval = WK_UP_PRES; } 
    } 
    // 检查所有按键是否都已松开 
    else if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0) { 
        // 按键已松开，将key_up置为1 
        key_up = 1; 
    } 
 
    return keyval; 
} 
 
// 按键测试函数 
// 此函数用于循环检测按键状态，并根据不同的按键执行相应的操作 
void Key_Test(void) { 
    uint8_t key = 0; 
 
    while (1) { 
        // 扫描按键，不检查按键是否松开 
        key = Key_Scan(0); 
 
        // 根据不同的按键执行相应的操作 
        switch (key) { 
            case KEY0_PRES: { LED0_TOGGLE(); break;} 
            case KEY1_PRES: { LED1_TOGGLE(); break;} 
            case KEY2_PRES: { for(uint8_t i = 0;i<5;i++){ LED0(1); HAL_Delay(500);LED0(0);HAL_Delay(500);} break;} 
            case WK_UP_PRES:{ LED0_TOGGLE(); LED1_TOGGLE(); break;} 
            default: break; 
        } 
 
        // 延时50ms 
        HAL_Delay(50); 
    } 
} 
