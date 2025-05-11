 
#ifdef CAR

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Control_Mode.h"
#include "Steering_engine.h"
#include "lcd.h"
#include "pic.h"
#include "MP3.h"
#include "string.h"

#define CAR_MAX 20

#define CMD_Check	0X01		//
#define CMD_Read	0X02		//{0XA0,0X04,0X01,0X89,0X01,0XD1}
#define CMD_Stop	0X03		//{0xA0,0X03,0X01,0X8C,0XD0}
#define CMD_RST		0X04		//CMD_RST
#define CMD_END		0X05		

#define SPK_WLC		0X01
#define SPK_YES		0X02
#define SPK_NOT		0X03

void Mian_Init(void);
char Send_CMD (char CMD_ID);
void V831_Data(char *i);
void RFID_Data(char *i);
void Show_LCD(char *i);

char Free_Array[64];	//
char array_watch[64];	//
char RFID_succ[] = {0XA0,0X04,0X00,0X70,0X10,0XDC};
char RFID_temp[100];

struct RFID_Cfg
{
    char auth;
	char CMD; 
	char CMD_N; 
	char no_card;
    char Survive; 
};

struct CAR_Data
{
    char ID_Run;
    char ID_Num;                    //
	char step;
    char auth[CAR_MAX];             //
    char CAR_String[CAR_MAX][24];
    char EPC_ARRAY[CAR_MAX][80];
};

struct RFID_Cfg RFID_CAR;
struct CAR_Data CARS_DATA;

float vol;
float deviation;

int cloFlag,clo = 0,show_flag,show_flag2;
char chance = 0;
char mode;
char condition = 0;

int main(void)
{
	Mian_Init();
	CARS_DATA.ID_Num = 2;
	for(int i = 0;i <= 10;i++)
	{
		memcpy(CARS_DATA.CAR_String[i],"{A123}",sizeof("A123"));
	}
	RFID_CAR.CMD = 0;
	while(RFID_CAR.Survive == 1)												//main
	{
        //get a chance

		condition = CV_UART.Read_Flag[DEBUG_OUT];
		V831_Data(&condition);
		
		if((RFID_CAR.CMD_N != RFID_CAR.CMD))
		{ RFID_CAR.CMD_N = RFID_CAR.CMD; Send_CMD (RFID_CAR.CMD); show_flag = ~show_flag;}
		
		if(RFID_CAR.CMD == CMD_Read)		//读模式必须等到卡
		{
			do{
				Delay_ms(20);
				RFID_CAR.no_card++;
				if(RFID_CAR.no_card > 60)		//小于60就是读到
				{
					RFID_CAR.CMD = CMD_Stop;	//读不到
					break;
				}
			}while(CV_UART.Rxd_Num[3] == 0);
		}
		
		condition = (char) ((chance * CV_UART.Rxd_Num[3]) & 0x00FF);			//
		RFID_Data(&condition);
		
		if(CARS_DATA.step == 2)
		{
			if(mode)
			{

			}
			else		//default   failure
			{
				condition = memcmp(CARS_DATA.EPC_ARRAY[CARS_DATA.ID_Run],RFID_temp,RFID_temp[1] + 2);
				if(condition == 0)
				{
					Steering_Engine_Angle(1,90);
					Steering_Engine_Angle(2,90);
					Voice(JQ8900,SPK_YES);
				}
				else 
				{
					Steering_Engine_Angle(1,0);
					Steering_Engine_Angle(2,0);
					Voice(JQ8900,SPK_NOT);
				}
			}
			CARS_DATA.step = 0;
			memset(RFID_temp,0,sizeof(RFID_temp));
		}
		condition = 0;
		if(RFID_CAR.CMD == CMD_Read)
		{clo = RED;}
		else
		{clo = GREEN;}
		
		if(show_flag != show_flag2)
		{
			show_flag2 = show_flag;
			condition = 1;
		}
		Show_LCD(&condition);
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			Delay_ms(2);
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
			show_flag = ~show_flag;
			chance = 1;
			mode = !mode;
			// RFID_CAR.CMD = CMD_Read;
			printf("key %d \r\n",chance);
		}
		
	}
}

/////////////////////////////

void V831_Data(char *i)
{
	if(*i)
	{
		char temp_a = 1;
		int i = 0;
		printf("go \r\n");
		if(CV_UART.UARTX_Array[DEBUG_OUT][0] == '{') 
		{
	
			CARS_DATA.ID_Run = 0xdd;
			do
			{
				temp_a = memcmp(CV_UART.UARTX_Array[DEBUG_OUT],CARS_DATA.CAR_String[i],CV_UART.Rxd_Num[DEBUG_OUT]);
				if(temp_a == 0)				//相同的
				{
					CARS_DATA.ID_Run = i;
					break;
				}
			} while ((i++) < CARS_DATA.ID_Num);
			if(CARS_DATA.ID_Run <= CARS_DATA.ID_Num)		//存在
			{
				printf("same \r\n");
				
				//doing
			}
			else											//不存在
			{
				printf("add \r\n");
				show_flag = !show_flag;
				CARS_DATA.ID_Num++;
				CARS_DATA.ID_Run = CARS_DATA.ID_Num;
				memcpy(CARS_DATA.CAR_String[CARS_DATA.ID_Num],CV_UART.UARTX_Array[DEBUG_OUT],CV_UART.Rxd_Num[DEBUG_OUT]);		//添加
			}

			//接下来开始读RFID
			chance = 1;		//开启chance
			CV_UART.Read_Flag[3] = 0;
			RFID_CAR.CMD = CMD_Read;
			CARS_DATA.step = 1;
		}
		Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
		CV_UART.Read_Flag[2] = 0;
		CV_UART.Rxd_Num[2] = 0;
	}
}

