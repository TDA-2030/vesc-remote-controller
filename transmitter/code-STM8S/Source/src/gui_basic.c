/****************************************************************************************
* 文件名：GUI_BASIC.C
* 功能：GUI基本绘图函数。进行基本绘图运算，并调用相应的刷新程序更新LCD显示。
* 作者：黄绍斌
* 日期：2004.02.26
* 备注：图形操作层，进行各种图形运算操作。
****************************************************************************************/

#include <math.h>

#include "oled.h"

#define GUI_HLine(x0, y0, x1) GUI_DrawLine((x0), (y0), (x1), (y0))
#define GUI_RLine(x0, y0, y1) GUI_DrawLine((x0), (y0), (x0), (y1))
#define GUI_Point OLED_DrawPointFast


#define GUI_LCM_XMAX X_WIDTH
#define GUI_LCM_YMAX Y_WIDTH


/* 有宽度的直线函数 */
#define   GUI_LineWith_EN		1
/* 画圆函数 */
#define   GUI_CircleX_EN		1
/* 椭圆函数 */
#define   GUI_EllipseX_EN		0
/* 填充函数 */
#define   GUI_FloodFill_EN		0
/* 向上及向下节点个数(填充函数) */
#define   DOWNP_N			20
#define   UPP_N				20
/* 圆弧函数 */
#define   GUI_ArcX_EN			1
/* 扇形函数 */
#define   GUI_Pieslice_EN		0


static uint8_t *g_gui_point_color;


uint8_t GUI_GetPointColor(void)
{
    OLED_GetPointColor(&g_gui_point_color);
    return *g_gui_point_color;
}

void GUI_SetPointColor(uint8_t color)
{
    OLED_GetPointColor(&g_gui_point_color);
    *g_gui_point_color = color;
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,清空;1,填充
void GUI_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)GUI_Point(x, y);
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void GUI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int16_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int16_t incx, incy, uRow, uCol;
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
        GUI_Point(uRow, uCol); //画点
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

/****************************************************************************
* 名称：GUI_Rectangle()
* 功能：画矩形。
* 入口参数： x0		矩形左上角的x坐标值
*           y0		矩形左上角的y坐标值
*           x1      矩形右下角的x坐标值
*           y1      矩形右下角的y坐标值
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    GUI_HLine(x0, y0, x1);
    GUI_HLine(x0, y1, x1);
    GUI_RLine(x0, y0, y1);
    GUI_RLine(x1, y0, y1);
}


/****************************************************************************
* 名称：GUI_RectangleFill()
* 功能：填充矩形。画一个填充的矩形，填充色与边框色一样。
* 入口参数： x0		矩形左上角的x坐标值
*           y0		矩形左上角的y坐标值
*           x1      矩形右下角的x坐标值
*           y1      矩形右下角的y坐标值
*           color	填充颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_RectangleFill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint16_t  i;

    /* 先找出矩形左上角与右下角的两个点，保存在(x0,y0)，(x1,y1) */
    if (x0 > x1) 						// 若x0>x1，则x0与x1交换
    {
        i = x0;
        x0 = x1;
        x1 = i;
    }

    if (y0 > y1)						// 若y0>y1，则y0与y1交换
    {
        i = y0;
        y0 = y1;
        y1 = i;
    }

    /* 判断是否只是直线 */
    if (y0 == y1)
    {
        GUI_HLine(x0, y0, x1);
        return;
    }

    if (x0 == x1)
    {
        GUI_RLine(x0, y0, y1);
        return;
    }

    //    while(y0<=y1)
    //    {  GUI_HLine(x0, y0, x1);	// 当前画水平线
    //       y0++;							// 下一行
    //    }
    while (x0 <= x1)						//刘西振，其实有时候画竖线更好
    {
        GUI_RLine(x0, y0, y1);	// 当前画水平线
        x0++;							// 下一行
    }
}


/****************************************************************************
* 名称：GUI_Square()
* 功能：画正方形。
* 入口参数： x0		正方形左上角的x坐标值
*           y0		正方形左上角的y坐标值
*           with    正方形的边长
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Square(uint16_t x0, uint16_t y0, uint16_t  with)
{
    if (with == 0) return;

    if ((x0 + with) > GUI_LCM_XMAX) return;

    if ((y0 + with) > GUI_LCM_YMAX) return;

    GUI_Rectangle(x0, y0, x0 + with, y0 + with);
}



/****************************************************************************
* 名称：GUI_Line()
* 功能：画任意两点之间的直线。
* 入口参数： x0		直线起点的x坐标值
*           y0		直线起点的y坐标值
*           x1      直线终点的x坐标值
*           y1      直线终点的y坐标值
*           color	显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    GUI_DrawLine(x0, y0, x1, y1);
    // int32_t   dx;						// 直线x轴差值变量
    // int32_t   dy;          			// 直线y轴差值变量
    // int8_t    dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
    // int8_t    dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
    // int32_t   dx_x2;					// dx*2值变量，用于加快运算速度
    // int32_t   dy_x2;					// dy*2值变量，用于加快运算速度
    // int32_t   di;						// 决策变量
    // dx = x1 - x0;						// 求取两点之间的差值
    // dy = y1 - y0;
    // /* 判断增长方向，或是否为水平线、垂直线、点 */
    // if (dx > 0)							// 判断x轴方向
    // {
    //     dx_sym = 1;					// dx>0，设置dx_sym=1
    // }
    // else
    // {
    //     if (dx < 0)
    //     {
    //         dx_sym = -1;				// dx<0，设置dx_sym=-1
    //     }
    //     else
    //     {
    //         // dx==0，画垂直线，或一点
    //         GUI_RLine(x0, y0, y1);
    //         return;
    //     }
    // }
    // if (dy > 0)							// 判断y轴方向
    // {
    //     dy_sym = 1;					// dy>0，设置dy_sym=1
    // }
    // else
    // {
    //     if (dy < 0)
    //     {
    //         dy_sym = -1;				// dy<0，设置dy_sym=-1
    //     }
    //     else
    //     {
    //         // dy==0，画水平线，或一点
    //         GUI_HLine(x0, y0, x1);
    //         return;
    //     }
    // }
    // /* 将dx、dy取绝对值 */
    // dx = dx_sym * dx;
    // dy = dy_sym * dy;
    // /* 计算2倍的dx及dy值 */
    // dx_x2 = dx * 2;
    // dy_x2 = dy * 2;
    // /* 使用Bresenham法进行画直线 */
    // if (dx >= dy)						// 对于dx>=dy，则使用x轴为基准
    // {
    //     di = dy_x2 - dx;
    //     while (x0 != x1)
    //     {
    //         GUI_Point(x0, y0);
    //         x0 += dx_sym;
    //         if (di < 0)
    //         {
    //             di += dy_x2;			// 计算出下一步的决策值
    //         }
    //         else
    //         {
    //             di += dy_x2 - dx_x2;
    //             y0 += dy_sym;
    //         }
    //     }
    //     GUI_Point(x0, y0);		// 显示最后一点
    // }
    // else								// 对于dx<dy，则使用y轴为基准
    // {
    //     di = dx_x2 - dy;
    //     while (y0 != y1)
    //     {
    //         GUI_Point(x0, y0);
    //         y0 += dy_sym;
    //         if (di < 0)
    //         {
    //             di += dx_x2;
    //         }
    //         else
    //         {
    //             di += dx_x2 - dy_x2;
    //             x0 += dx_sym;
    //         }
    //     }
    //     GUI_Point(x0, y0);		// 显示最后一点
    // }
}


