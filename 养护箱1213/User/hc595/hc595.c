#include "hc595.h"
#include "delay.h"
//uint8_t seg_table_p[]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6};
//uint8_t seg_table_n[]={0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1F,0x01,0x09};
uint8_t seg_table_p[]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6};
uint8_t seg_table_n[]={0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1F,0x01,0x09,0xFD,0xFF,0x71};
/*                       0   1     2    3     4   5     6    7   8    9   10-  11灭 12F  不加dp*/
/*段选方式abcdefg(dp)由高到低，共阳（共片选），片选供电，要想显示dp那么&0xfe即可*/
void hc595_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
//段初始化
  GPIO_InitStructure.GPIO_Pin = HC595_DATA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(HC595_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = HC595_SCLK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(HC595_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = HC595_RCLK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(HC595_PORT, &GPIO_InitStructure);

  GPIO_ResetBits(HC595_PORT, HC595_DATA);
  GPIO_ResetBits(HC595_PORT, HC595_SCLK);
  GPIO_ResetBits(HC595_PORT, HC595_RCLK);
//位初始化
//	GPIO_InitStructure.GPIO_Pin = HC595_DATA10;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(HC595_PORT10, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = HC595_SCLK10;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(HC595_PORT10, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = HC595_RCLK10;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(HC595_PORT10, &GPIO_InitStructure);

//  GPIO_ResetBits(HC595_PORT10, HC595_DATA10);
//  GPIO_ResetBits(HC595_PORT10, HC595_SCLK10);
//  GPIO_ResetBits(HC595_PORT10, HC595_RCLK10);
	

}

void hc595_write_byte(uint8_t byte)
{
  uint8_t i, num;
  num = byte;
  for (i=0; i<8; i++)
  {
    GPIO_ResetBits(HC595_PORT, HC595_SCLK);
    if ((num&0x01)==0x01)
      GPIO_SetBits(HC595_PORT, HC595_DATA);
    else
      GPIO_ResetBits(HC595_PORT, HC595_DATA);
    GPIO_SetBits(HC595_PORT, HC595_SCLK);
    num = num>>1;
  }
}

void hc595_write_byte10(uint16_t byte)
{
  uint16_t i, num;
  num = byte<<6;
  for (i=0; i<16; i++)
  {
    GPIO_ResetBits(HC595_PORT, HC595_SCLK);
    if ((u16)(num&0x01)==0x01)
      GPIO_SetBits(HC595_PORT, HC595_DATA);
    else
      GPIO_ResetBits(HC595_PORT, HC595_DATA);
    GPIO_SetBits(HC595_PORT, HC595_SCLK);
    num = num>>1;
  }
}

void hc595_latch(void)
{
	//锁存段
//  GPIO_ResetBits(HC595_PORT, HC595_RCLK);
//  GPIO_SetBits(HC595_PORT, HC595_RCLK);
//  GPIO_ResetBits(HC595_PORT, HC595_RCLK);
//	//锁存位
//	GPIO_ResetBits(HC595_PORT10, HC595_RCLK10);
//  GPIO_SetBits(HC595_PORT10, HC595_RCLK10);
//  GPIO_ResetBits(HC595_PORT10, HC595_RCLK10);
	GPIO_ResetBits(HC595_PORT, HC595_RCLK);
//	GPIO_ResetBits(HC595_PORT10, HC595_RCLK10);
	
  GPIO_SetBits(HC595_PORT, HC595_RCLK);
//	GPIO_SetBits(HC595_PORT10, HC595_RCLK10);
	
  GPIO_ResetBits(HC595_PORT, HC595_RCLK);
//	GPIO_ResetBits(HC595_PORT10, HC595_RCLK10);
	
}

void hc595_display(uint8_t data[])
{
	//G9
  hc595_write_byte10(0x01);
  hc595_write_byte(seg_table_n[data[9]]);
  hc595_latch();
  HC595_DELAY
	//G8
  hc595_write_byte10(0x02);
  hc595_write_byte(seg_table_n[data[8]]);
  hc595_latch();
  HC595_DELAY
	//G7
  hc595_write_byte10(0x04);
  hc595_write_byte(seg_table_n[data[7]]);
  hc595_latch();
  HC595_DELAY
	//G6
  hc595_write_byte10(0x08);
  hc595_write_byte(seg_table_n[data[6]]);
  hc595_latch();
  HC595_DELAY
	//G5
  hc595_write_byte10(0x10);
  hc595_write_byte(seg_table_n[data[5]]);
  hc595_latch();
  HC595_DELAY
	//G4
  hc595_write_byte10(0x20);
  hc595_write_byte(seg_table_n[data[4]]&0xfe);
  hc595_latch();
  HC595_DELAY
	//G3
  hc595_write_byte10(0x40);
  hc595_write_byte(seg_table_n[data[3]]);
  hc595_latch();
  HC595_DELAY
	//G2
  hc595_write_byte10(0x80);
  hc595_write_byte(seg_table_n[data[2]]);
  hc595_latch();
  HC595_DELAY
	//G1
	hc595_write_byte10(0x100);
  hc595_write_byte(seg_table_n[data[1]]&0xfe);
  hc595_latch();
  HC595_DELAY
	//G0
	hc595_write_byte10(0x200);
  hc595_write_byte(seg_table_n[data[0]]);
  hc595_latch();
  HC595_DELAY
}
void display_test()
{
		hc595_write_byte10(0x3fe);
	hc595_write_byte(0x9f);
	hc595_latch();
	HC595_DELAY

	hc595_write_byte10(1);
	hc595_write_byte(0x03);
	hc595_latch();
	HC595_DELAY
}

