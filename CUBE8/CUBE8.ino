#define SER0 5  //串行数据输入
#define SCK0 6  //输入到移位寄存器
#define RCK0 7  //上升沿是串行输入，并行输出并锁存

void setup()
{
   pinMode(SER0,OUTPUT);
   pinMode(SCK0,OUTPUT);
   pinMode(RCK0,OUTPUT);
}

const char tab_rain[]=
{//rain(s8 menu,s8 num,s8 v);
	0X00,0X84,0X01,0X00,0X00,0X01,0X02,0X44,
	0X84,0X00,0X24,0X08,0X04,0X20,0X80,0X00,
	0X00,0X20,0X00,0X00,0X20,0X00,0X40,0X00,
	0X00,0X04,0X00,0X40,0X00,0X00,0X08,0X00,
	
	0X40,0X00,0X10,0X00,0X00,0X04,0X04,0X11,
	0X00,0X41,0X00,0X00,0X80,0X00,0X40,0X00,
	0X00,0X00,0X00,0X24,0X01,0X18,0X00,0X00,
	0X22,0X10,0X02,0X00,0X40,0X00,0X02,0X00
};

void storey(char *a)//层填充函数，控制某层灯点亮方式		
{
	char i,j,num;
	for(i=0;i<8;i++)
	{
		num=a[i];		 //将数组中数输入寄存器
		for(j=0;j<8;j++) //串行数据输入
		{
			if(num&0x80)
				digitalWrite(SER0,1);	  // SER串行输入端口
			else
				digitalWrite(SER0,0);
			
			digitalWrite(SCK0,0);		  //上升沿，输入到移位寄存器
			delayMicroseconds(1);		
			digitalWrite(SCK0,1);		
			num<<=1;
		}
  
	}
}
void frame(char *a,char v)//一帧,a是一帧编码起始地址
                      //一个画面，v表示一帧画面扫描的次数
					  // 可以看作这帧显示的时间
{
	char i,j,num;  // 有符号定义
	while(v--)
	{	
		num=0x01;
		for(i=0;i<8;i++) //层数层控制，选通某一层，
                         //使得第1层到第8层，依次点亮
		{	
			num<<=i;
			digitalWrite(RCK0,0);
			for(j=0;j<8;j++) //串行数据输入
			{
				digitalWrite(SER0,0);
				delayMicroseconds(1);
				digitalWrite(SCK0,0);	  //上升沿，输入到移位寄存器
				delayMicroseconds(1);	
				digitalWrite(SCK0,1);		
			}
			for(j=0;j<8;j++) //串行数据输入
			{
				if(num&0x80)
					digitalWrite(SER0,1);  // SER串行输入端口
				else
					digitalWrite(SER0,0);
				
				digitalWrite(SCK0,0);	  //上升沿，输入到移位寄存器
				delayMicroseconds(1);		
				digitalWrite(SCK0,1);	
				num<<=1;
			}
			storey(a+i*8);//层填充函数，控制某层灯点亮方式
			digitalWrite(RCK0,1);
			num=0x01;			
			delay(1);   // 层显示时间
		}
	
	}	
}
void dot(char v)	//依次点亮00-1,01-1,02-1,03-1....63-1
{
	char b[64]={0};
	char x,z;
	frame(b,v);
	for(z=0;z<8;z++)
	{
		for(x=0;x<8;x++)
		{
		    b[z*8+x]=0x01;
		    frame(b,v);	
		}
	}	
	for(x=0;x<64;x++)
		b[x]=0;	
}
void move1(char *a,char kind,char length)// kind=0 x轴,1 y轴,2 z轴；
                              //direction=0,负向,1正向;length移动位数，不能为8
{
	char i,x,z;
	if(kind==0)//x轴
	{

			for(z=0;z<8;z++)
			{
				for(x=7;x>=length;x--)
					a[z*8+x]=a[z*8+(x-length)];
				for(x=0;x<length;x++)
					a[z*8+x]=0;			
			}				
	}
	else if(kind==1)//y轴
	{

			for(i=0;i<64;i++)
				a[i]<<=length;
	}
	else		   //z轴
	{
		for(x=0;x<8;x++)
			{
				for(z=length;z<8;z++)
					a[(z-length)*8+x]=a[z*8+x];
				for(z=(8-length);z<8;z++)
					a[z*8+x]=0;			
			}			
	}
}
void rain(char menu,char num,char v)
          //menu=0,down;menu=1,up；num循环次数
{
	char b[64]={0};
	char x,z;
	if(menu==1)	//up
	{
		for(x=0;x<8;x++)
			b[56+x]=tab_rain[x];
		frame(b,v);
		for(z=1;z<8;z++)
		{
			move1(b,2,1);
			for(x=0;x<8;x++)
				b[56+x]=tab_rain[z*8+x];
			frame(b,v);
		}									 
		while(num--)
		{	
			for(z=0;z<8;z++)
			{
				move1(b,2,1);
				for(x=0;x<8;x++)
					b[56+x]=tab_rain[z*8+x];
				frame(b,v);
			}
		}
	}
	else//down
	{
		for(x=0;x<8;x++)
			b[x]=tab_rain[x];
		frame(b,v);
		for(z=1;z<8;z++)
		{
			move1(b,2,1);
			for(x=0;x<8;x++)
				b[x]=tab_rain[z*8+x];
			frame(b,v);
		}
		while(num--)
		{	
			for(z=0;z<8;z++)
			{
				move1(b,2,1);
				for(x=0;x<8;x++)
					b[x]=tab_rain[z*8+x];
				frame(b,v);
			}				
		}
	}
}
void loop()
{

char a[65]={

  
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF}; 

	frame(a,5);  
  dot(80);
  rain(1,20,8);
}
