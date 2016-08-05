/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  //���ļ�������ҪΪ3���̵���������ã����������ã��ĸ�led������gpio
#include "io_output.h" 
#include "..\User\dht11\bsp_dht11.h"

/*��ʪ�ȱ���*/
extern value humi_val,temp_val;

/*��ʪ���趨ֵ*/
extern u8 humi_set;
extern u8 temp_set;
extern u8 humi_distance;//�¶�ԣ��
extern u8 temp_distance;//ʪ��ԣ��
extern u8 work_state;
u8 wet_flag=0;//��ʪ��ʶ
extern u8 active;
extern u32 time_s_remainder;
extern DHT11_Data_TypeDef DHT11_Data;
 /**
  * @brief  ��ʼ������output��IO
  * @param  ��
  * @retval ��
  */
void IO_OUTPUT_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOA��GPIOB,GPIOC,GPIOE������ʱ��*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE, ENABLE); 

		/*ѡ��Ҫ���Ƶ�GPIOB����,�����ʪ*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8;

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIOE*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
		/*ѡ��Ҫ���Ƶ�GPIOE����,���ȼ�ʪ*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4;
		
		/*���ÿ⺯������ʼ��GPIOE*/
		GPIO_Init(GPIOE, &GPIO_InitStructure);	
		
				/*ѡ��Ҫ���Ƶ�GPIOA����,BUZ*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
		
		/*���ÿ⺯������ʼ��GPIOA*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		/*ѡ��Ҫ���Ƶ�GPIOC����,ENC*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		
		/*���ÿ⺯������ʼ��GPIOC*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		/*ѡ��Ҫ���Ƶ�GPIOB����,ENB,ENA*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
		
		/*���ÿ⺯������ʼ��GPIOB*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		//�رռ��£����䣬��ʪ�̵�����ָʾ�ƣ��رշ������ͺ�ʪ��
		WARM(0);
		WARM_LED(0);
		COLD(0);
		COLD_LED(0);
		WET(0);
		WET_LED(0);
		ALWAYS_WET_LED(0);
		BEEP_SCREAM(0);
		 
}

/*�Զ�������ʪ�ȼ�����״̬*/
void automation(void)
{
	if((active=='a')||(time_s_remainder>0))
	{
		//�¶ȿ���1213
		if(DHT11_Data.temp_int<(float)temp_set-1-((float)temp_distance)/10)
		{
			//����hw��pb5
			WARM(1);
			WARM_LED(1);
			work_state|=1;
			COLD(0);
			COLD_LED(0);
			work_state&=~4;
		}else if(DHT11_Data.temp_int>(float)temp_set+1+((float)temp_distance)/10)
		{
			//����jw,pe0
			WARM(0);
			WARM_LED(0);
			work_state&=~1;
			COLD(1);
			COLD_LED(1);
			work_state|=4;
		}else//�رռ̵�����״̬��
		{
			WARM(0);
			WARM_LED(0);
			work_state&=~1;
			COLD(0);
			COLD_LED(0);
			work_state&=~4;
		}
		
		//ʪ�ȿ���
		if(humi_val.f<(float)humi_set-(float)humi_distance)
		{
			//��ʪjs��pc13
			wet_flag=1;
			
		}
		//1213ʪ�ȳ���90
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
