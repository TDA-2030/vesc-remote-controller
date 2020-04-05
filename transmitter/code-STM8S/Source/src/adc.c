/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ***************************
 * �ļ���  ��adc.c
 * ����    ��AD���ú�����
 * ʵ��ƽ̨��iCreate STM8������
 * �Ĵ����汾  ��V2.0.0
 * ����    ��ling_guansheng  QQ��779814207
 * ����    ��
 * �޸�ʱ�� ��2012-6-16

 * iCreate STM8������Ӳ������
   STM8��PF0��(Ҳ����ADC2��AIN10)�ӵ��ǹ�������������

****************************************************************************************/

#include "type_def.h"
#include "adc.h"


#define ARRAY_LEN  12
static uint16_t filter_array[ARRAY_LEN] = {0};
static uint8_t filter_index = 0;

#define REFE_VOL (122.0f) //�Ŵ���100��


/**************************************************************************
 * ��������ADC_conf
 * ����  ��ADCģ���ʼ��
 * ����  ����
 *
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 *************************************************************************/
void ADC_conf(void)
{
    ADC_CR1 = (2 << 4) | (0 << 1) | (0 << 0); //ADCʱ������Ƶ��Ϊ16MHz �������÷�Ƶϵ��Ϊ2  ����ת��ģʽ �Ƚ�ֹADCת��
    ADC_CR2 = (1 << 3) | (0 << 1);     //���������Ҷ���  ��ֹɨ��ģʽ
    ADC_TDRL |= 3 << 2;                   //AIN2,AIN3��ֹʩ���ش���������
    ADC_CR1 |= 1 << 0;                    //��һ��д1�Ǵӵ͹���ģʽ����

    for (size_t i = 0; i < ARRAY_LEN; i++)
    {
        filter_array[i] = 370;
    }
}



/**************************************************************************
 * ��������ADC_GetConversionValue
 * ����  ����ȡADCת�����
 * ����  ����
 *
 * ���  ����
 * ����  ����
 * ����  ���ڲ�����
 *************************************************************************/
uint16_t ADC_Get_Val(u8 channel)
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

uint16_t ADC_Get_Voltage(void)
{
    uint16_t sum = 0;
    float temp = REFE_VOL * (float)ADC_Get_Val(BATT_CH);
    uint16_t Vref = ADC_Get_Val(VREF_CH);
    temp = temp / Vref;
    temp = ((float)temp) * 5.228;
    filter_array[filter_index++] = (uint16_t)temp;

    if (filter_index == ARRAY_LEN)
    {
        filter_index = 0;
    }

    for (size_t i = 0; i < ARRAY_LEN; i++)
    {
        sum += filter_array[i];
    }

    return sum / ARRAY_LEN; ///�Ŵ���100��
}