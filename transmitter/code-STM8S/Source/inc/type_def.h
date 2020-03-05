/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ********************
 * �ļ���  ��type_def.h
 * ����    �����iCreate STM8������ר��ͷ�ļ� 
 * ʵ��ƽ̨��iCreate STM8������
 * �Ĵ����汾  ��V1.0.0
 * ����    ��ling_guansheng  QQ��779814207
 * ����    ��
 * �޸�ʱ�� ��2012-6-12
**********************************************************************************/
#ifndef __type_def_h
#define __type_def_h

/*����оƬ��ͷ�ļ�*/
#include "iostm8s105k6.h"

/*bool ����ͷ�ļ�*/
#include "stdbool.h"

/*���ж�ͷ�ļ�*/
#include "intrinsics.h"

/*�����������Ͷ���*/
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


/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/