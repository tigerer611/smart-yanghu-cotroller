/**
  ******************************************************************************
  * @file    bsp_net.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_net.h"

unsigned short net_time05=0;
unsigned short net_time10=0;
unsigned char net_timeover05=0;
unsigned char net_timeover10=0;
vu8 ETH_INT=0;

u8 ipaddress[4]={192,168,3,248};
u8 gateaddress[4]={192,168,3,1};
u8 maskaddress[4]={255,255,255,0};
extern u8 i2c_eeprom_data[48];
extern u8 mymac[6];


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
//A1-->A8
#define ETH_rec_f GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)          //ENC28J60接收帧完成中断脚 

/****************************************************************************
* 名    称：void GPIO_NET_Configuration(void)
* 功    能：通用IO口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
//void GPIO_NET_Configuration(void)
//{
//  
//  GPIO_InitTypeDef GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);

//B5--->C1  	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				     //LED1控制
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOC, &GPIO_InitStructure);					 

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 //LED2, LED3控制
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					 //SST25VF016B SPI片选
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_7;		 //PB12---VS1003 SPI片选（V2.1) 
//  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //PB7---触摸屏芯片XPT2046 SPI 片选
  
  /* 禁止SPI1总线上的其他设备 */
//  GPIO_SetBits(GPIOB, GPIO_Pin_7);						     //触摸屏芯片XPT2046 SPI 片选禁止  
//  GPIO_SetBits(GPIOB, GPIO_Pin_12);						     //VS1003 SPI片选（V2.1)禁止 
//  GPIO_SetBits(GPIOC, GPIO_Pin_4);						     //SST25VF016B SPI片选禁止  


//A1--->A8
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	         	 	//ENC28J60接收完成中断引脚 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//内部上拉输入
//  GPIO_Init(GPIOA, &GPIO_InitStructure);		 


//  GPIO_ResetBits(GPIOE, GPIO_Pin_1);						 //复位ENC28J60
//  Delay(0xAFFF);					   
//  GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 	             
//  Delay(0xAFFF);		 			
//}

/****************************************************************************
* 名    称：void eth_poll(void)
* 功    能：定时查询TCP连接收发状态	ARP表更新， 并响应
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void eth_poll(void){
	unsigned char i=0;
	if(net_timeover05==1)			/* 0.5秒定时器超时 */
	{
		net_time05=0;
		net_timeover05=0;			/* 复位0.5秒定时器 */

		/* 轮流处理每个TCP连接, UIP_CONNS缺省是10个 */
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);		/* 处理TCP通信事件 */
			/*
				当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
				需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
			*/
			if(uip_len > 0)
			{
				uip_arp_out();		//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();		//发送数据到以太网（设备驱动程序）
			}
		}

	#if UIP_UDP
		/* 轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个 */
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);	/*处理UDP通信事件 */
			/* 如果上面的函数调用导致数据应该被发送出去，全局变量uip_len设定值> 0 */
			if(uip_len > 0)
			{
				uip_arp_out();		//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();		//发送数据到以太网（设备驱动程序）
			}
		}
	#endif /* UIP_UDP */

		/* 每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃*/
		if (net_timeover10==1)
		{
			net_time10=0;
			net_timeover10=0;		/* 复位10秒定时器 */
			uip_arp_timer();
		}
	}
}
/*******************************************************************************
*	函数名：UipPro
*	输  入:
*	输  出:
*	功能说明：中断触发读取网络接收缓存
********************************************************************/
void UipPro(void)
{
	if(ETH_INT==1){					//当网络接收到数据时，会产生中断
		rep:;
		ETH_INT=0;
		uip_len = tapdev_read();	//从网络设备读取一个IP包,返回数据长度
		if(uip_len > 0)			    //收到数据
		{
			/* 处理IP数据包(只有校验通过的IP包才会被接收) */
			if(BUF->type == htons(UIP_ETHTYPE_IP))   //是IP包吗？
			{
				uip_arp_ipin();		   //去除以太网头结构，更新ARP表
				uip_input();		   //IP包处理
				/*
					当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
					需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
				*/
				if (uip_len > 0)		//有带外回应数据
				{
					uip_arp_out();		//加以太网头结构，在主动连接时可能要构造ARP请求
					tapdev_send();		//发送数据到以太网（设备驱动程序）
				}
			}
			/* 处理arp报文 */
			else if (BUF->type == htons(UIP_ETHTYPE_ARP))	//是ARP请求包
			{
				uip_arp_arpin();		//如是是ARP回应，更新ARP表；如果是请求，构造回应数据包
				/*
					当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
					需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
				*/
				if (uip_len > 0)		//是ARP请求，要发送回应
				{
					tapdev_send();		//发ARP回应到以太网上
				}
			}
		}
  	}
  	else{	                   //防止大包造成接收死机,当没有产生中断，而ENC28J60中断信号始终为低说明接收死机
  	  if(ETH_rec_f==0) goto rep; 	
  	}
}
/*******************************************************************************
*	函数名：InitNet
*	输  入:
*	输  出:
*	功能说明：初始化网络硬件、UIP协议栈、配置本机IP地址
************************************************************/
void InitNet(void)
{
	uip_ipaddr_t ipaddr;
//	GPIO_NET_Configuration();
	
	tapdev_init();                     		 //ENC28J60初始化
	
//	USART_OUT(USART1,"uip_init\n\r");
	uip_init();								 //UIP协议栈初始化
	
		
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

//	USART_OUT(USART1,"uip ip address : 192,168,3,248\r\n");
	uip_ipaddr(ipaddr, ipaddress[0],ipaddress[1],ipaddress[2],ipaddress[3]);		 //设置IP地址
	uip_sethostaddr(ipaddr);

//	USART_OUT(USART1,"uip route address : 192,168,3,1\r\n");
	uip_ipaddr(ipaddr, gateaddress[0],gateaddress[1],gateaddress[2],gateaddress[3]);		 //设置默认路由器IP地址
	uip_setdraddr(ipaddr);

//	USART_OUT(USART1,"uip net mask : 255,255,255,0\r\n");
	uip_ipaddr(ipaddr, maskaddress[0],maskaddress[1],maskaddress[2],maskaddress[3]);		 //设置网络掩码
	uip_setnetmask(ipaddr);
	
	//20150327从main中移到这里来
	uip_listen(HTONS(1200));
	uip_listen(HTONS(80));	
}
/*********************************************END OF FILE**********************/
