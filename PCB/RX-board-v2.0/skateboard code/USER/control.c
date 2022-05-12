
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

nRf24l01ModeType NRF_Mode=MODE_RX;   //NRF模块连接模式


//初始化IO口
void control_Init(void)
{
    //引脚配置 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//时钟使能 
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	TIM1_PWM_config();
	TIM3_Int_Init();
	uart2_init(115200);	 //串口初始化
	bldc_comm_uart_init();// Initialize the bldc interface and provide a send function
	
    memset(&Rx_block,0,sizeof(Rx_block));
	memset(&Tx_block,0,sizeof(Tx_block));
}


uint8_t Upload_data(void) //更新要上传的数据
{
    
    Tx_block.crc = crc16((uint8_t *)&Tx_block,sizeof(Tx_block));//写入接收端校验码
    
    return 0;
}

//返回Succeed：成功 ，CRC_Err：校验失败 Resquest：接到上传请求命令 
uint8_t Exue_Control(uint8_t *buf)
{
    memcpy(&Rx_block, buf, sizeof(Rx_block));
	
    if(crc16((uint8_t *)&Rx_block,sizeof(Rx_block)) != Rx_block.crc) //判断遥控端校验码
    {
        Rx_block.crc=0;//校验之后清零
        return 1;
    }
    else Rx_block.crc=0;//校验之后清零
	
	system.state = Rx_block.System_status;//设置系统状态
	
    return 0;
}



