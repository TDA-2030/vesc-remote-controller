/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:38:35
 * @Description: file content
 */

#include "type_def.h"
#include "led.h"
#include "delay.h"


// Gamma correction table
static const uint8_t gammaTable[256] = {
     0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
     2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   4,   4,   4,   4,
     5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
    10,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,  14,  15,  15,  16,  16,
    17,  17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,
    26,  26,  27,  28,  28,  29,  30,  30,  31,  32,  32,  33,  34,  34,  35,  36,
    37,  37,  38,  39,  40,  41,  41,  42,  43,  44,  45,  45,  46,  47,  48,  49,
    50,  51,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  80,  81,
    82,  83,  84,  85,  86,  88,  89,  90,  91,  92,  94,  95,  96,  97,  98, 100,
   101, 102, 103, 105, 106, 107, 109, 110, 111, 113, 114, 115, 117, 118, 119, 121,
   122, 123, 125, 126, 128, 129, 130, 132, 133, 135, 136, 138, 139, 141, 142, 144,
   145, 147, 148, 150, 151, 153, 154, 156, 157, 159, 161, 162, 164, 165, 167, 169,
   170, 172, 173, 175, 177, 178, 180, 182, 183, 185, 187, 189, 190, 192, 194, 196,
   197, 199, 201, 203, 204, 206, 208, 210, 212, 213, 215, 217, 219, 221, 223, 225,
   226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 255 };


/**************************************************************************
 * 函数名：PWM_Init
 * 描述  ：PWM初始化
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 *************************************************************************/
static void PWM_Init(void)
{
    u16 TIM1_Period = 256 - 1; //计数
    u16 TIM1_Prescaler = 15; //16分频
    TIM1_CR1 &= ~(MASK_TIM1_CR1_CEN);
    //------------------------------初始化TIM1时间基本单位----------------------------
    /* Set the Prescaler value */
    TIM1_PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
    TIM1_PSCRL = (uint8_t)(TIM1_Prescaler);
    /* Set the Autoreload value */
    TIM1_ARRH = (uint8_t)(TIM1_Period >> 8);
    TIM1_ARRL = (uint8_t)(TIM1_Period);
    /* Select the Counter Mode */
    TIM1_CR1 = (uint8_t)((1 << 7) | (0 << 5) | (0 << 4) | (0 << 3) | (0 << 1));
    /* Set the Repetition Counter value */
    TIM1_RCR = 0x00;
    //--------------------------------初始化TIM1 Channel1，Channel2，Channel3------------------------
    /* Set the Output State & Set the Output N State & Set the Output Polarity &
    Set the Output N Polarity */
    TIM1_CCER1 = 0x00;
    TIM1_CCER2 = 0x00;
    TIM1_CCER1 |= (uint8_t)((0 << 5) | (1 << 4) | (0 << 1) | (1 << 0));
    TIM1_CCER2 |= (uint8_t)((0 << 5) | (1 << 4) | (0 << 1) | (1 << 0));
    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM1_CCMR1 = (uint8_t)((uint8_t)(TIM1_CCMR1 & (uint8_t)(~MASK_TIM1_CCMR1_OC1M)) | (uint8_t)0x70);
    TIM1_CCMR2 = (uint8_t)((uint8_t)(TIM1_CCMR2 & (uint8_t)(~MASK_TIM1_CCMR2_OC2M)) | (uint8_t)0x70);
    TIM1_CCMR3 = (uint8_t)((uint8_t)(TIM1_CCMR3 & (uint8_t)(~MASK_TIM1_CCMR3_OC3M)) | (uint8_t)0x70);
    TIM1_CCMR4 = (uint8_t)((uint8_t)(TIM1_CCMR4 & (uint8_t)(~MASK_TIM1_CCMR4_OC4M)) | (uint8_t)0x70);
    /* set  the CEN Bit */
    TIM1_CR1 |= MASK_TIM1_CR1_CEN;
    /* Set the MOE Bit */
    TIM1_BKR |= MASK_TIM1_BKR_MOE;
}

static void PWM_Close(void)
{
    /* set  the CEN Bit */
    TIM1_CR1 &= ~(MASK_TIM1_CR1_CEN);
}

static void PWM_Open(void)
{
    /* set  the CEN Bit */
    TIM1_CR1 |= MASK_TIM1_CR1_CEN;
}


