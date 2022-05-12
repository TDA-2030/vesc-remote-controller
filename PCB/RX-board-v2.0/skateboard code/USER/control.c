
#include "sys.h"
#include "timer.h"
#include "adc.h"
#include "usart.h"

#include "24l01.h"
#include "control.h"
#include "main.h"

#include "comm_uart.h"
#include "bldc_interface.h"

#include "crc.h"
#include <string.h>

Tx_block_TypeDef Tx_block;
Rx_block_TypeDef Rx_block;

nRf24l01ModeType NRF_Mode=MODE_RX;   //NRFģ������ģʽ


//��ʼ��IO��
void control_Init(void)
{
    //�������� 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//ʱ��ʹ�� 
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	TIM1_PWM_config();
	TIM3_Int_Init();
	uart2_init(115200);	 //���ڳ�ʼ��
	bldc_comm_uart_init();// Initialize the bldc interface and provide a send function
	
    memset(&Rx_block,0,sizeof(Rx_block));
	memset(&Tx_block,0,sizeof(Tx_block));
}


uint8_t Upload_data(void) //����Ҫ�ϴ�������
{
    
    Tx_block.crc = crc16((uint8_t *)&Tx_block,sizeof(Tx_block));//д����ն�У����
    
    return 0;
}

//����Succeed���ɹ� ��CRC_Err��У��ʧ�� Resquest���ӵ��ϴ��������� 
uint8_t Exue_Control(uint8_t *buf)
{
    memcpy(&Rx_block, buf, sizeof(Rx_block));
	
    if(crc16((uint8_t *)&Rx_block,sizeof(Rx_block)) != Rx_block.crc) //�ж�ң�ض�У����
    {
        Rx_block.crc=0;//У��֮������
        return 1;
    }
    else Rx_block.crc=0;//У��֮������
	
	system.state = Rx_block.System_status;//����ϵͳ״̬
	
    return 0;
}



