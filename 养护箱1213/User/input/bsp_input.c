/**
  ******************************************************************************
  * @file    bsp_input.c
  * @author  xzk
  * @version V1.0
  * @date    2015-03-18
  * @brief   input应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:奋斗v5 开发板 
  * 论坛    :
  * 淘宝    :
  *
  ******************************************************************************
  */
  
#include "bsp_input.h"   

u8 key_input_before;//按键去抖动辅助

/*开关输入变量key_input
bit	0:设定
		1:存储
		2:+
		3:-
		8：key按住不放，蜂鸣器标志
*/
u8 key_input=0;//有效按键

 /**
  * @brief  初始化控制key的IO
  * @param  无
  * @retval 无
  */
void GPIO_INPUT_INIT(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIOABCD的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = KEY_ADD|KEY_SUB|KEY_SAVE;	

		//设置引脚模式为输入下拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;

		/*调用库函数，初始化GPIOC5 6*/
		GPIO_Init(KEYPORT, &GPIO_InitStructure);			
	
	/*选择要控制的GPIOD引脚*/															   
		GPIO_InitStructure.GPIO_Pin = KEY_SET;	
	
	/*调用库函数，初始化GPIOD12*/
		GPIO_Init(GPIOD, &GPIO_InitStructure);		
	
	//就地键PA12,0地1网络
	/*选择要控制的GPIOA引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	
	
	/*调用库函数，初始化GPIOA12*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

	//就地启动停止键PC3,20150719
	/*选择要控制的GPIOC引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
	
	/*调用库函数，初始化GPIOC3*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief   检测是否有按键按下
  * @param   具体的端口和端口位
  *		@arg GPIOx: x可以是（A...G） 
  *		@arg GPIO_PIN 可以是GPIO_PIN_x（x可以是1...16）
  *   @arg Down_state:按键按下时的电平，1为高电平，0为低电平
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{			
	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state ) 
	{	   		
			return KEY_ON;
	}
	else
		return KEY_OFF;
}

 /**
  * @brief  按键检测
  * @param  无
  * @retval 无
  */
void GPIO_INPUT_SCAN(void)
{
	u8 key_input_temp;
	key_input_temp=0;
	/*检测KEY_SET*/
	if( Key_Scan(GPIOD,KEY_SET,1) == KEY_ON  )
	{
		/*按键有效*/
		key_input_temp+=1;
		
	}else
	/*检测KEY_SAVE*/
	if( Key_Scan(KEYPORT,KEY_SAVE,1) == KEY_ON  )
	{
		/*按键有效*/
		key_input_temp+=2;
		
	}else
	/*检测KEY_ADD*/
	if( Key_Scan(KEYPORT,KEY_ADD,1) == KEY_ON  )
	{
		/*按键有效*/
		key_input_temp+=4;
		
	}else
	/*检测KEY_SUB*/
	if( Key_Scan(KEYPORT,KEY_SUB,1) == KEY_ON  )
	{
		/*按键有效*/
		key_input_temp+=8;
		
	}
	if(key_input_temp==key_input_before)//消抖，每次大循环都执行该子函数，一个循环建议在30-100ms
	{
		key_input=key_input_temp;
		//key按住不放，也是蜂鸣器响的标志
	}else
	{
		key_input=0;//按键无效
	}
	key_input_before=key_input_temp;
}

/*********************************************END OF FILE**********************/
