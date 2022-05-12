/*****************************************************************
*  @file     : xx.x            
*  @brief    :     
*  Details.          
*                       
*  @author   : zhouli        
*  @version  : xx.xx         
*  @date     : 2018-12-xx    
*                           
*  Change History 
******************************************************************/

#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"




#define TIM1_MOE_ON() {TIM1->BDTR |= TIM_BDTR_MOE;}/* Enable the TIM Main Output */
#define TIM1_MOE_OFF() {TIM1->CCR1 = 0;TIM1->BDTR &= (uint16_t)(~((uint16_t)TIM_BDTR_MOE));}/* Disable the TIM Main Output */
#define TIM1_SET_CCR(t) TIM1->CCR1 = (t)


#define THROTTLE_MAX 1000



void TIM3_Int_Init(void);
void TIM1_PWM_config(void);
void TIM1_Throttle_Set(uint16_t throttle);


#endif
