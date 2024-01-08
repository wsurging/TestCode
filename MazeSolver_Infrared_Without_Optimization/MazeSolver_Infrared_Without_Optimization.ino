/*********************************************************************
  迷宫解谜小车

  使用4红外传感器，分别安装在小车前，左，右，上方（检测终点），使用左侧优先原则
  检测到终点停止，无解谜最优解。

  @Author: YFROBOT-WST
  @Version: V1.0
  @Date: 04/23/2023
  @URL: www.yfrobot.com.cn
**********************************************************************/
#define Fsensor 3    //前端红外
#define Lsensor 9    //左侧红外
#define Rsensor 11   //右侧红外
#define UPsensor A1  //上方红外

#define MADIR 4  //电机MA方向引脚  
#define MAPWM 5  //电机MA速度引脚
#define MBPWM 6  //电机MB速度引脚
#define MBDIR 7  //电机MB方向引脚

#define MASPEED 205    // 电机A速度0-255
#define MBSPEED 200    // 电机B速度0-255
#define turnSpeed 240  // 调整电机转向时速度 (0-255)240
#define slowSpeed 150  // 向前运行一段距离时速度

#define LturnTime 350  // 左转时间延迟350
#define RturnTime 350  // 右转时间延迟380
#define BturnTime 700  // 调头时间延迟700
#define upTime 560     // 检测到向前移动时间

int F, L, R, UP;

void setup() {
  Serial.begin(9600);
  pinMode(MAPWM, OUTPUT);
  pinMode(MBPWM, OUTPUT);
  pinMode(MADIR, OUTPUT);
  pinMode(MBDIR, OUTPUT);
  pinMode(Fsensor, INPUT);
  pinMode(Lsensor, INPUT);
  pinMode(Rsensor, INPUT);
  pinMode(UPsensor, INPUT);

  delay(3000);
  readSensor();
  setMotor(MASPEED, MBSPEED);
  delay(1000);
  setMotor(0, 0);
  delay(1000);
}

void MazeSolve() {
  //循环直到我们解决了迷宫。
  while (1) {
    // 现在读取传感器并检查交叉点类型。
    readSensor();
    // 检查左右出口
    if (L == HIGH) {
      setMotor(slowSpeed, slowSpeed);  // 再直行一点，让我们的车轮与十字路口对齐,根据迷宫大小调整。
      delay(upTime);
      Left();
      // Serial.println("  Left");
    } else if (L == LOW && F == HIGH) {
      Forward();
      // Serial.println("  Forward");
    } else if (R == HIGH && L == LOW && F == LOW) {
      setMotor(slowSpeed, slowSpeed);  // 再直行一点，让我们的车轮与十字路口对齐,根据迷宫大小调整。
      delay(upTime);
      Right();
      // Serial.println("  Right");
    } else if (F == LOW && L == LOW && R == LOW && UP == HIGH) {
      Back();
      // Serial.println("  Back");
    } else if (F == LOW && L == LOW && R == LOW && UP == LOW) {  //所有传感器都检测到停止，迷宫解决。
      setMotor(0, 0);
      // Serial.println("  Stop");
      return;
    }
  }
}

void loop() {
  readSensor();
  MazeSolve();  // 开始解迷宫
}

void readSensor() {
  F = digitalRead(Fsensor);
  L = digitalRead(Lsensor);
  R = digitalRead(Rsensor);
  // B = digitalRead(Bsensor);
  UP = digitalRead(UPsensor);
}

void Left() {
  setMotor(turnSpeed, -turnSpeed);
  delay(LturnTime);
  setMotor(0, 0);
  delay(50);
  setMotor(MASPEED, MBSPEED);
  delay(2000);
  Serial.print("   L");
}

void Right() {
  setMotor(-turnSpeed, turnSpeed);
  delay(RturnTime);
  setMotor(0, 0);
  delay(50);
  Serial.print("   R");
}

void Back() {
  setMotor(-turnSpeed, turnSpeed);
  delay(BturnTime);
  setMotor(0, 0);
  delay(50);
  Serial.print("   B");
}

void Forward() {
  setMotor(MASPEED, MBSPEED);
  Serial.print("   F");
}

void setMotor(int MASpeed, int MBSpeed) {
  if (MBSpeed > 0) {
    digitalWrite(MBDIR, LOW);
    analogWrite(MBPWM, MBSpeed);
  } else if (MBSpeed < 0) {
    digitalWrite(MBDIR, HIGH);
    analogWrite(MBPWM, abs(MBSpeed));
  } else {
    analogWrite(MBPWM, MBSpeed);
  }
  if (MASpeed > 0) {
    digitalWrite(MADIR, HIGH);
    analogWrite(MAPWM, MASpeed);
  } else if (MASpeed < 0) {
    digitalWrite(MADIR, LOW);
    analogWrite(MAPWM, abs(MASpeed));
  } else {
    analogWrite(MAPWM, MASpeed);
  }
}