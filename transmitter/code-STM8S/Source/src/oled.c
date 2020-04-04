
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
uint8_t OLED_GRAM[128][8];

//更新显存到LCD
void OLED_Refresh_Gram(void)
{
    uint8_t i, n;

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
void OLED_WR_Byte(u8 dat, u8 cmd)
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
void OLED_WR_Byte(u8 dat, u8 cmd)
{
    u8 i;

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


////开启OLED显示
//void OLED_Display_On(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
//	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
//	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
//}
////关闭OLED显示
//void OLED_Display_Off(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
//	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
//	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
//}


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
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,清空;1,填充
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)
{
    u8 x, y;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)OLED_DrawPoint(x, y, dot);
    }
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
    u8 i, n;

    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)OLED_GRAM[n][i] = 0X00;

    //OLED_Refresh_Gram();//更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数

    if (8 == size) csize = 6;

    if (24 == size)
    {
        chr = chr - '0';

        if (chr > 9)return;
    }
    else chr = chr - ' '; //得到偏移后的值

    for (t = 0; t < csize; t++)
    {
        if (size == 8)temp = asc2_0806[chr][t];     //调用0608字体
        else if (size == 12)temp = asc2_1206[chr][t]; 	 	//调用1206字体
        else if (size == 16)temp = asc2_1608[chr][t];	//调用1608字体
        else if (size == 24)temp = asc2_2412[chr][t];	//调用2412字体
        else return;								//没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//m^n函数
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;

    while (n--)result *= m;

    return result;
}
//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 offset;

    if (size == 8) offset = 6;
    else offset =	size / 2;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (offset * t), y, '0', size, 1);
                continue;
            }
            else enshow = 1;
        }

        OLED_ShowChar(x + (offset * t), y, temp + '0', size, 1);
    }
}

void OLED_ShowNum_n(u8 x, u8 y, u32 num, u8 len, u8 size, u8 m)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 offset;

    if (size == 8) offset = 6;
    else offset =	size / 2;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (offset * t), y, ' ', size, m);
                continue;
            }
            else enshow = 1;
        }

        OLED_ShowChar(x + (offset * t), y, temp + '0', size, m);
    }
}

//显示字符串
//x,y:起点坐标
//size:字体大小
//*p:字符串起始地址
void OLED_ShowString(u8 x, u8 y, const u8 *p, u8 size)
{
    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }

        if (y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }

        OLED_ShowChar(x, y, *p, size, 1);

        if (8 == size)x += 6;
        else  x += size / 2;

        p++;
    }
}
void OLED_ShowString_m(u8 x, u8 y, const u8 *p, u8 size, u8 m)
{
    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }

        if (y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }

        OLED_ShowChar(x, y, *p, size, m);

        if (8 == size)x += 6;
        else  x += size / 2;

        p++;
    }
}
//坐标设置
void OLED_Set_Pos(u8 x, u8 y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y的范围0～63*****************/
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, const u8 BMP[])
{
    unsigned char x, y;
    y1++;
    x1++;

    if (y0 % 8 || y1 % 8) return;

    y0 /= 8;
    y1 /= 8;

    for (y = y0; y < y1; y++)
    {
        for (x = x0; x < x1; x++)
            OLED_GRAM[x][y] = *BMP++;
    }

    OLED_Refresh_Gram();//更新显示
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1; //设置单步方向
    else if (delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++) //画线输出
    {
        OLED_DrawPoint(uRow, uCol, 1); //画点
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}
//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //判断下个点位置的标志

    while (a <= b)
    {
        OLED_DrawPoint(x0 + a, y0 - b, 1);       //5
        OLED_DrawPoint(x0 + b, y0 - a, 1);       //0
        OLED_DrawPoint(x0 + b, y0 + a, 1);       //4
        OLED_DrawPoint(x0 + a, y0 + b, 1);       //6
        OLED_DrawPoint(x0 - a, y0 + b, 1);       //1
        OLED_DrawPoint(x0 - b, y0 + a, 1);
        OLED_DrawPoint(x0 - a, y0 - b, 1);       //2
        OLED_DrawPoint(x0 - b, y0 - a, 1);       //7
        a++;

        //使用Bresenham算法画圆
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

//
//
////显示信号强度
//void OLED_Show_RSSI(u8 x,u8 y,u8 val)
//{
//	u8 t,temp;
//	const u8 icon[][16]={
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

