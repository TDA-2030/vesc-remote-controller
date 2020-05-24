/*
 * @Author: zhouli
 * @Date: 2020-05-25 01:14:54
 * @LastEditTime: 2020-05-25 23:58:56
 * @Description: file content
 */ 


#include "oled.h"



/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y的范围0～63*****************/
// void GUI_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
// {
//     uint8_t x, y;
//     uint8_t temp, yy;
//     y1++;
//     x1++;

//     for (y = y0; y < y1; y+=8)
//     {
//         temp = y % 8;
//         yy = y / 8;
//         uint8_t mask_l = ;
//         uint8_t mask_h;

//         if (temp)  // 不是8的倍数，需要拼接位数
//         {
//             uint8_t d, h, l;

//             for (x = x0; x < x1; x++)
//             {
//                 d = *BMP++;
//                 h = OLED_GRAM[x][yy];
//                 l = OLED_GRAM[x][yy+1];
//                 OLED_GRAM[x][yy] = ;
//                 OLED_GRAM[x][yy] = ;
//             }
//         }
//         else
//         {
//             for (x = x0; x < x1; x++)
//                 OLED_GRAM[x][y] = *BMP++;
//         }
//     }

//     OLED_Refresh_Gram();//更新显示
// }

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y的范围0～63*****************/
void GUI_DrawBMP(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, const uint8_t BMP[])
{
    uint8_t x, y;
    uint8_t *color;
    OLED_GetPointColor(&color);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            uint8_t bit = BMP[((y) / 8) * (width) + (x)];

            if (bit & (1 << (7 - (y % 8))))
            {
                OLED_DrawPoint(x + x0, y + y0, *color); //画点
            }
            else
            {
                OLED_DrawPoint(x + x0, y + y0, !(*color)); //画点
            }
        }
    }

    // OLED_Refresh_Gram();//更新显示
}

