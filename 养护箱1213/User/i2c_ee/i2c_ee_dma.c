/**************************************************************************
* i2c_ee_dma.c 
* 2010-12-01 lijie
***************************************************************************/
#include "i2c_ee_dma.h"

/* specified by user  ----------------------------------------------------*/
//#define I2C_REMAP
//#define SLAVE_10BIT_ADDRESS


/* Private define --------------------------------------------------------*/
#if (AT24Cxx<4)	//AT24C02
 	#define I2C_PageSize      8
#elif  (AT24Cxx<=16)//AT24C04, AT24C08, AT24C16
	#define I2C_PageSize      16	
#elif  (AT24Cxx<=64) //AT24C32, AT24C64
	#define I2C_PageSize      32
	#define EE_ADDRESS_NUM	  2
#else
#error "unsupport eeprom"
#endif
	  
#define I2C_Speed_I1		200000		//����Ƶ��
#define I2C_Speed_I2		200000		//����Ƶ��

//#define I2C1_DR_Address        0x40005410
#define I2C2_DR_Address        0x40005810
#define Transmitter             0x00
#define Receiver                0x01

/* Local variables -------------------------------------------------------*/
static vu8 MasterDirection = Transmitter;
static u16 SlaveADDR;
static u16 DeviceOffset=0x0;
static u8 check_begin = 0;
static u8 OffsetDone = 0;

vu8 MasterReceptionComplete = 0;
vu8 MasterTransitionComplete = 0; // to indicat master's send process
vu8 WriteComplete = 0; // to indicat target's internal write process

int I2C_NumByteToWrite = 0;
u8 I2C_NumByteWritingNow = 0;
u8* I2C_pBuffer = 0; 
u16 I2C_WriteAddr = 0;
u8 EE_Addr_Count = 0;//��ַ�ֽ���
/*P-V operation on I2C1 */
static vu8 PV_flag_1;
#define I2C_2
#define I2C_DMA_I2
volatile I2C_STATE i2c_comm_state;

//*************************************************//
// ������  : RCC_Configuration					   //
// ����    : ʱ��ʱ��						   	   //
// Input   : None								   //
// Output  : None								   //
// Return  : None								   //
//*************************************************//
void RCC_Configuration_I2C(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	#ifdef I2C_1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	#endif

	#ifdef I2C_2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	#endif	
}
//*************************************************//
// ������  : GPIO_Configuration					   //
// ����    : ����I/O�˿�						   //
// Input   : None								   //
// Output  : None								   //
// Return  : None								   //
//*************************************************//
void GPIO_Configuration_I2C(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	#ifdef I2C_1
	/* Configure I2C1 pins: SCL��PB6�� and SDA��PB7�� */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

	#ifdef I2C_2
	/* Configure I2C1 pins: SCL��PB6�� and SDA��PB7�� */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif
}

//*************************************************//
// ������  : NVIC_Configuration					   //
// ����    : ����NVICj						       //
// Input   : None								   //
// Output  : None								   //
// Return  : None								   //
//*************************************************//
#ifdef I2C_DMA_I1
void NVIC_Configuration_I2C_1()
{	
	NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	
}
#endif

#ifdef I2C_DMA_I2
void NVIC_Configuration_I2C_2()
{	
	NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	
}
#endif

