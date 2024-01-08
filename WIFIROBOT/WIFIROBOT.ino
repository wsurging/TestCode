//电机驱动引脚控制
int AIN1 = 6;   //PWMA
int AIN2 = 5;   //DIRA
int BIN1 = 10;  //PWMB             
int BIN2 = 9;   //DIRB
#include<ServoTimer2.h>
ServoTimer2 myServo;
ServoTimer2 myServo1;
#define MINPULSE 544
#define MAXPULSE 2400
#define MIDPULSE 1500
void initServo()
{  
   myServo.attach(11);
   myServo1.attach(3);
   myServo.write(MIDPULSE);
   delay(1000);
}
int  cmdData[4];
void setup() {
  pinMode(13,OUTPUT);
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  Serial.begin(9600);
  initServo();
}
int tmpData;
int UARTDataCount = 0;
int n = 0;
void loop() {
  // put your main code here, to run repeatedly: 
  if(Serial.available())
  {
    tmpData=Serial.read();
    if(tmpData == 0xFF && UARTDataCount <2)
    {
      cmdData[0] = tmpData;
      UARTDataCount++; 
      n = 1;
    }
    else
    {
      cmdData[n]=tmpData;
      n++;
    }
    if(UARTDataCount == 2)
    {
      cmdData[0] = 0xFF;
      cmdData[4] = 0xFF;
      n = 1;
      UARTDataCount = 0;
      Serial.flush();
    }
  }
  if(cmdData[0]==0xFF && cmdData[4]==0xFF)
  {
    switch(cmdData[1])
    {
      case 0x00:
        switch(cmdData[2])
        {
          case 0x01:
            setMotor(100,100);
            break;
          case 0x00:
            setMotor(0,0);
            break;
          case 0x02:
            setMotor(-100,-100);
            break;
          case 0x03:
            setMotor(-100,100);
            break;
          case 0x04:
            setMotor(100,-100);
            break;
        }
      break;
      case 0x01:
        switch(cmdData[2])
        {
          case 0x01:
            myServo1.write(cmdData[3]*13+500);break;
          case 0x04:
            myServo.write(cmdData[3]*13+500);break;
        }
      break;
    }
  }
}


void setMotor(int MOTORA,int MOTORB) //电机驱动函数
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

