#include "lora.h"
#include "usart3.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

char lora_tx_data[100];
char lora_rx_data[100];
u8 Lora_Ok_Flag = 0;

u8 LORA_Init(void)
{
	//u8 len, result;
	//u8 cmd[] = "AT";  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOB时钟
	
	//MD0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);

	
	//AUX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	
	LORA_Check();
	LORA_Set_Mode();
	
//	while(check_cmd_result == 0)
//	{
//		To_Configure("AT","OK");
//		delay_xms(10);
//		if(t == 100)
//		{
//			printf("没有找到lora\r\n");
//			return 99;
//		}
//		t++;
//	}
//	To_Configure("AT+ADDR=00,00","OK");
	return 1;
}

u8 AUX_Check()
{
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))
	{
		//返回1表示正忙
		return 1;
	}
	else
	{
		//返回0表示不忙
		return 0;
	}
}

void LORA_Check()
{
	int LORA_State = 0;
	while(AUX_Check())
	{
		printf("LORA_Check正忙\r\n");
		delay_xms(50);
	}
	
	//进入配置模式
	MD0 = 1;
	AUX = 0;
	printf("检测LORA是否在线\r\n");
	delay_xms(20);
	while(!LORA_State)
	{
		printf("发送:AT\r\n");
		u3_printf("AT\r\n");
		delay_xms(200);
		LORA_State = Check_LORA_Return_Is_OK();
		//Lora_Ok_Flag = 0;
		printf("Lora_State:%d\r\n", LORA_State);
		if(LORA_State == 2)
		{
			printf("接收到ERROR\r\n");
			LORA_State = 0;
		}
		else if(LORA_State == 0)
		{
			printf("还未接收完\r\n");
		}
		delay_xms(1500);
	}
	printf("LORA检测成功\r\n");
}

void LORA_Set_Mode()
{
	
	//检测是否正忙
	while(AUX_Check())
	{
		printf("LORA_Set_Mode正忙\r\n");
		delay_xms(20);
	}
	
	//进入配置模式
	MD0 = 1;
	AUX = 0;
	delay_xms(10);
	
	//设置地址
	printf("\r\n设置地址:");
	Send_Cmd("AT+ADDR=00,00");
	delay_xms(12);
	//读取设置后的地址
	printf("\r\n读取地址:");
	Quer_Cmd("AT+ADDR?");
	delay_xms(12);
	
	/**设置信道和速率**/
	printf("\r\n设置信道及速率:");
	Send_Cmd("AT+WLRATE=23,5");
	delay_xms(12);

	
	/**读取信道**/
	printf("\r\n读取信道:");
	Quer_Cmd("AT+WLRATE?");
	delay_xms(12);
	delay_xms(12);

	
	/**配置发射功率**/
	printf("\r\n配置发射功率:");
	Send_Cmd("AT+TPOWER=3");
	delay_xms(12);
	/**查询发射功率**/
	printf("\r\n查询发射功率:");
	Quer_Cmd("AT+TPOWER?");
	delay_xms(12);

	/**配置工作模式**/
	printf("\r\n配置工作模式");
	Send_Cmd("AT+CWMODE=0");
	delay_xms(12);

	/**查询工作模式**/
	printf("\r\n查询工作模式");
	Quer_Cmd("AT+CWMODE?");
	delay_xms(12);
	
	/**配置发送状态**/
	printf("\r\n配置发送状态");
	Send_Cmd("AT+TMODE=1");
	delay_xms(12);

	/**查询发送状态**/
	printf("\r\n查询发送状态");
	Quer_Cmd("AT+TMODE?");
	delay_xms(12);
	
	/**配置睡眠时间**/
	printf("\r\n配置睡眠时间");
	Send_Cmd("AT+WLTIME=0");
	delay_xms(12);

	/**查询睡眠时间**/
	printf("\r\n查询睡眠时间");
	Quer_Cmd("AT+WLTIME?");
	delay_xms(12);

	/**配置波特率，数据校验位**/
	printf("\r\n配置波特率，数据校验位");
	Send_Cmd("AT+UART=7,0");
	delay_xms(12);

	/**查询波特率，数据校验位**/
	printf("\r\n查询波特率，数据校验位");
	Quer_Cmd("AT+UART?");
	delay_xms(12);

	/**配置保存模式**/
	printf("\r\n配置保存模式");
	Send_Cmd("AT+FLASH=0");
	delay_xms(12);
	

	
	printf("\r\n**********lora配置完成************\r\n");
	
	//进入通信模式
	MD0 = 0;
	AUX = 0;
}

//设置
void Send_Cmd(u8*cmd)
{
	u8 LORA_State = 0;
	
	LORA_State = 0;
	delay_xms(10);
	while(!LORA_State)
	{
		u3_printf("%s\r\n", cmd);
		delay_xms(20);
		LORA_State = Check_LORA_Return_Is_OK();
		printf("Lora_State:%d\r\n", LORA_State);
		if(LORA_State == 2)
		{
			printf("设置时接收到ERROR\r\n");
			LORA_State = 0;
		}
		else if(LORA_State == 0)
		{
			printf("还未接收完\r\n");
		}
	}
}

//查询
void Quer_Cmd(u8 *cmd)
{
	u8 LORA_State = 0;
	
	LORA_State = 0;
	delay_xms(10);
	while(!LORA_State)
	{
		//printf("\r\n信道为:");
		u3_printf("%s\r\n",cmd);
		delay_xms(20);
		LORA_State = Check_LORA_Return_Is_OK();
		printf("Lora_State:%d\r\n", LORA_State);
		if(LORA_State == 2)
		{
			printf("查询时接收到ERROR\r\n");
			LORA_State = 0;
		}
		else if(LORA_State == 0)
		{
			printf("查询还未接收完\r\n");
		}
	}
}


//发送数据
void  Send_Data(u8 addh, u8 addl,u8 channel,u8 *a)
{
	int len = 0, i = 0;
	
	u3_send_byte(addh);
	u3_send_byte(addl);
	u3_send_byte(channel);
	len = strlen((char*)a);
	while(i <= len)
	{
		// printf("send data, i:%d\r\n", i);
		u3_send_byte(a[i]);
		i++;
	}
	/* u3_send_byte('$');//结束符
	u3_send_byte('$');//结束符 */
	u3_send_byte('\r');
	u3_send_byte('\n');
}

void Receive_Data(void)
{
	unsigned char i = 0;
	int t1 = 0,t2 = 0;
	while(1)
	{
		u3_get_byte(&i);
		if(i ==0xff)
		{
			while(1)
			{
				if(u3_get_byte(&i) == 1 && i!='$')
				{
					lora_rx_data[t1++]=i;
				}
				else if(i=='$')
					break;	 
			}
			while(1)
			{
					u1_send_byte(lora_rx_data[t2++]);
					if(t1 == t2)
					{
						u1_send_byte('\r');
						u1_send_byte('\n');
						break;
					}
			}
			break;
		} 
	}
}
//进入配置模式
void To_Configure(u8 *cmd, u8 *str)
{
	MD0 = 1;
	AUX = 0;
	
	u3_printf("%s\r\n",cmd);
}