//*************************************************//
// ������  : I2C_Configuration					   //
// ����    : ����I2C				     		   //
// Input   : None								   //
// Output  : None								   //
// Return  : None								   //
//*************************************************//
void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure; 
	
	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = EEPROM_ADDRESS;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	
	#ifdef I2C_1
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed_I1;
	/* I2C Peripheral Enable */
	I2C_Cmd(I2C1, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);
	#endif

	#ifdef I2C_2
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed_I2;
	/* I2C Peripheral Enable */
	I2C_Cmd(I2C2, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C2, &I2C_InitStructure);
	#endif
}
/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  I2C_EE_Init(void)
{
	//����ʱ��
	RCC_Configuration_I2C();
	//����IO
	GPIO_Configuration_I2C();
	#ifdef I2C_DMA_I1
	//����NVIC
	NVIC_Configuration_I2C_1();
	#endif
	#ifdef I2C_DMA_I2
	//����NVIC
	NVIC_Configuration_I2C_2();
	#endif
	//����I2C
	I2C_Configuration();
//	/******* GPIO configuration and clock enable *********/
//	GPIO_InitTypeDef  GPIO_InitStructure; 
//	I2C_InitTypeDef  I2C_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//0415 RCC_APB2Periph_AFIO
//    
//#ifdef I2C_REMAP
//	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE); 
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
//#else
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
//#endif
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
////    I2C_DeInit(I2C2);
// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//  
//  
//	/*********** I2C periphral configuration **********/
//	I2C_DeInit(I2C2);
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; // fixed
//	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  // fixed
//	I2C_InitStructure.I2C_OwnAddress1 = EEPROM_ADDRESS;//0415 I2C1_ADDRESS7;  // user parameter
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // fixed
//	#ifdef SLAVE_10BIT_ADDRESS  
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;  // user define
//	#else
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	#endif
//	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed; // user parameter
//	I2C_Cmd(I2C2, ENABLE);
//	I2C_Init(I2C2, &I2C_InitStructure);
//  
//  
///************** I2C NVIC configuration *************************/  
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//    NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_ReadBuffer(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
{
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* PV operation */
	if (PV_flag_1 !=0)
	  return;
	PV_flag_1 = 1;
	
	/* DMA initialization */
	
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)I2C2_DR_Address;
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pBuffer; // from function input parameter
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // fixed for receive function
	DMA_InitStructure.DMA_BufferSize = NumByteToRead; // from function input parameter
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // fixed
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // fixed
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte; //fixed
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //fixed
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // fixed
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  // up to user
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // fixed
	
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*initialize static parameter*/
	MasterDirection = Receiver;
	MasterReceptionComplete = 0;
	
	/*initialize static parameter according to input parameter*/ 
	SlaveADDR = EEPROM_ADDRESS;
	DeviceOffset = ReadAddr;
	OffsetDone = 0;
	
	/* global state variable i2c_comm_state */
	i2c_comm_state = COMM_PRE;
	    
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_ERR , ENABLE);//only SB int allowed
	/* Send START condition */
	if(I2C2->CR1 & 0x200)
	I2C2->CR1 &= 0xFDFF;
	I2C_GenerateSTART(I2C2, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to (1-16).
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	/* enter rountine safely */
	
	/* DMA initialization */
	
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)I2C2_DR_Address;
	
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pBuffer; // from function input parameter
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // fixed for send function
	DMA_InitStructure.DMA_BufferSize = NumByteToWrite; // from function input parameter
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // fixed
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // fixed
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte; //fixed
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //fixed
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // fixed
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  // up to user
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // fixed
	
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	/*initialize static parameter*/
	MasterDirection = Transmitter;
	MasterTransitionComplete = 0;
	
	/*initialize static parameter according to input parameter*/ 
	SlaveADDR = EEPROM_ADDRESS; // this byte shoule be send by F/W (in loop or ISR way)
	DeviceOffset = WriteAddr; // this byte can be send by both F/W and DMA
	OffsetDone = 0;
	    
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
	/* Send START condition */
	if(I2C2->CR1 & 0x200)
	I2C2->CR1 &= 0xFDFF;
	I2C_GenerateSTART(I2C2, ENABLE);

}


void I2C_EE_WriteOnePage(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
  
  I2C_NumByteWritingNow=0;
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
  	  I2C_NumByteWritingNow=NumOfSingle;
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
        I2C_NumByteWritingNow=I2C_PageSize;
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_NumByteWritingNow=NumOfSingle;
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_NumByteWritingNow=count;
      }
	  else 
	  {
//	  	printf("address error\r\n");
	  } 
    }
  }  
}

void I2C_EE_WriteOnePageCompleted(void)
{
	I2C_pBuffer += I2C_NumByteWritingNow;
	I2C_WriteAddr += I2C_NumByteWritingNow;
	I2C_NumByteToWrite -= I2C_NumByteWritingNow;
}
/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WriteBuffer(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
	/* PV operation */ 
	if (PV_flag_1 !=0)
	  return;
	PV_flag_1 = 1;
	/* global state variable i2c_comm_state */
	i2c_comm_state = COMM_PRE;

	I2C_pBuffer=pBuffer;
	I2C_WriteAddr=WriteAddr;
	I2C_NumByteToWrite = NumByteToWrite;

	I2C_EE_WriteOnePage(I2C_pBuffer,I2C_WriteAddr,I2C_NumByteToWrite);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitOperationIsCompleted
* Description    : wait operation is completed
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitOperationIsCompleted(void)
{
	while(i2c_comm_state != COMM_DONE);
}	

/**********************  Interrupt Service Routines	 **************************/

void i2c2_evt_isr()
{  
  	uint32_t lastevent=  I2C_GetLastEvent(I2C2);
    switch (lastevent)
    {
/************************** Master Invoke**************************************/
          case I2C_EVENT_MASTER_MODE_SELECT:        /* EV5 */
            // MSL SB BUSY 30001
            if(!check_begin)
              i2c_comm_state = COMM_IN_PROCESS;
            
              if (MasterDirection == Receiver)
              {
                if (!OffsetDone)
				#if (EE_ADDRESS_NUM>1)
				I2C_Send7bitAddress(I2C2,SlaveADDR,I2C_Direction_Transmitter);
				#else
				I2C_Send7bitAddress(I2C2,((DeviceOffset & 0x0700) >>7) | SlaveADDR,
									 I2C_Direction_Transmitter);
				#endif
                      
                else
                {
                  /* Send slave Address for read */
				  #if (EE_ADDRESS_NUM>1)
				  I2C_Send7bitAddress(I2C2, SlaveADDR, I2C_Direction_Receiver); 
				  #else
                  I2C_Send7bitAddress(I2C2, ((DeviceOffset & 0x0700) >>7) | SlaveADDR, I2C_Direction_Receiver);      
                  #endif
				  OffsetDone = 0;

                }
              }
              else
              {
                  /* Send slave Address for write */
				  #if (EE_ADDRESS_NUM>1)
				  I2C_Send7bitAddress(I2C2, SlaveADDR, I2C_Direction_Transmitter);
				  #else
				  I2C_Send7bitAddress(I2C2, ((DeviceOffset & 0x0700) >>7) | SlaveADDR, I2C_Direction_Transmitter);
				  #endif
                  
              }
              I2C_ITConfig(I2C2, I2C_IT_BUF , ENABLE);//also TxE int allowed
              break;
              
/********************** Master Receiver events ********************************/
          case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:  /* EV6 */
            // MSL BUSY ADDR 0x30002
              I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
              
              //without it, no NAK signal on bus after last Data
              //I2C data line would be hold low 
              I2C_DMALastTransferCmd(I2C2, ENABLE);
              
              I2C_DMACmd(I2C2, ENABLE);
              DMA_Cmd(DMA1_Channel5, ENABLE);            
              break;
      
          case I2C_EVENT_MASTER_BYTE_RECEIVED:    /* EV7 */
            // MSL BUSY RXNE 0x30040
              break;
      
/************************* Master Transmitter events **************************/
          case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:     /* EV8 just after EV6 */
            //BUSY, MSL, ADDR, TXE and TRA 0x70082
            if (check_begin)
			{
				check_begin = 0;
				I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF |I2C_IT_ERR, DISABLE);
				I2C_GenerateSTOP(I2C2, ENABLE);
				//!! write over
				I2C_EE_WriteOnePageCompleted();
				if(I2C_NumByteToWrite>0)
				{
					 I2C_EE_WriteOnePage(I2C_pBuffer,I2C_WriteAddr,I2C_NumByteToWrite);
				}
				else
				{
				    WriteComplete = 1;
				    i2c_comm_state = COMM_DONE;
					PV_flag_1 = 0;
				}
				
				break;
			}

             #if (EE_ADDRESS_NUM>1)
				EE_Addr_Count++;
			  	if (EE_Addr_Count < (EE_ADDRESS_NUM))
		        {
		            I2C_SendData(I2C2, DeviceOffset>>8);
		        }
				else
				{
					I2C_SendData(I2C2, DeviceOffset);
				}
			#else
				I2C_SendData(I2C2, DeviceOffset);
				OffsetDone = 1;
			#endif
          break;
              
          case I2C_EVENT_MASTER_BYTE_TRANSMITTING:       /* EV8 I2C_EVENT_MASTER_BYTE_TRANSMITTING*/
              #if (EE_ADDRESS_NUM>1)
			  if(!OffsetDone)
              {
                
	            if (EE_Addr_Count < (EE_ADDRESS_NUM))
	            {
					while ((I2C2->CR1 & 0x200) == 0x200); 
	            	I2C_GenerateSTART(I2C2, ENABLE);	
	            }
				else
				{
					EE_Addr_Count = 0;
					OffsetDone = 1;
				}
				break;
              }
			  #endif
			  if (MasterDirection == Receiver)
              {
                I2C_ITConfig(I2C2, I2C_IT_BUF , DISABLE);
                while ((I2C2->CR1 & 0x200) == 0x200); 
                I2C_GenerateSTART(I2C2, ENABLE);
                break;
              }
              else
              {
                I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
                I2C_DMACmd(I2C2, ENABLE);
                DMA_Cmd(DMA1_Channel4, ENABLE);
                break;
              }
      
          case I2C_EVENT_MASTER_BYTE_TRANSMITTED:       /* EV8-2 */
            //TRA, BUSY, MSL, TXE and BTF 0x70084
              break;
    }
}


