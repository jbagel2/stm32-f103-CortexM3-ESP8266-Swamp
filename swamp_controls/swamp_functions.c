/*
 * Pump -> PB12
 * Fan Low -> PB13
 * Fan High -> PB14
*/

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

void Refresh_States()
{
	PumpControl(pumpMode_Current);
	FanControl(fanMode_Current);
}

void Update_State_Variables(KeyValuePair_String_Uint16_t newStates)
{

	if(strstr(newStates.key, "pump"))
	{
		pumpMode_Current = newStates.value;
	}
	else if(strstr(newStates.key, "fan"))
	{
		fanMode_Current = newStates.value;
	}
}

//---------- Control request evaluation -----------

void PumpControl(On_Off mode)
{
	switch (mode) {
		case ON:
			PumpON();
			break;
		case OFF:
			PumpOFF();
			break;
		default:
			PumpOFF();
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
//---------- ---------------------- ------------

//---------- Relay Control Section ------------

void PumpON()
{
	GPIOB->BRR = PUMP_PIN;
	pumpMode_Current = ON;
}

void PumpOFF()
{
	GPIOB->BSRR = PUMP_PIN;
	pumpMode_Current = OFF;
}

void FanOFF()
{
	GPIOB->BSRR = FAN_LOW_PIN;
	GPIOB->BSRR = FAN_HIGH_PIN;
	fanMode_Current = Fan_OFF;
}

void FanLOW()
{
	GPIOB->BRR = FAN_LOW_PIN;
	GPIOB->BSRR = FAN_HIGH_PIN;
	fanMode_Current = Fan_Low;
}

void FanHIGH()
{
	GPIOB->BSRR = FAN_LOW_PIN;
	GPIOB->BRR = FAN_HIGH_PIN;
	fanMode_Current = Fan_High;
}

//---------- ---------------------- ------------


