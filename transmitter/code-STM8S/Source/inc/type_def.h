
#ifndef __TYPE_DEF_H
#define __TYPE_DEF_H

/*主控芯片的头文件*/
#include "iostm8s105k6.h"

/*bool 类型头文件*/
#include "stdbool.h"
#include "stddef.h"

/*总中断头文件*/
#include "intrinsics.h"

/*常用数据类型定义*/
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned long int    uint32_t;

typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed long int    int32_t;


typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned char BYTE;


#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define EnableInterrupt  __enable_interrupt() 
#endif


/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/