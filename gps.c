
//包含头文件
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "gps.h"

//GPS数据存储数组
volatile unsigned char JD[10]={"00000.0000"};	//经度
volatile unsigned char JD_a='E';				//经度方向
volatile unsigned char WD[9]={"0000.0000"};		//纬度
volatile unsigned char WD_a='N';				//纬度方向
volatile unsigned char time[6]={"000000"};		//时间
volatile unsigned char date[6]={"000000"};		//日期
volatile unsigned char speed[5]={"0.0"};		//速度
volatile unsigned char high[6];					//高度
volatile unsigned char angle[5]={"0.0"};		//方位角
volatile unsigned char use_sat[2]={"00"};		//使用的卫星数
volatile unsigned char total_sat[2]={"00"};		//天空中总卫星数
volatile unsigned char av='V';					//定位状态

//串口中断需要的变量
volatile unsigned char seg_count;				//逗号计数器
volatile unsigned char dot_count;				//小数点计数器
volatile unsigned char byte_count;				//位数计数器
volatile unsigned char cmd_number;				//命令类型
volatile unsigned char mode;					//0：结束模式，1：命令模式，2：数据模式
volatile unsigned char buf_full;				//1：整句接收完成，相应数据有效。0：缓存数据无效。
volatile unsigned char cmd[5];					//命令类型存储数组


//显示需要的变量
unsigned char dsp_count;						//刷新次数计数器
unsigned char time_count;
volatile unsigned char high_num;


//USART 初始化
void init_USART(void)	
{
    UCSRC = (1<<URSEL) | 0x06;    				//异步，8位数据，无奇偶校验，一个停止位，无倍速
    UBRRL= (F_CPU/GPS_RPS1/16-1)%256;    		//UBRR= (F_CPU/(BAUDRATE*16))-1;
    UBRRH= (F_CPU/GPS_RPS1/16-1)/256;
    UCSRA = 0x00;    							//使能接收中断，使能接收，使能发送
    UCSRB = (1<<RXCIE)|(1<<RXEN);
	sei();										//总中断允许	
}


//计算星期
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


//串口接收中断
SIGNAL(SIG_UART_RECV) //串口接收中断服务程序
{
	unsigned char tmp;
	tmp=UDR;
	switch(tmp){
		case '$':
			cmd_number=0;		//命令类型清空
			mode=1;				//接收命令模式
			byte_count=0;		//接收位数清空
			break;
		case ',':
			seg_count++;		//逗号计数加1
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
				//命令种类判断
				cmd[byte_count]=tmp;			//接收字符放入类型缓存
				if(byte_count>=4){				//如果类型数据接收完毕，判断类型
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
				//接收数据处理
				switch (cmd_number){
					case 1:				//类型1数据接收。GPGGA
						switch(seg_count){
							case 2:								//纬度处理
								if(byte_count<9){
									WD[byte_count]=tmp;
								}
								break;
							case 3:								//纬度方向处理
								if(byte_count<1){
									WD_a=tmp;
								}
								break;
							case 4:								//经度处理
								if(byte_count<10){
									JD[byte_count]=tmp;
								}
								break;
							case 5:								//经度方向处理
								if(byte_count<1){
									JD_a=tmp;
								}
								break;
							case 7:								//定位使用的卫星数
								if(byte_count<2){
									use_sat[byte_count]=tmp;
								}
								break;
							case 9:								//高度处理
								if(byte_count<6){
									high[byte_count]=tmp;
									high_num++;
								}
								break;
						}
						break;
					case 2:				//类型2数据接收。GPGSV
						switch(seg_count){
							case 3:								//天空中的卫星总数
								if(byte_count<2){
									total_sat[byte_count]=tmp;
								}
								break;
						}
						break;
					case 3:				//类型3数据接收。GPRMC
						switch(seg_count){
							case 1:								//时间处理
								if(byte_count<6){				//时间处理
									time[byte_count]=tmp;	
								}
								break;
							case 2:
								if(byte_count<1){
									av=tmp;
								}
								break;
							case 7:								//速度处理
								if(byte_count<5){
									speed[byte_count]=tmp;
								}
								break;
							case 8:								//方位角处理
								if(byte_count<5){
									angle[byte_count]=tmp;
								}
								break;
							case 9:								//日期处理
								if(byte_count<6){
									date[byte_count]=tmp;
								}
								break;
						}
						break;
				}
			}
			byte_count++;		//接收数位加1
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
