/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "comm_uart.h"
#include "bldc_interface.h"

#include "24l01.h"
#include "control.h"

#include "stmflash.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define SETTINGS_ADDR DATA_EEPROM_BASE
#define FLASH_CHECK (0x12345678)   //检验值
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

Setting_TypeDef setting={0};
sample_TypeDef SampleData={0};
System_TypeDef system={0};


//延时nus
//nus为要延时的us数.		    								   
void delay_us(uint32_t nus)
{		
	uint32_t t;
	while(nus--)
	{
		t=2;
		while(t--){}
		
	}
}


/** 
* @brief  : systick中断回调函数，1ms调用一次
* @param  : 
* @param  : 
* @return : 
*/
void HAL_SYSTICK_Callback(void)
{
	bldc_packet_timer();
	
}

/**
  * @brief  Wake Up Timer callback.
  * @param  hrtc RTC handle
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	
}

/**
  * @brief  Conversion complete callback in non-blocking mode.
  * @param  hadc ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	
}



//系统进入待机模式
void Sys_Enter_Standby(void)
{			 
	
	RF24L01_Set_Mode(MODE_PD);//设置为掉电模式
	LL_GPIO_ResetOutputPin(IO_VIN_EN_GPIO_Port, IO_VIN_EN_Pin);
//	hal_gpio_;	//复位所有IO口	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
	HAL_PWREx_EnableUltraLowPower();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);	 
}

#define ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))
/** 
* @brief  : flash参数读取
* @param  : 
* @param  : 
* @return : 
*/
int8_t setting_read(void)
{
	static uint32_t nword;
	nword = sizeof(Setting_TypeDef);
	nword = ROUND_UP(sizeof(Setting_TypeDef), 4)>>2;
	
	STMFLASH_Read(SETTINGS_ADDR,(uint32_t*)&setting,nword);
	if(setting.check != FLASH_CHECK)
	{
		setting.check = FLASH_CHECK;
		setting.adc_offset_crg_cur = 0;
		setting.adc_offset_dcrg_cur = 0;
		setting.adc_offset_vol = 0;
		
		STMFLASH_Write(SETTINGS_ADDR,(uint32_t*)&setting,nword);
		return -1;
	}
	return 0;
}


/** 
* @brief  : flash参数保存
* @param  : 
* @param  : 
* @return : 
*/
int8_t setting_write(void)
{
	uint32_t nword = ROUND_UP(sizeof(Setting_TypeDef), 4)>>2;
	setting.check = FLASH_CHECK;
	STMFLASH_Write(SETTINGS_ADDR,(uint32_t*)&setting,nword);
	return 0;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint32_t time_ls=0;
	uint32_t time_50ms=0;
	uint32_t time_bldc=0;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	
	MX_USART1_UART_Init();
	
	NRF24L01_Base_Init();//-- 包含SPI初始化
	while (NRF24L01_Check())
    {
        HAL_Delay(200);
		LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
	RF24L01_Init();
    RF24L01_Set_Mode(MODE_RX);//设置为接收模式
	system.state = SYSTEM_STATE_STANDBY;
	
//	if(__HAL_PWR_GET_FLAG(PWR_FLAG_WU))
//	{
//		uint16_t t=100;
//		LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
//		while(1)
//		{
//			delay_us(100);
//			if(0 == --t)
//			{
//				LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);
//				if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 3500, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
//				  {
//					Error_Handler();
//				  }
//				Sys_Enter_Standby();
//			}
//			if(SYSTEM_STATE_IDLE == system.state)
//			{
//				__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
//				HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
//				break;
//			}
//		}
//	}
	system.state = SYSTEM_STATE_IDLE;
	
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */
	
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
  
	if(0x32F2 != HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0))
	{
		MX_RTC_Init();
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
	}
	
//	if(0 > setting_read())
//	{
//		while(1)
//		{
//			HAL_Delay(100);
//			LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//		}
//	}setting_write();
	control_Init();
	bldc_comm_uart_init();
	MX_ADC_Init();
	
  /* USER CODE END 2 */
 
	uint8_t retry;
	uint32_t t_end;
	
	retry = 5;
	while(retry--)
	{
		bldc_interface_get_fw_version();
		t_end = HAL_GetTick() + 1000;
		while(1)
		{
			bldc_packet_process();
			if((HAL_GetTick() > t_end) || (vesc_info.fw_major>2))
			{
				break;
			}
		}
		if(vesc_info.fw_major>2)
		{
			HAL_Delay(200);
			bldc_interface_get_mcconf();
			t_end = HAL_GetTick() + 1000;
			while(1)
			{
				bldc_packet_process();
				if((HAL_GetTick() > t_end) || (vesc_info.mcconf.l_current_max != 0))
				{
					break;
				}
			}
			break;
		}
		
	}


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		bldc_packet_process();

		if(HAL_GetTick()>time_ls)
		{
			time_ls = HAL_GetTick()+1000;
			RTC_Get();
			LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}
		
		if(HAL_GetTick() > time_50ms)
		{
			time_50ms = HAL_GetTick()+50;
			HAL_ADC_Start_DMA(&hadc, (uint32_t*)SampleData.ADC_Value_Array, ADC_ARRAY_SIZE);
		}
		
		if(HAL_GetTick() > time_bldc)
		{
			time_bldc = HAL_GetTick()+600;
			if(system.state==SYSTEM_STATE_RUNNING)
			{
				bldc_interface_get_values();
			}
		}
		

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

char error_message[32];

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void __Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	while(1)
	{
		HAL_Delay(50);
		LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
