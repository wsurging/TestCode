const int TrigPin = A1;
const int EchoPin = A0;
int cm;
 
void setup()
{
  Serial.begin(9600);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}
void loop()
{
  digitalWrite(TrigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
 
  cm = pulseIn(EchoPin, HIGH) / 58.0;   //echo time conversion into a distance
  cm = (int(cm * 100.0)) / 100.0;       //keep two decimal places
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(200);
}
