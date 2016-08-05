/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
  *          常用的有 1us 10us 1ms 中断。     
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_SysTick.h"

vu8 TimingInput,TimingInputCount=0;
extern unsigned short net_time05;
extern unsigned short net_time10;
extern unsigned char net_timeover05;
extern unsigned char net_timeover10;
extern u8 shtWaitFinishMeasure05;
extern u8 shtWaitFinishMeasureOver05;
extern u8 massuring,massureflag;
u8 massuringTime=0;
extern u16 beeptime;
extern u8 blickflag;
extern u8 showdigital;
extern u8 key_input;
u8 blickcount=0;
//u8 request_initnet_count=0;
extern u8 request_initnet;
extern u16 set_wait;
extern u8 set_start_flag;
u16 time1s=0;
extern u32 time_s_remainder;
u16 remainder_write=0;

 
/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 20    	 50ms中断一次
	 * SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0库版本
	if (SysTick_Config(SystemCoreClock / 100))	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while (1);
	}
		// 关闭滴答定时器  
//	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;

}



/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
extern u8 time18ms_out,start_test;
void TimingTick_Decrement(void)
{	
	if(TimingInputCount++>3)//30ms一置位，按键输入扫描后置零
	{
		TimingInput=1;
		TimingInputCount=0;
	}

	if(net_time05++>=50){					 //0.5秒溢出标志
	   net_time05=0;
	   net_timeover05=1;	
	}  
	if(net_time10++>=1000){				 //10秒溢出标志
	   net_time10=0;
	   net_timeover10=1;	
	} 

	if(start_test)
		time18ms_out++;
	
	if(massuring==0)//没有进行测量
	{
		massuringTime++;
		if(massuringTime>200)//2s+一测
		{
			massureflag=1;
			massuringTime=0;
		}
	}else massuringTime=0;
	
	//蜂鸣器响时间计时最长3s
	if(key_input)
	{
		if(beeptime++>300)
			beeptime=301;
	}else
	{
		beeptime=0;
	}
	
	//闪烁计时
	if(showdigital)
	{
		blickcount++;//1s
		if(blickcount>50)
		{
			blickflag=0;
			if(blickcount>100)
				blickcount=0;
		}else
		{
			blickflag=1;
		}
	}else
	{
		blickcount=0;
	}
	
	//网络初始化标志
	if(request_initnet)
	{
		request_initnet++;
		if(request_initnet>250)
			request_initnet=251;
	}
	
	if(++time1s>=100)//1s时间到20150421
	{
		remainder_write++;//5分钟一写入
		if(time_s_remainder>0)
			time_s_remainder--;
		time1s=0;
	}
	//设置等待10s
	if(set_wait<=1000)
	{
		set_wait++;
		if(set_start_flag)
			set_wait=0;
		if(set_wait>1000)
			set_wait=1001;
	}

}
/*********************************************END OF FILE**********************/
