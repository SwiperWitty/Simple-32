#include "SU_03T.h"



void SU_O3T_Init(void)
{
	UART_Init (UART_3,SU_O3T_Baud);
}

void SU_O3T_Voice(char Num)
{
	char Array_mp3[8];
	Array_mp3[0] = 0xAA;
	Array_mp3[1] = 0x55;
	Array_mp3[2] = Num;
	Array_mp3[3] = 0X01;
	Array_mp3[4] = 0x55;
	Array_mp3[5] = 0xAA;
	if(Num == 5)
	{
		Array_mp3[3] = Num;
	}
	
	UART_Send_Data(UART_3,Array_mp3,6);
}
