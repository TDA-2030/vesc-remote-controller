 #include "adc.h"
 #include "main.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
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



////�õ��¶�ֵ
////����ֵ:�¶�ֵ(������100��,��λ:��.)
//short Get_Temprate(void)	//��ȡ�ڲ��¶ȴ������¶�ֵ
//{
//	u32 adcx;
//	short result;
// 	double temperate;
//	adcx=Get_Adc_Average(ADC_Channel_16,20);	//��ȡͨ��16,20��ȡƽ��
//	temperate=(float)adcx*(3.3/4096);		//��ѹֵ 
//	temperate=(1.43-temperate)/0.0043+25;	//ת��Ϊ�¶�ֵ 	 
//	result=temperate*=100;					//����100��.
//	return result;
//}



void ADC_DMA_Base_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��DMA1ʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);      
  
	//��ʼ��IO 
    GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;        
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;  
    GPIO_Init(GPIOA,&GPIO_InitStructure);  
	
	//ADCʱ�� ���ܴ���14MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72/6 = 12MHz 
	
	//��λDMA1 ͨ��1 ADC
    DMA_DeInit(DMA1_Channel1);
	ADC_DeInit(ADC1);//��λADC1
	
	DMA_Cmd(DMA1_Channel1, DISABLE);//�ر�DMA ͨ��1
	//ʹ��ADC DMA
	ADC_DMACmd(ADC1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


//-------------��ͨ�� ADC DMA��ȡ----------------
void ADC_DMA_Regularchannel_Init(u32 ADC_Buff) {
	
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
	
	DMA_Cmd(DMA1_Channel1, DISABLE);//�ر�DMA ͨ��1
	
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//ADC�ڴ����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr  = ADC_Buff;//��������ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//���赽�ڴ�
    DMA_InitStructure.DMA_BufferSize = 3;//���ݿ��
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ڴ��ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�����ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16�ֽ� 12λADC
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//16�ֽ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//�ر��ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��DMA1 ͨ��1
	
	ADC_SoftwareStartConvCmd(ADC1, DISABLE); //ֹͣת��
	ADC_Cmd(ADC1, DISABLE);//�ر�ADC
	
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; //����ɨ�� ��ͨ����ȡ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����������ת�� ����һ�� ת������һ��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������ת��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 3;//ת��3��ͨ��
    ADC_Init(ADC1, &ADC_InitStructure);//��ʼ��ADC
	
	//����ָ��ADC����ͨ��������ת��ʱ��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_13Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_13Cycles5);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	
    ADC_Cmd(ADC1, ENABLE);//ʹ��ADC
	DMA_Cmd(DMA1_Channel1, ENABLE);//ʹ��DMA ͨ��1
	
	ADC_ResetCalibration(ADC1);//��λУ׼�Ĵ���  
    while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�У׼�Ĵ�����λ���  
	
    ADC_StartCalibration(ADC1);//ADCУ׼  
    while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼���  
	
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//����û�в����ⲿ����������ʹ���������ADCת�� 
}


void ADC_ShuntCurrentReadingCalibration(void)
{
	uint8_t bIndex;
	
//	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,DISABLE);
//	ADC_DMACmd(ADC1, DISABLE);
//	
//	for(bIndex=0; bIndex <16; bIndex++)//�õ��ص�ѹ
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
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);  // ͨ���������ת��
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
	for(bIndex=0; bIndex <32; bIndex++)//�õ��ص�ѹ
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
//���ߣ�major_jm 
//��Դ��CSDN 
//ԭ�ģ�https://blog.csdn.net/u012854263/article/details/80180094 
//��Ȩ����������Ϊ����ԭ�����£�ת���븽�ϲ������ӣ�











