#include "stm32f10x.h"
#include  "uip.h"  
#include  "enc28j60.h"
#include "uipopt.h"
#include "uip_arch.h"
#include <string.h>	 
#include "stdio.h"
#include "i2c_ee_dma.h"
#include "SHT10.h"
#define RECEIVE_IP_CMD "IPSET"
#define IP_CMD_NUM 5

extern u8 active;
extern u32 time_s_remainder;

extern u8 poweron;

//���縨������
extern u8 mymac[6];
extern u8 ipaddress[4],gateaddress[4],maskaddress[4];

//eeprom�е�ֵ
extern u8 i2c_eeprom_data[48];

/*��ʪ�ȱ���*/
extern value humi_val,temp_val;

/*��ʪ���趨ֵ*/
extern u8 humi_set;
extern u8 temp_set;
extern u8 humi_distance;//�¶�ԣ��
extern u8 temp_distance;//ʪ��ԣ��

extern u8 poweron;

/*����״̬������1����,2��ʪ,4����,8��ʪ�������ǣ�0ֹͣ1����*/
extern u8 work_state;

/*����״̬0Զ�̣�1����*/
extern u8 network_state;

extern u8 request_initnet;

/*����ʱ��*/
extern u16 work_time;

/*ѹ�����޺�����*/
extern float press_up;
extern float press_down;
extern float press_current;



extern void InitNet(void); 

extern unsigned short LPORT;
double cal_test[6]={0};
extern void tcp_demo_appcall(void);
void myudp_appcall(void);



/*******************************************************************************
*	������: u8 CheckPacket(char *str,u8 first,u8 last) 
*	��  ��: 
*	��  ��: У���
*	��  ��: UDP ����У���20150327
**************************************************************************/
char CheckPacket(char *str,u8 first,u8 last) 
{ 

  char sumT=0;
	u8 i;
	for(i=first;i<=last;i++)
		sumT+=str[i];
	return sumT%128;
} 

/*******************************************************************************
*	������: void myudp_send(char *str,short n) 
*	��  ��: 
*	��  ��: ��
*	��  ��: UDP ���ݰ�����
**************************************************************************/
void myudp_send(char *str,short n) 
{ 

   char   *nptr;   
   nptr = (char *)uip_appdata;       
   memcpy(nptr, str, n); 
   uip_udp_send(n);   				//����n������ 
} 

