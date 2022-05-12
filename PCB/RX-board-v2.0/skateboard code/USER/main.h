#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"


#define ADC_ARRAY_SIZE 3



typedef struct
{
	uint32_t check;
	float vin_voltage;
	float cap_cur;
	
	int16_t adc_offset_crg_cur;
	int16_t adc_offset_dcrg_cur;
	int16_t adc_offset_vol;
	
	uint8_t auto_charge:1;
	uint8_t auto_weld:1;
	uint8_t :6;
	uint16_t weld_energy;
	
}Setting_TypeDef;
extern Setting_TypeDef setting;

typedef struct
{
	int16_t ADC_Value_Array[ADC_ARRAY_SIZE]; //ADC DMA´«ÊäµØÖ·
	
	float charge_cur;
	float discharge_cur;
	uint16_t ntc;
	uint16_t vin;
	uint8_t contact;
	uint64_t welding_energy;
	
}sample_TypeDef;
extern sample_TypeDef SampleData;


typedef struct
{
	uint8_t state;

}System_TypeDef;
extern System_TypeDef system;

enum{
	SYSTEM_STATE_IDLE,
	SYSTEM_STATE_RUNNING,
	SYSTEM_STATE_CHARGING,
	SYSTEM_STATE_STANDBY,
	SYSTEM_STATE_NO_SIGNAL,
};


#endif
