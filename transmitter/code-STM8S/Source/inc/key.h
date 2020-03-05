#ifndef _KEY_H__
#define _KEY_H__


#define	KEY_1 		PB_IDR_IDR4
#define	KEY_2 		PB_IDR_IDR5
	

#define key_longTimes       200     //10msһ��,200�μ�2��,���峤�����ж�ʱ��  
#define key_autoTimes       20      //����ʱ�䶨��,20*10=200,200���뷢һ��


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
	STEP_keyUp,              //��ʼ״̬,δ����  
	STEP_keyDown,            //��������  
	STEP_keyLong,            //����  
	STEP_keyTime,            //������ʱ̬ 
};

void KEY_Init(void);          //������ʼ��
void key_read(void);



#endif

