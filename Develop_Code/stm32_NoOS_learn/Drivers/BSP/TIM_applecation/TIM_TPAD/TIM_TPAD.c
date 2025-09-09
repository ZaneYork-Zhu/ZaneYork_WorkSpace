#include "TIM_TPAD.h"
#include "led.h"
#include "debug_com.h"


uint32_t threshold_cap_val = 0;     /* [TPADδ����]�����ֵʱ�� */
uint32_t TPAD_cap_val = 0;          /* [TPAD����]���ʱ�� */

// �жϻص�������ʹ�õ�ȫ�ֱ��� 
volatile uint8_t capture_flag = 0;

/************************************************************����������********************************************* */
// ѭ���������� 
uint8_t t = 0;

/** 
 * @brief �����������Գ�ʼ������ 
 *        �˺������ڳ�ʼ�����ڡ�����������LED�Ƶ����� 
 * @param �� 
 * @retval �� 
 */ 
void TPAD_text_init(void)
{
    // ��ʼ������ͨ�ţ�������Ϊ115200 
    UART_Init(115200); 
    // ��ӡ��ʾ��Ϣ����ʾTIM��ʼ���� 
    printf("TIM��ʼ����\r\n"); 
    // ��ʼ������������Ԥ��Ƶϵ��Ϊ8 
    tpad_init(8); 
    // ��ʼ��LED�� 
    led_init(); 
}
/** 
 * @brief ������������ѭ������ 
 *        �˺�������һ������ѭ�������ڲ���ɨ�败������������LED�Ƶ�״̬ 
 * @param �� 
 * @retval �� 
 */ 
void TPAD_text_while(void) 
{ 
    while (1) 
    { 
        /* USER CODE END WHILE */ 
        // ɨ�败������������ɹ�����һ�������� 
        if (tpad_scan(0))                    
        { 
            // ��תLED1��״̬ 
            LED1_TOGGLE();                  
        } 
 
        // ѭ������������1 
        t++; 
        // ��ѭ�������ﵽ15ʱ 
        if (t == 15) 
        { 
            // ѭ�������������� 
            t = 0; 
            // ��תLED0��״̬ 
            LED0_TOGGLE();                  
        } 
 
        // ��ʱ10ms 
        HAL_Delay(10); 
        /* USER CODE BEGIN 3 */ 
    } 
} 
/************************************************************���벶���ʼ������********************************************* */


// ��ʱ����� 
TIM_HandleTypeDef TIM_TPAD_Handle; 
 
/** 
 * @brief TIM2��ʼ������ 
 *        �˺������ڳ�ʼ��TIM2��ʱ��������ʱ��Դ�����벶��Ȳ��� 
 * @param TIM: ��ʱ��ʵ�� 
 * @param arr: �Զ���װ��ֵ 
 * @param psc: Ԥ��Ƶϵ�� 
 * @retval �� 
 */ 
