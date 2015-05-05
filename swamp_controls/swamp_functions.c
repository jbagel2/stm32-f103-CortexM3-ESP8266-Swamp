

#include "swamp_functions.h"

void Swamp_relay_GPIO_Init()
{
	GPIO_InitTypeDef Relay_ESP8266_Config;

	Relay_ESP8266_Config.GPIO_Mode = GPIO_Mode_Out_OD; // For PNP Transistor base sink
	Relay_ESP8266_Config.GPIO_Speed = GPIO_Speed_2MHz;
	Relay_ESP8266_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &Relay_ESP8266_Config);
}


void Swamp_Init()
{
	Swamp_relay_GPIO_Init();
	GPIOB->BSRR = PUMP_PIN | FAN_LOW_PIN | FAN_HIGH_PIN; //Make sure all relays are off at startup
}


void PumpControl(On_Off mode)
{
	switch (mode) {
		case ON:
		GPIOB->BRR = PUMP_PIN;
			break;
		case OFF:
		GPIOB->BSRR = PUMP_PIN;
			break;
		default:
		GPIOB->BSRR = PUMP_PIN;
			break;
	}
}

void FanControl(Fan_Mode mode)
{
	switch (mode){
	case Fan_Low:
		FanLOW();
		break;
	case Fan_High:
		FanHIGH();
		break;
	case Fan_OFF:
		FanOFF();
		break;
	default:
		FanOFF();
		break;
	}
}

void FanOFF()
{
	GPIOB->BSRR = FAN_LOW_PIN;
	GPIOB->BSRR = FAN_HIGH_PIN;
}

void FanLOW()
{
	GPIOB->BSRR = FAN_LOW_PIN;
	GPIOB->BRR = FAN_HIGH_PIN;
}

void FanHIGH()
{
	GPIOB->BRR = FAN_LOW_PIN;
	GPIOB->BRR = FAN_HIGH_PIN;
}
