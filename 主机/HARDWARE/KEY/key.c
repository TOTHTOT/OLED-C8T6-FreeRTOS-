
#include "key.h"
#include "delay.h"
/*****************************************************************************
 * @name       :void KEY_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization key GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/ 
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTC时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1||GPIO_Pin_2||GPIO_Pin_3||GPIO_Pin_4;//PA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOC5
	
	/* GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.0 */
	
} 

/*****************************************************************************
 * @name       :u8 KEY_Scan(void)
 * @date       :2018-08-09 
 * @function   :Key processing,Response priority:KEY0>KEY1>WK_UP!!
 * @parameters :None
 * @retvalue   :0-No buttons were pressed.
								1-The KEY0 button is pressed
								2-The KEY1 button is pressed
								3-The WK_UP button is pressed
******************************************************************************/
u8 KEY_Scan(void)
{
	/* if (BUTTON1 == 0)
	{
		delay_xms(10			); //去抖动
		if (BUTTON1 == 0)
			return BUTTON1_PRESS;
	}
	else if (BUTTON2 == 0)
	{
		delay_xms(10); //去抖动
		if (BUTTON2 == 0)
			return BUTTON2_PRESS;
	}
	else if (BUTTON3 == 0)
	{
		delay_xms(10); //去抖动
		if (BUTTON3 == 0)
			return BUTTON3_PRESS;
	}
	else if (BUTTON4 == 0)
	{
		delay_xms(10); //去抖动
		if (BUTTON4 == 0)
			return BUTTON4_PRESS;
	} */

	return 0; // 无按键按下
}


