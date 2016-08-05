/**
  ******************************************************************************
  * @file    bsp_input.c
  * @author  xzk
  * @version V1.0
  * @date    2015-03-18
  * @brief   inputӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:�ܶ�v5 ������ 
  * ��̳    :
  * �Ա�    :
  *
  ******************************************************************************
  */
  
#include "bsp_input.h"   

u8 key_input_before;//����ȥ��������

/*�����������key_input
bit	0:�趨
		1:�洢
		2:+
		3:-
		8��key��ס���ţ���������־
*/
u8 key_input=0;//��Ч����

 /**
  * @brief  ��ʼ������key��IO
  * @param  ��
  * @retval ��
  */
void GPIO_INPUT_INIT(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOABCD������ʱ��*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE); 

		/*ѡ��Ҫ���Ƶ�GPIOB����*/															   
		GPIO_InitStructure.GPIO_Pin = KEY_ADD|KEY_SUB|KEY_SAVE;	

		//��������ģʽΪ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;

		/*���ÿ⺯������ʼ��GPIOC5 6*/
		GPIO_Init(KEYPORT, &GPIO_InitStructure);			
	
	/*ѡ��Ҫ���Ƶ�GPIOD����*/															   
		GPIO_InitStructure.GPIO_Pin = KEY_SET;	
	
	/*���ÿ⺯������ʼ��GPIOD12*/
		GPIO_Init(GPIOD, &GPIO_InitStructure);		
	
	//�͵ؼ�PA12,0��1����
	/*ѡ��Ҫ���Ƶ�GPIOA����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	
	
	/*���ÿ⺯������ʼ��GPIOA12*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

	//�͵�����ֹͣ��PC3,20150719
	/*ѡ��Ҫ���Ƶ�GPIOC����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
	
	/*���ÿ⺯������ʼ��GPIOC3*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief   ����Ƿ��а�������
  * @param   ����Ķ˿ںͶ˿�λ
  *		@arg GPIOx: x�����ǣ�A...G�� 
  *		@arg GPIO_PIN ������GPIO_PIN_x��x������1...16��
  *   @arg Down_state:��������ʱ�ĵ�ƽ��1Ϊ�ߵ�ƽ��0Ϊ�͵�ƽ
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{			
	/*����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state ) 
	{	   		
			return KEY_ON;
	}
	else
		return KEY_OFF;
}

 /**
  * @brief  �������
  * @param  ��
  * @retval ��
  */
void GPIO_INPUT_SCAN(void)
{
	u8 key_input_temp;
	key_input_temp=0;
	/*���KEY_SET*/
	if( Key_Scan(GPIOD,KEY_SET,1) == KEY_ON  )
	{
		/*������Ч*/
		key_input_temp+=1;
		
	}else
	/*���KEY_SAVE*/
	if( Key_Scan(KEYPORT,KEY_SAVE,1) == KEY_ON  )
	{
		/*������Ч*/
		key_input_temp+=2;
		
	}else
	/*���KEY_ADD*/
	if( Key_Scan(KEYPORT,KEY_ADD,1) == KEY_ON  )
	{
		/*������Ч*/
		key_input_temp+=4;
		
	}else
	/*���KEY_SUB*/
	if( Key_Scan(KEYPORT,KEY_SUB,1) == KEY_ON  )
	{
		/*������Ч*/
		key_input_temp+=8;
		
	}
	if(key_input_temp==key_input_before)//������ÿ�δ�ѭ����ִ�и��Ӻ�����һ��ѭ��������30-100ms
	{
		key_input=key_input_temp;
		//key��ס���ţ�Ҳ�Ƿ�������ı�־
	}else
	{
		key_input=0;//������Ч
	}
	key_input_before=key_input_temp;
}

/*********************************************END OF FILE**********************/
