

#ifndef __DHT22__
#define __DHT22__

#include "math.h"

typedef struct
{
	uint8_t Response;
	float Temp;
	float Humid;
	uint16_t CheckSumErrors;
	uint16_t CheckSumPass;
}DHT22_Data;



void DHT22_Init();
void DHT22_Start_Read(DHT22_Data *tempAndHumid, DHT22_Data *previous_tempAndHumid);
void DHT22_Config_CLK();
void DHT22_Config_GPIO_INPUT();
void DHT22_Config_GPIO_OUTPUT();
void DHT22_Config_EXTInterrupt_Enable();
void DHT22_Config_EXTInterrupt_Disable();
void DHT22_Config_NVIC();
void DHT_Value_Checksum();


#endif
