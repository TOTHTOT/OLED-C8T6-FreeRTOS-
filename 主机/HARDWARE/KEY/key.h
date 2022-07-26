#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
	 

 
#define BUTTON1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#define BUTTON2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)
#define BUTTON3 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)
#define BUTTON4 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)

#define BUTTON1_PRESS 1
#define BUTTON2_PRESS 2
#define BUTTON3_PRESS 3
#define BUTTON4_PRESS 4

#define KEY0_PRES	1		//KEY0  
//#define KEY1_PRES	2		//KEY1 
//#define WKUP_PRES	3		//WK_UP  

void KEY_Init(void);//IO初始化
u8 KEY_Scan(void);  	//按键扫描函数					    
#endif
