


#ifndef __SWAMP_CONTROLS_H__
#define __SWAMP_CONTROLS_H__

#include "stm32f10x_gpio.h"
#include "KeyValuePair.h"

#define FAN_HIGH_PIN GPIO_Pin_14
#define FAN_LOW_PIN GPIO_Pin_13
#define PUMP_PIN GPIO_Pin_12

typedef enum
{
	Fan_OFF,
	Fan_Low,
	Fan_High
}Fan_Mode;

typedef enum
{
	OFF,
	ON
}On_Off;


On_Off pumpMode_Current;
Fan_Mode fanMode_Current;
On_Off fanLow_Current;
On_Off fanHigh_Current;
uint8_t temp_Current; //Static for testing
uint8_t humid_Current; //Static for testing



void Swamp_Init();
void Refresh_States();
void PumpControl(On_Off mode);
void FanControl(Fan_Mode mode);

//May want to keep these private eventually
void PumpOFF();
void PumpON();
void FanOFF();
void FanLOW();
void FanHIGH();


#endif //__SWAMP_CONTROLS_H__
