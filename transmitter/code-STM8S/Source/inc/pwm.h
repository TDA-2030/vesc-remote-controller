#ifndef __TIM1_PWM_H
#define __TIM1_PWM_H




#define TIM2_Channel   2
#define TIM2_Channel1  1
#define TIM2_Channel2  2
#define TIM2_Channel3  3

#define TIM2_OCMODE_PWM1 0x60 
#define TIM2_OCMODE_PWM2 0x70
#define TIM2_OUTPUTSTATE_ENABLE 0x11
#define TIM2_OCPOLARITY_HIGH 0x00 
#define TIM2_OCPOLARITY_LOW 0x22

void PWM_Init(void);
void PWM_Close(void);
void PWM_Open(void);
void SetTIM1_PWM_DutyCycle(uint8_t ch, uint16_t Compare);

#endif

