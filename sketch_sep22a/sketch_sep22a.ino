int signPin = 8;
int ledPin = 13;
 
void setup() {
  pinMode(ledPin, OUTPUT);
}
 
void loop() {
  if (digitalRead(signPin) == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}
