/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   systickϵͳ��ʱ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "i2c_ee_dma.h"
#include "bsp_input.h"
#include "..\User\dht11\bsp_dht11.h"
#include "bsp_usart1.h"
#include "bsp_exti.h"
#include "spi.h"
#include "bsp_net.h"
#include "delay.h"
#include "hc595.h"
#include "io_output.h"
#include "wdg.h"
#include "..\User\timer3\timer3.h"






// ����Ա
DHT11_Data_TypeDef DHT11_Data;
value humi_val,temp_val;

/*��ʪ���趨ֵ*/

float temp_f;
u8 humi_set=98;
u8 temp_set=20;
u8 humi_distance=0;//�¶�ԣ��
u8 temp_distance=5;//ʪ��ԣ��

/*����״̬1����,2��ʪ,4����,8��ʪ*/
u8 work_state=0;

/*����״̬0Զ�̣�1����*/
u8 network_state=0;

/*����ʱ��*/
u16 work_time=0;
/*ѹ�����޺�����*/
float press_up=4.0;
float press_down=0.1;
float press_current=0;

//��������
extern u8 key_input;

//�������ʱ��50ms
extern vu8 TimingInput;

extern u16 remainder_write;


extern double cal_test[6];

//��ʪ�Ȳ������
u8 shtWaitFinishMeasureOver05=0;
u8 shtWaitFinishMeasure05=0;

//���縨������
extern u8 mymac[6];
extern u8 ipaddress[4],gateaddress[4],maskaddress[4];
extern void DelayInit(void);

u8 cmd_sended=0;
u8 cmd_humi=1;
u8 massuring=0;
u8 massureflag=0;
u16 beeptime=0;
u8 blickflag=0;
u8 showdigital=0;
u8 key_input_last=0;
u8 press_same_key_flag=0;
u16 sensor_error=0;
u8 request_initnet=0;
u16 set_wait=0;
u8 set_start_flag=0;
u8 poweron=1;//20150420
u32 time_s_remainder=360000;//3600000
u8 active=0;



/*û�и���
0������������Ч��־
1���¶��趨
2��ʪ���趨
3���¶�ԣ��
4��ʪ��ԣ��
5-7������
8-11��ip
12-15��mask
16-19��gate
20-23������
24-29��mac
32-35��ʱ��
36�������־��'a'�Ǽ������������
37-39:����
40-47�������

	*/	
u8 i2c_eeprom_data[48]={0};
u8 tempForControl=0;

//���ļ���������
void I2C_EE_Restore(void);

