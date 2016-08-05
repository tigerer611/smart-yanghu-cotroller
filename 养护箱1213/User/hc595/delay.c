#include "delay.h"
#include "stm32f10x.h"

#define  fac_us 9//us��ʱ������
#define fac_ms 9000//ms��ʱ������


enum CALC_TYPE {CALC_TYPE_S,CALC_TYPE_MS,CALC_TYPE_US};

static void TIM5_Init_Query(enum CALC_TYPE type)  
{  
	TIM_TimeBaseInitTypeDef Tim5;  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  
	Tim5.TIM_Period=1; //???  
	if(type==CALC_TYPE_S) //��ʱ��SΪ��λʱ��ʱ��Ƶ��57600Hz���ⲿ��Ҫ1250�μ�ʱ  
	{  
		Tim5.TIM_Prescaler=57600-1; //Ԥ��Ƶ 72MHz / 57600= 1250Hz  
	}else if(type==CALC_TYPE_MS)  
	{  
		Tim5.TIM_Prescaler=2880-1; //25000Hz ,��ʱ������25��Ϊms  
	}else if(type==CALC_TYPE_US)  
	{     
		Tim5.TIM_Prescaler=72-1; //1MHz ,����1��Ϊus  
	}else  
	{  
		Tim5.TIM_Prescaler=7200-1;  
	}  
	Tim5.TIM_ClockDivision=0;  
	Tim5.TIM_CounterMode=TIM_CounterMode_Down; //���¼���  
	TIM_TimeBaseInit(TIM5,&Tim5);         
}  
      
static void TIM5_S_CALC(uint32_t s)  
{  
	u16 counter=(s*1250)&0xFFFF; //ǰ�ᶨʱ��ʱ��Ϊ1250Hz  
	TIM_Cmd(TIM5,ENABLE);  
	TIM_SetCounter(TIM5,counter); //���ü���ֵ  
	  
	while(counter>1)  
	{  
		counter=TIM_GetCounter(TIM5);  
	}  
	TIM_Cmd(TIM5,DISABLE);  
}  
  
static void TIM5_MS_CALC(u16 ms)  
{  
	u16 counter=ms*1000;   
	TIM_Cmd(TIM5,ENABLE);  
	TIM_SetCounter(TIM5,counter); //���ü���ֵ  
	  
	while(counter>1)  
	{  
		counter=TIM_GetCounter(TIM5);  
	}  
	TIM_Cmd(TIM5,DISABLE);  
}  
  
static void TIM5_US_CALC(uint32_t us)  
{  
	u32 counter=us;  
	TIM_Cmd(TIM5,ENABLE);  
	TIM_SetCounter(TIM5,counter); //���ü���ֵ  
  
	while(counter>1)  
	{  
		counter=TIM_GetCounter(TIM5);  
	}  
	TIM_Cmd(TIM5,DISABLE);  
}  
  

//	TIM5_Init_Query(CALC_TYPE_US);  





//��ʼ���ӳٺ���
//��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void DelayInit()
{
	TIM5_Init_Query(CALC_TYPE_US); 
}

//��ʱnus
//nusΪҪ��ʱ��us��.		


//void delay_us(u32 nus)
//{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD; //LOAD��ֵ       
//	ticks=nus*fac_us;  //��Ҫ�Ľ�����     
//	tcnt=0;
//	__disable_irq();
//	told=SysTick->VAL;         //�ս���ʱ�ļ�����ֵ
//	while(1)
//	{
//		tnow=SysTick->VAL;
//		if(tnow!=told)
//		{     
//			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
//			else tcnt+=reload-tnow+told;     
//			told=tnow;
//			if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
//		}  
//	}
//	__enable_irq();
//}


//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(u16 nms)
{
//	__disable_irq();
	TIM5_MS_CALC(nms); 
//	__enable_irq();
//	delay_us(nms*1000);
}

void delay_us(__IO u32 nus)
{
	__disable_irq();
	TIM5_US_CALC(nus);
	__enable_irq();
}

//static __IO u32 TimingDelay;
//void delay_us(__IO u32 nTime)
//{ 
//	TimingDelay = nTime;	

//	// ʹ�ܵδ�ʱ��  
//	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

//	while(TimingDelay != 0);
//}

//void TimingTick_Decrement(void)
//{
//	if (TimingDelay != 0x00)
//	{ 
//		TimingDelay--;
//	}
//}
