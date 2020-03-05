/******************************************************************************

//按键检测程序

*******************************************************************************/
#include "type_def.h"
#include "key.h"
#include "delay.h"



key_state_t Key_State;




/*******************************************************************************
****函数功能:初始化KEY接口
****入口参数:
****出口参数:
****函数备注:
*******************************************************************************/
void KEY_Init(void)
{
	PB_DDR &= ~(3<<4);//设置PB0,1为输入模式
	PB_CR1 &= ~(3<<4);//设置PB0,1为浮空输入
	PB_CR2 &= ~(3<<4);//设置PB0,1没有中断
}

/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击 
**************************************************************************/
u8 key_scan(void)
{
	
	if(KEY_1==1)return 0x01;
		
	if(KEY_2==1)return 0x02;
				
	return 0x00;
}
/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击 
**************************************************************************/
void key_read(void)
{
	static enum key_step_e keyStep=STEP_keyUp;        //未按，普通短按，长按,连发等状态  
    static u16 keyTimeCounts=0;  //在计时状态的计数器  
    static u8  LastKey = 0 ; //保存按键释放时的P3口数据  
//	static u8 key_return=0;         //函数返回值 
    u8 Key=0;                //键对应io口的电平  
    Key=key_scan();          //提取所有的key对应的io口  

    switch(keyStep)          //这里检测到的是先前的状态  
    {
        case STEP_keyUp:   //如果先前是初始态，即无动作  
        {
            if(0!=Key) //如果键被按下
            {
                keyStep=STEP_keyDown; //更新键的状态，普通被按下				
            }
        }
        break;
		
        case STEP_keyDown: //如果先前是被按着的  
        {
            if(0!=Key) //如果现在还被按着  
            {
                keyStep=STEP_keyTime; //转换到计时态  
                keyTimeCounts=0;  
                LastKey = Key;     //保存键值  
            }
            else
            {
                keyStep=STEP_keyUp; //键没被按着，回初始态，说明是干扰  
            }
        }
        break;

        case STEP_keyTime:  //如果先前已经转换到计时态(值为3)  
        {  //如果真的是手动按键,必然进入本代码块,并且会多次进入  
            if(0==Key||LastKey!=Key) //如果未按键或按的不是上次按下的键
            {
                keyStep=STEP_keyUp;
                Key_State.state=KEY_PRESS;    //返回1,一次完整的普通按键
		Key_State.KeyNum = LastKey;
                //程序进入这个语句块，说明已经有2次以上10ms的中断，等于已经消抖  
                //那么此时检测到按键被释放，说明是一次普通短按  
            }
            else  //在计时态，检测到键还被按着  
            {
                if(++keyTimeCounts>key_longTimes) //时间达到2秒  
                {
                    keyStep=STEP_keyLong;  //进入长按状态  
                    keyTimeCounts=0;        //计数器清空,便于进入连发重新计数  
                    Key_State.state=KEY_LONG_PRESS;   //返回state_keyLong 
                    Key_State.KeyNum = Key;					
                }
                //代码中,在2秒内如果我们一直按着key的话,返回值只会是0,不会识别为短按或长按的  
            }
        }
        break;

        case STEP_keyLong:  //在长按状态检测连发  ,每0.2秒发一次  
        {
            if(0==Key)
            {
               keyStep=STEP_keyUp;
            }
            else //按键时间超过2秒时
            {
//                if(++keyTimeCounts>key_autoTimes)//10*20=200ms
//                {
//                    keyTimeCounts=0;
//                    Key_State.state=KEY_LONG_AUTO;  //每0.2秒返回值的第2位置位(1<<2) 
//                     Key_State.KeyNum = Key;					
//                }//连发的时候,肯定也伴随着长按
            }
//            Key_State.state = KEY_LONG_PRESS;  //0x02是肯定的,0x04|0x02是可能的 
//            Key_State.KeyNum = Key;
        }
        break;
		
        default: break;
    }
}


