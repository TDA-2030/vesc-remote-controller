#ifndef  __KEY_EXTI_H
#define  __KEY_EXTI_H

#define BitStatus bool
#define PD5_In  (BitStatus)(PD_IDR&0x20)
void EXIT_conf(void);

#endif



