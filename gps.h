
#define TIME_AREA 8							//ʱ��
#define GPS_RPS1		4800				//GPS����
#define GPS_RPS2		9600				//GPS����

//GPS���ݴ洢����
extern volatile unsigned char JD[10];		//����
extern volatile unsigned char JD_a;			//���ȷ���
extern volatile unsigned char WD[9];		//γ��
extern volatile unsigned char WD_a;			//γ�ȷ���
extern volatile unsigned char time[6];		//ʱ��
extern volatile unsigned char date[6];		//����
extern volatile unsigned char speed[5];		//�ٶ�
extern volatile unsigned char high[6];		//�߶�
extern volatile unsigned char angle[5];		//��λ��
extern volatile unsigned char use_sat[2];	//ʹ�õ�������
extern volatile unsigned char total_sat[2];	//�������������
extern volatile unsigned char av;			//��λ״̬

//�����ж���Ҫ�ı���
extern volatile unsigned char seg_count;	//���ż�����
extern volatile unsigned char dot_count;	//С���������
extern volatile unsigned char byte_count;	//λ��������
extern volatile unsigned char cmd_number;	//��������
extern volatile unsigned char mode;			//0������ģʽ��1������ģʽ��2������ģʽ
extern volatile unsigned char buf_full;		//1�����������ɣ���Ӧ������Ч��0������������Ч��
extern volatile unsigned char cmd[5];		//�������ʹ洢����

//��ʾ��Ҫ�ı���
extern unsigned char dsp_count;				//ˢ�´���������
extern unsigned char time_count;
extern volatile unsigned char high_num;


void init_USART(void);
unsigned char week(unsigned int y, unsigned char m, unsigned char d);
