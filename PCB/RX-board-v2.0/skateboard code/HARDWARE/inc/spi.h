#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(uint8_t SpeedSet); //����SPI�ٶ�   
uint8_t SPI1_ReadWriteByte(uint8_t TxData);//SPI���߶�дһ���ֽ�
 				  	    													  
void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(uint8_t SpeedSet); //����SPI�ٶ�   
uint8_t SPI2_ReadWriteByte(uint8_t TxData);//SPI���߶�дһ���ֽ�
		 
#endif