int main1()
{
//����ģ�鿪ʼ	
//	//�жϿ���,����net�ܽźʹ�����data�ܽ��ж�
//	NVIC_Configuration();
//		/*���������ʼ��*/
//	GPIO_INPUT_INIT();
//	
//	/*�����ʼ��*/
//	IO_OUTPUT_Config();

//	//�жϿ���,����net�ܽźʹ�����data�ܽ��ж�
//	NVIC_Configuration();


//	SPI1_Init();    
//	InitNet();		/* ��ʼ�������豸�Լ�UIPЭ��ջ������IP��ַ */	


///* ����SysTick Ϊ10ms�ж�һ�� */
////	SysTick_Init();
//	while(1)
//	{
//		/* ������������ĳ��򣬴���uip�¼���������뵽�û������ѭ������ */
//		UipPro();			  //�жϴ�����ȡ������ջ���
//		eth_poll();           //��ʱ��ѯTCP��UDP�����շ�״̬	ARP����£� ����Ӧ				
//	}
//����ģ�����

	/* ����SysTick Ϊ1us�ж�һ�� */
	SysTick_Init();
	DelayInit();
	USART1_Config();
	printf("\r\n***Ұ��ISO dht11 ��ʪ�ȴ�����ʵ��***\r\n");

	/*��ʼ��DTT11������*/
	DHT11_GPIO_Config();
	while(1)
	/*����Read_DHT11��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
	{
		if( Read_DHT11(&DHT11_Data)==SUCCESS)
		{
			printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			//printf("\r\n ʪ��:%d,�¶�:%d \r\n" ,DHT11_Data.humi_int,DHT11_Data.temp_int);
		}
		else
		{
			printf("Read DHT11 ERROR!\r\n");
		}
	}	
			
}

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
u8 digitaldata[10];
extern u8 time18ms_out;
int main(void)
{	
	

	u8 readstatus,firstread=0;
	u8 humi_set_temp=humi_set;
	u8 temp_set_temp=temp_set;
	//main1();
	/* USART1 config 115200 8-N-1 */
//	USART1_Config();
//	
//	printf("\r\n this is a yhx firmware \r\n");	
		/* ����SysTick Ϊ1us�ж�һ�� */

	SysTick_Init();
	
//	TIM3_Int_Init(100,7199);
	
	DelayInit();
//	delay_init(72);
	
	/*���������ʼ��*/

	GPIO_INPUT_INIT();
	
	/*�����ʼ��*/
	IO_OUTPUT_Config();
	
	/* I2C �����(AT24C02)ʼ�� */
	I2C_EE_Init();	
  
 /*������-��ʱ�������ִ��Ĭ�ϵ�IP192.168.3.248,�����eeprom��ȡ����*/
 if( Key_Scan(KEYPORT,KEY_SUB,1) != KEY_ON  )
		I2C_EE_Restore();
 if(EE_FLAG!=i2c_eeprom_data[0])
	{		
		//��ʼ��ʪ���趨��ԣ��
		i2c_eeprom_data[0]=EE_FLAG;
		i2c_eeprom_data[1]=temp_set;
		i2c_eeprom_data[2]=humi_set;

		i2c_eeprom_data[3]=temp_distance;
		i2c_eeprom_data[4]=humi_distance;		

		//��ʼ��net
		//ip
		i2c_eeprom_data[8]=ipaddress[0];
		i2c_eeprom_data[9]=ipaddress[1];
		i2c_eeprom_data[10]=ipaddress[2];
		i2c_eeprom_data[11]=ipaddress[3];
		//mask
		i2c_eeprom_data[12]=maskaddress[0];
		i2c_eeprom_data[13]=maskaddress[1];
		i2c_eeprom_data[14]=maskaddress[2];
		i2c_eeprom_data[15]=maskaddress[3];
		//gate
		i2c_eeprom_data[16]=gateaddress[0];
		i2c_eeprom_data[17]=gateaddress[1];
		i2c_eeprom_data[18]=gateaddress[2];
		i2c_eeprom_data[19]=gateaddress[3];
		//mac
		i2c_eeprom_data[24]=mymac[0];
		i2c_eeprom_data[25]=mymac[1];
		i2c_eeprom_data[26]=mymac[2];
		i2c_eeprom_data[27]=mymac[3];
		i2c_eeprom_data[28]=mymac[4];
		i2c_eeprom_data[29]=mymac[5];
		
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data, 0, 8);	
		//����
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+8, 8, 8);	
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+16, 16, 8);	
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+24, 24, 8);		
	}
  
	//�жϿ���,����net�ܽźʹ�����data�ܽ��ж�
//	NVIC_Configuration();
	
	/*SHT10-ģ��I2C��ȡ��ʪ�ȣ���ʼ��*/
//	Sht_GPIO_Config();	
//	s_connectionreset();
//	sht10_read();
	/*��ʼ��DTT11������*/
	DHT11_GPIO_Config();
	
	SPI1_Init();    
	//InitNet();		/* ��ʼ�������豸�Լ�UIPЭ��ջ������IP��ַ */	
	
//����ܳ�ʼ��	
  hc595_init();

