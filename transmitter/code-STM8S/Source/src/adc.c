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


#define REFE_VOL (122.0f) //放大了100倍F

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
    ADC_CR1 = (2 << 4) | (0 << 1) | (0 << 0); //ADC时钟输入频率为16MHz 这里设置分频系数为2  单次转换模式 先禁止ADC转换
    ADC_CR2 = (1 << 3) | (0 << 1);     //设置数据右对齐  禁止扫描模式
    ADC_TDRL |= 3 << 2;                   //AIN2,AIN3禁止施密特触发器功能
    ADC_CR1 |= 1 << 0;                    //第一次写1是从低功耗模式唤醒

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
    uint8_t templ;                  // 定义templ存储低8位数据  temph存储高8位数据
    channel &= 0x0f;
    ADC_CSR &= ~0x0f; //清除上次通道号
    ADC_CSR |= (channel << 0);       //不用外部触发 禁止转换结束中断 设置转换通道
    ADC_CR1 |= 1;

    while (!(ADC_CSR & 0x80));          //等待转换完成

    templ = ADC_DRL;
    temph = ADC_DRH;                  //读取ADC转换  在左对齐和右对齐模式下 读取数据的顺序不同  参考STM8寄存器.PDFP371
    ADC_CSR &= ~0x80;   //清除标志位
    value = (unsigned int)(templ | (temph << 8));   //注意是10位的转换精度 value、temph应为unsigned int 变量
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

    return sum / ARRAY_LEN; // 放大了100倍
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

    return sum / len; // 放大了100倍
}