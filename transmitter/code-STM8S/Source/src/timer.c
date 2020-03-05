/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ***************************
 * 文件名  ：tim1.c
 * 描述    ：定时器TM1配置函数库   
 * 实验平台：iCreate STM8开发板
 * 寄存器版本  ：V1.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 * 修改时间 ：2011-12-20



****************************************************************************************/

#include "type_def.h"
#include "timer.h"

uint32_t Sys_Time=0;


/*******************************************************************************
****函数功能:初始化TIM2
****入口参数:
****出口参数:
****函数备注:
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


/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/