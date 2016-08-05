#include "stm32f10x.h"

typedef union 
{  u16 i;
  float f;
	u8 active;//温湿度读取成功标志
} value;
enum {TEMP,HUMI};

void Sht_GPIO_Config(void);
void SDA_SET_IN(void);
void SCL_OUT(u8 out);
void SDA_OUT(u8 out);
u8 SDA_READ(void);
float calc_dewpoint(float h,float t);
void calc_sth11(float *p_humidity ,float *p_temperature);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
char s_write_statusreg(unsigned char *p_value);
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum);
char s_softreset(void);
void s_connectionreset(void);
void s_transstart(void);
char s_read_byte(unsigned char ack);
char s_write_byte(unsigned char value);
void sht10_read(void);
void sht10_read1(void);
