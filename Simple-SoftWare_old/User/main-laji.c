 
#ifdef LAJI

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Steering_engine.h"
#include "lcd.h"
#include "pic.h"
#include "SU_03T.h"

void Mian_Init(void);
void Show_LCD(char *i);
void V831_Data(char *data);
void SU_03T_Data (char *data);

char Free_Array[64];
char array_watch[64];
char LCD_String[10][64];
char late_data[64];
char now_data[64];

char laji_num[5];	//[1]Recyclable、[2]Hazardous、[3]Other、[4]Kitchen

float vol;
int clo = 0;
char chance = 1;
int temp = 0;

int main(void)
{
	Mian_Init();
	printf("go \r\n");
	while(1)
	{
        //get a chance

        if(CV_UART.Read_Flag[DEBUG_OUT] == 1)																//2.收到视觉模块的数据
        {
//			printf("k210: s\r\n");
			
			if(Data_Filtration(CV_UART.UARTX_Array[DEBUG_OUT],8,4) == 1)	//第一次数字滤波
			{
				temp = CV_UART.Rxd_Num[DEBUG_OUT];
				memcpy(now_data,CV_UART.UARTX_Array[DEBUG_OUT],temp);
				if(memcmp(now_data,late_data,temp) != 0)					//第二次数字去重
				{
					memcpy(late_data,now_data,temp);
					V831_Data(now_data);
				}
			}
            Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
            
			CV_UART.Rxd_Num[DEBUG_OUT] = 0;
			CV_UART.Read_Flag[DEBUG_OUT] = 0;

//			Steering_Engine_Angle(tag,0);		//关闭
        }
		else if(CV_UART.Read_Flag[3] == 1)
		{
			SU_03T_Data (CV_UART.UARTX_Array[3]);
			
            Data_Replace("123", CV_UART.UARTX_Array[3], 0, sizeof(CV_UART.UARTX_Array[3]));
			CV_UART.Rxd_Num[3] = 0;
			CV_UART.Read_Flag[3] = 0;
		}
		else
		{
			Show_LCD(&chance);
			Delay_ms(1);
		}
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			Delay_ms(2);
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
			memset(laji_num,0,sizeof(laji_num));

			Delay_ms(1000);
		}
	}
}

void SU_03T_Data (char *data)
{
	char tag;
	if(data[0] == '{')
	{
		if(Data_Judge("{kai1}",data,sizeof("{kai1}")) > 0)			// 可回收
		{
			tag = 1;
			Steering_Engine_Angle(2,0);Steering_Engine_Angle(3,0);Steering_Engine_Angle(4,0);
		}
		else if(Data_Judge("{kai2}",data,sizeof("{kai2}")) > 0)		//危险
		{
			tag = 2;
			Steering_Engine_Angle(1,0);Steering_Engine_Angle(3,0);Steering_Engine_Angle(4,0);
		}
		else if(Data_Judge("{kai3}",data,sizeof("{kai3}")) > 0)				//厨余垃圾
		{
			tag = 4;
			Steering_Engine_Angle(1,0);Steering_Engine_Angle(2,0);Steering_Engine_Angle(3,0);
		}
		else if(Data_Judge("{kai4}",data,sizeof("{kai4}")) > 0)			//其他垃圾
		{
			tag = 3;
			Steering_Engine_Angle(1,0);Steering_Engine_Angle(2,0);Steering_Engine_Angle(4,0);
		}
		else
		{
			tag = 0;
			Steering_Engine_Angle(1,0);
			Steering_Engine_Angle(2,0);
			Steering_Engine_Angle(3,0);
			Steering_Engine_Angle(4,0);
		}
		if(laji_num[tag] > 5)
		{
			SU_O3T_Voice(5);
			Delay_ms(500);
		}
		else
		{
			if(tag > 0)
			{
//				SU_O3T_Voice(tag);
				Steering_Engine_Angle(tag,60);
				laji_num[tag] ++;
				Delay_ms(2500);
			}
			Steering_Engine_Angle(tag,0);
		}
	}
}

void V831_Data(char *data)
{
	char tag = 0;
	if(data[0] == '{')
	{
		if(Data_Judge("{K210:Recyclable}",data,sizeof("{K210:Recyclable}")) > 0)			// 可回收
		{
			tag = 1;
			Steering_Engine_Angle(2,0);Steering_Engine_Angle(3,0);Steering_Engine_Angle(4,0);
		}
		else if(Data_Judge("{K210:Hazardous}",data,sizeof("{K210:Hazardous}")) > 0)		//危险
		{
			tag = 2;
			Steering_Engine_Angle(1,0);Steering_Engine_Angle(3,0);Steering_Engine_Angle(4,0);
		}
		else if(Data_Judge("{K210:Other}",data,sizeof("{K210:Other}")) > 0)				//其他垃圾
		{
			tag = 3;
			Steering_Engine_Angle(1,0);Steering_Engine_Angle(2,0);Steering_Engine_Angle(4,0);
		}
		else if(Data_Judge("{K210:Kitchen}",data,sizeof("{K210:Kitchen}")) > 0)			//厨余垃圾
		{
			tag = 4;
			Steering_Engine_Angle(1,0);Steering_Engine_Angle(2,0);Steering_Engine_Angle(3,0);
		}
		else
		{
			tag = 0;
			Steering_Engine_Angle(1,0);
			Steering_Engine_Angle(2,0);
			Steering_Engine_Angle(3,0);
			Steering_Engine_Angle(4,0);
		}
		if(laji_num[tag] > 5)
		{
			SU_O3T_Voice(5);
			Delay_ms(500);
		}
		else
		{
			if(tag > 0)
			{
				SU_O3T_Voice(tag);
				Steering_Engine_Angle(tag,60);
				laji_num[tag] ++;
				Delay_ms(1000);
			}
//					Delay_ms(1500);
		}
	}
}

void Show_LCD(char *i)
{
	char k = 0;
	float t;
	if(*i)
	{
//		*i = 0;
		k = 1;t = laji_num[k];t = (t / 6) * 100;
		sprintf(LCD_String[k],"Recycled: %3d%% ",(int)t);
		k = 2;t = laji_num[k];t = (t / 6) * 100;
		sprintf(LCD_String[k],"Harmful : %3d%% ",(int)t);
		k = 3;t = laji_num[k];t = (t / 6) * 100;
		sprintf(LCD_String[k],"Other   : %3d%% ",(int)t);
		k = 4;t = laji_num[k];t = (t / 6) * 100;
		sprintf(LCD_String[k],"Kitchen : %3d%% ",(int)t);
		for(k = 1;k <= 4;k++)
		{
			LCD_ShowString(0,k,LCD_String[k],BLUE,BLACK,32); 
		}
	}
}

/////////////////////////////			以上

void Mian_Init(void)
{
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}
	Delay_ms(50);
	
	UART_Init(DEBUG_OUT, 9600);
	UART_Init(3,9600);			//SU-03T
	LCD_Init();//LCD初始
	Init_Steering_Engine_T4();
    SU_O3T_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //复用推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	Steering_Engine_Angle(1,0);
	Steering_Engine_Angle(2,0);
	Steering_Engine_Angle(3,0);
	Steering_Engine_Angle(4,0);
	LCD_ShowString(3,0,"Trash can ",GREEN,BLACK,32); 
	Delay_ms(20);
	printf("hello world !\r\n ");

	Delay_S(1);

}

#endif
