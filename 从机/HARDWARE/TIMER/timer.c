/*
 * @Author: TOTHTOT
 * @Date: 2022-02-19 20:18:16
 * @LastEditTime: 2022-02-19 20:26:48
 * @LastEditors: Please set LastEditors
 * @Description: ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\ѧϰ\C8T6FreeRTOS��ֲ\HARDWARE\TIMER\timer.c
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
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//    ?   ?     ?  TIMx  ?       ��

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //?  ?    TIM3 �� ,         ��

	// ��    ? NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  // TIM3 ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7; //  ?   ? 4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //     ? 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ?    ?
	NVIC_Init(&NVIC_InitStructure);							  //  ?  NVIC ?

	TIM_Cmd(TIM3, ENABLE); //?  TIMx
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //    ��
	{
		FreeRTOSRunTimeTicks++;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //    ��? ?��
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

extern EventGroupHandle_t EventGroup_Handler; //�¼���־����

void TIM4_IRQHandler(void)
{
	char *time_data;
	BaseType_t xHigherPriorityTaskWoken, err;
	// �����ж�
	//printf("�����ж�\r\n");
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		/* if (TIM4_EN_COUN == 1)
		{
			TIM4_EN_COUN_NUM++;
			printf("�ж��Ƿ�������\r\n");
			if (TIM4_EN_COUN_NUM >= 4)
			{
				USART3_RX_STA |= 0x8000;
				printf("�������:%s\r\n", USART3_RX_BUF);
				TIM4_EN_COUN = 0;
			}
		} */
		// printf("�������, ����:%d\r\n", USART3_RX_STA&0x3fff);
		USART3_RX_STA |= 1 << 15;
		printf("�������, ����%d\r\n", USART3_RX_STA & 0x3fff);
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
				printf("���յ�start,��1����\r\n");
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
				printf("���յ�reboot,��1����\r\n");
			}
		}
		else if ((strstr((char *)USART3_RX_BUF, "ame1") != NULL))
		{
			// ��ʾ��һ���˵�����
			USART3_RX_STA = 0;
			//printf("receive name1\r\n");
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 2, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name1 set 1 error\r\n");
			}
			//�����ݰ��н���ʱ����Ϣ,������йؼ��ֵĻ�
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}
		else if ((strstr((char *)USART3_RX_BUF, "name2") != NULL))
		{
			// ��ʾ�ڶ����˵�����
			USART3_RX_STA = 0;
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 3, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name2 set 1 error\r\n");
			}
			//�����ݰ��н���ʱ����Ϣ,������йؼ��ֵĻ�
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}
		else if ((strstr((char *)USART3_RX_BUF, "name3") != NULL))
		{
			// ��ʾ�������˵�����
			USART3_RX_STA = 0;
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 4, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name3 set 1 error\r\n");
			}
			//�����ݰ��н���ʱ����Ϣ,������йؼ��ֵĻ�
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}
		else if ((strstr((char *)USART3_RX_BUF, "name4") != NULL))
		{
			// ��ʾ��4���˵�����
			USART3_RX_STA = 0;
			err = xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 5, &xHigherPriorityTaskWoken);
			if (err == pdFALSE)
			{
				printf("receive name4 set 1 error\r\n");
			}
			//�����ݰ��н���ʱ����Ϣ,������йؼ��ֵĻ�
			time_data = strstr((char *)USART3_RX_BUF, "time");
			printf("time:%s\r\n", (time_data+5));
			Set_Time(time_data+5);
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		}

//		if ((strstr((char *)USART3_RX_BUF, "time") != NULL))
//		{
//			//�����ݰ��н���ʱ����Ϣ,������йؼ��ֵĻ�
//			time_data = strstr((char *)USART3_RX_BUF, "time");
//			printf("time:%s\r\n", (time_data+4));
//			Set_Time(time_data+5);
//		}
		TIM_Cmd(TIM4, DISABLE);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
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
	uint8_t month_day_tab[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //ÿ�µ�����
	TDATATIME.msec++;
	if (TDATATIME.msec >= 2) // 500ms�����ж�һ�Σ�2�ξ���1ms
	{
		TDATATIME.msec = 0L;
		TDATATIME.second++; //���λ
		sprintf(now_time, "%d%d%d%d%d%d", TDATATIME.year, TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
		printf("time: %s\r\n", now_time);
		if (TDATATIME.second >= 60)
		{
			TDATATIME.second = 0;
			TDATATIME.minute++; //���ӽ�λ
			if (TDATATIME.minute >= 60)
			{
				TDATATIME.minute = 0;
				TDATATIME.hour++; //Сʱ��λ
				if (TDATATIME.hour >= 24)
				{
					TDATATIME.hour = 0;
					TDATATIME.day++; //���λ
					if (TDATATIME.day > (((TDATATIME.year % 4 == 0) && (TDATATIME.month == 2)) ? month_day_tab[TDATATIME.month] + 1 : month_day_tab[TDATATIME.month]))
					{
						TDATATIME.day = 1;
						TDATATIME.month++; //�½�λ
						if (TDATATIME.month > 12)
						{
							TDATATIME.month = 1;
							TDATATIME.year++; //���λ
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
