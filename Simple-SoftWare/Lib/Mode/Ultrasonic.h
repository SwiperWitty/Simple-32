#ifndef __Ultrasonic_H
#define __Ultrasonic_H

/* 
	基于STM32F10X的GPIO、SYS_TIME系统时钟
	增加无设备不死机功能（基于SYS_TIM）
								2021.10.27
*/

#include "stm32f10x.h"
#include "sys_time.h"

#define Echo	GPIO_Pin_8
#define Trig	GPIO_Pin_9

#define CSB_T_GPIO	GPIOA
#define CSB_E_GPIO	GPIOA

#define CSB_T_GPIO_TIM	RCC_APB2Periph_GPIOA
#define CSB_E_GPIO_TIM	RCC_APB2Periph_GPIOA

void Ultrasonic_Init(void);//初始化
float Distance(void);//距离


#endif
