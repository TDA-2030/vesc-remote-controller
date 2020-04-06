
#ifndef __LED_CONF_H
#define __LED_CONF_H

//#define LED_R     PC_ODR_ODR4
//#define LED_G     PC_ODR_ODR3
//#define LED_B     PC_ODR_ODR2
//#define LED_LG    PC_ODR_ODR1

/* LED  */
void led_init(void);
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b);
void led_set_hsv(uint16_t h, uint8_t s, uint8_t v);
void led_set_headlight(uint16_t v);

#endif




