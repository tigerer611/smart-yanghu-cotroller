#include "stm32f10x.h"
#include "SHT10.h"
#include <math.h>

#define SHT10PORT GPIOC
#define SCL GPIO_Pin_8	//6
#define SDA GPIO_Pin_9	//5
#define noACK 0
#define ACK   1
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0
/*温湿度变量*/
value humi_val,temp_val;
/*温湿度测量标志
	0:湿度
	1:温度
	*/


void delay_us(uint32_t nms)
{	
	uint32_t i;
	for(i=nms*15;i>0;i--);	
}

void Sht_GPIO_Config(void)
{
   
  GPIO_InitTypeDef  GPIO_InitStructure;
	/* 使能与 I2C1 有关的时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	//SHT10PORT
    
  /* PE6-I2C1_SCL、PE5-I2C1_SDA*/
  GPIO_InitStructure.GPIO_Pin =  SCL | SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       // 开漏输出
  GPIO_Init(SHT10PORT, &GPIO_InitStructure);
}

void SDA_SET_OUT(void)
{
	/* 使能与 I2C1 有关的时钟 */
    
  /* PE6-I2C1_SCL、PE5-I2C1_SDA*/
  GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = SDA;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       // 开漏输出
   GPIO_Init(SHT10PORT, &GPIO_InitStructure);	
}
void SDA_SET_IN(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = SDA;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	       // 开漏输出
   GPIO_Init(SHT10PORT, &GPIO_InitStructure);	
}
void SCL_OUT(u8 out)
{

  if (out)
	GPIO_SetBits(SHT10PORT,SCL);
	else
	GPIO_ResetBits(SHT10PORT,SCL);
}
void SDA_OUT(u8 out)
{
	SDA_SET_OUT();
	if (out)
	GPIO_SetBits(SHT10PORT,SDA);
	else
	GPIO_ResetBits(SHT10PORT,SDA);
}
u8 SDA_READ(void)
{
	SDA_SET_IN();
	return GPIO_ReadInputDataBit(SHT10PORT,SDA);
	
}

//----------------------------------------------------------------------------------
char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge 
{ 
  unsigned char i,error=0;  
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { 
  	if (i & value)
    SDA_OUT(1);          //masking value with i , write to SENSI-BUS
    else SDA_OUT(0);                        
    delay_us(2);                      //observe setup time
    SCL_OUT(1);                          //clk for SENSI-BUS
    delay_us(6);        //pulswith approx. 5 us  	
    SCL_OUT(0);
    delay_us(2);                         //observe hold time
  }
  SDA_OUT(1);                          //release DATA-line
  delay_us(2);                         //observe setup time
  SCL_OUT(1);                          //clk #9 for ack 
  error=SDA_READ();                      //check ack (DATA will be pulled down by SHT11)
  SCL_OUT(0);       
  return error;                     //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
{ 
  unsigned char i,val=0;
  SDA_OUT(1);                           //release DATA-line
  for (i=0x80;i>0;i=i>>1)             //shift bit for masking
  { 
    SCL_OUT(1);                        //clk for SENSI-BUS
    if (SDA_READ()) val=(val | i);        //read bit  
    SCL_OUT(0);  					 
  }
  SDA_OUT(!ack);                        //in case of "ack==1" pull down DATA-Line
  delay_us(2);                          //observe setup time
  SCL_OUT(1);                            //clk #9 for ack
  delay_us(6);          //pulswith approx. 5 us 
  SCL_OUT(0);
  delay_us(2);                         //observe hold time						    
  SDA_OUT(1);                          //release DATA-line
  return val;
}


//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{  
   SDA_OUT(1); SCL_OUT(0);                   //Initial state
   delay_us(2);
   SCL_OUT(1);
   delay_us(2);
   SDA_OUT(0);
   delay_us(2);
   SCL_OUT(0);  
   delay_us(6);
   SCL_OUT(1);
   delay_us(2);
   SDA_OUT(1);		   
   delay_us(2);
   SCL_OUT(0);		   
}

