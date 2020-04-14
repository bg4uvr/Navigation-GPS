/* ----------------------------------------------------------
 * 基于T6963C芯片的240x128点阵液晶屏驱动程序
 * --------------------------------------------------------*/
#include "240128.h"
#include "font.c"

//读取状态字节
unsigned char glcd_sget(void)  
{
	unsigned char glcd_status;
	glcd_SET_DATADIR_IN();	
	glcd_cd_high();		// C/D高，读状态字节
	glcd_rd_low();		// RD低，打开读信号
	_NOP();
	_NOP();
	glcd_status = (glcd_DATA_PIN1&0x0f)|(glcd_DATA_PIN2&0xf0);      // 读取状态字节
	glcd_rd_high();         // 关闭读信号
	return(glcd_status);
}

// 写数据
void glcd_dput(unsigned char byte) 
{
	do {} while ((0x03 & glcd_sget()) != 0x03); // 等待屏幕可用
	glcd_SET_DATADIR_OUT(); // 数据口转换为输出口
	glcd_DATA_PORT1 = (glcd_DATA_PORT1&0xf0)|(byte&0x0f);          // 写数据到数据口
	glcd_DATA_PORT2 = (glcd_DATA_PORT2&0x0f)|(byte&0xf0);
	glcd_cd_low();
	glcd_wr_low();         			// 打开写数据方式
	_NOP();
	_NOP();
	glcd_wr_high();                 // WR拉高
}

// 写命令
void glcd_cput(unsigned char byte) 
{
	do {} while ((0x03 & glcd_sget()) != 0x03); // 等待屏幕可用
	glcd_SET_DATADIR_OUT(); // 数据口转换为输出口
	glcd_DATA_PORT1 = (glcd_DATA_PORT1&0xf0)|(byte&0x0f);          // 写数据到数据口
	glcd_DATA_PORT2 = (glcd_DATA_PORT2&0x0f)|(byte&0xf0);
	glcd_cd_high();         // C/D置高
	glcd_wr_low();         	// WR置低
	_NOP();
	_NOP();
	glcd_wr_high();         // WR轩高
}

//设置地址
void glcd_set_address(unsigned int addr){
	glcd_dput(addr%256);
	glcd_dput(addr>>8);
	glcd_cput(0x24);		//0x24为设定地址命令
}

// 图形清屏
extern void glcd_clear_graph(void)    
{
	int i;
	glcd_set_address(glcd_G_BASE);     // 设置地址为图形区基地址
	for (i=0;i<3840;i++)	// 必须为3840
	{
		glcd_dput(0); 		// 写入数据0x00
		glcd_cput(0xc0);    // 0xc0为数据写入、增加指针 命令
	}
}

// 初始化端口
void glcd_setup(void)  
{
	rst_ddr|=_BV(rst_pin);
	rst_port&=~_BV(rst_pin);
	ctl_ddr=_BV(glcd_wr_PIN)|_BV(glcd_rd_PIN)|_BV(glcd_cd_PIN);
	ctl_port=_BV(glcd_wr_PIN)|_BV(glcd_rd_PIN)|_BV(glcd_cd_PIN);
}

// 初始化显存和显示模式
extern void glcd_init(void)  
{
	_delay_ms(20);
	glcd_setup();
	_delay_ms(20);
	rst_port|=_BV(rst_pin);

	glcd_dput(0x00);
	glcd_dput(0x00);
	glcd_cput(0x42);       	// 0x42为设置图形存储区基地址命令
	
	glcd_dput(glcd_BYTES_PER_ROW);
	glcd_dput(0);
	glcd_cput(0x43);  		// 0x43为设置图形模式每行有多少个像素 命令

	glcd_cput(0x80);  		// 模式设置: 异或 使用内部西文字库

	glcd_cput(0x98);		// 显示设置为：  0x9c图形开、文本开  0x98图形  0x94文本 +3光标开、闪
}

/*
*****************************************************************************
* Display_ASCII - 显示一个ASCII
* X为列数<30，Y为行数<8
*****************************************************************************
*/
extern void Display_ASCII(unsigned char Char, unsigned char X, unsigned char Y)
{
    unsigned char i;
	unsigned int addr;
	addr=glcd_G_BASE + (Y*16*glcd_BYTES_PER_ROW)  +  X;				//计算首地址
    for(i=0;i<16;i++){
        glcd_set_address(addr);										//设置地址
        glcd_dput(pgm_read_byte(&chardot_8x16[i+(Char-0x20)*16]));	//写入数据
		glcd_cput(0xc4);											//数据写入，地址不变
		addr+=glcd_BYTES_PER_ROW;
    }
}

/*
*****************************************************************************
* Display_Chinese - 指定位置显示一个16x16的汉字
* DESCRIPTION: -
* @Param UniCode:汉字内码
*****************************************************************************
*/
extern void Display_Chinese(unsigned char id, unsigned char x0, unsigned char y0)
{
	unsigned int addr;
	unsigned char i;

	addr=glcd_G_BASE + (y0*16*glcd_BYTES_PER_ROW)  +  x0;				//计算首地址

    for(i=0;i<32;i+=2){
        glcd_set_address(addr);										//设置地址
        glcd_dput(pgm_read_byte(&hzdot[i+id*32]));	//写入数据
		glcd_cput(0xc0);											//数据写入，地址不变
        glcd_dput(pgm_read_byte(&hzdot[i+1+id*32]));	//写入数据
		glcd_cput(0xc4);											//数据写入，地址不变
		addr+=glcd_BYTES_PER_ROW;
    }
}

/*
*****************************************************************************
指定位置显示一个字符串
*****************************************************************************
*/

extern void DispString(prog_char *s,unsigned char x0, unsigned char y0)
{
    while(pgm_read_byte(s)!='\0'){
		Display_ASCII(pgm_read_byte(s), x0, y0);
		x0++;
		s++;
    }
}

