/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:40:20
 * @Description: file content
 */


#include "type_def.h"
#include "main.h"
#include "oled.h"
#include "gui_windows.h"

static win_state_e g_win_state = WIN_STATE_INIT;
static win_fun_t g_current_win = NULL;
static win_fun_t g_previous_win = NULL;
static uint8_t g_flash_time;


static void (*g_exec_cb)(void *) = NULL;

void win_init(void)
{
    g_flash_time = 50;
}

void win_set(win_fun_t set)
{
    if (NULL != set)
    {
        g_previous_win = g_current_win;
        g_current_win = set;
        g_win_state = WIN_STATE_EXIT;
    }
}

win_fun_t win_get_current(void)
{
    return g_current_win;
}

win_fun_t win_get_previous(void)
{
    return g_previous_win;
}

win_state_e win_get_state(void)
{
    return g_win_state;
}

void win_set_flash_time(uint8_t n_ms)
{
    g_flash_time = n_ms;
}
void win_set_exec_callback(void (*cb)(void*))
{
    if (NULL != cb)
    {
        g_exec_cb = cb;
    }
}


void win_exec(void)
{
    static uint32_t refresh_time = 0;

    switch (g_win_state)
    {
        case WIN_STATE_INIT:
            if (NULL != g_current_win)
            {
                g_current_win();
            }

            OLED_Refresh_Gram();
            g_win_state = WIN_STATE_EXEC;
            break;

        case WIN_STATE_EXEC:
            if (NULL != g_exec_cb)
            {
                g_exec_cb((void*)g_current_win);
            }

            if (NULL != g_current_win)
            {
                g_current_win();
            }

            if (Sys_Time > refresh_time)
            {
                refresh_time = Sys_Time + g_flash_time;
                OLED_Refresh_Gram();
            }

            break;

        case WIN_STATE_EXIT:
            if (NULL != g_previous_win)
            {
                g_previous_win();
            }

            g_win_state = WIN_STATE_INIT;
            break;

        default:
            break;
    }
}