void TIM_TPAD_init(TIM_TypeDef* TIM, uint32_t arr, uint16_t psc) 
{ 
    /* USER CODE BEGIN TIM2_Init 0 */ 
 
    /* USER CODE END TIM2_Init 0 */ 
 
    // ��ʱ��ʱ�����ýṹ�� 
    TIM_ClockConfigTypeDef sClockSourceConfig = {0}; 
    // ��ʱ����ģʽ���ýṹ�� 
    TIM_MasterConfigTypeDef sMasterConfig = {0}; 
    // ��ʱ�����벶�����ýṹ�� 
    TIM_IC_InitTypeDef sConfigIC = {0}; 
 
    /* USER CODE BEGIN TIM2_Init 1 */ 
 
    /* USER CODE END TIM2_Init 1 */ 
    // ָ����ʱ��ʵ�� 
    TIM_TPAD_Handle.Instance = TIM; 
    // ����Ԥ��Ƶϵ�� 
    TIM_TPAD_Handle.Init.Prescaler = psc; 
    // ���ü���ģʽΪ���ϼ��� 
    TIM_TPAD_Handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
    // �����Զ���װ��ֵ 
    TIM_TPAD_Handle.Init.Period = arr; 
    // ����ʱ�ӷ�ƵΪ1 
    TIM_TPAD_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
    // ʹ���Զ���װ��Ԥ���� 
    TIM_TPAD_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 
    // ��ʼ����ʱ���������ܣ����ʧ������ô������� 
    if (HAL_TIM_Base_Init(&TIM_TPAD_Handle) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // ���ö�ʱ��ʱ��ԴΪ�ڲ�ʱ�� 
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; 
    // Ӧ��ʱ��Դ���ã����ʧ������ô������� 
    if (HAL_TIM_ConfigClockSource(&TIM_TPAD_Handle, &sClockSourceConfig) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // ��ʼ����ʱ�����벶���ܣ����ʧ������ô������� 
    if (HAL_TIM_IC_Init(&TIM_TPAD_Handle) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // �������������Ϊ��λ 
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; 
    // ��������ģʽ 
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE; 
    // Ӧ����ģʽ���ã����ʧ������ô������� 
    if (HAL_TIMEx_MasterConfigSynchronization(&TIM_TPAD_Handle, &sMasterConfig) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
    // �������벶����Ϊ�����ش��� 
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING; 
    // �������벶��ѡ��Ϊֱ������ 
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; 
    // �������벶��Ԥ��ƵΪ1 
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1; 
    // �������벶���˲���Ϊ0 
    sConfigIC.ICFilter = 0; 
    // ���ö�ʱ��ͨ��1�����벶����������ʧ������ô������� 
    if (HAL_TIM_IC_ConfigChannel(&TIM_TPAD_Handle, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) 
    { 
        Error_Handler(); 
    } 
} 
 
/** 
 * @brief TIM2�ײ�Ӳ����ʼ������ 
 *        �˺�������ʹ�ܶ�ʱ��ʱ�ӡ�����GPIO���š���ʼ���жϵ� 
 * @param tim_baseHandle: ��ʱ����� 
 * @retval �� 
 */ 
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle) 
{ 
    // GPIO��ʼ���ṹ�� 
    GPIO_InitTypeDef GPIO_InitStruct = {0}; 
    // �ж϶�ʱ��ʵ���Ƿ�ΪTIM_TPAD_INT 
    if(tim_baseHandle->Instance == TIM_TPAD_INT) 
    { 
        /* USER CODE BEGIN TIM2_MspInit 0 */ 
 
        /* USER CODE END TIM2_MspInit 0 */ 
        // ʹ��TIM2ʱ�� 
        TIM_TPAD_INT_CLK_ENABLE(); 
 
        // ʹ�ܴ����������ʱ�� 
        __HAL_RCC_TAPD_CLK_ENABLE(); 
        /**TIM2 GPIO Configuration 
        PA5     ------> TIM2_CH1 
        */ 
        // ����GPIO���� 
        GPIO_InitStruct.Pin = TAPD_KEY_Pin; 
        // ����GPIOģʽΪ����������� 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; 
        // ��ʹ������������ 
        GPIO_InitStruct.Pull = GPIO_NOPULL; 
        // ����GPIO�ٶ�Ϊ�ǳ����� 
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // ����GPIO���ù��� 
        GPIO_InitStruct.Alternate = TAPD_KEY_Alternate; 
        // ��ʼ��GPIO���� 
        HAL_GPIO_Init(TAPD_KEY_GPIO_Port, &GPIO_InitStruct); 
 
        // ��ʼ��TIM2�ж� 
        HAL_NVIC_SetPriority(TIM_TPAD_INT_IRQn, 0, 0); 
        // ʹ��TIM2�ж� 
        HAL_NVIC_EnableIRQ(TIM_TPAD_INT_IRQn); 
        /* USER CODE BEGIN TIM2_MspInit 1 */ 
 
        /* USER CODE END TIM2_MspInit 1 */ 
    } 
} 
 
/** 
 * @brief TIM2�ײ�Ӳ������ʼ������ 
 *        �˺������ڽ��ö�ʱ��ʱ�ӡ�����ʼ��GPIO���š������жϵ� 
 * @param tim_baseHandle: ��ʱ����� 
 * @retval �� 
 */ 
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle) 
{ 
    // �ж϶�ʱ��ʵ���Ƿ�ΪTIM_TPAD_INT 
    if(tim_baseHandle->Instance == TIM_TPAD_INT) 
    { 
        /* USER CODE BEGIN TIM2_MspDeInit 0 */ 
 
        /* USER CODE END TIM2_MspDeInit 0 */ 
        // ����TIM2ʱ�� 
        TIM_TPAD_INT_CLK_ENABLE(); 
 
        /**TIM2 GPIO Configuration 
        PA5     ------> TIM2_CH1 
        */ 
        // ����ʼ��GPIO���� 
        HAL_GPIO_DeInit(TAPD_KEY_GPIO_Port, TAPD_KEY_Pin); 
 
        // ����TIM2�ж� 
        HAL_NVIC_DisableIRQ(TIM_TPAD_INT_IRQn); 
        /* USER CODE BEGIN TIM2_MspDeInit 1 */ 
 
        /* USER CODE END TIM2_MspDeInit 1 */ 
    } 
} 

/** 
 * @brief TIM2ȫ���жϴ����� 
 *        �˺�������HAL����жϴ�������������жϱ�־λ 
 * @param �� 
 * @retval �� 
 */ 
void TIM_TPAD_INT_IRQHandler(void) 
{ 
    // ����HAL����жϴ����� 
    HAL_TIM_IRQHandler(&TIM_TPAD_Handle); 
    // �����־λ��HAL������Ѵ���������ʽ����� 
    __HAL_TIM_CLEAR_FLAG(&TIM_TPAD_Handle, TIM_FLAG_CC1); 
    /* USER CODE END TIM2_IRQn 1 */ 
} 
 
/* USER CODE BEGIN 1 */ 
/** 
 * @brief ���������ػص����� 
 *        ����⵽������ʱ�����ò����־λ��ֹͣ���벶���ж� 
 * @param htim: ��ʱ����� 
 * @retval �� 
 */ 
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) 
{ 
    // �ж��Ƿ�ΪTIM_TPAD_Handle��ʱ�� 
    if (htim == &TIM_TPAD_Handle) 
    { 
        // ���ò����־λ 
        capture_flag = 1; 
        // ֹͣ���벶���ж� 
        HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    } 
} 
 
/** 
 * @brief ��ʱ������ص����� 
 *        ����ʱ�����ʱ����תLED1��״̬��������ֵΪ���ֵ 
 * @param htim: ��ʱ����� 
 * @retval �� 
 */ 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
    // �ж��Ƿ�ΪTIM_TPAD_INT��ʱ�� 
    if(TIM_TPAD_INT == htim->Instance) 
    { 
        // ��תLED1��״̬ 
        LED1_TOGGLE(); 
        // ������ֵΪ���ֵ 
        threshold_cap_val = TPAD_ARR_MAX_VAL; 
        printf("��ƽ��ȡʧ�� \r\n");
    } 
} 
/****************************************************����ģʽ��ת����****************************************************************************** */

/** 
 * @brief ��λ����---���Ϊ�͵�ƽ 
 *        �˺������ڸ�λ���ţ���������Ϊ�������������͵�ƽ��Ȼ�����ö�ʱ�����������л�Ϊ����������� 
 * @param �� 
 * @retval �� 
 */ 
static void tpad_reset(void) 
{ 
    // GPIO��ʼ���ṹ�� 
    GPIO_InitTypeDef GPIO_InitStruct_reset = {0}; 
 
    // �����л�Ϊ����ͨ���������--����͵�ƽ 
    { 
        // ����GPIO���� 
        GPIO_InitStruct_reset.Pin = TAPD_KEY_Pin; 
        // ����GPIOģʽΪ������� 
        GPIO_InitStruct_reset.Mode = GPIO_MODE_OUTPUT_PP; 
        // ����GPIO������Ϊ���� 
        GPIO_InitStruct_reset.Pull = GPIO_PULLDOWN; 
        // ����GPIO�ٶ�Ϊ�ǳ����� 
        GPIO_InitStruct_reset.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
        // ��ʼ��GPIO���� 
        HAL_GPIO_Init(TAPD_KEY_GPIO_Port, &GPIO_InitStruct_reset); 
        // д��͵�ƽ 
        HAL_GPIO_WritePin(TAPD_KEY_GPIO_Port, TAPD_KEY_Pin, GPIO_PIN_RESET); 
        
        // ��ʱ������ 
        for(int time = 0; time < 2000; time++) {;} 
    } 
 
    // ���ö�ʱ�� 
    { 
        // �����ʱ�����жϱ�־λ 
        __HAL_TIM_CLEAR_FLAG(&TIM_TPAD_Handle, TIM_FLAG_CC1|TIM_FLAG_UPDATE); 
        // ֹͣ���� 
        HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_CHANNEL_1); 
        // CNT���� 
        __HAL_TIM_SET_COUNTER(&TIM_TPAD_Handle, 0); 
        // �������� 
        HAL_TIM_IC_Start_IT(&TIM_TPAD_Handle, TIM_CHANNEL_1); 
    } 
    // �����л�Ϊ�����á��������--��ʼ��Ϊ���벶���� 
    { 
        // ����GPIO���� 
        GPIO_InitStruct_reset.Pin = TAPD_KEY_Pin; 
        // ����GPIOģʽΪ����������� 
        GPIO_InitStruct_reset.Mode = GPIO_MODE_AF_PP; 
        // ��ʹ������������ 
        GPIO_InitStruct_reset.Pull = GPIO_NOPULL; 
        // ����GPIO�ٶ�Ϊ���� 
        GPIO_InitStruct_reset.Speed = GPIO_SPEED_FREQ_HIGH; 
        // ����GPIO���ù��� 
        GPIO_InitStruct_reset.Alternate = TAPD_KEY_Alternate; 
        // ��ʼ��GPIO���� 
        HAL_GPIO_Init(TAPD_KEY_GPIO_Port, &GPIO_InitStruct_reset); 
    } 
    // ��������־λ 
    capture_flag = 0; 
} 
/****************************************************���ܺ�������****************************************************************************** */
/** 
 * @brief 1.��ʼ���������� 
 *        2.ƽ���˲���ȡ��ֵ 
 * @param psc: Ԥ��Ƶϵ�� 
 * @retval 1:��ʼ���ɹ���0:��ʼ��ʧ�ܣ� 
 */ 
