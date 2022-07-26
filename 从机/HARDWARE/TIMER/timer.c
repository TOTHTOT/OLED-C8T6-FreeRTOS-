/*
 * @Author: TOTHTOT
 * @Date: 2022-02-19 20:18:16
 * @LastEditTime: 2022-02-19 20:26:48
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\学习\C8T6FreeRTOS移植\HARDWARE\TIMER\timer.c
 */
/* SYSTEM */
#include "string.h"
#include "sys.h"
/*ARDWARE */
#include "led.h"
#include "delay.h"
#include "timer.h"
#include "malloc.h"
#include "usart.h"
#include "usart3.h"
#include "oled.h"
#include "lora.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "queue.h"

u32 FreeRTOSRunTimeTicks;
_TDateTime TDATATIME;

void ConfigureTimeForRunTimeStats(void)
{
	FreeRTOSRunTimeTicks = 0;
	TIM3_Int_Init(50 - 1, 72 - 1); //  ?  TIM3
}

void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //?  ?

	//  ?  TIM3  ?
	TIM_TimeBaseStructure.TIM_Period = arr;						//        ?       ? ?  ?   ?   ? ??      ? ?
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//          ?TIMx?  ? ?     ?  ??
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//    ? ?? :TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM   ?   ??
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//    ?   ?     ?  TIMx  ?       λ

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //?  ?    TIM3 ж ,         ж

	// ж    ? NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  // TIM3 ж
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7; //  ?   ? 4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //     ? 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ?    ?
	NVIC_Init(&NVIC_InitStructure);							  //  ?  NVIC ?

	TIM_Cmd(TIM3, ENABLE); //?  TIMx
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //    ж
	{
		FreeRTOSRunTimeTicks++;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //    ж? ?λ
}

void TIM4_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

extern EventGroupHandle_t EventGroup_Handler; //事件标志组句柄

void TIM4_IRQHandler(void)
{
	char *time_data;
	BaseType_t xHigherPriorityTaskWoken, err;
	// 触发中断
	//printf("触发中断\r\n");
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		/* if (TIM4_EN_COUN == 1)
		{
			TIM4_EN_COUN_NUM++;
			printf("判断是否接收完成\r\n");
			if (TIM4_EN_COUN_NUM >= 4)
			{
				USART3_RX_STA |= 0x8000;
				printf("接收完成:%s\r\n", USART3_RX_BUF);
				TIM4_EN_COUN = 0;
			}
		} */
		// printf("接收完成, 长度:%d\r\n", USART3_RX_STA&0x3fff);
		USART3_RX_STA |= 1 << 15;
		printf("接收完成, 长度%d\r\n", USART3_RX_STA & 0x3fff);
		printf("%s\r\n", USART3_RX_BUF);
		if (strstr((char *)USART3_RX_BUF, "OK") != NULL)
		{
			// printf("%S\r\n", USART3_RX_BUF);
			USART3_RX_STA = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			Lora_Ok_Flag = 1;
		}
		else if (strstr((char *)USART3_RX_BUF, "ERROR") != NULL)
		{
			// printf("%S\r\n", USART3_RX_BUF);
			USART3_RX_STA = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			Lora_Ok_Flag = 2;
		}
		else if (strstr((char *)USART3_RX_BUF, "start") != NULL)
		{
			// printf("%S\r\n", USART3_RX_BUF);
			USART3_RX_STA = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 0, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("接收到start,置1错误\r\n");
			}
		}
		else if (strstr((char *)USART3_RX_BUF, "reboot") != NULL)
		{
			// printf("%S\r\n", USART3_RX_BUF);
			USART3_RX_STA = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 1, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("接收到reboot,置1错误\r\n");
			}
		}
		else if ((strstr((char *)USART3_RX_BUF, "ame1") != NULL))
		{
			// 显示第一个人的名字
			USART3_RX_STA = 0;
			//printf("receive name1\r\n");
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 2, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name1 set 1 error\r\n");
			}
			//从数据包中解析时间信息,如果含有关键字的话
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}
		else if ((strstr((char *)USART3_RX_BUF, "name2") != NULL))
		{
			// 显示第二个人的名字
			USART3_RX_STA = 0;
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 3, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name2 set 1 error\r\n");
			}
			//从数据包中解析时间信息,如果含有关键字的话
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}
		else if ((strstr((char *)USART3_RX_BUF, "name3") != NULL))
		{
			// 显示第三个人的名字
			USART3_RX_STA = 0;
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 4, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name3 set 1 error\r\n");
			}
			//从数据包中解析时间信息,如果含有关键字的话
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}
		else if ((strstr((char *)USART3_RX_BUF, "name4") != NULL))
		{
			// 显示第4个人的名字
			USART3_RX_STA = 0;
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 5, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name4 set 1 error\r\n");
			}
			//从数据包中解析时间信息,如果含有关键字的话
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}