#if	GUI_LineWith_EN==1
/****************************************************************************
* 名称：GUI_LineWith()
* 功能：画任意两点之间的直线，并且可设置线的宽度。
* 入口参数： x0		直线起点的x坐标值
*           y0		直线起点的y坐标值
*           x1      直线终点的x坐标值
*           y1      直线终点的y坐标值
*           with    线宽(0-50)
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LineWith(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t with)
{
    int16_t   dx;						// 直线x轴差值变量
    int16_t   dy;          			    // 直线y轴差值变量
    int8_t    dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
    int8_t    dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
    int16_t   dx_x2;					// dx*2值变量，用于加快运算速度
    int16_t   dy_x2;					// dy*2值变量，用于加快运算速度
    int16_t   di;						// 决策变量
    int16_t   wx, wy;					// 线宽变量
    int16_t   draw_a, draw_b;

    /* 参数过滤 */
    if (with == 0) return;

    if (with > 50) with = 50;

    dx = x1 - x0;						// 求取两点之间的差值
    dy = y1 - y0;
    wx = with / 2;
    wy = with - wx - 1;

    /* 判断增长方向，或是否为水平线、垂直线、点 */
    if (dx > 0)							// 判断x轴方向
    {
        dx_sym = 1;					// dx>0，设置dx_sym=1
    }
    else
    {
        if (dx < 0)
        {
            dx_sym = -1;				// dx<0，设置dx_sym=-1
        }
        else
        {
            /* dx==0，画垂直线，或一点 */
            wx = x0 - wx;

            if (wx < 0) wx = 0;

            wy = x0 + wy;

            while (1)
            {
                x0 = wx;
                GUI_RLine(x0, y0, y1);

                if (wx >= wy) break;

                wx++;
            }

            return;
        }
    }

    if (dy > 0)							// 判断y轴方向
    {
        dy_sym = 1;					// dy>0，设置dy_sym=1
    }
    else
    {
        if (dy < 0)
        {
            dy_sym = -1;				// dy<0，设置dy_sym=-1
        }
        else
        {
            /* dy==0，画水平线，或一点 */
            wx = y0 - wx;

            if (wx < 0) wx = 0;

            wy = y0 + wy;

            while (1)
            {
                y0 = wx;
                GUI_HLine(x0, y0, x1);

                if (wx >= wy) break;

                wx++;
            }

            return;
        }
    }

    /* 将dx、dy取绝对值 */
    dx = dx_sym * dx;
    dy = dy_sym * dy;
    /* 计算2倍的dx及dy值 */
    dx_x2 = dx * 2;
    dy_x2 = dy * 2;

    /* 使用Bresenham法进行画直线 */
    if (dx >= dy)						// 对于dx>=dy，则使用x轴为基准
    {
        di = dy_x2 - dx;

        while (x0 != x1)
        {
            /* x轴向增长，则宽度在y方向，即画垂直线 */
            draw_a = y0 - wx;

            if (draw_a < 0) draw_a = 0;

            draw_b = y0 + wy;
            GUI_RLine(x0, draw_a, draw_b);
            x0 += dx_sym;

            if (di < 0)
            {
                di += dy_x2;			// 计算出下一步的决策值
            }
            else
            {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }

        draw_a = y0 - wx;

        if (draw_a < 0) draw_a = 0;

        draw_b = y0 + wy;
        GUI_RLine(x0, draw_a, draw_b);
    }
    else								// 对于dx<dy，则使用y轴为基准
    {
        di = dx_x2 - dy;

        while (y0 != y1)
        {
            /* y轴向增长，则宽度在x方向，即画水平线 */
            draw_a = x0 - wx;

            if (draw_a < 0) draw_a = 0;

            draw_b = x0 + wy;
            GUI_HLine(draw_a, y0, draw_b);
            y0 += dy_sym;

            if (di < 0)
            {
                di += dx_x2;
            }
            else
            {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }

        draw_a = x0 - wx;

        if (draw_a < 0) draw_a = 0;

        draw_b = x0 + wy;
        GUI_HLine(draw_a, y0, draw_b);
    }
}
#endif



/****************************************************************************
* 名称：GUI_LineS()
* 功能：多个点之间的连续连线。从第一点连到第二点，再连到第三点...
* 入口参数： points  多个点坐标数据的指针，数据排列为(x0,y0)、(x1,y1)、(x2,y2)...
*           no      点数目，至少要大于1
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LineS(uint16_t const *points, uint8_t no)
{
    uint16_t  x0, y0;
    uint16_t  x1, y1;
    uint8_t  i;

    /* 入口参数过滤 */
    if (0 == no) return;

    if (1 == no)						// 单点
    {
        x0 = *points++;
        y0 = *points;
        GUI_Point(x0, y0);
    }

    /* 画多条线条 */
    x0 = *points++;					// 取出第一点坐标值，作为原起点坐标值
    y0 = *points++;

    for (i = 1; i < no; i++)
    {
        x1 = *points++;				// 取出下一点坐标值
        y1 = *points++;
        GUI_Line(x0, y0, x1, y1);
        x0 = x1;						// 更新原起点坐标
        y0 = y1;
    }
}



