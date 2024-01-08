/*#include<ServoTimer2.h>
//#include <Servo.h>
ServoTimer2 myServo;
#define MINPULSE 544
#define MAXPULSE 2400
#define MIDPULSE 1500
void initServo()
{  
   myServo.attach(4);
   myServo.write(MIDPULSE);
}
void turnServo(int dir)
{
  int i;
  switch (dir)
  { 
     case 'L': 
       for(i=MIDPULSE;i<MAXPULSE;i++)
       {
          myServo.write(i);
          delayMicroseconds(300);
       }
     break;
     case 'R':
       for(i=MAXPULSE;i>MINPULSE;i--)
       {
          myServo.write(i);
          delayMicroseconds(300);
       }
     break;
     case 'M':
       for(i=MINPULSE;i<MIDPULSE;i++)
       {
          myServo.write(i);
          delayMicroseconds(300);
       }
     break;  
  }
}
void distanceLR(int leftDistance,int rightDistance)
{   
   turnServo('L');
   leftDistance = countDistance();
//   Serial.println(leftDistance);
   turnServo('R');
   rightDistance = countDistance();
//   Serial.println(rightDistance);
}*/
