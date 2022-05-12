#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define ADC_CHANNEL_VOLTAGE   0
#define ADC_CHANNEL_DISCHARGE 1
#define ADC_CHANNEL_CHARGE    2

void ADC_DMA_Base_Init(void);
void ADC_DMA_Regularchannel_Init(u32 ADC_Buff);

void ADC_ShuntCurrentReadingCalibration(void);
 
#endif 