#if  GUI_CircleX_EN==1
/****************************************************************************
* 名称：GUI_Circle()
* 功能：指定圆心位置及半径，画圆。
* 入口参数： x0		圆心的x坐标值
*           y0		圆心的y坐标值
*           r       圆的半径
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Circle(uint16_t x0, uint16_t y0, uint16_t r)
{
    int16_t  draw_x0, draw_y0;			// 刽图点坐标变量
    int16_t  draw_x1, draw_y1;
    int16_t  draw_x2, draw_y2;
    int16_t  draw_x3, draw_y3;
    int16_t  draw_x4, draw_y4;
    int16_t  draw_x5, draw_y5;
    int16_t  draw_x6, draw_y6;
    int16_t  draw_x7, draw_y7;
    int16_t  xx, yy;					// 画圆控制变量
    int16_t  di;						// 决策变量

    /* 参数过滤 */
    if (0 == r) return;

    /* 计算出8个特殊点(0、45、90、135、180、225、270度)，进行显示 */
    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;

    if (draw_y0 < GUI_LCM_YMAX) GUI_Point(draw_x0, draw_y0);	// 90度

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;

    if (draw_y2 >= 0) GUI_Point(draw_x2, draw_y2);			// 270度

    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;

    if (draw_x4 < GUI_LCM_XMAX) GUI_Point(draw_x4, draw_y4);	// 0度

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;

    if (draw_x5 >= 0) GUI_Point(draw_x5, draw_y5);			// 180度

    if (1 == r) return;					// 若半径为1，则已圆画完

    /* 使用Bresenham法进行画圆 */
    di = 3 - 2 * r;					// 初始化决策变量
    xx = 0;
    yy = r;

    while (xx < yy)
    {
        if (di < 0)
        {
            di += 4 * xx + 6;
        }
        else
        {
            di += 4 * (xx - yy) + 10;
            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
            draw_x4--;
            draw_x5++;
            draw_x6--;
            draw_x7++;
        }

        xx++;
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        draw_y4++;
        draw_y5++;
        draw_y6--;
        draw_y7--;

        /* 要判断当前点是否在有效范围内 */
        if ((draw_x0 <= GUI_LCM_XMAX) && (draw_y0 >= 0))
        {
            GUI_Point(draw_x0, draw_y0);
        }

        if ((draw_x1 >= 0) && (draw_y1 >= 0))
        {
            GUI_Point(draw_x1, draw_y1);
        }

        if ((draw_x2 <= GUI_LCM_XMAX) && (draw_y2 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x2, draw_y2);
        }

        if ((draw_x3 >= 0) && (draw_y3 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x3, draw_y3);
        }

        if ((draw_x4 <= GUI_LCM_XMAX) && (draw_y4 >= 0))
        {
            GUI_Point(draw_x4, draw_y4);
        }

        if ((draw_x5 >= 0) && (draw_y5 >= 0))
        {
            GUI_Point(draw_x5, draw_y5);
        }

        if ((draw_x6 <= GUI_LCM_XMAX) && (draw_y6 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x6, draw_y6);
        }

        if ((draw_x7 >= 0) && (draw_y7 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x7, draw_y7);
        }
    }
}


