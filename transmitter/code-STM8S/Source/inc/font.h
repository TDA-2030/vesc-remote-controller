/*
 * @Author: zhouli
 * @Date: 2020-04-06 23:44:53
 * @LastEditTime: 2020-04-11 16:37:25
 * @Description: file content
 */
#ifndef __OLEDFONT_H
#define __OLEDFONT_H  

#include "type_def.h"

typedef struct {
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
} FONT_t;

extern FONT_t Font24;
extern FONT_t Font16;
extern FONT_t Font12;
extern FONT_t Font8;


extern const unsigned char BMP1[];
extern const unsigned char icon_power[];
extern const unsigned char icon_light[];
extern const unsigned char icon_arrow_up[];
extern const unsigned char icon_arrow_down[];
extern const unsigned char icon_clock[];
extern const unsigned char icon_setting[];
extern const unsigned char icon_light_s[];
extern const unsigned char icon_light_d[];
extern const unsigned char icon_low_power[];

#endif
