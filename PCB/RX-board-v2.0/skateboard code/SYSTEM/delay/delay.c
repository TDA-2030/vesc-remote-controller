#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 


static uint8_t  fac_us=0;							//us延时倍乘数			   
static uint16_t fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数
static uint32_t SysTime=0;


#define delay_ostickspersec 1000


extern void Systick_callback(void);

//systick中断服务函数,使用ucos时用到
void SysTick_Handler(void)
{	
	SysTime++;
	
	Systick_callback();
	
}

uint32_t GetTick(void)
{
	return SysTime;
}

//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  
	reload=SystemCoreClock/8000000;				//每秒钟的计数次数 单位为K	   
	reload*=1000000/delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
												//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 						//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    

}								    


//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//LOAD的值	    	 
	ticks=nus*fac_us; 							//需要的节拍数	  		 
	tcnt=0;
	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//时间超过/等于要延迟的时间,则退出.
		}  
	};
}

//延时nms
//nms:要延时的ms数
void delay_ms(u16 nms)
{
	uint32_t dt;
	if(nms>=fac_ms)							//延时的时间大于OS的最少时间周期 
	{
		dt = nms+SysTime;
		
		while(dt>SysTime)
		{
			
		}
	}
}








































