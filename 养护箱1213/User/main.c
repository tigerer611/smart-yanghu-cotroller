/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   systick系统定时器
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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






// 挨踢员
DHT11_Data_TypeDef DHT11_Data;
value humi_val,temp_val;

/*温湿度设定值*/

float temp_f;
u8 humi_set=98;
u8 temp_set=20;
u8 humi_distance=0;//温度裕量
u8 temp_distance=5;//湿度裕量

/*工作状态1加热,2加湿,4制冷,8恒湿*/
u8 work_state=0;

/*网络状态0远程，1本地*/
u8 network_state=0;

/*工作时间*/
u16 work_time=0;
/*压力上限和下限*/
float press_up=4.0;
float press_down=0.1;
float press_current=0;

//按键输入
extern u8 key_input;

//输入采样时间50ms
extern vu8 TimingInput;

extern u16 remainder_write;


extern double cal_test[6];

//温湿度测量间隔
u8 shtWaitFinishMeasureOver05=0;
u8 shtWaitFinishMeasure05=0;

//网络辅助变量
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



/*没有负数
0：铁电数据有效标志
1：温度设定
2：湿度设定
3：温度裕量
4：湿度裕量
5-7：待定
8-11：ip
12-15：mask
16-19：gate
20-23：待定
24-29：mac
32-35：时间
36：激活标志，'a'是激活，其它都不是
37-39:待定
40-47：激活号

	*/	
u8 i2c_eeprom_data[48]={0};
u8 tempForControl=0;

//本文件函数声明
void I2C_EE_Restore(void);

int main1()
{
//网络模块开始	
//	//中断开启,配置net管脚和传感器data管脚中断
//	NVIC_Configuration();
//		/*按键输入初始化*/
//	GPIO_INPUT_INIT();
//	
//	/*输出初始化*/
//	IO_OUTPUT_Config();

//	//中断开启,配置net管脚和传感器data管脚中断
//	NVIC_Configuration();


//	SPI1_Init();    
//	InitNet();		/* 初始化网络设备以及UIP协议栈，配置IP地址 */	


///* 配置SysTick 为10ms中断一次 */
////	SysTick_Init();
//	while(1)
//	{
//		/* 下面是网络检测的程序，处理uip事件，必须插入到用户程序的循环体中 */
//		UipPro();			  //中断触发读取网络接收缓存
//		eth_poll();           //定时查询TCP及UDP连接收发状态	ARP表更新， 并响应				
//	}
//网络模块结束

	/* 配置SysTick 为1us中断一次 */
	SysTick_Init();
	DelayInit();
	USART1_Config();
	printf("\r\n***野火ISO dht11 温湿度传感器实验***\r\n");

	/*初始化DTT11的引脚*/
	DHT11_GPIO_Config();
	while(1)
	/*调用Read_DHT11读取温湿度，若成功则输出该信息*/
	{
		if( Read_DHT11(&DHT11_Data)==SUCCESS)
		{
			printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			//printf("\r\n 湿度:%d,温度:%d \r\n" ,DHT11_Data.humi_int,DHT11_Data.temp_int);
		}
		else
		{
			printf("Read DHT11 ERROR!\r\n");
		}
	}	
			
}

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
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
		/* 配置SysTick 为1us中断一次 */

	SysTick_Init();
	
//	TIM3_Int_Init(100,7199);
	
	DelayInit();
//	delay_init(72);
	
	/*按键输入初始化*/

	GPIO_INPUT_INIT();
	
	/*输出初始化*/
	IO_OUTPUT_Config();
	
	/* I2C 外设初(AT24C02)始化 */
	I2C_EE_Init();	
  
 /*当按下-键时，程序会执行默认的IP192.168.3.248,不会从eeprom读取数据*/
 if( Key_Scan(KEYPORT,KEY_SUB,1) != KEY_ON  )
		I2C_EE_Restore();
 if(EE_FLAG!=i2c_eeprom_data[0])
	{		
		//初始温湿度设定及裕量
		i2c_eeprom_data[0]=EE_FLAG;
		i2c_eeprom_data[1]=temp_set;
		i2c_eeprom_data[2]=humi_set;

		i2c_eeprom_data[3]=temp_distance;
		i2c_eeprom_data[4]=humi_distance;		

		//初始化net
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
		//网络
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+8, 8, 8);	
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+16, 16, 8);	
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+24, 24, 8);		
	}
  
	//中断开启,配置net管脚和传感器data管脚中断
