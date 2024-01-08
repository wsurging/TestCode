
#include "PinChangeInterrupt.h"
#include <MotorDriver_PCA9685.h>

// called this way, it uses the default address 0x40
MotorDriver_PCA9685 motorDirver = MotorDriver_PCA9685();


#define ENCODEROUTPUTA 13 // 每次旋转插入电机编码器输出脉冲(cpr)
#define ENCODEROUTPUTB 13 // 每次旋转插入电机编码器输出脉冲(cpr)
#define ENCODEROUTPUTC 13 // 每次旋转插入电机编码器输出脉冲(cpr)
#define ENCODEROUTPUTD 13 // 每次旋转插入电机编码器输出脉冲(cpr)


#define encoderA 4       //external interrupt
#define encoderB 5       //external interrupt
#define encoderC 7       //external interrupt
#define encoderD 9       //external interrupt
int rpmA = 0;
int rpmB = 0;
int rpmC = 0;
int rpmD = 0;
int intervalA = 1000;
long previousMillisA = 0;
long currentMillisA = 0;
int intervalB = 1000;
long previousMillisB = 0;
long currentMillisB = 0;
int intervalC = 1000;
long previousMillisC = 0;
long currentMillisC = 0;
int intervalD = 1000;
long previousMillisD = 0;
long currentMillisD = 0;
volatile long encoderValueA = 0;
volatile long encoderValueB = 0;
volatile long encoderValueC = 0;
volatile long encoderValueD = 0;



void setup() {
  Serial.begin(115200);
  //  Serial.println("Motor Drive test!");
  motorDirver.begin();
  pwm.setPWMFreq(1600);  // Set to whatever you like, we don't use it in this demo!
  Wire.setClock(400000);
  Wire.begin();                             // join the TWI as the master
  delay(2000);   // wait 2s

  pinMode(encoderA, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  pinMode(encoderB, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  pinMode(encoderC, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  pinMode(encoderD, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉


  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!

  //  attachInterrupt(digitalPinToInterrupt(encoderA), updateEncoderA, RISING);
  attachPCINT(digitalPinToPCINT(encoderA), updateEncoderA, RISING);
  attachPCINT(digitalPinToPCINT(encoderB), updateEncoderB, RISING);
  attachPCINT(digitalPinToPCINT(encoderC), updateEncoderC, RISING);
  attachPCINT(digitalPinToPCINT(encoderD), updateEncoderD, RISING);
  Serial.println("Start...");
}

void loop() {
  motorDirver.setMotor(4096, 4096, 4096, 4096);  // 电机M1/M2/M3/M4 全速正转
  delay(1500);
  motorDirver.stopMotor(MAll);  // 电机M1 刹车
  delay(1000);
  motorDirver.setMotor(-4096, -4096, -4096, -4096);  // 电机M1/M2/M3/M4 全速反转
  delay(1500);
  motorDirver.stopMotor(MAll);  // 电机M1/M2/M3/M4 刹车
  delay(1000);

  //每秒更新RPM值
  currentMillisA = millis();
  if (currentMillisA - previousMillisA > intervalA) {
    previousMillisA = currentMillisA;
    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
    rpmA = (float)(encoderValueA * 60 / ENCODEROUTPUTA / 18.8);
    Serial.print(encoderValueA);
    Serial.print(" pulseA / ");
    Serial.print(ENCODEROUTPUTA);
    Serial.print(" pulse per rotation x 60 seconds = ");
    Serial.print(rpmA);
    Serial.println(" RPM");  //电机转速
    encoderValueA = 0;
  }


  currentMillisB = millis();
  if (currentMillisB - previousMillisB > intervalB) {
    previousMillisB = currentMillisB;
    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
    rpmB = (float)(encoderValueB * 60 / ENCODEROUTPUTB / 18.8);
    Serial.print(encoderValueB);
    Serial.print(" pulseB / ");
    Serial.print(ENCODEROUTPUTB);
    Serial.print(" pulse per rotation x 60 seconds = ");
    Serial.print(rpmB);
    Serial.println(" RPM");  //电机转速
    encoderValueB = 0;
  }

  currentMillisC = millis();
  if (currentMillisC - previousMillisC > intervalC) {
    previousMillisC = currentMillisC;
    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
    rpmC = (float)(encoderValueC * 60 / ENCODEROUTPUTC / 18.8);
    Serial.print(encoderValueC);
    Serial.print(" pulseC / ");
    Serial.print(ENCODEROUTPUTC);
    Serial.print(" pulse per rotation x 60 seconds = ");
    Serial.print(rpmC);
    Serial.println(" RPM");  //电机转速
    encoderValueC = 0;
  }

  currentMillisD = millis();
  if (currentMillisD - previousMillisD > intervalD) {
    previousMillisD = currentMillisD;
    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
    rpmD = (float)(encoderValueD * 60 / ENCODEROUTPUTD / 18.8);
    Serial.print(encoderValueD);
    Serial.print(" pulseD / ");
    Serial.print(ENCODEROUTPUTD);
    Serial.print(" pulse per rotation x 60 seconds = ");
    Serial.print(rpmD);
    Serial.println(" RPM");  //电机转速
    Serial.println(       );  //电机转速
    encoderValueD = 0;
  }
}

void updateEncoderA()
{
  encoderValueA++;
}

void updateEncoderB()
{
  encoderValueB++;
}

void updateEncoderC()
{
  encoderValueC++;
}

void updateEncoderD()
{
  encoderValueD++;
}
