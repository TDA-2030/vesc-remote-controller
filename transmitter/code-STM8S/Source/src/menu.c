

#include "type_def.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "24l01.h"
#include "control.h"
#include "adc.h"
#include "menu.h"
#include "main.h"
#include "timer.h"
#include "key.h"
#include "pwm.h"


extern const unsigned char icon_power[];
extern const unsigned char icon_light[];
extern const unsigned char icon_arrow_up[];
extern const unsigned char icon_arrow_down[];

#define PAGE_MAIN 0
#define PAGE_SETTING 66
#define PAGE_EXIT_SETTING 67
#define PAGE_PARAM1 1
#define PAGE_PARAM2 2
#define PAGE_PARAM3 3
#define PAGE_MAX   4

uint8_t page=0;

static uint8_t cur_index=0,is_edit=0;
static uint8_t start,offset;
static uint8_t is_chg=1;
uint8_t chg_time=0;

#define INDEX_MAX 11
const unsigned char options[INDEX_MAX][9]={
"power   ",
"light1  ",
"light2  ",
"direct  ",
"off_time",
"year    ",
"month   ",
"day     ",
"hour    ",
"minute  ",
"seconds ",
};

uint32_t hs_time=0,ls_time=0,ms_time=0;


void page_init(uint8_t p);


void menu_init(void)
{
  page=PAGE_MAIN;
  page_init(page);
}


void page_init(uint8_t p)
{
  switch(p)
  {
    case PAGE_MAIN:
      OLED_Clear();
      OLED_ShowString(65,28,"km/h",16);
      LCD_DrawLine(36,25,17,44);
      LCD_DrawLine(92,25,111,44);
      
      LCD_DrawRectangle(38,0,90,5);
      LCD_DrawRectangle(38,7,90,12);

      OLED_ShowChar(121,0,'%',12,1);
      OLED_ShowChar(30,51,'M',12,1);
    break;
    
    case PAGE_SETTING:
      OLED_Clear();
      OLED_ShowString(0,0,"setings",16);
	  cur_index=0;
	  is_edit=0;
	  is_chg=1;
	  start=0;
	  offset=0;
	  send_info.year = skate_info.year;
	  send_info.month = skate_info.month;
	  send_info.day = skate_info.day;
	  send_info.hour = skate_info.hour;
	  send_info.min = skate_info.min;
	  send_info.sec = skate_info.sec;
    break;
	
	case PAGE_EXIT_SETTING:
      OLED_Clear();
	  OLED_ShowString(0,0,"warning",16);
      OLED_ShowString(0,16,"Please release",16);
	  OLED_ShowString(0,32,"the throttle!",16);
	  
    break;
    
    case PAGE_PARAM1:
      OLED_Clear();
      OLED_ShowString(0,0,"parameter1",16);
      OLED_ShowString(0,16,"U:  . v",12);
      OLED_ShowString(0,28,"Ib:   . a",12);
      OLED_ShowString(0,40,"Im:   . a",12);
      OLED_ShowString(0,52,"P:    w",12);
      
      OLED_ShowString(64,16,"rpm:     ",12);
      OLED_ShowString(64,28,"tem:  `C",12);
      OLED_ShowString(64,40,"dah:  .  ",12);
      OLED_ShowString(64,52,"rah:  .  ",12);
      
    break;
    
    case PAGE_PARAM2:
      OLED_Clear();
      OLED_ShowString(0,0,"parameter2",16);
      OLED_ShowString(0,16,"t:  -  -  |  -  -  ",12);
      OLED_ShowString(0,28,"dcc:   .  a",12);
      OLED_ShowString(0,40,"cc:  .  a",12);
	  //OLED_ShowString(0,40,"cc:  .  a",12);
    break;
    
    case PAGE_PARAM3:
      OLED_Clear();
      OLED_ShowString(0,0,"parameter3",16);
	  OLED_ShowString(0,16,"dwh:  .  ",12);
      OLED_ShowString(0,28,"rwh:  .  ",12);
    break;
    
  default:break;
  }
}


