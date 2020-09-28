#ifndef _MAIN_H__
#define _MAIN_H__

#define SETTING_FLASH_ADDR (0)
#define FLASH_CHECK (0x1278)   //检验值

#define MAJOR_VERSION 2
#define MINOR_VERSION 1


typedef enum {
    SPEED_STYLE_NUMBER,
    SPEED_STYLE_DASHBOARD,
    SPEED_STYLE_CURVE,
}speed_show_style_t;

typedef enum
{
    SYSTEM_STATE_IDLE,     //已连接，未开机
    SYSTEM_STATE_RUNNING,  //运行中
    SYSTEM_STATE_CHARGING, //充电中
    SYSTEM_STATE_NO_SIGNAL,//信号丢失
}sys_status_t;

typedef struct
{
    uint16_t check;

    speed_show_style_t speed_style;

    uint8_t light_en;
    uint8_t light1;
    uint8_t light2;

    uint16_t auto_off_time;

} Setting_TypeDef;
extern Setting_TypeDef setting;


typedef struct
{
    volatile sys_status_t state;
    uint16_t auto_off_timer;
	uint16_t bat_vol;

} System_TypeDef;
extern System_TypeDef system;



extern uint32_t Sys_Time;

void setting_write(void);
void setting_read(void);

#endif