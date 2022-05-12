/*****************************************************************
*  @file     : timer.c            
*  @brief    :     
*  Details.          
*                       
*  @author   : zhouli        
*  @version  : xx.xx         
*  @date     : 2018-xx-xx    
*                           
*  Change History 
******************************************************************/

#include "timer.h"
#include "led.h"
#include "usart.h"

void TIM1_Throttle_Set(uint16_t throttle)
{
	if(throttle>THROTTLE_MAX)
		throttle = THROTTLE_MAX;
	TIM1_SET_CCR(1000+throttle);
}

//--------------------- 
//作者：QQ1034313020 
//来源：CSDN 
//原文：https://blog.csdn.net/lushoumin/article/details/79018261 
//版权声明：本文为博主原创文章，转载请附上博文链接！
/* 定时器配置 */
void TIM1_PWM_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;				 //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化
	GPIO_ResetBits(GPIOA,GPIO_Pin_8 );			 //输出低
		
	/* 允许TIM1的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* 将定时器1寄存器设为初始值 */
	TIM_DeInit(TIM1);

	/* 设置定时器1由内部时钟 */
	TIM_InternalClockConfig(TIM1);

	/* 预分频值 */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;  //72分频
	/* 时钟分割 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	/* 向上计数 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* 自动重载值 */
	TIM_TimeBaseStructure.TIM_Period = 20000;  //频率50hz
	/* 循环次数 */
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	/* 初始化定时器1 */
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	/* PWM设置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	/* 设置死区 */
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 0x00;  //死区时间
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); 
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	/* 开启定时器1 */
	TIM_Cmd(TIM1, ENABLE);
	
	/* PWM关闭 */
	TIM_CtrlPWMOutputs(TIM1, DISABLE);
}


//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = 50; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
	
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}


