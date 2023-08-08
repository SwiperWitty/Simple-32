 
#ifdef CAR2

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
void V831_Data(void);
void RFID_Data(void);
void Show_LCD(char *i);

char Free_Array[64];	//
char array_watch[64];	//
char RFID_succ[] = {0XA0,0X04,0X00,0X70,0X10,0XDC};
char RFID_default[] = {0XA0,0X19,0X00,0X89,0X01,0X34,0X00,0XE2,0X80,0X11,0X70,0X00,0X00,0X02,0X10,0X6E,0XF2,0X02,0X13,0X00};

struct RFID_Cfg
{
    char auth;
	char CMD; 
	char CMD_N; 
	char no_card;
    char Survive; 
	char Num; 
	char Temp[100];
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
char car_ck[2][20];
char car[10][10];
char car_num = 0;
int main(void)
{
	Mian_Init();
	CARS_DATA.ID_Num = 2;

	memcpy(CARS_DATA.CAR_String[1],"{8P8F8}",sizeof("{8P8F8}"));
	memcpy(CARS_DATA.CAR_String[2],"{05EV8}",sizeof("{05EV8}"));
	
	memcpy(CARS_DATA.EPC_ARRAY[1],RFID_default,sizeof(RFID_default));
	memcpy(CARS_DATA.EPC_ARRAY[2],RFID_default,sizeof(RFID_default));

	CARS_DATA.step = 1;
	while(RFID_CAR.Survive == 1)												//main
	{
        //get a chance
		V831_Data();
		RFID_Data();
		if(CARS_DATA.step >= 3)
		{
			if(mode)
			{
				memcpy(CARS_DATA.EPC_ARRAY[CARS_DATA.ID_Run],RFID_CAR.Temp,RFID_CAR.Num);		//add mode default
				mode = 0;
			}
			
			if(mode == 0)							//default
			{
				if(CARS_DATA.step == 4)
				{
					printf("mode 4 \r\n");
					Voice(JQ8900,SPK_NOT);
					Steering_Engine_Angle(1,00); 
					Steering_Engine_Angle(2,00); 
				}
				else
				{
					if(memcmp(CARS_DATA.EPC_ARRAY[CARS_DATA.ID_Run],RFID_CAR.Temp,19) == 0 && (CARS_DATA.EPC_ARRAY[CARS_DATA.ID_Run][0] == 0xA0))
					{
						Voice(JQ8900,SPK_YES); 
						Steering_Engine_Angle(1,90); 
						Steering_Engine_Angle(2,90); 
						do{
							Delay_ms(50);
							CARS_DATA.step = 2;
							RFID_Data();
						}while(RFID_CAR.Temp[0] != 0);
						Steering_Engine_Angle(1,0); 
						Steering_Engine_Angle(2,0); 
						memset(car_ck[1],0,10);
					}
					else if(CARS_DATA.EPC_ARRAY[CARS_DATA.ID_Run][7] == 0xAA)
					{
						Voice(JQ8900,SPK_YES); 
						Steering_Engine_Angle(1,90); 
						Steering_Engine_Angle(2,90); 
						do{
							Delay_ms(50);
							CARS_DATA.step = 2;
							RFID_Data();
						}while(RFID_CAR.Temp[0] != 0);
						Steering_Engine_Angle(1,0); 
						Steering_Engine_Angle(2,0); 
						memset(car_ck[1],0,10);
					}
					else
					{
						printf("mode 3 \r\n");
						Voice(JQ8900,SPK_NOT); 
						Steering_Engine_Angle(1,00); 
						Steering_Engine_Angle(2,00); 
					}

				}
			}
			memset(RFID_CAR.Temp,0,sizeof(RFID_CAR.Temp));
			CARS_DATA.step = 1;
		}
		
		if(mode)
		{clo = GREEN;}
		else
		{clo = BLUE;}
		if(cloFlag != clo)
		{
			cloFlag = clo;
			condition = 1;
		}
		Show_LCD(&condition);
		
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			Delay_ms(2);
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);

			mode = 1;
			// RFID_CAR.CMD = CMD_Read;
			printf("key \r\n");
		}
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0)		// 從
		{
			Delay_ms(2);
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0)
			{
				Steering_Engine_Angle(3,90);
			}
			while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0);
			Steering_Engine_Angle(3,0);
		}
	}
}

/////////////////////////////

