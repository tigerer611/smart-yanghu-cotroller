/**
  ******************************************************************************
  * @file    bsp_net.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
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
#define ETH_rec_f GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)          //ENC28J60����֡����жϽ� 

/****************************************************************************
* ��    �ƣ�void GPIO_NET_Configuration(void)
* ��    �ܣ�ͨ��IO������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
//void GPIO_NET_Configuration(void)
//{
//  
//  GPIO_InitTypeDef GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);

//B5--->C1  	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				     //LED1����
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOC, &GPIO_InitStructure);					 

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 //LED2, LED3����
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;					 //SST25VF016B SPIƬѡ
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_7;		 //PB12---VS1003 SPIƬѡ��V2.1) 
//  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //PB7---������оƬXPT2046 SPI Ƭѡ
  
  /* ��ֹSPI1�����ϵ������豸 */
//  GPIO_SetBits(GPIOB, GPIO_Pin_7);						     //������оƬXPT2046 SPI Ƭѡ��ֹ  
//  GPIO_SetBits(GPIOB, GPIO_Pin_12);						     //VS1003 SPIƬѡ��V2.1)��ֹ 
//  GPIO_SetBits(GPIOC, GPIO_Pin_4);						     //SST25VF016B SPIƬѡ��ֹ  


//A1--->A8
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	         	 	//ENC28J60��������ж����� 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//�ڲ���������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);		 


//  GPIO_ResetBits(GPIOE, GPIO_Pin_1);						 //��λENC28J60
//  Delay(0xAFFF);					   
//  GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 	             
//  Delay(0xAFFF);		 			
//}

/****************************************************************************
* ��    �ƣ�void eth_poll(void)
* ��    �ܣ���ʱ��ѯTCP�����շ�״̬	ARP����£� ����Ӧ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void eth_poll(void){
	unsigned char i=0;
	if(net_timeover05==1)			/* 0.5�붨ʱ����ʱ */
	{
		net_time05=0;
		net_timeover05=0;			/* ��λ0.5�붨ʱ�� */

		/* ��������ÿ��TCP����, UIP_CONNSȱʡ��10�� */
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);		/* ����TCPͨ���¼� */
			/*
				������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
				��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
			*/
			if(uip_len > 0)
			{
				uip_arp_out();		//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();		//�������ݵ���̫�����豸��������
			}
		}

	#if UIP_UDP
		/* ��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10�� */
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);	/*����UDPͨ���¼� */
			/* �������ĺ������õ�������Ӧ�ñ����ͳ�ȥ��ȫ�ֱ���uip_len�趨ֵ> 0 */
			if(uip_len > 0)
			{
				uip_arp_out();		//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();		//�������ݵ���̫�����豸��������
			}
		}
	#endif /* UIP_UDP */

		/* ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����*/
		if (net_timeover10==1)
		{
			net_time10=0;
			net_timeover10=0;		/* ��λ10�붨ʱ�� */
			uip_arp_timer();
		}
	}
}
/*******************************************************************************
*	��������UipPro
*	��  ��:
*	��  ��:
*	����˵�����жϴ�����ȡ������ջ���
********************************************************************/
void UipPro(void)
{
	if(ETH_INT==1){					//��������յ�����ʱ��������ж�
		rep:;
		ETH_INT=0;
		uip_len = tapdev_read();	//�������豸��ȡһ��IP��,�������ݳ���
		if(uip_len > 0)			    //�յ�����
		{
			/* ����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) */
			if(BUF->type == htons(UIP_ETHTYPE_IP))   //��IP����
			{
				uip_arp_ipin();		   //ȥ����̫��ͷ�ṹ������ARP��
				uip_input();		   //IP������
				/*
					������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
					��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
				*/
				if (uip_len > 0)		//�д����Ӧ����
				{
					uip_arp_out();		//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
					tapdev_send();		//�������ݵ���̫�����豸��������
				}
			}
			/* ����arp���� */
			else if (BUF->type == htons(UIP_ETHTYPE_ARP))	//��ARP�����
			{
				uip_arp_arpin();		//������ARP��Ӧ������ARP����������󣬹����Ӧ���ݰ�
				/*
					������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
					��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
				*/
				if (uip_len > 0)		//��ARP����Ҫ���ͻ�Ӧ
				{
					tapdev_send();		//��ARP��Ӧ����̫����
				}
			}
		}
  	}
  	else{	                   //��ֹ�����ɽ�������,��û�в����жϣ���ENC28J60�ж��ź�ʼ��Ϊ��˵����������
  	  if(ETH_rec_f==0) goto rep; 	
  	}
}
/*******************************************************************************
*	��������InitNet
*	��  ��:
*	��  ��:
*	����˵������ʼ������Ӳ����UIPЭ��ջ�����ñ���IP��ַ
************************************************************/
void InitNet(void)
{
	uip_ipaddr_t ipaddr;
//	GPIO_NET_Configuration();
	
	tapdev_init();                     		 //ENC28J60��ʼ��
	
//	USART_OUT(USART1,"uip_init\n\r");
	uip_init();								 //UIPЭ��ջ��ʼ��
	
		
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
	uip_ipaddr(ipaddr, ipaddress[0],ipaddress[1],ipaddress[2],ipaddress[3]);		 //����IP��ַ
	uip_sethostaddr(ipaddr);

//	USART_OUT(USART1,"uip route address : 192,168,3,1\r\n");
	uip_ipaddr(ipaddr, gateaddress[0],gateaddress[1],gateaddress[2],gateaddress[3]);		 //����Ĭ��·����IP��ַ
	uip_setdraddr(ipaddr);

//	USART_OUT(USART1,"uip net mask : 255,255,255,0\r\n");
	uip_ipaddr(ipaddr, maskaddress[0],maskaddress[1],maskaddress[2],maskaddress[3]);		 //������������
	uip_setnetmask(ipaddr);
	
	//20150327��main���Ƶ�������
	uip_listen(HTONS(1200));
	uip_listen(HTONS(80));	
}
/*********************************************END OF FILE**********************/
