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
char *InfoBuff;   //������������״̬��Ϣ
#endif
// ����һ���¼���־��
EventGroupHandle_t EventGroup_Handler;
//����1����ֵ�ź���,OLED��ʾ����
xSemaphoreHandle OLED_Refresh_Semaphore;
// ����һ�������ʱ�����,����ʱ��1��2��
xTimerHandle AutoReload_Handle;
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����ϵͳ�ж����ȼ�����4
	delay_init();									//��ʱ������ʼ��
	uart_init(115200);								//��ʼ������
	TIM4_Int_Init(100 - 1, 7200 - 1);				//��ʱ��7��ʼ�� 1msûˢ�¼���ֵ���ж��������
	LED_Init();										//��ʼ��LED
	mem_init();										//��ʼ���ڲ��ڴ��
	OLED_Init();									//��ʼ��OLED
	main_page();									//������

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
	taskENTER_CRITICAL();							   //�����ٽ���
	EventGroup_Handler = xEventGroupCreate();		   // �����¼���־��
	OLED_Refresh_Semaphore = xSemaphoreCreateBinary(); // ������ֵ�ź���
													   // ���ʹ���������ʱ��
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
	//����OLED��ʾ����
	xTaskCreate((TaskFunction_t)wait_tcmd_task,
				(const char *)"oled_task",
				(uint16_t)WAIT_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)WAIT_TASK_PRIO,
				(TaskHandle_t *)&WAITTask_Handler);
	taskEXIT_CRITICAL();			//�˳��ٽ���
	usart3_init(115200);			//��ʼ������3
	LORA_Init();					//��ʼ��LORA
	xTimerStart(AutoReload_Handle, 0); //���������ʱ��
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
}

//LED0������
void led0_task(void *pvParameters)
{
	while (1)
	{ 
		LED0 = ~LED0;
// ��������ʱ��ͳ�ƹ�
#if (configGENERATE_RUN_TIME_STATS == 1)	
		printf("LED0��������ִ��\r\n");
		//memset(RunTimeInfo, 0, 400);	   //��Ϣ����������
		RunTimeInfo = (char*)mymalloc(400);
		vTaskGetRunTimeStats(RunTimeInfo); //��ȡ��������ʱ����Ϣ
		printf("������\t\t\t����ʱ��\t������ռ�ٷֱ�\r\n");
		printf("%s\r\n", RunTimeInfo);
		myfree(RunTimeInfo);
		//memset(InfoBuff, 0, 1000); //��Ϣ����������
		InfoBuff = (char*)mymalloc(1000);
		vTaskList(InfoBuff);
		printf("%s\r\n", InfoBuff);
		myfree(InfoBuff);
#endif
		delay_ms(500);
	}
}

//�ȴ���ʼ��������
void wait_tcmd_task(void *pvParameters)
{
//	BaseType_t err = pdFALSE;
	EventBits_t EventValue;
	while (1)
	{
		//������null˵�������ɹ�
		if (EventGroup_Handler != NULL)
		{
			//��ȡ��־�¼����0λ�͵�1λ,��ȡ�����0, �ȴ����¼���1������1�󷵻�ֵ��Ϊ0
			EventValue = xEventGroupWaitBits(EventGroup_Handler, 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5, pdTRUE, pdFALSE, portMAX_DELAY);
			if (EventValue != 0)
			{
				switch (EventValue)
				{
				case 1 << 2:
					printf("olde show zhangsan\r\n");
					OLED_ShowCHinese(32, 0, 4); //��
					OLED_ShowCHinese(48, 0, 5); //��
					break;
				case 1 << 3:
					printf("olde show lisi\r\n");
					OLED_ShowCHinese(32, 0, 6); //��
					OLED_ShowCHinese(48, 0, 7); //��
					break;
				case 1 << 4:
					printf("olde show wangwu\r\n");
					OLED_ShowCHinese(32, 0, 8); //��
					OLED_ShowCHinese(48, 0, 9); //��
					break;
				case 1 << 5:
					printf("olde show zhaoliu\r\n");
					OLED_ShowCHinese(32, 0, 10); //��
					OLED_ShowCHinese(48, 0, 11); //��
					break;
				}
			}
		}
		delay_ms(100);
	}
}
// �����ʱ���ص�����
void AutoReloadCallback(void)
{
	// static u8 i = 0;
	system_time_increase();
	main_page_data();
	
}
