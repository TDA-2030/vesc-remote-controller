/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:39:17
 * @Description: file content
 */


#include "type_def.h"
#include "timer.h"
#include "led.h"
#include "24l01.h"
#include "control.h"
#include "key.h"
#include "main.h"


#pragma vector=1
__interrupt void TRAP_IRQHandler(void)
{
}
#pragma vector=2
__interrupt void TLI_IRQHandler(void)
{
}
#pragma vector=3
__interrupt void AWU_IRQHandler(void)
{
}
#pragma vector=4
__interrupt void CLK_IRQHandler(void)
{
}
#pragma vector=5
__interrupt void EXTI_PORTA_IRQHandler(void)
{
}
#pragma vector=6
__interrupt void EXTI_PORTB_IRQHandler(void)
{
}
#pragma vector=7
__interrupt void EXTI_PORTC_IRQHandler(void)
{
}
static u8 retrycnt = 0;
#pragma vector=8
__interrupt void EXTI_PORTD_IRQHandler(void)//---NRF24L01读取
{
    uint8_t status;
    uint8_t RxLength;

    if (NRF24L01_IRQ == 0)
    {
        status = NRF24L01_Read_Status_Register();

        if (status & MAX_TX)
        {
            //OLED_DrawPoint(100,0,1);
            NRF24L01_Write_Reg(FLUSH_TX, 0xff);	//清除TX FIFO寄存器

            if (SYSTEM_STATE_NO_SIGNAL == system.state)
            {
            }
            else
            {
                if (++retrycnt > 5)
                {
                    retrycnt = 0;
                    NRF_Mode = MODE_RX;
                    RF24L01_Set_Mode(MODE_RX);
                }
            }
        }
        else if (status & RX_OK)
        {
            RxLength = NRF24L01_Read_Reg(R_RX_PL_WID);		//读取接收到的数据个数
            NRF24L01_Read_Buf(RD_RX_PLOAD, nrf_rx_buf, RxLength);	//接收到数据
            NRF24L01_Write_Reg(FLUSH_RX, 0xff);				//清除RX FIFO

            if (Rx_Data_Handle() == 0)
            {
                retrycnt = 0;
                NRF_Mode = MODE_TX;
                RF24L01_Set_Mode(MODE_TX);
            }
        }
        else if (status & TX_OK)
        {
            NRF24L01_Write_Reg(FLUSH_TX, 0xff);	//清除TX FIFO寄存器
            retrycnt = 0;
            TX_succeed_cnt++;
            NRF_Mode = MODE_RX;
            RF24L01_Set_Mode(MODE_RX);

            if (SYSTEM_STATE_NO_SIGNAL == system.state)
            {
                system.state = SYSTEM_STATE_IDLE;
            }
        }

        NRF24L01_Write_Reg(STATUS, status);		//清中断标志
        //LED=!LED;
    }
}
#pragma vector=9
__interrupt void EXTI_PORTE_IRQHandler(void)
{
}

#pragma vector=0xC
__interrupt void SPI_IRQHandler(void)
{
}
#pragma vector=0xD
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
}
#pragma vector=0xE
__interrupt void TIM1_CAP_COM_IRQHandler(void)
{
}

#pragma vector=0xF
__interrupt void TIM2_UPD_OVF_BRK_IRQHandler(void)//-----定时器2溢出中断
{
    static u8 count_time = 0, TX_cnt = 0;

    if (NRF_Mode == MODE_TX)
    {
        Upload_data();
        NRF24L01_TxPacket(nrf_tx_buf, 32);
        TX_cnt++;
    }
    else
    {
        if (++retrycnt > 5)
        {
            retrycnt = 0;
            NRF_Mode = MODE_TX;
            RF24L01_Set_Mode(MODE_TX);
        }
    }

    if (++count_time > 100)
    {
        count_time = 0;
        TX_succeed_cnt *= 100;  //扩大100倍
        Sys_Tx_Rate = TX_succeed_cnt / TX_cnt;
        TX_cnt = TX_succeed_cnt = 0;
    }

    /* Clear the IT pending Bit */
    TIM2_SR1 = (uint8_t)(~0x01);
}
#pragma vector=0x10
__interrupt void TIM2_CAP_COM_IRQHandler(void)
{
}


#pragma vector=0x11
__interrupt void TIM3_UPD_OVF_BRK_IRQHandler(void)
{
    static uint8_t t = 0;

    if (++t == 5)
    {
        t = 0;
        button_ticks();
    }

    Sys_Time++;
    /* Clear the IT pending Bit */
    TIM3_SR1 = (uint8_t)(~0x01);
}
#pragma vector=0x12
__interrupt void TIM3_CAP_COM_IRQHandler(void)
{
}


#pragma vector=0x13
__interrupt void UART1_TX_IRQHandler(void)
{
}
#pragma vector=0x14
__interrupt void UART1_RX_IRQHandler(void)
{
}

#pragma vector=0x15
__interrupt void I2C_IRQHandler(void)
{
}


#pragma vector=0x16
__interrupt void UART3_TX_IRQHandler(void)
{
}
#pragma vector=0x17
__interrupt void UART3_RX_IRQHandler(void)
{
}


#pragma vector=0x18
__interrupt void ADC2_IRQHandler(void)
{
}


#pragma vector=0x19
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
}

#pragma vector=0x1A
__interrupt void EEPROM_EEC_IRQHandler(void)
{
}


/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/