/*****************************************************************
*  @file     : xx.x            
*  @brief    :     
*  Details.          
*                       
*  @author   : zhouli        
*  @version  : xx.xx         
*  @date     : 2017-xx-xx    
*                           
*  Change History 
******************************************************************/



#include "main.h"
#include "24l01.h"
#include "24l01_protocol.h"





#define REINIT_TIME  13  //13*10 = 130ms
#define MAX_LOST_TIME 25  //25*10 = 250ms
#define LOST_PD_TIME  1000  //1000*10 = 10s


enum{
	PROTOCOL_TX_FRAME_1,
	PROTOCOL_TX_FRAME_2,
	PROTOCOL_TX_FRAME_3,
};

enum{
	PROTOCOL_RX_FRAME_1,
	
};


uint8_t nrf24l01_status = 0;

nRf24l01ModeType NRF_Mode=MODE_RX;   //NRF模块连接模式
static uint8_t nrf_tx_buf[NRF_FIXED_PACKET_LEN],nrf_rx_buf[NRF_FIXED_PACKET_LEN];


static uint16_t notfound_time=0;  //丢失信号时间计数器


void NRF24L01_Protocol_Init(void)
{
	
}



void NRF24L01_Protocol_Exec(void)
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
			nrf24l01_status = NRF_STATUS_DISCONNECT; //计数器满，说明长时间没有信号
			
		}
		if(notfound_time == REINIT_TIME)
		{
//				RF24L01_Init();
//				RF24L01_Set_Mode(MODE_RX);//设置为接收模式
		}
	}else
	{
		nrf24l01_status = NRF_STATUS_CONNECTED;
	}
}



void NRF24L01_Protocol_Irq_Handle(void)
{
	uint8_t status;
	uint8_t RxLength;
	
	if((RF24L01_GET_IRQ_STATUS( )==0))
	{
		status=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,status); //清除TX_DS或MAX_RT中断标志
		if(status&MAX_TX)
		{
			NRF24L01_Write_Reg( FLUSH_TX,0xff );	//清除TX FIFO寄存器
		}
		else if(status&RX_OK)
		{
			RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//读取接收到的数据个数
			if(RxLength<=32)
			{
				NRF24L01_Read_Buf( RD_RX_PLOAD,nrf_rx_buf,RxLength );	//接收到数据
				NRF24L01_Write_Reg( FLUSH_RX,0xff );				//清除RX FIFO
				if(Exue_Control()==0)
				{
					notfound_time=0; //接收到数据，清空计数器
					RF24L01_Set_Mode(MODE_TX);
					NRF_Mode=MODE_TX;
				}
			}
		}
		else if(status&TX_OK)
		{
			NRF24L01_Write_Reg( FLUSH_TX,0xff );	//清除TX FIFO寄存器
			notfound_time=0; //发送成功数据，清空计数器
			
			NRF_Mode=MODE_RX;
			RF24L01_Set_Mode(MODE_RX);
		}
		
	}
}