/* ����SysTick Ϊ10ms�ж�һ�� */
//	SysTick_Init();
	TimingInput=0;//�������20ms
	IWDG_Init(4,625);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s	   
	while(1)
	{
		
		
		/*��ʪ�Ȳ�����massureflag���������־*/
		if(massureflag)
		{	
			//����α��1213���¶ȿ����õ�����ʵ�ģ���ʾ���ϴ��������õ��ǽ����α������
			if( (readstatus=Read_DHT11(&DHT11_Data))==SUCCESS)
			{
				
				if(firstread==0) 
				{
					temp_val.f=DHT11_Data.temp_int;
					firstread=1;
				}
				else
				{
					if(DHT11_Data.temp_int>(int)temp_val.f)
						temp_val.f=temp_val.f+0.5;
					else if(DHT11_Data.temp_int<(int)temp_val.f)
						temp_val.f=temp_val.f-0.5;
				}
				
				humi_val.f=DHT11_Data.humi_int;
				sensor_error=0;
				massureflag=0;
				time18ms_out=0;
				
				if(set_wait>1000)//��ʼ���ѹ�
					if((temp_val.f>-10)&&(temp_val.f<100)&&(humi_val.f>=0/*)&&(humi_val.f<100*/)&&poweron)//������û�������
						automation();
				
		
			}else if(readstatus!=NOTREADY)
			{
				sensor_error++;				
			}
		}
		if(sensor_error>5)//5�ζ�û�вɵ���ʪ��
		{
			temp_val.f=humi_val.f=-100;
		}
				
		
		//printf("�¶ȣ�%.1f\tʪ��%.1f\r\n",temp_val.f,humi_val.f);
		
		//���ݼ�⵽����ʪ�ȿ���led�ͼ̵������տ���3s��ʱ����δ��⵽��ʪ�ȣ���ʱ��ò�Ҫ���������while֮ǰ���sht10_read()��,����֮ǰ�Ͷ�һ������
		
		
		if((0==poweron)||((time_s_remainder==0)&&(active!='a')))
		{
			WARM(0);
			WARM_LED(0);
			COLD(0);
			COLD_LED(0);
			WET(0);
			WET_LED(0);
			ALWAYS_WET_LED(0);
			BEEP_SCREAM(0);
			work_state=0;
		}

			
		if(network_state==0)
		{
			//����Ĵ����ǰ�����⣬�����趨��ʪ���趨ֵ��
			if(TimingInput)//������20msһ��
			{
				TimingInput=0;
				key_input_last=key_input;
				GPIO_INPUT_SCAN();		
				if(key_input==key_input_last)//����Ƿ�һֱ����ͬһ������
					press_same_key_flag=1;//һֱ����ͬһ������
				else press_same_key_flag=0;
				
				//��������Ӧ
				if((key_input)&&(beeptime<300))
				{			
						//��������
						BEEP_SCREAM(1);
					
				}else
				{
					if(key_input==0)
						beeptime=0;
					//�������ر�
					BEEP_SCREAM(0);
				}			
				
				//�Ƿ�����趨״̬����˸��
				if(0==showdigital)//֮ǰ������˸״̬��temp_set_temp����Ҫ����
				{
					temp_set_temp=temp_set;
					humi_set_temp=humi_set;
				}
				
				if((0==press_same_key_flag)&&(0!=key_input))//�а������µ�����ͬһ������
				{
					
					//���봦��
					switch (key_input)
					{
					case 1://set
						if(set_wait<=1000)
							set_start_flag=1;
						showdigital++;//��showdigitalλ��˸
						if(showdigital>4)
							showdigital=1;
						break;
					case 2://save
						showdigital=0;
						set_start_flag=0;//��ʼ����ɱ�־
						set_wait=1001;//���õȴ�10s����
						//�����ݱ��浽eeprom,��ʪ���趨ֵ
						temp_set=temp_set_temp;
						humi_set=humi_set_temp;
						i2c_eeprom_data[1]=temp_set;
						i2c_eeprom_data[2]=humi_set;
						
						//д��eeprom
						i2c_eeprom_data[0]=EE_FLAG;
						I2C_EE_WaitOperationIsCompleted();
						I2C_EE_WriteBuffer(i2c_eeprom_data, 0, 5);
						break;
					case 4://+
						//��showdigitalλ+
						switch(showdigital)
						{
						case 1:
							temp_set_temp=(temp_set_temp/10+1)%10*10+temp_set_temp%10;
							break;
						case 2:
							temp_set_temp=temp_set_temp/10*10+(temp_set_temp%10+1)%10;
							break;
						case 3:
							humi_set_temp=(humi_set_temp/10+1)%10*10+humi_set_temp%10;
							break;
						case 4:
							humi_set_temp=humi_set_temp/10*10+(humi_set_temp%10+1)%10;
							break;
						default:;
						}
						break;
					case 8://-
						//��showdigitalλ+
						switch(showdigital)
						{
						case 1:
							temp_set_temp=(temp_set_temp<10)?90+temp_set_temp:(temp_set_temp/10-1)%10*10+temp_set_temp%10;
							break;
						case 2:
							temp_set_temp=(temp_set_temp%10==0)?temp_set_temp+9:temp_set_temp-1;
							break;
						case 3:
							humi_set_temp=(humi_set_temp<10)?90+humi_set_temp:(humi_set_temp/10-1)%10*10+humi_set_temp%10;
							break;
						case 4:
							humi_set_temp=(humi_set_temp%10==0)?humi_set_temp+9:humi_set_temp-1;
							break;
						default:;
						}
						break;
					default:;
					}
				}
			}
		}else
		{//����״̬��ʾ
			humi_set_temp=humi_set;
			temp_set_temp=temp_set;
			key_input=0;
			showdigital=0;
			BEEP_SCREAM(0);
			beeptime=0;
		}
		
		//������д��
		if((active!='a')&&(remainder_write>300))
		{
			remainder_write=0;
			//д��eeprom
			//if(time_s_remainder>0)
			{
				i2c_eeprom_data[32]=(u8)time_s_remainder;
				i2c_eeprom_data[33]=(u8)(time_s_remainder>>8);
				i2c_eeprom_data[34]=(u8)(time_s_remainder>>16);
				i2c_eeprom_data[35]=(u8)(time_s_remainder>>24);
				I2C_EE_WaitOperationIsCompleted();
				I2C_EE_WriteBuffer(i2c_eeprom_data+32, 32, 4);
			}
		}
//sht10_j:		
		//��������
		
		digitaldata[0]=digitaldata[1]=digitaldata[2]=digitaldata[3]=digitaldata[4]=digitaldata[5]=12;//12
		if((active=='a')||(time_s_remainder>0))//����̬20150421
		{
			//�¶�
			if((temp_val.f<0)&&(temp_val.f>-10))//��ֵ
			{
				digitaldata[0]=10;//-
				digitaldata[1]=(u8)(-temp_val.f);
				digitaldata[2]=(u8)(-temp_val.f*10)%10;
			}else if((temp_val.f>=0)&&(temp_val.f<100))//����
			{
				digitaldata[0]=((u8)temp_val.f)/10;
				digitaldata[1]=((u8)temp_val.f)%10;//.
				digitaldata[2]=(u8)(((u16)(temp_val.f*10))%10);
			}
			//ʪ��
			if(humi_val.f>=0)//((humi_val.f>=0)&&(humi_val.f<100))
			{
				if(humi_val.f>=100) 
				{
					temp_f=humi_val.f;//20150712 add temp xzk
					humi_val.f=99.9;
				}
				
				digitaldata[3]=((u8)humi_val.f)/10;
				digitaldata[4]=((u8)humi_val.f)%10;//.
				digitaldata[5]=(u8)(((u16)(humi_val.f*10))%10);
				if(humi_val.f>=99.9)
					humi_val.f=temp_f;
			}
		}else	//�������Ѿ�����,��ʾoff 20150421
		{
			digitaldata[0]=0;
			digitaldata[3]=0;
			BEEP_SCREAM(1);
		}
		//�趨�¶�ֵ
		digitaldata[6]=temp_set_temp/10;
		digitaldata[7]=temp_set_temp%10;
		//ʪ���趨
		digitaldata[8]=humi_set_temp/10;
		digitaldata[9]=humi_set_temp%10;
		
		if((blickflag)&&(network_state==0))//������˸			
			switch(showdigital)
			{//showdigital=0//��λ����λ�Ƿ���ʾ6 7 8 9��0����ʾ��1��9����ʾ��2��8����ʾ��3:7����ʾ��4��6����ʾ
				case 1:
					digitaldata[6]=11;
					break;
				case 2:
					digitaldata[7]=11;
					break;
				case 3:
					digitaldata[8]=11;
					break;
				case 4:
					digitaldata[9]=11;
					break;
				default:;
			}
		hc595_display(digitaldata);		
			
			
		//�͵�״̬������״̬PC3
		if( Key_Scan(GPIOC,GPIO_Pin_3,0) == KEY_ON  )
		{
			/*������Ч*/
			network_state=1;
			
		}else//�͵�
		{
			network_state=0;
			//poweron=1; //0716
			//��������PA12��� 20150719
			if( Key_Scan(GPIOA,GPIO_Pin_12,1) == KEY_ON  )
			{
				poweron=1;
			}else
			{
				poweron=0;
			}
		}
			
			
		/* ������������ĳ��򣬴���uip�¼���������뵽�û������ѭ������ */
		UipPro();			  //�жϴ�����ȡ������ջ���
		eth_poll();           //��ʱ��ѯTCP��UDP�����շ�״̬	ARP����£� ����Ӧ		
		
		IWDG_Feed();//ι��
		
		if(request_initnet>250)//�ȴ�2s��ʼ������
		{
			InitNet();
			request_initnet=0;
		}
	}    
}

