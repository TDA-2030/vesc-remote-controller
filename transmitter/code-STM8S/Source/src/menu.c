

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
#include "windows.h"

#include <stddef.h>

extern const unsigned char icon_power[];
extern const unsigned char icon_light[];
extern const unsigned char icon_arrow_up[];
extern const unsigned char icon_arrow_down[];
extern const unsigned char BMP1[];


static void page_main(void);
static void page_setting(void);
static void page_para(void);
static void page_init(void);
static void page_connect(void);


void page_main(void)
{
	switch (g_win_state)
	{
	case WIN_STATE_INIT:
		OLED_Clear();
		OLED_ShowString(65, 28, "km/h", 16);
		LCD_DrawLine(36, 25, 17, 44);
		LCD_DrawLine(92, 25, 111, 44);

		LCD_DrawRectangle(38, 0, 90, 5);
		LCD_DrawRectangle(38, 7, 90, 12);

		OLED_ShowChar(121, 0, '%', 12, 1);
		OLED_ShowChar(30, 51, 'M', 12, 1);
		break;
	case WIN_STATE_EXEC:
		{
			if (setting.light_en)
			{
				OLED_DrawBMP(0, 0, 15, 15, icon_light); //图片显示
			}
			else
			{
				OLED_Fill(0, 0, 15, 15, 0);
			}

			if (send_info.status)
			{
				OLED_DrawBMP(16, 0, 31, 15, icon_power); //图片显示
			}
			else
			{
				OLED_Fill(16, 0, 31, 15, 0);
			}

			OLED_ShowNum(40, 20, skate_info.speed / 221, 2, 24);
			OLED_ShowNum(45, 44, send_info.throttle, 4, 8);
		}
		
		{
			if (send_info.direction == 0)
			{
				OLED_DrawBMP(0, 16, 15, 31, icon_arrow_up); //图片显示
			}
			else
			{
				OLED_DrawBMP(0, 16, 15, 31, icon_arrow_down); //图片显示
			}

			OLED_Fill(39, 1, 39 + (ADC_Get_Voltage() - 370), 4, 1);
			OLED_Fill(39 + (ADC_Get_Voltage() - 370), 1, 89, 4, 0);

			{
				uint16_t v;
				if (skate_info.voltage > 2160)
					v = skate_info.voltage - 2160;
				else
					v = 0;
				v /= 7;
				OLED_Fill(39, 8, 39 + v, 11, 1);
				OLED_Fill(39 + v, 8, 89, 11, 0);
			}
			OLED_ShowNum(103, 0, Sys_Tx_Rate, 3, 12);
			OLED_ShowNum(0, 51, skate_info.tacho_single, 5, 12);
		}
		break;

	default:
		break;
	}
}

void page_setting(void)
{
	switch (g_win_state)
	{
	case WIN_STATE_INIT:
		OLED_Clear();
		OLED_ShowString(0, 0, "setings", 16);

		send_info.year = skate_info.year;
		send_info.month = skate_info.month;
		send_info.day = skate_info.day;
		send_info.hour = skate_info.hour;
		send_info.min = skate_info.min;
		send_info.sec = skate_info.sec;
		break;
	case WIN_STATE_EXEC:
		
		
		break;

	default:
		break;
	}
}

void page_para(void)
{
	switch (g_win_state)
	{
	case WIN_STATE_INIT:
		OLED_Clear();
		OLED_ShowString(0, 0, "para", 16);

		send_info.year = skate_info.year;
		send_info.month = skate_info.month;
		send_info.day = skate_info.day;
		send_info.hour = skate_info.hour;
		send_info.min = skate_info.min;
		send_info.sec = skate_info.sec;
		break;
	case WIN_STATE_EXEC:
	{


	}break;

	default:
		break;
	}
}

void page_init(void)
{
	switch (g_win_state)
	{
	case WIN_STATE_INIT:
		OLED_Clear();
		break;
	case WIN_STATE_EXEC:
		if ((LED_STDBY == 0) || (LED_CHRG == 0))
		{
			if ((LED_CHRG == 0))
			{
				OLED_ShowString(0, 0, "charging", 16);
				OLED_ShowNum(70, 0, (ADC_Get_Voltage() - 370) << 1, 3, 16);
				OLED_ShowString(94, 0, "%", 16);
				OLED_ShowNum(0, 20, ADC_Get_Voltage(), 3, 16);
			}
			else
			{
				OLED_ShowString(0, 0, "fully charged", 16);
			}
			delay_us(5000);
		}

		if (LONG_PRESS_HOLD == btn_1.event)
		{
			PB_DDR |= 1 << 2; //数据方向寄存器 1为输出，0为输入
			PB_CR1 |= 1 << 2; //推挽输出
			PB_ODR_ODR2 = 1;
			SetTIM1_PWM_DutyCycle('R', 0);
			SetTIM1_PWM_DutyCycle('G', 500);
			SetTIM1_PWM_DutyCycle('B', 0);
			OLED_DrawBMP(0, 0, 127, 63, BMP1); //图片显示
			delay_ms(700);
			while (LONG_PRESS_HOLD == btn_1.event)
				;
			SetTIM1_PWM_DutyCycle('G', 0);

			win_set(page_connect);
		}
		break;
	default:
		break;
	}
}

