#ifndef _UART_H_
#define _UART_H_

#include "stm32f10x_rcc.h"

#include "stdio.h"
#include "string.h"

#define UART_1 1
#define UART_2 2
#define UART_3 3

#define DEBUG_OUT UART_1

typedef struct 
{
	char UARTX_Array[6][128];			
	__IO char Read_Flag[6];					
	__IO int Rxd_Num[6];					
} CV_UART_TypeDef;


extern CV_UART_TypeDef CV_UART; 


void UART_Init (char Channel,uint32_t Baud);
void UART_Send_String(char Channel,char *String);				
void UART_Send_Data(char Channel,char *Data,u8 Length);			

#endif
