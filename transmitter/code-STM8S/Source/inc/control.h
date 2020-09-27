#ifndef __CONTROL_H
#define __CONTROL_H

#include "24l01.h"
  
#define TP4056_CHRG   PA_IDR_IDR2 //
#define TP4056_STDBY  PA_IDR_IDR1 //   



//需要下达的数据结构体
typedef struct {

    uint8_t status;     //系统状态 0：空闲，1：运行
    uint16_t throttle;             //油门 
    uint8_t direction;
    uint8_t light1;             //大灯控制1
    uint8_t light2;             //大灯控制2
	
	uint8_t year,month,day,hour,min,sec;

}send_info_t;
extern send_info_t send_info;

typedef struct {
  
        uint16_t speed;            //速度值
	uint32_t tacho;           //里程
        uint16_t tacho_single;    //单次里程
        uint16_t voltage;         //电池电压
	uint16_t mot_current;     //电机电流
	uint16_t bat_current;     //电池电流
	uint8_t board_temp;       //板子温度
	uint16_t wh_drawn;        //消耗能量
	uint16_t wh_regen;        //再生能量
        uint16_t ah_drawn;        //消耗能量
	uint16_t ah_regen;        //再生能量
        
        uint16_t charge_cur;
        uint16_t discharge_cur;
	
	uint8_t year,month,day,hour,min,sec;

}skate_info_t;
extern skate_info_t skate_info;



extern nRf24l01ModeType NRF_Mode;
extern u16 TX_succeed_cnt;
extern u8 Sys_Tx_Rate;


extern u8 nrf_tx_buf[];
extern u8 nrf_rx_buf[];

void control_Init(void);
void control_power(uint8_t state);
u8 Rx_Data_Handle(void);
u8 Upload_data(void);
void prepare_upload_cmd(void);


#endif
