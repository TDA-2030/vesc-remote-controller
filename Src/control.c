
#include "main.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "rtc.h"

#include "24l01.h"
#include "control.h"

#include "comm_uart.h"
#include "bldc_interface.h"

#include "crc.h"
#include <string.h>




#define REINIT_TIME  13  //13*10 = 130ms
#define MAX_LOST_TIME 25  //25*10 = 250ms
#define LOST_PD_TIME  1000  //1000*10 = 10s


enum{
	TX_FRAME_1,
	TX_FRAME_2,
	TX_FRAME_MAX,
};

enum{
	RX_FRAME_1,
	RX_FRAME_2,
	RX_FRAME_MAX,
	
};



nRf24l01ModeType NRF_Mode=MODE_RX;   //NRFģ������ģʽ
static uint8_t nrf_tx_buf[NRF_FIXED_PACKET_LEN],
		nrf_rx_buf[NRF_FIXED_PACKET_LEN];
static uint16_t notfound_time=0;  //��ʧ�ź�ʱ�������


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




uint8_t Exue_Control(void);
uint8_t Upload_data(void);


void nrf_int_handler(void)
{
	uint8_t status;
	uint8_t RxLength;
	
	status=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,status); //���TX_DS��MAX_RT�жϱ�־
	if(status&MAX_TX)
	{
		NRF24L01_Write_Reg( FLUSH_TX,0xff );	//���TX FIFO�Ĵ���
	}
	else if(status&RX_OK)
	{
		RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
		if(RxLength<=32)
		{
			NRF24L01_Read_Buf( RD_RX_PLOAD,nrf_rx_buf,RxLength );	//���յ�����
			NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
			if(Exue_Control()==0)
			{
				notfound_time=0; //���յ����ݣ���ռ�����
				RF24L01_Set_Mode(MODE_TX);
				NRF_Mode=MODE_TX;
			}
		}
	}
	else if(status&TX_OK)
	{
		NRF24L01_Write_Reg( FLUSH_TX,0xff );	//���TX FIFO�Ĵ���
		notfound_time=0; //���ͳɹ����ݣ���ռ�����
		
		NRF_Mode=MODE_RX;
		RF24L01_Set_Mode(MODE_RX);
	}
}

/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(&htim6 == htim)
	{
		if(NRF_Mode == MODE_TX)
		{
			Upload_data();
			NRF24L01_TxPacket(nrf_tx_buf,32);
		}
		else if(NRF_Mode == MODE_RX)
		{
			
		}
		
		if(notfound_time < LOST_PD_TIME)
		{
			notfound_time++;
			if(notfound_time == MAX_LOST_TIME)
			{
				system.state = SYSTEM_STATE_NO_SIGNAL; //����������˵����ʱ��û���ź�
				throttle_set(THROTTLE_MAX/2);//����ֹͣ���
			}
			if(notfound_time == REINIT_TIME)
			{
//				RF24L01_Init();
//				RF24L01_Set_Mode(MODE_RX);//����Ϊ����ģʽ
			}
		}else
		{
			system.state = SYSTEM_STATE_STANDBY;
		}
	}


}




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


//��ʼ��IO��
void control_Init(void)
{
	exit3_callback = nrf_int_handler;
	
	throttle_init();
	light_init();
	bootstrap_init();
	
	MX_TIM6_Init();
	HAL_TIM_Base_Start_IT(&htim6);
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,NRF24L01_Read_Reg(STATUS)); //���TX_DS��MAX_RT�жϱ�־
	NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
	NRF24L01_Write_Reg( FLUSH_TX,0xff );				//���TX FIFO
	
}




