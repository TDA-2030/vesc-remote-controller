/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:39:17
 * @Description: file content
 */


#include "type_def.h"
#include "key.h"
#include "main.h"
#include "control.h"

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
#pragma vector=8
__interrupt void EXTI_PORTD_IRQHandler(void)//---NRF24L01读取
{
    nrf_irq_handler();
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
    timer_period_handler();
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