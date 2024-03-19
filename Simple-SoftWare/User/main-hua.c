 
#ifdef TASK_HUA

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Steering_engine.h"
#include "lcd.h"
#include "OLED.h"
#include "pic.h"
#include "MP3.h"
#include "adc.h"

#define Debug DEBUG_OUT
void Mian_Init(void);

char Free_Array[64];
char array_watch[64];

float light_val,PM25_val,hum_val,temp_val,f_temp;
int set_humid_min = 5,set_humid_max = 10;
int clo = 0;
int chance = 0;
int temp_run = 0;
int main(void)
{
	Mian_Init();
	while(1)
	{
        //get a chance
		if(CV_UART.Read_Flag[DEBUG_OUT] == 1)
		{
            if(CV_UART.UARTX_Array[DEBUG_OUT][0] == '{') 
            {
				printf("get num : %d \n",CV_UART.Rxd_Num[DEBUG_OUT]);
				if(CV_UART.Rxd_Num[DEBUG_OUT] > 20)
				{
					CV_UART.Rxd_Num[DEBUG_OUT] = 20;
				}
				UART_Send_Data(Debug,CV_UART.UARTX_Array[DEBUG_OUT],CV_UART.Rxd_Num[DEBUG_OUT]);
				
			}
			Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
			CV_UART.Rxd_Num[DEBUG_OUT] = 0;
			CV_UART.Read_Flag[DEBUG_OUT] = 0;
		}
		temp_val = Read_MCU_Temp ();
		f_temp = 3.3 - ADCx_Read_Vol (ADC_1);
		light_val = (f_temp / 3.3) * 100;
        f_temp = 3.3 - ADCx_Read_Vol (ADC_2);
		hum_val = (f_temp / 3.3) * 100;
		PM25_val = 0;

        temp_run = 2;
        sprintf(Free_Array,">humid:%5.2f ",hum_val);LCD_ShowString(0,temp_run++,Free_Array,BLACK,BACK_COLOR,32);
        sprintf(Free_Array,">temp :%5.2f ",temp_val);LCD_ShowString(0,temp_run++,Free_Array,BLACK,BACK_COLOR,32);
        sprintf(Free_Array,">light:%5.2f ",light_val);LCD_ShowString(0,temp_run++,Free_Array,BLACK,BACK_COLOR,32);
        
        sprintf(Free_Array,">hum min:%d ",set_humid_min);LCD_ShowString(0,temp_run++,Free_Array,BLACK,BACK_COLOR,32);
        sprintf(Free_Array,">hum max:%d ",set_humid_max);LCD_ShowString(0,temp_run++,Free_Array,BLACK,BACK_COLOR,32);
        
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			Delay_ms(2);
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
			chance = !chance;
			printf("key : %d \r\n",chance);
			
		}
		if(light_val > 80)
		{
			chance = 1;
		}
		if(chance)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_11);
		}
		else
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_11);
		}
		

		Delay_ms(20);
	}
}


/////////////////////////////

void Mian_Init(void)
{
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//AFIO复用功能模块时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//禁用JTAG
	
	UART_Init(Debug, 115200);
    UART_Init(UART_3, 115200);
    ADCx_Init_Caven(ADC_1);
    ADCx_Init_Caven(ADC_2);
	ADCx_Init_Caven(MCU_Temp);
	Init_Steering_Engine_T4();
    LCD_Init();//LCD
//	OLED_Init (ENABLE);
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //复用推
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    LCD_ShowString(1,0,"Flower nurse",BLUE,BACK_COLOR,32);
	
	Steering_Engine_Angle(1,0);
	Steering_Engine_Angle(2,0);
	Steering_Engine_Angle(3,0);
	Steering_Engine_Angle(4,0);
	
	UART_Send_Data(Debug,"hello world !\n",sizeof("hello world !\n"));
	printf("sysclk frequency :%d \r\n",SystemCoreClock);
	Timewatch.sys_time = Free_Array[0];
	Timewatch.hour = (Timewatch.sys_time / 3600) % 24;
	Timewatch.minutes = (Timewatch.sys_time / 60) % 60;
	Timewatch.second = Timewatch.sys_time % 60;
	Delay_S(1);
}

#endif
