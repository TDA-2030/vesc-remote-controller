/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:38:43
 * @Description: file content
 */


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
#include "windows.h"
#include "main.h"
#include "stm8s_flash.h"
#include "menu.h"

Setting_TypeDef setting = {0};
System_TypeDef system = {0};


void system_clock_init(void);
void setting_read(void);
void setting_write(void);

int main(void)
{
    uint32_t time_1s = 0;
    uint32_t time_200ms = 0;
    system_clock_init();
    KEY_Init();
    led_init();
    setting_read();
    system.state = SYSTEM_STATE_NO_SIGNAL;
    control_Init();                                                        //--外部控制初始化
    ADC_Init();                                                            //--ADC模块初始化 PB2,3端口
    NRF24L01_Base_Init();                                                  //-- 包含SPI初始化
    NRF24L01_Clear_IRQ_Flag(IRQ_ALL);
    InitTIM3(4, 1000 - 1);  //--系统ms计时
    EnableInterrupt; //--开启中断
    OLED_Init();     //--屏幕初始化
    menu_init();

    while (1)
    {
        win_exec();

        if (Sys_Time > time_1s)
        {
            time_1s = Sys_Time + 1000;
            system.auto_off_timer++;

            if (system.auto_off_timer > setting.auto_off_time)
            {
                btn_1.event = LONG_PRESS_HOLD;
            }
        }

        if (Sys_Time > time_200ms)
        {
            time_200ms = Sys_Time + 200;
            system.bat_vol = ADC_Get_Voltage();
        }

        
    }
}

void system_clock_init(void)
{
    CLK_CKDIVR &= ~(3 << 3); //时钟初始化
    CLK_CKDIVR &= ~(7 << 0);
}

/**
* @brief  : flash参数读取
* @return :
*/
void setting_read(void)
{
    EEPROM_read(SETTING_FLASH_ADDR, (uint8_t *)&setting, sizeof(Setting_TypeDef));

    if (setting.check != FLASH_CHECK)
    {
        setting.check = FLASH_CHECK;
        setting.auto_off_time = 600;
        setting.light_en = 0;
        setting.light1 = 50;
        setting.light2 = 50;
        EEPROM_write(SETTING_FLASH_ADDR, (uint8_t *)&setting, sizeof(Setting_TypeDef));
    }
}

/**
* @brief  : flash参数保存
* @return :
*/
void setting_write(void)
{
    setting.check = FLASH_CHECK;
    EEPROM_write(SETTING_FLASH_ADDR, (uint8_t *)&setting, sizeof(Setting_TypeDef));
}
