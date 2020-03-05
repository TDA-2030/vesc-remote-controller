/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ***************************
 * 文件名  ：adc.c
 * 描述    ：AD配置函数库   
 * 实验平台：iCreate STM8开发板
 * 寄存器版本  ：V2.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 * 修改时间 ：2012-6-16

 * iCreate STM8开发板硬件连接
   STM8的PF0口(也就是ADC2的AIN10)接的是光敏或热敏电阻

****************************************************************************************/

#include "type_def.h"
#include "adc.h"


#define REFE_VOL (122.0f) //放大了100倍


/**************************************************************************
 * 函数名：ADC_conf
 * 描述  ：ADC模块初始化
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void ADC_conf(void)
{
	ADC_CR1 = (2<<4)|(0<<1)|(0<<0);    //ADC时钟输入频率为16MHz 这里设置分频系数为2  单次转换模式 先禁止ADC转换       
	ADC_CR2 = (1<<3)|(0<<1);           //设置数据右对齐  禁止扫描模式
	ADC_TDRL|= 3<<2;                      //AIN2,AIN3禁止施密特触发器功能 
	ADC_CR1 |= 1<<0;                      //第一次写1是从低功耗模式唤醒 
}



/**************************************************************************
 * 函数名：ADC_GetConversionValue
 * 描述  ：获取ADC转换结果
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：内部调用 
 *************************************************************************/
uint16_t ADC_Get_Val(u8 channel)
{
    uint16_t value,temph;        
    uint8_t templ;                  // 定义templ存储低8位数据  temph存储高8位数据

	channel &= 0x0f;
	ADC_CSR &= ~0x0f; //清除上次通道号
    ADC_CSR |= (channel<<0);         //不用外部触发 禁止转换结束中断 设置转换通道
    ADC_CR1 |= 1;

    while(!(ADC_CSR & 0x80));           //等待转换完成
    templ = ADC_DRL;
    temph = ADC_DRH;                  //读取ADC转换  在左对齐和右对齐模式下 读取数据的顺序不同  参考STM8寄存器.PDFP371   
	ADC_CSR &= ~0x80;   //清除标志位

    value = (unsigned int)(templ | (temph << 8));   //注意是10位的转换精度 value、temph应为unsigned int 变量
    return  value;
}

u16 ADC_Get_Voltage(void)
{
    float temp=REFE_VOL * (float)ADC_Get_Val(BATT_CH);
    u16 Vref=ADC_Get_Val(VREF_CH);
	
    temp=temp/Vref;
 
	temp = ((float)temp)*5.228;
    return (u16)temp;///放大了100倍
                  
}