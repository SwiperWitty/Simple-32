#ifndef _DATA_HANDLE_H__
#define _DATA_HANDLE_H__
//Data_Handle
/*				纯C#数据处理算法

	1.数据转换	Data_Replace
	2.数据判断	Data_Judge
	3.数字提取	Data_Chang_NUM


							2021.10.06

************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define U8 unsigned char 
#define U32 unsigned int 

extern char Data_Array[20];

void Data_Replace (char *Source,char *Target,char Data,int Length);		// Source 源	Target 要被目标		Data 源（单字节）	Length 长度
char Data_Judge(char *Source,char *Target,char Length);

int String_Chang_Number (char *Source,float *Num,char Sign,char Length);
char String_Chang_NUMs(char *String,float *NUMs,char *Sign,char Count);
char Data_Filtration(void *data,char Length,char num);     //每次塞一个 data,如果连续重复数超过num，那么就算成功通过 return(1),失败是(0)
#endif
