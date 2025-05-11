 
#ifdef JIAJU

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Steering_engine.h"
#include "lcd.h"
#include "pic.h"
#include "Ultrasonic.h"
#include "MP3.h"
#include "adc.h"

#define Debug UART_2
void Mian_Init(void);

char Free_Array[64];
char array_watch[64];

float vol;
int clo = 0;
int chance = 0;

u8 temp = 26;//定义温度
u8 humi = 62;//定义湿度
int Human = 250;
u8 Fan;
u8 LED;
char CL;
float Light=0;//定义光照度
float far = 0;
u8 try_age =0;
u8 BOOT;

int main(void)
{
	Mian_Init();

	sprintf(Free_Array, "Temp:     %d  ", temp);			// 定义温度 
	LCD_ShowString(1,2,Free_Array,BLUE,BLACK,32);
	sprintf(Free_Array, "Humidity: %d  ", humi);			// 定义湿度 
	LCD_ShowString(1,3,Free_Array,BLUE,BLACK,32);

	while(1)
	{
        //get a chance
		sprintf(array_watch, "Time: %2d:%2d:%2d  ", Timewatch.hour, Timewatch.minutes, Timewatch.second);			//1.显示时间
		LCD_ShowString(8,14,array_watch,WHITE,BLACK,16); Data_Replace("123", array_watch, 0, sizeof(array_watch));	//1.显示时间
		
		if(CV_UART.Read_Flag[DEBUG_OUT] == 1)																//2.收到视觉模块的数据
        {
			if(CV_UART.Rxd_Num[DEBUG_OUT] > 5 && CV_UART.UARTX_Array[2][0] == '{')
			{
				if(Data_Judge("{\"Ble\":LED-ON}",CV_UART.UARTX_Array[2],CV_UART.Rxd_Num[DEBUG_OUT]) > 0)		//	ON
				{
					LED = 1;
					BOOT = 1;
					printf("LED ON\r\n");
				}
				else if(Data_Judge("{\"Ble\":LED-OF}",CV_UART.UARTX_Array[2],CV_UART.Rxd_Num[DEBUG_OUT]) > 0)
				{
					BOOT = 0;
					LED = 0;
				}
				else if(Data_Judge("{\"Ble\":Fan-ON}",CV_UART.UARTX_Array[2],CV_UART.Rxd_Num[DEBUG_OUT]) > 0)		//	ON
				{
					Fan = 1;
				}
				else if(Data_Judge("{\"Ble\":Fan-OF}",CV_UART.UARTX_Array[2],CV_UART.Rxd_Num[DEBUG_OUT]) > 0)
				{
					Fan = 0;
				}
				else if(Data_Judge("{\"Ble\":CL-ON}",CV_UART.UARTX_Array[2],CV_UART.Rxd_Num[DEBUG_OUT]) > 0)		//	ON
				{
					CL = 1;
				}
				else if(Data_Judge("{\"Ble\":CL-OF}",CV_UART.UARTX_Array[2],CV_UART.Rxd_Num[DEBUG_OUT]) > 0)
				{
					CL = 0;
				}
				else
				{
					printf("Ble:error \r\n");
				}
				
			}
            Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
            
			CV_UART.Rxd_Num[DEBUG_OUT] = 0;
			CV_UART.Read_Flag[DEBUG_OUT] = 0;
        }
		
		Light = 3.30 - ADCx_Read_Vol (ADC_2);		// 光照度 
		if((Light < 0.56) && (BOOT == 0))
		{LED = 1;}
		sprintf(Free_Array, "Light:  %4.2f ", Light);
		LCD_ShowString(1,4,Free_Array,BLUE,BLACK,32);
		
		far = Distance();Human = (int)(far);
		if(Human < 16)
		{
			Voice(JQ8900,2);
			Delay_ms(600);
		}
		sprintf(Free_Array, "Human :   %d  ", Human);		// 人體 
		LCD_ShowString(1,5,Free_Array,BLUE,BLACK,32);
		
		if(Fan)
		{GPIO_ResetBits(GPIOB, GPIO_Pin_5);}
		else
		{GPIO_SetBits(GPIOB, GPIO_Pin_5);}
		sprintf(Free_Array, "Fan :     %d  ",   Fan);		// 風扇 
		LCD_ShowString(1,6,Free_Array,BLUE,BLACK,32);
		
		if(LED)
		{GPIO_SetBits(GPIOA, GPIO_Pin_6);}
		else
		{GPIO_ResetBits(GPIOA, GPIO_Pin_6);}	//GPIO_ResetBits(GPIOA, GPIO_Pin_6);
		if(CL)
		{
			Steering_Engine_Angle(1,90);
			Steering_Engine_Angle(2,90);
			Steering_Engine_Angle(3,90);
			Steering_Engine_Angle(4,90);
		} 
		else
		{
			Steering_Engine_Angle(1,0);
			Steering_Engine_Angle(2,0);
			Steering_Engine_Angle(3,0);
			Steering_Engine_Angle(4,0);
		}
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			Delay_ms(2);
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
			chance = 1;
			
			LED = !LED;
			printf("key %d \r\n",chance);
		}

		Delay_ms(2);
	}
}


/////////////////////////////			以上

void Mian_Init(void)
{
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}

	UART_Init(Debug, 9600);
	UART_Init(UART_3, 9600);
	Init_Steering_Engine_T4(); 
    LCD_Init();//LCD初始化
    LCD_ShowString(4,0,"AI House",GREEN,BLACK,32); 
	Ultrasonic_Init();
	Distance();
	ADCx_Init_Caven(ADC_2);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);GPIO_SetBits(GPIOB, GPIO_Pin_5);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	
	Steering_Engine_Angle(1,0);
	Steering_Engine_Angle(2,0);
	Steering_Engine_Angle(3,0);
	Steering_Engine_Angle(4,0);
	
	UART_Send_Data(Debug,"u2:hello world !",sizeof("hello world !"));
	UART_Send_Data(UART_3,"u3:hello world !",sizeof("hello world !"));
	Light = ADCx_Read_Vol (ADC_2);
	far = Distance();
	Delay_S(1);
	Voice_Init();
	Voice(JQ8900,1);		//9600
	
}

#endif
