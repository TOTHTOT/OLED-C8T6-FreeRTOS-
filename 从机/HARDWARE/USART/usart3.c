#include "delay.h"
#include "usart3.h"
#include "lora.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "stdlib.h"
#include "led.h"
#include "FreeRTOS.h" //FreeRTOSʹ��
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

//������LORAͨ��

//���ڽ��ջ�����
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
u8 check_cmd_result = 0;
u8 cmd_flag;
//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART3_RX_STA = 0;
u8 ReceiveData_Com = 0; //�ж��Ƿ�������ַ���
u8 str_len, s1;
u8 TIM4_EN_COUN = 0;
u8 TIM4_EN_COUN_NUM = 0;

extern EventGroupHandle_t EventGroup_Handler; //�¼���־����

void USART3_IRQHandler(void)
{
	u8 res;

	// BaseType_t xHigherPriorityTaskWoken, err;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
	{
		res = USART_ReceiveData(USART3);
		if ((USART3_RX_STA & (1 << 15)) == 0) //�������һ������,��û�б�����,���ٽ�����������
		{

			if (USART3_RX_STA < USART3_MAX_RECV_LEN) //�����Խ�������

			{
				TIM_SetCounter(TIM4, 0); //��������գ���ʱ��4��ʼ��һ�ζ�ʱ

				if (USART3_RX_STA == 0) //ʹ�ܶ�ʱ��7���ж�

				{
					TIM_Cmd(TIM4, ENABLE); //ʹ�ܶ�ʱ��7
				}

				USART3_RX_BUF[USART3_RX_STA++] = res; //��¼���յ���ֵ
			}
			else
			{
				USART3_RX_STA |= 1 << 15; //ǿ�Ʊ�ǽ������
			}
		}
		/* else if ((USART3_RX_STA & 0x8000))
		{
			if(strstr((char*)USART3_RX_BUF, "OK")!= NULL)//BUF�а���OK����ֵ��ΪNULL
			{
				Lora_Ok_Flag = 1;
			}
			else if(strstr((char*)USART3_RX_BUF, "ERROR")!= NULL)
			{
				Lora_Ok_Flag = 2;
			}
			else
			{
				printf("unknown error(%s)\r\n", USART3_RX_BUF);
			}
		} */
	}
}

// void USART3_IRQHandler(void)
// {
// /* 	u8 res, t, len ;
// 	LED0 =~ LED0;
// 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
// 	{

// 		res = USART_ReceiveData(USART3);
// 		//USART_SendD++ata(USART1, res);

// 		USART3_RX_BUF[str_len] = res;
// 		//�����յ�'O', 'K', 'R'ʱ��ʾ�������,���ڽ�β����\r\n
// 		if(res == 'O')
// 		{
// 			s1=1;
// 		}
// 		if(res == 'K'&&s1 == 1)
// 		{
// 			cmd_flag = 1;
// 			s1 = 0;

// 			ReceiveData_Com = 1;
// 			USART3_RX_BUF[str_len+1] = '\r';
// 			USART3_RX_BUF[str_len+2] = '\n';
// 		}
// 		else if(res == 'R'&& s1 == 1)
// 		{
// 			s1 = 0;
// 			cmd_flag = 2;
// 			ReceiveData_Com = 1;
// 			USART3_RX_BUF[str_len+1] = '\r';
// 			USART3_RX_BUF[str_len+2] = '\n';
// 		}
// 		if(res == '$'&&s1 == 0)
// 		{
// 			s1 = 3;

// 			USART3_RX_BUF[str_len+1] = '\r';
// 			USART3_RX_BUF[str_len+2] = '\n';
// 		}
// 		str_len++;
// 	} */
// 	//USART_ClearITPendingBit(USART3, USART_IT_RXNE);
// 	u8 res;
// 	res =USART_ReceiveData(USART3);