//	NVIC_Configuration();
	
	/*SHT10-模拟I2C读取温湿度，初始化*/
//	Sht_GPIO_Config();	
//	s_connectionreset();
//	sht10_read();
	/*初始化DTT11的引脚*/
	DHT11_GPIO_Config();
	
	SPI1_Init();    
	//InitNet();		/* 初始化网络设备以及UIP协议栈，配置IP地址 */	
	
//数码管初始化	
  hc595_init();

/* 配置SysTick 为10ms中断一次 */
//	SysTick_Init();
	TimingInput=0;//输入节拍20ms
	IWDG_Init(4,625);    //与分频数为64,重载值为625,溢出时间为1s	   
	while(1)
	{
		
		
		/*温湿度测量，massureflag请求测量标志*/
		if(massureflag)
		{	
			//数据伪造1213，温度控制用的是真实的，显示和上传的数据用的是渐变的伪造数据
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
				
				if(set_wait>1000)//初始化已过
					if((temp_val.f>-10)&&(temp_val.f<100)&&(humi_val.f>=0/*)&&(humi_val.f<100*/)&&poweron)//传感器没坏才输出
						automation();
				
		
			}else if(readstatus!=NOTREADY)
			{
				sensor_error++;				
			}
		}
		if(sensor_error>5)//5次都没有采到温湿度
		{
			temp_val.f=humi_val.f=-100;
		}
				
		
		//printf("温度：%.1f\t湿度%.1f\r\n",temp_val.f,humi_val.f);
		
		//根据检测到的温湿度控制led和继电器，刚开机3s的时候尚未检测到温湿度，此时最好不要有输出，在while之前添加sht10_read()吧,进来之前就读一次数据
		
		
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
			//下面的代码是按键检测，用来设定温湿度设定值的
			if(TimingInput)//输入检测20ms一次
			{
				TimingInput=0;
				key_input_last=key_input;
				GPIO_INPUT_SCAN();		
				if(key_input==key_input_last)//检测是否一直按着同一个按键
					press_same_key_flag=1;//一直按着同一个按键
				else press_same_key_flag=0;
				
				//蜂鸣器响应
				if((key_input)&&(beeptime<300))
				{			
						//蜂鸣器响
						BEEP_SCREAM(1);
					
				}else
				{
					if(key_input==0)
						beeptime=0;
					//蜂鸣器关闭
					BEEP_SCREAM(0);
				}			
				
				//是否进入设定状态，闪烁。
				if(0==showdigital)//之前就是闪烁状态，temp_set_temp数据要保留
				{
					temp_set_temp=temp_set;
					humi_set_temp=humi_set;
				}
				
				if((0==press_same_key_flag)&&(0!=key_input))//有按键按下但不是同一个按键
				{
					
					//输入处理
					switch (key_input)
					{
					case 1://set
						if(set_wait<=1000)
							set_start_flag=1;
						showdigital++;//第showdigital位闪烁
						if(showdigital>4)
							showdigital=1;
						break;
					case 2://save
						showdigital=0;
						set_start_flag=0;//初始化完成标志
						set_wait=1001;//设置等待10s结束
						//将数据保存到eeprom,温湿度设定值
						temp_set=temp_set_temp;
						humi_set=humi_set_temp;
						i2c_eeprom_data[1]=temp_set;
						i2c_eeprom_data[2]=humi_set;
						
						//写入eeprom
						i2c_eeprom_data[0]=EE_FLAG;
						I2C_EE_WaitOperationIsCompleted();
						I2C_EE_WriteBuffer(i2c_eeprom_data, 0, 5);
						break;
					case 4://+
						//第showdigital位+
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
						//第showdigital位+
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
		{//本地状态显示
			humi_set_temp=humi_set;
			temp_set_temp=temp_set;
			key_input=0;
			showdigital=0;
			BEEP_SCREAM(0);
			beeptime=0;
		}
		
		//试用期写入
		if((active!='a')&&(remainder_write>300))
		{
			remainder_write=0;
			//写入eeprom
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
		//数码管输出
		
		digitaldata[0]=digitaldata[1]=digitaldata[2]=digitaldata[3]=digitaldata[4]=digitaldata[5]=12;//12
		if((active=='a')||(time_s_remainder>0))//激活态20150421
		{
			//温度
			if((temp_val.f<0)&&(temp_val.f>-10))//负值
			{
				digitaldata[0]=10;//-
				digitaldata[1]=(u8)(-temp_val.f);
				digitaldata[2]=(u8)(-temp_val.f*10)%10;
			}else if((temp_val.f>=0)&&(temp_val.f<100))//正数
			{
				digitaldata[0]=((u8)temp_val.f)/10;
				digitaldata[1]=((u8)temp_val.f)%10;//.
				digitaldata[2]=(u8)(((u16)(temp_val.f*10))%10);
			}
			//湿度
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
		}else	//试用期已经到了,显示off 20150421
		{
			digitaldata[0]=0;
			digitaldata[3]=0;
			BEEP_SCREAM(1);
		}
		//设定温度值
		digitaldata[6]=temp_set_temp/10;
		digitaldata[7]=temp_set_temp%10;
		//湿度设定
		digitaldata[8]=humi_set_temp/10;
		digitaldata[9]=humi_set_temp%10;
		
		if((blickflag)&&(network_state==0))//可以闪烁			
			switch(showdigital)
			{//showdigital=0//低位到高位是否显示6 7 8 9，0都显示，1：9不显示；2：8不显示；3:7不显示；4：6不显示
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
			
			
		//就地状态或网络状态PC3
		if( Key_Scan(GPIOC,GPIO_Pin_3,0) == KEY_ON  )
		{
			/*按键有效*/
			network_state=1;
			
		}else//就地
		{
			network_state=0;
			//poweron=1; //0716
			//启动开关PA12检测 20150719
			if( Key_Scan(GPIOA,GPIO_Pin_12,1) == KEY_ON  )
			{
				poweron=1;
			}else
			{
				poweron=0;
			}
		}
			
			
		/* 下面是网络检测的程序，处理uip事件，必须插入到用户程序的循环体中 */
		UipPro();			  //中断触发读取网络接收缓存
		eth_poll();           //定时查询TCP及UDP连接收发状态	ARP表更新， 并响应		
		
		IWDG_Feed();//喂狗
		
		if(request_initnet>250)//等待2s初始化网络
		{
			InitNet();
			request_initnet=0;
		}
	}    
}

/**
  * @brief  I2C 外设(EEPROM)数据恢复
  * @param  无
  * @retval 无
  */
void I2C_EE_Restore(void)
{
	I2C_EE_WaitOperationIsCompleted();
	I2C_EE_ReadBuffer(i2c_eeprom_data, 0, 37);	
	I2C_EE_WaitOperationIsCompleted();
	//通过读取的20个数据的和是否是0来判断，数据是否有效
	if(EE_FLAG==i2c_eeprom_data[0])//eeprom中数据有效
	{
		//初始化湿度和温度设定值
		humi_set=i2c_eeprom_data[2];
		temp_set=i2c_eeprom_data[1];
		humi_distance=i2c_eeprom_data[4];
		temp_distance=i2c_eeprom_data[3];
		//初始化ip地址信息，条件是eeprom不为空
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
		
		//激活
		active=i2c_eeprom_data[36];
		//active='a';
		//剩余使用时间
		time_s_remainder=i2c_eeprom_data[32]|(((u32)i2c_eeprom_data[33])<<8)|(((u32)i2c_eeprom_data[34])<<16)|(((u32)i2c_eeprom_data[35])<<24);
	}
}
/*********************************************END OF FILE**********************/




