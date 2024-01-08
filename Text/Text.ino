
int AIN1 = 6;  //PWMA
int AIN2 = 5;  //DIRA
int BIN1 = 10;  //PWMB             
int BIN2 = 9;  //DIRB
int melody[]={330,330,330,262,392,200,280};
int noteDurations[]={8,4,4,8,4,4,2};
void setup(){
  delay(500);
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  delay(2000);
  digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration
  for(int thisNote = 0;thisNote < 7;thisNote ++){
    tone(12,melody[thisNote],1000/noteDurations[thisNote]);
    delay(1300/noteDurations[thisNote]);
    noTone(12);
  }
  Serial.begin(9600);
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
    analogWrite(BIN1,256-MOTORB);
  }
  else
  {
    digitalWrite(BIN1,HIGH);
    analogWrite(BIN2,254+MOTORB); 
  }
}
void loop()
{
 setMotor(200,200);
 delay(1000);
 setMotor(-200,-200);
 delay(1000);
}
