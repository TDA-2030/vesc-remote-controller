
/*********************************************************************************
 * �ļ���  ��main.c
 * ����    ��STM8S����ģ��
 * ʵ��ƽ̨�� STM8������
 * ��汾  ��V1.0.0
 * ʱ�䣺2016-08-25
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "type_def.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "adc.h"
#include "24l01.h"
#include "control.h"
#include "timer.h"
#include "key.h"
#include "pwm.h"
#include "menu.h"
#include "main.h"
#include "stm8s_flash.h"



Setting_TypeDef setting={0};
System_TypeDef system={0};

extern const unsigned char BMP1[];



void setting_read(void);
void setting_write(void);

int main(void)
{
    uint32_t _1s_time=0,refresh_time=0;
    uint16_t key_time;
    
    CLK_CKDIVR &= ~(3<<3);//
    CLK_CKDIVR &= ~(7<<0);
    
    KEY_Init();
    LED_conf(); //--LED��ʼ��
    setting_read();
    system.state = SYSTEM_STATE_NO_SIGNAL;
    
    Control_Init();//--�ⲿ���Ƴ�ʼ��
    ADC_conf(); //--ADCģ���ʼ�� PB2,3�˿�
    NRF24L01_Base_Init( );//-- ����SPI��ʼ��
    NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,NRF24L01_Read_Reg(STATUS)); //���TX_DS��MAX_RT�жϱ�־
    InitTIM3(4,1000-1);
    EnableInterrupt;  //--�����ж�
    OLED_Init(); //--��Ļ��ʼ��
    
    key_time=0;
    while(1)
    {
		if((LED_STDBY==0)||(LED_CHRG==0))
		{
			if((LED_CHRG==0))
			{
				OLED_ShowString(0,0,"charging",16);
				OLED_ShowNum(70,0,(ADC_Get_Voltage()-370)<<1,3,16);OLED_ShowString(94,0,"%",16);
				OLED_ShowNum(0,20,ADC_Get_Voltage(),3,16);
				
			}else
			{
				OLED_ShowString(0,0,"fully charged",16);
			}
			OLED_Refresh_Gram();
		}
		
		if(KEY_1 == 1) 
		  key_time++;
		else 
		  key_time=0;
		
		if(key_time>200)
		{
			PB_DDR|=1<<2;   //���ݷ���Ĵ��� 1Ϊ�����0Ϊ����
			PB_CR1|=1<<2;   //�������
			PB_ODR_ODR2=1;
			SetTIM1_PWM_DutyCycle('R',0);
			SetTIM1_PWM_DutyCycle('G',500);
			SetTIM1_PWM_DutyCycle('B',0);
			OLED_DrawBMP(0,0,127,63,BMP1);  //ͼƬ��ʾ
			delay_ms(700);
			while(KEY_1);
			SetTIM1_PWM_DutyCycle('G',0);
			
			break;
		}
		delay_us(5000);
    }
    
    
    while (NRF24L01_Check())
    {
        OLED_ShowString(0,0,"NRF24L01 ERROR",16);
        OLED_Refresh_Gram();
    }
    RF24L01_Init();
    RF24L01_Set_Mode(MODE_TX);//����Ϊ����ģʽ
    
    OLED_Clear();
    OLED_Refresh_Gram();
    OLED_ShowString(0,0,"connecting",16);
	OLED_ShowString(62,48,"version: . ",12);
	OLED_ShowNum(110,48,MAJOR_VERSION,1,12);OLED_ShowNum(122,48,MINOR_VERSION,1,12);
    OLED_Refresh_Gram();
	
	key_time=0;
	InitTIM2(4,1000-1); //--��ʱ����ʼ�� 16��Ƶ
    while(system.state != SYSTEM_STATE_IDLE)
    {
      	delay_ms(20);
		OLED_Fill(8,23,8+key_time,25,1);
		OLED_Refresh_Gram();
		if(++key_time>112)
		{
			OLED_ShowString(8,26,"failed!!!",16);
			OLED_Refresh_Gram();
			delay_ms(800);
			system.state = SYSTEM_STATE_IDLE;
			break;
		}
    }
	
    Key_State.state=0;
	
    InitTIM2(4,10000-1); //--��ʱ����ʼ�� 16��Ƶ
    
    OLED_Clear();
    OLED_Refresh_Gram();
    menu_init();
    
    while(1)
    {
      
      menu_process();
      
      if(Sys_Time>refresh_time)
      {
        refresh_time=Sys_Time+60;
        OLED_Refresh_Gram();
      }
      
      if(Sys_Time>_1s_time)
      {
        _1s_time=Sys_Time+1000;
        
        system.auto_off_timer++;
        if(system.auto_off_timer>setting.auto_off_time)
        {
          Key_State.state=KEY_LONG_PRESS;
          Key_State.KeyNum=0x01;
        }
      }
      
       
      if(Key_State.state==KEY_LONG_PRESS)
      {
        
        if(Key_State.KeyNum==0x01)
        {
              Key_State.state=0;
              SetTIM1_PWM_DutyCycle('R',500);
              SetTIM1_PWM_DutyCycle('G',0);
              SetTIM1_PWM_DutyCycle('B',0);
              OLED_DrawBMP(0,0,127,63,BMP1);  //ͼƬ��ʾ
              delay_ms(700);
			  setting_write();
              while(KEY_1);
              SetTIM1_PWM_DutyCycle('R',0);
              PB_ODR_ODR2=0;
			  while(1);
              
        }
       
      }

    }
}

/** 
* @brief  : flash������ȡ
* @param  : 
* @param  : 
* @return : 
*/
void setting_read(void)
{
	EEPROM_read(SETTING_FLASH_ADDR,(uint8_t*)&setting,sizeof(Setting_TypeDef));
	if(setting.check != FLASH_CHECK)
	{
		setting.check = FLASH_CHECK;
        setting.auto_off_time = 600;
		setting.light_en=0;
		setting.light1=50;
		setting.light2=50;
		
		EEPROM_write(SETTING_FLASH_ADDR,(uint8_t*)&setting,sizeof(Setting_TypeDef));
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
	EEPROM_write(SETTING_FLASH_ADDR,(uint8_t*)&setting,sizeof(Setting_TypeDef));
	
}

