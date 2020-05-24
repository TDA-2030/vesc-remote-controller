/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-05-26 01:17:06
 * @Description: file content
 */

#include "oled.h"
#include "delay.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
static uint8_t OLED_GRAM[128][8];
static bool g_update = 1;
static uint8_t g_point_color = 1;

//更新显存到LCD
void OLED_Refresh_Gram(void)
{
    uint8_t i, n;

    if (0 == g_update)
    {
        return;
    }

    g_update = 0;

    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + 7 - i, OLED_CMD); //设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte(0x10, OLED_CMD);      //设置显示位置—列高地址

        for (n = 0; n < 128; n++)OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}

#if OLED_MODE==1
//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    DATAOUT(dat);

    if (cmd)
        OLED_DC_Set();
    else
        OLED_DC_Clr();

    OLED_CS_Clr();
    OLED_WR_Clr();
    OLED_WR_Set();
    OLED_CS_Set();
    OLED_DC_Set();
}
#else
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    uint8_t i;

    if (cmd)
        OLED_DC_Set();
    else
        OLED_DC_Clr();

    OLED_CS_Clr();

    for (i = 0; i < 8; i++)
    {
        OLED_SCLK_Clr();

        if (dat & 0x80)
        {
            OLED_SDIN_Set();
        }
        else
        {
            OLED_SDIN_Clr();
        }

        OLED_SCLK_Set();
        dat <<= 1;
    }

    OLED_CS_Set();
    OLED_DC_Set();
}
#endif


void OLED_GetPointColor(uint8_t **out_p)
{
    if(NULL != out_p)
    {
        *out_p = &g_point_color;
    }
}

void OLED_SetPointColor(uint8_t color)
{
    g_point_color = color;
}

//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDC命令
    OLED_WR_Byte(0X14, OLED_CMD); //DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDC命令
    OLED_WR_Byte(0X10, OLED_CMD); //DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); //DISPLAY OFF
}


//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63)return; //超出范围了.

    pos = y / 8;
    bx = 7 - y % 8;
    temp = 1 << (bx);

    if (t)OLED_GRAM[x][pos] |= temp;
    else OLED_GRAM[x][pos] &= ~temp;

    g_update = 1;
}

//画点
//x:0~127
//y:0~63
void OLED_DrawPointFast(uint8_t x, uint8_t y)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63)return; //超出范围了.

    pos = y / 8;
    bx = 7 - y % 8;
    temp = 1 << (bx);

    if (g_point_color)OLED_GRAM[x][pos] |= temp;
    else OLED_GRAM[x][pos] &= ~temp;

    g_update = 1;
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)OLED_GRAM[n][i] = 0X00;

    //OLED_Refresh_Gram();//更新显示
}

//坐标设置
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}


//
//
////显示信号强度
//void OLED_Show_RSSI(uint8_t x,uint8_t y,uint8_t val)
//{
//	uint8_t t,temp;
//	const uint8_t icon[][16]={
//	{0x08,0x18,0x28,0x48,0xF8,0x48,0x28,0x18,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
//	{0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"RSSI",0*/
//	};
//	OLED_Set_Pos(x,y);
//    for(t=0;t<16;t++)
//	{
//		temp=icon[0][t];
//		if(val>3&&t==12)temp|=0x80;
//		if(val>4&&t==14)temp|=0xE0;
//		OLED_WR_Byte(temp,OLED_DATA);
//    }
//	OLED_Set_Pos(x,y+1);
//    for(t=0;t<16;t++)
//	{
//		temp=icon[1][t];
//		if(val>0&&t==6)temp|=0xE0;
//		if(val>1&&t==8)temp|=0xF8;
//		if(val>2&&t==10)temp|=0xFE;
//		if(val>3&&t==12)temp|=0xFF;
//		if(val>4&&t==14)temp|=0xFF;
//		OLED_WR_Byte(temp,OLED_DATA);
//    }
//}

//初始化SSD1306
void OLED_Init(void)
{
    PD_DDR |= 0xf8; /* 设置数据方向寄存器 1为输出，0为输入--查看STM8寄存器.pdf P87 */
    PD_CR1 |= 0xf8; /* 设置推挽输出--查看STM8寄存器.pdf P88 */
    PD_CR2 |= 0xF8; /* 设置输出频率 1为10M，0为2M--查看STM8寄存器.pdf P89 */
    OLED_RST_Set();
    delay_ms(80);
    OLED_RST_Clr();
    delay_ms(80);
    OLED_RST_Set();
    OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
    OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
    OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
    OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
    OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC0, OLED_CMD); //Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display
    OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty
    OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00, OLED_CMD); //-not offset
    OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
    OLED_WR_Byte(0xF1, OLED_CMD); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
    OLED_WR_Byte(0x40, OLED_CMD); //Set VCOM Deselect Level
    OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02, OLED_CMD); //
    OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
    OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
    OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
    OLED_WR_Byte(0xAF, OLED_CMD); //--turn on oled panel
    OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
    OLED_Clear();
    OLED_Refresh_Gram();
    OLED_Set_Pos(0, 0);
}

