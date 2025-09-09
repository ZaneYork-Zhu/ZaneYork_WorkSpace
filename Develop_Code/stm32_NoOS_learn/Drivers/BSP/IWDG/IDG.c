#include "IDG.h"
#include "led.h"
#include "key.h"

IWDG_HandleTypeDef iwdg_handler; /*�������Ź���� */

void iwdg_text_init(void)
{
    led_init();                             /* ��ʼ��LED */
    key_init();                             /* ��ʼ������ */
    HAL_Delay(100);                          /* ��ʱ100ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�" */
    iwdg_init(IWDG_PRESCALER_64, 500);      /* Ԥ��Ƶ��Ϊ64,����ֵΪ500,���ʱ��ԼΪ1s */
    LED0(0);                                /* ����LED0(���) */    
}

void iwdg_text_while(void)
{

   if (key_scan(1) == WK_UP_PRES)        /* ���WK_UP����,��ι��,֧������ */
    {
        iwdg_feed();                    /* ι�� */
    }

    HAL_Delay(10);
}

/**
 * @brief       ��ʼ���������Ź� 
 * @param       prer: IWDG_PRESCALER_4~IWDG_PRESCALER_256,��Ӧ4~256��Ƶ
 *   @arg       ��Ƶ���� = 4 * 2^prer. �����ֵֻ����256!
 * @param       rlr: �Զ���װ��ֵ,0~0XFFF. 
 * @note        ʱ�����(���):Tout=((4 * 2^prer) * rlr) / 32 (ms). 
 * @retval      ��
 */
void iwdg_init(uint32_t prer, uint16_t rlr)
{
    iwdg_handler.Instance = IWDG;
    iwdg_handler.Init.Prescaler = prer; /* ����IWDG��Ƶϵ�� */
    iwdg_handler.Init.Reload = rlr;     /* �Ӽ��ؼĴ��� IWDG->RLR ��װ��ֵ */
    HAL_IWDG_Init(&iwdg_handler);       /* ��ʼ��IWDG��ʹ�� */
}

/**
 * @brief       ι�������Ź�
 * @param       ��
 * @retval      ��
 */
void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&iwdg_handler);    /* ι�� */
}
