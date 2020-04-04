
/*********************************************************************************
 * 文件名  ：main.c
 * 描述    ：STM8S工程模板
 * 实验平台： STM8开发板
 * 库版本  ：V1.0.0
 * 时间：2016-08-25
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
    uint32_t _1s_time = 0;
    system_clock_init();
    KEY_Init();
    led_init(); //--LED初始化
    setting_read();
    system.state = SYSTEM_STATE_NO_SIGNAL;
    control_Init();                                                        //--外部控制初始化
    ADC_conf();                                                            //--ADC模块初始化 PB2,3端口
    NRF24L01_Base_Init();                                                  //-- 包含SPI初始化
    NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, NRF24L01_Read_Reg(STATUS)); //清除TX_DS或MAX_RT中断标志
    InitTIM3(4, 1000 - 1);
    EnableInterrupt; //--开启中断
    OLED_Init();     //--屏幕初始化
    menu_init();

    while (1)
    {
        win_exec();

        if (Sys_Time > _1s_time)
        {
            _1s_time = Sys_Time + 1000;
            system.auto_off_timer++;

            if (system.auto_off_timer > setting.auto_off_time)
            {
                btn_1.event = LONG_PRESS_HOLD;
            }
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
