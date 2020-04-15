
#define TIME_AREA 8							//时区
#define GPS_RPS1		4800				//GPS速率
#define GPS_RPS2		9600				//GPS速率

//GPS数据存储数组
extern volatile unsigned char JD[10];		//经度
extern volatile unsigned char JD_a;			//经度方向
extern volatile unsigned char WD[9];		//纬度
extern volatile unsigned char WD_a;			//纬度方向
extern volatile unsigned char time[6];		//时间
extern volatile unsigned char date[6];		//日期
extern volatile unsigned char speed[5];		//速度
extern volatile unsigned char high[6];		//高度
extern volatile unsigned char angle[5];		//方位角
extern volatile unsigned char use_sat[2];	//使用的卫星数
extern volatile unsigned char total_sat[2];	//天空中总卫星数
extern volatile unsigned char av;			//定位状态

//串口中断需要的变量
extern volatile unsigned char seg_count;	//逗号计数器
extern volatile unsigned char dot_count;	//小数点计数器
extern volatile unsigned char byte_count;	//位数计数器
extern volatile unsigned char cmd_number;	//命令类型
extern volatile unsigned char mode;			//0：结束模式，1：命令模式，2：数据模式
extern volatile unsigned char buf_full;		//1：整句接收完成，相应数据有效。0：缓存数据无效。
extern volatile unsigned char cmd[5];		//命令类型存储数组

//显示需要的变量
extern unsigned char dsp_count;				//刷新次数计数器
extern unsigned char time_count;
extern volatile unsigned char high_num;


void init_USART(void);
unsigned char week(unsigned int y, unsigned char m, unsigned char d);
