/*****************************************************************
*  @file     : main.c            
*  @brief    :     
*  Details.          
*                       
*  @author   : zhouli        
*  @version  : xx.xx         
*  @date     : 2018-12-06    
*                           
*  Change History 
******************************************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "timer.h"
#include "adc.h"
#include "rtc.h"
#include "led.h"

#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "comm_uart.h"
#include "packet.h"
#include "bldc_interface.h"

#include "24l01.h"
#include "control.h"

#include "main.h"

#define SETTING_FLASH_ADDR (STM32_FLASH_BASE+63*1024)
#define FLASH_CHECK (0x12345678)   //����ֵ



Setting_TypeDef setting={0};
sample_TypeDef SampleData={0};
System_TypeDef system={0};

uint16_t notfound_time=0;  //��ʧ�ź�ʱ�������



/** 
* @brief  : systick�жϻص�������1ms����һ��
* @param  : 
* @param  : 
* @return : 
*/
void Systick_callback(void)
{
	packet_timerfunc();
	if(SYSTEM_STATE_RUNNING == system.state)
	{
		
	}
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);//
}

/** 
* @brief  : ADC DMA��������ж�
* @param  : 
* @param  : 
* @return : 
*/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA1->ISR & DMA1_FLAG_TC1 )
	{
		SampleData.ADC_Value_Array[ADC_CHANNEL_VOLTAGE]   += setting.adc_offset_vol;
		SampleData.ADC_Value_Array[ADC_CHANNEL_DISCHARGE] += setting.adc_offset_dcrg_cur;
		SampleData.ADC_Value_Array[ADC_CHANNEL_CHARGE]    += setting.adc_offset_crg_cur;
	}
	DMA1->IFCR = DMA1_FLAG_TC1;
}


void EXTI9_5_IRQHandler(void)
{
	if ((EXTI->PR & EXTI_Line8) && (EXTI->IMR & EXTI_Line8) )
	{
		uint8_t status;
		uint8_t RxLength;
		
		EXTI->PR = EXTI_Line8;
		
		status=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,status); //���TX_DS��MAX_RT�жϱ�־
		if(status&MAX_TX)
		{
			NRF24L01_Write_Reg( FLUSH_TX,0xff );	//���TX FIFO�Ĵ���
		}
		else if(status&RX_OK)
		{
			uint8_t Rx_buff[32];
			
			RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
			if(RxLength<32)
			{
				NRF24L01_Read_Buf( RD_RX_PLOAD,Rx_buff,RxLength );	//���յ�����
				NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
				if(Exue_Control(Rx_buff)==0)
				{
					notfound_time=0; //���յ����ݣ���ռ�����
					RF24L01_Set_Mode(MODE_TX);
					NRF_Mode=MODE_TX;
				}
			}
		}
		else if(status&TX_OK)
		{
			NRF24L01_Write_Reg( FLUSH_TX,0xff );	//���TX FIFO�Ĵ���
			notfound_time=0; //���ͳɹ����ݣ���ռ�����
			NRF_Mode=MODE_RX;
			RF24L01_Set_Mode(MODE_RX);
		}
	}
}


//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if ((TIM3->SR & TIM_IT_Update) && (TIM3->DIER & TIM_IT_Update))  //���TIM3�����жϷ������
	{
		TIM3->SR = (uint16_t)~TIM_IT_Update;//���TIMx�����жϱ�־ 
		
		//LED=!LED;
		if(NRF_Mode == MODE_TX)
		{
			Upload_data();
			NRF24L01_TxPacket((uint8_t*)&Tx_block,32);
		}
		
//		if(notfound_time<MAX_LOST_TIME)
//		{
//			notfound_time++;
//		}else
//		{
//			system.state = SYSTEM_STATE_NO_SIGNAL; //����������˵����ʱ��û���ź�
//			TIM1_MOE_OFF();//����ֹͣ���
//			if(notfound_time<LOST_PD_TIME)
//			{
//				notfound_time++;
//			}
//			else system.state = SYSTEM_STATE_STANDBY;
//		}
	}
}

