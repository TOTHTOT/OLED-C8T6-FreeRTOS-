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
char *InfoBuff;   //保存任务运行状态信息
#endif
// 声明一个事件标志组
EventGroupHandle_t EventGroup_Handler;
//声明1个二值信号量,OLED显示任务
xSemaphoreHandle OLED_Refresh_Semaphore;
// 声明一个软件定时器句柄,更新时间1秒2次
xTimerHandle AutoReload_Handle;
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置系统中断优先级分组4
	delay_init();									//延时函数初始化
	uart_init(115200);								//初始化串口
	TIM4_Int_Init(100 - 1, 7200 - 1);				//定时器7初始化 1ms没刷新计数值就判定接收完成
	LED_Init();										//初始化LED
	mem_init();										//初始化内部内存池
	OLED_Init();									//初始化OLED
	main_page();									//主界面

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
	taskENTER_CRITICAL();							   //进入临界区
	EventGroup_Handler = xEventGroupCreate();		   // 创建事件标志组
	OLED_Refresh_Semaphore = xSemaphoreCreateBinary(); // 创建二值信号量
													   // 如果使能了软件定时器
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
	//创建OLED显示任务
	xTaskCreate((TaskFunction_t)wait_tcmd_task,
				(const char *)"oled_task",
				(uint16_t)WAIT_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)WAIT_TASK_PRIO,
				(TaskHandle_t *)&WAITTask_Handler);
	taskEXIT_CRITICAL();			//退出临界区
	usart3_init(115200);			//初始化串口3
	LORA_Init();					//初始化LORA
	xTimerStart(AutoReload_Handle, 0); //启动软件定时器
	vTaskDelete(StartTask_Handler); //删除开始任务
}

//LED0任务函数
void led0_task(void *pvParameters)
{
	while (1)
	{ 
		LED0 = ~LED0;
// 开启任务时间统计功
#if (configGENERATE_RUN_TIME_STATS == 1)	
		printf("LED0任务正在执行\r\n");
		//memset(RunTimeInfo, 0, 400);	   //信息缓冲区清零
		RunTimeInfo = (char*)mymalloc(400);
		vTaskGetRunTimeStats(RunTimeInfo); //获取任务运行时间信息
		printf("任务名\t\t\t运行时间\t运行所占百分比\r\n");
		printf("%s\r\n", RunTimeInfo);
		myfree(RunTimeInfo);
		//memset(InfoBuff, 0, 1000); //信息缓冲区清零
		InfoBuff = (char*)mymalloc(1000);
		vTaskList(InfoBuff);
		printf("%s\r\n", InfoBuff);
		myfree(InfoBuff);
#endif
		delay_ms(500);
	}
}

//等待开始命令任务
void wait_tcmd_task(void *pvParameters)
{
//	BaseType_t err = pdFALSE;
	EventBits_t EventValue;
	while (1)
	{
		//不等于null说明创建成功
		if (EventGroup_Handler != NULL)
		{
			//获取标志事件组第0位和第1位,获取完后置0, 等待的事件有1个被置1后返回值不为0
			EventValue = xEventGroupWaitBits(EventGroup_Handler, 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5, pdTRUE, pdFALSE, portMAX_DELAY);
			if (EventValue != 0)
			{
				switch (EventValue)
				{
				case 1 << 2:
					printf("olde show zhangsan\r\n");
					OLED_ShowCHinese(32, 0, 4); //张
					OLED_ShowCHinese(48, 0, 5); //三
					break;
				case 1 << 3:
					printf("olde show lisi\r\n");
					OLED_ShowCHinese(32, 0, 6); //李
					OLED_ShowCHinese(48, 0, 7); //四
					break;
				case 1 << 4:
					printf("olde show wangwu\r\n");
					OLED_ShowCHinese(32, 0, 8); //王
					OLED_ShowCHinese(48, 0, 9); //五
					break;
				case 1 << 5:
					printf("olde show zhaoliu\r\n");
					OLED_ShowCHinese(32, 0, 10); //赵
					OLED_ShowCHinese(48, 0, 11); //六
					break;
				}
			}
		}
		delay_ms(100);
	}
}
// 软件定时器回调函数
void AutoReloadCallback(void)
{
	// static u8 i = 0;
	system_time_increase();
	main_page_data();
	
}
