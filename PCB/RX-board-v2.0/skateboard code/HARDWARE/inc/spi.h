#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


void SPI1_Init(void);			 //初始化SPI口
void SPI1_SetSpeed(uint8_t SpeedSet); //设置SPI速度   
uint8_t SPI1_ReadWriteByte(uint8_t TxData);//SPI总线读写一个字节
 				  	    													  
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(uint8_t SpeedSet); //设置SPI速度   
uint8_t SPI2_ReadWriteByte(uint8_t TxData);//SPI总线读写一个字节
		 
#endif

