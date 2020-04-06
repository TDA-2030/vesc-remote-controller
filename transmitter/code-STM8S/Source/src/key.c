/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:38:26
 * @Description: file content
 */

#include "type_def.h"
#include "key.h"
#include "delay.h"



#define	KEY_1 		PB_IDR_IDR4
#define	KEY_2 		PB_IDR_IDR5


struct Button btn_1;
struct Button btn_2;

static uint8_t key1_get_level(void)
{
    return KEY_1;
}

static uint8_t key2_get_level(void)
{
    return KEY_2;
}

/*******************************************************************************
****��������:��ʼ��KEY�ӿ�
****��ڲ���:
****���ڲ���:
****������ע:
*******************************************************************************/
void KEY_Init(void)
{
	PB_DDR &= ~(3<<4);//����PB0,1Ϊ����ģʽ
	PB_CR1 &= ~(3<<4);//����PB0,1Ϊ��������
	PB_CR2 &= ~(3<<4);//����PB0,1û���ж�

    button_init(&btn_1, key1_get_level, 1);
    button_init(&btn_2, key2_get_level, 1);
    button_start(&btn_1);
    button_start(&btn_2);
    
}



