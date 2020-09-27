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
    /*1位起始位 8位数据位 结束位由CR3设置 不使用奇偶校验 不使能奇偶校验中断*/
    UART1_CR2 = (0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<3)|(0<<2); 
    /*使能发送和接收 接收中断使能 禁止发送中断*/
    UART1_CR3 = (0<<6)|(0<<4)|(0<<3); /*设置1位停止位 不使能SCLK*/        
    UART1_CR5 = (0<<2)|(0<<1);     
    /*使用智能卡模式需要设置的，基本上保持默认就行了 */ 
    
    /*设置波特率*/
    baud_div =HSIClockFreq/BaudRate;  /*求出分频因子*/
    UART1_BRR2 = baud_div & 0x0f;
    UART1_BRR2 |= ((baud_div & 0xf000) >> 8);
    UART1_BRR1 = ((baud_div & 0x0ff0) >> 4);    /*先给BRR2赋值 最后再设置BRR1*/
    
    UART1_CR1 |= (0<<5);         /*使能UART*/

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
 /*将Printf内容发往串口*/ 
  UART1_DR=(unsigned char)ch;
  while (!(UART1_SR & UART1_FLAG_TXE));
  return (ch);
}