//		if ((strstr((char *)USART3_RX_BUF, "time") != NULL))
//		{
//			//从数据包中解析时间信息,如果含有关键字的话
//			time_data = strstr((char *)USART3_RX_BUF, "time");
//			printf("time:%s\r\n", (time_data+4));
//			Set_Time(time_data+5);
//		}
		TIM_Cmd(TIM4, DISABLE);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

char now_time[20];

void Set_Time(char* time_unanalysis)
{
	//u8 y1, y2, y3, y4;
	/* TDATATIME.year = (TDATATIME.year<<8)&time_unanalysis [0];
	TDATATIME.year = TDATATIME.year&time_unanalysis [1];
	TDATATIME.month = time_unanalysis [2];
	TDATATIME.day = time_unanalysis [3];
	TDATATIME.hour = TDATATIME.year&time_unanalysis [4];
	TDATATIME.minute = TDATATIME.year&time_unanalysis [5];
	TDATATIME.second = TDATATIME.year&time_unanalysis [7]; */

	TDATATIME.year = 2022;
//	y1 = (time_unanalysis[0]>>4&0xf);
//	y2 = (time_unanalysis[0]&0xf);
//	y3 = (time_unanalysis[1]>>4&0xf);
//	y4 = (time_unanalysis[1]&0xf);
//	TDATATIME.year = y1*1000+y2*100+y3*10+y4;
//	printf("1:%c, 2:%c, 3:%c, y1 = %d, y2 = %d, y3 = %d, y4 = %d\r\n",time_unanalysis[0], time_unanalysis[1], time_unanalysis[2] , y1, y2, y3, y4);
	TDATATIME.month = time_unanalysis[0]&0xf0+time_unanalysis [0]&0x0f;
	TDATATIME.day = time_unanalysis[1]&0xf0+time_unanalysis [1]&0x0f;
	TDATATIME.hour = time_unanalysis[2]&0xf0+time_unanalysis [2]&0x0f;
	TDATATIME.minute = time_unanalysis[3]&0xf0+time_unanalysis [3]&0x0f;
	TDATATIME.second = time_unanalysis[4]&0xf0+time_unanalysis [4]&0x0f;

	printf("year:%d, mon:%d, day:%d, hour:%d, minum:%d, sec:%d\r\n", TDATATIME.year, TDATATIME.month,TDATATIME.day,TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
//	TDATATIME.year = (TDATATIME.year<<8)&time_unanalysis [0]; 
//	TDATATIME.year = TDATATIME.year&time_unanalysis [1];
//	TDATATIME.month = time_unanalysis[2]&0xf0+time_unanalysis [2]&0x0f;
//	TDATATIME.day = time_unanalysis[3]&0xf0+time_unanalysis [3]&0x0f;
//	TDATATIME.hour = time_unanalysis[4]&0xf0+time_unanalysis [4]&0x0f;
//	TDATATIME.minute = time_unanalysis[5]&0xf0+time_unanalysis [5]&0x0f;
//	TDATATIME.second = time_unanalysis[6]&0xf0+time_unanalysis [6]&0x0f;
//	printf("year_h:%x,year_l:%x, %x, %x\r\n\r\n", time_unanalysis[0], time_unanalysis[1], time_unanalysis[2], time_unanalysis[3]);
//	printf("year:%d, mon:%d, day:%d, hour:%d, minum:%d, sec:%d\r\n", TDATATIME.year, TDATATIME.month,TDATATIME.day,TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
}

void system_time_increase(void)
{
	uint8_t month_day_tab[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //每月的天数
	TDATATIME.msec++;
	if (TDATATIME.msec >= 2) // 500ms进入中断一次，2次就是1ms
	{
		TDATATIME.msec = 0L;
		TDATATIME.second++; //秒进位
		sprintf(now_time, "%d%d%d%d%d%d", TDATATIME.year, TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
		printf("time: %s\r\n", now_time);
		if (TDATATIME.second >= 60)
		{
			TDATATIME.second = 0;
			TDATATIME.minute++; //分钟进位
			if (TDATATIME.minute >= 60)
			{
				TDATATIME.minute = 0;
				TDATATIME.hour++; //小时进位
				if (TDATATIME.hour >= 24)
				{
					TDATATIME.hour = 0;
					TDATATIME.day++; //天进位
					if (TDATATIME.day > (((TDATATIME.year % 4 == 0) && (TDATATIME.month == 2)) ? month_day_tab[TDATATIME.month] + 1 : month_day_tab[TDATATIME.month]))
					{
						TDATATIME.day = 1;
						TDATATIME.month++; //月进位
						if (TDATATIME.month > 12)
						{
							TDATATIME.month = 1;
							TDATATIME.year++; //年进位
							if (TDATATIME.year > 99)
							{
								TDATATIME.year = 0;
							}
						}
					}
				}
			}
		}
	}
}