/*******************************************************************************
*	������: void UDP_newdata(void) 
*	��  ��: 
*	��  ��: ��
*	��  ��: UDP ���ݰ�����
**************************************************************************/
void UDP_newdata(void) 
{ 
    char   *nptr;
		char strtmp[35];
    short len; 
	len = uip_datalen();									//��ȡ���ݳ��� 
    nptr = (char *)uip_appdata; 							//ȡ��������ʼָ�� 
	if(len<4)
		return; 
  else if(strncmp(nptr,"GETVER",6)==0) 
	{
		myudp_send("YHXDVERYHX1.0\n",13); 
		//printf("YHXDVERYHX1.0\r\n");
	}
	else if(strncmp(nptr,RECEIVE_IP_CMD,IP_CMD_NUM)==0){//�趨IP��Ϣ20150327
//����			IP												            RIP												                      MASK												                    MAC(������ʱ����)												               У��										
//0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	       53										
//I	P	S	E	T	1	9	2	1	6	8	   0	0	   3	 2	 4	 8	 1	 9	 2	 1	 6	 8	 0	 0	 3	 0	 0	 1	 2	 5	 5	 2	 5	 5	 2	 5	 5	 0	 0	 0	 1	 5	 0	 3	 2	 9	 2	 1	 0	 4	 0	 5	 5-52B�Ͷ�128ȡ��

		//У���յ���ip���ð�
		if(nptr[53]!=CheckPacket(nptr,5,52))
			return;
		//myudp_send("hello",5);
		//��������ȡ��ip��ַ�����ص�ַ�����벢д��eeprom
		//����ip��ַ��Ϣ
		//ip
		i2c_eeprom_data[8]=(nptr[5]-'0')*100+(nptr[6]-'0')*10+nptr[7]-'0';
		i2c_eeprom_data[9]=(nptr[8]-'0')*100+(nptr[9]-'0')*10+nptr[10]-'0';
		i2c_eeprom_data[10]=(nptr[11]-'0')*100+(nptr[12]-'0')*10+nptr[13]-'0';
		i2c_eeprom_data[11]=(nptr[14]-'0')*100+(nptr[15]-'0')*10+nptr[16]-'0';
		ipaddress[0]=i2c_eeprom_data[8];
		ipaddress[1]=i2c_eeprom_data[9];
		ipaddress[2]=i2c_eeprom_data[10];
		ipaddress[3]=i2c_eeprom_data[11];
		//mask
		i2c_eeprom_data[12]=(nptr[29]-'0')*100+(nptr[30]-'0')*10+nptr[31]-'0';
		i2c_eeprom_data[13]=(nptr[32]-'0')*100+(nptr[33]-'0')*10+nptr[34]-'0';
		i2c_eeprom_data[14]=(nptr[35]-'0')*100+(nptr[36]-'0')*10+nptr[37]-'0';
		i2c_eeprom_data[15]=(nptr[38]-'0')*100+(nptr[39]-'0')*10+nptr[40]-'0';
		maskaddress[0]=i2c_eeprom_data[12];
		maskaddress[1]=i2c_eeprom_data[13];
		maskaddress[2]=i2c_eeprom_data[14];
		maskaddress[3]=i2c_eeprom_data[15];
		//gate
		i2c_eeprom_data[16]=(nptr[17]-'0')*100+(nptr[18]-'0')*10+nptr[19]-'0';
		i2c_eeprom_data[17]=(nptr[20]-'0')*100+(nptr[21]-'0')*10+nptr[22]-'0';
		i2c_eeprom_data[18]=(nptr[23]-'0')*100+(nptr[24]-'0')*10+nptr[25]-'0';
		i2c_eeprom_data[19]=(nptr[26]-'0')*100+(nptr[27]-'0')*10+nptr[28]-'0';
		gateaddress[0]=i2c_eeprom_data[16];
		gateaddress[1]=i2c_eeprom_data[17];
		gateaddress[2]=i2c_eeprom_data[18];
		gateaddress[3]=i2c_eeprom_data[19];
		//mac
		i2c_eeprom_data[24]=(nptr[51]-'0')*10+nptr[52]-'0';
		i2c_eeprom_data[25]=(nptr[49]-'0')*10+nptr[50]-'0';
		i2c_eeprom_data[26]=(nptr[47]-'0')*10+nptr[48]-'0';
		i2c_eeprom_data[27]=(nptr[45]-'0')*10+nptr[46]-'0';
		i2c_eeprom_data[28]=(nptr[43]-'0')*10+nptr[44]-'0';
		i2c_eeprom_data[29]=(nptr[41]-'0')*10+nptr[42]-'0';
		mymac[0]=i2c_eeprom_data[24];
		mymac[1]=i2c_eeprom_data[25];
		mymac[2]=i2c_eeprom_data[26];
		mymac[3]=i2c_eeprom_data[27];
		mymac[4]=i2c_eeprom_data[28];
		mymac[5]=i2c_eeprom_data[29];
		//д��eeprom
		//����
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+8, 8, 8);	
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+16, 16, 8);	
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+24, 24, 6);	
		
		
//����			IP												            RIP												                      MASK												                    MAC(������ʱ����)												               У��										
//0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	       53										
//I	P	R	E	T	1	9	2	1	6	8	   0	0	   3	 2	 4	 8	 1	 9	 2	 1	 6	 8	 0	 0	 3	 0	 0	 1	 2	 5	 5	 2	 5	 5	 2	 5	 5	 0	 0	 0	 1	 5	 0	 3	 2	 9	 2	 1	 0	 4	 0	 5	 5-52B�Ͷ�128ȡ��
	  nptr[2]='R';
		myudp_send(nptr,54);//�������ͻ᲻��ʧ�ܣ�

		//printf("%s\r\n",nptr);
		
		//��ʼ�����磬��֮���ͨ��ʧ�ܣ��������豸�����ѡ�񣩣�����������δ���
		//InitNet();
		request_initnet=1;
	} 
	else if(strncmp(nptr,"YHCHK",5)==0){//��������Ϣ��ѯ
		//����Ϣ������ַ������ͳ�ȥ

//��������Ϣ��ѯ����Լ��																																																								//20150422						
//cmd				��ǰ�¶� ��ǰʪ��		    �趨�¶�	 �趨ʪ��		����״̬		  ����״̬		�¶�ԣ��		ʪ��ԣ��	ʣ��ʱ��		      ����̬   �̵���״̬ check																																	
//0	1	2	3	4	5	6	7	8	 9	10	11	12	13	14	   15	16	    17 18	19 20	  21	           22	      23	      24	25	26	27      28       29					30																																		
//Y	H	R	E	T	2	5	.	5	 3	1	   .	 5	 2	 5	    3	 0	     0	0	 0	0	   0	            5	       5	      9	   9	 9	 9       a       1					5-29B�Ͷ�128ȡģ

		
		sprintf(strtmp,"YHRETFF.FFF.F%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",temp_set/10+'0',temp_set%10+'0',humi_set/10+'0',humi_set%10+'0',(work_state&1)?'1':'0',(work_state&2)?'1':'0',(work_state&4)?'1':'0',(work_state&8)?'1':'0',network_state+'0',temp_distance+'0',humi_distance+'0',time_s_remainder/3600/1000+'0',time_s_remainder/3600/100%10+'0',time_s_remainder/3600%100/10+'0',time_s_remainder/3600%10+'0',((active=='a')?'a':'0'),poweron+'0');
		if((active=='a')||(time_s_remainder>0))//����̬20150421
		{
			//��ǰ�¶�ת��
			if((temp_val.f>=0)&&(temp_val.f<100))//����
			{
				strtmp[5]='0'+((u8)temp_val.f)/10;
				strtmp[6]='0'+((u8)temp_val.f)%10;
				strtmp[8]='0'+((u16)(temp_val.f*10))%10;
			}else if((temp_val.f>-10)&&(temp_val.f<0))//����
			{
				strtmp[5]='-';
				strtmp[6]=((u8)(-temp_val.f))%10+'0';
				strtmp[8]='0'+((u16)(-temp_val.f*10))%10;
			}
			
			//��ǰʪ��ת��
			if((humi_val.f>=0)&&(humi_val.f<100))
			{
				strtmp[9]='0'+((u8)humi_val.f)/10;
				strtmp[10]='0'+((u8)humi_val.f)%10;
				strtmp[12]='0'+((u8)(humi_val.f*10))%10;
			}else if((humi_val.f<0))//20150802
			{
				strtmp[9]='F';
				strtmp[10]='F';
				strtmp[12]='F';
			}else if(humi_val.f>=100)
			{
				strtmp[9]='9';
				strtmp[10]='9';
				strtmp[12]='9';
			}
		}else
		{
			strtmp[5]='0';
			strtmp[9]='0';
		}
		
		strtmp[30]=CheckPacket(strtmp,5,29);
		//����

		myudp_send(strtmp,31);
		//printf("%s\r\n",strtmp);
		
	}
	else if(strncmp(nptr,"YHTHSET",7)==0){//��������λ����ʪ�ȼ�ԣ����������


//cmd						�¶��趨 ʪ���趨	�¶�ԣ��	ʪ��ԣ��	check
//0	1	2	3	4	5	6	7	8	     9	10	    11	      12	     13		
//Y	H	T	H	S	E	T	2	5	     3	 0	     5	       5	     7-12B�Ͷ�128ȡģ		

		//У���յ������ݰ�
		if(nptr[13]!=CheckPacket(nptr,7,12))
			return;
		//�������ݰ�������ֵ
		temp_set=(nptr[7]-'0')*10+nptr[8]-'0';
		humi_set=(nptr[9]-'0')*10+nptr[10]-'0';
		temp_distance=nptr[11]-'0';
		humi_distance=nptr[12]-'0';
		nptr[4]='R';
		//sprintf(strtmp,"YHTHRET%c%c%c%c%c",)
		myudp_send(nptr,14);
		//20150522 ���˼��ˡ�����
		i2c_eeprom_data[2]=humi_set;
		i2c_eeprom_data[1]=temp_set;
		i2c_eeprom_data[4]=humi_distance;
		i2c_eeprom_data[3]=temp_distance;
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+1, 1, 4);	
		//printf("%s\r\n",strtmp);
	}else //20150421
	if(strncmp(nptr,"POWERON",7)==0)//����
	{
		/*
		cmd						
0	1	2	3	4	5	6
P	O	W	E	R	O	N
		*/
		if(network_state==1)//����ܼ��ϣ����ʧ�ܣ��з���0716����λ���͵ؽ��ÿ���/�ػ���ť
			poweron=1;
	}else
	if(strncmp(nptr,"SHUTDOWN",8)==0)//�ػ�
	{
		/*
		cmd							
0	1	2	3	4	5	6	7
S	H	U	T	D	O	W	N
		*/
		if(network_state==1)//����ܼ��ϣ����ʧ�ܣ��з���0716����λ���͵ؽ��ÿ���/�ػ���ť
			poweron=0;
	}else
	if(strncmp(nptr,"USERACTIVE",10)==0)//���ü���
	{
		/*
		cmd							���ü���ʱ��(Сʱ)				У���
0	1	2	3	4	5	6	7	8	9	10	11	12	13							14
U	S	E	R	A	C	T	I	V	E	1		0		0		0							10-13B�Ͷ�128ȡģ		
		*/
		if(nptr[14]!=CheckPacket(nptr,10,13))
			return;
		time_s_remainder=((nptr[10]-'0')*1000+(nptr[11]-'0')*100+(nptr[12]-'0')*10+nptr[13]-'0')*3600;
		nptr[9]='R';//����9B��ΪR
		i2c_eeprom_data[32]=(u8)time_s_remainder;
		i2c_eeprom_data[33]=(u8)(time_s_remainder>>8);
		i2c_eeprom_data[34]=(u8)(time_s_remainder>>16);
		i2c_eeprom_data[35]=(u8)(time_s_remainder>>24);
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+32, 32, 4);	
		myudp_send(nptr,15);
	}else 
	if(strncmp(nptr,"FULLACTIVE",10)==0)//��ȫ����
	{
		/*
		cmd										�����(8λ����ʱд���һ�޶�)								У���
0	1	2	3	4	5	6	7	8	9				10	11	12	13	14	15	16	17								18
F	U	L	L	A	C	T	I	V	E				1		0		0		0		1		0		0		0									10-17B�Ͷ�128ȡģ
		*/
		if(nptr[18]!=CheckPacket(nptr,10,17))
			return;
		//��ȡeeprom�е������뼤��űȽϣ�������ͬ�򼤻���ǲ�ͬ��ô��������㷵��
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_ReadBuffer(i2c_eeprom_data+40, 40, 8);	
		for(len=0;len<8;len++)
			if(i2c_eeprom_data[40+len]!=(u8)nptr[10+len])
				break;
		if(len<8)//����Ų�ƥ��
		{
			nptr[9]='F';//����ʧ�ܷ���
			myudp_send(nptr,19);
		}else//ƥ��
		{
			active='a';
			i2c_eeprom_data[36]='a';
			I2C_EE_WaitOperationIsCompleted();
			I2C_EE_WriteBuffer(i2c_eeprom_data+36, 36, 1);	
			nptr[9]='R';//����ɹ�
			myudp_send(nptr,19);
		}
		
	}else
	if(strncmp(nptr,"LICENCE",7)==0)//�����д������
	{
		/*
		����																У���
0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
L	I	C	E	N	C	E	1	2	3	4		5		6		7		8		7-14B�Ͷ�128ȡģ
		*/
		if(nptr[15]!=CheckPacket(nptr,7,14))
			return;
		for(len=0;len<8;len++)
			i2c_eeprom_data[40+len]=(u8)nptr[7+len];
		//д�뼤���
		I2C_EE_WaitOperationIsCompleted();
		I2C_EE_WriteBuffer(i2c_eeprom_data+40, 40, 8);	
		nptr[6]='R';
		myudp_send(nptr,16);
		
	}//�������ݶ���	
} 
/*******************************************************************************
*	������: void myudp_appcall(void) ) 
*	��  ��: 
*	��  ��: ��
*	��  ��: UDP������
**************************************************************************/
void myudp_appcall(void) 
{ 
   if(uip_newdata()) 						
   { 
       UDP_newdata();      
   } 
} 

/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/