void i2c2_err_isr()
{
    if (I2C_GetFlagStatus(I2C2, I2C_FLAG_AF))
    {
      if (check_begin)
        I2C_GenerateSTART(I2C2, ENABLE);
      else if (I2C2->SR2 &0x01)
      {	
	  	//!! receive over
        I2C_GenerateSTOP(I2C2, ENABLE);
        i2c_comm_state = COMM_EXIT;
        PV_flag_1 = 0;
      }
      
      I2C_ClearFlag(I2C2, I2C_FLAG_AF);
    }

    if (I2C_GetFlagStatus(I2C2, I2C_FLAG_BERR))
    {
      if (I2C2->SR2 &0x01)
      {
        I2C_GenerateSTOP(I2C2, ENABLE);
        i2c_comm_state = COMM_EXIT;
        PV_flag_1 = 0;
      }
      
      I2C_ClearFlag(I2C2, I2C_FLAG_BERR);
    }
}



void i2c2_send_dma_isr()
{

  if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    if (I2C2->SR2 & 0x01) // master send DMA finish, check process later
    {
      // DMA1-6 (I2C1 Tx DMA)transfer complete ISR
      I2C_DMACmd(I2C2, DISABLE);
      DMA_Cmd(DMA1_Channel4, DISABLE);
      // wait until BTF
      while (!(I2C2->SR1 & 0x04));
      I2C_GenerateSTOP(I2C2, ENABLE);
      // wait until BUSY clear
      while (I2C2->SR2 & 0x02);
    
      MasterTransitionComplete=1;
      i2c_comm_state = COMM_IN_PROCESS;
      I2C_ITConfig(I2C2, I2C_IT_EVT, ENABLE); // use interrupt to handle check process
      check_begin = 1;
      if(I2C2->CR1 & 0x200)
        I2C2->CR1 &= 0xFDFF;
      I2C_GenerateSTART(I2C2, ENABLE);
    }
    else // slave send DMA finish
    {

    }

    
    DMA_ClearFlag(DMA1_FLAG_TC4);
  }
  if (DMA_GetFlagStatus(DMA1_FLAG_GL4))
  {
    DMA_ClearFlag( DMA1_FLAG_GL4);
  }
    if (DMA_GetFlagStatus(DMA1_FLAG_HT4))
  {
    DMA_ClearFlag( DMA1_FLAG_HT4);
  }
}

void i2c2_receive_dma_isr()
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    if (I2C2->SR2 & 0x01) // master receive DMA finish
    {
      I2C_DMACmd(I2C2, DISABLE);
      I2C_GenerateSTOP(I2C2, ENABLE);
      i2c_comm_state = COMM_DONE;
      MasterReceptionComplete = 1;
      PV_flag_1 =0;
    }
    else // slave receive DMA finish
    {

    }
    DMA_ClearFlag(DMA1_FLAG_TC5);
  }
  if (DMA_GetFlagStatus(DMA1_FLAG_GL5))
  {
    DMA_ClearFlag( DMA1_FLAG_GL5);
  }
    if (DMA_GetFlagStatus(DMA1_FLAG_HT5))
  {
    DMA_ClearFlag( DMA1_FLAG_HT5);
  }
}


/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
  i2c2_send_dma_isr();
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
  i2c2_receive_dma_isr();
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
  i2c2_evt_isr();
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
  i2c2_err_isr();
}