uint8_t Upload_data(void) //����Ҫ�ϴ�������
{
     uint8_t *ptr = nrf_tx_buf;
	static uint8_t fid=TX_FRAME_1;

	memset(&nrf_tx_buf,0,sizeof(nrf_tx_buf));
	
	ptr+=data2bytes[nrf_data_crc][1];
	AddToArray(&ptr,nrf_data_frameid,fid);
	switch(fid)
	{
		case TX_FRAME_1:
			AddToArray(&ptr,nrf_data_speed        ,SampleData.speed);
			AddToArray(&ptr,nrf_data_tacho_single ,SampleData.tacho_single);
			AddToArray(&ptr,nrf_data_ah_drawn     ,SampleData.ah_drawn);
			AddToArray(&ptr,nrf_data_ah_regen     ,SampleData.ah_regen);
			AddToArray(&ptr,nrf_data_year         ,calendar.w_year);
			AddToArray(&ptr,nrf_data_month        ,calendar.w_month);
			AddToArray(&ptr,nrf_data_day          ,calendar.w_date);
			AddToArray(&ptr,nrf_data_hour         ,calendar.hour);
			AddToArray(&ptr,nrf_data_min          ,calendar.min);
			AddToArray(&ptr,nrf_data_sec          ,calendar.sec);
			fid = TX_FRAME_2;
		break;
		
		case TX_FRAME_2:
			AddToArray(&ptr,nrf_data_voltage      ,SampleData.vin);
			AddToArray(&ptr,nrf_data_bat_current  ,SampleData.bat_current);
			AddToArray(&ptr,nrf_data_mot_current  ,SampleData.mot_current);
			AddToArray(&ptr,nrf_data_board_temp   ,SampleData.board_temp);
			AddToArray(&ptr,nrf_data_charge_cur   ,SampleData.charge_cur);
			AddToArray(&ptr,nrf_data_discharge_cur,SampleData.discharge_cur);
			AddToArray(&ptr,nrf_data_wh_drawn     ,SampleData.wh_drawn);
			AddToArray(&ptr,nrf_data_wh_regen     ,SampleData.wh_regen);
			fid = TX_FRAME_1;
		break;
		default:break;
	}
	
	ptr = nrf_tx_buf;
    AddToArray(&ptr,nrf_data_crc,crc16(nrf_tx_buf+2,sizeof(nrf_tx_buf)-2));//д����ն�У����
	//AddToArray(&ptr,nrf_data_crc,0x0607);//д����ն�У����
    return 0;
}


//����Succeed���ɹ� ��CRC_Err��У��ʧ�� Resquest���ӵ��ϴ��������� 
uint8_t Exue_Control(void)
{
	uint8_t* ptr = nrf_rx_buf;
	uint8_t fid;
	
		
    if(crc16(nrf_rx_buf+2,sizeof(nrf_rx_buf)-2) != ParseToData(&ptr,nrf_data_crc)) //�ж�ң�ض�У����
	//if(ParseToData(&ptr,nrf_data_crc) != 0x0006)
    {
        return 1;
    }
	fid = ParseToData(&ptr,nrf_data_frameid);//
	switch(fid)
	{
		case RX_FRAME_1:
		{
			uint16_t throttle,light1,light2;
			uint8_t state;
			
			state = ParseToData(&ptr,nrf_data_status);//����ϵͳ״̬
			if(SYSTEM_STATE_NO_SIGNAL == system.state)
			{
				system.state = SYSTEM_STATE_IDLE;
				state = !bootstrap_isenable();
			}
			
			{
				if((SYSTEM_STATE_IDLE == system.state)&&(state==1))
				{
					system.state = SYSTEM_STATE_RUNNING;
					if(bootstrap_isenable()!=state)
						bootstrap_enable(1);
				}
				else if((SYSTEM_STATE_RUNNING == system.state)&&(state==0))
				{
					system.state = SYSTEM_STATE_IDLE;
					if(bootstrap_isenable()!=state)
						bootstrap_enable(0);
				}
				
			}
			if(SYSTEM_STATE_STANDBY == system.state)
			{
				system.state = SYSTEM_STATE_IDLE;
			}
			
			throttle = ParseToData(&ptr,nrf_data_throttle);
			light1 = ParseToData(&ptr,nrf_data_light1);
			light2 = ParseToData(&ptr,nrf_data_light2);
			if(SYSTEM_STATE_RUNNING == system.state)
			{
				if(throttle<32768)
				{
					
					throttle_set((uint16_t)(throttle)); //����
				}else
				{
					throttle-=32768;
					throttle_set((uint16_t)(throttle));//����
				}
			}
			else
			{
				throttle_set(THROTTLE_MAX/2);
			}
			light_set(1,light1);
			light_set(2,light2);
		}break;
		case RX_FRAME_2:
		{
			uint8_t d[6];
			
			d[0] = ParseToData(&ptr,nrf_data_year);
			d[1] = ParseToData(&ptr,nrf_data_month);
			d[2] = ParseToData(&ptr,nrf_data_day);
			d[3] = ParseToData(&ptr,nrf_data_hour);
			d[4] = ParseToData(&ptr,nrf_data_min);
			d[5] = ParseToData(&ptr,nrf_data_sec);
			
			RTC_Set(d[0],d[1],d[2],d[3],d[4],d[5]);  //����ʱ��
		}break;
		default:break;
	}
	
	
    return 0;
}



