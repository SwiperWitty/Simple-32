#ifndef _IIC__H_
#define _IIC__H_

#include "stm32f10x.h"

/****************/

/*
 * 标准软件模拟IIC
 * MAX  36kHZ(8bit+ACK)
 * MAX  33kHZ(8bit+WaitASK-YES)
 * MAX  19kHZ(8bit+WaitASK-NO)
 * MAX-Pro  142kHZ(8bit+ACK)
 */
#define Exist_IIC
//配置
#ifdef Exist_IIC
#define IIC_Base_Speed  12
#define IIC_Mode_IN     GPIO_Mode_IPU
#define IIC_Mode_OUT    GPIO_Mode_Out_PP


//IIC_GPIO

#define IIC_SCL        GPIO_Pin_10
#define IIC_SDA        GPIO_Pin_11
#define GPIO_IIC       GPIOB

#define IIC_SDA_H() GPIO_IIC->BSRR = IIC_SDA         //置高电平
#define IIC_SDA_L() GPIO_IIC->BRR = IIC_SDA         //置低电平
#define IIC_SCL_H() GPIO_IIC->BSRR = IIC_SCL
#define IIC_SCL_L() GPIO_IIC->BRR = IIC_SCL

#define IIC_SDA_R() GPIO_ReadInputDataBit(GPIO_IIC,IIC_SDA)        //读取引脚电平
#endif
//

void IIC_Start_Init(int SET);
char IIC_Send_DATA(char Addr,const char *Data,char ACK,int Length,int Speed);
char IIC_Receive_DATA(char Addr, char *Target,char ACK,int Length,int Speed);

void IIC_StartBit(void);
void IIC_StopBit(void);
char IIC_WaitASK(void);
void IIC_Write_DATA(char DATA,int Speed);
char IIC_Read_DATA(int Speed);
	

#endif
