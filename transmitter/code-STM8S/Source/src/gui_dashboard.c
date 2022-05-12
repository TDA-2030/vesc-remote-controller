/*
 * @Author: zhouli
 * @Date: 2020-05-25 01:14:54
 * @LastEditTime: 2020-06-03 00:30:12
 * @Description: file content
 */


#include "oled.h"
#include "math.h"

typedef struct
{
    int8_t x_prev;
    int8_t y_prev;
    uint8_t rx0;
    uint8_t ry0;
    uint8_t r;
} dashboard_t;

static dashboard_t dbs[2];


void GUI_DashboardDraw(uint8_t db_index, uint16_t x0, uint16_t y0, uint8_t scale_num, uint16_t r)
{
    uint8_t i;
    uint16_t interval;
    int16_t angle;
    int8_t xa, ya, xb, yb;
    dbs[db_index].rx0 = x0 + r;
    dbs[db_index].ry0 = y0 + r;
    dbs[db_index].r = r - 4;
    GUI_Arc(dbs[db_index].rx0, dbs[db_index].ry0, r, 38, 142);
    interval = 256 / (scale_num - 1);
    angle = -39;

    for (i = 0; i < scale_num; i++)
    {
        float v_cos = cosf((float)angle / 57.29578f);
        float v_sin = sinf((float)angle / 57.29578f);
        xa = (r - 1) * v_cos;
        ya = (r - 1) * v_sin;
        xb = (r - 3) * v_cos;
        yb = (r - 3) * v_sin;
        GUI_Line(dbs[db_index].rx0 - xb, dbs[db_index].ry0 - yb, dbs[db_index].rx0 - xa, dbs[db_index].ry0 - ya);
        angle += interval;
    }
}

void GUI_DashboardSetAngle(uint8_t db_index, uint8_t in_angle)
{
    int16_t angle = in_angle - 38;
    GUI_SetPointColor(0);
    GUI_Line(dbs[db_index].rx0, dbs[db_index].ry0, dbs[db_index].rx0 - dbs[db_index].x_prev, dbs[db_index].ry0 - dbs[db_index].y_prev);
    GUI_SetPointColor(1);
    int8_t xx = dbs[db_index].r * cosf((float)angle / 57.29578f);
    int8_t yy = dbs[db_index].r * sinf((float)angle / 57.29578f);
    dbs[db_index].x_prev = xx;
    dbs[db_index].y_prev = yy;
    GUI_Line(dbs[db_index].rx0, dbs[db_index].ry0, dbs[db_index].rx0 - xx, dbs[db_index].ry0 - yy);
}