static void SetTIM1_PWM_DutyCycle(uint8_t ch, uint8_t Compare)
{
    Compare = gammaTable[Compare];
    if ('G' == ch)
    {
        /* Set the Capture Compare3 Register value */
        TIM1_CCR3H = (uint8_t)(Compare >> 8);
        TIM1_CCR3L = (uint8_t)(Compare);
    }
    else if ('B' == ch)
    {
        /* Set the Capture Compare3 Register value */
        TIM1_CCR2H = (uint8_t)(Compare >> 8);
        TIM1_CCR2L = (uint8_t)(Compare);
    }
    else if ('L' == ch)
    {
        /* Set the Capture Compare3 Register value */
        TIM1_CCR1H = (uint8_t)(Compare >> 8);
        TIM1_CCR1L = (uint8_t)(Compare);
    }
    else if ('R' == ch)
    {
        /* Set the Capture Compare4 Register value */
        TIM1_CCR4H = (uint8_t)(Compare >> 8);
        TIM1_CCR4L = (uint8_t)(Compare);
    }
}



/**
 * ————————————————
 * 版权声明：本文为CSDN博主「liefyuan」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
 * 原文链接：https://blog.csdn.net/qq_28877125/java/article/details/83022379
*/
static void HSVtoRGB(uint16_t h, uint16_t s, uint16_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    // R,G,B from 0-255, H from 0-360, S,V from 0-100
    int i;
    float RGB_min, RGB_max;
    RGB_max = v * 2.55f;
    RGB_min = RGB_max * (100 - s) / 100.0f;
    i = h / 60;
    int difs = h % 60; // factorial part of h
    // RGB adjustment amount by hue
    float RGB_Adj = (RGB_max - RGB_min) * difs / 60.0f;

    switch (i)
    {
        case 0:
            *r = (uint8_t)RGB_max;
            *g = (uint8_t)(RGB_min + RGB_Adj);
            *b = (uint8_t)RGB_min;
            break;

        case 1:
            *r = (uint8_t)(RGB_max - RGB_Adj);
            *g = (uint8_t)RGB_max;
            *b = (uint8_t)RGB_min;
            break;

        case 2:
            *r = (uint8_t)RGB_min;
            *g = (uint8_t)RGB_max;
            *b = (uint8_t)(RGB_min + RGB_Adj);
            break;

        case 3:
            *r = (uint8_t)RGB_min;
            *g = (uint8_t)(RGB_max - RGB_Adj);
            *b = (uint8_t)RGB_max;
            break;

        case 4:
            *r = (uint8_t)(RGB_min + RGB_Adj);
            *g = (uint8_t)RGB_min;
            *b = (uint8_t)RGB_max;
            break;

        default:		// case 5:
            *r = (uint8_t)RGB_max;
            *g = (uint8_t)RGB_min;
            *b = (uint8_t)(RGB_max - RGB_Adj);
            break;
    }
}

void led_init(void)
{
    PC_DDR |= 0x0f << 1; /* 设置数据方向寄存器 1为输出，0为输入--查看STM8寄存器.pdf P87 */
    PC_CR1 |= 0x0f << 1; /* 设置推挽输出--查看STM8寄存器.pdf P88 */
    PC_CR2 |= 0x0f << 1; /* 设置输出频率 1为10M，0为2M--查看STM8寄存器.pdf P89 */
    PWM_Init();
    PWM_Open();
    SetTIM1_PWM_DutyCycle('L', 0);
    SetTIM1_PWM_DutyCycle('R', 0);
    SetTIM1_PWM_DutyCycle('G', 0);
    SetTIM1_PWM_DutyCycle('B', 0);
}

void led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    SetTIM1_PWM_DutyCycle('R', r);
    SetTIM1_PWM_DutyCycle('G', g);
    SetTIM1_PWM_DutyCycle('B', b);
}

void led_set_hsv(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t r, g, b;
    HSVtoRGB(h, s, v, &r, &g, &b);
    SetTIM1_PWM_DutyCycle('R', r);
    SetTIM1_PWM_DutyCycle('G', g);
    SetTIM1_PWM_DutyCycle('B', b);
}
void led_set_headlight(uint16_t v)
{
    SetTIM1_PWM_DutyCycle('L', v);
}


/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/