// 	if((USART3_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
// 	{
// 		if(USART3_RX_STA //�����Խ�������
// 		{
// 			TIM_SetCounter(TIM4, 0); //��������գ���ʱ��7��ʼ��һ�ζ�ʱ
// 			if (USART3_RX_STA == 0) //ʹ�ܶ�ʱ��7���ж�
// 			{
// 				TIM_Cmd(TIM4, ENABLE); //ʹ�ܶ�ʱ��7
// 			}
// 			USART3_RX_BUF[USART3_RX_STA++] = res; //��¼���յ���ֵ
// 		}
// 		else
// 		{
// 			USART3_RX_STA |= 1 << 15; //ǿ�Ʊ�ǽ������

// 		}
// 	}
// 	if (USART3_RX_STA & 0x8000)
// 	{
// 		printf("����:%d, �ַ���:%s\r\n", cisu, USART3_RX_BUF);
// 		USART3_RX_STA = 0;
// 		memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
// 		// printf("����:%d, �ַ���:%s\r\n", cisu, USART3_RX_BUF);
// 	}

// 	//USART_SendData(USART1, Res);
// 	// ����������¼���־�鲻Ϊ��
// 	// if (USART3_RX_STA & 0x8000)
// 	// {
// 	// 	printf("���յ���:%s\r\n", USART3_RX_BUF);
// 	// 	if( EventGroup_Handler != NULL)
// 	// 	{
// 	// 		if (strcmp((char *)USART3_RX_BUF, STARTCMD) == 0)
// 	// 		{
// 	// 			//�¼���־���0λ��1
// 	// 			xEventGroupSetBitsFromISR(EventGroup_Handler, 1 << 0, &xHigherPriorityTaskWoken);
// 	// 		}
// 	// 		if (strcmp((char *)USART3_RX_BUF, "OK") == 0)
// 	// 		{
// 	// 			Lora_Ok_Flag = 1;
// 	// 		}
// 	// 		if (strcmp((char *)USART3_RX_BUF, "ERROR") == 0)
// 	// 		{
// 	// 			Lora_Ok_Flag = 2;
// 	// 		}
// 	// 		USART3_RX_STA = 0;
// 	// 		memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
// 	// 		portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //�����Ҫ�Ļ�����һ�������л�
// 	// 	}
// 	// 	else
// 	// 	{
// 	// 		printf("�¼���־��յ�\r\n");
// 	// 	}
// 	// }
// 	// else
// 	// {
// 	// 	printf("����δ���\r\n");
// 	// }
// 	// Lora_Ok_Flag = 0;							      //����δ�����0;
// 	// USART_ClearITPendingBit(USART3, USART_IT_RXNE);
// }

u8 Check_LORA_Return_Is_OK(void)
{
	// u8 len;
	// len = str_len; // USART3_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
	str_len = 0;
	/* if (ReceiveData_Com == 1)
	{
		printf("str1:%s\r\n", USART3_RX_BUF);
		//		printf("%c %c %c %c\r\n", USART3_RX_BUF[0], USART3_RX_BUF[1],USART3_RX_BUF[len-2], USART3_RX_BUF[len-1]);

		if (USART3_RX_BUF[len - 4] == 'O' && USART3_RX_BUF[len - 3] == 'K')
		{
			//			printf("\r\nlen:%d\r\n",len);
			printf("1%s\r\n", USART3_RX_BUF);
			USART3_RX_STA = 0;
			ReceiveData_Com = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			return 1;
		}
		else // if(USART3_RX_BUF[len-2] == 'O'&&USART3_RX_BUF[len-1] == 'R')
		{
			printf("\r\nlen:%d\r\n", len);
			printf("2%c %c %s\r\n", USART3_RX_BUF[len - 4], USART3_RX_BUF[len - 3], USART3_RX_BUF);
			USART3_RX_STA = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			ReceiveData_Com = 0;
			//���մ���
			return 2;
		} */

	if (Lora_Ok_Flag == 1)
	{
		// ���յ���ȷ����
		Lora_Ok_Flag = 0;
		// USART3_RX_STA = 0;
		// memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		return 1;
	}
	else if (Lora_Ok_Flag == 2)
	{
		//���մ���
		Lora_Ok_Flag = 0;
		// USART3_RX_STA = 0;
		// memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		return 2;
	}
	else if (Lora_Ok_Flag == 0)
	{
		// ����δ���
		Lora_Ok_Flag = 0;
		// USART3_RX_STA = 0;
		// memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		return 0;
	}
	return 0;
	/* 	}
		else
		{
			printf("\r\nstr2:%s\r\n", USART3_RX_BUF);
			USART3_RX_STA = 0;
			memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
			return 0;
		} */
	//	printf("%c %c %s", USART3_RX_BUF[len-2], USART3_RX_BUF[len-1],USART3_RX_BUF);
	//	memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
	//	USART3_RX_STA = 0;
	//	ReceiveData_Com = 0;
}

