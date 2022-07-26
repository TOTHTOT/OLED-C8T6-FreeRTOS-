/*
 * @Author: your name
 * @Date: 2022-02-19 19:59:36
 * @LastEditTime: 2022-02-19 21:19:14
 * @LastEditors: your name
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USER\main.h
 */
#ifndef __MAIN_H
#define __MAIN_H

/* SYSTEM */
#include "string.h"
#include "sys.h"
/*HARDWARE */
#include "led.h"
#include "delay.h"
#include "timer.h"
#include "malloc.h"
#include "usart.h"
#include "usart3.h"
#include "lora.h"
#include "key.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

void AutoReloadCallback(void);
//任务优先级
#define START_TASK_PRIO 21
//任务堆栈大小
#define START_STK_SIZE 150
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO 2
//任务堆栈大小
#define LED0_STK_SIZE 180
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define SEND_DATA_TASK_PRIO 3
//任务堆栈大小
#define SEND_DATA_STK_SIZE 180
//任务句柄
TaskHandle_t SEND_DATATask_Handler;
//任务函数
void send_data_task(void *pvParameters);

//任务优先级
#define KEY_SCAN_TASK_PRIO 4
//任务堆栈大小
#define KEY_SCAN_STK_SIZE 180
//任务句柄
TaskHandle_t KEY_SCANTask_Handler;
//任务函数
void key_scan_task(void *pvParameters);

#endif
