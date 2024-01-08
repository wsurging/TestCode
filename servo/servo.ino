#include<ServoTimer2.h>
ServoTimer2 servoLev;
ServoTimer2 servoVcl;
#define MINPULSE 500
#define MAXPULSE 2500
#define MIDPULSE 1500
void setup() {
  // put your setup code here, to run once:
  servoLev.attach(2);
  servoVcl.attach(3);
  servoLev.write(MIDPULSE);
  servoVcl.write(MIDPULSE);
  delay(200);
}

void loop() {
  // put your main code here, to run repeatedly: 
  for(int i=MIDPULSE;i>=MINPULSE;i--)
  {
    servoLev.write(i);
    servoVcl.write(i);
    delayMicroseconds(500);
  }
  
  for(int i=MINPULSE;i<=MAXPULSE;i++)
  {
    servoLev.write(i);
    servoVcl.write(i);
    delayMicroseconds(500);
  }
  for(int i=MAXPULSE;i>=MIDPULSE;i--)
  {
    servoLev.write(i);
    servoVcl.write(i);
    delayMicroseconds(500);
  }  
}
