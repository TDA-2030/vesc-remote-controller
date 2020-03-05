#ifndef _KEY_H__
#define _KEY_H__


#define	KEY_1 		PB_IDR_IDR4
#define	KEY_2 		PB_IDR_IDR5
	

#define key_longTimes       200     //10ms一次,200次即2秒,定义长按的判定时间  
#define key_autoTimes       20      //连发时间定义,20*10=200,200毫秒发一次


enum key_states_e{
    KEY_PRESS       = 0x01,
	KEY_LONG_PRESS  = 0x02,
	KEY_LONG_AUTO   = 0x04,
};

typedef struct {
	u8 state;
	u8 KeyNum;
}key_state_t;
extern key_state_t Key_State;

enum key_step_e{
	STEP_keyUp,              //初始状态,未按键  
	STEP_keyDown,            //键被按下  
	STEP_keyLong,            //长按  
	STEP_keyTime,            //按键计时态 
};

void KEY_Init(void);          //按键初始化
void key_read(void);



#endif

