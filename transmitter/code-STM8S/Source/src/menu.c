/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:38:49
 * @Description: file content
 */


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
#include "windows.h"
#include "stdio.h"

static void page_boot(void);
static void page_shutdown(void);
static void page_connect(void);
static void page_charge(void);
static void page_lowpower(void);
static void page_main(void);
static void page_setting(void);
static void page_para(void);
static void cb_page_exec(void *arg);

static uint32_t g_page_time = 0;

void menu_init(void)
{
    win_init();
    win_set(page_boot);
}

void page_boot(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_Clear();
            led_set_rgb(0, 0, 0);
            break;

        case WIN_STATE_EXEC:
        {
            static uint8_t green = 0;

            if ((TP4056_STDBY == 0) || (TP4056_CHRG == 0))
            {
                win_set(page_charge);
            }

            if (LONG_PRESS_HOLD == get_button_event(&btn_1))
            {
                control_power(1);
                led_set_rgb(200, 200, 200);
                OLED_DrawBMP(0, 0, 127, 63, BMP1); //图片显示
                delay_ms(700);
                win_set(page_connect);
            }

            if (PRESS_DOWN == get_button_event(&btn_1))
            {
                delay_ms(10);

                if (green < 250)
                {
                    green++;
                }
            }
            else
            {
                green = 0;
            }

            led_set_rgb(0, green, 0);
        }
        break;

        default:
            break;
    }
}

void page_shutdown(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_DrawBMP(0, 0, 127, 63, BMP1); //图片显示
            break;

        case WIN_STATE_EXEC:
        {
            uint8_t v = 80;

            while (v--)
            {
                led_set_hsv(120 + v, 100, v);
                delay_ms(25);
            }

            setting_write();
            delay_ms(10);
            led_set_rgb(0, 0, 0);
            control_power(0);

            while (1)
                ;
        }
        break;

        default:
            break;
    }
}


void page_connect(void)
{
    static uint16_t key_time = 0;

    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_Clear();
            led_set_rgb(0, 0, 0);

            while (NRF24L01_Check())
            {
                OLED_ShowString(0, 0, "NRF24L01 ERROR", 16);
                OLED_Refresh_Gram();
            }

            RF24L01_Init();
            RF24L01_Set_Mode(MODE_TX); //设置为发射模式
            OLED_Clear();
            OLED_ShowString(0, 0, "connecting", 16);
            OLED_ShowString(0, 40, __DATE__, 12);
            OLED_ShowString(18, 52, __TIME__, 12);
            OLED_ShowString(86, 48, "ver: . ", 12);
            OLED_ShowNum(110, 48, MAJOR_VERSION, 1, 12);
            OLED_ShowNum(122, 48, MINOR_VERSION, 1, 12);
            InitTIM2(4, 1000 - 1); //--定时器初始化 16分频
            break;

        case WIN_STATE_EXEC:
            if (system.state != SYSTEM_STATE_IDLE)
            {
                delay_ms(20);
                OLED_Fill(8, 23, 8 + key_time, 25, 1);

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

static void cb_charge_single(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_1 == btn)
    {
        control_power(1);
        win_set(page_connect);
    }
}
void page_charge(void)
{
    static uint8_t val_index;
    static uint16_t bat_val;

    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_Clear();
            LCD_DrawRectangle(37, 16, 88, 34);
            LCD_DrawRectangle(89, 19, 90, 31);
            button_attach(&btn_1, SINGLE_CLICK, cb_charge_single);
            system.bat_vol = ADC_Get_Voltage();
            bat_val = system.bat_vol - BAT_VOL_LOW;
            val_index = bat_val;
            break;

        case WIN_STATE_EXEC:
        {
            static uint32_t led_time = 0;
            static uint8_t led_v = 0;
            static uint8_t led_dir = 0;

            if (Sys_Time > g_page_time)
            {
                g_page_time = Sys_Time + 200;
                bat_val = system.bat_vol - BAT_VOL_LOW;

                if ((TP4056_CHRG == 0))
                {
                    val_index += 1;

                    if (val_index > 48)
                    {
                        val_index = bat_val;
                    }
                }
                else if ((TP4056_STDBY == 0))
                {
                    val_index = bat_val;
                }

                if (val_index > 48)
                {
                    val_index = 48;
                }

                OLED_Fill(39, 18, 39 + val_index, 32, 1);
                OLED_Fill(39 + val_index, 18, 87, 32, 0);
            }

            if (Sys_Time > led_time)
            {
                led_time = Sys_Time + 25;

                if ((TP4056_STDBY == 0))
                {
                    led_set_hsv(120, 100, 60);
                }
                else
                {
                    led_set_hsv(bat_val << 1, 100, led_v);

                    if (led_dir)
                    {
                        if (led_v-- < 2)
                        {
                            led_dir = 0;
                        }
                    }
                    else
                    {
                        if (led_v++ > 80)
                        {
                            led_dir = 1;
                        }
                    }
                }
            }
        }
        break;

        case WIN_STATE_EXIT:
            button_attach(&btn_1, SINGLE_CLICK, NULL);
            break;

        default:
            break;
    }
}