/*ð������*/ 
uint8_t tpad_init(uint16_t psc) 
{ 
    // ��ʼ��TIM_TPAD_INT��ʱ�� 
    TIM_TPAD_init(TIM_TPAD_INT, 0x100000000 - 1, psc - 1); 
    // ��ʱ���� 
    uint32_t temp = 0; 
    // �洢����ֵ������ 
    uint32_t current_buf[10] = {0}; 
    // ѭ��10�λ�ȡ����ֵ 
    for(int i = 0; i < 10; i++) 
    { 
        current_buf[i] = tpad_get_val(); 
    } 
 
    // ð������ 
    for(int i = 0; i < 9; i++) 
    { 
        for(int j = i + 1; j < 10; j++) 
        { 
            if(current_buf[i] > current_buf[j]) 
            { 
                temp = current_buf[i]; 
                current_buf[i] = current_buf[j]; 
                current_buf[j] = temp; 
            } 
        } 
    } 
  
    temp = 0; 
    // ȡ�м�6������ƽ��ֵ 
    for(int i = 2; i < 8; i++) 
    { 
        temp += current_buf[i]; 
    } 
    // ������ֵ 
    threshold_cap_val = temp / 6; 
 
    // ��ӡδ����ʱ�ĳ��ʱ�� 
    printf("δ����ʱ���ʱ�� = %d\r\n", threshold_cap_val); 
  
    // �ж���ֵ�Ƿ񳬹����ֵ��һ�룬����������ʼ��ʧ�� 
    if(threshold_cap_val > TPAD_ARR_MAX_VAL / 2) 
    { 
        return 0; 
    } 
    return 1; 
} 
 
