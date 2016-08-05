/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  //本文件内容主要为3个继电器输出配置，蜂鸣器配置，四个led灯配置gpio
#include "io_output.h" 
#include "..\User\dht11\bsp_dht11.h"

/*温湿度变量*/
extern value humi_val,temp_val;

/*温湿度设定值*/
extern u8 humi_set;
extern u8 temp_set;
extern u8 humi_distance;//温度裕量
extern u8 temp_distance;//湿度裕量
extern u8 work_state;
u8 wet_flag=0;//加湿标识
extern u8 active;
extern u32 time_s_remainder;
extern DHT11_Data_TypeDef DHT11_Data;
 /**
  * @brief  初始化控制output的IO
  * @param  无
  * @retval 无
  */
void IO_OUTPUT_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIOA和GPIOB,GPIOC,GPIOE的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE, ENABLE); 

		/*选择要控制的GPIOB引脚,制冷恒湿*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8;

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOE*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
		/*选择要控制的GPIOE引脚,加热加湿*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4;
		
		/*调用库函数，初始化GPIOE*/
		GPIO_Init(GPIOE, &GPIO_InitStructure);	
		
				/*选择要控制的GPIOA引脚,BUZ*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
		
		/*调用库函数，初始化GPIOA*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		/*选择要控制的GPIOC引脚,ENC*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		
		/*调用库函数，初始化GPIOC*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		/*选择要控制的GPIOB引脚,ENB,ENA*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
		
		/*调用库函数，初始化GPIOB*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		//关闭加温，制冷，加湿继电器及指示灯，关闭蜂鸣器和恒湿灯
		WARM(0);
		WARM_LED(0);
		COLD(0);
		COLD_LED(0);
		WET(0);
		WET_LED(0);
		ALWAYS_WET_LED(0);
		BEEP_SCREAM(0);
		 
}

/*自动控制温湿度及工作状态*/
void automation(void)
{
	if((active=='a')||(time_s_remainder>0))
	{
		//温度控制1213
		if(DHT11_Data.temp_int<(float)temp_set-1-((float)temp_distance)/10)
		{
			//加温hw，pb5
			WARM(1);
			WARM_LED(1);
			work_state|=1;
			COLD(0);
			COLD_LED(0);
			work_state&=~4;
		}else if(DHT11_Data.temp_int>(float)temp_set+1+((float)temp_distance)/10)
		{
			//制冷jw,pe0
			WARM(0);
			WARM_LED(0);
			work_state&=~1;
			COLD(1);
			COLD_LED(1);
			work_state|=4;
		}else//关闭继电器和状态灯
		{
			WARM(0);
			WARM_LED(0);
			work_state&=~1;
			COLD(0);
			COLD_LED(0);
			work_state&=~4;
		}
		
		//湿度控制
		if(humi_val.f<(float)humi_set-(float)humi_distance)
		{
			//加湿js，pc13
			wet_flag=1;
			
		}
		//1213湿度超过90
		if(humi_val.f>=90)
		{
			wet_flag=0;
		}
			
		if(wet_flag)
		{
			WET(1);
			WET_LED(1);
			work_state|=2;
			work_state&=~8;
			ALWAYS_WET_LED(0);
			if(humi_val.f>humi_set)
				wet_flag=0;
		}else
		{
			WET(0);
			WET_LED(0);
			work_state&=~2;
			ALWAYS_WET_LED(1);
			work_state|=8;
		}
	}else
	{
		WARM(0);
		WARM_LED(0);
		work_state&=~1;
		COLD(0);
		COLD_LED(0);
		work_state&=~4;
		WET(0);
		WET_LED(0);
		work_state&=~2;
		ALWAYS_WET_LED(0);
		work_state&=~8;
	}
	
}




/*********************************************END OF FILE**********************/
