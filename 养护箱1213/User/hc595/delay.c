#include "delay.h"
#include "stm32f10x.h"

#define  fac_us 9//us延时倍乘数
#define fac_ms 9000//ms延时倍乘数


enum CALC_TYPE {CALC_TYPE_S,CALC_TYPE_MS,CALC_TYPE_US};

static void TIM5_Init_Query(enum CALC_TYPE type)  
{  
	TIM_TimeBaseInitTypeDef Tim5;  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  
	Tim5.TIM_Period=1; //???  
	if(type==CALC_TYPE_S) //延时以S为单位时，时钟频率57600Hz，外部需要1250次计时  
	{  
		Tim5.TIM_Prescaler=57600-1; //预分频 72MHz / 57600= 1250Hz  
	}else if(type==CALC_TYPE_MS)  
	{  
		Tim5.TIM_Prescaler=2880-1; //25000Hz ,定时器计数25次为ms  
	}else if(type==CALC_TYPE_US)  
	{     
		Tim5.TIM_Prescaler=72-1; //1MHz ,计数1次为us  
	}else  
	{  
		Tim5.TIM_Prescaler=7200-1;  
	}  
	Tim5.TIM_ClockDivision=0;  
	Tim5.TIM_CounterMode=TIM_CounterMode_Down; //向下计数  
	TIM_TimeBaseInit(TIM5,&Tim5);         
}  
      
static void TIM5_S_CALC(uint32_t s)  
{  
	u16 counter=(s*1250)&0xFFFF; //前提定时器时钟为1250Hz  
	TIM_Cmd(TIM5,ENABLE);  
	TIM_SetCounter(TIM5,counter); //设置计数值  
	  
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
	TIM_SetCounter(TIM5,counter); //设置计数值  
	  
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
	TIM_SetCounter(TIM5,counter); //设置计数值  
  
	while(counter>1)  
	{  
		counter=TIM_GetCounter(TIM5);  
	}  
	TIM_Cmd(TIM5,DISABLE);  
}  
  

//	TIM5_Init_Query(CALC_TYPE_US);  





//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void DelayInit()
{
	TIM5_Init_Query(CALC_TYPE_US); 
}

//延时nus
//nus为要延时的us数.		


//void delay_us(u32 nus)
//{		
//	u32 ticks;
//	u32 told,tnow,tcnt=0;
//	u32 reload=SysTick->LOAD; //LOAD的值       
//	ticks=nus*fac_us;  //需要的节拍数     
//	tcnt=0;
//	__disable_irq();
//	told=SysTick->VAL;         //刚进入时的计数器值
//	while(1)
//	{
//		tnow=SysTick->VAL;
//		if(tnow!=told)
//		{     
//			if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
//			else tcnt+=reload-tnow+told;     
//			told=tnow;
//			if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
//		}  
//	}
//	__enable_irq();
//}


//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
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

//	// 使能滴答定时器  
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
