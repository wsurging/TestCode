/*
   迷宫解谜小车
*/
#include <Wire.h>  // Include the I2C library (required)
#include "YFLineFollow.h"

YFLINEFOLLOW YFLS;

// Pin definition:
const byte lineSensor1 = 0;  // ir sensor to SX1508's pin 0
const byte lineSensor2 = 1;  // ir sensor to SX1508's pin 1
const byte lineSensor3 = 2;  // ir sensor to SX1508's pin 2
const byte lineSensor4 = 3;  // ir sensor to SX1508's pin 3
const byte lineSensor5 = 4;  // ir sensor to SX1508's pin 4

int MADIR = 4;  //电机MA设置方向引脚
int MAPWM = 5;  //电机MA设置速度引脚
int MBPWM = 6;  //电机MB设置速度引脚
int MBDIR = 7;  //电机MB设置方向引脚

int Speed = 80;      // 电机PID调节时的初始速度200
int turnSpeed = 50;  // 调整电机转向时速度 (0-255)200
int slowSpeed = 50;  // 向前运行一段距离时速度100

int LturnTime = 580;   // 左转向时间延迟200
int RturnTime = 560;   // 右转向时间延迟200
int BturnTime = 1200;  // 调头时间延迟280
int upTime = 450;      // 检测到线向前移动时间，使转向时对准线中心300

int Time = 300;  // 解谜后检测到路口时向前运行时间130

// path 变量将存储机器人走过的路径。 它存储为一个字符数组，每个字符代表应该在序列中的一个交叉点处进行的转弯。
// 您应该检查以确保迷宫设计的 path_length 没有超出数组的边界。
char path[100] = "";
unsigned char path_length = 0;  // 路径的长度

void sensorRead() {
  uint8_t ls1 = YFLS.readSensor(lineSensor1);
  uint8_t ls2 = YFLS.readSensor(lineSensor2);
  uint8_t ls3 = YFLS.readSensor(lineSensor3);
  uint8_t ls4 = YFLS.readSensor(lineSensor4);
  uint8_t ls5 = YFLS.readSensor(lineSensor5);
}

void setup() {
  Serial.begin(9600);
  // Call xx.begin() to initialize the line follow sensor. If it successfully communicates, it'll return 1.
  if (YFLS.begin() == false) {
    Serial.println("Failed to communicate. Check wiring.");
    while (1)
      ;  // If we fail to communicate, loop forever.
  }
  pinMode(MAPWM, OUTPUT);
  pinMode(MBPWM, OUTPUT);
  pinMode(MADIR, OUTPUT);
  pinMode(MBDIR, OUTPUT);
  Serial.println("OK");
  delay(1000);
  YFLS.enableSensor();  // 使能传感器，默认使能
}

void follow_line() {  // 跟随子程序
  while (1) {
    uint8_t ls1 = YFLS.readSensor(lineSensor1);
    uint8_t ls2 = YFLS.readSensor(lineSensor2);
    uint8_t ls3 = YFLS.readSensor(lineSensor3);
    uint8_t ls4 = YFLS.readSensor(lineSensor4);
    uint8_t ls5 = YFLS.readSensor(lineSensor5);
    switch (ls3) {
      default:
        if (ls2 == LOW) {
          setMotor(Speed, Speed - 50);
        } else if (ls3 == LOW) {
          setMotor(Speed, Speed);
        } else if (ls4 == LOW) {
          setMotor(Speed - 50, Speed);
        }
        break;
    }

    // 确定前方是否有直线，如果路径转弯，则使用传感器1和5
    if (ls2 == HIGH && ls3 == HIGH && ls4 == HIGH) {
      Serial.print("dead end");  // 无路可走
      return;
    } else if (ls1 == LOW || ls5 == LOW) {  // 传感器1或者5检测到黑线
      Serial.print("found intersection ");  // 找到一个交叉点
      return;
    }
  }
}

