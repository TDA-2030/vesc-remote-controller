/******************************************************************************/
/*                        -- ���ڰ��ſɿƼ����޹�˾ --                        */
/*
//  �ļ�����GPIO.h                                                              
//  ˵������GPIO��غ궨��                                                                  
//  ��д��Ա��Wu                                                                   
//  ��д���ڣ�2012��11��12��                                                              
//  ����ά����
//  ά����¼��
//
//                                                          
*/ 
// �����������ó��������ѧϰ�뽻�� 
// (c) Anxinke Corporation. All rights reserved.                                                               
/******************************************************************************/
#ifndef __GPIO_H
#define __GPIO_H

#define KEY1                                 P30
#define KEY2                                 P31

#define key1ON()                             (KEY1 == 0)
#define key2ON()                             (KEY2 == 0)

#define LED1                                 P16
#define LED2                                 P35
#define led1ON()                             (LED1 = 0)
#define led1OFF()                            (LED1 = 1)
#define led2ON()                             (LED2 = 0)
#define led2OFF()                            (LED2 = 1)


#define UART_TX_PIN                          P31
#define UART_TX_SET(n)                       UART_TX_PIN = n
#define UART_TX_HIGH()                       UART_TX_SET(1)
#define UART_TX_LOW()                        UART_TX_SET(0)
#define UART_TX_FLIP()                       UART_TX_PIN = !UART_TX_PIN

#define UART_RX_PIN                          P30
#define UART_RX_SET(n)                       UART_RX_PIN = n
#define UART_RX_HIGH()                       UART_RX_SET(1)
#define UART_RX_LOW()                        UART_RX_SET(0)
#define UART_RX_FLIP()                       UART_RX_PIN = !UART_RX_PIN









#endif 





