/*
 * @Author: your name
 * @Date: 2022-02-20 11:11:39
 * @LastEditTime: 2022-02-21 09:29:32
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\实例\茶园环境监测_从机\FreeRTOS\HARDWARE\USART\usart3.h
 */
#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  

#define STARTCMD "&&start$$"

#define USART3_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.
 
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART3_RX_STA;   						//接收数据状态
extern u8 check_cmd_result, cmd_flag, s1, str_len;
extern u8 TIM4_EN_COUN;                             //使定时器4开始计数
extern u8 TIM4_EN_COUN_NUM;                         //定时器4计数值
void usart3_init(u32 bound);				//串口3初始化 
void u3_printf(char* fmt,...);
void USART3_RX_Data(void);
u8 Check_LORA_Return_Is_OK(void); 
void u3_send_byte(unsigned char SendData);
unsigned char u3_get_byte(unsigned char* GetData);
#endif
 
 
 
 
 
 
 
 
 





