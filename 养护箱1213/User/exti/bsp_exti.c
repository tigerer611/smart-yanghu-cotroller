/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_exti.h"

/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void NVIC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO , ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	         	 	//ENC28J60接收完成中断引脚 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//内部上拉输入
//  GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         	 	//传感器data拉底电瓶管脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//内部上拉输入
  GPIO_Init(GPIOE, &GPIO_InitStructure);		
	
  /* Configure one bit for preemption priority */
  /* 优先级组 说明了抢占优先级所用的位数，和子优先级所用的位数   在这里是1， 7 */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI2 Interrupt EXTI9_5_IRQHandler*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				 //外部中断2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //抢占优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			 //子优先级0  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //使能
  NVIC_Init(&NVIC_InitStructure);

  //用于配置AFIO外部中断配置寄存器AFIO_EXTICR1，用于选择EXTI8外部中断的输入源是PA8。
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);     //外部中断配置AFIO--ETXI8

//  EXTI_InitStructure.EXTI_Line = EXTI_Line8;					  //PA8 作为NET中断
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //中断模式
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //下降沿触发
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
	
	 //用于配置AFIO外部中断配置寄存器AFIO_EXTICR1，用于选择EXTI5外部中断的输入源是PE5。
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);     //外部中断配置AFIO--ETXI5

  EXTI_InitStructure.EXTI_Line = EXTI_Line5;					  //PE5 作为传感器测量完成
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //上升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	EXTI->IMR&=~(1<<5);//屏蔽中断 传感器
	
	
}
