#include "globalDefines.h"
#include "time.h"
#include "USART3_Config.h"

#include "esp8266/include/esp8266.h"

/*
 * Swamp Cooler Relay controller
 *
 * ClockOut -> PA8
 *
 * Pump -> PB12
 * Fan Low -> PB13
 * Fan High -> PB14
 *
 * USART3 TX -> PB10
 * USART3 RX -> PB11
 *
 * DHT22 -> PB6
 *
 * DEBUG
 * SWDIO -> PA13
 * SWCLK -> PA14
 * SWO -> PB3
 *
 */



#define ESP_RX_DMA_BUF_POLL_Interval_ms 1000


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


volatile char USART3_RxBuffer[RxBuffSize];

GPIO_InitTypeDef Relay_ESP8266_Config;
IPD_Data currentIPD;

uint32_t testTimeStamp = 0;
uint32_t debounceCurrent = 0;
uint32_t debounceTime_ms = 300;

void SetSystemClockOut()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
}

void RelayGPIOConfig()
{
	Relay_ESP8266_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	Relay_ESP8266_Config.GPIO_Speed = GPIO_Speed_2MHz;
	Relay_ESP8266_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &Relay_ESP8266_Config);
}

void MapleButtonConfig()
{
	GPIO_InitTypeDef ButtonConfig;
	ButtonConfig.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	ButtonConfig.GPIO_Speed = GPIO_Speed_50MHz;
	ButtonConfig.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &ButtonConfig);
}

void PumpControl(On_Off mode)
{
	switch (mode) {
		case OFF:
		GPIOB->BRR = GPIO_Pin_12;
			break;
		case ON:
		GPIOB->BSRR = GPIO_Pin_12;
			break;
		default:
		GPIOB->BRR = GPIO_Pin_12;
			break;
	}
}

void FanControl(Fan_Mode mode)
{

}

int main(void)
{
	uint8_t pumpMode_Current = 0;
	Init_Time(MILLISEC);
	Init_USART3_DMA(2000000,USART3_RxBuffer, RxBuffSize);
	Wifi_Init();
	Wifi_OFF();

	void MapleButtonConfig();
	RelayGPIOConfig();
	//SetSystemClockOut();

	testTimeStamp = Millis();
    while(1)
    {
    	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) && ((Millis() - debounceCurrent) >= debounceTime_ms))
    	{
    		debounceCurrent = Millis();
    		pumpMode_Current ^= (1<<0);
    		PumpControl(pumpMode_Current);

    	}

    	if((Millis() - testTimeStamp) > 10000)
    	{
    		Wifi_ON();
    	}

    }
}


