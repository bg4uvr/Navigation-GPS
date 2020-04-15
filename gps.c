
//����ͷ�ļ�
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "gps.h"

//GPS���ݴ洢����
volatile unsigned char JD[10]={"00000.0000"};	//����
volatile unsigned char JD_a='E';				//���ȷ���
volatile unsigned char WD[9]={"0000.0000"};		//γ��
volatile unsigned char WD_a='N';				//γ�ȷ���
volatile unsigned char time[6]={"000000"};		//ʱ��
volatile unsigned char date[6]={"000000"};		//����
volatile unsigned char speed[5]={"0.0"};		//�ٶ�
volatile unsigned char high[6];					//�߶�
volatile unsigned char angle[5]={"0.0"};		//��λ��
volatile unsigned char use_sat[2]={"00"};		//ʹ�õ�������
volatile unsigned char total_sat[2]={"00"};		//�������������
volatile unsigned char av='V';					//��λ״̬

//�����ж���Ҫ�ı���
volatile unsigned char seg_count;				//���ż�����
volatile unsigned char dot_count;				//С���������
volatile unsigned char byte_count;				//λ��������
volatile unsigned char cmd_number;				//��������
volatile unsigned char mode;					//0������ģʽ��1������ģʽ��2������ģʽ
volatile unsigned char buf_full;				//1�����������ɣ���Ӧ������Ч��0������������Ч��
volatile unsigned char cmd[5];					//�������ʹ洢����


//��ʾ��Ҫ�ı���
unsigned char dsp_count;						//ˢ�´���������
unsigned char time_count;
volatile unsigned char high_num;


//USART ��ʼ��
void init_USART(void)	
{
    UCSRC = (1<<URSEL) | 0x06;    				//�첽��8λ���ݣ�����żУ�飬һ��ֹͣλ���ޱ���
    UBRRL= (F_CPU/GPS_RPS1/16-1)%256;    		//UBRR= (F_CPU/(BAUDRATE*16))-1;
    UBRRH= (F_CPU/GPS_RPS1/16-1)/256;
    UCSRA = 0x00;    							//ʹ�ܽ����жϣ�ʹ�ܽ��գ�ʹ�ܷ���
    UCSRB = (1<<RXCIE)|(1<<RXEN);
	sei();										//���ж�����	
}


//��������
unsigned char week(unsigned int y, unsigned char m, unsigned char d)
{  
    if(m<3)
    {
        m+=12;
        y--;
    }    
    unsigned char w=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7+1;
    return w==7?0:w;
}


//���ڽ����ж�
SIGNAL(SIG_UART_RECV) //���ڽ����жϷ������
{
	unsigned char tmp;
	tmp=UDR;
	switch(tmp){
		case '$':
			cmd_number=0;		//�����������
			mode=1;				//��������ģʽ
			byte_count=0;		//����λ�����
			break;
		case ',':
			seg_count++;		//���ż�����1
			byte_count=0;
			break;
		case '*':
			switch(cmd_number){
				case 1:
					buf_full|=0x01;
					break;
				case 2:
					buf_full|=0x02;
					break;
				case 3:
					buf_full|=0x04;
					break;
			}
			mode=0;
			break;
		default:
			if(mode==1){
				//���������ж�
				cmd[byte_count]=tmp;			//�����ַ��������ͻ���
				if(byte_count>=4){				//����������ݽ�����ϣ��ж�����
					if(cmd[0]=='G'){
						if(cmd[1]=='P'){
							if(cmd[2]=='G'){
								if(cmd[3]=='G'){
									if(cmd[4]=='A'){
										cmd_number=1;
										mode=2;
										seg_count=0;
										byte_count=0;
										high_num=0;
									}
								}
								else if(cmd[3]=='S'){
									if(cmd[4]=='V'){
										cmd_number=2;
										mode=2;
										seg_count=0;
										byte_count=0;
									}
								}
							}
							else if(cmd[2]=='R'){
								if(cmd[3]=='M'){
									if(cmd[4]=='C'){
										cmd_number=3;
										mode=2;
										seg_count=0;
										byte_count=0;
									}
								}
							}
						}
					}
				}
			}
			else if(mode==2){
				//�������ݴ���
				switch (cmd_number){
					case 1:				//����1���ݽ��ա�GPGGA
						switch(seg_count){
							case 2:								//γ�ȴ���
								if(byte_count<9){
									WD[byte_count]=tmp;
								}
								break;
							case 3:								//γ�ȷ�����
								if(byte_count<1){
									WD_a=tmp;
								}
								break;
							case 4:								//���ȴ���
								if(byte_count<10){
									JD[byte_count]=tmp;
								}
								break;
							case 5:								//���ȷ�����
								if(byte_count<1){
									JD_a=tmp;
								}
								break;
							case 7:								//��λʹ�õ�������
								if(byte_count<2){
									use_sat[byte_count]=tmp;
								}
								break;
							case 9:								//�߶ȴ���
								if(byte_count<6){
									high[byte_count]=tmp;
									high_num++;
								}
								break;
						}
						break;
					case 2:				//����2���ݽ��ա�GPGSV
						switch(seg_count){
							case 3:								//����е���������
								if(byte_count<2){
									total_sat[byte_count]=tmp;
								}
								break;
						}
						break;
					case 3:				//����3���ݽ��ա�GPRMC
						switch(seg_count){
							case 1:								//ʱ�䴦��
								if(byte_count<6){				//ʱ�䴦��
									time[byte_count]=tmp;	
								}
								break;
							case 2:
								if(byte_count<1){
									av=tmp;
								}
								break;
							case 7:								//�ٶȴ���
								if(byte_count<5){
									speed[byte_count]=tmp;
								}
								break;
							case 8:								//��λ�Ǵ���
								if(byte_count<5){
									angle[byte_count]=tmp;
								}
								break;
							case 9:								//���ڴ���
								if(byte_count<6){
									date[byte_count]=tmp;
								}
								break;
						}
						break;
				}
			}
			byte_count++;		//������λ��1
			break;
	}
}

/*
$GPGGA,024518.00,3153.7225,N,12111.9951,E,1,04,1.48,-00009,M,007,M,,*4C
$GPGLL,3153.7225,N,12111.9951,E,024518.00,A,A*63
$GPVTG,000.0,T,004.7,M,000.0,N,000.0,K,A*20
$GPGSA,A,2,04,08,17,20,,,,,,,,,1.48,1.48,0.03*08
$GPGSV,2,1,08,04,15,231,38,08,29,218,42,11,49,043,,19,09,082,*76
$GPGSV,2,2,08,27,14,198,29,28,71,316,,17,32,300,36,20,45,124,43*70
$GPRMC,024518.00,A,3153.7225,N,12111.9951,E,000.0,000.0,280107,04.7,W,A*12
$GPZDA,024519.45,28,01,2007,,*62
*/

/*
012345678901234567890

E121.22.2231 AG:112.1
N 23.23.2341 SP:234.1
HI:12345.5m U/T:03/12
2007/12/13 23:23:53 7

*/
