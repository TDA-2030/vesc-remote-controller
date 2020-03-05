#ifndef __ADC_H
#define __ADC_H


#define BATT_CH   3
#define VREF_CH   7

void ADC_conf(void);
u16 ADC_Get_Val(u8 channel);
u16 ADC_Get_Voltage(void);

#endif