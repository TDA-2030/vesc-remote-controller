/******************************************************************************/
/*                        -- 深圳安信可科技有限公司 --                        */
/*
//  文件名：uart.h                                                              
//  说明：串口宏定义                                                                  
//  编写人员：Wu                                                                   
//  编写日期：2012年11月12日                                                              
//  程序维护：
//	维护记录：
//
//                                                          
*/ 
// 免责声明：该程序仅用于学习与交流 
// (c) Anxinke Corporation. All rights reserved.                                                               
/******************************************************************************/
#ifndef __UART_H
#define __UART_H

#define UART_BUAD                            115200



void uartInit(void);
void uartSendData(u8 ucData);
void uartSendString(char *s);




#endif
