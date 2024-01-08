int signPin1 = 12;
int signPin2 = 14;
int ledPin = 13;
 
void setup() {
  pinMode(ledPin, OUTPUT);
}
 
void loop() {
  if (digitalRead(signPin1) == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
    if (digitalRead(signPin2) == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}