//��ʼ��IO ����3
// pclk1:PCLK1ʱ��Ƶ��(Mhz)
// bound:������
void usart3_init(u32 bound)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);						//����3ʱ��ʹ��
	/* Enable the USART3 Pins Software Remapping */
	//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO ,ENABLE);
	//    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_DeInit(USART3);					   //��λ����3
											   // USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��PB10

	// USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);				  //��ʼ��PB11

	USART_InitStructure.USART_BaudRate = bound;										//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure); //��ʼ������	3

	USART_Cmd(USART3, ENABLE); //ʹ�ܴ���

	//ʹ�ܽ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //�����ж�

	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8; //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  //����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART3_RX_STA = 0; //����
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char *fmt, ...) //...��ʾ�ɱ����������ɱ�������һ���б�������ר�ŵ�ָ��ָ�����������޶�����������
{
	u16 i, j;
	va_list ap;								  //��ʼ��ָ��ɱ�����б��ָ��
	va_start(ap, fmt);						  //����һ���ɱ�����ĵ�ַ����ap����apָ��ɱ�����б�Ŀ�ʼ
	vsprintf((char *)USART3_TX_BUF, fmt, ap); //������fmt��apָ��Ŀɱ����һ��ת���ɸ�ʽ���ַ�������(char*)USART3_TX_BUF�����У�������ͬsprintf������ֻ�ǲ������Ͳ�ͬ
	va_end(ap);
	i = strlen((const char *)USART3_TX_BUF); //�˴η������ݵĳ���
	for (j = 0; j < i; j++)					 //ѭ����������
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			;									  //ѭ������,ֱ���������
		USART_SendData(USART3, USART3_TX_BUF[j]); //�Ѹ�ʽ���ַ����ӿ����崮���ͳ�ȥ
	}
}

/*����һ���ֽ�����*/
void u3_send_byte(unsigned char SendData)
{
	USART_SendData(USART3, SendData);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		;
}

/*����һ���ֽ�����*/
unsigned char u3_get_byte(unsigned char *GetData)
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
	{
		return 0; //û���յ�����
	}
	*GetData = USART_ReceiveData(USART3);
	return 1; //�յ�����
}

void USART3_RX_Data(void)
{
	u16 len = 0;
	if (USART3_RX_STA & 0x8000)
	{
		len = USART3_RX_STA & 0X7FFF; //�õ��˴ν��յ������ݳ���
		USART3_RX_BUF[len] = 0;		  //���������

		if (len > USART3_MAX_RECV_LEN - 2)
		{
			len = USART3_MAX_RECV_LEN - 1;
			USART3_RX_BUF[len] = 0; //���������
		}

		USART3_RX_BUF[USART3_MAX_RECV_LEN - 1] = 0x01;
		//			u3_printf("%s\r\n",USART3_RX_BUF);
		USART3_RX_STA = 0;
	}
}
