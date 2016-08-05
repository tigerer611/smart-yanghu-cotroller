/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
  *          ���õ��� 1us 10us 1ms �жϡ�     
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
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
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 20    	 50ms�ж�һ��
	 * SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if (SysTick_Config(SystemCoreClock / 100))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);
	}
		// �رյδ�ʱ��  
//	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;

}



/**
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
extern u8 time18ms_out,start_test;
void TimingTick_Decrement(void)
{	
	if(TimingInputCount++>3)//30msһ��λ����������ɨ�������
	{
		TimingInput=1;
		TimingInputCount=0;
	}

	if(net_time05++>=50){					 //0.5�������־
	   net_time05=0;
	   net_timeover05=1;	
	}  
	if(net_time10++>=1000){				 //10�������־
	   net_time10=0;
	   net_timeover10=1;	
	} 

	if(start_test)
		time18ms_out++;
	
	if(massuring==0)//û�н��в���
	{
		massuringTime++;
		if(massuringTime>200)//2s+һ��
		{
			massureflag=1;
			massuringTime=0;
		}
	}else massuringTime=0;
	
	//��������ʱ���ʱ�3s
	if(key_input)
	{
		if(beeptime++>300)
			beeptime=301;
	}else
	{
		beeptime=0;
	}
	
	//��˸��ʱ
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
	
	//�����ʼ����־
	if(request_initnet)
	{
		request_initnet++;
		if(request_initnet>250)
			request_initnet=251;
	}
	
	if(++time1s>=100)//1sʱ�䵽20150421
	{
		remainder_write++;//5����һд��
		if(time_s_remainder>0)
			time_s_remainder--;
		time1s=0;
	}
	//���õȴ�10s
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
