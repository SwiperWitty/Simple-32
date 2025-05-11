#ifndef _MODE_BYJ_H__
#define _MODE_BYJ_H__

#include "stm32f10x.h"

/*
    28BJY48(28表示电机直径28毫米，B表示步进电机，Y表示永磁，J表示带减速箱48表示可以四拍和八拍运行)
    
*/

#define Reduction 512 
#define Exist_STEP_Motor
#ifdef Exist_STEP_Motor
    #define STEP_OUT1   GPIO_Pin_3
    #define STEP_OUT2   GPIO_Pin_4 
    #define STEP_OUT3   GPIO_Pin_5 
    #define STEP_OUT4   GPIO_Pin_15 
    #define STEP_Clock  GPIOB

    #define STEP_OUT_L(x) (STEP_Clock->BRR = STEP_OUT1 << (x))
    #define STEP_OUT_H(x) (STEP_Clock->BSRR = STEP_OUT1 << (x))

#endif
void Motor_BYJ_Init(int SET);
char Motor_BYJ_Drive(char Rotation,char Mode,int Code);


#endif
