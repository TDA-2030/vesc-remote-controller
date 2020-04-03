#ifndef __WINDOWS_H__
#define __WINDOWS_H__

typedef void (*win_fun_t)(void);
typedef enum
{
	WIN_STATE_INIT,
	WIN_STATE_EXEC,
} win_state_e;
extern win_state_e g_win_state;

void win_exec(void);
void win_set(win_fun_t set);
win_fun_t win_get_current(void);
win_fun_t win_get_previous(void);
void win_init(void);


#endif
