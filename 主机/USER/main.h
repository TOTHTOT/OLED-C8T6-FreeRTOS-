/*
 * @Author: your name
 * @Date: 2022-02-19 19:59:36
 * @LastEditTime: 2022-02-19 21:19:14
 * @LastEditors: your name
 * @Description: ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
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
//�������ȼ�
#define START_TASK_PRIO 21
//�����ջ��С
#define START_STK_SIZE 150
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO 2
//�����ջ��С
#define LED0_STK_SIZE 180
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define SEND_DATA_TASK_PRIO 3
//�����ջ��С
#define SEND_DATA_STK_SIZE 180
//������
TaskHandle_t SEND_DATATask_Handler;
//������
void send_data_task(void *pvParameters);

//�������ȼ�
#define KEY_SCAN_TASK_PRIO 4
//�����ջ��С
#define KEY_SCAN_STK_SIZE 180
//������
TaskHandle_t KEY_SCANTask_Handler;
//������
void key_scan_task(void *pvParameters);

#endif
