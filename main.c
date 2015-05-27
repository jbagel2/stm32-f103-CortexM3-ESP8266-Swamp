#include "globalDefines.h"
#include "time.h"
#include "USART3_Config.h"
#include "stm32f10x_flash.h"


#include "esp8266/include/esp8266.h"
#include "Server/WebServer.h"
#include "swamp_controls/swamp_functions.h"
#include "dht22.h"
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
 * DHT22 -> PB6 (leaf 16)
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




//#define ESP_RX_DMA_BUF_POLL_Interval_ms 1000





volatile char USART3_RxBuffer[RxBuffSize];
extern char customRESTResponse[400];

IPD_Data currentIPD;
ESP_Status currentESPStatus;
DHT22_Data Current_DHT22_Reading;

//uint32_t testTimeStamp = 0;
uint32_t debounceCurrent = 0;
uint32_t debounceTime_ms = 300;
uint32_t lastDMABuffPoll = 0;
uint32_t lastESPResetPoll = 0;
uint32_t lastDHT22update = 0;
#define ESP_RESET_CHECK_INTERVAL 20000 //20 seconds

#define DHT_UPDATE_INTERVAL 10000 //10 seconds

uint32_t mj = 0;


void RefreshCustomRESTResponse(char *IPWAN, char *IPLAN, char *nodeKeyName, char *nodeValue);


void Configure_HSI_Clock()
{
	FLASH_SetLatency(FLASH_ACR_LATENCY_2);

	RCC_HSICmd(ENABLE);
	RCC_HSEConfig(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_CFGR_PLLMULL16);
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);
}


//sends configured system clock (divided by 2)
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

/*
void RelayGPIOConfig()
{
	Relay_ESP8266_Config.GPIO_Mode = GPIO_Mode_Out_OD; // For PNP Transistor base sink
	Relay_ESP8266_Config.GPIO_Speed = GPIO_Speed_2MHz;
	Relay_ESP8266_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &Relay_ESP8266_Config);
}
*/

void DEBUG_DevBoardButtonConfig()
{
	GPIO_InitTypeDef ButtonConfig;
	ButtonConfig.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	ButtonConfig.GPIO_Speed = GPIO_Speed_50MHz;
	ButtonConfig.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &ButtonConfig);
}



int main(void)
{
	Configure_HSI_Clock();
	//Start SysTick and set to millisecond resolution
	Init_Time(MILLISEC,64);

	//Enable USART3 and attach DMA Circular buffer for Rx
	Init_USART3_DMA(2000000,USART3_RxBuffer, RxBuffSize);

	//Configure CH_PD pin for ESP Power Control and Set Wifi as "Initialized"
	Wifi_Init();

	//Force CH_PD Pin Low to make sure ESP has a clean boot up (a few lines down)
	Wifi_OFF();


	//*********DEBUG ONLY********** Configures Devboard button for manual command toggling.
	//void DEBUG_DevBoardButtonConfig();
	//RelayGPIOConfig();
	Swamp_Init();
	//SetSystemClockOut();
	Wifi_ON();
	for (mj=0;mj<1305000;mj++);

	uint16_t WaitForReady_TimeStmp = Millis();
	while(!Wifi_CheckDMABuff_ForReady() && (Millis() - WaitForReady_TimeStmp) < ESP_ResponseTimeout_ms){}


	//Just a static wait for now (Will add a DMA buffer parse for "ready"), for the ESP8266 boot-up
	//for (mj=0;mj<130500;mj++);

	//Connect to a given Wifi Network
	Wifi_SendCommand(WIFI_JOIN_NONYA);

	//Start the ESP hosted IPD Server
	StartServer(1,80);

	//Has the ESP Dump the current connection info to USART (IP as AP and Client (and MACs))
	Wifi_SendCommand(WIFI_GET_CURRENT_IP);

	uint8_t readyFound = 0;
	//DHT22_Init();
    while(1)
    {

    	Wifi_CheckDMABuff_ForCIFSRData();
    	//Checks (polls) the DMA buffer every {DMA_Rx_Buff_Poll_Int_ms} milliseconds
    	if((Millis() - lastDMABuffPoll) >= DMA_Rx_Buff_Poll_Int_ms)
    			{
    				lastDMABuffPoll = Millis();
    				//If IPD data is found it is converted to a IPD_Data object (basically an HTTP Request object)
    				currentIPD = Wifi_CheckDMABuff_ForIPDData(&Current_DHT22_Reading);
    				//If it is valid (if it passed the object complete validation)
    				if(currentIPD.Valid == 1)
    				{
    					//Sets the pump mode to the requested
    					PumpControl(pumpMode_Current);
    					//Sets the fan mode to the requested
    					FanControl(fanMode_Current);
    					//Sends updated REST response with updated system status
    					SendRESTResponse(currentIPD.ConnectionNum, RESTResponse_Headers_Test_OK, customRESTResponse);
    					//Wifi_CloseConnection(currentIPD.ConnectionNum);
    				}
    			}

    	if((Millis() - lastDHT22update) >= DHT_UPDATE_INTERVAL)
    	{
    		lastDHT22update = Millis();
    		DHT22_Start_Read(&Current_DHT22_Reading);
    	}

    	if((Millis() - lastESPResetPoll) >= ESP_RESET_CHECK_INTERVAL)
    	{
    		lastESPResetPoll = Millis();
    		readyFound = Wifi_CheckDMABuff_ForReady();

    		if(readyFound)
    		{
    			StartServer(1,80);
    			Wifi_SendCommand(WIFI_GET_CURRENT_IP);
    		}


    		//Check for ip and mac address data (and update the current settings if found.)

    	}

    }
}



