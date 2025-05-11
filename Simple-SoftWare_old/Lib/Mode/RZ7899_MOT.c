#include "RZ7899_MOT.h"
#include "stm32f10x.h"

// 
void RZ7899_MOT_init (void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1);
}

/* 
	1左
	2右
*/
void RZ7899_MOT_Controls (char cmd)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1);
	switch(cmd)
	{
		case 1:
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
			break;
		case 2:
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
			break;
		default:
			break;
	}
	
}