/** 
 * @brief �õ���ʱ������ֵ 
 *   @note �����ʱ, ��ֱ�ӷ��ض�ʱ���ļ���ֵ 
 *         ���Ƕ��峬ʱʱ��Ϊ: TPAD_ARR_MAX_VAL - 500 
 * @param �� 
 * @retval ����ֵ/����ֵ����ʱ������·��أ� 
 */ 
static uint16_t tpad_get_val(void) 
{ 
    // �����ֵ 
    uint32_t  capture_value = 0; 
    // ��λ���� 
    tpad_reset(); 
 
    // ������ʱ�����벶���ж� 
    HAL_TIM_IC_Start_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    { 
        // ��ʱ���� 
        uint32_t timeout = 0; 
        // �ȴ������־λ������ 
        while (!capture_flag) 
        { 
            // �ж��Ƿ�ʱ 
            if (timeout++ > (TPAD_ARR_MAX_VAL - 500)) 
            { 
                // ֹͣ��ʱ�����벶���ж� 
                HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
                // ���ؼ�������ǰֵ 
                return __HAL_TIM_GET_COUNTER(&TIM_TPAD_Handle); 
            } 
            // ��ʱ1ms 
            HAL_Delay(1); 
        } 
    } 
    // �رն�ʱ�����벶���ж� 
    HAL_TIM_IC_Stop_IT(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    // ��������־λ 
    capture_flag = 0; 
    // ��ȡ���벶��ĵ�ǰֵ 
    capture_value = HAL_TIM_ReadCapturedValue(&TIM_TPAD_Handle, TIM_TPAD_INT_CAP_CHY); 
    return (uint16_t)capture_value; 
} 
/**
* @brief ��ȡ n ��, ȡ���ֵ[��ȡ����TPADʱ�ĳ��ʱ��]
* @param n ��������ȡ�Ĵ���
* @retval n �ζ������������������ֵ
*/
static uint16_t tpad_get_maxval(uint8_t n)
{
  uint16_t TPAD_temp = 0;
  uint16_t TPAD_maxval = 0;
  while (n--)
  {
    tpad_reset();
    TPAD_temp = tpad_get_val();
    //printf("���µ�ֵ[%d] = %d\r\n",n,current_cnt_buffer[n]);
    if(TPAD_temp > TPAD_maxval )
    {
      
      TPAD_maxval = TPAD_temp;
    }
  }
  return TPAD_maxval;
}
/**
 * @brief ɨ�败������-------------�Ƚ�current���ֵ��threshold���ֵ
 * @param mode:ɨ��ģʽ
 *        cnt��ɨ�谴�����´���
 *  @arg  0:��֧��������
 *  @arg  1:֧��������
 * @retval 0��û���£�1������
 */
uint8_t tpad_scan(uint8_t mode)
{
  /*static��֤���������ͷ�*/
  static uint8_t PTAD_state = 0;   /*0 ���Կ�ʼ��⣻ !0 �����ܿ�ʼ���*/
  uint8_t scan_result = 0;     /*����ֵ �Ƿ���*/
  uint8_t TPAD_sample_cnt = DEFAULT_SAMPLE_TIMES;/*Ĭ�ϲ������� = 3��*/

  /*������ģʽ�£���������*/
  if(mode)                    
  {
    TPAD_sample_cnt = CONTINUOUS_SAMPLE_TIMES;
    PTAD_state = 0;
  }

  /*��ȡ��ǰ���ֵ*/
  TPAD_cap_val = tpad_get_maxval(TPAD_sample_cnt);
  //printf("��ǰֵ = %d\r\n",TPAD_cap_val);
  /*����threshold+���Ԥ�����ӳ��ʱ��--------->��Ч����*/
  if(TPAD_cap_val>(threshold_cap_val+TPAD_GATE_VALUE))
  {
    /*������ָ�Ƿ����ߣ����°���*/
    if(0 == PTAD_state)
    {
      printf("TPAD����ʱʱ�� = %d\r\n",TPAD_cap_val);
      scan_result = 1;
    }
    /*mode = 0��--->ɨ��1��-����3�Ρ�1�ΰ���+2��δ���¡�-->keyen = 0�����ٴν���ɨ�谴��*/
    PTAD_state = DEFAULT_SAMPLE_TIMES;
    /*********************** */
  }
  if(PTAD_state>0) 
  {
    PTAD_state--;
  }
  
  return scan_result;

}  
