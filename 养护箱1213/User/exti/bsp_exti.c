/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_exti.h"

/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void NVIC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO , ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	         	 	//ENC28J60��������ж����� 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//�ڲ���������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         	 	//������data���׵�ƿ�ܽ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//�ڲ���������
  GPIO_Init(GPIOE, &GPIO_InitStructure);		
	
  /* Configure one bit for preemption priority */
  /* ���ȼ��� ˵������ռ���ȼ����õ�λ�����������ȼ����õ�λ��   ��������1�� 7 */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI2 Interrupt EXTI9_5_IRQHandler*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				 //�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     //��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			 //�����ȼ�0  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ��
  NVIC_Init(&NVIC_InitStructure);

  //��������AFIO�ⲿ�ж����üĴ���AFIO_EXTICR1������ѡ��EXTI8�ⲿ�жϵ�����Դ��PA8��
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);     //�ⲿ�ж�����AFIO--ETXI8

//  EXTI_InitStructure.EXTI_Line = EXTI_Line8;					  //PA8 ��ΪNET�ж�
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //�ж�ģʽ
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //�½��ش���
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
	
	 //��������AFIO�ⲿ�ж����üĴ���AFIO_EXTICR1������ѡ��EXTI5�ⲿ�жϵ�����Դ��PE5��
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);     //�ⲿ�ж�����AFIO--ETXI5

  EXTI_InitStructure.EXTI_Line = EXTI_Line5;					  //PE5 ��Ϊ�������������
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //�ж�ģʽ
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //�����ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	EXTI->IMR&=~(1<<5);//�����ж� ������
	
	
}