static void cb_main_repeat(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (3 == btn->repeat)
    {
        if (&btn_1 == btn)
        {
            send_info.status = 1;
        }
        else if (&btn_2 == btn)
        {
            send_info.status = 0;
        }
    }
}
static void cb_main_long(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_1 == btn)
    {
        win_set(page_shutdown);
    }
    else if (&btn_2 == btn)
    {
        win_set(page_setting);
    }
}
static void cb_main_single(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_1 == btn)
    {
        setting.light_en = !setting.light_en;
    }
    else if (&btn_2 == btn)
    {
        win_set(page_para);
    }
}
void page_main(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_Clear();
            OLED_ShowString(65, 23, "km/h", 16);
            LCD_DrawLine(36, 20, 17, 39);
            LCD_DrawLine(35, 20, 16, 39);
            LCD_DrawLine(92, 20, 111, 39);
            LCD_DrawLine(93, 20, 112, 39);
            LCD_DrawRectangle(38, 0, 90, 5);
            OLED_ShowChar(121, 0, '%', 12);
            OLED_ShowChar(30, 51, 'M', 12);
            button_attach(&btn_1, PRESS_REPEAT, cb_main_repeat);
            button_attach(&btn_1, LONG_RRESS_START, cb_main_long);
            button_attach(&btn_1, SINGLE_CLICK, cb_main_single);
            button_attach(&btn_2, PRESS_REPEAT, cb_main_repeat);
            button_attach(&btn_2, LONG_RRESS_START, cb_main_long);
            button_attach(&btn_2, SINGLE_CLICK, cb_main_single);
            win_set_flash_time(60);
            win_set_exec_callback(cb_page_exec);
            break;

        case WIN_STATE_EXEC:
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

            if (send_info.direction == 0)
            {
                OLED_DrawBMP(0, 16, 15, 31, icon_arrow_up); //图片显示
            }
            else
            {
                OLED_DrawBMP(0, 16, 15, 31, icon_arrow_down); //图片显示
            }

            OLED_ShowNum(40, 15, skate_info.speed / 221, 2, 24);
            OLED_ShowNum(42, 39, send_info.throttle, 4, 8);
            OLED_Showfloat(33, 8, skate_info.voltage, 'V', 3, 1, 8);
            OLED_Showfloat(63, 8, skate_info.mot_current, 'A', 4, 1, 8);
            OLED_ShowNum(103, 0, Sys_Tx_Rate, 3, 12);
            OLED_ShowNum(0, 51, skate_info.tacho_single, 5, 12);

            if (Sys_Time > g_page_time)
            {
                static uint8_t val_index;
                uint8_t bat_val;
                g_page_time = Sys_Time + 200;
                bat_val = system.bat_vol - BAT_VOL_LOW;

                if ((TP4056_CHRG == 0))
                {
                    val_index += 1;

                    if (val_index > 51)
                    {
                        val_index = bat_val;
                    }
                }
                else
                {
                    val_index = bat_val;
                }

                OLED_Fill(39, 1, 39 + val_index, 4, 1);
                OLED_Fill(39 + val_index, 1, 89, 4, 0);
            }

            break;

        case WIN_STATE_EXIT:
            button_attach(&btn_1, SINGLE_CLICK, NULL);
            button_attach(&btn_1, PRESS_REPEAT, NULL);
            button_attach(&btn_2, PRESS_REPEAT, NULL);
            button_attach(&btn_2, LONG_RRESS_START, NULL);
            button_attach(&btn_2, SINGLE_CLICK, NULL);
            break;

        default:
            break;
    }
}

