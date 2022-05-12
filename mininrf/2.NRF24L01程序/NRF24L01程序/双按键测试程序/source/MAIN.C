/******************************************************************************/
/*                        -- 深圳安信可科技有限公司 --                        */
/*
//  文件名：main.c                                                              
//  说明：供客户测试模块通信使用程序                                                                  
//  编写人员：                                                                   
//  编写日期：                                                              
//  程序维护：
//  维护记录：
//
//                                                          
*/ 
// 免责声明：该程序仅用于学习与交流 
// (c) Anxinke Corporation. All rights reserved.                                                               
/******************************************************************************/
#include "typedef.h"
#include "gpio.h"
#include "uart.h"
#include "nrf24l01p.h"
#include "delay.h"

uchar txData[32] = {0x00};
uchar rxData[32] = {0x00};

/******************************************************************************/
// 函数名称：main 
// 输入参数： 
// 输出参数： 
// 函数功能： 
/******************************************************************************/
void main(void)
{
    unsigned char i=0;
    uartInit();   
    Delay100ms();
    init_nrf24l01_io();
    led1ON();
    Delay100ms();
    led1OFF();
    Delay100ms();
    led1ON();
    Delay100ms();
    led1OFF();
    
    Delay100ms();
    uartSendString("System initialization is complete!\r\n");
    receive_mode_init();
    
    while(1)
    {
        KEY1 = 1;
        KEY2 = 1;
        /*
         * 判断是否按下KEY
        */
        if (key1ON() || key2ON()) {
            _delay_us(500);
            txData[0] = 0;
            if (key1ON()) {
                txData[0] = 0xAA; // 如果按下K1  则将数据置为0xAA  
                uartSendString("You press the left button\r\n"); 
            } else if (key2ON()){
                txData[0] = 0x55; // 如果按下K2  则将数据置为0x55
                uartSendString("You press the Right button\r\n");
            }
            /*
             * 根据不同的数据LED提示
            */
            if (send_data(txData) == 1) {
                uartSendString("Success of sending data\r\n");
                if (txData[0] == 0xAA) {
                    led1ON();    
                } else if (txData[0] == 0x55) {
                    led2ON(); 
                } 
                Delay100ms();   
                led1OFF();
                led2OFF(); 
            } else {
                uartSendString("Failed to send data\r\n");
            }      
            receive_mode_init();
        }
        rxData[0] = 0;
        led1OFF();
        led2OFF();
        if (receive_data(rxData)) {
            if (rxData[0] == 0xAA) {
                led1ON();
                uartSendString("Receiving the data, from the Left\r\n");
            } else if (rxData[0] == 0x55) {
                led2ON();
                uartSendString("Receiving the data, from the Right\r\n");
            }
            Delay100ms();
        }
    
    }
}