void setting_page_dis_param(uint8_t y,uint8_t index)
{
	uint8_t x=80;
	switch(index)
	{
		case 0:
			OLED_ShowNum_n(x,y,send_info.status,5,12,1);
		break;
		case 1:
			OLED_ShowNum_n(x,y,setting.light1,5,12,1);
		break;
		case 2:
			OLED_ShowNum_n(x,y,setting.light2,5,12,1);
		break;
		case 3:
			OLED_ShowNum_n(x,y,send_info.direction,5,12,1);
		break;
		case 4:
			OLED_ShowNum_n(x,y,setting.auto_off_time,5,12,1);
		break;
		case 5:
			OLED_ShowNum_n(x,y,send_info.year,5,12,1);
		break;
		case 6:
			OLED_ShowNum_n(x,y,send_info.month,5,12,1);
		break;
		case 7:
			OLED_ShowNum_n(x,y,send_info.day,5,12,1);
		break;
		case 8:
			OLED_ShowNum_n(x,y,send_info.hour,5,12,1);
		break;
		case 9:
			OLED_ShowNum_n(x,y,send_info.min,5,12,1);
		break;
		case 10:
			OLED_ShowNum_n(x,y,send_info.sec,5,12,1);
		break;
		
		case 11:
			
		break;
		default:break;
	}
}

void setting_page_edit_param(uint8_t y,uint8_t index)
{
	uint8_t x=80;
	uint16_t v = 1023-ADC_Get_Val(1);
	switch(index)
	{
		case 0:
			send_info.status = v>512?1:0;
			OLED_ShowNum_n(x,y,send_info.status,5,12,0);
		break;
		case 1:
			setting.light1 = v/10;
			OLED_ShowNum_n(x,y,setting.light1,5,12,0);
		break;
		case 2:
			setting.light2 = v/10;
			OLED_ShowNum_n(x,y,setting.light2,5,12,0);
		break;
		case 3:
			send_info.direction = v>512?0:1;
			OLED_ShowNum_n(x,y,send_info.direction,5,12,0);
		break;
		case 4:
			setting.auto_off_time = (v+5)*60;
			system.auto_off_timer=0;
			OLED_ShowNum_n(x,y,setting.auto_off_time,5,12,0);
		break;
		case 5:
			send_info.year = v/10;
			OLED_ShowNum_n(x,y,send_info.year,5,12,0);
		break;
		case 6:
			send_info.month = v/85;
			OLED_ShowNum_n(x,y,send_info.month,5,12,0);
		break;
		case 7:
			send_info.day = v/33;
			OLED_ShowNum_n(x,y,send_info.day,5,12,0);
		break;
		case 8:
			send_info.hour = v/44;
			OLED_ShowNum_n(x,y,send_info.hour,5,12,0);
		break;
		case 9:
			send_info.min = v/17;
			OLED_ShowNum_n(x,y,send_info.min,5,12,0);
		break;
		case 10:
			send_info.sec = v/17;
			OLED_ShowNum_n(x,y,send_info.sec,5,12,0);
		break;
		
		case 11:
			
		break;
		default:break;
	}
}

