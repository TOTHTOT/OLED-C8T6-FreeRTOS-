/*
 * @Author: your name
 * @Date: 2022-02-20 11:11:39
 * @LastEditTime: 2022-02-23 10:42:34
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\实例\茶园环境监测_从机\FreeRTOS\HARDWARE\LORA\lora.h
 */
#ifndef __LORA_H
#define __LORA_H

#include "sys.h"

#define MD0 PBout(1)
#define AUX PBout(0)
#define MASTER_LORA_ADDH 0x00					//主机地址及信道
#define MASTER_LORA_ADDL 0x00
#define MASTER_LORA_CHANNEL 0x17

#define SLAVE1_LORA_ADDH 0x00					//从机地址及信道
#define SLAVE1_LORA_ADDL 0x01
#define SLAVE1_LORA_CHANNEL 0x17

u8 LORA_Init(void);
extern char lora_tx_data[100];
extern char lora_rx_data[100];
extern u8 Lora_Ok_Flag;

void To_Configure(u8 *cmd, u8 *str);
void LORA_Check(void);
void LORA_Set_Mode(void);
void Send_Cmd(u8*cmd);
void Quer_Cmd(u8 *cmd);
void  Send_Data(u8 addh, u8 addl,u8 channel,u8 *a);
u8 AUX_Check(void);
#endif
