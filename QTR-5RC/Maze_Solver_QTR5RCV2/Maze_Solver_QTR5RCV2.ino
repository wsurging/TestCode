/**********************************************************************************************
   迷宫解谜小车  
   Legoduino PRO V2主板适配程序
***********************************************************************************************/
#include <QTRSensors.h>

QTRSensors qtr;
const uint8_t SensorCount = 5;  // 设置传感器数量
uint16_t sensorValues[SensorCount];

unsigned int position = 0;  // 0-4000 之间的值表示传感器 0 - 4 之间的线的位置

int MADIR = 4;  //电机MA设置方向引脚
int MAPWM = 5;  //电机MA设置速度引脚
int MBPWM = 6;  //电机MB设置速度引脚
int MBDIR = 7;  //电机MB设置方向引脚

int Speed = 200;      // 电机PID调节时的初始速度200
int turnSpeed = 200;  // 调整电机转向时速度 (0-255)200
int calSpeed = 100;   // 初始校准时速度100
int slowSpeed = 100;  // 向前运行一段距离时速度100

int LturnTime = 260;  // 左转向时间延迟200
int RturnTime = 260;  // 右转向时间延迟200
int upTime = 300;     // 检测到线向前移动时间，使转向时对准线中心300

int Time = 130;  // 解谜后检测到路口时向前运行时间130

#define WT 150  // 黑色大于 500 白色小于 150
#define BT 500

// path 变量将存储机器人走过的路径。 它存储为一个字符数组，每个字符代表应该在序列中的一个交叉点处进行的转弯。
// 您应该检查以确保迷宫设计的 path_length 没有超出数组的边界。
char path[100] = "";
unsigned char path_length = 0;  // 路径的长度

void calibrate_LDR() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  // 打开 Arduino 的 LED 指示我们处于校准模式
  delay(1000);
  for (uint16_t i = 0; i < 100; i++) {
    qtr.calibrate();  // 以每次读取 2500 us 的速度读取所有传感器
    if (i < 25 || i >= 75) {
      setMotor(-calSpeed, calSpeed);  // 右转
    } else {
      setMotor(calSpeed, -calSpeed);  // 左转
    }
  }
  setMotor(0, 0);
  digitalWrite(LED_BUILTIN, HIGH);  // 关闭 Arduino 的 LED 以指示我们已完成校准
  delay(500);
}

void setup() {
  Serial.begin(9600);
  // 传感器连接端口IR1-IR5连接到端口10, 11, 9, 8, 3端口,使能端口2
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){ 10, 11, 9, 8, 3 }, SensorCount);
  qtr.setEmitterPin(2);
  pinMode(MAPWM, OUTPUT);
  pinMode(MBPWM, OUTPUT);
  pinMode(MADIR, OUTPUT);
  pinMode(MBDIR, OUTPUT);
  calibrate_LDR();
}

void follow_line() {  // 跟随子程序 PD控制   添加恒速speed_lt
  while (1) {
    uint16_t position = qtr.readLineBlack(sensorValues);
    switch (position) {
      default:
        int val = map(position, 1000, 3000, -(255 - Speed), (255 - Speed));
        if (position < 2000) {
          setMotor(Speed - val, Speed + val);
        } else if (position > 2000) {
          setMotor(Speed - val, Speed + val);
        } else {
          setMotor(Speed, Speed);
        }
        break;
    }
    // for (uint8_t i = 0; i < SensorCount; i++) {
    //   Serial.print(sensorValues[i]);
    //   Serial.print('\t');
    // }
    // Serial.println(position);

    // 确定前方是否有直线，如果路径转弯，则使用传感器 0 - 4。读取原始传感器值并更改它们
    if (sensorValues[0] < WT && sensorValues[1] < WT && sensorValues[2] < WT && sensorValues[3] < WT && sensorValues[4] < WT) {
      // Serial.print(" dead end");  // 无路可走
      return;
    } else if (sensorValues[0] > BT || sensorValues[4] > BT) {  // 传感器1或者4检测到黑线
      // Serial.print(" intersection found");                      // 找到一个交叉点
      return;
    }
  }
}

void turn(char dir) {
  switch (dir) {
    case 'L':  // 左转90度
      // Serial.print(" L ");
      // delay(1000);
      setMotor(turnSpeed, -turnSpeed);
      delay(LturnTime);
      while (sensorValues[2] < BT) {  // 调整-等待线位置找到靠近中心的位置
        position = qtr.readLineBlack(sensorValues);
      }
      setMotor(0, 0);
      delay(50);
      break;

    case 'R':  // 向右转 90 度
      // Serial.print(" R ");
      // delay(1000);
      setMotor(-turnSpeed, turnSpeed);
      delay(RturnTime);
      while (sensorValues[2] < BT) {  // 调整-等待线位置找到靠近中心的位置
        position = qtr.readLineBlack(sensorValues);
      }
      setMotor(0, 0);
      delay(50);
      break;

    case 'B':  // 右转180度返回
      // Serial.print(" B ");
      // delay(1000);
      setMotor(-turnSpeed, turnSpeed);
      delay(RturnTime);
      while (sensorValues[2] < BT) {  // 调整-等待线位置找到靠近中心的位置
        position = qtr.readLineBlack(sensorValues);
      }
      setMotor(0, 0);
      delay(50);
      break;

    case 'S':  // 直行
      // Serial.print(" S ");
      // delay(1000);
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
    position = qtr.readLineBlack(sensorValues);

    // 检查左右出口
    if (sensorValues[0] > BT || (sensorValues[0] > BT && sensorValues[1] > BT))
      found_left = 1;

    if (sensorValues[4] > BT || (sensorValues[3] > BT && sensorValues[4] > BT) || (sensorValues[2] > BT && sensorValues[3] > BT && sensorValues[4] > BT))
      found_right = 1;

    // 再直行一点,让车轮与十字路口对齐。
    setMotor(slowSpeed, slowSpeed);
    delay(upTime);

    position = qtr.readLineBlack(sensorValues);

    if (sensorValues[0] > BT || sensorValues[1] > BT || sensorValues[2] > BT || sensorValues[3] > BT || sensorValues[4] > BT)
      found_straight = 1;

    // 检查结束点。如果所有五个中间传感器都是深黑色，我们就解决了迷宫。
    if (sensorValues[0] > BT && sensorValues[1] > BT && sensorValues[2] > BT && sensorValues[3] > BT && sensorValues[4] > BT)
      break;

    // 交叉口识别完成。如果迷宫已经解决了，我们可以沿着现有的路径走。 否则，我们需要学习解决方案。
    unsigned char dir = select_turn(found_left, found_straight, found_right);
    int recint = record_intersec(found_left, found_straight, found_right);
    //    Serial.print(" turning");
    //    Serial.println(dir);
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
  uint16_t position = qtr.readLineBlack(sensorValues);
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