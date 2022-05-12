#ifndef __CONTROL_H
#define __CONTROL_H


//�˿ڶ���

#define POWER_EN  PBout(0) //GPIO_Pin_0 //
#define MOS_TRIG  PBout(1) //GPIO_Pin_1 //


#define MAX_LOST_TIME 25  //25*10 = 250ms
#define LOST_PD_TIME  1000  //1000*10 = 10s


//��Ҫ����ִ�е����ݽṹ��
typedef struct {
	
    uint16_t crc;               //У����
    uint8_t System_status;     //ϵͳ״̬
    uint16_t speed;            //�ٶ�ֵ
	uint8_t light;             //��ƿ���
}Rx_block_TypeDef;



//��Ҫ�ϴ������ݽṹ��
typedef struct {
	
    uint16_t crc;
 	uint8_t System_status;   //ϵͳ״̬
    uint16_t speed;           //�ٶ�ֵ
    uint16_t voltage;       //��ص�ѹ
}Tx_block_TypeDef;  


extern Tx_block_TypeDef Tx_block;
extern Rx_block_TypeDef Rx_block;

extern nRf24l01ModeType NRF_Mode;


void control_Init(void);
uint8_t Exue_Control(uint8_t *buf);
uint8_t Upload_data(void);


#endif
