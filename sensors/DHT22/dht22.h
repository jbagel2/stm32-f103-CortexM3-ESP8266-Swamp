

#ifndef __DHT22__
#define __DHT22__

typedef struct
{
 uint8_t Response;
	uint8_t TempInteger;
	uint8_t TempRemain;
	uint8_t HumidInterger;
	uint8_t HumidRemain;
	uint8_t ValidChecksum;
}DHT22_Data;

void DHT22_Init();
void DHT22_Start_Read();
void DHT22_Config_CLK();
void DHT22_Config_GPIO_INPUT();
void DHT22_Config_GPIO_OUTPUT();
void DHT22_Config_EXTInterrupt_Enable();
void DHT22_Config_EXTInterrupt_Disable();
void DHT22_Config_NVIC();


#endif