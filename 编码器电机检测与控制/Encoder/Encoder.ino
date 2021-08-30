#define ENCODEROUTPUT 24 // 每次旋转插入电机编码器输出脉冲

#define ENCODER_A 2 //external interrupt
#define PWM 6       // PWM pin 6
#define DIR 4       // DIR pin 7

int Val = 255;       //电机占空比 0-255;
int rpm = 0;
int interval = 1000;
long previousMillis = 0;
long currentMillis = 0;
volatile long encoderValue = 0;

void setup()
{
  Serial.begin(9600); // 波特率
  pinMode(ENCODER_A, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
  // 在每个上升信号上在霍尔传感器A处附加中断
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoder, FALLING);
}

void loop()
{
  digitalWrite(DIR, HIGH);
  analogWrite(PWM, Val);
  //每秒更新RPM值
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
    rpm = (float)(encoderValue  / ENCODEROUTPUT);
    Serial.print(encoderValue);
    Serial.print("cpr");  //电机脉冲数
    Serial.print("\t");  
    Serial.print(rpm);
    Serial.println("RPM/min");  //电机每分钟转速
    encoderValue = 0;
  }
}

void updateEncoder()
{
  encoderValue++;
}
