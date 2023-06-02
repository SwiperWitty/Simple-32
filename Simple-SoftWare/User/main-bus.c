// 欠姨50  
#ifdef BUS

#include "stm32f10x.h"
#include "sys_time.h"
#include "Data_Handle.h"
#include "Steering_engine.h"
#include "MODE_Motor_BYJ.h"
#include "lcd.h"
//#include "pic.h"
#include "uart.h"
#include "MP3.h"
#include "RFID_Read.h"

#define Debug UART_2



#define SPK_WLC		0X03        //欢迎
#define SPK_OPE		0X01        //开门
#define SPK_CLS		0X02

//关门

void Mian_Init(void);
char Send_CMD (char CMD_ID);
char V831_Data(void);
char RFID_Data(void);
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


struct RFID_Cfg RFID_BUS;

float vol;
float deviation;
char chance = 0;
char mode;
char condition = 0;

char Step = 0;

int main(void)
{
	Mian_Init();
	while(1)
	{
        //get a chance
        

        switch(Step)
        {
            case (1):
            {
				
				LCD_ShowString(0,6,"A ",YELLOW,BLACK,32);
				LCD_ShowString(3,4,"BUS Close  ",BLUE,BLACK,32);
                if(V831_Data() != 0)             //V831 发现存在 轮椅
				{
					LCD_ShowString(8,6,"chair",YELLOW,BLACK,32);
					Step = 2;
				}
				else if(RFID_Data() != 0)
				{
					LCD_ShowString(8,6,"stick",YELLOW,BLACK,32);
					Step = 2;
				}
                else
                {
                    Delay_S(1);
					LCD_ShowString(8,6,"     ",YELLOW,BLACK,32);
                }
                break;
            }
            case (2):
            {
				CV_UART.Read_Flag[Debug] = 1;
                Voice(JQ8900,SPK_OPE);          //欢迎
				LCD_ShowString(3,4,"BUS Open   ",BLUE,BLACK,32);
                Steering_Engine_Angle(1,90);
                Steering_Engine_Angle(2,90);
                Steering_Engine_Angle(3,90);
                Steering_Engine_Angle(4,90);
                Motor_BYJ_Drive(1,1,1);
                Step = 3;                               //进行第三步
                Delay_ms(200);
				LCD_ShowString(0,6,"A ",GREEN,BLACK,32);
                break;
            }
            case (3):
            {
                if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
                {
                    Delay_ms(2);
                    while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
					printf("key \r\n");
					LCD_ShowString(0,6,"A ",RED,BLACK,32);
					LCD_ShowString(3,4,"BUS Close  ",BLUE,BLACK,32);
                    Voice(JQ8900,SPK_CLS);      //关门
                    Steering_Engine_Angle(1,0);
                    Steering_Engine_Angle(2,0);
                    Steering_Engine_Angle(3,0);
                    Steering_Engine_Angle(4,0);
                    Motor_BYJ_Drive(0,1,1);
                    Step = 1;                           //重开
                    CV_UART.Read_Flag[Debug] = 0;
                    Delay_ms(100);
                }
                else
                {
                    Delay_ms(2);
                }
                break;
            }
            default:
            {
                Step = 0;
                Delay_ms(2);
                break;
            }
                
        }
//        printf("Step %d \r\n",Step);
	}
}

char V831_Data(void)
{
    char Retval = 0;
	printf("RUN V831 \r\n");
    if(CV_UART.Read_Flag[Debug])
    {
        chance = 0;
        if(CV_UART.UARTX_Array[Debug][0] == '{')
        {
            Retval = 1;
            if(Data_Judge("{V831:wheel }",CV_UART.UARTX_Array[2],sizeof("{V831:wheel }")) > 0)
            {
                Retval = 2;
            }
            else if(Data_Judge("{V831:",CV_UART.UARTX_Array[Debug],sizeof("{V831:")) > 0)
            {
				//e
            }
            
        }
        Data_Replace("123", CV_UART.UARTX_Array[Debug], 0, sizeof(CV_UART.UARTX_Array[Debug]));
		CV_UART.Rxd_Num[Debug] = 0;
        CV_UART.Read_Flag[Debug] = 0;
    }
    return Retval;
}

char RFID_Data(void)
{
    char Retval = 0;
    U8 array[64];
    if(RFID_Read_Card(array) > 0)   //存在RFID
    {
        printf("Find RFID !!! \r\n");
		Retval = 1;
    }
	
	printf("RUN RFID \r\n");
    return Retval;
}

void Show_LCD(char *i)
{
	if(*i)
	{
        
	}
}


/////////////////////////////

void Mian_Init(void)
{
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
	Steering_Engine_Angle(4,0);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //复用推
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	UART_Send_String(DEBUG_OUT,"hello world !\r\n");

	if (RFID_Init() == 0)
    {
        while(1);
    }
    RFID_BUS.Survive = 1;
    printf("Start up syslck %d \r\n",SystemCoreClock);
	LCD_ShowString(5,0,"AI BUS ",GREEN,BLACK,32); 
	
	Motor_BYJ_Init(1);
//	Motor_BYJ_Drive(1,1,1);
	Voice_Init();
	Voice(JQ8900,SPK_WLC);		//9600
	
	CV_UART.Read_Flag[DEBUG_OUT] = 0;   //开始接收 uart_2
    Step = 1;
}

#endif
