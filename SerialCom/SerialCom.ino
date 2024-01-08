#include<stdint.h>
#include<Arduino.h>
#define UART_NUMBER 1
#define CURRENTPORT 0
uint8_t cmdData[CURRENTPORT];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,OUTPUT);
}

void serialCom()
{
  uint8_t c,n;
  static enum _serial_state 
  {
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_CMD,
  } c_state[UART_NUMBER];// = IDLE;
  for(n=0;n<UART_NUMBER;n++) 
  {
    while(Serial.available()>0)
    {
  //    digitalWrite(13,HIGH);
      c = Serial.read();
      if (c_state[CURRENTPORT] == IDLE) 
      {
        c_state[CURRENTPORT] = (c==0xFF) ? HEADER_START : IDLE;
//        if (c_state[CURRENTPORT] == IDLE)        
//            evaluateOtherData(c); // evaluate all other incoming serial data
      } 
      else if (c_state[CURRENTPORT] == HEADER_START) 
      {
        c_state[CURRENTPORT] = (c==0x00) ? HEADER_M : IDLE;
      } 
      else if (c_state[CURRENTPORT] == HEADER_M)
      {
        cmdData[CURRENTPORT] = c;
        if(cmdData[CURRENTPORT]==0x01)  
          digitalWrite(13,HIGH);
          break;
        if(cmdData[CURRENTPORT]==0x02)
          digitalWrite(13,LOW);     
          break;
      } 
      else if (c_state[CURRENTPORT] == HEADER_ARROW) 
      {
        c_state[CURRENTPORT] = (c==0xFF) ? HEADER_CMD: IDLE;
      }
    }
  }
}
void loop() {
  // put your main code here, to run repeatedly: 
  serialCom();
}
