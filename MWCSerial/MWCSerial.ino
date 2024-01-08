#include<avr/io.h>
#include<Arduino.h>
#include<avr/pgmspace.h>
#include<stdint.h>
#include"def.h"
int ledOn=0;
static int16_t rcData[18];
void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  SerialOpen(SERIAL_NUM,SERIAL0_COM_SPEED);
}
void loop() {
  serialCom();
 /* if(rcData[0]>0)
  {  
    digitalWrite(13,HIGH);
      SerialWrite(0,rcData[0]);
  SerialWrite(0,'\n');
  }*/
}
