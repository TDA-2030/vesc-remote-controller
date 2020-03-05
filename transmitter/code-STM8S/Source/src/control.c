
#include "type_def.h"
#include "24l01.h"
#include "delay.h"
#include "string.h"
#include "control.h"
#include "adc.h"
#include "exti.h"
#include "crc.h"
#include "menu.h"



//���պͷ������ݻ�������
u8 nrf_tx_buf[32]={0};
u8 nrf_rx_buf[32]={0};

skate_info_t skate_info={0};
send_info_t send_info={0};



nRf24l01ModeType NRF_Mode=MODE_TX;   //NRFģ������ģʽ
u16 TX_succeed_cnt=0;  //���ͳɹ�������
u8 Sys_Tx_Rate=0;

enum{
	nrf_data_speed,//�ٶ�ֵ
	nrf_data_tacho_single,//�������
	nrf_data_tacho,//���
	nrf_data_voltage,//��ص�ѹ
	nrf_data_mot_current,//�������
	nrf_data_bat_current,//��ص��� <32768��ʾ�ŵ磬>32768����ʾ���
	nrf_data_board_temp,//�����¶�
	nrf_data_wh_drawn,//��������
	nrf_data_wh_regen,//��������
	nrf_data_ah_drawn,//��������
	nrf_data_ah_regen,//��������
	nrf_data_charge_cur,
	nrf_data_discharge_cur,
	nrf_data_year,
	nrf_data_month,
	nrf_data_day,
	nrf_data_hour,
	nrf_data_min,
	nrf_data_sec,
	
	nrf_data_crc,
	nrf_data_frameid,
	
	nrf_data_throttle,//���� <1024ɲ����>1024����
	nrf_data_light1,//��ƿ���1
	nrf_data_light2,//��ƿ���2
	nrf_data_status,//ϵͳ״̬ 0�����У�1������
	
	nrf_data_MAX,
};

static const uint8_t data2bytes[nrf_data_MAX][2]={
	{nrf_data_speed        ,2},
	{nrf_data_tacho_single ,2},
	{nrf_data_tacho        ,4},
	{nrf_data_voltage      ,2},
	{nrf_data_mot_current  ,2},
	{nrf_data_bat_current  ,2},
	{nrf_data_board_temp   ,1},
	{nrf_data_wh_drawn     ,2},
	{nrf_data_wh_regen     ,2},
	{nrf_data_ah_drawn     ,2},
	{nrf_data_ah_regen     ,2},
	{nrf_data_charge_cur   ,2},
	{nrf_data_discharge_cur,2},
	{nrf_data_year         ,1},
	{nrf_data_month        ,1},
	{nrf_data_day          ,1},
	{nrf_data_hour         ,1},
	{nrf_data_min          ,1},
	{nrf_data_sec          ,1},
	
	{nrf_data_crc          ,2},
	{nrf_data_frameid      ,1},
	
	{nrf_data_throttle     ,2},
	{nrf_data_light1       ,2},
	{nrf_data_light2       ,2},
	{nrf_data_status       ,1},
	
};

enum{
	RX_FRAME_1,
	RX_FRAME_2,
	RX_FRAME_MAX,
};

enum{
	TX_FRAME_1,
	TX_FRAME_2,
	TX_FRAME_MAX,
	
};


void AddToArray(uint8_t **pt,uint8_t d_type,uint32_t dat)
{
	uint8_t *p=*pt;
	
	if(1 == data2bytes[d_type][1])
	{
		uint8_t d = dat;
		*p = d;
	}
	else if(2 == data2bytes[d_type][1])
	{
		uint16_t d = dat;
		*p++ = (uint8_t)(d>>8);
		*p++ = (uint8_t)(d&0x00ff);
	}
	else if(4 == data2bytes[d_type][1])
	{
		uint32_t d = dat;
		*p++ = (uint8_t)(d>>24);
		*p++ = (uint8_t)((d&0x00ff0000)>>16);
		*p++ = (uint8_t)((d&0x0000ff00)>>8);
		*p++ = (uint8_t)(d&0x000000ff);
		
	}
	*pt+=data2bytes[d_type][1];
}

uint32_t ParseToData(uint8_t **pt,uint8_t d_type)
{
	uint32_t r;
	uint8_t *p=*pt;
	
	if(1 == data2bytes[d_type][1])
	{
		r = (uint32_t)(*p++);
	}
	else if(2 == data2bytes[d_type][1])
	{
		r  = ((uint32_t)(*p++)<<8);
		r |= ((uint32_t)(*p++));
	}
	else if(4 == data2bytes[d_type][1])
	{
		r  = ((uint32_t)(*p++)<<24);
		r |= ((uint32_t)(*p++)<<16);
		r |= ((uint32_t)(*p++)<<8);
		r |= ((uint32_t)(*p++));
	}
	else r=0;
	
	*pt+=data2bytes[d_type][1];
	return r;
}

