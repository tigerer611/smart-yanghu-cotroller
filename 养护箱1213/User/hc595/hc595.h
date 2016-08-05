#ifndef __HC595_H
#define __HC595_H
#include "stm32f10x.h"
// 74HC595
#define HC595_PORT  GPIOD
#define HC595_DATA  GPIO_Pin_3    // serial data input, pin14
#define HC595_SCLK  GPIO_Pin_2    // shift register clock input, pin11
#define HC595_RCLK  GPIO_Pin_6     // storage register clock input, pin12

//#define HC595_PORT10  GPIOC
//#define HC595_DATA10  GPIO_Pin_10    // serial data input, pin14
//#define HC595_SCLK10  GPIO_Pin_12    // shift register clock input, pin11
//#define HC595_RCLK10  GPIO_Pin_11    // storage register clock input, pin12


#define HC595_DELAY delay_us(100);

void hc595_init(void);
void hc595_write_byte(uint8_t byte);
void hc595_write_byte10(uint16_t byte);
void hc595_latch(void);
void hc595_display(uint8_t data[]);
void display_test(void);

#endif
