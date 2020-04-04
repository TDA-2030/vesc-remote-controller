


#ifndef __OLED_H
#define __OLED_H			  	 

#include "font.h"
#include "type_def.h"

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
#define OLED_MODE 0



#define OLED_SCL   PD_ODR_ODR3//ʱ�� D0��SCLK)
#define OLED_SDIN  PD_ODR_ODR4//D1��MOSI�� ����
#define OLED_RST   PD_ODR_ODR6//��λ
#define OLED_DC    PD_ODR_ODR7//����/�������
#define OLED_CS    PD_ODR_ODR5 //Ƭѡ


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



//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED�˿ڶ���----------------  					   
 		     

//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);  		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowNum_n(u8 x,u8 y,u32 num,u8 len,u8 size,u8 m);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);
void OLED_ShowString_m(u8 x,u8 y,const u8 *p,u8 size,u8 m);
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,const u8 BMP[]);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);
#endif  
	 



