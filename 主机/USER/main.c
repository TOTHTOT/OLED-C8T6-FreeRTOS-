/*
 * @Author: TOTHTOT
 * @Date: 2022-02-19 19:48:43
 * @LastEditTime: 2022-02-19 21:16:53
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USER\main.c
 */

#include "main.h"

// 开启任务时间统计功能时执行以下代码
#if (configGENERATE_RUN_TIME_STATS == 1)
char *RunTimeInfo; //保存任务运行时间信息
char *InfoBuff;	   //保存任务运行状态信息
#endif
// 声明一个软件定时器句柄,更新时间1秒2次
xTimerHandle AutoReload_Handle;
// 声明一个事件标志组
EventGroupHandle_t EventGroup_Handler;
EventGroupHandle_t Send_Data_EventGroup_Handler;
int main(void)
{						//年_H  年_L   月    日    时    分    秒
	u8 system_time[] = {0x20, 0x22, 0x05, 0x07, 0x11, 0x01, 0x14}; //时间是2022年5月7日11时1分14秒
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);				   //设置系统中断优先级分组4
	delay_init();												   //延时函数初始化
	uart_init(115200);											   //初始化串口
	TIM4_Int_Init(100 - 1, 7200 - 1);							   //定时器7初始化 1ms没刷新计数值就判定接收完成
	LED_Init();													   //初始化LED
	mem_init();													   //初始化内部内存池
	Set_Time(system_time);										   //设置系统时间
	//创建开始任务
	xTaskCreate((TaskFunction_t)start_task,			 //任务函数
				(const char *)"start_task",			 //任务名称
				(uint16_t)START_STK_SIZE,			 //任务堆栈大小
				(void *)NULL,						 //传递给任务函数的参数
				(UBaseType_t)START_TASK_PRIO,		 //任务优先级
				(TaskHandle_t *)&StartTask_Handler); //任务句柄
	vTaskStartScheduler();							 //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();								//进入临界区
	EventGroup_Handler = xEventGroupCreate();			// 创建事件标志组
	Send_Data_EventGroup_Handler = xEventGroupCreate(); // 创建事件标志组
#if (configUSE_TIMERS == 1)
	// 创建软件定时器, 定时0.5s(500)个时钟节拍 , 周期模式pdFALSE为单次模式
	AutoReload_Handle = xTimerCreate((const char *)"AutoRelodTimers",
									 (TickType_t)500 / portTICK_PERIOD_MS,
									 (UBaseType_t)pdTRUE,
									 (void *)1,
									 (TimerCallbackFunction_t)AutoReloadCallback);
#endif
	//创建LED0任务
	xTaskCreate((TaskFunction_t)led0_task,
				(const char *)"led0_task",
				(uint16_t)LED0_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED0_TASK_PRIO,
				(TaskHandle_t *)&LED0Task_Handler);
	//创建发送数据任务
	xTaskCreate((TaskFunction_t)send_data_task,
				(const char *)"senddata_task",
				(uint16_t)SEND_DATA_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)SEND_DATA_TASK_PRIO,
				(TaskHandle_t *)&SEND_DATATask_Handler);
	//创建键盘扫描任务
	xTaskCreate((TaskFunction_t)key_scan_task,
				(const char *)"KeyScan_task",
				(uint16_t)KEY_SCAN_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)KEY_SCAN_TASK_PRIO,
				(TaskHandle_t *)&SEND_DATATask_Handler);

	taskEXIT_CRITICAL();			   //退出临界区
	usart3_init(115200);			   //初始化串口3
	LORA_Init();					   //初始化LORA
	xTimerStart(AutoReload_Handle, 0); //启动软件定时器
	vTaskDelete(StartTask_Handler);	   //删除开始任务
}

// LED0任务函数
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
// 开启任务时间统计功? 苁敝葱幸韵麓?码
#if (configGENERATE_RUN_TIME_STATS == 1)
		printf("LED0任务正在执行\r\n");
		// memset(RunTimeInfo, 0, 400);	   //信息缓冲区清零
		RunTimeInfo = (char *)mymalloc(400);
		vTaskGetRunTimeStats(RunTimeInfo); //获取任务运行时间信息
		printf("任务名\t\t\t运行时间\t运行所占百分比\r\n");
		printf("%s\r\n", RunTimeInfo);
		myfree(RunTimeInfo);
		// memset(InfoBuff, 0, 1000); //信息缓冲区清零
		InfoBuff = (char *)mymalloc(1000);
		vTaskList(InfoBuff);
		printf("%s\r\n", InfoBuff);
		myfree(InfoBuff);
#endif
		delay_ms(500);
	}
}

// 软件定时器回调函数
void AutoReloadCallback(void)
{
	// static u8 i = 0;
	// printf("AutoReloadCallback\r\n");
	system_time_increase();
}

// 键盘扫描任务函数
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

// 发送数据给从机任务,根据按键判断发送哪个
void send_data_task(void *pvParameters)
{
	EventBits_t EventValue;
	char data[100];
	while (1)
	{
		//不等于null说明创建成功
		if (EventGroup_Handler != NULL)
		{
			//获取标志事件组第0位和第1位,获取完后置0, 等待的事件有1个被置1后返回值不为0
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