/****************************************************************************
* 名称：GUI_CircleFill()
* 功能：指定圆心位置及半径，画圆并填充，填充色与边框色一样。
* 入口参数： x0		圆心的x坐标值
*           y0		圆心的y坐标值
*           r       圆的半径
*           color	填充颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_CircleFill(uint16_t x0, uint16_t y0, uint16_t r)
{
    int16_t  draw_x0, draw_y0;			// 刽图点坐标变量
    int16_t  draw_x1, draw_y1;
    int16_t  draw_x2, draw_y2;
    int16_t  draw_x3, draw_y3;
    int16_t  draw_x4, draw_y4;
    int16_t  draw_x5, draw_y5;
    int16_t  draw_x6, draw_y6;
    int16_t  draw_x7, draw_y7;
    int16_t  fill_x0, fill_y0;			// 填充所需的变量，使用垂直线填充
    int16_t  fill_x1;
    int16_t  xx, yy;					// 画圆控制变量
    int16_t  di;						// 决策变量

    /* 参数过滤 */
    if (0 == r) return;

    /* 计算出4个特殊点(0、90、180、270度)，进行显示 */
    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;

    if (draw_y0 < GUI_LCM_YMAX)
    {
        GUI_Point(draw_x0, draw_y0);	// 90度
    }

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;

    if (draw_y2 >= 0)
    {
        GUI_Point(draw_x2, draw_y2);	// 270度
    }

    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;

    if (draw_x4 < GUI_LCM_XMAX)
    {
        GUI_Point(draw_x4, draw_y4);	// 0度
        fill_x1 = draw_x4;
    }
    else
    {
        fill_x1 = GUI_LCM_XMAX;
    }

    fill_y0 = y0;							// 设置填充线条起始点fill_x0
    fill_x0 = x0 - r;						// 设置填充线条结束点fill_y1

    if (fill_x0 < 0) fill_x0 = 0;

    GUI_HLine(fill_x0, fill_y0, fill_x1);
    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;

    if (draw_x5 >= 0)
    {
        GUI_Point(draw_x5, draw_y5);	// 180度
    }

    if (1 == r) return;

    /* 使用Bresenham法进行画圆 */
    di = 3 - 2 * r;							// 初始化决策变量
    xx = 0;
    yy = r;

    while (xx < yy)
    {
        if (di < 0)
        {
            di += 4 * xx + 6;
        }
        else
        {
            di += 4 * (xx - yy) + 10;
            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
            draw_x4--;
            draw_x5++;
            draw_x6--;
            draw_x7++;
        }

        xx++;
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        draw_y4++;
        draw_y5++;
        draw_y6--;
        draw_y7--;

        /* 要判断当前点是否在有效范围内 */
        if ((draw_x0 <= GUI_LCM_XMAX) && (draw_y0 >= 0))
        {
            GUI_Point(draw_x0, draw_y0);
        }

        if ((draw_x1 >= 0) && (draw_y1 >= 0))
        {
            GUI_Point(draw_x1, draw_y1);
        }

        /* 第二点水直线填充(下半圆的点) */
        if (draw_x1 >= 0)
        {
            /* 设置填充线条起始点fill_x0 */
            fill_x0 = draw_x1;
            /* 设置填充线条起始点fill_y0 */
            fill_y0 = draw_y1;

            if (fill_y0 > GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;

            if (fill_y0 < 0) fill_y0 = 0;

            /* 设置填充线条结束点fill_x1 */
            fill_x1 = x0 * 2 - draw_x1;

            if (fill_x1 > GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;

            GUI_HLine(fill_x0, fill_y0, fill_x1);
        }

        if ((draw_x2 <= GUI_LCM_XMAX) && (draw_y2 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x2, draw_y2);
        }

        if ((draw_x3 >= 0) && (draw_y3 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x3, draw_y3);
        }

        /* 第四点垂直线填充(上半圆的点) */
        if (draw_x3 >= 0)
        {
            /* 设置填充线条起始点fill_x0 */
            fill_x0 = draw_x3;
            /* 设置填充线条起始点fill_y0 */
            fill_y0 = draw_y3;

            if (fill_y0 > GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;

            if (fill_y0 < 0) fill_y0 = 0;

            /* 设置填充线条结束点fill_x1 */
            fill_x1 = x0 * 2 - draw_x3;

            if (fill_x1 > GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;

            GUI_HLine(fill_x0, fill_y0, fill_x1);
        }

        if ((draw_x4 <= GUI_LCM_XMAX) && (draw_y4 >= 0))
        {
            GUI_Point(draw_x4, draw_y4);
        }

        if ((draw_x5 >= 0) && (draw_y5 >= 0))
        {
            GUI_Point(draw_x5, draw_y5);
        }

        /* 第六点垂直线填充(上半圆的点) */
        if (draw_x5 >= 0)
        {
            /* 设置填充线条起始点fill_x0 */
            fill_x0 = draw_x5;
            /* 设置填充线条起始点fill_y0 */
            fill_y0 = draw_y5;

            if (fill_y0 > GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;

            if (fill_y0 < 0) fill_y0 = 0;

            /* 设置填充线条结束点fill_x1 */
            fill_x1 = x0 * 2 - draw_x5;

            if (fill_x1 > GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;

            GUI_HLine(fill_x0, fill_y0, fill_x1);
        }

        if ((draw_x6 <= GUI_LCM_XMAX) && (draw_y6 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x6, draw_y6);
        }

        if ((draw_x7 >= 0) && (draw_y7 <= GUI_LCM_YMAX))
        {
            GUI_Point(draw_x7, draw_y7);
        }

        /* 第八点垂直线填充(上半圆的点) */
        if (draw_x7 >= 0)
        {
            /* 设置填充线条起始点fill_x0 */
            fill_x0 = draw_x7;
            /* 设置填充线条起始点fill_y0 */
            fill_y0 = draw_y7;

            if (fill_y0 > GUI_LCM_YMAX) fill_y0 = GUI_LCM_YMAX;

            if (fill_y0 < 0) fill_y0 = 0;

            /* 设置填充线条结束点fill_x1 */
            fill_x1 = x0 * 2 - draw_x7;

            if (fill_x1 > GUI_LCM_XMAX) fill_x1 = GUI_LCM_XMAX;

            GUI_HLine(fill_x0, fill_y0, fill_x1);
        }
    }
}
#endif




#if	 GUI_EllipseX_EN==1
/****************************************************************************
* 名称：GUI_Ellipse()
* 功能：画正椭圆。给定椭圆的四个点的参数，最左、最右点的x轴坐标值为x0、x1，最上、最下点
*      的y轴坐标为y0、y1。
* 入口参数： x0		最左点的x坐标值
*           x1		最右点的x坐标值
*           y0		最上点的y坐标值
*           y1      最下点的y坐标值
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Ellipse(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1)
{
    int32_t  draw_x0, draw_y0;			// 刽图点坐标变量
    int32_t  draw_x1, draw_y1;
    int32_t  draw_x2, draw_y2;
    int32_t  draw_x3, draw_y3;
    int32_t  xx, yy;					// 画图控制变量
    int32_t  center_x, center_y;		// 椭圆中心点坐标变量
    int32_t  radius_x, radius_y;		// 椭圆的半径，x轴半径和y轴半径
    int32_t  radius_xx, radius_yy;		// 半径乘平方值
    int32_t  radius_xx2, radius_yy2;	// 半径乘平方值的两倍
    int32_t  di;						// 定义决策变量

    /* 参数过滤 */
    if ((x0 == x1) || (y0 == y1)) return;

    /* 计算出椭圆中心点坐标 */
    center_x = (x0 + x1) >> 1;
    center_y = (y0 + y1) >> 1;

    /* 计算出椭圆的半径，x轴半径和y轴半径 */
    if (x0 > x1)
    {
        radius_x = (x0 - x1) >> 1;
    }
    else
    {
        radius_x = (x1 - x0) >> 1;
    }

    if (y0 > y1)
    {
        radius_y = (y0 - y1) >> 1;
    }
    else
    {
        radius_y = (y1 - y0) >> 1;
    }

    /* 计算半径平方值 */
    radius_xx = radius_x * radius_x;
    radius_yy = radius_y * radius_y;
    /* 计算半径平方值乘2值 */
    radius_xx2 = radius_xx << 1;
    radius_yy2 = radius_yy << 1;
    /* 初始化画图变量 */
    xx = 0;
    yy = radius_y;
    di = radius_yy2 + radius_xx - radius_xx2 * radius_y ;	// 初始化决策变量
    /* 计算出椭圆y轴上的两个端点坐标，作为作图起点 */
    draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;
    draw_y0 = draw_y1 = center_y + radius_y;
    draw_y2 = draw_y3 = center_y - radius_y;
    GUI_Point(draw_x0, draw_y0);					// 画y轴上的两个端点
    GUI_Point(draw_x2, draw_y2);

    while ((radius_yy * xx) < (radius_xx * yy))
    {
        if (di < 0)
        {
            di += radius_yy2 * (2 * xx + 3);
        }
        else
        {
            di += radius_yy2 * (2 * xx + 3) + 4 * radius_xx - 4 * radius_xx * yy;
            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
        }

        xx ++;						// x轴加1
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        GUI_Point(draw_x0, draw_y0);
        GUI_Point(draw_x1, draw_y1);
        GUI_Point(draw_x2, draw_y2);
        GUI_Point(draw_x3, draw_y3);
    }

    di = radius_xx2 * (yy - 1) * (yy - 1) + radius_yy2 * xx * xx + radius_yy + radius_yy2 * xx - radius_xx2 * radius_yy;

    while (yy >= 0)
    {
        if (di < 0)
        {
            di += radius_xx2 * 3 + 4 * radius_yy * xx + 4 * radius_yy - 2 * radius_xx2 * yy;
            xx ++;						// x轴加1
            draw_x0++;
            draw_x1--;
            draw_x2++;
            draw_x3--;
        }
        else
        {
            di += radius_xx2 * 3 - 2 * radius_xx2 * yy;
        }

        yy--;
        draw_y0--;
        draw_y1--;
        draw_y2++;
        draw_y3++;
        GUI_Point(draw_x0, draw_y0);
        GUI_Point(draw_x1, draw_y1);
        GUI_Point(draw_x2, draw_y2);
        GUI_Point(draw_x3, draw_y3);
    }
}


/****************************************************************************
* 名称：GUI_EllipseFill()
* 功能：画正椭圆，并填充。给定椭圆的四个点的参数，最左、最右点的x轴坐标值为x0、x1，最上、最下点
*      的y轴坐标为y0、y1。
* 入口参数： x0		最左点的x坐标值
*           x1		最右点的x坐标值
*           y0		最上点的y坐标值
*           y1      最下点的y坐标值
*           color	填充颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_EllipseFill(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1)
{
    int32_t  draw_x0, draw_y0;			// 刽图点坐标变量
    int32_t  draw_x1, draw_y1;
    int32_t  draw_x2, draw_y2;
    int32_t  draw_x3, draw_y3;
    int32_t  xx, yy;					// 画图控制变量
    int32_t  center_x, center_y;		// 椭圆中心点坐标变量
    int32_t  radius_x, radius_y;		// 椭圆的半径，x轴半径和y轴半径
    int32_t  radius_xx, radius_yy;		// 半径乘平方值
    int32_t  radius_xx2, radius_yy2;	// 半径乘平方值的两倍
    int32_t  di;						// 定义决策变量

    /* 参数过滤 */
    if ((x0 == x1) || (y0 == y1)) return;

    /* 计算出椭圆中心点坐标 */
    center_x = (x0 + x1) >> 1;
    center_y = (y0 + y1) >> 1;

    /* 计算出椭圆的半径，x轴半径和y轴半径 */
    if (x0 > x1)
    {
        radius_x = (x0 - x1) >> 1;
    }
    else
    {
        radius_x = (x1 - x0) >> 1;
    }

    if (y0 > y1)
    {
        radius_y = (y0 - y1) >> 1;
    }
    else
    {
        radius_y = (y1 - y0) >> 1;
    }

    /* 计算半径乘平方值 */
    radius_xx = radius_x * radius_x;
    radius_yy = radius_y * radius_y;
    /* 计算半径乘4值 */
    radius_xx2 = radius_xx << 1;
    radius_yy2 = radius_yy << 1;
    /* 初始化画图变量 */
    xx = 0;
    yy = radius_y;
    di = radius_yy2 + radius_xx - radius_xx2 * radius_y ;	// 初始化决策变量
    /* 计算出椭圆y轴上的两个端点坐标，作为作图起点 */
    draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;
    draw_y0 = draw_y1 = center_y + radius_y;
    draw_y2 = draw_y3 = center_y - radius_y;
    GUI_Point(draw_x0, draw_y0);					// 画y轴上的两个端点
    GUI_Point(draw_x2, draw_y2);

    while ((radius_yy * xx) < (radius_xx * yy))
    {
        if (di < 0)
        {
            di += radius_yy2 * (2 * xx + 3);
        }
        else
        {
            di += radius_yy2 * (2 * xx + 3) + 4 * radius_xx - 4 * radius_xx * yy;
            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
        }

        xx ++;						// x轴加1
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        GUI_Point(draw_x0, draw_y0);
        GUI_Point(draw_x1, draw_y1);
        GUI_Point(draw_x2, draw_y2);
        GUI_Point(draw_x3, draw_y3);

        /* 若y轴已变化，进行填充 */
        if (di >= 0)
        {
            GUI_HLine(draw_x0, draw_y0, draw_x1);
            GUI_HLine(draw_x2, draw_y2, draw_x3);
        }
    }

    di = radius_xx2 * (yy - 1) * (yy - 1) + radius_yy2 * xx * xx + radius_yy + radius_yy2 * xx - radius_xx2 * radius_yy;

    while (yy >= 0)
    {
        if (di < 0)
        {
            di += radius_xx2 * 3 + 4 * radius_yy * xx + 4 * radius_yy - 2 * radius_xx2 * yy;
            xx ++;						// x轴加1
            draw_x0++;
            draw_x1--;
            draw_x2++;
            draw_x3--;
        }
        else
        {
            di += radius_xx2 * 3 - 2 * radius_xx2 * yy;
        }

        yy--;
        draw_y0--;
        draw_y1--;
        draw_y2++;
        draw_y3++;
        GUI_Point(draw_x0, draw_y0);
        GUI_Point(draw_x1, draw_y1);
        GUI_Point(draw_x2, draw_y2);
        GUI_Point(draw_x3, draw_y3);
        /* y轴已变化，进行填充 */
        GUI_HLine(draw_x0, draw_y0, draw_x1);
        GUI_HLine(draw_x2, draw_y2, draw_x3);
    }
}
#endif




#if  GUI_FloodFill_EN==1
/****************************************************************************
* 名称：GUI_ReadLeftPoint()
* 功能：找出指定点左边最近的非color点。
* 入口参数： x0		指定点的x坐标值
*           y0		指定点的y坐标值
*           color	指定颜色值
* 出口参数：返回该点的x轴坐标值。
* 说明：若没有找出，则返回最左的x坐标0。
****************************************************************************/
uint32_t  GUI_ReadLeftPoint(uint32_t x0, uint32_t y0)
{
    uint32_t  i;
    TCOLOR  bakc;

    for (i = x0 - 1; i > 0; i--)
    {
        GUI_ReadPoint(i, y0, &bakc);

        if (GUI_CmpColor(bakc, color) == 0) return (i + 1);	// 若找到，则返回
    }

    GUI_ReadPoint(i, y0, &bakc);

    if (GUI_CmpColor(bakc, color) == 0) return (1);		// 若找到，则返回

    return (0);
}


/****************************************************************************
* 名称：GUI_ReadRightPoint()
* 功能：找出指定点右边最近的非color点。
* 入口参数： x0		指定点的x轴坐标值
*           y0		指定点的y轴坐标值
*           color	指定颜色值
* 出口参数：返回该点的x轴坐标值。
* 说明：若没有找出，则返回最右的x坐标GUI_LCM_XMAX。
****************************************************************************/
uint32_t  GUI_ReadRightPoint(uint32_t x0, uint32_t y0)
{
    uint32_t  i;
    TCOLOR  bakc;

    for (i = x0 + 1; i < GUI_LCM_XMAX; i++)
    {
        GUI_ReadPoint(i, y0, &bakc);

        if (GUI_CmpColor(bakc, color) == 0) return (i - 1);	// 若找到，则返回
    }

    return (GUI_LCM_XMAX);
}


/****************************************************************************
* 名称：GUI_CmpPointColor()
* 功能：判断指定点上的颜色是否为某种颜色。
* 入口参数：x			指定点的x轴坐标值
*		   y		指定点的y轴坐标值
*          color	颜色值
* 出口参数：返回1表示相同，返回0表示不相同。
* 说明：
****************************************************************************/
int  GUI_CmpPointColor(uint32_t x, uint32_t y)
{
    TCOLOR  bakc;
    GUI_ReadPoint(x, y, &bakc);
    return (GUI_CmpColor(bakc, color));
}


/* 定义折点个数 */
#ifndef  DOWNP_N
#define  DOWNP_N		20
#endif
#ifndef	 UPP_N
#define  UPP_N			20
#endif
/****************************************************************************
* 名称：GUI_FloodFill()
* 功能：图形填充，将指定点内的封闭图形进行填充。对指定点的颜色区域进行填充，即不是该颜色
*      的像素为边界(如，指定点上的颜色为红色，则其它颜色像素均为边界)。
* 入口参数： x0		指定点的x坐标值
*           y0		指定点的y坐标值
*           color	填充颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围、指定点不在封闭图形内。
****************************************************************************/
void  GUI_FloodFill(uint32_t x0, uint32_t y0)
{
    PointXY  down_point[DOWNP_N];	// 定义向下填充转折点缓冲区
    uint8_t    down_no;				// 向下折点个数
    PointXY  up_point[UPP_N];		// 定义向上填充转折点缓冲区
    uint8_t    up_no;					// 向上折点个数
    TCOLOR   fcolor;					// 填充点上的颜色
    uint32_t  xx, yy;					// 填充临时x，y变量 (当前填充行的中点)
    uint32_t  xx0;						// 当前填充行的左x值变量
    uint32_t  xx1;						// 当前填充行的右y值变量
    uint32_t  i;
    uint32_t  x0_bak, y0_bak;
    uint32_t  x1_bak;

    /* 参数过滤 */
    if (x0 >= GUI_LCM_XMAX) return;

    if (y0 >= GUI_LCM_YMAX) return;

    /* 判断指定点是否为填充颜色，若是则直接返回 */
    GUI_ReadPoint(x0, y0, &fcolor);						// 取得填充点的颜色

    if (GUI_CmpColor(fcolor, color) != 0) return;

    y0_bak = y0;
    x0_bak = xx0 = GUI_ReadLeftPoint(x0, y0, fcolor);				// 找出当前y坐标上的最左边的点
    x1_bak = xx1 = GUI_ReadRightPoint(x0, y0, fcolor);			// 找出当前y坐标上的最右边的点
    down_point[0].x = up_point[0].x = (xx1 + xx0) / 2;
    down_point[0].y = up_point[0].y = y0;
    down_no = 1;
    up_no = 1;
    /* 开始向上填充 */
FILL_UP:

    if (0 == up_no) goto FILL_DOWN;							// 若向下扫描已完成，则退出

    xx = up_point[up_no - 1].x;							// 否则取出下一折点
    yy = up_point[up_no - 1].y;
    up_no--;
    xx0 = GUI_ReadLeftPoint(xx, yy, fcolor);
    xx1 = GUI_ReadRightPoint(xx, yy, fcolor);

    while (1)
    {
        yy += 1;											// 中心点向上一点

        if (GUI_CmpPointColor(xx, yy, fcolor) == 0)
        {
            /* 判断此点是否为终点，若是则退出此次循环 */
            for (i = xx0; i <= xx1; i++)     					// 查找此行是否有需填充点
            {
                if (GUI_CmpPointColor(i, yy, fcolor) != 0) break;
            }

            if (i > xx1) goto FILL_UP;

            /* 找出新一行中最右边的点 */
            xx = i;										// 更新xx到要填充的有效区域内
            xx1 = GUI_ReadRightPoint(xx, yy, fcolor);
        }
        else
        {
            /* 找出新一行中最右边的点 */
            xx1 = GUI_ReadRightPoint(xx, yy, fcolor);
        }

        xx0 = GUI_ReadLeftPoint(xx, yy, fcolor);

        /* 向下折点。使用y0作为折点变量，x0作为上一折点变量 */
        if (down_no < DOWNP_N)
        {
            y0 = xx0;
            x0 = y0 - 1;

            for (i = y0; i <= xx1; i++)
            {
                if (GUI_CmpPointColor(i, yy - 1, fcolor) == 0)	// 更新折点
                {
                    y0 = i;
                }
                else
                {
                    if (x0 != y0)								// 找到新的折点
                    {
                        x0 = y0;
                        down_point[down_no].x = i;
                        down_point[down_no].y = yy;
                        down_no++;
                    }
                }

                if (down_no >= DOWNP_N) break;					// 若缓冲区已保存满，则退出
            } // end  of for(i=y0+1; i<xx1; i++)
        } // end of if(down_no<DOWNP_N)

        xx = (xx1 + xx0) / 2;								// 更新中心点
        GUI_HLine(xx0, yy, xx1);					// 填充一行

        /* 向上折点。使用y0作为折点变量，x0作为上一折点变量 */
        if (up_no < UPP_N)
        {
            y0 = xx0;
            x0 = y0 - 1;

            for (i = y0; i <= xx1; i++)
            {
                if (GUI_CmpPointColor(i, yy + 1, fcolor) == 0)	// 更新折点
                {
                    y0 = i;
                }
                else
                {
                    if (x0 != y0)								// 找到新的折点
                    {
                        x0 = y0;
                        up_point[up_no].x = i;
                        up_point[up_no].y = yy;
                        up_no++;
                    }
                }

                if (up_no >= UPP_N) break;						// 若缓冲区已保存满，则退出
            }
        } // end of if(up_no<UPP_N)
    } // end of while(1)

    /* 向下填充 */
FILL_DOWN:

    if (0 == down_no)
    {
        if (0 == up_no)
        {
            GUI_HLine(x0_bak, y0_bak, x1_bak);
            return;								// 若向下扫描已完成，且没有发现新的向上折点，则退出
        }
        else
        {
            goto FILL_UP;
        }
    }

    xx = down_point[down_no - 1].x;						// 否则取出下一折点
    yy = down_point[down_no - 1].y;
    down_no--;
    xx0 = GUI_ReadLeftPoint(xx, yy, fcolor);
    xx1 = GUI_ReadRightPoint(xx, yy, fcolor);

    while (1)
    {
        yy -= 1;											// 中心点向上一点

        if (GUI_CmpPointColor(xx, yy, fcolor) == 0)
        {
            /* 判断此点是否为终点，若是则退出此次循环 */
            for (i = xx0; i <= xx1; i++)     					// 查找下一行是否有需填充点
            {
                if (GUI_CmpPointColor(i, yy, fcolor) != 0) break;
            }

            if (i > xx1) goto FILL_DOWN;

            /* 找出新一行中最右边的点 */
            xx = i;
            xx1 = GUI_ReadRightPoint(xx, yy, fcolor);
        }
        else
        {
            /* 找出新一行中最右边的点 */
            xx1 = GUI_ReadRightPoint(xx, yy, fcolor);
        }

        xx0 = GUI_ReadLeftPoint(xx, yy, fcolor);

        /* 向上折点。使用y0作为折点变量，x0作为上一折点变量 */
        if (up_no < UPP_N)
        {
            y0 = xx0;
            x0 = y0 - 1;

            for (i = y0; i <= xx1; i++)
            {
                if (GUI_CmpPointColor(i, yy + 1, fcolor) == 0)	// 更新折点
                {
                    y0 = i;
                }
                else
                {
                    if (x0 != y0)								// 找到新的折点
                    {
                        x0 = y0;
                        up_point[up_no].x = i;
                        up_point[up_no].y = yy;
                        up_no++;
                    }
                }

                if (up_no >= UPP_N) break;						// 若缓冲区已保存满，则退出
            }
        }

        xx = (xx1 + xx0) / 2;
        GUI_HLine(xx0, yy, xx1);					// 填充一行

        /* 向下折点。使用y0作为折点变量，x0作为上一折点变量 */
        if (down_no < DOWNP_N)
        {
            y0 = xx0;
            x0 = y0 - 1;

            for (i = y0; i <= xx1; i++)
            {
                if (GUI_CmpPointColor(i, yy - 1, fcolor) == 0)	// 更新折点
                {
                    y0 = i;
                }
                else
                {
                    if (x0 != y0)								// 找到新的折点
                    {
                        x0 = y0;
                        down_point[down_no].x = i;
                        down_point[down_no].y = yy;
                        down_no++;
                    }
                }

                if (down_no >= DOWNP_N) break;					// 若缓冲区已保存满，则退出
            }
        } // end of if(down_no<DOWNP_N)
    } // end of while(1)

    GUI_HLine(x0_bak, y0_bak, x1_bak);
}
#endif




#if  GUI_ArcX_EN==1
/****************************************************************************
* 名称：GUI_Arc4()
* 功能：画弧。起点及终点只能为0度-90度、90度-180度、180度-270度、270度-0度等。即分别
*      为第1-4像限的90度弧。
* 入口参数： x0		圆心的x坐标值
*           y0		圆心的y坐标值
*           r       圆弧的半径
*           angle	画弧的像限(1-4)
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Arc4(uint16_t x, uint16_t y, uint16_t r, uint8_t angle)
{
    int16_t  draw_x, draw_y;
    int16_t  op_x, op_y;
    int16_t  op_2rr;

    if (r == 0) return;

    op_2rr = 2 * r * r;										// 计算r平方乖以2

    switch (angle)
    {
        case  1:
            draw_x = x + r;
            draw_y = y;
            op_x = r;
            op_y = 0;

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_y++;
                draw_y++;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_x--;
                    draw_x--;
                }

                if (op_y >= op_x) break;
            }

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_x--;
                draw_x--;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_y++;
                    draw_y++;
                }

                if (op_x <= 0)
                {
                    GUI_Point(draw_x, draw_y);		// 开始画图
                    break;
                }
            }

            break;

        case  2:
            draw_x = x - r;
            draw_y = y;
            op_x = r;
            op_y = 0;

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_y++;
                draw_y++;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_x--;
                    draw_x++;
                }

                if (op_y >= op_x) break;
            }

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_x--;
                draw_x++;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_y++;
                    draw_y++;
                }

                if (op_x <= 0)
                {
                    GUI_Point(draw_x, draw_y);		// 开始画图
                    break;
                }
            }

            break;

        case  3:
            draw_x = x - r;
            draw_y = y;
            op_x = r;
            op_y = 0;

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_y++;
                draw_y--;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_x--;
                    draw_x++;
                }

                if (op_y >= op_x) break;
            }

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_x--;
                draw_x++;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_y++;
                    draw_y--;
                }

                if (op_x <= 0)
                {
                    GUI_Point(draw_x, draw_y);		// 开始画图
                    break;
                }
            }

            break;

        case  4:
            draw_x = x + r;
            draw_y = y;
            op_x = r;
            op_y = 0;

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_y++;
                draw_y--;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_x--;
                    draw_x--;
                }

                if (op_y >= op_x) break;
            }

            while (1)
            {
                GUI_Point(draw_x, draw_y);		// 开始画图
                /* 计算下一点 */
                op_x--;
                draw_x--;

                if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) 	// 使用逐点比较法实现画圆弧
                {
                    op_y++;
                    draw_y--;
                }

                if (op_x <= 0)
                {
                    GUI_Point(draw_x, draw_y);		// 开始画图
                    break;
                }
            }

            break;

        default:
            break;
    }
}

