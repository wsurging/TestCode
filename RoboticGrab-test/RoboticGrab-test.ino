/*********************************************************************
  机械臂抓取

  @Author: YFROBOT-WST
  @Version: V1.0
  @Date: 11/30/2022
  @URL: www.yfrobot.com.cn
**********************************************************************/
#include <MotorDriver.h>

//初始角度
int S1MID = 110;  // 底部旋转初始角度
int S2MID = 90;   // 底部抬臂初始角度
int S3MID = 90;   // 中间抬臂初始角度
int S4MID = 90;   // 头部抬臂初始角度
int S5MID = 90;   // 手爪张开角度初始角度

#define MOTORTYPE YF_IIC_RZ  // rz7889
uint8_t SerialDebug = 0;     // 串口打印调试 0-否 1-是

const int offsetm1 = 1;
const int offsetm2 = -1;
const int offsetm3 = -1;
const int offsetm4 = 1;

MotorDriver motorDriver = MotorDriver(MOTORTYPE);

void setup() {
  Serial.begin(115200);
  motorDriver.begin();
  motorDriver.motorConfig(offsetm1, offsetm2, offsetm3, offsetm4);
  motorDriver.setPWMFreq(50);  // 控制舵机时，需要设置PWM频率 ~50
  motorDriver.servoWrite(S1, S1MID);
  motorDriver.servoWrite(S2, S2MID);
  motorDriver.servoWrite(S3, S3MID);
  motorDriver.servoWrite(S4, S4MID);
  motorDriver.servoWrite(S5, S5MID);
  delay(1000);
}

void loop() {
  // motorDriver.setAllMotor(4096);  // 前进
  // delay(1000);
  // motorDriver.setAllMotor(-4096);  // 电机M1/M2/M3/M4 后退
  // delay(1000);
  Step1();
  delay(1000);
  Step2();
  delay(1000);
  Step3();
  delay(1000);
  Step4();
  delay(1000);
  Step5();
  delay(1000);
  Step6();
  delay(1000);
  Step7();
  delay(1000);
  Step8();
  delay(1000);
  Step9();
  delay(2000);
}

void Step1() {
  motorDriver.servoWrite(S1, S1MID);
  motorDriver.servoWrite(S2, S2MID);
  motorDriver.servoWrite(S3, S3MID);
  motorDriver.servoWrite(S4, S4MID);
  motorDriver.servoWrite(S5, S5MID);
}

void Step2() {
  motorDriver.servoWrite(S4, 120);
  delay(300);
  motorDriver.servoWrite(S3, 15);
  delay(300);
  motorDriver.servoWrite(S2, 10);
}

void Step3() {
  motorDriver.servoWrite(S5, 130);
}

void Step4() {
  motorDriver.servoWrite(S2, S2MID);
  delay(80);
  motorDriver.servoWrite(S3, S3MID);
  delay(80);
  motorDriver.servoWrite(S4, S4MID);
  delay(80);
}

void Step5() {
  motorDriver.servoWrite(S1, 110);  //最大180度
}

void Step6() {
  motorDriver.servoWrite(S4, 30);
  delay(300);
  motorDriver.servoWrite(S3, 95);
  delay(300);
  motorDriver.servoWrite(S2, 90);
}

void Step7() {
  motorDriver.servoWrite(S5, S5MID);
}

void Step8() {
  motorDriver.servoWrite(S2, S2MID);
  delay(80);
  motorDriver.servoWrite(S3, S3MID);
  delay(80);
  motorDriver.servoWrite(S4, S4MID);
  delay(80);
}

void Step9() {
  motorDriver.servoWrite(S1, S1MID);
}