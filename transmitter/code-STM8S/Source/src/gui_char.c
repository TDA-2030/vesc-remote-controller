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
void GUI_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数
    uint8_t *color;

    if (8 == size) csize = 6;

    if (24 == size)
    {
        chr = chr - '0';

        if (chr > 9)return;
    }
    else chr = chr - ' '; //得到偏移后的值

    OLED_GetPointColor(&color);

    for (t = 0; t < csize; t++)
    {
        if (size == 8)temp = asc2_0806[chr][t];     //调用0608字体
        else if (size == 12)temp = asc2_1206[chr][t]; 	 	//调用1206字体
        else if (size == 16)temp = asc2_1608[chr][t];	//调用1608字体
        else if (size == 24)temp = asc2_2412[chr][t];	//调用2412字体
        else return;								//没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)OLED_DrawPoint(x, y, *color);
            else OLED_DrawPoint(x, y, !(*color));

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
void GUI_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t offset;

    if (size == 8) offset = 6;
    else offset =	size / 2;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                GUI_ShowChar(x + (offset * t), y, '0', size);
                continue;
            }
            else enshow = 1;
        }

        GUI_ShowChar(x + (offset * t), y, temp + '0', size);
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

void GUI_Showfloat(uint16_t x, uint16_t y, uint32_t num, char uint, uint8_t len, uint8_t point, uint8_t size)
{
    uint8_t width;
    char buf[10];
    int8_t num_len;

    if (8 == size) width = 6;
    else width = size >> 1;

    x += (width * len);
    num_len = my_i2a(num, buf);

    if (uint != 0)
    {
        GUI_ShowChar(x + width, y, uint, size);
    }

    for (size_t i = 0; i < len; i++)
    {
        if (i == point)
        {
            GUI_ShowChar(x, y, '.', size);
            x -= width;
        }

        if (i < num_len)
        {
            GUI_ShowChar(x, y, (uint8_t)buf[i], size);
        }
        else
        {
            GUI_ShowChar(x, y, '0', size);
        }

        x -= width;
    }
}

//显示字符串
//x,y:起点坐标
//size:字体大小
//*p:字符串起始地址
void GUI_ShowString(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size)
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
            GUI_Clear();
        }

        GUI_ShowChar(x, y, *p, size);

        if (8 == size)x += 6;
        else  x += size / 2;

        p++;
    }
}