void V831_Data(void)
{
	if(CARS_DATA.step == 1 && CV_UART.Read_Flag[2] == 1)
	{
		char temp_a = 1;
		int i = 0;
		
		if(CV_UART.UARTX_Array[DEBUG_OUT][0] == '{') 
		{
			CARS_DATA.ID_Run = 0xdd;
			Delay_ms(80);
			if (car_num < 6)
			{
				memcpy(car[car_num],CV_UART.UARTX_Array[DEBUG_OUT],CV_UART.Rxd_Num[DEBUG_OUT]); 
				car_num++;
					Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
					CV_UART.Read_Flag[2] = 0;
					CV_UART.Rxd_Num[2] = 0;
				return;
			}
			else
			{
				car_num = 0;
				for(int p = 0;p < 6;p++)
				{
					if(memcmp(car[0],car[p],6) != 0)
					{
						memset(car,0,sizeof(car));
							Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
							CV_UART.Read_Flag[2] = 0;
							CV_UART.Rxd_Num[2] = 0;
						return;		//车牌不稳定
					}
				}		//车牌稳定
				memcpy(car_ck[0],CV_UART.UARTX_Array[DEBUG_OUT],CV_UART.Rxd_Num[DEBUG_OUT]);
				if(memcmp(car_ck[0],car_ck[1],6) == 0)		//same 
				{
					Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
					CV_UART.Read_Flag[2] = 0;
					CV_UART.Rxd_Num[2] = 0;
					CARS_DATA.ID_Run = CARS_DATA.ID_Num;
					return;
				}
				else
				{
					memcpy(car_ck[1],car_ck[0],10);
				}
			}

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
				CARS_DATA.ID_Run = i;
				//doing
			}
			else if(mode)					//不存在
			{
				printf("add \r\n");
				show_flag = !show_flag;
				CARS_DATA.ID_Num++;
				CARS_DATA.ID_Run = CARS_DATA.ID_Num;
				memcpy(CARS_DATA.CAR_String[CARS_DATA.ID_Num],CV_UART.UARTX_Array[DEBUG_OUT],CV_UART.Rxd_Num[DEBUG_OUT]);		//添加车牌
				condition = 1;
			}
			else							//识别模式，但是没有录入 (不允许) 
			{
				Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
				CV_UART.Read_Flag[2] = 0;
				CV_UART.Rxd_Num[2] = 0;

				CARS_DATA.step = 4;
				return; 
			}
			printf(" run %d \r\n",CARS_DATA.ID_Run);

			//接下来开始读RFID
			chance = 1;		//开启chance
			CV_UART.Read_Flag[3] = 0;
			CARS_DATA.step = 2;
		}
		Data_Replace("123", CV_UART.UARTX_Array[DEBUG_OUT], 0, sizeof(CV_UART.UARTX_Array[DEBUG_OUT]));
		CV_UART.Read_Flag[2] = 0;
		CV_UART.Rxd_Num[2] = 0;
	}
}

void RFID_Data(void)
{
	if(CARS_DATA.step == 2)
	{
		CARS_DATA.step = 3;
		int go_a0 = 0xFFFF;
		char temp[200];
		int num,n = 0;
		CV_UART.Rxd_Num[3] = 0;
		CV_UART.Read_Flag[3] = 0;
		memset(RFID_CAR.Temp,0,sizeof(RFID_CAR.Temp));
		Send_CMD (CMD_Read);
		do{
			Delay_ms(50);
			n++;
			if(n > 10) 
			{ 
				CARS_DATA.step = 4;
				CV_UART.Read_Flag[3] = 1;
				Send_CMD (CMD_Stop);
				return; 
			} 
		}while(CV_UART.Rxd_Num[3] == 0); 
		
		CV_UART.Read_Flag[3] = 1;		//停止接收
		num = CV_UART.Rxd_Num[3];
		
		memcpy(temp,CV_UART.UARTX_Array[3],num);
		n = 0;
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
			RFID_CAR.Num = temp[go_a0 + 1] + 2;
			memcpy(RFID_CAR.Temp,&temp[go_a0],RFID_CAR.Num);
//			UART_Send_Data(2,RFID_CAR.Temp,RFID_CAR.Num);
		}
		else
		{
			printf("no a0 \r\n");
		}
		Data_Replace("123", CV_UART.UARTX_Array[3], 0, sizeof(CV_UART.UARTX_Array[3])); 
		
		CV_UART.Rxd_Num[3] = 0;
		CV_UART.Read_Flag[3] = 1;
		Send_CMD (CMD_Stop); 
		
	}
}

void Show_LCD(char *i)
{
	if(*i)
	{
		*i = 0;
		LCD_ShowString(4,0,"Car Stop ",clo,BLACK,32); 
		for(int i = 6;i > CARS_DATA.ID_Num;i--)
		{
			if(i)
			{
				LCD_ShowString(0,i,"            ",clo,BLACK,32); 
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
	GPIO_InitTypeDef GPIO_InitStructure;
	if(SysTick_Config(SystemCoreClock/100000))
	{
		while(1);
	}
	
	UART_Init(DEBUG_OUT, 9600);		//v831
	UART_Init(3, 115200);			//RFID
	Init_Steering_Engine_T4();
    LCD_Init();//LCD
	
	LCD_ShowString(6,0,"Null ",GREEN,BLACK,32); 
	Steering_Engine_Angle(1,0); 
	Steering_Engine_Angle(2,0); 
	Steering_Engine_Angle(3,0); 
//	Steering_Engine_Angle(4,0);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //复用推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	UART_Send_String(DEBUG_OUT,"hello world !\r\n");
	Send_CMD(CMD_RST);
	Delay_ms(400);
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
	
	Voice_Init();
	Voice(JQ8900,SPK_WLC);		//9600
	RFID_CAR.Survive = 1;
	CV_UART.Read_Flag[DEBUG_OUT] = 0;
}

#endif
