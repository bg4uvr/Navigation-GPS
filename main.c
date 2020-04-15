/*********************************

	M8+340128��ʾGPS������

	by BG4UVR	 2007.9.17

*********************************/


//���幫�ƻ���Ӣ����ʾ 1���� 0Ӣ��
#define KM	1

//include files
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "240128.h"
#include "gps.h"

//define hardware port
#define LED			PC4
#define LED_PORT	PORTC
#define LED_DDR		DDRC

//
unsigned int longH __attribute__((section(".eeprom")));
unsigned int longL __attribute__((section(".eeprom")));


void main_init(void);
void dsp_screen(void);


int main(void)
{
	unsigned char i=0,j;
	unsigned char Bhour=0,Bday=0,Bmonth=0;
	unsigned int Byear=0;

	unsigned int temp=0,temp1=0;
	unsigned long temp2;

	unsigned int speed1=0;

	main_init();

	//�����ϵ�������������
	eeprom_busy_wait();
	if(eeprom_read_word(&longH)==0xffff)
	{
		eeprom_busy_wait();
		if(eeprom_read_word(&longL)==0xffff)
		{
			eeprom_busy_wait();
			eeprom_write_word(&longL,0);
			eeprom_busy_wait();
			eeprom_write_word(&longH,0);
		}
	}

	dsp_screen();				//��ʾ�̶��ַ�

	while(1){
		if(buf_full==0)				//��GPS�ź�ʱ
		{
			dsp_count++;
			_delay_ms(18);
			if(dsp_count>=200){
				glcd_clear_graph();			//����
				DispString(PSTR("GPS"),10,3);
				Display_Chinese(0,13,3);
				Display_Chinese(42,15,3);
				Display_Chinese(43,17,3);
				DispString(PSTR("..."),19,3);

				//���ź�ʱ������2���л�һ�δ�������
				while(buf_full==0)
				{
					LED_PORT&=~_BV(LED);
					i++;
					if(i>=2)
					{
						i=0;
					}

					switch(i)
					{
						case 0:
							temp1=GPS_RPS1;
							break;
						case 1:
							temp1=GPS_RPS2;
							break;
					}

					UBRRL= (F_CPU/temp1/16-1)%256;    		//UBRR= (F_CPU/(BAUDRATE*16))-1;
    				UBRRH= (F_CPU/temp1/16-1)/256;
					for(i=0;i<100;i++)
					{
						_delay_ms(20);
					}
				}

				glcd_clear_graph();
				dsp_screen();				//��ʾ�̶��ַ�
				dsp_count=0;
			}
		}
		else{						//��GPS�ź�ʱ
			if(buf_full&0x01){				//GGA���
				//��ʾ���ȷ���
				switch(JD_a)
				{
					case 'E':
						Display_Chinese(31,0,1);		//��
						break;
					case 'W':
						Display_Chinese(32,0,1);		//��
						break;
				}
				//��ʾ����3λ
				for(i=0;i<3;i++){
					Display_ASCII(JD[i],i+7,1);
				}
				//��ʾʣ�ྭ�ȵ�λ��
				for(i=3;i<10;i++){
					Display_ASCII(JD[i],i+9,1);
				}
				//��ʾγ�ȷ���
				switch(WD_a)
				{
					case 'N':
						Display_Chinese(34,0,2);		//��
						break;
					case 'S':
						Display_Chinese(33,0,2);		//��
						break;
				}
				//��ʾγ��
				for(i=0;i<2;i++){
					Display_ASCII(WD[i],i+8,2);
				}			
				for(i=2;i<9;i++){
					Display_ASCII(WD[i],i+10,2);
				}
				//��ʾ��λ״̬
				switch(av)
				{
					case 'A':
						Display_Chinese(35,15,4);	//��
						LED_PORT|=_BV(LED);
						break;						
					case 'V':
						Display_Chinese(0,15,4);	//δ
						LED_PORT^=_BV(LED);
						break;
				}
				Display_ASCII(use_sat[0],24,1);		//��ʾ����������
				Display_ASCII(use_sat[1],25,1);
				//�߶���ʾ
				if(high_num<6)							
				{
					for(i=0;i<(6-high_num);i++)
					{
						Display_ASCII(0x20,22+i,7);
					}
				}
				for(i=0;i<high_num;i++)
				{						
					Display_ASCII(high[i],28-high_num+i,7);
				}
				buf_full&=~0x01;
				dsp_count=0;
			}

			if(buf_full&0x02){				//GSV���
				Display_ASCII(total_sat[0],24,4);
				Display_ASCII(total_sat[1],25,4);
				buf_full&=~0x02;
				dsp_count=0;
			}

			if(buf_full&0x04)
			{
				//��ʾ����
				for(i=1;i<=3;i++)						//�ҳ�С�������ڵ�λ
				{
					if(angle[i]=='.')
					break;
				}
				j=i;
				while((++j)<=3)					//д����Ӧ�����Ŀո�
				{
					Display_ASCII(0x20,j+4,0);
				}

				for(j=0;j<(i+2);j++)						//��ʾ����
				{						
					Display_ASCII(angle[j],j+7-i,0);
				}
				//��ʾ�ٶ�
				for(i=1;i<=3;i++)						//�ҳ�С�������ڵ�λ
				{
					if(speed[i]=='.')
					break;
				}
				j=i;
				
				switch(i)								//ת���ٶ�ֵΪ������
				{
					case 3:
						speed1=(speed[0]-0x30)*1000+(speed[1]-0x30)*100+(speed[2]-0x30)*10+speed[4]-0x30;
						break;
					case 2:
						speed1=(speed[0]-0x30)*100+(speed[1]-0x30)*10+speed[3]-0x30;
						break;
					case 1:
						speed1=(speed[0]-0x30)*10+speed[2]-0x30;
						break;
				}

				#ifdef KM	//ת���ɹ��Ƶ�λ
					speed1=(unsigned long)(speed1)*1832/1000;
				#endif

				if((speed1/1000)!=0)
				{
					Display_ASCII(speed1/1000+0x30,14,0);
				}
				else
				{
					Display_ASCII(0x20,14,0);
				}
				if((speed1%1000/100)!=0)
				{
					Display_ASCII(speed1%1000/100+0x30,15,0);
				}
				else
				{
					Display_ASCII(0x20,15,0);
				}
				Display_ASCII(speed1%100/10+0x30,16,0);
				Display_ASCII('.',17,0);
				Display_ASCII(speed1%10+0x30,18,0);
					

				//���㺽��
				if(av=='A')
				{
					temp+=(speed1*10/36);	//�ٶȳ��� 0.36km/h �Ϳ�ʼ�����

					if(temp>1000)								//����Ѿ��ﵽ0.1�������
					{
						temp-=1000;									//��ȥ��Ӧ��ֵ
						eeprom_busy_wait();							//������������ĵ�16λ
						temp1=eeprom_read_word(&longL);	
						if(temp1==0xffff)								//�����1�����
						{
							eeprom_busy_wait();								//��16λ��0
							eeprom_write_word(&longL,0);
							eeprom_busy_wait();								//��16λ��1
							temp1=eeprom_read_word(&longH);
							temp1++;
							eeprom_busy_wait();
							eeprom_write_word(&longH,temp1);
						}
						else
						{
							temp1++;										//��1
							eeprom_busy_wait();
							eeprom_write_word(&longL,temp1);
						}
					}
				}
				//�����������
				eeprom_busy_wait();
				temp2=eeprom_read_word(&longH)*0x10000;
				eeprom_busy_wait();
				temp2+=eeprom_read_word(&longL);
			
				//�������ʾ
				temp2%=10000000;
				Display_ASCII(temp2/1000000+0x30,8,3);
				Display_ASCII((temp2%1000000)/100000+0x30,9,3);
				Display_ASCII((temp2%100000)/10000+0x30,10,3);
				Display_ASCII((temp2%10000)/1000+0x30,11,3);
				Display_ASCII((temp2%1000)/100+0x30,12,3);
				Display_ASCII((temp2%100)/10+0x30,13,3);
				Display_ASCII(temp2%10+0x30,15,3);

				//ʱ����ʾ
				if((Bhour!=((time[0]-0x30)*10+time[1]-0x30)+TIME_AREA)||(Bday!=((date[0]-0x30)*10+date[1]-0x30)))//����µ����ں�Сʱ���͵�ǰ�����
				{
					Bhour=((time[0]-0x30)*10+time[1]-0x30)+TIME_AREA;			//����ʱ��ת��
					Bday=(date[0]-0x30)*10+date[1]-0x30;
					Bmonth=(date[2]-0x30)*10+date[3]-0x30;
					Byear=(date[4]-0x30)*10+date[5]-0x30+2000;
					if(Bhour>=24){					//���Сʱ������24
						Bhour-=24;						//Сʱ����24
						Bday++;							//��������1
						switch(Bday){						//�ж�����
							case 29:							//��ͨ���2�·�
								if((!((Byear%400==0)||((Byear%4==0)&&(Byear%100!=0)))&&(Bmonth==2))){
									Bday=1;
									Bmonth++;
								}
								break;
							case 30:							//����������2��
								if(((Byear%400==0)||((Byear%4==0)&&(Byear%100!=0)))&&(Bmonth==2)){
									Bday=1;
									Bmonth++;
								}
								break;
							case 31:
								if((Bmonth==4)||(Bmonth==6)||(Bmonth==9)||(Bmonth==11)){
									Bday=1;
									Bmonth++;
								}
								break;
							case 32:
								Bday=1;
								Bmonth++;
								if(Bmonth>=13){
									Byear++;
									Bmonth=1;
								}
								break;
						}
					}
					Display_ASCII((Byear%100)/10+0x30,8,6);
					Display_ASCII(Byear%10+0x30,9,6);
					Display_ASCII(Bmonth/10+0x30,12,6);
					Display_ASCII(Bmonth%10+0x30,13,6);
					Display_ASCII(Bday/10+0x30,16,6);
					Display_ASCII(Bday%10+0x30,17,6);
				
					switch(week(Byear,Bmonth,Bday)){
						case 0:
							Display_Chinese(22,18,7);		//��
							break;
						case 1:
							Display_Chinese(36,18,7);		//һ
							break;
						case 2:
							Display_Chinese(37,18,7);
							break;						
						case 3:
							Display_Chinese(38,18,7);
							break;						
						case 4:
							Display_Chinese(39,18,7);
							break;						
						case 5:
							Display_Chinese(40,18,7);
							break;						
						case 6:
							Display_Chinese(41,18,7);
							break;						
					}
				}
				Display_ASCII(Bhour/10+0x30,6,7);
				Display_ASCII(Bhour%10+0x30,7,7);
				Display_ASCII(time[2],9,7);
				Display_ASCII(time[3],10,7);
				Display_ASCII(time[4],12,7);
				Display_ASCII(time[5],13,7);
				
				buf_full&=~0x04;
				dsp_count=0;
			}
		}
	}
	return(0);
}