/**
  * @brief  I2C ����(EEPROM)���ݻָ�
  * @param  ��
  * @retval ��
  */
void I2C_EE_Restore(void)
{
	I2C_EE_WaitOperationIsCompleted();
	I2C_EE_ReadBuffer(i2c_eeprom_data, 0, 37);	
	I2C_EE_WaitOperationIsCompleted();
	//ͨ����ȡ��20�����ݵĺ��Ƿ���0���жϣ������Ƿ���Ч
	if(EE_FLAG==i2c_eeprom_data[0])//eeprom��������Ч
	{
		//��ʼ��ʪ�Ⱥ��¶��趨ֵ
		humi_set=i2c_eeprom_data[2];
		temp_set=i2c_eeprom_data[1];
		humi_distance=i2c_eeprom_data[4];
		temp_distance=i2c_eeprom_data[3];
		//��ʼ��ip��ַ��Ϣ��������eeprom��Ϊ��
		//ip
		ipaddress[0]=i2c_eeprom_data[8];
		ipaddress[1]=i2c_eeprom_data[9];
		ipaddress[2]=i2c_eeprom_data[10];
		ipaddress[3]=i2c_eeprom_data[11];
		//mask
		maskaddress[0]=i2c_eeprom_data[12];
		maskaddress[1]=i2c_eeprom_data[13];
		maskaddress[2]=i2c_eeprom_data[14];
		maskaddress[3]=i2c_eeprom_data[15];
		//gate
		gateaddress[0]=i2c_eeprom_data[16];
		gateaddress[1]=i2c_eeprom_data[17];
		gateaddress[2]=i2c_eeprom_data[18];
		gateaddress[3]=i2c_eeprom_data[19];
		//mac
		mymac[0]=i2c_eeprom_data[24];
		mymac[1]=i2c_eeprom_data[25];
		mymac[2]=i2c_eeprom_data[26];
		mymac[3]=i2c_eeprom_data[27];
		mymac[4]=i2c_eeprom_data[28];
		mymac[5]=i2c_eeprom_data[29];	
		
		//����
		active=i2c_eeprom_data[36];
		//active='a';
		//ʣ��ʹ��ʱ��
		time_s_remainder=i2c_eeprom_data[32]|(((u32)i2c_eeprom_data[33])<<8)|(((u32)i2c_eeprom_data[34])<<16)|(((u32)i2c_eeprom_data[35])<<24);
	}
}
/*********************************************END OF FILE**********************/




