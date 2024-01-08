#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <QTRSensors.h>
#include <avr/pgmspace.h>

QTRSensors qtr;

const uint8_t SensorCount = 5;     // 设置传感器数量
uint16_t sensorValues[SensorCount];

int MADIR = 4;   // 电机MA设置方向引脚
int MAPWM = 5;   // 电机MA设置速度引脚
int MBDIR = 7;   // 电机MB设置方向引脚
int MBPWM = 6;   // 电机MB设置速度引脚

#define TURNLEFT setMotor(50,-50);   //设置电机左转时电机速度和方向
#define TURNRIGHT setMotor(-50,50);  //设置电机右转时电机速度和方向
#define GOSTRAIGHT setMotor(50,50);  //设置电机直行时电机速度和方向
#define TESTDELAY delay(200);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char welcomeLine1[]  =   "YFROBOT";
const char welcomeLine2[]  =   "  Maze  solver  ";
byte levels[]  = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

void setCharacters() {
  lcd.init();
  lcd.createChar(0, levels);
  lcd.createChar(1, levels + 1);
  lcd.createChar(2, levels + 2);
  lcd.createChar(3, levels + 3);
  lcd.createChar(4, levels + 4);
  lcd.createChar(5, levels + 5);
  lcd.createChar(6, levels + 6);
  lcd.clear();
}

// 电机驱动函数
void setMotor(int MOTORA, int MOTORB)
{
  if (MOTORA >= 0) {
    digitalWrite(MADIR, HIGH);
    analogWrite(MAPWM, MOTORA);
  } else {
    digitalWrite(MADIR, LOW);
    analogWrite(MAPWM, abs(MOTORA));
  }
  if (MOTORB > 0) {
    digitalWrite(MBDIR, LOW);
    analogWrite(MBPWM, MOTORB);
  } else {
    digitalWrite(MBDIR, HIGH);
    analogWrite(MBPWM, abs(MOTORB));
  }
}

void displayReading(unsigned int *calibratedValues) {
  unsigned int i;
  for (i = 0; i < 5; i++) {
    const int displayCharacters[10] = {' ', 0, 0, 1, 2, 3, 4, 5, 6, 255};
    int c = displayCharacters[sensorValues[i] / 101];
    lcd.write(c);
  }
}
void setup()
{
  // 设置QTR-5RC传感器的连接端口IR1-IR5对应2.3.8.9.14，并设置使能端口为15（A1）
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]) {
    2, 3, 8, 9, 14
  }, SensorCount);
  qtr.setEmitterPin(15);

  delay(500);
  setCharacters(); 
  pinMode(MADIR, OUTPUT);  // 端口设置为输出模式
  pinMode(MAPWM, OUTPUT);
  pinMode(MBDIR, OUTPUT);
  pinMode(MBPWM, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);        // 打开 Arduino 的 LED 指示我们处于校准模式
  for (int i = 0; i < 102; i++)          // 给时间以在启动是左右转使传感器校准
  {
    qtr.calibrate();
    if (i < 25 || i >= 75)
      setMotor(50, -50);
    else
      setMotor(-50, 50);
  }
  digitalWrite(LED_BUILTIN, HIGH);       // 关闭 Arduino 的 LED 以指示我们已完成校准
  setMotor(0, 0);
  lcd.setCursor(0, 0);
  lcd.print(welcomeLine1);
  lcd.setCursor(0, 1);
  lcd.print(welcomeLine2);
  delay(1500);
  lcd.clear();
  uint16_t position = qtr.readLineBlack(sensorValues);
  lcd.print(position);
  lcd.setCursor(0, 1);
  displayReading(sensorValues);
  delay(1000);
}

