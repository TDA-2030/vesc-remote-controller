 #include "adc.h"
 #include "main.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
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



////得到温度值
////返回值:温度值(扩大了100倍,单位:℃.)
//short Get_Temprate(void)	//获取内部温度传感器温度值
//{
//	u32 adcx;
//	short result;
// 	double temperate;
//	adcx=Get_Adc_Average(ADC_Channel_16,20);	//读取通道16,20次取平均
//	temperate=(float)adcx*(3.3/4096);		//电压值 
//	temperate=(1.43-temperate)/0.0043+25;	//转换为温度值 	 
//	result=temperate*=100;					//扩大100倍.
//	return result;
//}



void ADC_DMA_Base_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能DMA1时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);      
  
	//初始化IO 
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;        
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);  
	
	//ADC时钟 不能大于14MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72/6 = 12MHz 
	
	//复位DMA1 通道1 ADC
    DMA_DeInit(DMA1_Channel1);
	ADC_DeInit(ADC1);//复位ADC1
	
	DMA_Cmd(DMA1_Channel1, DISABLE);//关闭DMA 通道1
	//使能ADC DMA
	ADC_DMACmd(ADC1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


//-------------多通道 ADC DMA读取----------------
void ADC_DMA_Regularchannel_Init(u32 ADC_Buff) {
	
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
	
	DMA_Cmd(DMA1_Channel1, DISABLE);//关闭DMA 通道1
	
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//ADC内存基地址
    DMA_InitStructure.DMA_MemoryBaseAddr  = ADC_Buff;//变量基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设到内存
    DMA_InitStructure.DMA_BufferSize = 3;//数据宽度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设内存地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//变量内存地址增长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16字节 12位ADC
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//16字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//关闭内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);//初始化DMA1 通道1
	
	ADC_SoftwareStartConvCmd(ADC1, DISABLE); //停止转换
	ADC_Cmd(ADC1, DISABLE);//关闭ADC
	
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; //开启扫描 多通道读取
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //不开启连续转换 触发一次 转换触发一次
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 3;//转换3个通道
    ADC_Init(ADC1, &ADC_InitStructure);//初始化ADC
	
	//设置指定ADC规则通道，设置转换时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_13Cycles5);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	
    ADC_Cmd(ADC1, ENABLE);//使能ADC
	DMA_Cmd(DMA1_Channel1, ENABLE);//使能DMA 通道1
	
	ADC_ResetCalibration(ADC1);//复位校准寄存器  
    while(ADC_GetResetCalibrationStatus(ADC1));//等待校准寄存器复位完成  
	
    ADC_StartCalibration(ADC1);//ADC校准  
    while(ADC_GetCalibrationStatus(ADC1));//等待校准完成  
	
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//由于没有采用外部触发，所以使用软件触发ADC转换 
}


void ADC_ShuntCurrentReadingCalibration(void)
{
	uint8_t bIndex;
	
//	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,DISABLE);
//	ADC_DMACmd(ADC1, DISABLE);
//	
//	for(bIndex=0; bIndex <16; bIndex++)//得到地电压
//	{
//		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
//		
//		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)) { }
//		
//		setting.adc_offset_crg_cur  += SampleData.ADC_Value_Array[ADC_CHANNEL_CHARGE];
//		setting.adc_offset_dcrg_cur += SampleData.ADC_Value_Array[ADC_CHANNEL_DISCHARGE];
//		setting.adc_offset_vol      += SampleData.ADC_Value_Array[ADC_CHANNEL_VOLTAGE];
//		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
//	}
//	
//	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
//	ADC_DMACmd(ADC1, ENABLE);
	
	/* ADC1 Injected group of conversions end interrupt disabling */
	ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
	
	/* ADC1 Injected conversions trigger is given by software and enabled */ 
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);  // 通过软件出发转换
	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
	
	/* ADC1 Injected conversions configuration */
	ADC_InjectedSequencerLengthConfig(ADC1,2);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_6,1,ADC_SampleTime_239Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_5,2,ADC_SampleTime_239Cycles5);
	
	/* Clear the ADC1 JEOC pending flag */
	ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);  
	ADC_SoftwareStartInjectedConvCmd(ADC1,ENABLE);
	
	/* ADC Channel used for current reading are read 
	in order to get zero currents ADC values*/ 
	for(bIndex=0; bIndex <32; bIndex++)//得到地电压
	{
		ADC_SoftwareStartInjectedConvCmd(ADC1,ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)) { }
		
		setting.adc_offset_dcrg_cur  += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2));
		setting.adc_offset_crg_cur   += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1));
		
		/* Clear the ADC1 JEOC pending flag */
		ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
	}
	ADC_ExternalTrigInjectedConvCmd(ADC1,DISABLE);
	
	setting.adc_offset_dcrg_cur = -(setting.adc_offset_dcrg_cur>>5);
	setting.adc_offset_crg_cur = -(setting.adc_offset_crg_cur>>5);
}


//--------------------- 
//作者：major_jm 
//来源：CSDN 
//原文：https://blog.csdn.net/u012854263/article/details/80180094 
//版权声明：本文为博主原创文章，转载请附上博文链接！











