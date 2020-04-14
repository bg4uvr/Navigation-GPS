/* ----------------------------------------------------------
 * ����T6963CоƬ��240x128����Һ������������
 * --------------------------------------------------------*/
#include "240128.h"
#include "font.c"

//��ȡ״̬�ֽ�
unsigned char glcd_sget(void)  
{
	unsigned char glcd_status;
	glcd_SET_DATADIR_IN();	
	glcd_cd_high();		// C/D�ߣ���״̬�ֽ�
	glcd_rd_low();		// RD�ͣ��򿪶��ź�
	_NOP();
	_NOP();
	glcd_status = (glcd_DATA_PIN1&0x0f)|(glcd_DATA_PIN2&0xf0);      // ��ȡ״̬�ֽ�
	glcd_rd_high();         // �رն��ź�
	return(glcd_status);
}

// д����
void glcd_dput(unsigned char byte) 
{
	do {} while ((0x03 & glcd_sget()) != 0x03); // �ȴ���Ļ����
	glcd_SET_DATADIR_OUT(); // ���ݿ�ת��Ϊ�����
	glcd_DATA_PORT1 = (glcd_DATA_PORT1&0xf0)|(byte&0x0f);          // д���ݵ����ݿ�
	glcd_DATA_PORT2 = (glcd_DATA_PORT2&0x0f)|(byte&0xf0);
	glcd_cd_low();
	glcd_wr_low();         			// ��д���ݷ�ʽ
	_NOP();
	_NOP();
	glcd_wr_high();                 // WR����
}

// д����
void glcd_cput(unsigned char byte) 
{
	do {} while ((0x03 & glcd_sget()) != 0x03); // �ȴ���Ļ����
	glcd_SET_DATADIR_OUT(); // ���ݿ�ת��Ϊ�����
	glcd_DATA_PORT1 = (glcd_DATA_PORT1&0xf0)|(byte&0x0f);          // д���ݵ����ݿ�
	glcd_DATA_PORT2 = (glcd_DATA_PORT2&0x0f)|(byte&0xf0);
	glcd_cd_high();         // C/D�ø�
	glcd_wr_low();         	// WR�õ�
	_NOP();
	_NOP();
	glcd_wr_high();         // WR����
}

//���õ�ַ
void glcd_set_address(unsigned int addr){
	glcd_dput(addr%256);
	glcd_dput(addr>>8);
	glcd_cput(0x24);		//0x24Ϊ�趨��ַ����
}

// ͼ������
extern void glcd_clear_graph(void)    
{
	int i;
	glcd_set_address(glcd_G_BASE);     // ���õ�ַΪͼ��������ַ
	for (i=0;i<3840;i++)	// ����Ϊ3840
	{
		glcd_dput(0); 		// д������0x00
		glcd_cput(0xc0);    // 0xc0Ϊ����д�롢����ָ�� ����
	}
}

// ��ʼ���˿�
void glcd_setup(void)  
{
	rst_ddr|=_BV(rst_pin);
	rst_port&=~_BV(rst_pin);
	ctl_ddr=_BV(glcd_wr_PIN)|_BV(glcd_rd_PIN)|_BV(glcd_cd_PIN);
	ctl_port=_BV(glcd_wr_PIN)|_BV(glcd_rd_PIN)|_BV(glcd_cd_PIN);
}

// ��ʼ���Դ����ʾģʽ
extern void glcd_init(void)  
{
	_delay_ms(20);
	glcd_setup();
	_delay_ms(20);
	rst_port|=_BV(rst_pin);

	glcd_dput(0x00);
	glcd_dput(0x00);
	glcd_cput(0x42);       	// 0x42Ϊ����ͼ�δ洢������ַ����
	
	glcd_dput(glcd_BYTES_PER_ROW);
	glcd_dput(0);
	glcd_cput(0x43);  		// 0x43Ϊ����ͼ��ģʽÿ���ж��ٸ����� ����

	glcd_cput(0x80);  		// ģʽ����: ��� ʹ���ڲ������ֿ�

	glcd_cput(0x98);		// ��ʾ����Ϊ��  0x9cͼ�ο����ı���  0x98ͼ��  0x94�ı� +3��꿪����
}

/*
*****************************************************************************
* Display_ASCII - ��ʾһ��ASCII
* XΪ����<30��YΪ����<8
*****************************************************************************
*/
extern void Display_ASCII(unsigned char Char, unsigned char X, unsigned char Y)
{
    unsigned char i;
	unsigned int addr;
	addr=glcd_G_BASE + (Y*16*glcd_BYTES_PER_ROW)  +  X;				//�����׵�ַ
    for(i=0;i<16;i++){
        glcd_set_address(addr);										//���õ�ַ
        glcd_dput(pgm_read_byte(&chardot_8x16[i+(Char-0x20)*16]));	//д������
		glcd_cput(0xc4);											//����д�룬��ַ����
		addr+=glcd_BYTES_PER_ROW;
    }
}

/*
*****************************************************************************
* Display_Chinese - ָ��λ����ʾһ��16x16�ĺ���
* DESCRIPTION: -
* @Param UniCode:��������
*****************************************************************************
*/
extern void Display_Chinese(unsigned char id, unsigned char x0, unsigned char y0)
{
	unsigned int addr;
	unsigned char i;

	addr=glcd_G_BASE + (y0*16*glcd_BYTES_PER_ROW)  +  x0;				//�����׵�ַ

    for(i=0;i<32;i+=2){
        glcd_set_address(addr);										//���õ�ַ
        glcd_dput(pgm_read_byte(&hzdot[i+id*32]));	//д������
		glcd_cput(0xc0);											//����д�룬��ַ����
        glcd_dput(pgm_read_byte(&hzdot[i+1+id*32]));	//д������
		glcd_cput(0xc4);											//����д�룬��ַ����
		addr+=glcd_BYTES_PER_ROW;
    }
}

/*
*****************************************************************************
ָ��λ����ʾһ���ַ���
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

