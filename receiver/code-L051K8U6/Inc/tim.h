/**
  ******************************************************************************
  * File Name          : TIM.h
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim22;

/* USER CODE BEGIN Private defines */
#define THROTTLE_MAX 1000

/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM6_Init(void);
void MX_TIM22_Init(void);
                        
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                        
/* USER CODE BEGIN Prototypes */

void throttle_init(void);
void throttle_set(uint16_t throttle);
void bootstrap_init(void);
void bootstrap_enable(uint8_t state);
uint8_t bootstrap_isenable(void);
void light_init(void);
void light_set(uint8_t ch ,uint32_t val);



/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ tim_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
