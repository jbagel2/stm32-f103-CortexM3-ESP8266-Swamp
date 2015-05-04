


#ifndef __SWAMP_CONTROLS_H__
#define __SWAMP_CONTROLS_H__

#include "stm32f10x_gpio.h"


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


static On_Off pumpMode_Current = OFF;
static Fan_Mode fanMode_Current = Fan_OFF;
static On_Off fanLow_Current = OFF;
static On_Off fanHigh_Current = OFF;
static uint8_t temp_Current = 70; //Static for testing
static uint8_t humid_Current = 15; //Static for testing



void Swamp_Init();
void PumpControl(On_Off mode);
void FanControl(Fan_Mode mode);

//May want to keep these private eventually
void FanOFF();
void FanLOW();
void FanHIGH();


#endif //__SWAMP_CONTROLS_H__
