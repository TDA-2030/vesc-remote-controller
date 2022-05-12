/*****************************************************************
*  @file     : usart3.h            
*  @brief    :     
*  Details.          
*                       
*  @author   : zhouli        
*  @version  : xx.xx         
*  @date     : 2017-12-29    
*                           
*  Change History 
******************************************************************/
#ifndef __USRAT3_H
#define __USRAT3_H 
#include "sys.h"	  	

#define USART3_MAX_RECV_LEN		50					//�����ջ����ֽ���
//#define USART3_MAX_SEND_LEN		50					//����ͻ����ֽ���

#define APP_MULTIPLE 100.0f      //��appͨ��ʱpid�������Ŵ�ı���


enum _actions{
	NONEACT  = 0x00,
	FORWARD  = 0x01,  //ǰ��
	BACKWARD = 0x02,  //����
	LEFT     = 0x04,  //��ת
	RIGHT    = 0x08,  //��ת
};

enum _commands{
	NONECMD    = 0x00,
	FLASHWRITE = 0x01,  //дflash�������
	FLASHREAD  = 0x02,  //��flash�ж�ȡ������ͨ�����ڷ��ͳ�ȥ
	PARAMETERBACK = 0x04,  //ͨ�����ڽ���ǰ�������ͳ�ȥ
	PARAMETERMODIFY  = 0x08,  //�޸Ĳ�����������
	DEFAULTPARA  = 0x10,  //��ȡĬ�ϲ���
	SYSTEMREBOOT  = 0x20,  //����
};

//��������ṹ��
typedef struct
{
	enum _actions action;  //�˶���������_actionsö��
	enum _commands cmd;    //����
	u8 flag_usart3;  //print�Ƿ�ʹ��usart3��Ϊ1��ʹ��
	
}Usart3CMD_t;
extern Usart3CMD_t Usart3Cmds;

extern u8 PID_Send,Flash_Send,flag_usart3;
extern float Show_Data_Mb; 

void uart3_init(u32 pclk2,u32 bound);

#endif