void turn(char dir) {
  uint8_t ls1 = YFLS.readSensor(lineSensor1);
  uint8_t ls2 = YFLS.readSensor(lineSensor2);
  uint8_t ls3 = YFLS.readSensor(lineSensor3);
  uint8_t ls4 = YFLS.readSensor(lineSensor4);
  uint8_t ls5 = YFLS.readSensor(lineSensor5);
  switch (dir) {
    case 'L':  // 左转90度
      Serial.println("     L");
      setMotor(turnSpeed, -turnSpeed);
      delay(LturnTime);
      setMotor(0, 0);
      delay(50);
      break;

    case 'R':  // 向右转 90 度
      Serial.println("     R");
      setMotor(-turnSpeed, turnSpeed);
      delay(RturnTime);
      setMotor(0, 0);
      delay(50);
      break;

    case 'B':  // 右转180度返回
      Serial.println("     B");
      setMotor(-turnSpeed, turnSpeed);
      delay(BturnTime);
      // while (ls3 == LOW) {  // 调整-等待线位置找到靠近中心的位置
      // sensorRead();
      // }
      setMotor(0, 0);
      delay(50);
      break;

    case 'S':  // 直行
      Serial.println("     S");
      break;
  }
}

void MazeSolve() {
  while (1) {  // 循环直到我们解决了迷宫。
    // 第一个主循环体
    follow_line();
    // 这些变量记录机器人在检查当前交叉点时是否看到了向左、直行和向右的线
    unsigned char found_left = 0;
    unsigned char found_straight = 0;
    unsigned char found_right = 0;

    // 现在读取传感器并检查交叉点类型。
    uint8_t ls1 = YFLS.readSensor(lineSensor1);
    uint8_t ls2 = YFLS.readSensor(lineSensor2);
    uint8_t ls3 = YFLS.readSensor(lineSensor3);
    uint8_t ls4 = YFLS.readSensor(lineSensor4);
    uint8_t ls5 = YFLS.readSensor(lineSensor5);

    if (ls1 == LOW && ls2 == LOW && ls3 == LOW && ls4 == LOW && ls5 == LOW) {
      setMotor(slowSpeed, slowSpeed);  // 再直行一点,让车轮与十字路口对齐。
      delay(upTime);
      uint8_t ls1 = YFLS.readSensor(lineSensor1);
      uint8_t ls2 = YFLS.readSensor(lineSensor2);
      uint8_t ls3 = YFLS.readSensor(lineSensor3);
      uint8_t ls4 = YFLS.readSensor(lineSensor4);
      uint8_t ls5 = YFLS.readSensor(lineSensor5);
      // sensorRead();
      if (ls1 == LOW && ls3 == LOW && ls5 == LOW) {
        Serial.println(" STOP--------OK");
        break;
      } else {
        found_left = 1;
        Serial.println(" found   L");
      }
    }

    if (ls1 == HIGH && ls3 == LOW && ls5 == LOW || ls1 == HIGH && ls3 == LOW && ls4 == LOW && ls5 == LOW) {
      setMotor(slowSpeed, slowSpeed);  // 再直行一点,让车轮与十字路口对齐。
      delay(upTime);
      uint8_t ls1 = YFLS.readSensor(lineSensor1);
      uint8_t ls2 = YFLS.readSensor(lineSensor2);
      uint8_t ls3 = YFLS.readSensor(lineSensor3);
      uint8_t ls4 = YFLS.readSensor(lineSensor4);
      uint8_t ls5 = YFLS.readSensor(lineSensor5);
      // sensorRead();
      if (ls3 == LOW) {
        found_straight = 1;
        Serial.println(" found   S");
      } else {
        found_right = 1;
        Serial.println(" found   R");
      }
    }

    if (ls1 == LOW && ls5 == HIGH) {
      setMotor(slowSpeed, slowSpeed);  // 再直行一点,让车轮与十字路口对齐。
      delay(upTime);
      found_left = 1;
      Serial.println(" found   L");
    }




    // // 检查左右出口
    // if (ls1 == LOW || ls1 == LOW && ls3 == LOW && ls5 == LOW) {
    //   found_left = 1;
    //   Serial.println(" found   L");
    //   delay(50);
    // }
    // if (ls2 == LOW || ls3 == LOW || ls4 == LOW) {
    //   found_straight = 1;
    //   Serial.println(" found   S");
    //   delay(50);
    // }
    // if (ls5 == LOW) {
    //   found_right = 1;
    //   Serial.println(" found   R");
    //   delay(50);
    // }

    // // 再直行一点,让车轮与十字路口对齐。
    // setMotor(slowSpeed, slowSpeed);
    // delay(upTime);

    // sensorRead();

    // if (ls2 == LOW || ls3 == LOW || ls4 == LOW)
    //   found_straight = 1;

    // // 检查结束点。如果所有五个中间传感器都是深黑色，我们就解决了迷宫。
    // if (ls1 == LOW && ls2 == LOW && ls3 == LOW && ls4 == LOW && ls5 == LOW)
    //   break;
    // // Serial.println(" found  sssss");

    // 交叉口识别完成。如果迷宫已经解决了，我们可以沿着现有的路径走。 否则，我们需要学习解决方案。
    unsigned char dir = select_turn(found_left, found_straight, found_right);
    int recint = record_intersec(found_left, found_straight, found_right);

    // Serial.print(" turning");
    //  Serial.println(dir);

    turn(dir);  // 使路径指示的转弯。
    path[path_length] = dir;

    path_length++;    // 将交集存储在路径变量中。
    simplify_path();  // 简化学习路径。
  }
  setMotor(0, 0);
  // 写出解决路径的指示
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  //   解决了迷宫！现在进入一个无限循环 - 我们可以根据需要多次重新运行迷宫。
  while (1) {
    setMotor(0, 0);
    // 延迟让你有时间放开机器人
    delay(5000);  // 删除或减少此项以节省时间
    // 重新运行现在解决的迷宫。 没有必要识别交叉点，所以这个循环非常简单。
    int i;
    for (i = 0; i < path_length; i++) {
      // 第二个主循环体
      follow_line();
      // 驶过交叉路口稍微慢一点和定时延迟以在线对齐车轮
      setMotor(Speed, Speed);
      delay(Time);
      // 调整时间以允许车轮定位以进行正确转弯 根据路径 [i] 中存储的指令进行转弯。
      turn(path[i]);
    }
    // 跟随最后一段直到结束。
    follow_line();
    setMotor(Speed, Speed);
    // 调整时间以允许车轮定位以正确转弯 现在我们应该在终点了！ 现在再次移动机器人，它将再次使用解决方案重新运行此循环。
    delay(Time);
  }
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

void loop() {
  sensorRead();
  MazeSolve();  // 开始解迷宫
}

// 这个函数决定在解谜的学习阶段转向哪条路。 它使用变量found_left、found_straight和found_right，
// 表示三个方向的每个方向是否都有退出，应用“左手上墙”策略。
char select_turn(unsigned char found_left, unsigned char found_straight, unsigned char found_right) {
  // 决定如何转弯。 下面的代码实现了左手靠墙策略，我们总是尽可能向左转。
  if (found_left)
    return 'L';
  else if (found_straight)
    return 'S';
  else if (found_right)
    return 'R';
  else
    return 'B';
}

// 路口记录代码
int record_intersec(unsigned char found_left, unsigned char found_straight, unsigned char found_right) {
  // 决定如何转弯。 下面的代码实现了左手靠墙策略，我们总是尽可能向左转。
  if (found_left && found_right && found_straight)  // LRS->4
    return '4';
  else if (found_left && found_right)  // LR->3
    return '3';
  else if (found_right && found_straight)  // RS->2
    return '2';
  else if (found_left && found_straight)  // LS->1
    return '1';
  else  // No intersection->0
    return '0';
}

void simplify_path() {
  // 仅当倒数第二个转弯是“B”时才简化路径
  if (path_length < 3 || path[path_length - 2] != 'B')
    return;

  int total_angle = 0;
  int i;
  for (i = 1; i <= 3; i++) {
    switch (path[path_length - i]) {
      case 'R':
        total_angle += 90;
        break;
      case 'L':
        total_angle += 270;
        break;
      case 'B':
        total_angle += 180;
        break;
    }
  }

  total_angle = total_angle % 360;  // 获取角度为 0 到 360 度之间的数字。

  switch (total_angle)  // 用一个回合替换所有这些回合。
  {
    case 0:
      path[path_length - 3] = 'S';
      break;
    case 90:
      path[path_length - 3] = 'R';
      break;
    case 180:
      path[path_length - 3] = 'B';
      break;
    case 270:
      path[path_length - 3] = 'L';
      break;
  }
  path_length -= 2;  // 路径现在缩短了两步。
}