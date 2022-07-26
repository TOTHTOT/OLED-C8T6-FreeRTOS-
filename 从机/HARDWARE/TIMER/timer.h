/*
 * @Author: TOTHTOT
 * @Date: 2022-02-19 20:19:03
 * @LastEditTime: 2022-02-19 20:43:41
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\学习\C8T6FreeRTOS移植\HARDWARE\TIMER\timer.h
 */

#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

typedef struct 
{
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8  msec;
}_TDateTime;


extern _TDateTime TDATATIME;
extern u32 FreeRTOSRunTimeTicks;
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr, u16 psc);
void ConfigureTimeForRunTimeStats(void);
void system_time_increase(void);
void Set_Time(char* time_unanalysis);

#endif

