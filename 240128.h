#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <compat/ina90.h>   //����_NOP(),_CLI(),_SEI()

//���Ŷ���
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

/* ----- ����Һ�����ڲ�RAM  ------ */
//#define glcd_T_BASE 0x000            // �ı����ڴ��׵�ַ
#define glcd_G_BASE 0x000            // ͼ�����ڴ��׵�ַ
#define glcd_BYTES_PER_ROW 30         // ������Ļ�ı����Ϊ30�ֽ�

//�����
#define glcd_wr_high() ctl_port  |=  _BV(glcd_wr_PIN)
#define glcd_wr_low()  ctl_port  &= ~_BV(glcd_wr_PIN)
#define glcd_rd_high() ctl_port  |=  _BV(glcd_rd_PIN)
#define glcd_rd_low()  ctl_port  &= ~_BV(glcd_rd_PIN)
#define glcd_cd_high() ctl_port  |=  _BV(glcd_cd_PIN)
#define glcd_cd_low()  ctl_port  &= ~_BV(glcd_cd_PIN)

#define glcd_SET_DATADIR_OUT()	{glcd_DATADIR1 |= 0x0f; glcd_DATADIR2 |= 0xf0;}
#define glcd_SET_DATADIR_IN()	{glcd_DATADIR1 &= 0xf0; glcd_DATADIR2 &= 0x0f; glcd_DATA_PORT1|=0x0f; glcd_DATA_PORT2|=0xf0;}

// ��ͼ��������
#define glcd_XMAX 240        
#define glcd_XMIN 0
#define glcd_YMAX 128
#define glcd_YMIN 0

//��������
extern void glcd_init(void);     			 				// ��ʼ���Դ����ʾ��ʽ

extern void glcd_clear_graph(void);    					// ͼ������

extern void Display_ASCII(unsigned char Char, unsigned char X, unsigned char Y);
extern void DispString(prog_char *s,unsigned char x0, unsigned char y0);
extern void Display_Chinese(unsigned char id, unsigned char x0, unsigned char y0);
