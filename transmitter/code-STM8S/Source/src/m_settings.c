/*
 * @Author: zhouli
 * @Date: 2020-04-18 16:14:34
 * @LastEditTime: 2020-05-27 02:50:03
 * @Description: file content
 */

#include "type_def.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "key.h"
#include "gui_windows.h"
#include "control.h"
#include "stdio.h"
#include "timer.h"
#include "m_settings.h"
#include "math.h"

void page_main(void);


static uint8_t select = 0;
static uint8_t select_ok = 0;

typedef struct
{
    win_fun_t page_fun;
    const char *name;
    const uint8_t *icon;

} option_t;

void page_set_time(void);
void page_set_light(void);
void page_set_sys(void);

#define OPTIONS_MAX 3
static const option_t options[OPTIONS_MAX] =
{
    {page_set_time,  "time    ", icon_clock},
    {page_set_light, "lighting", icon_light_d},
    {page_set_sys,   "system  ", icon_setting},
};



static void cb_set_time_long(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_2 == btn)
    {
        win_set(page_setting);
    }
}
static void cb_set_time_single(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_1 == btn)
    {
        select_ok = 1;
    }
    else if (&btn_2 == btn)
    {
        select++;

        if (6 == select)
        {
            select = 0;
        }
    }
}

static void page_set_time(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            GUI_Clear();
            send_info.year = skate_info.year;
            send_info.month = skate_info.month;
            send_info.day = skate_info.day;
            send_info.hour = skate_info.hour;
            send_info.min = skate_info.min;
            send_info.sec = skate_info.sec;
            GUI_ShowString(24, 16, "    -  -  ", 16);
            GUI_ShowNum(24, 16, 2000 + send_info.year, 4, 16);
            GUI_ShowNum(64, 16, send_info.month, 2, 16);
            GUI_ShowNum(88, 16, send_info.day, 2, 16);
            GUI_ShowString(32, 16, "  -  -  ", 16);
            GUI_ShowNum(32, 16, send_info.hour, 2, 16);
            GUI_ShowNum(56, 16, send_info.min, 2, 16);
            GUI_ShowNum(80, 16, send_info.sec, 2, 16);
            button_attach(&btn_1, PRESS_DOWN, cb_set_time_single);
            button_attach(&btn_2, PRESS_DOWN, cb_set_time_single);
            button_attach(&btn_2, LONG_RRESS_START, cb_set_time_long);
            select = 0;
            break;

        case WIN_STATE_EXEC:
        {
            GUI_Line(0, 0, 3, 3);
        }
        break;

        case WIN_STATE_EXIT:
            break;

        default:
            break;
    }
}


static void page_set_light(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            GUI_Clear();
            GUI_ShowString(0, 0, options[select].name, 16);
            win_set_flash_time(20);
            

            break;

        case WIN_STATE_EXEC:
        {
    

          
        }
        break;

        case WIN_STATE_EXIT:
        win_set_flash_time(60);
            break;

        default:
            break;
    }
}

static void page_set_sys(void)
{
    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            GUI_Clear();
            GUI_ShowString(0, 0, options[select].name, 16);
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




static void cb_setting_long(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_2 == btn)
    {
        win_set(page_main);
    }
}

static void cb_setting_single(void *arg)
{
    struct Button *btn = (Button *)arg;

    if (&btn_1 == btn)
    {
        select_ok = 1;
    }
    else if (&btn_2 == btn)
    {
        select++;

        if (OPTIONS_MAX == select)
        {
            select = 0;
        }
    }
}
/**
 * @brief: --     --
 *         |        |
 *
 *         |        |
 *         --      --
 * @param {type}
 * @return:
 */
static void select_icon(uint16_t x0, uint16_t y0, uint16_t xx, uint16_t yy)
{
    GUI_Line(x0, y0, x0 + 5, y0);
    GUI_Line(x0, y0, x0, y0 + 5);
    GUI_Line(x0 + xx, y0, x0 + xx - 5, y0);
    GUI_Line(x0 + xx, y0, x0 + xx, y0 + 5);
    GUI_Line(x0 + xx, y0 + yy, x0 + xx - 5, y0 + yy);
    GUI_Line(x0 + xx, y0 + yy, x0 + xx, y0 + yy - 5);
    GUI_Line(x0, y0 + yy, x0 + 5, y0 + yy);
    GUI_Line(x0, y0 + yy, x0, y0 + yy - 5);
}
void page_setting(void)
{
    static uint8_t bak = 0;

    switch (win_get_state())
    {
        case WIN_STATE_INIT:
            GUI_Clear();
            GUI_ShowString(0, 0, "setings", 16);
            button_attach(&btn_2, LONG_RRESS_START, cb_setting_long);
            button_attach(&btn_1, PRESS_DOWN, cb_setting_single);
            button_attach(&btn_2, PRESS_DOWN, cb_setting_single);
            win_set_flash_time(30);

            for (size_t i = 0; i < OPTIONS_MAX; i++)
            {
                GUI_DrawBMP(18 + (i * 33), 20, 24, 24, options[i].icon);
            }

            select_icon(17 + 33 * select, 19, 26, 26);
            select = 0;
            break;

        case WIN_STATE_EXEC:
        {
            if (bak != select)
            {
                bak = select;
                GUI_ShowString(0, 0, options[select].name, 16);
                GUI_SetPointColor(0);

                if (select == 0)
                {
                    select_icon(17 + 33 * 2, 19, 26, 26);
                }
                else
                {
                    select_icon(17 + 33 * (select - 1), 19, 26, 26);
                }

                GUI_SetPointColor(1);
                select_icon(17 + 33 * select, 19, 26, 26);
            }

            if (select_ok)
            {
                select_ok = 0;
                win_set(options[select].page_fun);
            }
        }
        break;

        case WIN_STATE_EXIT:
            button_attach(&btn_1, PRESS_DOWN, NULL);
            button_attach(&btn_2, PRESS_DOWN, NULL);
            break;

        default:
            break;
    }
}



