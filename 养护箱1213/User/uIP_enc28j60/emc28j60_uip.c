#include "stm32f10x.h"
#include  "uip.h"  
#include  "enc28j60.h"
#include "uipopt.h"
#include "uip_arch.h"
#include <string.h>	 

//用于固定IP地址开关打开后的IP设置，本例程没有用这个
#define UIP_DRIPADDR0   192
#define UIP_DRIPADDR1   168
#define UIP_DRIPADDR2   1
#define UIP_DRIPADDR3   15

void etherdev_init(void);
//extern void httpd_appcall(void);
extern void tcp_demo_appcall(void);
/*=============================================================================
  系统全域的变量
  =============================================================================*/
#define ETH_ADDR_LEN			6

u8 mymac[6] = {0x04,0x02,0x35,0x00,0x00,0x01};  //MAC地址	   
/*******************************************************************************
*	函数名: etherdev_init
*	参  数: 无
*	返  回: 无
*	功  能: uIP 接口函数,初始化网卡
********************************************************************************/
void etherdev_init(void)
{	 u8 i;	 
	/*初始化 enc28j60*/
	enc28j60Init(mymac);					  
	//把IP地址和MAC地址写入各自的缓存区	ipaddr[] macaddr[]
	//init_ip_arp_udp_tcp(mymac,myip,mywwwport);
	for (i = 0; i < 6; i++)
	{
		uip_ethaddr.addr[i] = mymac[i];
	}
    //指示灯状态:0x476 is PHLCON LEDA(绿)=links status, LEDB(红)=receive/transmit
    //enc28j60PhyWrite(PHLCON,0x7a4);
	//PHLCON：PHY 模块LED 控制寄存器	
	enc28j60PhyWrite(PHLCON,0x0476);	
	enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz

}


/*******************************************************************************
*	函数名: void tcp_server_appcall(void)
*	参  数: 
*	返  回: 无
*	功  能: 应用接口函数--完成TCP服务器和HTTP服务器
**************************************************************************/
void tcp_server_appcall(void){		
	switch(uip_conn->lport) 
	{
//		case HTONS(80):			           //http服务器
//			httpd_appcall(); 
//			break;
		case HTONS(1200):				   //TCP服务器
		    tcp_demo_appcall(); 
			break;
	}
}

/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/

