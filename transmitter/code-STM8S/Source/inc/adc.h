#ifndef __ADC_H
#define __ADC_H


#define BAT_VOL_LOW (370)

void ADC_Init(void);
uint16_t ADC_Get_Val(uint8_t channel);
uint16_t ADC_Get_Voltage(void);
uint16_t ADC_Get_Throttle(void);

#endif