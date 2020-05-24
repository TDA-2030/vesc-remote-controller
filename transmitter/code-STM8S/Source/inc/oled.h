/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-05-26 00:29:08
 * @Description: file content
 */



#ifndef __OLED_H
#define __OLED_H

#include "font.h"
#include "type_def.h"
#include "gui_basic.h"
#include "gui_bitmap.h"
#include "gui_char.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define OLED_MODE 0



#define OLED_SCL   PD_ODR_ODR3//时钟 D0（SCLK)
#define OLED_SDIN  PD_ODR_ODR4//D1（MOSI） 数据
#define OLED_RST   PD_ODR_ODR6//复位
#define OLED_DC    PD_ODR_ODR7//数据/命令控制
#define OLED_CS    PD_ODR_ODR5 //片选


#define OLED_CS_Clr()  OLED_CS=0
#define OLED_CS_Set()  OLED_CS=1

#define OLED_RST_Clr() OLED_RST=0
#define OLED_RST_Set() OLED_RST=1

#define OLED_DC_Clr() OLED_DC=0
#define OLED_DC_Set() OLED_DC=1

#define OLED_SCLK_Clr() OLED_SCL=0
#define OLED_SCLK_Set() OLED_SCL=1

#define OLED_SDIN_Clr() OLED_SDIN=0
#define OLED_SDIN_Set() OLED_SDIN=1;



//OLED模式设置
//0:4线串行模式
//1:并行8080模式

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64
//-----------------OLED端口定义----------------


//OLED控制用函数
void OLED_Init(void);
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_SetPointColor(uint8_t color);
void OLED_GetPointColor(uint8_t **out_p);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_DrawPointFast(uint8_t x, uint8_t y);

#endif