void RFID_Data(char *i)
{
	if(*i)
	{
		printf("R go \r\n");
		int go_a0 = 0xFFFF;
		char temp[200];
		int num,n = 0;
		Delay_ms(80);
		CV_UART.Read_Flag[3] = 1;		//停止接收
		num = CV_UART.Rxd_Num[3];
		
		memcpy(temp,CV_UART.UARTX_Array[3],num);
		do{
			if(temp[n] == 0xA0 && temp[n + 1] < 0x40)
			{
				go_a0 = n;
				break;
			}
			n++;
		}while(num--);
		
		if(go_a0 != 0xFFFF)
		{
			// memcpy(CARS_DATA.EPC_ARRAY[CARS_DATA.ID_Num],&temp[go_a0],temp[go_a0 + 1] + 2);
			memcpy(RFID_temp,&temp[go_a0],temp[go_a0 + 1] + 2);
			UART_Send_Data(2,RFID_temp,RFID_temp[1] + 2);
		}
		else
		{
			printf("no a0 \r\n");
		}

		Data_Replace("123", CV_UART.UARTX_Array[3], 0, sizeof(CV_UART.UARTX_Array[3]));
		chance = 0;
		CV_UART.Rxd_Num[3] = 0;
		CV_UART.Read_Flag[3] = 1;
		RFID_CAR.CMD = CMD_Stop;		//
		CARS_DATA.step = 2;
	}
}

void Show_LCD(char *i)
{
	if(*i)
	{
	LCD_ShowString(4,0,"Car Stop ",clo,BLACK,32); 
	for(int i = 6;i > CARS_DATA.ID_Num;i--)
	{
		if(i)
		{
			LCD_ShowString(0,i,"        ",clo,BLACK,32); 
		}
	}
	for(int i = 0;i <= CARS_DATA.ID_Num;i++)
	{
		if(i)
		{
			LCD_ShowString(0,i,CARS_DATA.CAR_String[i],clo,BLACK,32); 
		}
	if(i > 6)
		break;
	}
	}
}

char Send_CMD (char Cfg)
{
	char retval = 0;
	char array[10][20] = {
		{0,0},								//empty
		{0xA0,0X03,0X01,0X72,0XEA},			//CMD_Check
		{0XA0,0X04,0X01,0X89,0X01,0XD1},	//CMD_Read
		{0xA0,0X03,0X01,0X8C,0XD0},			//CMD_Stop
		{0xA0,0X04,0X00,0X70,0X10,0XDC},	//rst
	};
	
	if(Cfg < CMD_END)
	{
		UART_Send_Data(3,array[Cfg],(array[Cfg][1] + 2));
		retval = 1;
	}
	return retval;
}

void Mian_Init(void)
{
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}
	
	UART_Init(DEBUG_OUT, 115200);	//v831
	UART_Init(3, 115200);			//RFID
	Init_Steering_Engine_T4();
    LCD_Init();//LCD
	
	LCD_ShowString(6,0,"Null ",GREEN,BLACK,32); 
	Steering_Engine_Angle(1,0);
	Steering_Engine_Angle(2,0);
	Steering_Engine_Angle(3,0);
	Steering_Engine_Angle(4,0);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //复用推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	UART_Send_String(DEBUG_OUT,"hello world !\r\n");
	Send_CMD(CMD_RST);
	Delay_ms(500);
	char try1 = 0;
	do{
		CV_UART.Read_Flag[3] = 0;
		Send_CMD (CMD_Check);
		try1 ++;
		Delay_ms(200);
		if(CV_UART.Rxd_Num[3])
		{
			Delay_ms(100);
			clo = CV_UART.UARTX_Array[3][0];
//			UART_Send_Data(2,CV_UART.UARTX_Array[3],10);
			CV_UART.Rxd_Num[3] = 0;
		}
		if(try1 > 10)
		{while(1);}
	}while(clo != 0xA0);
	
	CV_UART.Read_Flag[3] = 1;
	CV_UART.Rxd_Num[3] = 0;
	memset(CV_UART.UARTX_Array[3],0,sizeof(CV_UART.UARTX_Array[3]));
	LCD_ShowString(4,0,"Car Stop ",GREEN,BLACK,32); 
	
//	Voice_Init();
//	Voice(JQ8900,SPK_WLC);		//9600
	RFID_CAR.Survive = 1;
	CV_UART.Read_Flag[DEBUG_OUT] = 0;
}

#endif
