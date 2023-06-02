#include "Data_Handle.h"


int String_Chang_Number (char *Source,float *Num,char Sign,char Length)
{
	char Ascii;
	int i,j = 0;
	float temp;
    char Data_Array[20];
	memset(Data_Array,0,sizeof(Data_Array));
	for(i = 0;i < Length;i++)
	{
		Ascii = *(Source + i);
		if(Ascii == Sign)
		{
			Sign = 0;
			continue;
		}
		
		if(Sign == 0)
		{
			if((Ascii <= '9' && Ascii >= '0') || Ascii == '.' || Ascii == '-'|| Ascii == '+')
			{
				*(Data_Array + j) = Ascii;
				j++;
			}
			else
			{
				if(j > 0)	break;
			}
		}
	}
	temp = atof(Data_Array);
	*Num = temp;
	
	return i;   //返回本次处理Source的长度
}

//atof() 用于将字符串转换为双精度浮点数(double)
//atoi() 函数用来将字符串转换成整数(int)
//strtod() 用来将字符串转换成双精度浮点数(double)

char String_Chang_NUMs(char *String,float *NUMs,char *Sign,char Count)      //
{
    int Length = strlen(String);
    int Run_num = 0;
    char Reval = 0;
    if(String == NULL || NUMs == NULL || Sign == NULL || Count == 0)
    {
        return Reval;
    }
    for(int i = 0;i < Count;i++)
    {
        Run_num += String_Chang_Number (String + Run_num,NUMs + i,*(Sign + i),Length - Run_num);
    }
    Reval = 1;
    return Reval;
}

void Data_Replace (char *Source,char *Target,char Data,int Length)
{
	int n;
	for(n = 0;n < Length;n++)
	{
		if(Data == 'F')
		{
			*(Target + n) = *(Source + n);
		}
		else 
		{
			*(Target + n) = Data;
		}
	}
}

char Data_Judge(char *Source,char *Target,char Length)
{
	char n,Data_S,Data_T;
	for(n = 0;n < Length - 1;n++)
	{
		Data_S = *(Source + n);
		Data_T = *(Target + n);
		if(Data_T == Data_S)
		{
			
		}
		else 
		{
			return 0;
		}
	}
	return n;
}

char Data_Filtration(void *data,char Length,char num)   //积水过滤函数
{
	static char data_temp[10][64];
	static char run_data = 0;
	char temp = 0;
	if(num >= 10 || Length >= 64 || data == NULL)
	{
		memset(data_temp,0,sizeof(data_temp));
		run_data = 0;
		return 0;
	}
	if(run_data >= num)
	{
		for(int i = 0;i < num;i++)
		{
			temp = memcmp(data,data_temp[i],Length);
			if(temp != 0)
			{
				memset(data_temp,0,sizeof(data_temp));
				run_data = 0;
				return 0;
			}
		}
		memset(data_temp,0,sizeof(data_temp));
		run_data = 0;
		return 1;			//成功
	}
	memcpy(data_temp[run_data],data,Length);
	run_data++;
	if(run_data > 2)    //数量超过2才开始比较
	{
		if(memcmp(data_temp[run_data-2],data_temp[run_data-1],Length) != 0)
		{
			memset(data_temp,0,sizeof(data_temp));
			run_data = 0;
			return 0;		//0是失败
		}
	}
	printf("Data_Filt_run:%d \r\n",run_data);
	return (char)(-1);		//进行中
}