/****************************************************************************
* 名称：GUI_Arc()
* 功能：指定起点、终点及半径画弧(不能画圆)。使用的是顺时针方向画图。
* 入口参数： x			圆心的x轴坐标值
*           y			圆心的y轴坐标值
*           stangle 	起始角度(0-359度)
*           endangle	终止角度(0-359度)
*           r  			圆的半径终点
*           color		显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Arc(uint16_t x, uint16_t y, uint16_t r, uint16_t stangle, uint16_t endangle)
{
    int16_t  draw_x, draw_y;					// 画图坐标变量
    int16_t  op_x, op_y;						// 操作坐标
    int16_t  op_2rr;							// 2*r*r值变量
    int16_t  pno_angle;						// 度角点的个数
    uint8_t  draw_on;							// 画点开关，为1时画点，为0时不画

    /* 参数过滤 */
    if (r == 0) return;							// 半径为0则直接退出

    if (stangle == endangle) return;			// 起始角度与终止角度相同，退出

    if ((stangle >= 360) || (endangle >= 360)) return;

    op_2rr = 2 * r * r;							// 计算r平方乖以2
    pno_angle = 0;
    /* 先计算出在此半径下的45度的圆弧的点数 */
    op_x = r;
    op_y = 0;

    while (1)
    {
        pno_angle++; 							// 画点计数
        /* 计算下一点 */
        op_y++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
        {
            op_x--;
        }

        if (op_y >= op_x) break;
    }

    draw_on = 0;								// 最开始关画点开关

    /* 设置起始点及终点 */
    if (endangle > stangle) draw_on = 1;		// 若终点大于起点，则从一开始即画点(359)

    stangle = (360 - stangle) * pno_angle / 45;
    endangle = (360 - endangle) * pno_angle / 45;

    if (stangle == 0) stangle = 1;

    if (endangle == 0) endangle = 1;

    /* 开始顺时针画弧，从359度开始(第4像限) */
    pno_angle = 0;
    draw_x = x + r;
    draw_y = y;
    op_x = r;
    op_y = 0;

    while (1)
    {
        /* 计算下一点 */
        op_y++;
        draw_y--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
        {
            op_x--;
            draw_x--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_y >= op_x)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_x--;
        draw_x--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_y++;
            draw_y--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_x <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    /* 开始顺时针画弧，从269度开始(第3像限) */
    draw_y = y - r;
    draw_x = x;
    op_y = r;
    op_x = 0;

    while (1)
    {
        /* 计算下一点 */
        op_x++;
        draw_x--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_y + 1) > 0) // 使用逐点比较法实现画圆弧
        {
            op_y--;
            draw_y++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_x >= op_y)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_y--;
        draw_y++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_x + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_x++;
            draw_x--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_y <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }

    /* 开始顺时针画弧，从179度开始(第2像限) */
    draw_x = x - r;
    draw_y = y;
    op_x = r;
    op_y = 0;

    while (1)
    {
        /* 计算下一点 */
        op_y++;
        draw_y++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
        {
            op_x--;
            draw_x++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_y >= op_x)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_x--;
        draw_x++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_y++;
            draw_y++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_x <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    /* 开始顺时针画弧，从89度开始(第1像限) */
    draw_y = y + r;
    draw_x = x;
    op_y = r;
    op_x = 0;

    while (1)
    {
        /* 计算下一点 */
        op_x++;
        draw_x++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_y + 1) > 0) // 使用逐点比较法实现画圆弧
        {
            op_y--;
            draw_y--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_x >= op_y)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_y--;
        draw_y--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_x + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_x++;
            draw_x++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);
        }

        if (op_y <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }
}
#endif




