/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#include <stdio.h>
extern char error_message[];
#define Error_Handler() {snprintf(error_message, 32, "err:%s,%d\r\n", __FILE__, __LINE__);__Error_Handler();}
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void __Error_Handler(void);

/* USER CODE BEGIN EFP */

#define ADC_ARRAY_SIZE 3

typedef struct
{
	uint32_t check;
	float vin_voltage;
	float cap_cur;
	
	int16_t adc_offset_crg_cur;
	int16_t adc_offset_dcrg_cur;
	int16_t adc_offset_vol;

	
}Setting_TypeDef;
extern Setting_TypeDef setting;

typedef struct
{
	int16_t ADC_Value_Array[ADC_ARRAY_SIZE]; //ADC DMA传输地址
	
	uint16_t charge_cur;
	uint16_t discharge_cur;
	uint16_t vin;
	
	uint16_t speed;            //速度值
	uint32_t tacho;           //里程
    uint16_t tacho_single;    //单次里程
	uint16_t mot_current;     //电机电流
	uint16_t bat_current;     //电池电流
	uint8_t board_temp;       //板子温度
	uint16_t wh_drawn;        //消耗能量
	uint16_t wh_regen;        //再生能量
	uint16_t ah_drawn;        //消耗能量
	uint16_t ah_regen;        //再生能量
	
}sample_TypeDef;
extern sample_TypeDef SampleData;


typedef struct
{
	volatile uint8_t state;

}System_TypeDef;
extern System_TypeDef system;

enum{
	SYSTEM_STATE_STANDBY,  //低功耗待机
	SYSTEM_STATE_IDLE,     //已连接，未开机
	SYSTEM_STATE_RUNNING,  //运行中
	SYSTEM_STATE_CHARGING, //充电中
	SYSTEM_STATE_NO_SIGNAL,//信号丢失
};


extern void(*exit2_callback)(void);
extern void(*exit3_callback)(void);
extern void(*usart1_callback)(void);
extern void(*usart2_callback)(void);




void delay_us(uint32_t nus);




/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin LL_GPIO_PIN_14
#define LED_GPIO_Port GPIOC
#define NRF_CS_Pin LL_GPIO_PIN_4
#define NRF_CS_GPIO_Port GPIOA
#define NRF_CE_Pin LL_GPIO_PIN_6
#define NRF_CE_GPIO_Port GPIOA
#define MPU_INT_Pin LL_GPIO_PIN_2
#define MPU_INT_GPIO_Port GPIOB
#define MPU_INT_EXTI_IRQn EXTI2_3_IRQn
#define NRF_IRQ_Pin LL_GPIO_PIN_3
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_IRQ_EXTI_IRQn EXTI2_3_IRQn
#define IO_VIN_EN_Pin LL_GPIO_PIN_5
#define IO_VIN_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