void followSegment() {
  int lastProportional = 0;
  long integral = 0;
  while (1) {
    uint16_t position = qtr.readLineBlack(sensorValues);
    int proportional = ((int)position) - 2000;

    int derivative = proportional - lastProportional;
    integral += proportional;

    lastProportional = proportional;
    int powerDifference = proportional / 3 + derivative * 3 / 400;
    
    const int maxcount = 60;
    if (powerDifference > maxcount)
      powerDifference = maxcount;
    if (powerDifference < -maxcount)
      powerDifference = -maxcount;

    if (powerDifference < 0)
      setMotor(maxcount + powerDifference, maxcount);
    else
      setMotor(maxcount, maxcount - powerDifference);
    if (sensorValues[1] < 200 && sensorValues[2] < 200 && sensorValues[3] < 200) {
      // There is no line visible ahead, and we didn't see any
      // intersection.  Must be a dead end.
      return;
    }
    else if (sensorValues[0] > 300 || sensorValues[4] > 300)
    {
      // Found an intersection.
      return;
    }
  }
}
void turn(unsigned char dir) {
  switch (dir) {
    case 'L':
      TURNLEFT;
      delay(330);
      break;
    case 'R':
      TURNRIGHT;
      delay(310);
      break;
    case 'B':
      TURNRIGHT;
      delay(600);
      break;
    case 'S':
      break;
  }
}

char path[100] = "";
unsigned char pathLength = 0;
void displayPath() {
  path[pathLength] = 0;
  lcd.clear();
  lcd.print(path);
  if (pathLength > 8) {
    lcd.setCursor(0, 1);
    lcd.print (path + 8);
  }
}

unsigned char selectTurn(unsigned char foundLeft, unsigned char foundStraight, unsigned char foundRight) {
  if (foundLeft)
    return'L';
  else if (foundStraight)
    return 'S';
  else if (foundRight)
    return 'R';
  else
    return 'B';
}

void simplifyPath() {
  if (pathLength < 3 || path[pathLength - 2] != 'B')
    return;
  int totalAngle = 0;
  int i;
  for (i = 1; i <= 3; i++)
  {
    switch (path[pathLength - i])
    {
      case 'R':
        totalAngle += 90;
        break;
      case 'L':
        totalAngle += 270;
        break;
      case 'B':
        totalAngle += 180;
        break;
    }
  }
  totalAngle = totalAngle % 360;
  // 用一个回合替换所有这些回合。
  switch (totalAngle)
  {
    case 0:
      path[pathLength - 3] = 'S';
      break;
    case 90:
      path[pathLength - 3] = 'R';
      break;
    case 180:
      path[pathLength - 3] = 'B';
      break;
    case 270:
      path[pathLength - 3] = 'L';
      break;
  }

  // 路径现在缩短了两步。
  pathLength -= 2;
}

void loop() {
  while (1) {
    setMotor(70, 70);
    delay(50);
    unsigned char foundLeft = 0;
    unsigned char foundStraight = 0;
    unsigned char foundRight = 0;
    // unsigned int sensorValues[5];
    uint16_t position = qtr.readLineBlack(sensorValues);
    followSegment();
    if (sensorValues[0] > 200)
      foundLeft = 1;
    if (sensorValues[4] > 200)
      foundRight = 1;
    setMotor(50, 50);
    delay(360);
    if (sensorValues[1] > 200 || sensorValues[2] > 200 || sensorValues[3] > 200)
      foundStraight = 1 ;
    if (sensorValues[1] > 600 && sensorValues[2] > 600 && sensorValues[3] > 600)
      break;
    unsigned char dir = selectTurn(foundLeft, foundStraight, foundRight);
    turn(dir);
    path[pathLength] = dir;
    pathLength ++;
    simplifyPath();
    displayPath();
  }
  while (1) {}
  /*  setMotor(MOTORAVALUE,MOTORBVALUE);
    delay(1800);
    setMotor(-MOTORAVALUE,MOTORBVALUE);
    TESTDELAY;
    setMotor(MOTORAVALUE,MOTORBVALUE);
    delay(1800);
    setMotor(-MOTORAVALUE,MOTORBVALUE);
    TESTDELAY;
    setMotor(MOTORAVALUE,MOTORBVALUE);
    delay(1800);
    setMotor(-MOTORAVALUE,MOTORBVALUE);
    TESTDELAY;
    setMotor(MOTORAVALUE,MOTORBVALUE);
    delay(1800);
    setMotor(-MOTORAVALUE,MOTORBVALUE);*
    TESTDELAY;
    }*/
}
