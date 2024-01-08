#include <IRremote.h>
int RECV_PIN = 11;   //红外线接收器OUTPUT端接在pin 11
IRrecv irrecv(RECV_PIN);   // 定义IRrecv 对象来接收红外线信号
decode_results  results;   //解码结果放在decode_results构造的对象results里
//定义电机控制接口
int AIN1 = 6;  //PWMA
int AIN2 = 5;  //DIRA
int BIN1 = 10;  //PWMB             
int BIN2 = 9;  //DIRB
void setup()       //初使化程序
{
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  irrecv.enableIRIn(); //    启动红外解码
}
void setMotor(int MOTORA,int MOTORB)    //电机控制函数
{
  if(MOTORA>=0)
  {
    digitalWrite(AIN2,HIGH);
    analogWrite(AIN1,255-MOTORA);
  }
  else
  {
   digitalWrite(AIN1,HIGH);
    analogWrite(AIN2,MOTORA+255);
  }
  if(MOTORB>=0)
  {
    digitalWrite(BIN2,HIGH);
    analogWrite(BIN1,255-MOTORB);
  }
  else
  {
    digitalWrite(BIN1,HIGH);
    analogWrite(BIN2,255+MOTORB); 
  }
}
void loop() {
   if (irrecv.decode(&results))  {        // 解码成功，收到一组红外线信号
                                    // 红外线码元位数  
    irrecv.resume();
    switch(results.value){
      case 0xFF02FD:          // 收到“向前”命令的控制符
        setMotor(100,100);      // 向前转动
        break;
      case 0xFF9867:           // 收到“向后”命令的控制符
        setMotor(-100,-100);     // 向后转动
        break;
      case 0xFFE01F:           // 收到“向左”命令的控制符
        setMotor(-100,100);      // 向左转动
        break;
      case 0xFF906F:           // 收到“向右”命令的控制符
        setMotor(100,-100);     // 向右转动
        break;
      case 0xFFA857:           //收到“停止”命令的控制符
        setMotor(0,0);           //停止转动
        break;
    }
  }   
}
