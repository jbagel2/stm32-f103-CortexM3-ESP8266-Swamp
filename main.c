#include "globalDefines.h"
#include "time.h"
#include "USART3_Config.h"

#include "esp8266/include/esp8266.h"
#include "Server/WebServer.h"

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
 *
 * Fan Modes
 * 0 -> Fan Off
 * 1 -> Fan Low
 * 2 -> Fan High
 *
 * Pump Modes
 * 0 -> Pump Off
 * 1 -> Pump On
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

uint8_t pumpMode_Current = 0;
uint8_t fanLow_Current = 0;
uint8_t fanHigh_Current = 0;
uint8_t temp_Current = 70;
uint8_t humid_Current = 15;


uint32_t testTimeStamp = 0;
uint32_t debounceCurrent = 0;
uint32_t debounceTime_ms = 300;
uint32_t lastDMABuffPoll = 0;
char customRESTResponse[400];

uint32_t mj = 0;


void RefreshCustomRESTResponse(char *IPWAN, char *IPLAN, char *nodeKeyName, char *nodeValue);

void RelayStartMode()
{
	GPIOB->BSRR = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14; //Make sure all relays are off at startup
}

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
	Relay_ESP8266_Config.GPIO_Mode = GPIO_Mode_Out_OD; // For PNP Transistor base sink
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
		case ON:
		GPIOB->BRR = GPIO_Pin_12;
			break;
		case OFF:
		GPIOB->BSRR = GPIO_Pin_12;
			break;
		default:
		GPIOB->BSRR = GPIO_Pin_12;
			break;
	}
}

void FanControl(Fan_Mode mode)
{

}

int main(void)
{

	Init_Time(MILLISEC);
	Init_USART3_DMA(2000000,USART3_RxBuffer, RxBuffSize);
	Wifi_Init();
	Wifi_OFF();

	void MapleButtonConfig();
	RelayGPIOConfig();
	RelayStartMode();
	//SetSystemClockOut();
	Wifi_ON();

	for (mj=0;mj<130500;mj++);

	Wifi_SendCommand(WIFI_JOIN_NONYA);
	StartServer(1,80);
	Wifi_SendCommand(WIFI_GET_CURRENT_IP);

	testTimeStamp = Millis();
    while(1)
    {
    	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) && ((Millis() - debounceCurrent) >= debounceTime_ms))
    	{
    		debounceCurrent = Millis();
    		pumpMode_Current ^= (1<<0);
    		PumpControl(pumpMode_Current);
    	}

    	if((Millis() - lastDMABuffPoll) >= DMA_Rx_Buff_Poll_Int_ms)
    			{
    				lastDMABuffPoll = Millis();
    				currentIPD = Wifi_CheckDMABuff_ForIPDData();
    				if(currentIPD.Valid == 1)
    				{
    					SendRESTResponse(currentIPD.ConnectionNum, RESTResponse_Headers_Test_OK, customRESTResponse);
    					PumpControl(pumpMode_Current);
    				}
    			}
    }
}

void RefreshCustomRESTResponseSwamp(char *IPWAN, char *IPLAN, uint8_t pumpState, uint8_t fanState, uint8_t currentTemp, uint8_t currentHumid)
{
#ifndef NODE_ID
#error NODE_ID not defined, Please define NODE_ID as char*
#endif
snprintf(customRESTResponse, ARRAYSIZE(customRESTResponse),"{\"ID\":\"%s\",\"NodeStatus\":{\"pumpState\":\"%d\",\"fanState\":\"%d\",\"currentTemp\":\"%d\",\"currentHumid\":\"%d\",},\"GeneralStatus\":{\"CurrentIP_WAN\":\"%s\",\"currentIP_LAN\":\"%s\",\"self_check_result\":\"OK\"}} ",NODE_ID, pumpState, fanState, currentTemp, currentHumid, IPWAN, IPLAN);
}

void RefreshCustomRESTResponse(char *IPWAN, char *IPLAN, char *nodeKeyName, char *nodeValue)
{
#ifndef NODE_ID
#error NODE_ID not defined, Please define NODE_ID as char*
#endif
snprintf(customRESTResponse, ARRAYSIZE(customRESTResponse),"{\"ID\":\"%s\",\"Status\":{\"%s\":\"%s\",\"CurrentIP_WAN\":\"%s\",\"currentIP_LAN\":\"%s\",\"self_check_result\":\"OK\"}} ",NODE_ID, nodeKeyName, nodeValue, IPWAN, IPLAN);
}


