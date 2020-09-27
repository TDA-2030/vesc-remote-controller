/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:37:56
 * @Description: file content
 */

#include "type_def.h"
#include "exti.h"

/**
 * @brief EXIT initaliza
 * @param NONE
 * @return NONE
 */
void EXIT_conf(void)
{
  EXTI_CR1 &= (uint8_t)(~MASK_EXTI_CR1_PDIS);
  EXTI_CR1 |= (uint8_t)((uint8_t)(0x02) << 6);
}
