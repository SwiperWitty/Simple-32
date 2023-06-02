#include "RFID_Read.h"

unsigned char RFID_BUFF[20][64];

char RFID_Init (void)
{
    char Retval = 0;
    UART_Init(UART_3, 115200);			//RFID
    
    RFID_Send_CMD(CMD_RST);
	Delay_ms(500);
	char try0 = 0;
    unsigned char temp = 0;
	do{
		CV_UART.Read_Flag[UART_3] = 0;
		RFID_Send_CMD (CMD_Check);
		try0 ++;
		Delay_ms(200);
		if(CV_UART.Rxd_Num[UART_3])
		{
			Delay_ms(100);
			temp = CV_UART.UARTX_Array[UART_3][0];
//			UART_Send_Data(2,CV_UART.UARTX_Array[3],10);
			CV_UART.Rxd_Num[UART_3] = 0;
		}
		if(try0 > 10)
		{
            Retval = 0;
            return Retval;
        }
	}while(temp != 0xA0);
    
    CV_UART.Read_Flag[UART_3] = 1;
	CV_UART.Rxd_Num[UART_3] = 0;
	memset(CV_UART.UARTX_Array[UART_3],0,sizeof(CV_UART.UARTX_Array[UART_3]));
    
    Retval = 1;
    return Retval;
}

char RFID_Send_CMD (char Cfg)
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
		UART_Send_Data(UART_3,array[Cfg],(array[Cfg][1] + 2));
		retval = 1;
	}
	return retval;
}

char RFID_Read_Card(unsigned char *Target)      //返回收到的第一个RFID标签
{
    char Retval = 0;
    unsigned char temp[200];
    int i,num,run_num,n = 0;
    CV_UART.Rxd_Num[3] = 0;
    CV_UART.Read_Flag[3] = 0;   //允许接收
    
    if(Target == NULL)
    {
        return Retval;
    }
    memset(temp,0,sizeof(temp));
    RFID_Send_CMD (CMD_Read);
    do{
        Delay_ms(50);
        n++;
        if(n > 10) 
        {
            RFID_Send_CMD (CMD_Stop);
            CV_UART.Read_Flag[3] = 1;		//停止接收
            CV_UART.Rxd_Num[3] = 0;
            CV_UART.Read_Flag[3] = 1;
            return Retval; 
        } 
    }while(CV_UART.Rxd_Num[3] == 0); 

    CV_UART.Read_Flag[3] = 1;		//停止接收
    num = CV_UART.Rxd_Num[3];
    
    memcpy(temp,CV_UART.UARTX_Array[3],num);
    n = 0;
    run_num = 0;
    i = num;
    do{
        if(temp[n] == 0xA0 && temp[n + 1] < 0x40)   //找首
        {
            run_num = n;
            break;
        }
        n++;
    }while(i--);

    if(run_num != num)
    {
        Retval = temp[run_num + 1] + 2;     //RFID长度
        memcpy(Target,&temp[run_num],Retval);
//      UART_Send_Data(2,Target,Retval);
    }
    else
    {
        UART_Send_Data(2,"no a0 \r\n",sizeof("no a0 \r\n"));
    }
    Data_Replace("123", CV_UART.UARTX_Array[3], 0, sizeof(CV_UART.UARTX_Array[3])); 

    RFID_Send_CMD (CMD_Stop); 
    CV_UART.Rxd_Num[3] = 0;
    CV_UART.Read_Flag[3] = 1;
    
	return Retval;
}
