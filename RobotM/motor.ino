int AIN1 = 5;  //PWMA
int AIN2 = 6;  //DIRA
int BIN1 = 9;  //PWMB             
int BIN2 = 10;  //DIRB

void motorPinMode(){
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
}
void setMotor(int MOTORA,int MOTORB)
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
  if(MOTORB>0)
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
