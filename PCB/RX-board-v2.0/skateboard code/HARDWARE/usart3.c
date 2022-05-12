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


//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//接收缓冲,最大USART3_MAX_RECV_LEN个字节

Usart3CMD_t Usart3Cmds = {NONEACT,NONECMD,0};  //串口3接收的命令结构体

/**************************************************************************
函数功能：串口3初始化
入口参数：pclk2:PCLK2 时钟频率(Mhz)    bound:波特率
返回  值：无
**************************************************************************/
void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分
	mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //使能PORTB口时钟  
	RCC->APB1ENR|=1<<18;  //使能串口时钟 
	GPIOB->CRH&=0XFFFF00FF; 
	GPIOB->CRH|=0X00008B00;//IO状态设置
	GPIOB->ODR|=1<<10;	  
	RCC->APB1RSTR|=1<<18;   //复位串口1
	RCC->APB1RSTR&=~(1<<18);//停止复位
	//波特率设置
 	USART3->BRR=mantissa; // 波特率设置
	USART3->CR1|=0X200C;  //1位停止,无校验位.
	//使能接收中断
	USART3->CR1|=1<<8;    //PE中断使能
	USART3->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(0,1,USART3_IRQn,2);//组2，最低优先级
	
	Usart3Cmds.flag_usart3 = 1;
}
/** 
* @brief  : 串口3,printf 函数,确保一次发送数据不超过USART3_MAX_SEND_LEN字节
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
//	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
//	for(j=0;j<i;j++)							//循环发送数据
//	{
//		while((USART3->SR&0X40)==0);			//循环发送,直到发送完毕   
//		USART3->DR=USART3_TX_BUF[j];
//	} 
//}

/** 
* @brief  : 串口3接收中断
* @param  : 
* @param  : 
* @return : 
*/
void USART3_IRQHandler(void)
{
//	OSIntEnter();
	if(USART3->SR&(1<<5))//接收到数据
	{
		 u8 uart_receive=0;//蓝牙接收相关变量
		 static u8 Flag_PID,i;
		 static uint32_t Data;
		
		 uart_receive=(u8)USART3->DR;
		
//		if(uart_receive==0x59)  Flag_sudu=2;  //低速挡
//		if(uart_receive==0x58)  Flag_sudu=1;  //高速档   mini版不设低速档
		
		//if(uart_receive>10)  //默认使用app
		{
			if(uart_receive=='A')  Usart3Cmds.action |= FORWARD,Usart3Cmds.action &= ~BACKWARD;  //前
			if(uart_receive=='E')  Usart3Cmds.action |= BACKWARD,Usart3Cmds.action &= ~FORWARD;  //后
			if(uart_receive=='B'||uart_receive=='C'||uart_receive=='D')	 //右
			{
				Usart3Cmds.action |= RIGHT,Usart3Cmds.action &= ~LEFT;
			}
			else if(uart_receive=='F'||uart_receive=='G'||uart_receive=='H')  //左
			{
				Usart3Cmds.action |= LEFT,Usart3Cmds.action &= ~RIGHT;
			}
			else 
				Usart3Cmds.action &= ~LEFT,Usart3Cmds.action &= ~RIGHT; //直行
		}
		
		if(uart_receive=='{') Flag_PID=1;  //指令起始位
		if(uart_receive=='}') Flag_PID=2;  //指令停止位
		if(Flag_PID==1)                      //记录参数
		{
			USART3_RX_BUF[i++]=uart_receive;
		}
		if(Flag_PID==2)   //执行
		{
			if(USART3_RX_BUF[1] == 'Q')  //来自上位机发送来的request
			{
				switch(USART3_RX_BUF[3])
				{
					case 'P': Usart3Cmds.cmd |= PARAMETERBACK;  //获取设备参数
					break;
					case 'R': Usart3Cmds.cmd |= FLASHREAD;      //读取保存参数
					break;
					case 'D': Usart3Cmds.cmd |= FLASHREAD|DEFAULTPARA;    //读取默认参数
					break;
					case 'W': Usart3Cmds.cmd |= FLASHWRITE;     //掉电保存参数
					break;
					case 'B': Usart3Cmds.cmd |= SYSTEMREBOOT;     //重启设备
					break;
					case 'S': BalanceDev.BalancePoint = BalanceDev.Angle_Balance;     //设置当前角度为中位
					break;
					default:break;
				}
			}
			else if(USART3_RX_BUF[1]!='#')                    //更新PID参数
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
			Flag_PID=0;   //相关标志位清零
			i=0;
			Data=0;
			
		} 
	}
//	OSIntExit(); 	
} 