//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{  
  unsigned char i; 
  SDA_OUT(1); 
  SCL_OUT(0);                    //Initial state
  for(i=0;i<9;i++)                  //9 SCK cycles
  { SCL_OUT(1);
    SCL_OUT(0);
  }
  s_transstart();                   //transmission start
}

//----------------------------------------------------------------------------------
char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset 
{ 
  unsigned char error=0;  
  s_connectionreset();              //reset communication
  error+=s_write_byte(RESET);       //send RESET-command to sensor
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  s_transstart();                   //transmission start
  error=s_write_byte(STATUS_REG_R); //send command to sensor
  *p_value=s_read_byte(ACK);        //read status register (8-bit)
  *p_checksum=s_read_byte(noACK);   //read checksum (8-bit)  
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_write_statusreg(unsigned char *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  s_transstart();                   //transmission start
  error+=s_write_byte(STATUS_REG_W);//send command to sensor
  error+=s_write_byte(*p_value);    //send value of status register
  return error;                     //error>=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{ 
  unsigned char error=0;
  unsigned int i;

  s_transstart();                   //transmission start
	

	
	//发送测量命令
  switch(mode){                     //send command to sensor
    case TEMP	: error+=s_write_byte(MEASURE_TEMP); break;
    case HUMI	: error+=s_write_byte(MEASURE_HUMI); break;
    default     : break;	 
  }

	//读数

  for (i=0;i<60000;i++)   
		if(SDA_READ()==0) break; //wait until sensor has finished the measurement
  if(SDA_READ()) error+=1;               // or timeout (~2 sec.) is reached
  *(p_value+1)  =s_read_byte(ACK);    //read the first byte (MSB)
 // printf("p_value is %x",*p_value);
  *(p_value)=s_read_byte(ACK);
 // printf("p_value+1 is %x",*(p_value+1));    //read the second byte (LSB)
  *p_checksum =s_read_byte(noACK);  //read checksum
	
  return error;
//  }
}

//----------------------------------------------------------------------------------------
void calc_sth11(float *p_humidity ,float *p_temperature)
//----------------------------------------------------------------------------------------
// calculates temperature [癈] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [癈]
{ const float C1=-4.0;           // for 12 Bit RH
  const float C2=+0.0405;           // for 12 Bit RH
  const float C3=-0.0000028;     // for 12 Bit RH
  const float T1=+0.01;             // for 12 Bit RH
  const float T2=+0.00008;          // for 12 Bit RH	

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [癈]

  t_C=t*0.01 - 39.6;                //calc. temperature [癈] from 14 bit temp. ticks @ 5V
  rh_lin=C3*rh*rh + C2*rh + C1;     //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [癈]
  *p_humidity=rh_true;              //return humidity[%RH]
}


//--------------------------------------------------------------------
float calc_dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [癈]
// output:  dew point [癈]
{ float k,dew_point ;
  
  k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
  dew_point = 243.12*k/(17.62-k);
  return dew_point;
}

//--------------------------------------------------------------------
void sht10_read(void)
//--------------------------------------------------------------------
// 读温湿度
// input:   humidity [%RH], temperature [癈]
// output:  dew point [癈]
{
	
	 unsigned char error,checksum;
	 error=0;

	 error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity

	 error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature
	
	 
	 if(error!=0)	 {
		s_connectionreset();                 //in case of an error: connection reset
		humi_val.active=0;
		temp_val.active=0;
		//printf("读写SHT10,失败，重新初始化STH10\r\n");	
	 }
	 else
	 { 
			humi_val.f=(float)humi_val.i;                   //converts integer to float
			temp_val.f=(float)temp_val.i;                   //converts integer to float

			calc_sth11(&humi_val.f,&temp_val.f);            //calculate humidity, temperature
		 
			humi_val.i=temp_val.i=0;//20150328

			//printf("temp:%5.1fC humi:%5.1f%%\n",temp_val.f,humi_val.f);

			//printf("读写SHT10成功\r\n");																			
			humi_val.active=1;
			temp_val.active=1;
	 }
}
