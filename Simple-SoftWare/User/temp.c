 
#ifdef TEMP

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Control_Mode.h"
#include "Steering_engine.h"
#include "lcd.h"
#include "pic.h"
#include "MP3.h"
#include "string.h"


float vol;
float deviation;

void Mian_Init(void);

int main(void)
{
	Mian_Init();

	while(1)												//main
	{
		Delay_ms(2);
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			Delay_ms(2);
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);

			// RFID_CAR.CMD = CMD_Read;
			printf("key \r\n");
		}
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)		// 從
		{
			Delay_ms(2);
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)
			{
				Steering_Engine_Angle(1,90);
			}
			while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0);
			Steering_Engine_Angle(1,0);
		}
	}
}

/////////////////////////////



void Mian_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}
	
	UART_Init(DEBUG_OUT, 9600);		//v831
	UART_Init(3, 115200);			//RFID
	Init_Steering_Engine_T4();

	Steering_Engine_Angle(1,0); 
	Steering_Engine_Angle(2,0); 
	Steering_Engine_Angle(3,0); 
//	Steering_Engine_Angle(4,0);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //复用推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	UART_Send_String(DEBUG_OUT," \n hello world !\r\n");
	
}

#endif
