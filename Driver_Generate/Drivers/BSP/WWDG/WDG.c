#include "WDG.h"
#include "led.h"

WWDG_HandleTypeDef hwwdg;





/**
 * @brief       初始化窗口看门狗
 * @param       tr: T[6:0],计数器值
 * @param       tw: W[6:0],窗口值
 * @note        fprer:分频系数（WDGTB）,范围:WWDG_PRESCALER_1~WWDG_PRESCALER_8,表示2^WDGTB分频
 *              Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
 * @retval      无
 */
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer)
{


    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = fprer;         /* 设置分频系数 */
    hwwdg.Init.Window = wr;               /* 设置窗口值 */
    hwwdg.Init.Counter = tr;              /* 设置计数器值 */
    hwwdg.Init.EWIMode = WWDG_EWI_ENABLE; /* 使能窗口看门狗提前唤醒中断 */
    if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
    {
    Error_Handler();
    }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

}

/**
 * @brief       WWDG底层驱动，时钟配置，中断配置
                此函数会被HAL_WWDG_Init()调用
 * @param       hwwdg:窗口看门狗句柄
 * @retval      无
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef* wwdgHandle)
{

  if(wwdgHandle->Instance==WWDG)
  {
  /* USER CODE BEGIN WWDG_MspInit 0 */

  /* USER CODE END WWDG_MspInit 0 */
    /* WWDG clock enable */
    __HAL_RCC_WWDG_CLK_ENABLE();
    /* USER CODE BEGIN WWDG_MspInit 1 */
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 3);  /* 抢占优先级2，子优先级为3 */
    HAL_NVIC_EnableIRQ(WWDG_IRQn);          /* 使能窗口看门狗中断 */
  /* USER CODE END WWDG_MspInit 1 */
  }
}

/**
 * @brief       窗口看门狗中断服务程序
 * @param       无
 * @retval      无
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&hwwdg);     /* 调用WWDG共用中断处理函数 */
}

/**
 * @brief       中断服务函数处理过程
                此函数会被HAL_WWDG_IRQHandler()调用
 * @param       无
 * @retval      无
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg1)
{
    HAL_WWDG_Refresh(&hwwdg);        /* 更新窗口看门狗值 */
    LED1_TOGGLE();                          /*  绿灯闪烁 */
}


void wwdg_text_init(void)
{
  led_init();
  /* USER CODE END 2 */
  LED0(0);
  HAL_Delay(100);
  wwdg_init(0X7F, 0X5F, WWDG_PRESCALER_8);
    
    
}

void wwdg_text_while(void)
{
    while(1)
    {
        LED0(1);
    }
}
