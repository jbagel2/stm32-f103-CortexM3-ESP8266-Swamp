
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "dht22.h"
#include "time.h"

#define DHT22_Pin GPIO_Pin_6


GPIO_InitTypeDef DHT22_Pin_GPIO_Config;

volatile uint8_t DHT22_Buffer[5]; //5 byte array, to hold the 40 bits
volatile uint32_t upTimeStart = 0;
volatile uint32_t upTimeEnd = 0;
volatile uint32_t downTimeStart = 0;
volatile uint32_t downTimeEnd = 0;
volatile uint32_t DHT22_Bit_Time[42];
volatile uint8_t currentBit = 0;


void DHT22_Init()
{
	DHT22_Config_CLK();
	DHT22_Config_GPIO_OUTPUT();//output for pulse start
	DHT22_Config_EXTInterrupt_Enable();
	DHT22_Config_NVIC();
}


uint32_t dhtTimeStamp = 0;
void DHT22_Start_Read()
{
	currentBit =0;
	upTimeStart = 0;
	upTimeEnd = 0;
	downTimeStart = 0;
	downTimeEnd = 0;

	dhtTimeStamp = Micros();
	GPIOB->BRR = DHT22_Pin; //Pull pin LOW
	while((Micros() - dhtTimeStamp) < 1100){}
	GPIOB->BSRR = DHT22_Pin; //Pull pin HIGH
	DHT22_Config_GPIO_INPUT(); //Ready for incoming data
	//DHT22_Config_EXTInterrupt_Enable();
	dhtTimeStamp = Millis();
	while((Millis() - dhtTimeStamp) < 3000){}
	dhtTimeStamp = Millis();
	//Do something to check for data transmission completion

//Pull line low for at least 1ms

}


void DHT22_Config_CLK()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

}


void DHT22_Config_GPIO_INPUT()
{
	DHT22_Pin_GPIO_Config.GPIO_Pin = DHT22_Pin;
	DHT22_Pin_GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	DHT22_Pin_GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&DHT22_Pin_GPIO_Config);
}

void DHT22_Config_GPIO_OUTPUT()
{
	DHT22_Pin_GPIO_Config.GPIO_Pin = DHT22_Pin;
	DHT22_Pin_GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
	DHT22_Pin_GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&DHT22_Pin_GPIO_Config);
}


void DHT22_Config_EXTInterrupt_Enable()
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);
	EXTI_InitTypeDef DHT22_IntConfig;
	DHT22_IntConfig.EXTI_Line = EXTI_Line6;
	DHT22_IntConfig.EXTI_Mode = EXTI_Mode_Interrupt;
	DHT22_IntConfig.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	DHT22_IntConfig.EXTI_LineCmd = ENABLE;

	EXTI_Init(&DHT22_IntConfig);
}

void DHT22_Config_EXTInterrupt_Disable()
{
	EXTI_InitTypeDef DHT22_IntConfig;
	DHT22_IntConfig.EXTI_Line = EXTI_Line6;
	DHT22_IntConfig.EXTI_Mode = EXTI_Mode_Interrupt;
	DHT22_IntConfig.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	DHT22_IntConfig.EXTI_LineCmd = DISABLE;

	EXTI_Init(&DHT22_IntConfig);
}

void DHT22_Config_NVIC()
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef DHT22_Interrupt;

	DHT22_Interrupt.NVIC_IRQChannel = EXTI9_5_IRQn;
	DHT22_Interrupt.NVIC_IRQChannelPreemptionPriority = 4;
	DHT22_Interrupt.NVIC_IRQChannelSubPriority = 0;
	DHT22_Interrupt.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&DHT22_Interrupt);

	NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(4,15,0));
}


void EXTI9_5_IRQHandler(void)
{
if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
	if(GPIO_ReadInputDataBit(GPIOB,DHT22_Pin)) //If pin high
	{
		//currentBit++;
		upTimeStart = Micros();
		downTimeEnd = Micros();
		//if(downTimeStart != 0)
		//{
			//DHT22_Bit_Time[currentBit] = downTimeEnd - downTimeStart;
		//}
	}
	else
	{

		downTimeStart = Micros();
		upTimeEnd = Micros();
		if(upTimeStart != 0)
		{
			DHT22_Bit_Time[currentBit] = upTimeEnd - upTimeStart;
			currentBit++;
		}
	}


	//Need to count the length of pulses for DHT22 Data

    /* Clear the  EXTI line 8 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
}
