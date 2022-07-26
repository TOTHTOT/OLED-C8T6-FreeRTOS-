#include "delay.h"
#include "usart3.h"
#include "lora.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "stdlib.h"
#include "led.h"
#include "FreeRTOS.h" //FreeRTOS使用
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

//主机和LORA通信

//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
u8 check_cmd_result = 0;
u8 cmd_flag;
//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA = 0;
u8 ReceiveData_Com = 0; //判断是否接收完字符串
u8 str_len, s1;
u8 TIM4_EN_COUN = 0;
u8 TIM4_EN_COUN_NUM = 0;

extern EventGroupHandle_t EventGroup_Handler; //事件标志组句柄

void USART3_IRQHandler(void)
{
	u8 res;

	// BaseType_t xHigherPriorityTaskWoken, err;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{
		res = USART_ReceiveData(USART3);
		if ((USART3_RX_STA & (1 << 15)) == 0) //接收完的一批数据,还没有被处理,则不再接收其他数据
		{

			if (USART3_RX_STA < USART3_MAX_RECV_LEN) //还可以接收数据

			{
				TIM_SetCounter(TIM4, 0); //计数器清空，定时器4开始下一次定时

				if (USART3_RX_STA == 0) //使能定时器7的中断

				{
					TIM_Cmd(TIM4, ENABLE); //使能定时器7
				}

				USART3_RX_BUF[USART3_RX_STA++] = res; //记录接收到的值
			}
			else
			{
				USART3_RX_STA |= 1 << 15; //强制标记接收完成
			}
		}
		// else
		// 接受完成
		// if ((USART3_RX_STA & 0x8000))
		// {

		// }
	}
}

// void USART3_IRQHandler(void)
// {
// /* 	u8 res, t, len ;
// 	LED0 =~ LED0;
// 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
// 	{

// 		res = USART_ReceiveData(USART3);
// 		//USART_SendD++ata(USART1, res);

// 		USART3_RX_BUF[str_len] = res;
// 		//当接收到'O', 'K', 'R'时表示接收完毕,并在结尾补上\r\n
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

// 	if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
// 	{
// 		if(USART3_RX_STA //还可以接收数据
// 		{
// 			TIM_SetCounter(TIM4, 0); //计数器清空，定时器7开始下一次定时
// 			if (USART3_RX_STA == 0) //使能定时器7的中断
// 			{
// 				TIM_Cmd(TIM4, ENABLE); //使能定时器7
// 			}
// 			USART3_RX_BUF[USART3_RX_STA++] = res; //记录接收到的值
// 		}
// 		else
// 		{
// 			USART3_RX_STA |= 1 << 15; //强制标记接收完成

// 		}
// 	}
// 	if (USART3_RX_STA & 0x8000)
// 	{
// 		printf("次数:%d, 字符串:%s\r\n", cisu, USART3_RX_BUF);
// 		USART3_RX_STA = 0;
// 		memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
// 		// printf("次数:%d, 字符串:%s\r\n", cisu, USART3_RX_BUF);
// 	}

// 	//USART_SendData(USART1, Res);
// 	// 接收完成且事件标志组不为空
// 	// if (USART3_RX_STA & 0x8000)
// 	// {
// 	// 	printf("接收到的:%s\r\n", USART3_RX_BUF);
// 	// 	if( EventGroup_Handler != NULL)
// 	// 	{
// 	// 		if (strcmp((char *)USART3_RX_BUF, STARTCMD) == 0)
// 	// 		{
// 	// 			//事件标志组第0位置1
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
// 	// 		portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
// 	// 	}
// 	// 	else
// 	// 	{
// 	// 		printf("事件标志组空的\r\n");
// 	// 	}
// 	// }
// 	// else
// 	// {
// 	// 	printf("接收未完成\r\n");
// 	// }
// 	// Lora_Ok_Flag = 0;							      //接收未完成置0;
// 	// USART_ClearITPendingBit(USART3, USART_IT_RXNE);
// }

u8 Check_LORA_Return_Is_OK(void)
{
	// u8 len;
	// len = str_len; // USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
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
			//接收错误
			return 2;
		} */

	if (Lora_Ok_Flag == 1)
	{
		// 接收到正确内容
		Lora_Ok_Flag = 0;
		// USART3_RX_STA = 0;
		// memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		return 1;
	}
	else if (Lora_Ok_Flag == 2)
	{
		//接收错误
		Lora_Ok_Flag = 0;
		// USART3_RX_STA = 0;
		// memset(USART3_RX_BUF, '\0', USART3_MAX_RECV_LEN);
		return 2;
	}
	else if (Lora_Ok_Flag == 0)
	{
		// 接收未完成
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

//初始化IO 串口3
// pclk1:PCLK1时钟频率(Mhz)
// bound:波特率
void usart3_init(u32 bound)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);						//串口3时钟使能
	/* Enable the USART3 Pins Software Remapping */
	//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO ,ENABLE);
	//    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_DeInit(USART3);					   //复位串口3
											   // USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化PB10

	// USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);				  //初始化PB11

	USART_InitStructure.USART_BaudRate = bound;										//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口	3

	USART_Cmd(USART3, ENABLE); //使能串口

	//使能接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启中断

	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  //根据指定的参数初始化VIC寄存器

	USART3_RX_STA = 0; //清零
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char *fmt, ...) //...表示可变参数（多个可变参数组成一个列表，后面有专门的指针指向他），不限定个数和类型
{
	u16 i, j;
	va_list ap;								  //初始化指向可变参数列表的指针
	va_start(ap, fmt);						  //将第一个可变参数的地址付给ap，即ap指向可变参数列表的开始
	vsprintf((char *)USART3_TX_BUF, fmt, ap); //将参数fmt、ap指向的可变参数一起转换成格式化字符串，放(char*)USART3_TX_BUF数组中，其作用同sprintf（），只是参数类型不同
	va_end(ap);
	i = strlen((const char *)USART3_TX_BUF); //此次发送数据的长度
	for (j = 0; j < i; j++)					 //循环发送数据
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			;									  //循环发送,直到发送完毕
		USART_SendData(USART3, USART3_TX_BUF[j]); //把格式化字符串从开发板串口送出去
	}
}

/*发送一个字节数据*/
void u3_send_byte(unsigned char SendData)
{
	USART_SendData(USART3, SendData);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		;
}

/*接收一个字节数据*/
unsigned char u3_get_byte(unsigned char *GetData)
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
	{
		return 0; //没有收到数据
	}
	*GetData = USART_ReceiveData(USART3);
	return 1; //收到数据
}

void USART3_RX_Data(void)
{
	u16 len = 0;
	if (USART3_RX_STA & 0x8000)
	{
		len = USART3_RX_STA & 0X7FFF; //得到此次接收到的数据长度
		USART3_RX_BUF[len] = 0;		  //加入结束符

		if (len > USART3_MAX_RECV_LEN - 2)
		{
			len = USART3_MAX_RECV_LEN - 1;
			USART3_RX_BUF[len] = 0; //加入结束符
		}

		USART3_RX_BUF[USART3_MAX_RECV_LEN - 1] = 0x01;
		//			u3_printf("%s\r\n",USART3_RX_BUF);
		USART3_RX_STA = 0;
	}
}