#if  GUI_Pieslice_EN==1
/****************************************************************************
* 名称：GUI_Pieslice()
* 功能：指定起点、终点及半径扇形(不能画圆)。使用的是顺时针方向画图。
* 入口参数： x			圆心的x轴坐标值
*           y			圆心的y轴坐标值
*           stangle 	起始角度(0-359度)
*           endangle	终止角度(0-359度)
*           r  			圆的半径终点
*           color		显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_Pieslice(uint32_t x, uint32_t y, uint32_t r, uint32_t stangle, uint32_t endangl)
{
    int32_t  draw_x, draw_y;					// 画图坐标变量
    int32_t  op_x, op_y;						// 操作坐标
    int32_t  op_2rr;							// 2*r*r值变量
    int32_t  pno_angle;						// 度角点的个数
    uint8_t  draw_on;							// 画点开关，为1时画点，为0时不画

    /* 参数过滤 */
    if (r == 0) return;							// 半径为0则直接退出

    if (stangle == endangle) return;			// 起始角度与终止角度相同，退出

    if ((stangle >= 360) || (endangle >= 360)) return;

    op_2rr = 2 * r * r;							// 计算r平方乖以2
    pno_angle = 0;
    /* 先计算出在此半径下的45度的圆弧的点数 */
    op_x = r;
    op_y = 0;

    while (1)
    {
        pno_angle++; 							// 画点计数
        /* 计算下一点 */
        op_y++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
        {
            op_x--;
        }

        if (op_y >= op_x) break;
    }

    draw_on = 0;								// 最开始关画点开关

    /* 设置起始点及终点 */
    if (endangle > stangle) draw_on = 1;		// 若终点大于起点，则从一开始即画点(359)

    stangle = (360 - stangle) * pno_angle / 45;
    endangle = (360 - endangle) * pno_angle / 45;

    if (stangle == 0) stangle = 1;

    if (endangle == 0) endangle = 1;

    /* 开始顺时针画弧，从359度开始(第4像限) */
    pno_angle = 0;
    draw_x = x + r;
    draw_y = y;
    op_x = r;
    op_y = 0;

    while (1)
    {
        /* 计算下一点 */
        op_y++;
        draw_y--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
        {
            op_x--;
            draw_x--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_y >= op_x)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_x--;
        draw_x--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_y++;
            draw_y--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_x <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    /* 开始顺时针画弧，从269度开始(第3像限) */
    draw_y = y - r;
    draw_x = x;
    op_y = r;
    op_x = 0;

    while (1)
    {
        /* 计算下一点 */
        op_x++;
        draw_x--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_y + 1) > 0) // 使用逐点比较法实现画圆弧
        {
            op_y--;
            draw_y++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_x >= op_y)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_y--;
        draw_y++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_x + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_x++;
            draw_x--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_y <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }

    /* 开始顺时针画弧，从179度开始(第2像限) */
    draw_x = x - r;
    draw_y = y;
    op_x = r;
    op_y = 0;

    while (1)
    {
        /* 计算下一点 */
        op_y++;
        draw_y++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_x + 1) > 0) 	// 使用逐点比较法实现画圆弧
        {
            op_x--;
            draw_x++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_y >= op_x)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_x--;
        draw_x++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_y + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_y++;
            draw_y++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_x <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    /* 开始顺时针画弧，从89度开始(第1像限) */
    draw_y = y + r;
    draw_x = x;
    op_y = r;
    op_x = 0;

    while (1)
    {
        /* 计算下一点 */
        op_x++;
        draw_x++;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr - 2 * op_y + 1) > 0) // 使用逐点比较法实现画圆弧
        {
            op_y--;
            draw_y--;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_x >= op_y)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);		// 开始画图

            break;
        }
    }

    while (1)
    {
        /* 计算下一点 */
        op_y--;
        draw_y--;

        if ((2 * op_x * op_x + 2 * op_y * op_y - op_2rr + 2 * op_x + 1) <= 0) // 使用逐点比较法实现画圆弧
        {
            op_x++;
            draw_x++;
        }

        if (draw_on == 1) GUI_Point(draw_x, draw_y);			// 开始画图

        pno_angle++;

        if ((pno_angle == stangle) || (pno_angle == endangle))			// 若遇到起点或终点，画点开关取反
        {
            draw_on = 1 - draw_on;

            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            GUI_Line(x, y, draw_x, draw_y);
        }

        if (op_y <= 0)
        {
            if (draw_on == 1) GUI_Point(draw_x, draw_y);

            break;
        }
    }
}
#endif

