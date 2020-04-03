
#include "type_def.h"
#include "main.h"
#include "oled.h"
#include "windows.h"

win_state_e g_win_state;
static win_fun_t current_win = NULL;
static win_fun_t previous_win = NULL;



static void (*exec_cb)(void) = NULL;


void win_set(win_fun_t set)
{
	if (NULL != set)
	{
		previous_win = current_win;
		current_win = set;
		g_win_state = WIN_STATE_INIT;
	}
}

win_fun_t win_get_current(void)
{
	return current_win;
}

win_fun_t win_get_previous(void)
{
	return previous_win;
}

void win_exec(void)
{
	static uint32_t refresh_time = 0;

	switch (g_win_state)
	{
	case WIN_STATE_INIT:
		if (NULL != current_win)
		{
			current_win();
		}
		OLED_Refresh_Gram();
		g_win_state = WIN_STATE_EXEC;
		break;
	case WIN_STATE_EXEC:
		if (Sys_Time > refresh_time)
		{
			if (NULL != exec_cb)
			{
				exec_cb();
			}
			
			if (NULL != current_win)
			{
				current_win();
			}
			refresh_time = Sys_Time + 60;
			OLED_Refresh_Gram();
		}
		break;
	default:
		break;
	}
}

