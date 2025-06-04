#include "key.h"



void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
  
  /*Configure GPIO pins : KEY2_Pin KEY1_Pin KEY0_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin|KEY1_Pin|KEY0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  /*Configure GPIO pin : WK_UP_Pin */
  GPIO_InitStruct.Pin = WK_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(WK_UP_GPIO_Port, &GPIO_InitStruct);
  
  
}

/*键值获取函数*/
uint8_t key_scan(uint8_t mode)
{
  /* 按键松开标志*/
  static uint8_t key_up = 1;
  /* 键值*/
  uint8_t keyval = 0;

  if(mode)        /*不检查是否松开*/
  {
    key_up = 1;
  }
  if(key_up&&(KEY0 == 0||KEY1 == 0|| KEY2 == 0||WK_UP == 1))
  {
    HAL_Delay(10);  /*防抖动*/
    key_up = 0;     /*按键按下*/
    if (KEY0 == 0)
    {
      keyval = KEY0_PRES;
    }
    if (KEY1 == 0)
    {
      keyval = KEY1_PRES;
    }
    if (KEY2 == 0)
    {
      keyval = KEY2_PRES;
    }
    if (WK_UP == 1)
    {
      keyval = WK_UP_PRES;
    }
  }
  else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)
  {
    key_up = 1;
  }
  return keyval;
  
}

uint8_t key = 0;
void key_text(void)
{
    while(1)
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            LED0_TOGGLE();
        }
        if(key == KEY1_PRES)
        {
            LED1_TOGGLE();
        }
        if(key == KEY2_PRES)
        {
            LED0(1);
            LED1(0);
        }
        if(key == WK_UP_PRES)
        {
            LED0_TOGGLE();
            LED1_TOGGLE();
        }
        HAL_Delay(50);
    }
}
