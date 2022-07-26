/*
 * @Author: TOTHTOT
 * @Date: 2022-02-19 19:48:43
 * @LastEditTime: 2022-02-19 21:16:53
 * @LastEditors: Please set LastEditors
 * @Description: ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USER\main.c
 */

#include "main.h"

// ��������ʱ��ͳ�ƹ���ʱִ�����´���
#if (configGENERATE_RUN_TIME_STATS == 1)
char *RunTimeInfo; //������������ʱ����Ϣ
char *InfoBuff;	   //������������״̬��Ϣ
#endif
// ����һ�������ʱ�����,����ʱ��1��2��
xTimerHandle AutoReload_Handle;
// ����һ���¼���־��
EventGroupHandle_t EventGroup_Handler;
EventGroupHandle_t Send_Data_EventGroup_Handler;
int main(void)
{						//��_H  ��_L   ��    ��    ʱ    ��    ��
	u8 system_time[] = {0x20, 0x22, 0x05, 0x07, 0x11, 0x01, 0x14}; //ʱ����2022��5��7��11ʱ1��14��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);				   //����ϵͳ�ж����ȼ�����4
	delay_init();												   //��ʱ������ʼ��
	uart_init(115200);											   //��ʼ������
	TIM4_Int_Init(100 - 1, 7200 - 1);							   //��ʱ��7��ʼ�� 1msûˢ�¼���ֵ���ж��������
	LED_Init();													   //��ʼ��LED
	mem_init();													   //��ʼ���ڲ��ڴ��
	Set_Time(system_time);										   //����ϵͳʱ��
	//������ʼ����
	xTaskCreate((TaskFunction_t)start_task,			 //������
				(const char *)"start_task",			 //��������
				(uint16_t)START_STK_SIZE,			 //�����ջ��С
				(void *)NULL,						 //���ݸ��������Ĳ���
				(UBaseType_t)START_TASK_PRIO,		 //�������ȼ�
				(TaskHandle_t *)&StartTask_Handler); //������
	vTaskStartScheduler();							 //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();								//�����ٽ���
	EventGroup_Handler = xEventGroupCreate();			// �����¼���־��
	Send_Data_EventGroup_Handler = xEventGroupCreate(); // �����¼���־��
#if (configUSE_TIMERS == 1)
	// ���������ʱ��, ��ʱ0.5s(500)��ʱ�ӽ��� , ����ģʽpdFALSEΪ����ģʽ
	AutoReload_Handle = xTimerCreate((const char *)"AutoRelodTimers",
									 (TickType_t)500 / portTICK_PERIOD_MS,
									 (UBaseType_t)pdTRUE,
									 (void *)1,
									 (TimerCallbackFunction_t)AutoReloadCallback);
#endif
	//����LED0����
	xTaskCreate((TaskFunction_t)led0_task,
				(const char *)"led0_task",
				(uint16_t)LED0_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED0_TASK_PRIO,
				(TaskHandle_t *)&LED0Task_Handler);
	//����������������
	xTaskCreate((TaskFunction_t)send_data_task,
				(const char *)"senddata_task",
				(uint16_t)SEND_DATA_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)SEND_DATA_TASK_PRIO,
				(TaskHandle_t *)&SEND_DATATask_Handler);
	//��������ɨ������
	xTaskCreate((TaskFunction_t)key_scan_task,
				(const char *)"KeyScan_task",
				(uint16_t)KEY_SCAN_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)KEY_SCAN_TASK_PRIO,
				(TaskHandle_t *)&SEND_DATATask_Handler);

	taskEXIT_CRITICAL();			   //�˳��ٽ���
	usart3_init(115200);			   //��ʼ������3
	LORA_Init();					   //��ʼ��LORA
	xTimerStart(AutoReload_Handle, 0); //���������ʱ��
	vTaskDelete(StartTask_Handler);	   //ɾ����ʼ����
}

