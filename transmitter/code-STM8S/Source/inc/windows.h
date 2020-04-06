#ifndef __WINDOWS_H__
#define __WINDOWS_H__

typedef void (*win_fun_t)(void);
typedef enum
{
	WIN_STATE_INIT,
	WIN_STATE_EXEC,
	WIN_STATE_EXIT,
} win_state_e;

void win_init(void);
void win_exec(void);
void win_set(win_fun_t set);
win_fun_t win_get_current(void);
win_fun_t win_get_previous(void);
win_state_e win_get_state(void);
void win_set_flash_time(uint8_t n_ms);
void win_set_exec_callback(void (*cb)(void*));


#endif
