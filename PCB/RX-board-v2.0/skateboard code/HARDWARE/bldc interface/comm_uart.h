/*
 * comm_uart.h
 *
 *  Created on: 17 aug 2015
 *      Author: benjamin
 */

#ifndef COMM_UART_H_
#define COMM_UART_H_

// Functions
void bldc_comm_uart_init(void);
void bldc_rxchar(uint8_t c);
void bldc_packet_process(void);
	
#endif /* COMM_UART_H_ */
