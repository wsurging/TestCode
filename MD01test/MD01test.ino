int SLP = 7;
int pwm = 10;
int dir = 9;


void setup() {
  // put your setup code here, to run once:
  pinMode(SLP, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(dir, OUTPUT);
}

void loop() {

  digitalWrite(SLP, HIGH);  // 高电平使能，低电平睡眠模式（停止）
  digitalWrite(dir, HIGH);  // CW
  analogWrite(pwm, 255);
  delay(2000);

  digitalWrite(pwm, LOW);  // 刹车
  delay(1000);

  digitalWrite(dir, HIGH);  // CW
  analogWrite(pwm, 255);
  delay(2000);

  digitalWrite(SLP, LOW);  //自由停止
  delay(1000);
}
