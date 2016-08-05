#ifndef __INPUT_H
#define	__INPUT_H

#include "stm32f10x.h"
#define KEYPORT GPIOC
#define KEY_SET GPIO_Pin_12
#define KEY_SAVE GPIO_Pin_8
#define KEY_ADD GPIO_Pin_6
#define KEY_SUB GPIO_Pin_7
/*******
 *按键按下标置
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	0
#define KEY_OFF	1

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state);
void GPIO_INPUT_INIT(void);
void GPIO_INPUT_SCAN(void);


#endif /* __INPUT_H */
