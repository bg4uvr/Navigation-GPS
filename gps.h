//����ͷ�ļ�
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define TIME_AREA 8				//ʱ��
#define GPS_RPS1		4800		//GPS����
#define GPS_RPS2		9600		//GPS����

//GPS���ݴ洢����
volatile unsigned char JD[10]={"00000.0000"};	//����
volatile unsigned char JD_a='E';		//���ȷ���
volatile unsigned char WD[9]={"0000.0000"};	//γ��
volatile unsigned char WD_a='N';		//γ�ȷ���
volatile unsigned char time[6]={"000000"};	//ʱ��
volatile unsigned char date[6]={"000000"};	//����
volatile unsigned char speed[5]={"0.0"};		//�ٶ�
volatile unsigned char high[6];		//�߶�
volatile unsigned char angle[5]={"0.0"};		//��λ��
volatile unsigned char use_sat[2]={"00"};	//ʹ�õ�������
volatile unsigned char total_sat[2]={"00"};	//�������������
volatile unsigned char av='V';					//��λ״̬

//�����ж���Ҫ�ı���
volatile unsigned char seg_count;		//���ż�����
volatile unsigned char dot_count;		//С���������
volatile unsigned char byte_count;		//λ��������
volatile unsigned char cmd_number;		//��������
volatile unsigned char mode;			//0������ģʽ��1������ģʽ��2������ģʽ
volatile unsigned char buf_full;		//1�����������ɣ���Ӧ������Ч��0������������Ч��
volatile unsigned char cmd[5];			//�������ʹ洢����

//��ʾ��Ҫ�ı���
unsigned char dsp_count;			//ˢ�´���������
unsigned char time_count;

volatile unsigned char high_num;

extern void init_USART(void);