void Control_Init(void)
{
  	PA_DDR &= ~(3<<1);//����Ϊ����ģʽ
	PA_CR1 |= (3<<1);//����Ϊ��������
	PA_CR2 &= ~(3<<1);//����û���ж�

}



u8 Rx_Data_Handle(void)
{
    uint8_t* ptr = nrf_rx_buf;
    uint8_t fid;
  
    if(crc16(nrf_rx_buf+2,sizeof(nrf_rx_buf)-2) != ParseToData(&ptr,nrf_data_crc)) //�ж�ң�ض�У����
    //if(ParseToData(&ptr,nrf_data_crc) != 0x0607)
    {
        return 1;
    }
    
    fid = ParseToData(&ptr,nrf_data_frameid);//
    switch(fid)
    {
        case RX_FRAME_1:
        {
          skate_info.speed        = ParseToData(&ptr,nrf_data_speed);
          skate_info.tacho_single = ParseToData(&ptr,nrf_data_tacho_single);
          skate_info.ah_drawn     = ParseToData(&ptr,nrf_data_ah_drawn);
          skate_info.ah_regen     = ParseToData(&ptr,nrf_data_ah_regen);
          skate_info.year         = ParseToData(&ptr,nrf_data_year);
          skate_info.month        = ParseToData(&ptr,nrf_data_month);
          skate_info.day          = ParseToData(&ptr,nrf_data_day);
          skate_info.hour         = ParseToData(&ptr,nrf_data_hour);
          skate_info.min          = ParseToData(&ptr,nrf_data_min);
          skate_info.sec          = ParseToData(&ptr,nrf_data_sec);
        }break;
        
        case RX_FRAME_2:
        {
          skate_info.voltage       = ParseToData(&ptr,nrf_data_voltage);
          skate_info.bat_current   = ParseToData(&ptr,nrf_data_bat_current);
          skate_info.mot_current   = ParseToData(&ptr,nrf_data_mot_current);
          skate_info.board_temp    = ParseToData(&ptr,nrf_data_board_temp);
          skate_info.charge_cur    = ParseToData(&ptr,nrf_data_charge_cur);
          skate_info.discharge_cur = ParseToData(&ptr,nrf_data_discharge_cur);
	  skate_info.wh_drawn     = ParseToData(&ptr,nrf_data_wh_drawn);
          skate_info.wh_regen     = ParseToData(&ptr,nrf_data_wh_regen);
          
        }break;
        default:break;
    }

    return 0;
}

u8 Upload_data(void)
{
    uint8_t *ptr = nrf_tx_buf;
    static uint8_t fid=TX_FRAME_1;

    memset(&nrf_tx_buf,0,sizeof(nrf_tx_buf));
    ptr+=data2bytes[nrf_data_crc][1];
	
	if(chg_time==1)
	{
		chg_time=0;
		fid = TX_FRAME_2;
	}
		
    AddToArray(&ptr,nrf_data_frameid,fid);
	
    switch(fid)
    {
        case TX_FRAME_1:
            AddToArray(&ptr,nrf_data_status,send_info.status);
			if(send_info.direction==0)
            	AddToArray(&ptr,nrf_data_throttle,send_info.throttle+32768);
			else
				AddToArray(&ptr,nrf_data_throttle,send_info.throttle);
            AddToArray(&ptr,nrf_data_light1,send_info.light1);
            AddToArray(&ptr,nrf_data_light2,send_info.light2);
            fid = TX_FRAME_1;
        break;
		
		case TX_FRAME_2:
            AddToArray(&ptr,nrf_data_year,send_info.year);
            AddToArray(&ptr,nrf_data_month,send_info.month);
            AddToArray(&ptr,nrf_data_day,send_info.day);
            AddToArray(&ptr,nrf_data_hour,send_info.hour);
			AddToArray(&ptr,nrf_data_min,send_info.min);
			AddToArray(&ptr,nrf_data_sec,send_info.sec);
            fid = TX_FRAME_1;
        break;
        default:break;
    }
    
    ptr = nrf_tx_buf;
    AddToArray(&ptr,nrf_data_crc,crc16(nrf_tx_buf+2,sizeof(nrf_tx_buf)-2));//д����ն�У����
    //AddToArray(&ptr,nrf_data_crc,0x0006);    //д��ң�ض�У����
    return 0;
}


