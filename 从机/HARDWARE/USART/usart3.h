/*
 * @Author: your name
 * @Date: 2022-02-20 11:11:39
 * @LastEditTime: 2022-02-21 09:29:32
 * @LastEditors: Please set LastEditors
 * @Description: ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \USERe:\Learn\stm32\ʵ��\��԰�������_�ӻ�\FreeRTOS\HARDWARE\USART\usart3.h
 */
#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  

#define STARTCMD "&&start$$"

#define USART3_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		600					//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.
 
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern vu16 USART3_RX_STA;   						//��������״̬
extern u8 check_cmd_result, cmd_flag, s1, str_len;
extern u8 TIM4_EN_COUN;                             //ʹ��ʱ��4��ʼ����
extern u8 TIM4_EN_COUN_NUM;                         //��ʱ��4����ֵ
void usart3_init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt,...);
void USART3_RX_Data(void);
u8 Check_LORA_Return_Is_OK(void); 
void u3_send_byte(unsigned char SendData);
unsigned char u3_get_byte(unsigned char* GetData);
#endif
 
 
 
 
 
 
 
 
 





