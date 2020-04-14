//包含头文件
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define TIME_AREA 8				//时区
#define GPS_RPS1		4800		//GPS速率
#define GPS_RPS2		9600		//GPS速率

//GPS数据存储数组
volatile unsigned char JD[10]={"00000.0000"};	//经度
volatile unsigned char JD_a='E';		//经度方向
volatile unsigned char WD[9]={"0000.0000"};	//纬度
volatile unsigned char WD_a='N';		//纬度方向
volatile unsigned char time[6]={"000000"};	//时间
volatile unsigned char date[6]={"000000"};	//日期
volatile unsigned char speed[5]={"0.0"};		//速度
volatile unsigned char high[6];		//高度
volatile unsigned char angle[5]={"0.0"};		//方位角
volatile unsigned char use_sat[2]={"00"};	//使用的卫星数
volatile unsigned char total_sat[2]={"00"};	//天空中总卫星数
volatile unsigned char av='V';					//定位状态

//串口中断需要的变量
volatile unsigned char seg_count;		//逗号计数器
volatile unsigned char dot_count;		//小数点计数器
volatile unsigned char byte_count;		//位数计数器
volatile unsigned char cmd_number;		//命令类型
volatile unsigned char mode;			//0：结束模式，1：命令模式，2：数据模式
volatile unsigned char buf_full;		//1：整句接收完成，相应数据有效。0：缓存数据无效。
volatile unsigned char cmd[5];			//命令类型存储数组

//显示需要的变量
unsigned char dsp_count;			//刷新次数计数器
unsigned char time_count;

volatile unsigned char high_num;

extern void init_USART(void);
