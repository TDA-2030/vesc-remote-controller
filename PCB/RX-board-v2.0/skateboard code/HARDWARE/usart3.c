/*****************************************************************
*  @file     : usart3.c            
*  @brief    :     
*  Details.          
*                       
*  @author   : zhouli        
*  @version  : xx.xx         
*  @date     : 2017-12-29    
*                           
*  Change History 
******************************************************************/
#include "usart3.h"
#include "control.h"
//#include <math.h>


//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//���ջ���,���USART3_MAX_RECV_LEN���ֽ�

Usart3CMD_t Usart3Cmds = {NONEACT,NONECMD,0};  //����3���յ�����ṹ��

/**************************************************************************
�������ܣ�����3��ʼ��
��ڲ�����pclk2:PCLK2 ʱ��Ƶ��(Mhz)    bound:������
����  ֵ����
**************************************************************************/
void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������
	mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���ʱ�� 
	GPIOB->CRH&=0XFFFF00FF; 
	GPIOB->CRH|=0X00008B00;//IO״̬����
	GPIOB->ODR|=1<<10;	  
	RCC->APB1RSTR|=1<<18;   //��λ����1
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ
	//����������
 	USART3->BRR=mantissa; // ����������
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	USART3->CR1|=1<<8;    //PE�ж�ʹ��
	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(0,1,USART3_IRQn,2);//��2��������ȼ�
	
	Usart3Cmds.flag_usart3 = 1;
}
/** 
* @brief  : ����3,printf ����,ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
* @param  : 
* @param  : 
* @return : 
*/
//void u3_printf(char* fmt,...)  
//{  
//	u16 i,j; 
//	va_list ap; 
//	va_start(ap,fmt);
//	vsprintf((char*)USART3_TX_BUF,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
//	for(j=0;j<i;j++)							//ѭ����������
//	{
//		while((USART3->SR&0X40)==0);			//ѭ������,ֱ���������   
//		USART3->DR=USART3_TX_BUF[j];
//	} 
//}

/** 
* @brief  : ����3�����ж�
* @param  : 
* @param  : 
* @return : 
*/
void USART3_IRQHandler(void)
{
//	OSIntEnter();
	if(USART3->SR&(1<<5))//���յ�����
	{
		 u8 uart_receive=0;//����������ر���
		 static u8 Flag_PID,i;
		 static uint32_t Data;
		
		 uart_receive=(u8)USART3->DR;
		
//		if(uart_receive==0x59)  Flag_sudu=2;  //���ٵ�
//		if(uart_receive==0x58)  Flag_sudu=1;  //���ٵ�   mini�治����ٵ�
		
		//if(uart_receive>10)  //Ĭ��ʹ��app
		{
			if(uart_receive=='A')  Usart3Cmds.action |= FORWARD,Usart3Cmds.action &= ~BACKWARD;  //ǰ
			if(uart_receive=='E')  Usart3Cmds.action |= BACKWARD,Usart3Cmds.action &= ~FORWARD;  //��
			if(uart_receive=='B'||uart_receive=='C'||uart_receive=='D')	 //��
			{
				Usart3Cmds.action |= RIGHT,Usart3Cmds.action &= ~LEFT;
			}
			else if(uart_receive=='F'||uart_receive=='G'||uart_receive=='H')  //��
			{
				Usart3Cmds.action |= LEFT,Usart3Cmds.action &= ~RIGHT;
			}
			else 
				Usart3Cmds.action &= ~LEFT,Usart3Cmds.action &= ~RIGHT; //ֱ��
		}
		
		if(uart_receive=='{') Flag_PID=1;  //ָ����ʼλ
		if(uart_receive=='}') Flag_PID=2;  //ָ��ֹͣλ
		if(Flag_PID==1)                      //��¼����
		{
			USART3_RX_BUF[i++]=uart_receive;
		}
		if(Flag_PID==2)   //ִ��
		{
			if(USART3_RX_BUF[1] == 'Q')  //������λ����������request
			{
				switch(USART3_RX_BUF[3])
				{
					case 'P': Usart3Cmds.cmd |= PARAMETERBACK;  //��ȡ�豸����
					break;
					case 'R': Usart3Cmds.cmd |= FLASHREAD;      //��ȡ�������
					break;
					case 'D': Usart3Cmds.cmd |= FLASHREAD|DEFAULTPARA;    //��ȡĬ�ϲ���
					break;
					case 'W': Usart3Cmds.cmd |= FLASHWRITE;     //���籣�����
					break;
					case 'B': Usart3Cmds.cmd |= SYSTEMREBOOT;     //�����豸
					break;
					case 'S': BalanceDev.BalancePoint = BalanceDev.Angle_Balance;     //���õ�ǰ�Ƕ�Ϊ��λ
					break;
					default:break;
				}
			}
			else if(USART3_RX_BUF[1]!='#')                    //����PID����
			{
				u8 j;
				for(j=3;j<=i-1;j++)
				{
					USART3_RX_BUF[j] -= '0';
					Data = (((Data << 2) + Data) << 1) + USART3_RX_BUF[j]; // Data*10 + c
					//Data+=(USART3_RX_BUF[j-1])*pow(10,i-j);
				}
				switch(USART3_RX_BUF[1])
				{
					case 0x30:  BalanceDev.BalancePID.P=(float)Data/APP_MULTIPLE;break;
					case 0x31:  BalanceDev.BalancePID.I=(float)Data/APP_MULTIPLE;break;
					case 0x32:  BalanceDev.BalancePID.D=(float)Data/APP_MULTIPLE;break;
					case 0x33:  BalanceDev.ServoPID.P=(float)Data/APP_MULTIPLE;break;
					case 0x34:  BalanceDev.ServoPID.I=(float)Data/APP_MULTIPLE;break;
					case 0x35:  BalanceDev.ServoPID.D=(float)Data/APP_MULTIPLE;break;
					case 0x36:  BalanceDev.VelocityPID.P=(float)Data/APP_MULTIPLE;break;
					case 0x37:  BalanceDev.VelocityPID.I=(float)Data/APP_MULTIPLE;break;
					case 0x38:  BalanceDev.VelocityPID.D=(float)Data/APP_MULTIPLE;break;
					default:break;
				}
			}
			memset(USART3_RX_BUF, 0, i);
			Flag_PID=0;   //��ر�־λ����
			i=0;
			Data=0;
			
		} 
	}
//	OSIntExit(); 	
} 



