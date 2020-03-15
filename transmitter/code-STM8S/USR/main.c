
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
#include "pwm.h"
#include "menu.h"
#include "main.h"
#include "stm8s_flash.h"

Setting_TypeDef setting = {0};
System_TypeDef system = {0};

extern const unsigned char BMP1[];

void setting_read(void);
void setting_write(void);

int main(void)
{
  uint32_t _1s_time = 0;

  CLK_CKDIVR &= ~(3 << 3); //
  CLK_CKDIVR &= ~(7 << 0);

  KEY_Init();
  LED_conf(); //--LED初始化
  setting_read();
  system.state = SYSTEM_STATE_NO_SIGNAL;

  Control_Init();                                                        //--外部控制初始化
  ADC_conf();                                                            //--ADC模块初始化 PB2,3端口
  NRF24L01_Base_Init();                                                  //-- 包含SPI初始化
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, NRF24L01_Read_Reg(STATUS)); //清除TX_DS或MAX_RT中断标志
  InitTIM3(4, 1000 - 1);
  EnableInterrupt; //--开启中断
  OLED_Init();     //--屏幕初始化
  win_init();

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

    if (LONG_PRESS_HOLD == btn_1.event)
    {
        SetTIM1_PWM_DutyCycle('R', 500);
        SetTIM1_PWM_DutyCycle('G', 0);
        SetTIM1_PWM_DutyCycle('B', 0);
        OLED_DrawBMP(0, 0, 127, 63, BMP1); //图片显示
        delay_ms(700);
        setting_write();
        while (LONG_PRESS_HOLD == btn_1.event)
          ;
        SetTIM1_PWM_DutyCycle('R', 0);
        PB_ODR_ODR2 = 0;
        while (1)
          ;
      
    }
  }
}

/** 
* @brief  : flash参数读取
* @param  : 
* @param  : 
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
* @param  : 
* @param  : 
* @return : 
*/
void setting_write(void)
{
  setting.check = FLASH_CHECK;
  EEPROM_write(SETTING_FLASH_ADDR, (uint8_t *)&setting, sizeof(Setting_TypeDef));
}
