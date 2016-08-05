#ifndef __IO_OUTPUT_H
#define	__IO_OUTPUT_H

#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
//#define ON  1
//#define OFF 0

/* ���κ꣬��������������һ��ʹ�� */
//#define LED1(a)	if (a)	\
//					GPIO_SetBits(GPIOB,GPIO_Pin_5);\
//					else		\
//					GPIO_ResetBits(GPIOB,GPIO_Pin_5)

//#define LED2(a)	if (a)	\
//					GPIO_SetBits(GPIOD,GPIO_Pin_6);\
//					else		\
//					GPIO_ResetBits(GPIOD,GPIO_Pin_6)

//#define LED3(a)	if (a)	\
//					GPIO_SetBits(GPIOD,GPIO_Pin_3);\
//					else		\
//					GPIO_ResetBits(GPIOD,GPIO_Pin_3)
					
//������Х��PA11					//0824//1213���÷�����
#define BEEP_SCREAM(a) if(0)\
					GPIO_SetBits(GPIOA,GPIO_Pin_11);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_11)			

//ENA����PC1					//0824
#define WARM(a) if(!a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_1);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_1)						



//ENB����PC0//0824
#define COLD(a) if(!a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_0);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_0)						



//ENC��ʪPC13//0824
#define WET(a) if(!a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_13);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_13)						
					
					
					
//JW���µ�PB5//0824
#define WARM_LED(a) if(a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_5)
					
//HW�����PE0//0824
#define COLD_LED(a) if(a)\
					GPIO_SetBits(GPIOE,GPIO_Pin_0);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_0)
					
//JS��ʪ��PE4//0824
#define WET_LED(a) if(a)\
					GPIO_SetBits(GPIOE,GPIO_Pin_4);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_4)
					
//HS��ʪ��PB8//0824
#define ALWAYS_WET_LED(a) if(a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_8)
					

/* ֱ�Ӳ����Ĵ����ķ�������IO */
//#define	digitalHi(p,i)				{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
//#define digitalLo(p,i)				{p->BRR	=i;}				//����͵�ƽ
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */
//#define LED1_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_5)
//#define ADD_TEMP			digitalHi(GPIOB,GPIO_Pin_5)
//#define LED1_ON				digitalLo(GPIOB,GPIO_Pin_5)

////#define LED2_TOGGLE		digitalToggle(GPIOD,GPIO_Pin_6)
//#define LED2_OFF			digitalHi(GPIOD,GPIO_Pin_6)
//#define LED2_ON				digitalLo(GPIOD,GPIO_Pin_6)

////#define LED3_TOGGLE		digitalToggle(GPIOD,GPIO_Pin_3)
//#define LED3_OFF			digitalHi(GPIOD,GPIO_Pin_3)
//#define LED3_ON				digitalLo(GPIOD,GPIO_Pin_3)

void IO_OUTPUT_Config(void);
void automation(void);

#endif /* __IO_OUTPUT_H */
