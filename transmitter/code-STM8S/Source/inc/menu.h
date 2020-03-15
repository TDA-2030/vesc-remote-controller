#ifndef _MENU_H__
#define _MENU_H__


void win_exec(void);
void win_set(win_fun_t *set);
win_fun_t *win_get_current(void);
win_fun_t *win_get_previous(void);
void win_init(void);


#endif