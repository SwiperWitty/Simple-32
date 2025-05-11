 
#ifdef EYE	// if define 

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Control_Mode.h"
#include "Steering_engine.h"
#include "lcd.h"
//#include "pic.h"
#include "uart.h"
#include "BZZ.h"
#include "MODE_Motor_BYJ.h"

#define X_max (36)		//x
#define X_min (-36)
#define Y_max (20)		//y
#define Y_min (-10)

void Mian_Init(void);
char Drive_PID(float x,float y,float *Space_x,float *Space_y);

char Free_Array[64];	//
char array_watch[64];	//

float vol;
float Space_x = 0 ,Space_y = 10;
int deviation;
float Temp_F[6];
int clo = 0;
int Temp_t;
int chance = 1;
int OK_time = 0;

char flag;


int main(void)
{
	Mian_Init();
	while(1)
	{
        //get a chance
        if(CV_UART.Read_Flag[DEBUG_OUT] == 1 && chance != 0)
        {
//            chance = 0;
			
            if(CV_UART.UARTX_Array[DEBUG_OUT][0] == '{') 
            {
				String_Chang_NUMs (CV_UART.UARTX_Array[DEBUG_OUT],Temp_F,"==:",2);  //Temp_F
				flag = Drive_PID(Temp_F[0],Temp_F[1],&Space_x,&Space_y);
				
				if(Space_x > X_max)
				{Space_x = X_max; }
				else if(Space_x < X_min)
				{Space_x = X_min; }
				if(Space_y > Y_max)
				{Space_y = Y_max; }
				else if(Space_y < Y_min)
				{Space_y = Y_min; }
//				printf("V831- x : %f ,x : %f \r\n",Temp_F[0],Temp_F[1]); 
				printf(" Space_x = %f ,Space_y = %f \r\n",Space_x,Space_y); 
			}
			if(flag == 0)
			{
				Steering_Engine_Angle(2,10);
				Steering_Engine_Angle(4,-Space_x);
				Delay_ms(50);
				OK_time = 0;
			}
			else if(flag == 1)
			{
				OK_time++;
			}
			if(OK_time >= 20)
			{
				OK_time = 0;
				Control_Mode_OUT(1,OUT_ON);
				Delay_ms(1000);
				Control_Mode_OUT(1,OUT_OF);
				
				Delay_S(2);
				Steering_Engine_Angle(2,-6);
				Steering_Engine_Angle(4,0);		//归位
				Delay_S(2);
			}
			CV_UART.Rxd_Num[DEBUG_OUT] = 0;
			CV_UART.Read_Flag[DEBUG_OUT] = 0;
			
        }
		else
		{
			
		}
	}
}

char Drive_PID(float x,float y,float *Space_x,float *Space_y)
{
	char Retval = -1;
	int Temp_t = 0;
	float deviation;
	char Finish[2];
	if(x > 112 || x < -112)		//x max
	{
		return Retval;
	}
	if(y > 112 || y< -112)		//y max
	{
		return Retval;
	}
	Retval = 0;
	deviation = x;
	Temp_t = deviation;
	Temp_t = abs(Temp_t);
	if(Temp_t > 50)
	{
		*Space_x -= (deviation * 0.05);
		Finish[0] = 0;
	}
	else if(Temp_t > 5)
	{
		*Space_x -= (deviation * 0.03);
		Finish[0] = 0;
	}
	else
	{Finish[0] = 1;}
	
	deviation = y;
	Temp_t = deviation;
	Temp_t = abs(Temp_t);
	if(Temp_t > 30)
	{
		*Space_y -= (deviation * 0.03);
		Finish[1] = 0;
	}
	else if(Temp_t > 5)
	{
		*Space_y -= (deviation * 0.01);
		Finish[1] = 0;
	}
	else
	{Finish[1] = 1;}
	
	if(Finish[0] == 1 && Finish[1] == 1)
		Retval = 1;
	return Retval;
} 

/////////////////////////////

void Mian_Init(void)
{
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}
	
	UART_Init(DEBUG_OUT, 115200);
	Control_Mode_Init ();
	Init_Steering_Engine_T4();
	
//    LCD_Init();//LCD
//	LCD_ShowString(4,0,"AI aide",GREEN,BLACK,32); 
	Steering_Engine_Angle(1,0);			//empty
	Steering_Engine_Angle(2,10);		// y
	Steering_Engine_Angle(3,-20);		//hou
	Steering_Engine_Angle(4,0);			//x
	
	Control_Mode_OUT(1,OUT_OF);
	
	printf("Hi 姨姨 %f\r\n",22.0/7);
	
	Timewatch.sys_time = Free_Array[0];
	Timewatch.hour = (Timewatch.sys_time / 3600) % 24;
	Timewatch.minutes = (Timewatch.sys_time / 60) % 60;
	Timewatch.second = Timewatch.sys_time % 60;
	Delay_S(1);
	printf("UARTX_Array size %d \r\n",sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));

}

#endif