void menu_process(void)
{
  if(Key_State.state==KEY_LONG_PRESS)
  {
    Key_State.state=0;
    if(Key_State.KeyNum==0x02)
    {
      if(page!=PAGE_SETTING)
      {
       		page=PAGE_SETTING;//ÉèÖÃÒ³Ãæ
      }
      else
      {
		  page=PAGE_EXIT_SETTING;
      }
	  page_init(page);
    }
  }
  
  
  if(Key_State.state==KEY_PRESS)
  {
	  system.auto_off_timer=0;
    if(Key_State.KeyNum==0x01 && page==PAGE_MAIN)
    {
      Key_State.state=0;
        
        if(setting.light_en)
        {
          setting.light_en=0;
        }
        else
        {
          setting.light_en=1;
        }
      }   
    else if(Key_State.KeyNum==0x02 && page!=PAGE_SETTING)
    {
      Key_State.state=0;
      page++;//ÉèÖÃÒ³Ãæ
      if(page>=PAGE_MAX)
        page=0;
      
      page_init(page);
    }
  }
  
  if(Sys_Time>hs_time)
    {
		uint16_t th;
		hs_time=Sys_Time+20;
		th=1023-ADC_Get_Val(1);   //--¶ÁÈ¡Ò¡¸ËÖµ
		if(th>512)
		{
			SetTIM1_PWM_DutyCycle('B',th-512);
			SetTIM1_PWM_DutyCycle('R',0);
		}
		else
		{
			SetTIM1_PWM_DutyCycle('R',512-th);
			SetTIM1_PWM_DutyCycle('B',0);
		}
		if((page!=PAGE_SETTING)&&(page!=PAGE_EXIT_SETTING))
		{
			send_info.throttle=th;
		}
		else
		{
			send_info.throttle=512;
		}
		
		 if(0==setting.light_en)
        {
          send_info.light1 = 0;
          send_info.light2 = 0;
        }
        else
        {
          send_info.light1 = setting.light1;
          send_info.light2 = setting.light2;
        }
        
    }
  
  switch(page)
  {
    case PAGE_MAIN:
     
      if(Sys_Time>ms_time)
      {
          ms_time=Sys_Time+400;
          if(setting.light_en)
          {
            OLED_DrawBMP(0,0,15,15,icon_light);  //Í¼Æ¬ÏÔÊ¾
          }
          else
          {
            OLED_Fill(0,0,15,15,0);
          }
          
          if(send_info.status)
          {
            OLED_DrawBMP(16,0,31,15,icon_power);  //Í¼Æ¬ÏÔÊ¾
          }
          else
          {
            OLED_Fill(16,0,31,15,0);
          }
          
          OLED_ShowNum(40,20,skate_info.speed/221,2,24);
          OLED_ShowNum(45,44,send_info.throttle,4,8);
      }
      
      if(Sys_Time>ls_time)
      {
          ls_time=Sys_Time+1100;
          
          if(send_info.direction==0)
          {
            OLED_DrawBMP(0,16,15,31,icon_arrow_up);  //Í¼Æ¬ÏÔÊ¾
          }
          else
          {
            OLED_DrawBMP(0,16,15,31,icon_arrow_down);  //Í¼Æ¬ÏÔÊ¾
          }
          
          OLED_Fill(39,1,39+(ADC_Get_Voltage()-370),4,1);
          OLED_Fill(39+(ADC_Get_Voltage()-370),1,89,4,0);
          
		  {
			  uint16_t v;
			  if(skate_info.voltage>2160)
			      v = skate_info.voltage-2160;
			  else
				  v=0;
			  v/=7;
			  OLED_Fill(39,8,39+v,11,1);
			  OLED_Fill(39+v,8,89,11,0);
		  }
          OLED_ShowNum(103,0,Sys_Tx_Rate,3,12);
          OLED_ShowNum(0,51,skate_info.tacho_single,5,12);
          
          
      }
    break;
    
    case PAGE_PARAM1:
      if(Sys_Time>ms_time)
      {
        ms_time=Sys_Time+500;
        OLED_ShowNum(12,16,skate_info.voltage/100,2,12);OLED_ShowNum(30,16,skate_info.voltage/10,1,12);
        OLED_ShowNum(18,28,skate_info.bat_current/10,3,12);OLED_ShowNum(42,28,skate_info.bat_current,1,12);
        OLED_ShowNum(18,40,skate_info.mot_current/10,3,12);OLED_ShowNum(42,40,skate_info.mot_current,1,12);
        OLED_ShowNum(12,52,(skate_info.bat_current*(skate_info.voltage/100))/10,4,12);
        
        OLED_ShowNum(88,16,skate_info.speed,5,12);
        OLED_ShowNum(88,28,skate_info.board_temp,2,12);
        OLED_ShowNum(88,40,skate_info.ah_drawn/100,2,12);OLED_ShowNum(106,40,skate_info.ah_drawn,2,12);
        OLED_ShowNum(88,52,skate_info.ah_regen/100,2,12);OLED_ShowNum(106,52,skate_info.ah_regen,2,12);
      }
    break;
    
    case PAGE_PARAM2:
      if(Sys_Time>ls_time)
      {
        ls_time=Sys_Time+1000;
        OLED_ShowNum(12,16,skate_info.year,2,12);
        OLED_ShowNum(30,16,skate_info.month,2,12);
        OLED_ShowNum(48,16,skate_info.day,2,12);
        OLED_ShowNum(66,16,skate_info.hour,2,12);
        OLED_ShowNum(84,16,skate_info.min,2,12);
        OLED_ShowNum(102,16,skate_info.sec,2,12);
        
        OLED_ShowNum(24,28,skate_info.discharge_cur/100,3,12);OLED_ShowNum(48,28,skate_info.discharge_cur,2,12);
        OLED_ShowNum(18,40,skate_info.charge_cur/100,2,12);OLED_ShowNum(36,40,skate_info.charge_cur,2,12);
      }
    break;
	
  	case PAGE_PARAM3:
		if(Sys_Time>ms_time)
		{
			ms_time=Sys_Time+500;
			OLED_ShowNum(24,16,skate_info.wh_drawn/100,2,12);OLED_ShowNum(42,16,skate_info.wh_drawn,2,12);
			OLED_ShowNum(24,28,skate_info.wh_regen/100,2,12);OLED_ShowNum(42,28,skate_info.wh_regen,2,12);
		}
	break;
    
  case PAGE_SETTING:
	{
		if(Sys_Time>ms_time)
		{
			uint8_t i,y;
			ms_time=Sys_Time+100;
			
			if(Key_State.state==KEY_PRESS)
			{
				Key_State.state=0;
				
				if(Key_State.KeyNum==0x01)
				{
					if(is_edit==0)
						is_edit=1;
					else
					{
						is_edit=0;
						is_chg=1;
						if((cur_index>4)&&(cur_index<11))
						{
							chg_time=1;
						}
						
					}
				}
				else if((Key_State.KeyNum==0x02)&&(is_edit==0))
				{
					cur_index++;
					if(cur_index>=INDEX_MAX) cur_index=0;
					
					start = (cur_index>>2)<<2;
					offset = cur_index%4;

					is_chg=1;
					
				}
			}
			if(is_chg==1)
			{
				uint8_t index;
				is_chg=0;
				
				for(i=0;i<4;i++)
				{
					y=16+(12*i);
					index = start+i;
					if(index>=INDEX_MAX)
					{
						OLED_Fill(0,y,127,y+12,0);
					}
					else
					{
						
						if(i==offset)
						{
							OLED_ShowString_m(15,y,options[index],12,0);
						}
						else
						{
							OLED_ShowString_m(15,y,options[index],12,1);
						}
						OLED_ShowNum_n(0,y,index+1,2,12,1);
						setting_page_dis_param(y,index);
					}
				}
			}
			if(is_edit==1)
			{
				setting_page_edit_param(16+(12*offset),start+offset);
			}
		}
	}
    break;
  	case PAGE_EXIT_SETTING:
		if(Sys_Time>ms_time)
		{
			uint16_t a;
			ms_time=Sys_Time+10;
			a=ADC_Get_Val(1);
			if((a<530) && (a>494))
			{
				page=PAGE_MAIN;
				page_init(page);
			}
		}
	break;
    default:break;
  }
}




