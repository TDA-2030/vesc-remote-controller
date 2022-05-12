#ifndef __USART_H
#define __USART_H

#define USART1_REC_LEN  			200  	//定义最大接收字节数 200

extern uint8_t  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART1_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart1_init(uint32_t bound);
void UART1_SendByte(uint8_t ch);


#define USART2_REC_LEN  			200  	//定义最大接收字节数 200

extern uint8_t  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART2_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart2_init(uint32_t bound);
void UART2_SendByte(uint8_t ch);

#define USART3_REC_LEN  			200  	//定义最大接收字节数 200

extern uint8_t  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART3_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart3_init(uint32_t bound);
void UART3_SendByte(uint8_t ch);

#endif


