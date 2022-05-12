#ifndef __USART_H
#define __USART_H

#define USART1_REC_LEN  			200  	//�����������ֽ��� 200

extern uint8_t  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART1_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart1_init(uint32_t bound);
void UART1_SendByte(uint8_t ch);


#define USART2_REC_LEN  			200  	//�����������ֽ��� 200

extern uint8_t  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART2_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart2_init(uint32_t bound);
void UART2_SendByte(uint8_t ch);

#define USART3_REC_LEN  			200  	//�����������ֽ��� 200

extern uint8_t  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART3_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart3_init(uint32_t bound);
void UART3_SendByte(uint8_t ch);

#endif


