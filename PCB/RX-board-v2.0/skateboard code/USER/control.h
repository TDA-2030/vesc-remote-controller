#ifndef __CONTROL_H
#define __CONTROL_H


//端口定义

#define POWER_EN  PBout(0) //GPIO_Pin_0 //
#define MOS_TRIG  PBout(1) //GPIO_Pin_1 //


#define MAX_LOST_TIME 25  //25*10 = 250ms
#define LOST_PD_TIME  1000  //1000*10 = 10s


//需要解析执行的数据结构体
typedef struct {
	
    uint16_t crc;               //校验码
    uint8_t System_status;     //系统状态
    uint16_t speed;            //速度值
	uint8_t light;             //大灯控制
}Rx_block_TypeDef;



//需要上传的数据结构体
typedef struct {
	
    uint16_t crc;
 	uint8_t System_status;   //系统状态
    uint16_t speed;           //速度值
    uint16_t voltage;       //电池电压
}Tx_block_TypeDef;  


extern Tx_block_TypeDef Tx_block;
extern Rx_block_TypeDef Rx_block;

extern nRf24l01ModeType NRF_Mode;


void control_Init(void);
uint8_t Exue_Control(uint8_t *buf);
uint8_t Upload_data(void);


#endif
