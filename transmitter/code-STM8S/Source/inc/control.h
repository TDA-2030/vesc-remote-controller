#ifndef __CONTROL_H
#define __CONTROL_H

  
#define LED_CHRG   PA_IDR_IDR2 //
#define LED_STDBY  PA_IDR_IDR1 //   





//��Ҫ�´�����ݽṹ��
typedef struct {

    uint8_t status;     //ϵͳ״̬ 0�����У�1������
    uint16_t throttle;             //���� 
    uint8_t direction;
    uint8_t light1;             //��ƿ���1
    uint8_t light2;             //��ƿ���2
	
	uint8_t year,month,day,hour,min,sec;

}send_info_t;
extern send_info_t send_info;

typedef struct {
  
        uint16_t speed;            //�ٶ�ֵ
	uint32_t tacho;           //���
        uint16_t tacho_single;    //�������
        uint16_t voltage;         //��ص�ѹ
	uint16_t mot_current;     //�������
	uint16_t bat_current;     //��ص���
	uint8_t board_temp;       //�����¶�
	uint16_t wh_drawn;        //��������
	uint16_t wh_regen;        //��������
        uint16_t ah_drawn;        //��������
	uint16_t ah_regen;        //��������
        
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

void Control_Init(void);
u8 Rx_Data_Handle(void);
u8 Upload_data(void);
void prepare_upload_cmd(void);


#endif
