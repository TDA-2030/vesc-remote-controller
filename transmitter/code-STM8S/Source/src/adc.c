/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:33:44
 * @Description: STM8S ADC 
 */


#include "type_def.h"
#include "adc.h"


#define ADC_CH_BATT     (3)
#define ADC_CH_VREF     (7)
#define ADC_CH_THROTTLE (1)


#define REFE_VOL (122.0f) //�Ŵ���100��F

enum
{
    FILTER_ID_BAT_VOL,
    FILTER_ID_THROTTLE,
    FILTER_ID_MAX,
};

#define ARRAY_LEN  12
static uint16_t filter_array[FILTER_ID_MAX][ARRAY_LEN] = {0};
static uint8_t filter_index[FILTER_ID_MAX] = {0};



/**
 * @brief adc initaliza
 *
 */
void ADC_Init(void)
{
    ADC_CR1 = (2 << 4) | (0 << 1) | (0 << 0); //ADCʱ������Ƶ��Ϊ16MHz �������÷�Ƶϵ��Ϊ2  ����ת��ģʽ �Ƚ�ֹADCת��
    ADC_CR2 = (1 << 3) | (0 << 1);     //���������Ҷ���  ��ֹɨ��ģʽ
    ADC_TDRL |= 3 << 2;                   //AIN2,AIN3��ֹʩ���ش���������
    ADC_CR1 |= 1 << 0;                    //��һ��д1�Ǵӵ͹���ģʽ����

    for (size_t i = 0; i < ARRAY_LEN; i++)
    {
        filter_array[FILTER_ID_BAT_VOL][i] = BAT_VOL_LOW;
    }
}


/**
 * @brief get adc convert value
 *
 */
uint16_t ADC_Get_Val(uint8_t channel)
{
    uint16_t value, temph;
    uint8_t templ;                  // ����templ�洢��8λ����  temph�洢��8λ����
    channel &= 0x0f;
    ADC_CSR &= ~0x0f; //����ϴ�ͨ����
    ADC_CSR |= (channel << 0);       //�����ⲿ���� ��ֹת�������ж� ����ת��ͨ��
    ADC_CR1 |= 1;

    while (!(ADC_CSR & 0x80));          //�ȴ�ת�����

    templ = ADC_DRL;
    temph = ADC_DRH;                  //��ȡADCת��  ���������Ҷ���ģʽ�� ��ȡ���ݵ�˳��ͬ  �ο�STM8�Ĵ���.PDFP371
    ADC_CSR &= ~0x80;   //�����־λ
    value = (unsigned int)(templ | (temph << 8));   //ע����10λ��ת������ value��temphӦΪunsigned int ����
    return  value;
}

/**
 * @brief get battery voltage (10mV)
 *
 */
uint16_t ADC_Get_Voltage(void)
{
    uint16_t sum = 0;
    float temp = REFE_VOL * (float)ADC_Get_Val(ADC_CH_BATT);
    uint16_t Vref = ADC_Get_Val(ADC_CH_VREF);
    temp = temp / Vref;
    temp = ((float)temp) * 5.228;
    filter_array[FILTER_ID_BAT_VOL][filter_index[FILTER_ID_BAT_VOL]++] = (uint16_t)temp;

    if (filter_index[FILTER_ID_BAT_VOL] == ARRAY_LEN)
    {
        filter_index[FILTER_ID_BAT_VOL] = 0;
    }

    for (size_t i = 0; i < ARRAY_LEN; i++)
    {
        sum += filter_array[FILTER_ID_BAT_VOL][i];
    }

    return sum / ARRAY_LEN; // �Ŵ���100��
}


/**
 * @brief get throttle value (0 ~ 1023)
 *
 */
uint16_t ADC_Get_Throttle(void)
{
    uint16_t sum = 0;
    const uint8_t len = 3;
    filter_array[FILTER_ID_THROTTLE][filter_index[FILTER_ID_THROTTLE]++] =
        // Due to the hardware design, it needs to be reversed
        1023 - ADC_Get_Val(ADC_CH_THROTTLE);

    if (filter_index[FILTER_ID_THROTTLE] == len)
    {
        filter_index[FILTER_ID_THROTTLE] = 0;
    }

    for (size_t i = 0; i < len; i++)
    {
        sum += filter_array[FILTER_ID_THROTTLE][i];
    }

    return sum / len; // �Ŵ���100��
}