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

#define USART3_MAX_RECV_LEN		50					//最大接收缓存字节数
//#define USART3_MAX_SEND_LEN		50					//最大发送缓存字节数

#define APP_MULTIPLE 100.0f      //与app通信时pid参数被放大的倍数


enum _actions{
	NONEACT  = 0x00,
	FORWARD  = 0x01,  //前进
	BACKWARD = 0x02,  //后退
	LEFT     = 0x04,  //左转
	RIGHT    = 0x08,  //右转
};

enum _commands{
	NONECMD    = 0x00,
	FLASHWRITE = 0x01,  //写flash保存参数
	FLASHREAD  = 0x02,  //从flash中读取参数并通过串口发送出去
	PARAMETERBACK = 0x04,  //通过串口将当前参数发送出去
	PARAMETERMODIFY  = 0x08,  //修改参数而不保存
	DEFAULTPARA  = 0x10,  //读取默认参数
	SYSTEMREBOOT  = 0x20,  //重启
};

//串口命令结构体
typedef struct
{
	enum _actions action;  //运动动作，用_actions枚举
	enum _commands cmd;    //命令
	u8 flag_usart3;  //print是否使用usart3，为1则使用
	
}Usart3CMD_t;
extern Usart3CMD_t Usart3Cmds;

extern u8 PID_Send,Flash_Send,flag_usart3;
extern float Show_Data_Mb; 

void uart3_init(u32 pclk2,u32 bound);

#endif

