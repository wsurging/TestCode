void setup()
{
 Serial.begin(115200);
}
 
void loop()
{
  while(Serial.available())
   {
     char c=Serial.read();
      if(c=='A')
        {
          Serial.println("Open");
          digitalWrite(13,HIGH);
        }
        if(c=='S')
        {
          Serial.println("close");
          digitalWrite(13,LOW);
        }
   }
}
