/*
	Copyright 2015 Benjamin Vedder	benjamin@vedder.se

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

/*
 * comm_uart.c
 *
 *  Created on: 17 aug 2015
 *      Author: benjamin
 */
#include "main.h"
#include "usart.h"

#include "comm_uart.h"
#include "packet.h"
#include "bldc_interface.h"
#include "bldc_interface_uart.h"

#include <string.h>

// Settings

#define SERIAL_RX_BUFFER_SIZE	768

// Private functions
static void send_packet(unsigned char *data, unsigned int len);

// Threads
static bool rxchar_flag =0;

// Variables
static uint8_t serial_rx_buffer[SERIAL_RX_BUFFER_SIZE];
static int serial_rx_read_pos = 0;
static int serial_rx_write_pos = 0;


void usart2_isr(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART2))
	{
		bldc_rxchar(LL_USART_ReceiveData8(USART2));
	}
	LL_USART_ClearFlag_ORE(USART2);
}


/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
void bldc_rxchar(uint8_t c) {
	
	/*
	 * Put the character in a buffer and notify a thread that there is data
	 * available. An alternative way is to use
	 *
	 * packet_process_byte(c);
	 *
	 * here directly and skip the thread. However, this could drop bytes if
	 * processing packets takes a long time.
	 */

	serial_rx_buffer[serial_rx_write_pos++] = c;

	if (serial_rx_write_pos == SERIAL_RX_BUFFER_SIZE) {
		serial_rx_write_pos = 0;
	}
	rxchar_flag = 1;
}



void bldc_packet_timer(void)
{
	packet_timerfunc();
}


void bldc_packet_process(void) {
	
	if(rxchar_flag) {
		rxchar_flag = 0;
		
		while (serial_rx_read_pos != serial_rx_write_pos) {
			bldc_interface_uart_process_byte(serial_rx_buffer[serial_rx_read_pos++]);

			if (serial_rx_read_pos == SERIAL_RX_BUFFER_SIZE) {
				serial_rx_read_pos = 0;
			}
		}
	}
}

/**
 * Callback that the packet handler uses to send an assembled packet.
 *
 * @param data
 * Data array pointer
 * @param len
 * Data array length
 */
static void send_packet(unsigned char *data, unsigned int len) {
	

//	// Send the data over UART
	for (; len != 0;	len--)  
	{
		while(!LL_USART_IsActiveFlag_TXE(USART2));
		LL_USART_TransmitData8(USART2, *data++);
	}
//	uartStartSend(&UART_DEV, len, buffer);
}




static void bldc_val_received(mc_values *val) {
	
	SampleData.vin = val->vin*100;
	SampleData.bat_current = val->input_current*10;
	SampleData.mot_current = val->motor_current*10;
	SampleData.board_temp = val->temp_fet_filtered;
	SampleData.tacho_single = (uint16_t)((float)(val->tachometer_abs_value)/557.0439f);  //tacho是放大了（磁极数*3）倍，电机一圈行走0.075398m
	SampleData.speed = (uint32_t)(val->rpm>0 ? val->rpm : -val->rpm )/7;///221.049f;
	SampleData.wh_drawn = val->watt_hours*100;
	SampleData.wh_regen = val->watt_hours_charged*100;
	SampleData.ah_drawn = val->amp_hours*100;
	SampleData.ah_regen = val->amp_hours_charged*100;
	
}


void bldc_comm_uart_init(void) {
	// Initialize UART
	MX_USART2_UART_Init();
	LL_USART_Disable(USART2);
	LL_USART_EnableIT_RXNE(USART2);
	LL_USART_Enable(USART2);
	
	usart2_callback = usart2_isr;
	
	// Initialize the bldc interface and provide a send function
	bldc_interface_uart_init(send_packet);
	
	// Give bldc_interface a function to call when valus are received.
	bldc_interface_set_rx_value_func(bldc_val_received);

}
