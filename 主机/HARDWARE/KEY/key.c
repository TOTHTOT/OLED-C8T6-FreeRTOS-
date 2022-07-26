
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

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTCʱ��

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1||GPIO_Pin_2||GPIO_Pin_3||GPIO_Pin_4;//PA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC5
	
	/* GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.0 */
	
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
		delay_xms(10			); //ȥ����
		if (BUTTON1 == 0)
			return BUTTON1_PRESS;
	}
	else if (BUTTON2 == 0)
	{
		delay_xms(10); //ȥ����
		if (BUTTON2 == 0)
			return BUTTON2_PRESS;
	}
	else if (BUTTON3 == 0)
	{
		delay_xms(10); //ȥ����
		if (BUTTON3 == 0)
			return BUTTON3_PRESS;
	}
	else if (BUTTON4 == 0)
	{
		delay_xms(10); //ȥ����
		if (BUTTON4 == 0)
			return BUTTON4_PRESS;
	} */

	return 0; // �ް�������
}