//RTCʱ���ж�
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//�����ж�
	{	
		RTC_ClearITPendingBit(RTC_IT_SEC);
		RTC_Get();//����ʱ��
		
 	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����ж�
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);		//�������ж�	  	
		//RF24L01_Set_Mode(MODE_RX);//����Ϊ����ģʽ
  	}
	
	RTC_WaitForLastTask();
}

/** 
* @brief  : flash������ȡ
* @param  : 
* @param  : 
* @return : 
*/
void setting_read(void)
{
	STMFLASH_Read(SETTING_FLASH_ADDR,(uint16_t*)&setting,sizeof(Setting_TypeDef));
//	if(setting.check != FLASH_CHECK)
	{
		setting.check = FLASH_CHECK;
		setting.adc_offset_crg_cur = 0;
		setting.adc_offset_dcrg_cur = 0;
		setting.adc_offset_vol = 0;
		
		
		STMFLASH_Write(SETTING_FLASH_ADDR,(uint16_t*)&setting,sizeof(Setting_TypeDef));
	}
}


/** 
* @brief  : flash��������
* @param  : 
* @param  : 
* @return : 
*/
void setting_write(void)
{
	setting.check = FLASH_CHECK;
	STMFLASH_Write(SETTING_FLASH_ADDR,(uint16_t*)&setting,sizeof(Setting_TypeDef));
	
}


/**
* @brief  : ADC��������
* @param  : 
* @param  : 
* @return : 
*/
void adc_read_measure(void)
{
	float temp;
	
	temp = (float)SampleData.ADC_Value_Array[ADC_CHANNEL_VOLTAGE];  //�����ѹ
	SampleData.vin = (uint16_t)(temp*0.963); //�Ŵ���100��
	
	temp = (float)SampleData.ADC_Value_Array[ADC_CHANNEL_CHARGE];  //������  (�˷ŷŴ�6.8������������0.005ŷķ��I=U/(0.005*6.8) )
	SampleData.charge_cur = (float)(temp*0.02369); //
	
	temp = (float)SampleData.ADC_Value_Array[ADC_CHANNEL_DISCHARGE];  //�ŵ����
	SampleData.discharge_cur = (float)(temp*0.02369); //
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//
}


//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{			 
	
//	RF24L01_Set_Mode(MODE_PD);//����Ϊ����ģʽ
//	NRF_Mode=MODE_RX;
//	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//��λ����IO��	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//ʹ��PWR����ʱ��
	PWR_EnterSTANDBYMode();	  //���������STANDBY��ģʽ 
}

/** 
* @brief  : 
* @param  : 
* @param  : 
* @return : 
*/
void check_exec(void)
{
	
	switch(system.state)
	{
		case SYSTEM_STATE_IDLE:
			
		break;
		
		
		default:break;
	}
}


void bldc_val_received(mc_values *val) {
	usb_printf("\r\n");
	usb_printf("Input voltage: %.2f V\r\n", val->vin);
	usb_printf("Temp:          %.2f degC\r\n", val->temp_fet_filtered);
	usb_printf("Current motor: %.2f A\r\n", val->motor_current);
	usb_printf("Current in:    %.2f A\r\n", val->input_current);
	usb_printf("RPM:           %.1f RPM\r\n", val->rpm);
	usb_printf("Duty cycle:    %.1f %%\r\n", val->duty_cycle_now * 100.0);
	usb_printf("Ah Drawn:      %.4f Ah\r\n", val->amp_hours);
	usb_printf("Ah Regen:      %.4f Ah\r\n", val->amp_hours_charged);
	usb_printf("Wh Drawn:      %.4f Wh\r\n", val->watt_hours);
	usb_printf("Wh Regen:      %.4f Wh\r\n", val->watt_hours_charged);
	usb_printf("Tacho:         %i counts\r\n", val->tachometer_value);
	usb_printf("Tacho ABS:     %i counts\r\n", val->tachometer_abs_value);
	usb_printf("Fault Code:    %s\r\n", bldc_interface_fault_to_string(val->fault_code));
}

