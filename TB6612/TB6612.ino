/////////PWM输出引脚////////
#define EN 2
#define IN1 4
#define PWM 5

void setup() {
  // put your setup code here, to run once:
  pinMode(EN, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);
  digitalWrite(EN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(EN, HIGH);
  digitalWrite(IN1, LOW);
  analogWrite(PWM, 60);
  delay(2000);

  digitalWrite(IN1, LOW);
  digitalWrite(PWM, LOW);
  delay(500);

  digitalWrite(IN1, HIGH);
  analogWrite(PWM, 60);
  delay(2000);

  digitalWrite(IN1, LOW);
  digitalWrite(PWM, LOW);
  delay(500);
}