// LED0������
void led0_task(void *pvParameters)
{
	u8 i = 0;
	while (1)
	{
		if(i == 4)
		{
			i = 0;
		}
		// printf("i:%d\r\n", i);
		xEventGroupSetBits(Send_Data_EventGroup_Handler, 1 << i);
		i++;
		LED0 = ~LED0;
// ��������ʱ��ͳ�ƹ�? �ʱִ�����´?��
#if (configGENERATE_RUN_TIME_STATS == 1)
		printf("LED0��������ִ��\r\n");
		// memset(RunTimeInfo, 0, 400);	   //��Ϣ����������
		RunTimeInfo = (char *)mymalloc(400);
		vTaskGetRunTimeStats(RunTimeInfo); //��ȡ��������ʱ����Ϣ
		printf("������\t\t\t����ʱ��\t������ռ�ٷֱ�\r\n");
		printf("%s\r\n", RunTimeInfo);
		myfree(RunTimeInfo);
		// memset(InfoBuff, 0, 1000); //��Ϣ����������
		InfoBuff = (char *)mymalloc(1000);
		vTaskList(InfoBuff);
		printf("%s\r\n", InfoBuff);
		myfree(InfoBuff);
#endif
		delay_ms(500);
	}
}

// �����ʱ���ص�����
void AutoReloadCallback(void)
{
	// static u8 i = 0;
	// printf("AutoReloadCallback\r\n");
	system_time_increase();
}

// ����ɨ��������
void key_scan_task(void *pvParameters)
{
	while (1)
	{
		switch (KEY_Scan())
		{
			case BUTTON1_PRESS:
				xEventGroupSetBits(Send_Data_EventGroup_Handler, 1 << 0);
				break;
			case BUTTON2_PRESS:
				xEventGroupSetBits(Send_Data_EventGroup_Handler, 1 << 1);
				break;
			case BUTTON3_PRESS:
				xEventGroupSetBits(Send_Data_EventGroup_Handler, 1 << 2);
				break;
			case BUTTON4_PRESS:
				xEventGroupSetBits(Send_Data_EventGroup_Handler, 1 << 3);
				break;

		}
		delay_ms(10);
	}
}

// �������ݸ��ӻ�����,���ݰ����жϷ����ĸ�
void send_data_task(void *pvParameters)
{
	EventBits_t EventValue;
	char data[100];
	while (1)
	{
		//������null˵�������ɹ�
		if (EventGroup_Handler != NULL)
		{
			//��ȡ��־�¼����0λ�͵�1λ,��ȡ�����0, �ȴ����¼���1������1�󷵻�ֵ��Ϊ0
			EventValue = xEventGroupWaitBits(Send_Data_EventGroup_Handler, 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4, pdTRUE, pdFALSE, portMAX_DELAY);
			if (EventValue != 0)
			{
				printf("year:%x, %x, %x\r\n", TDATATIME.year, TDATATIME.year >> 8, TDATATIME.year & 0x00ff);
				// printf("EventValue:%d\r\n", EventValue);
				switch (EventValue)
				{
				case 1 << 0:
					printf("Send name1 and time\r\n");
					sprintf(data, "name1, time:%d%d%d%d%d", TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
					Send_Data(0x00, 0x02, 0x17, (u8 *)data);
					break;
				case 1 << 1:
					printf("Send name2 and time\r\n");
					sprintf(data, "name2, time:%d%d%d%d%d", TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
					Send_Data(0x00, 0x02, 0x17, (u8 *)data);
					break;
				case 1 << 2:
					printf("Send name3 and time\r\n");
					sprintf(data, "name3, time:%d%d%d%d%d", TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
					Send_Data(0x00, 0x02, 0x17, (u8 *)data);
					break;
				case 1 << 3:
					printf("Send name4 and time\r\n");
					sprintf(data, "name4, time:%d%d%d%d%d", TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
					Send_Data(0x00, 0x02, 0x17, (u8 *)data);
					break;
				case 1 << 4:
					printf("Send name5 and time\r\n");
					sprintf(data, "name5, time:%d%d%d%d%d", TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute, TDATATIME.second);
					Send_Data(0x00, 0x02, 0x17, (u8 *)data);
					break;
				}
			}
		}
		delay_ms(100);
	}
}
