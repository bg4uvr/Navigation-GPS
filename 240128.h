#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <compat/ina90.h>   //定义_NOP(),_CLI(),_SEI()

//引脚定义
#define rst_pin			PC5
#define rst_ddr			DDRC
#define rst_port		PORTC

#define glcd_rd_PIN		PB0
#define glcd_wr_PIN		PB1
#define glcd_cd_PIN		PB2

#define ctl_ddr			DDRB
#define ctl_port		PORTB

#define glcd_DATA_PORT1 	PORTC
#define glcd_DATA_PIN1		PINC
#define glcd_DATADIR1		DDRC

#define glcd_DATA_PORT2 	PORTD
#define glcd_DATA_PIN2		PIND
#define glcd_DATADIR2		DDRD

/* ----- 定义液晶屏内部RAM  ------ */
//#define glcd_T_BASE 0x000            // 文本区内存首地址
#define glcd_G_BASE 0x000            // 图形区内存首地址
#define glcd_BYTES_PER_ROW 30         // 定义屏幕文本宽度为30字节

//定义宏
#define glcd_wr_high() ctl_port  |=  _BV(glcd_wr_PIN)
#define glcd_wr_low()  ctl_port  &= ~_BV(glcd_wr_PIN)
#define glcd_rd_high() ctl_port  |=  _BV(glcd_rd_PIN)
#define glcd_rd_low()  ctl_port  &= ~_BV(glcd_rd_PIN)
#define glcd_cd_high() ctl_port  |=  _BV(glcd_cd_PIN)
#define glcd_cd_low()  ctl_port  &= ~_BV(glcd_cd_PIN)

#define glcd_SET_DATADIR_OUT()	{glcd_DATADIR1 |= 0x0f; glcd_DATADIR2 |= 0xf0;}
#define glcd_SET_DATADIR_IN()	{glcd_DATADIR1 &= 0xf0; glcd_DATADIR2 &= 0x0f; glcd_DATA_PORT1|=0x0f; glcd_DATA_PORT2|=0xf0;}

// 绘图坐标限制
#define glcd_XMAX 240        
#define glcd_XMIN 0
#define glcd_YMAX 128
#define glcd_YMIN 0

//声明函数
extern void glcd_init(void);     			 				// 初始化显存和显示方式

extern void glcd_clear_graph(void);    					// 图形清屏

extern void Display_ASCII(unsigned char Char, unsigned char X, unsigned char Y);
extern void DispString(prog_char *s,unsigned char x0, unsigned char y0);
extern void Display_Chinese(unsigned char id, unsigned char x0, unsigned char y0);
