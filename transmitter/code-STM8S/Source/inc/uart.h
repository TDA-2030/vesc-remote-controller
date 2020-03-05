#ifndef __UART_H
#define __UART_H


#include <stdio.h>
#define HSIClockFreq 16000000
#define BaudRate  115200
#define UART1_FLAG_TXE  (uint8_t)0x80  /*!< Transmit Data Register Empty flag */
#define UART1_FLAG_RXNE (uint8_t)0x20 /*!< Read Data Register Not Empty flag */
#define RxBufferSize 64
void uart_conf(void);
void UART1_SendByte(u8 data);
void UART1_SendString(u8* Data);
u8 UART1_ReceiveByte(void);
int fputc(int ch, FILE *f);
#endif