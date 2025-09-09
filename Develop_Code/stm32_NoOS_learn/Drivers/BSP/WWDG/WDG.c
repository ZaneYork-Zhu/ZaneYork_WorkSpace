#include "WDG.h"
#include "led.h"

WWDG_HandleTypeDef hwwdg;





/**
 * @brief       ��ʼ�����ڿ��Ź�
 * @param       tr: T[6:0],������ֵ
 * @param       tw: W[6:0],����ֵ
 * @note        fprer:��Ƶϵ����WDGTB��,��Χ:WWDG_PRESCALER_1~WWDG_PRESCALER_8,��ʾ2^WDGTB��Ƶ
 *              Fwwdg=PCLK1/(4096*2^fprer). һ��PCLK1=42Mhz
 * @retval      ��
 */
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer)
{


    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = fprer;         /* ���÷�Ƶϵ�� */
    hwwdg.Init.Window = wr;               /* ���ô���ֵ */
    hwwdg.Init.Counter = tr;              /* ���ü�����ֵ */
    hwwdg.Init.EWIMode = WWDG_EWI_ENABLE; /* ʹ�ܴ��ڿ��Ź���ǰ�����ж� */
    if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
    {
    Error_Handler();
    }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

}

/**
 * @brief       WWDG�ײ�������ʱ�����ã��ж�����
                �˺����ᱻHAL_WWDG_Init()����
 * @param       hwwdg:���ڿ��Ź����
 * @retval      ��
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
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 3);  /* ��ռ���ȼ�2�������ȼ�Ϊ3 */
    HAL_NVIC_EnableIRQ(WWDG_IRQn);          /* ʹ�ܴ��ڿ��Ź��ж� */
  /* USER CODE END WWDG_MspInit 1 */
  }
}

/**
 * @brief       ���ڿ��Ź��жϷ������
 * @param       ��
 * @retval      ��
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&hwwdg);     /* ����WWDG�����жϴ����� */
}

/**
 * @brief       �жϷ������������
                �˺����ᱻHAL_WWDG_IRQHandler()����
 * @param       ��
 * @retval      ��
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg1)
{
    HAL_WWDG_Refresh(&hwwdg);        /* ���´��ڿ��Ź�ֵ */
    LED1_TOGGLE();                          /*  �̵���˸ */
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
