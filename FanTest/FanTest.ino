#define fanPin 5
int i = 0;
void setup() {
  pinMode(fanPin, OUTPUT);
}

void loop()
{
  for (i = 0; i <= 255; i += 1) {
    analogWrite(fanPin, i);
    delay(15);
  }
    for (i = 255; i >= 0; i -= 1) {
    analogWrite(fanPin, i);
    delay(15);
  }
}