void main_init(void)
{
	glcd_init();			//��ʼ����Ļ
	glcd_clear_graph();
	init_USART();
	LED_DDR|=_BV(LED);
	LED_PORT&=~_BV(LED);
}


//��ʾ�̶�λ���ַ�
void dsp_screen(void)
{
	/*
	��ʾ����
	000000000000011111111111122222
	001234456788901223456678900123
	086420864208642086420864208642
	------------------------------
	000000000011111111112222222222
	012345678901234567890123456789
	------------------------------
	����xxx.x ����xxx.x��|��λ���� 0
	����:  xxx��xx.xxxx��|  00��   1
	��γ:   xx��xx.xxxx��|-------- 2
	�ܺ���: xxxxxx.x ����|������� 3
	���Ƕ�λ״̬:  δ��λ|  00��   4
	---------------------|-------- 5
	����:  xxxx��xx��xx��|�����߶� 6
	ʱ��:  xx:xx:xx����xx|Xxxxxx�� 7  
	*/

	//�̶�λ���ַ�����ʾ
	#ifdef KM
		Display_Chinese(44,0,0);		//��
	#else
		Display_Chinese(3,0,0);			//��	
	#endif

	Display_Chinese(4,2,0);				//��
	
	#ifdef KM
		Display_Chinese(5,10,0);		//��
		Display_Chinese(10,12,0);		//��
	#else			
		Display_Chinese(3,10,0);		//��
		Display_Chinese(5,12,0);		//��
	#endif

	#ifdef KM
		Display_Chinese(45,19,0);			//KM/H
	#else
		Display_Chinese(6,19,0);			//��
	#endif

	Display_ASCII('|',21,0);			//|
	Display_Chinese(1,22,0);			//��
	Display_Chinese(2,24,0);			//λ
	Display_Chinese(7,26,0);			//��
	Display_Chinese(8,28,0);			//��

	Display_Chinese(9,2,1);				//��
	Display_ASCII(':',4,1);				//:
	Display_Chinese(10,10,1);			//��
	Display_Chinese(11,19,1);			//��
	Display_ASCII('|',21,1);			//|
	Display_Chinese(12,26,1);			//��

	Display_Chinese(13,2,2);			//γ
	Display_ASCII(':',4,2);				//:
	Display_Chinese(10,10,2);			//��
	Display_Chinese(11,19,2);			//��
	DispString(PSTR("|--------"),21,2);	//
	
	Display_Chinese(14,0,3);			//��

	#ifdef KM
		Display_Chinese(17,2,3);		//��
	#else
		Display_Chinese(3,2,3);			//��
	#endif

	Display_Chinese(15,4,3);			//��
	Display_ASCII(':',6,3);				//:
	Display_ASCII('.',14,3);			//.

	#ifdef KM
		Display_Chinese(46,17,3);		//��
	#else
		Display_Chinese(16,17,3);		//��
	#endif

	Display_Chinese(17,19,3);			//��
	Display_ASCII('|',21,3);			//|
	Display_Chinese(18,22,3);			//��
	Display_Chinese(19,24,3);			//��
	Display_Chinese(7,26,3);			//��
	Display_Chinese(8,28,3);			//��

	Display_Chinese(7,0,4);				//��
	Display_Chinese(8,2,4);				//��
	Display_Chinese(1,4,4);				//��
	Display_Chinese(2,6,4);				//λ
	Display_Chinese(20,8,4);			//״
	Display_Chinese(21,10,4);			//̬
	Display_ASCII(':',12,4);			//:
	Display_Chinese(1,17,4);			//��
	Display_Chinese(2,19,4);			//λ
	Display_ASCII('|',21,4);			//|
	Display_Chinese(12,26,4);			//��

	DispString(PSTR("---------------------|--------"),0,5);

	Display_Chinese(22,0,6);			//��
	Display_Chinese(23,2,6);			//��
	Display_ASCII(':',4,6);				//:
	Display_ASCII('2',6,6);				//:
	Display_ASCII('0',7,6);				//:
	Display_Chinese(24,10,6);			//��
	Display_Chinese(25,14,6);			//��
	Display_Chinese(22,18,6);			//��
	Display_ASCII('|',21,6);			//|
	Display_Chinese(16,22,6);			//��
	Display_Chinese(26,24,6);			//��
	Display_Chinese(27,26,6);			//��
	Display_Chinese(10,28,6);			//��

	Display_Chinese(28,0,7);			//ʱ
	Display_Chinese(29,2,7);			//��
	Display_ASCII(':',4,7);				//:
	Display_ASCII(':',8,7);				//:
	Display_ASCII(':',11,7);			//:
	Display_Chinese(8,14,7);			//��
	Display_Chinese(23,16,7);			//��
	Display_ASCII('|',21,7);			//|
	Display_Chinese(30,28,7);			//��
}