void page_connect(void)
{
	static uint16_t key_time=0;
	
	switch (g_win_state)
	{
	case WIN_STATE_INIT:
		OLED_Clear();
		while (NRF24L01_Check())
		{
			OLED_ShowString(0, 0, "NRF24L01 ERROR", 16);
			OLED_Refresh_Gram();
		}
		RF24L01_Init();
		RF24L01_Set_Mode(MODE_TX); //设置为发射模式

		OLED_Clear();
		OLED_Refresh_Gram();
		OLED_ShowString(0, 0, "connecting", 16);
		OLED_ShowString(62, 48, "version: . ", 12);
		OLED_ShowNum(110, 48, MAJOR_VERSION, 1, 12);
		OLED_ShowNum(122, 48, MINOR_VERSION, 1, 12);
		OLED_Refresh_Gram();
		InitTIM2(4, 1000 - 1); //--定时器初始化 16分频
		break;
	case WIN_STATE_EXEC:
		if (system.state != SYSTEM_STATE_IDLE)
		{
			delay_ms(20);
			OLED_Fill(8, 23, 8 + key_time, 25, 1);
			OLED_Refresh_Gram();
			if (++key_time > 112)
			{
				OLED_ShowString(8, 26, "failed!!!", 16);
				OLED_Refresh_Gram();
				delay_ms(800);
				system.state = SYSTEM_STATE_IDLE;
			}
		}
		else
		{
			InitTIM2(4, 10000 - 1); //--定时器初始化 16分频
			win_set(page_main);
		}
		break;
	}
}

#define INDEX_MAX 11
const unsigned char options[INDEX_MAX][9] = {
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

void setting_page_dis_param(uint8_t y, uint8_t index)
{
	uint8_t x = 80;
	switch (index)
	{
	case 0:
		OLED_ShowNum_n(x, y, send_info.status, 5, 12, 1);
		break;
	case 1:
		OLED_ShowNum_n(x, y, setting.light1, 5, 12, 1);
		break;
	case 2:
		OLED_ShowNum_n(x, y, setting.light2, 5, 12, 1);
		break;
	case 3:
		OLED_ShowNum_n(x, y, send_info.direction, 5, 12, 1);
		break;
	case 4:
		OLED_ShowNum_n(x, y, setting.auto_off_time, 5, 12, 1);
		break;
	case 5:
		OLED_ShowNum_n(x, y, send_info.year, 5, 12, 1);
		break;
	case 6:
		OLED_ShowNum_n(x, y, send_info.month, 5, 12, 1);
		break;
	case 7:
		OLED_ShowNum_n(x, y, send_info.day, 5, 12, 1);
		break;
	case 8:
		OLED_ShowNum_n(x, y, send_info.hour, 5, 12, 1);
		break;
	case 9:
		OLED_ShowNum_n(x, y, send_info.min, 5, 12, 1);
		break;
	case 10:
		OLED_ShowNum_n(x, y, send_info.sec, 5, 12, 1);
		break;

	case 11:

		break;
	default:
		break;
	}
}

void setting_page_edit_param(uint8_t y, uint8_t index)
{
	uint8_t x = 80;
	uint16_t v = 1023 - ADC_Get_Val(1);
	switch (index)
	{
	case 0:
		send_info.status = v > 512 ? 1 : 0;
		OLED_ShowNum_n(x, y, send_info.status, 5, 12, 0);
		break;
	case 1:
		setting.light1 = v / 10;
		OLED_ShowNum_n(x, y, setting.light1, 5, 12, 0);
		break;
	case 2:
		setting.light2 = v / 10;
		OLED_ShowNum_n(x, y, setting.light2, 5, 12, 0);
		break;
	case 3:
		send_info.direction = v > 512 ? 0 : 1;
		OLED_ShowNum_n(x, y, send_info.direction, 5, 12, 0);
		break;
	case 4:
		setting.auto_off_time = (v + 5) * 60;
		system.auto_off_timer = 0;
		OLED_ShowNum_n(x, y, setting.auto_off_time, 5, 12, 0);
		break;
	case 5:
		send_info.year = v / 10;
		OLED_ShowNum_n(x, y, send_info.year, 5, 12, 0);
		break;
	case 6:
		send_info.month = v / 85;
		OLED_ShowNum_n(x, y, send_info.month, 5, 12, 0);
		break;
	case 7:
		send_info.day = v / 33;
		OLED_ShowNum_n(x, y, send_info.day, 5, 12, 0);
		break;
	case 8:
		send_info.hour = v / 44;
		OLED_ShowNum_n(x, y, send_info.hour, 5, 12, 0);
		break;
	case 9:
		send_info.min = v / 17;
		OLED_ShowNum_n(x, y, send_info.min, 5, 12, 0);
		break;
	case 10:
		send_info.sec = v / 17;
		OLED_ShowNum_n(x, y, send_info.sec, 5, 12, 0);
		break;

	case 11:

		break;
	default:
		break;
	}
}
