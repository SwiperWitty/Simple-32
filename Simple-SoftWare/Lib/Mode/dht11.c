#include "dht11.h"
#include "sys_time.h"

 //////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//DHT11������ʪ�ȴ�������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
      
//��λDHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_L(); 			//����DQ
    Delay_ms(20);    	//��������18ms
    DHT11_H(); 			//DQ=1 
	Delay_10us(3);     	//��������20~40us
}
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_Read() &&retry<100)//DHT11������40~80us
	{
		retry++;
		Delay_10us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_Read() &&retry<100)//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		Delay_10us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}
//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_Read() &&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		Delay_10us(1);
	}
	retry=0;
	while(!DHT11_Read() &&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		Delay_10us(1);
	}
	Delay_10us(4);//�ȴ�40us
	if(DHT11_Read() )return 1;
	else return 0;		   
}
//��DHT11��ȡһ���ֽ�
//����ֵ������������
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}else return 1;
	return 0;	    
}
//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����    	 
u8 DHT11_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PB5�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);				 //��ʼ��IO��
 	GPIO_SetBits(GPIOB,GPIO_Pin_4);						 //PGB5 �����
			    
	DHT11_Rst();  //��λDHT11
	return DHT11_Check();//�ȴ�DHT11�Ļ�Ӧ
} 






