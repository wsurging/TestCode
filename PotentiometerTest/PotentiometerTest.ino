/*
 Potentiometer Test
*/
int pPin = A0;
int pVal = 0;
int pVal_old = 0;
void setup()
{
  pinMode(pPin,INPUT);      
  Serial.begin(9600);  
}
void loop()
{
  //读取A0模拟口的数值（0-5V 对应 0-1204取值）
  pVal = analogRead(pPin);   
  if(abs(pVal - pVal_old) >= 2 ){
    Serial.println(pVal); 
    pVal_old = pVal;
  }
}