void usb_comm_process(void)
{
	if(USB_USART_RX_STA&0x8000)
	{
		uint8_t *str = &USB_USART_RX_BUF[0];
		
		USB_USART_RX_STA &= ~(0xC000);
		
		if(*str=='[')
		{
			str++;
			switch(*str)
			{
				case 't':
					str++;Tx_block.speed = ((*str-'0')*100)+((*(str+1)-'0')*10)+(*(str+2)-'0');
					TIM1_Throttle_Set(Tx_block.speed);
					
				break;
				default :break;
			}
		}
		USB_USART_RX_STA = 0x0000;
	}
}

/** 
* @brief  : ������
* @param  : 
* @param  : 
* @return : 
*/
int main(void)
{
	uint32_t time=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	delay_init();	    	 //��ʱ������ʼ��
	LED_Init();
	ADC_DMA_Base_Init();
	ADC_DMA_Regularchannel_Init((uint32_t)SampleData.ADC_Value_Array);
	NRF24L01_Base_Init();//-- ����SPI��ʼ��
//	while (NRF24L01_Check())
//    {
//        delay_ms(100);
//    }
    RF24L01_Init();
    RF24L01_Set_Mode(MODE_PD);//����Ϊ����ģʽ
	while(RTC_Init())	  	//RTC��ʼ��
	{
		
	}
	if(PWR_GetFlagStatus(PWR_FLAG_SB))
	{
		uint8_t t=100;
		LED=0;
		while(1)
		{
			delay_ms(100);LED=!LED;
			if(0 == --t)
			{
				RTC_Alarm_Second_Set(calendar.timecount+6);
				Sys_Enter_Standby();
			}
			if(SampleData.ADC_Value_Array[ADC_CHANNEL_VOLTAGE])
			if(SYSTEM_STATE_IDLE == system.state)
			{
				break;
			}
		}
	}
	
	
//	uart1_init(115200);	 //���ڳ�ʼ��Ϊ115200
//	uart3_init(115200);	 //���ڳ�ʼ��Ϊ115200
	
	control_Init();//--�ⲿ���Ƴ�ʼ��
	setting_read();
	
	delay_ms(1000);
	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(700);
	USB_Port_Set(1);	//USB�ٴ�����
 	Set_USBClock();
 	USB_Interrupts_Config();
 	USB_Init();
	
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(0x08f);
	IWDG_Enable();
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	
	ADC_ShuntCurrentReadingCalibration();
	//MOS_TRIG = 1;
	
	TIM1_MOE_ON();
	TIM1_Throttle_Set(THROTTLE_MAX/2);
	
	while(1)
	{
		IWDG_ReloadCounter();
		
		bldc_packet_process();
		
		usb_comm_process();
		
		if(GetTick()>time)
		{
			time = GetTick()+600;
			adc_read_measure();
			if(bDeviceState==CONFIGURED)
			{
//				usb_printf("[%4u-%2u-%2u %2u-%2u-%2u]\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,
//				calendar.min,calendar.sec);
//				
				usb_printf("[vin:%4d,c:%3.2f,dc:%3.2f]\r\n",
				SampleData.vin,
				SampleData.charge_cur,
				SampleData.discharge_cur);
				
				bldc_interface_get_values();
			}
			if(SYSTEM_STATE_IDLE==system.state)
				LED=!LED;
		}
		if(SYSTEM_STATE_STANDBY == system.state)
		{
			RTC_Alarm_Second_Set(calendar.timecount+6);
			Sys_Enter_Standby();
		}
		
	}	 
} 

