/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ***************************
 * 文件名  ：pwm.c
 * 描述    ：PWM配置函数库   
 * 实验平台：iCreate STM8开发板
 * 寄存器版本  ：V1.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 *修改时间 ：2011-12-20



****************************************************************************************/
#include "type_def.h"
#include "pwm.h"
#include "delay.h"




/**************************************************************************
 * 函数名：PWM_Init
 * 描述  ：PWM初始化
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void PWM_Init(void)
{
  u16 TIM1_Period=1024-1; //计数1000，刚好为输出1KHZ
  u16 TIM1_Prescaler=15;//16分频
  
  TIM1_CR1 &= ~(MASK_TIM1_CR1_CEN);
  //------------------------------初始化TIM1时间基本单位----------------------------
    /* Set the Prescaler value */
  TIM1_PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
  TIM1_PSCRL = (uint8_t)(TIM1_Prescaler);
  
    /* Set the Autoreload value */
  TIM1_ARRH = (uint8_t)(TIM1_Period >> 8);
  TIM1_ARRL = (uint8_t)(TIM1_Period);
  
  /* Select the Counter Mode */
  TIM1_CR1 = (uint8_t)((1<<7)|(0<<5)|(0<<4)|(0<<3)|(0<<1));
  
  /* Set the Repetition Counter value */
  TIM1_RCR = 0x00;
  
  //--------------------------------初始化TIM1 Channel1，Channel2，Channel3------------------------

  /* Set the Output State & Set the Output N State & Set the Output Polarity &
  Set the Output N Polarity */
  TIM1_CCER1 = 0x00; 
  TIM1_CCER2 = 0x00;
  TIM1_CCER1 |= (uint8_t)((1<<5)|(1<<4)|(1<<1)|(1<<0));
  TIM1_CCER2 |= (uint8_t)((1<<5)|(1<<4)|(1<<1)|(1<<0));
  
  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1_CCMR1 = (uint8_t)((uint8_t)(TIM1_CCMR1 & (uint8_t)(~MASK_TIM1_CCMR1_OC1M)) | (uint8_t)0x70);
  TIM1_CCMR2 = (uint8_t)((uint8_t)(TIM1_CCMR2 & (uint8_t)(~MASK_TIM1_CCMR2_OC2M)) | (uint8_t)0x70);
  TIM1_CCMR3 = (uint8_t)((uint8_t)(TIM1_CCMR3 & (uint8_t)(~MASK_TIM1_CCMR3_OC3M)) | (uint8_t)0x70);
  TIM1_CCMR4 = (uint8_t)((uint8_t)(TIM1_CCMR4 & (uint8_t)(~MASK_TIM1_CCMR4_OC4M)) | (uint8_t)0x70);
  

  /* set  the CEN Bit */
  TIM1_CR1 |= MASK_TIM1_CR1_CEN;
  /* Set the MOE Bit */
  TIM1_BKR |= MASK_TIM1_BKR_MOE;
  
}

void PWM_Close(void)
{
	/* set  the CEN Bit */
  	TIM1_CR1 &= ~(MASK_TIM1_CR1_CEN);
}

void PWM_Open(void)
{
	/* set  the CEN Bit */
  	TIM1_CR1 |= MASK_TIM1_CR1_CEN;
}


void SetTIM1_PWM_DutyCycle(uint8_t ch, uint16_t Compare)
{
  if(Compare>1023)Compare=1023;
  Compare = 1023-Compare;
  if('G'==ch)
  {
  /* Set the Capture Compare3 Register value */
  TIM1_CCR3H = (uint8_t)(Compare >> 8);
  TIM1_CCR3L = (uint8_t)(Compare);
  }
  else if('B'==ch)
  {
  /* Set the Capture Compare3 Register value */
  TIM1_CCR2H = (uint8_t)(Compare >> 8);
  TIM1_CCR2L = (uint8_t)(Compare);
  }
  else if('L'==ch)
  {
  /* Set the Capture Compare3 Register value */
  TIM1_CCR1H = (uint8_t)(Compare >> 8);
  TIM1_CCR1L = (uint8_t)(Compare);
  }
  else if('R'==ch)
  {
  /* Set the Capture Compare4 Register value */
  TIM1_CCR4H = (uint8_t)(Compare >> 8);
  TIM1_CCR4L = (uint8_t)(Compare);
  }
}






/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/