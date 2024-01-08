#include <QTRSensors.h>
#include<util/delay.h>
QTRSensors qtr;
const uint8_t SensorCount = 5;
uint16_t sensorValues[SensorCount];

unsigned int line_position = 0; // 0-5000 之间的值表示传感器 0 - 4 之间的线的位置

#define WT 250         //黑色大于 500 白色小于 300
#define BT 350

//int MADIR = 4;       // 电机设置方向引脚
//int MAPWM = 5;       // 电机设置速度引脚
//int MBDIR = 7;       // 电机设置方向引脚
//int MBPWM = 6;       // 电机设置速度引脚

int MAPWM = 5;         //电机设置速度引脚
int MBPWM = 6;         //电机设置速度引脚
int speed_lt = 50;
int MADIR = 4;         //电机设置方向引脚
int MBDIR = 7;         //电机设置方向引脚

// 用于迷宫导航的电机调整变量
int calSpeed = 50;    // 调谐值电机将在自动校准扫描转动时运行 (0-255)
int turnSpeed = 120;   // 调整值电机将在转动时运行 (0-255)
int turnSpeedSlow = 150;  // 调整值电机将在它们从转动周期减速时运行以避免超限 (0-255)
int drivePastDelay = 180; // 以毫秒为单位的调整值电机将运行通过交叉点以对齐车轮以进行转弯

// pid
float error = 0;
float lastError = 0;
float PV = 0 ;
float kp = 0;        // 在 follow_line() 函数中调整值
float ki = 0;
float kd = 0;        // 在 follow_line() 函数中调整值
int m1Speed = 0;
int m2Speed = 0;
int motorspeed = 0;

// path 变量将存储机器人走过的路径。 它存储为一个字符数组，每个字符代表应该在序列中的一个交叉点处进行的转弯：
// 'L' 为左
// 'R' 为右边
// 'S' 表示直行（直行通过交叉路口）
// 'B' 表示后退（掉头）
// 您应该检查以确保迷宫设计的 path_length 没有超出数组的边界。
char path[100] = "";
unsigned char path_length = 0; // the length of the path

void calibrate_LDR()
{
  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);       // 打开 Arduino 的 LED 指示我们处于校准模式
  delay(1000);
  for (uint16_t i = 0; i < 102; i++)
  {
    qtr.calibrate();                    // 以每次读取 2500 us 的速度读取所有传感器
    if (i < 25 || i >= 75) {
      digitalWrite(MADIR, HIGH);        // 左转
      analogWrite(MAPWM, calSpeed);
      digitalWrite(MBDIR, HIGH);
      analogWrite(MBPWM, calSpeed);
    }
    else {
      digitalWrite(MADIR, LOW);         // 右转
      analogWrite(MAPWM, calSpeed);
      digitalWrite(MBDIR, LOW);
      analogWrite(MBPWM, calSpeed);
    }
  }
  analogWrite(MAPWM, 0);
  analogWrite(MBPWM, 0);
  digitalWrite(LED_BUILTIN, HIGH);       // 关闭 Arduino 的 LED 以指示我们已完成校准
  delay(1000);
}

void setup()
{
  // 传感器连接端口IR1-IR5连接到端口2.3.8.9.14端口
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]) {
    2, 3, 8, 9, 14
  }, SensorCount);
  qtr.setEmitterPin(15);
  pinMode(MAPWM, OUTPUT);
  pinMode(MBPWM, OUTPUT);
  pinMode(MADIR, OUTPUT);
  pinMode(MBDIR, OUTPUT);
  calibrate_LDR();
  Serial.begin(9600);
}

