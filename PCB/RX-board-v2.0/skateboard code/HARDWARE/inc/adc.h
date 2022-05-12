#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define ADC_CHANNEL_VOLTAGE   0
#define ADC_CHANNEL_DISCHARGE 1
#define ADC_CHANNEL_CHARGE    2

void ADC_DMA_Base_Init(void);
void ADC_DMA_Regularchannel_Init(u32 ADC_Buff);

void ADC_ShuntCurrentReadingCalibration(void);
 
#endif 
