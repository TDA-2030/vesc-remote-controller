/*
 * @Author: zhouli
 * @Date: 2020-05-25 01:14:54
 * @LastEditTime: 2020-05-26 00:50:44
 * @Description: file content
 */ 


#include "oled.h"



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 12/16/24
void GUI_ShowChar(uint8_t x, uint8_t y, uint8_t chr, const FONT_t *font)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (font->Height / 8 + ((font->Height % 8) ? 1 : 0)) * font->Width;		//得到字体一个字符对应点阵集所占的字节数
    uint8_t *color;

    if (font == &Font24)
    {
        chr = chr - '0';
        if (chr > 9)return;
    }
    else chr = chr - ' '; //得到偏移后的值

    OLED_GetPointColor(&color);

    for (t = 0; t < csize; t++)
    {
        temp = font->table[chr * csize + t];

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)OLED_DrawPoint(x, y, *color);
            else OLED_DrawPoint(x, y, !(*color));

            temp <<= 1;
            y++;

            if ((y - y0) == font->Height)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//m^n函数
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}
//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void GUI_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, const FONT_t *font)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t offset = font->Width;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                GUI_ShowChar(x + (offset * t), y, '0', font);
                continue;
            }
            else enshow = 1;
        }

        GUI_ShowChar(x + (offset * t), y, temp + '0', font);
    }
}

static int8_t my_i2a(int32_t in_num, char *out_str)
{
    uint8_t remainder;
    uint8_t len = 0;

    if (NULL == out_str)
    {
        return 0;
    }

    do
    {
        remainder = in_num % 10;
        in_num = in_num / 10;
        *(out_str++) = remainder + '0';
        len++;
    }
    while (in_num);

    return len;
}

void GUI_Showfloat(uint16_t x, uint16_t y, uint32_t num, char uint, uint8_t len, uint8_t point, const FONT_t *font)
{
    char buf[10];
    int8_t num_len;
    uint8_t width = font->Width;

    x += (width * len);
    num_len = my_i2a(num, buf);

    if (uint != 0)
    {
        GUI_ShowChar(x + width, y, uint, font);
    }

    for (size_t i = 0; i < len; i++)
    {
        if (i == point)
        {
            GUI_ShowChar(x, y, '.', font);
            x -= width;
        }

        if (i < num_len)
        {
            GUI_ShowChar(x, y, (uint8_t)buf[i], font);
        }
        else
        {
            GUI_ShowChar(x, y, '0', font);
        }

        x -= width;
    }
}

//显示字符串
//x,y:起点坐标
//size:字体大小
//*p:字符串起始地址
void GUI_ShowString(uint8_t x, uint8_t y, const uint8_t *p, const FONT_t *font)
{
    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x > (128 - font->Width))
        {
            x = 0;
            y += font->Height;
        }

        if (y > (64 - font->Height))
        {
            y = x = 0;
        }
        GUI_ShowChar(x, y, *p, font);
        x += font->Width;
        p++;
    }
}

