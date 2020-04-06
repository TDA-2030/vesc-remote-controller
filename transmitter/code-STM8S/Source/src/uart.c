/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:40:09
 * @Description: file content
 */

#include "type_def.h"
#include "uart.h"


void uart_conf(void)
{
    unsigned int baud_div=0;
    
    UART1_CR1 = (0<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0); 
    /*1λ��ʼλ 8λ����λ ����λ��CR3���� ��ʹ����żУ�� ��ʹ����żУ���ж�*/
    UART1_CR2 = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<3)|(0<<2); 
    /*ʹ�ܷ��ͺͽ��� �����ж�ʹ�� ��ֹ�����ж�*/
    UART1_CR3 = (0<<6)|(0<<4)|(0<<3); /*����1λֹͣλ ��ʹ��SCLK*/        
    UART1_CR5 = (0<<2)|(0<<1);     
    /*ʹ�����ܿ�ģʽ��Ҫ���õģ������ϱ���Ĭ�Ͼ����� */ 
    
    /*���ò�����*/
    baud_div =HSIClockFreq/BaudRate;  /*�����Ƶ����*/
    UART1_BRR2 = baud_div & 0x0f;
    UART1_BRR2 |= ((baud_div & 0xf000) >> 8);
    UART1_BRR1 = ((baud_div & 0x0ff0) >> 4);    /*�ȸ�BRR2��ֵ ���������BRR1*/
    
    UART1_CR1 |= (0<<5);         /*ʹ��UART*/

}


void UART1_SendByte(u8 data)
{
    UART1_DR=data;
   /* Loop until the end of transmission */
   while (!(UART1_SR & UART1_FLAG_TXE));
}


void UART1_SendString(u8* Data)
{
  for (; *Data != 0;	Data++)  UART1_SendByte(*Data);
  
}

u8 UART1_ReceiveByte(void)
{
     u8 USART1_RX_BUF; 
     while (!(UART1_SR & UART1_FLAG_RXNE));
     USART1_RX_BUF=(uint8_t)UART1_DR;
     return  USART1_RX_BUF;
    
}



int fputc(int ch, FILE *f)
{  
 /*��Printf���ݷ�������*/ 
  UART1_DR=(unsigned char)ch;
  while (!(UART1_SR & UART1_FLAG_TXE));
  return (ch);
}

