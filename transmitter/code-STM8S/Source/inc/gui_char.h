/*
 * @Author: zhouli
 * @Date: 2020-05-25 01:15:10
 * @LastEditTime: 2020-05-26 00:09:38
 * @Description: file content
 */ 
#ifndef  GUI_CHAR_H
#define  GUI_CHAR_H

void GUI_ShowChar(uint8_t x, uint8_t y, uint8_t chr, const FONT_t *font);
void GUI_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, const FONT_t *font);
void GUI_Showfloat(uint16_t x, uint16_t y, uint32_t num, char uint, uint8_t len, uint8_t point, const FONT_t *font);
void GUI_ShowString(uint8_t x, uint8_t y, const uint8_t *p, const FONT_t *font);


#endif

