#ifndef __IO_OUTPUT_H
#define	__IO_OUTPUT_H

#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
//#define ON  1
//#define OFF 0

/* 带参宏，可以像内联函数一样使用 */
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
					
//蜂鸣器啸叫PA11					//0824//1213禁用蜂鸣器
#define BEEP_SCREAM(a) if(0)\
					GPIO_SetBits(GPIOA,GPIO_Pin_11);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_11)			

//ENA加温PC1					//0824
#define WARM(a) if(!a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_1);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_1)						



//ENB制冷PC0//0824
#define COLD(a) if(!a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_0);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_0)						



//ENC加湿PC13//0824
#define WET(a) if(!a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_13);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_13)						
					
					
					
//JW加温灯PB5//0824
#define WARM_LED(a) if(a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_5)
					
//HW制冷灯PE0//0824
#define COLD_LED(a) if(a)\
					GPIO_SetBits(GPIOE,GPIO_Pin_0);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_0)
					
//JS加湿灯PE4//0824
#define WET_LED(a) if(a)\
					GPIO_SetBits(GPIOE,GPIO_Pin_4);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_4)
					
//HS恒湿灯PB8//0824
#define ALWAYS_WET_LED(a) if(a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_8)
					

/* 直接操作寄存器的方法控制IO */
//#define	digitalHi(p,i)				{p->BSRR=i;}			//设置为高电平		
//#define digitalLo(p,i)				{p->BRR	=i;}				//输出低电平
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */
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
