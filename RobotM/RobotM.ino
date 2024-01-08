#include<avr/io.h>
#include<avr/pgmspace.h>
byte ch;                    //定义变量ch
void setup() {
  motorPinMode();
//  initServo();
  Serial.begin(9600);
}
void loop()
{
//   turnMotor();
//  judgeDistance();
//  setMotor(200,200);
  if ( Serial.available() )       //判断缓冲器是否接收到数据
  {
    ch = Serial.read();        //有数据，将接收到的数据赋值ch
    if ( ch == 'f' )           //收到字符‘a’
    {
       setMotor(250,250);
    }
    else if(ch == 'b')          //收到字符‘b’
    {
      setMotor(-250,-250);
    }
    else if(ch == 'l')
    {
      setMotor(-200,200);
    }
     else if(ch == 'r')
    {
      setMotor(200,-200);
    }
    else if(ch == 's')
    {
      setMotor(0,0);
    }
  }
}