static void cb_para_single(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_2 == btn)
    {
        win_set(page_main);
    }
}
void page_para(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_Clear();
            button_attach(&btn_1, SINGLE_CLICK, cb_main_single);
            button_attach(&btn_2, SINGLE_CLICK, cb_para_single);
            break;

        case WIN_STATE_EXEC:
        {
            char buf[64];

            if (Sys_Time > g_page_time)
            {
                g_page_time = Sys_Time + 200;
                sprintf(buf, "%02d-%02d-%02d %02d-%02d-%02d", skate_info.year, skate_info.month, skate_info.day,
                        skate_info.hour, skate_info.min, skate_info.sec);
                OLED_ShowString(0, 0, (uint8_t *)buf, 12);
                OLED_ShowNum(0, 16, skate_info.speed, 5, 12);
                OLED_ShowNum(0, 28, skate_info.tacho, 5, 12);
                OLED_ShowNum(0, 40, skate_info.board_temp, 5, 12);
                OLED_ShowNum(0, 52, skate_info.wh_drawn, 5, 12);
                OLED_ShowNum(32, 16, skate_info.wh_regen, 5, 12);
                OLED_ShowNum(32, 28, skate_info.ah_drawn, 5, 12);
                OLED_ShowNum(32, 40, skate_info.ah_regen, 5, 12);
                OLED_ShowNum(32, 52, skate_info.charge_cur, 5, 12);
                // OLED_ShowNum(0, 51, skate_info.mot_current, 5, 8);
            }
        }
        break;

        case WIN_STATE_EXIT:
            button_attach(&btn_1, SINGLE_CLICK, NULL);
            button_attach(&btn_2, SINGLE_CLICK, NULL);
            break;

        default:
            break;
    }
}


typedef enum
{
    SET_LIGHT,
    SET_OFF_TIME,
    SET_YEAR,
    SET_MONTH,
    SET_DAY,
    SET_HOUR,
    SET_MIN,
    SET_SEC,
    SET_INDEX_MAX,
} SET_INDEX;
static const unsigned char options[SET_INDEX_MAX][9] =
{
    "light   ",
    "off_time",
    "year    ",
    "month   ",
    "day     ",
    "hour    ",
    "minute  ",
    "seconds ",
};
static void cb_setting_long(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_2 == btn)
    {
        win_set(page_main);
    }
}
static uint8_t select = 0;
static void cb_setting_single(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_1 == btn &&
        (select > 0))
    {
        select--;
    }
    else if (&btn_2 == btn &&
             (select < (SET_INDEX_MAX - 1)))
    {
        select++;
    }
}
void page_setting(void)
{
    static uint8_t bak = 0;
    static uint8_t start = 0;
    static const uint8_t LINES = 4;
    uint8_t end;
    uint8_t index;

    switch (win_get_state())
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
            button_attach(&btn_2, LONG_RRESS_START, cb_setting_long);
            button_attach(&btn_1, PRESS_UP, cb_setting_single);
            button_attach(&btn_2, PRESS_UP, cb_setting_single);
            win_set_flash_time(33);

            for (uint8_t i = 0; i < LINES; i++)
            {
                if (i == select)
                {
                    OLED_ShowModeSet(0);
                    OLED_ShowString(2, (i * 12) + 16, options[index], 12);
                    OLED_ShowModeSet(1);
                }
                else
                {
                    OLED_ShowString(2, (i * 12) + 16, options[i], 12);
                }
            }

            break;

        case WIN_STATE_EXEC:
        {
            if (bak != select)
            {
                bak = select;
                end = start + LINES;

                if (select >= end)
                {
                    start++;
                }
                else if (select < start)
                {
                    start--;
                }

                end = start + LINES;

                for (uint8_t i = 0; i < LINES; i++)
                {
                    index = i + start;

                    if (index == select)
                    {
                        OLED_ShowModeSet(0);
                        OLED_ShowString(2, (i * 12) + 16, options[index], 12);
                        OLED_ShowModeSet(1);
                    }
                    else
                    {
                        OLED_ShowString(2, (i * 12) + 16, options[index], 12);
                    }
                }
            }
        }
        break;

        case WIN_STATE_EXIT:
            button_attach(&btn_2, LONG_RRESS_START, NULL);
            break;

        default:
            break;
    }
}

