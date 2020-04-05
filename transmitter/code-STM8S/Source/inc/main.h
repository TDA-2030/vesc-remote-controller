#ifndef _MAIN_H__
#define _MAIN_H__

#define SETTING_FLASH_ADDR (0)
#define FLASH_CHECK (0x1278)   //检验值

#define MAJOR_VERSION 2
#define MINOR_VERSION 1


typedef struct
{
    uint16_t check;


    uint8_t light_en;
    uint8_t light1;
    uint8_t light2;

    uint16_t auto_off_time;

} Setting_TypeDef;
extern Setting_TypeDef setting;


typedef struct
{
    volatile uint8_t state;
    uint16_t auto_off_timer;
	uint16_t bat_vol;

} System_TypeDef;
extern System_TypeDef system;

enum
{
    SYSTEM_STATE_IDLE,     //已连接，未开机
    SYSTEM_STATE_RUNNING,  //运行中
    SYSTEM_STATE_CHARGING, //充电中
    SYSTEM_STATE_NO_SIGNAL,//信号丢失
};

extern uint32_t Sys_Time;

void setting_write(void);
void setting_read(void);

#endif