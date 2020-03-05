/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ***************************
 * �ļ���  ��tim1.c
 * ����    ����ʱ��TM1���ú�����   
 * ʵ��ƽ̨��iCreate STM8������
 * �Ĵ����汾  ��V1.0.0
 * ����    ��ling_guansheng  QQ��779814207
 * ����    ��
 * �޸�ʱ�� ��2011-12-20



****************************************************************************************/

#include "type_def.h"
#include "timer.h"

uint32_t Sys_Time=0;


/*******************************************************************************
****��������:��ʼ��TIM2
****��ڲ���:
****���ڲ���:
****������ע:
*******************************************************************************/
void InitTIM2(u8 psc,u16 arr)
{
	TIM2_CR1 = 0;
     /* Set the Prescaler value */
    TIM2_PSCR = (psc);
    /* Set the Autoreload value */
    TIM2_ARRH = (uint8_t)(arr >> 8);
    TIM2_ARRL = (uint8_t)(arr);

    /* Set or Reset the ARPE Bit */
    TIM2_CR1 |= (uint8_t)MASK_TIM2_CR1_ARPE;

    /* Enable the Interrupt sources */
    TIM2_IER |= (uint8_t)0x01;
    
    /* set or Reset the CEN Bit */
    TIM2_CR1 |= (uint8_t)MASK_TIM2_CR1_CEN;
}

void InitTIM3(u8 psc,u16 arr)
{
     /* Set the Prescaler value */
    TIM3_PSCR = (psc);
    /* Set the Autoreload value */
    TIM3_ARRH = (uint8_t)(arr >> 8);
    TIM3_ARRL = (uint8_t)(arr);

    /* Set or Reset the ARPE Bit */
    TIM3_CR1 |= (uint8_t)MASK_TIM3_CR1_ARPE;

    /* Enable the Interrupt sources */
    TIM3_IER |= (uint8_t)0x01;
    
    /* set or Reset the CEN Bit */
    TIM3_CR1 |= (uint8_t)MASK_TIM3_CR1_CEN;
}


/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/