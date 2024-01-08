#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
//#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);        //定义液晶类型

//电机驱动引脚控制
int AIN1 = 6;   //PWMA
int AIN2 = 5;   //DIRA
int BIN1 = 10;  //PWMB             
int BIN2 = 9;   //DIRB

//蜂鸣器用到的音调
int melody[]={330,330,330,262,392,200,280};
int noteDurations[]={8,4,4,8,4,4,5};
//TimerOne timer1;

int checkSum=0;     
float Val = 0;      //设置变量Val，计数
float time = 0, old_time = 0;   //时间标记
float rpm = 0;          //存储转速
float Speed = 0;        //设置变量Speed 速度
boolean flagSpeed = false;  //测速标志
int toAndroid = 0;

void setup() {
  // put your setup code here, to run once:
  SerialOpen(0,9600);
  //  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  lcd.init();                      //液晶的初使化
  lcd.noBacklight();               //关闭液晶背光
  lcd.setCursor(0,0);              //从第一行第一个光标处显示
  lcd.print("CMD Data:");          //显示固定内容

//  timer1.initialize();

  //函数实现了播放一段音乐，播放完成后表示初使化已经完成 
  playMusic();

//  attachInterrupt(0,count,CHANGE);    //引脚电平发生改变时触发
}

void loop() {
      //  put your main code here, to run repeatedly: 
//  if(flagSpeed){  // 判断测速标志
//    time = millis();                      //以毫秒为单位，计算当前时间
 //   if(abs(time-old_time) >= 200){
 //     detachInterrupt(0); // 关闭外部中断0
      /* 
       40 -- 使用的是20格码盘，这里使用中断触发方式为CHANGE
       0.5 -- 时间为500毫秒
       6.5π -- 周长 轮子直径为65mm
      */
//      rpm = (Val/40)/0.2 ; 
//      Speed = rpm *6.5*3.14 ;
      /**
       toaAndroid -- 发送至上位机（手机端）的"速度"值，范围0 - 200（这里指的速度是换算的量程值）
       75 -- 仪表盘总量程 单位cm/s 
       200 -- Android端接收数据最大量程
      */
//      toAndroid = (Speed/75)*200; 

//      checkSum = 0;        //校验和 清零
//      SerialWrite(0,'$');  //HEAD
//      SerialWrite(0,'M');
//      SerialWrite(0,'>');
//     SerialWrite(0,2);   //data size
//      checkSum ^= 2;
//      SerialWrite(0,118); //code
//      checkSum ^= 118;
//      SerialWrite(0,7);   //data1 -- 电量 (0 - 8)
//      checkSum ^= 7;
//      SerialWrite(0,(int)toAndroid);  //data2 -- 速度(0 - 200)
//      checkSum ^= toAndroid;
//      SerialWrite(0,checkSum); //checkSum校验和
//      timer1.setPeriod(20);
//      Val = 0;                  //把脉冲计数值清零，以便计算下一个0.5秒的脉冲计数
//      old_time = millis();      // 记录每次0.5秒测速后的时间节点
//      attachInterrupt(0, count ,CHANGE); // 重新开放外部中断0
//    }
//  }
  serialCom();

}