void page_lowpower(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            OLED_Clear();
            break;

        case WIN_STATE_EXEC:
        {
        }
        break;

        case WIN_STATE_EXIT:
            break;

        default:
            break;
    }
}




// void setting_page_dis_param(uint8_t y, uint8_t index)
// {
//     uint8_t x = 80;

//     switch (index)
//     {
//         case 0:
//             OLED_ShowNum_n(x, y, send_info.status, 5, 12, 1);
//             break;

//         case 1:
//             OLED_ShowNum_n(x, y, setting.light1, 5, 12, 1);
//             break;

//         case 2:
//             OLED_ShowNum_n(x, y, setting.light2, 5, 12, 1);
//             break;

//         case 3:
//             OLED_ShowNum_n(x, y, send_info.direction, 5, 12, 1);
//             break;

//         case 4:
//             OLED_ShowNum_n(x, y, setting.auto_off_time, 5, 12, 1);
//             break;

//         case 5:
//             OLED_ShowNum_n(x, y, send_info.year, 5, 12, 1);
//             break;

//         case 6:
//             OLED_ShowNum_n(x, y, send_info.month, 5, 12, 1);
//             break;

//         case 7:
//             OLED_ShowNum_n(x, y, send_info.day, 5, 12, 1);
//             break;

//         case 8:
//             OLED_ShowNum_n(x, y, send_info.hour, 5, 12, 1);
//             break;

//         case 9:
//             OLED_ShowNum_n(x, y, send_info.min, 5, 12, 1);
//             break;

//         case 10:
//             OLED_ShowNum_n(x, y, send_info.sec, 5, 12, 1);
//             break;

//         case 11:
//             break;

//         default:
//             break;
//     }
// }

// void setting_page_edit_param(uint8_t y, uint8_t index)
// {
//     uint8_t x = 80;
//     uint16_t v = 1023 - ADC_Get_Val(1);

//     switch (index)
//     {
//         case 0:
//             send_info.status = v > 512 ? 1 : 0;
//             OLED_ShowNum_n(x, y, send_info.status, 5, 12, 0);
//             break;

//         case 1:
//             setting.light1 = v / 10;
//             OLED_ShowNum_n(x, y, setting.light1, 5, 12, 0);
//             break;

//         case 2:
//             setting.light2 = v / 10;
//             OLED_ShowNum_n(x, y, setting.light2, 5, 12, 0);
//             break;

//         case 3:
//             send_info.direction = v > 512 ? 0 : 1;
//             OLED_ShowNum_n(x, y, send_info.direction, 5, 12, 0);
//             break;

//         case 4:
//             setting.auto_off_time = (v + 5) * 60;
//             system.auto_off_timer = 0;
//             OLED_ShowNum_n(x, y, setting.auto_off_time, 5, 12, 0);
//             break;

//         case 5:
//             send_info.year = v / 10;
//             OLED_ShowNum_n(x, y, send_info.year, 5, 12, 0);
//             break;

//         case 6:
//             send_info.month = v / 85;
//             OLED_ShowNum_n(x, y, send_info.month, 5, 12, 0);
//             break;

//         case 7:
//             send_info.day = v / 33;
//             OLED_ShowNum_n(x, y, send_info.day, 5, 12, 0);
//             break;

//         case 8:
//             send_info.hour = v / 44;
//             OLED_ShowNum_n(x, y, send_info.hour, 5, 12, 0);
//             break;

//         case 9:
//             send_info.min = v / 17;
//             OLED_ShowNum_n(x, y, send_info.min, 5, 12, 0);
//             break;

//         case 10:
//             send_info.sec = v / 17;
//             OLED_ShowNum_n(x, y, send_info.sec, 5, 12, 0);
//             break;

//         case 11:
//             break;

//         default:
//             break;
//     }
// }

void cb_page_exec(void *arg)
{
    static uint32_t time_20ms = 0;

    if (Sys_Time > time_20ms)
    {
        time_20ms = Sys_Time + 20;
        send_info.throttle = ADC_Get_Throttle();
        int16_t v = send_info.throttle - 512;

        if (v > 0)
        {
            led_set_rgb(v >> 1, 0, 0);
        }
        else
        {
            v = -v;

            if (v > 511)
            {
                v = 511;
            }

            led_set_rgb(0, v >> 1, 0);
        }
    }
}