void follow_line()         // 跟随子程序 PD控制   添加恒速speed_lt
{
  lastError = 0;
  while (1)
  {
    //    uint16_t position = qtr.readLineBlack(sensorValues);
    line_position = qtr.readLineBlack(sensorValues);
    Serial.print(sensorValues[0]);
    Serial.print(" ");
    Serial.print(sensorValues[1]);
    Serial.print(" ");
    Serial.print(sensorValues[2]);
    Serial.print(" ");
    Serial.print(sensorValues[3]);
    Serial.print(" ");
    Serial.print(sensorValues[4]);
    Serial.print(" ");
    Serial.println(line_position);
    switch (line_position)
    {
      // case 0 和 case 4000 用于 PD 行的说明中，代码如下。
      // 保留在此处作为参考。否则可能会取消开关功能。
      // 线已移出传感器的左边缘传感器
      case 0:
        digitalWrite(MADIR, HIGH);
        analogWrite(MAPWM, 50);
        digitalWrite(MBDIR, HIGH);
        analogWrite(MBPWM, 50);
        Serial.println("Rotate Left\n");
        break;
      // 线已经离开传感器的左边缘
      case 4000:
        digitalWrite(MADIR, LOW);
        analogWrite(MAPWM, 50);
        digitalWrite(MBDIR, LOW);
        analogWrite(MBPWM, 50);
        Serial.println("Rotate Right\n");
        break;
      default:
        error = (float)line_position - 2000;
        // 根据比例和微分 PID 项设置电机速度
        // kp 是浮点比例常数（可能从 0.5 左右的值开始）
        // kd 是浮点导数常数（可能从 1 左右的值开始）
        // 请注意，在进行 PID 时，正确的标志非常重要，否则控制回路将不稳定
        //        kp = 0.2;
        //        kd = 3;
        //        PV = kp * error + kd * (error - lastError);
        kp = 3;
        ki = 0;
        kd = 3;
        PV = error / 3 + kd * (error - lastError) / 400;
        lastError = error;
        //此代码限制 PV（电机速度 pwm 值）  限制 PV 为 50
        if (PV > 50)
        {
          PV = 50;
        }
        if (PV < -50)
        {
          PV = -50;
        }
        m1Speed = 90 + PV - speed_lt;
        m2Speed = 90 - PV - speed_lt;

        digitalWrite(MADIR, HIGH);
        analogWrite(MAPWM, m2Speed);
        digitalWrite(MBDIR, LOW);
        analogWrite(MBPWM, m1Speed);
        break;
    }
    // 我们使用内部 3 个传感器（1 到 3）topid 完美运行！
    // 确定前方是否有直线，如果路径转弯，则使用传感器 0 和 4。
    // 读取原始传感器值并更改它们
    if (sensorValues[0] < WT && sensorValues[1] < WT && sensorValues[2] < WT && sensorValues[3] < WT && sensorValues[4] < WT)
    {
      // 无路可走
      Serial.print(" dead end");
      return;
    }
    else if ((sensorValues[0] > BT && sensorValues[1] > BT) || (sensorValues[3] > BT && sensorValues[4] > BT))
      // 根据原始传感器值变化
    {
      Serial.print(" intersection found");
      // 找到一个交叉点。
      delay(1000);
      return;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
// 这个函数决定在解谜的学习阶段转向哪条路。 它使用变量found_left、found_straight和found_right，
// 表示三个方向的每个方向是否都有退出，应用“左手上墙”策略。
char select_turn(unsigned char found_left, unsigned char found_straight, unsigned char found_right)
{
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
///////////////////////////////////////////////////////////////////////////////////////////

void turn(char dir)
{
  switch (dir)
  {
    case 'L':                       // 左转90度
      digitalWrite(MADIR, HIGH);
      digitalWrite(MBDIR, HIGH);
      analogWrite(MAPWM, turnSpeedSlow);
      analogWrite(MBPWM, turnSpeedSlow);
      while (sensorValues[2] > BT)  //调整-等待线位置找到靠近中心的位置
      {
        line_position = qtr.readLineBlack(sensorValues);
      }
      analogWrite(MAPWM, 0);
      analogWrite(MBPWM, 0);
      break;

    case 'R':                        // 向右转 90 度
      digitalWrite(MADIR, LOW);
      digitalWrite(MBDIR, LOW);
      analogWrite(MAPWM, turnSpeedSlow);
      analogWrite(MBPWM, turnSpeedSlow);
      while (sensorValues[2] > BT)   // 调整-等待线位置找到靠近中心的位置
      {
        line_position = qtr.readLineBlack(sensorValues);
      }
      analogWrite(MAPWM, 0);
      analogWrite(MBPWM, 0);
      break;

    case 'B':                        // 右转180度返回
      digitalWrite(MADIR, LOW);
      digitalWrite(MBDIR, LOW);
      analogWrite(MAPWM, turnSpeedSlow);
      analogWrite(MBPWM, turnSpeedSlow);
      while (sensorValues[2] < BT)  // 调整-等待线位置找到靠近中心的位置
      {
        line_position = qtr.readLineBlack(sensorValues);
      }
      analogWrite(MAPWM, 0);
      analogWrite(MBPWM, 0);
      break;

    case 'S':       // 直行
      break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////

void simplify_path()
{
  // 仅当倒数第二个转弯是“B”时才简化路径
  if (path_length < 3 || path[path_length - 2] != 'B')
    return;

  int total_angle = 0;
  int i;
  for (i = 1; i <= 3; i++)
  {
    switch (path[path_length - i])
    {
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
  // 获取角度为 0 到 360 度之间的数字。
  total_angle = total_angle % 360;
  // 用一个回合替换所有这些回合。
  switch (total_angle)
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
  // 路径现在缩短了两步。
  path_length -= 2;
}

///////////////////////////////////////////////////////////////////////////////////////////
void MazeSolve()
{
  //循环直到我们解决了迷宫。
  while (1)
  {
    // 第一个主循环体
    follow_line();
    // 这些变量记录机器人在检查当前交叉点时是否看到了向左、直行和向右的线
    unsigned char found_left = 0;
    unsigned char found_straight = 0;
    unsigned char found_right = 0;

    // 现在读取传感器并检查交叉点类型。
    line_position = qtr.readLineBlack(sensorValues);

    // 检查左右出口
    if (sensorValues[4] > BT)
      found_right = 1;
    if (sensorValues[0] > BT)
      found_left = 1;

    // 再直行一点 - 这足以让我们的车轮与十字路口对齐。
    digitalWrite(MADIR, HIGH);
    digitalWrite(MBDIR, LOW);
    analogWrite(MBPWM, 50);
    analogWrite(MAPWM, 50);
    delay(drivePastDelay);
    digitalWrite(MAPWM, 0);
    digitalWrite(MBPWM, 0);
    delay(400);
    line_position = qtr.readLineBlack(sensorValues);
    if (sensorValues[0] > BT || sensorValues[1] > BT || sensorValues[2] > BT || sensorValues[3] > BT || sensorValues[4] > BT)
      found_straight = 1;

    // 检查结束点。如果所有五个中间传感器都是深黑色，我们就解决了迷宫。
    if (sensorValues[0] > BT && sensorValues[1] > BT && sensorValues[2] > BT && sensorValues[3] > BT && sensorValues[4] > BT)
      break;

    // 交叉口识别完成。如果迷宫已经解决了，我们可以沿着现有的路径走。 否则，我们需要学习解决方案。
    unsigned char dir = select_turn(found_left, found_straight, found_right);
    Serial.print(" turning");
    Serial.println(dir);
    turn(dir);              // 使路径指示的转弯。
    path[path_length] = dir;
    path_length ++;         // 将交集存储在路径变量中。
    simplify_path();        // 简化学习路径。
  }
  analogWrite(MAPWM, 0);
  analogWrite(MBPWM, 0);
  // 写出解决路径的指示
  for (int i = 0 ; i < 5; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    _delay_ms(250);
  }
  // 解决了迷宫！现在进入一个无限循环 - 我们可以根据需要多次重新运行迷宫。
  while (1)
  {
    analogWrite(MAPWM, 0);
    analogWrite(MBPWM, 0);
    // 延迟让你有时间放开机器人
    delay(5000);                       // 删除或减少此项以节省时间
    // 重新运行现在解决的迷宫。 没有必要识别交叉点，所以这个循环非常简单。
    int i;
    for (i = 0; i < path_length; i++)
    {
      // 第二个主循环体
      follow_line();
      // 驶过交叉路口稍微慢一点和定时延迟以在线对齐车轮
      digitalWrite(MADIR, LOW);
      analogWrite(MAPWM, 50);
      digitalWrite(MBDIR, LOW);
      analogWrite(MBPWM, 50);
      delay(drivePastDelay);
      // 调整时间以允许车轮定位以进行正确转弯 根据路径 [i] 中存储的指令进行转弯。
      turn(path[i]);
    }
    // 跟随最后一段直到结束。
    follow_line();
    digitalWrite(MADIR, LOW);
    analogWrite(MAPWM, 50);
    digitalWrite(MBDIR, LOW);
    analogWrite(MBPWM, 50);
    // 调整时间以允许车轮定位以正确转弯 现在我们应该在终点了！ 现在再次移动机器人，它将再次使用解决方案重新运行此循环。
    delay(drivePastDelay);
  }
}
///////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  line_position = qtr.readLineBlack(sensorValues);
  // 开始解迷宫
  MazeSolve(); // 注释掉并运行串行监视器来测试传感器，同时校准线
  delay(50);
}
