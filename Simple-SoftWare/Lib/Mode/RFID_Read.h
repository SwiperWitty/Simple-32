#ifndef _RFID_READ_H__
#define _RFID_READ_H__

#include "uart.h"
#include "sys_time.h"
#include "Data_Handle.h"

/*
    使用串口3 115200
*/

#define CMD_Check	0X01		//
#define CMD_Read	0X02		//{0XA0,0X04,0X01,0X89,0X01,0XD1}
#define CMD_Stop	0X03		//{0xA0,0X03,0X01,0X8C,0XD0}
#define CMD_RST		0X04		//CMD_RST
#define CMD_END		0X05		

char RFID_Init (void);
char RFID_Send_CMD (char Cfg);
char RFID_Read_Card(unsigned char *Target);

#endif
