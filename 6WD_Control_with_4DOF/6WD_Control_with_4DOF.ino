/*
  PS2无线手柄控制6WD搜救平台

  YFROBOT-WST
  WWW.YFROBOT.COM.CN
  Apr/2021
*/
#include <Servo.h>
#include "PS2X_lib.h"

PS2X ps2x;
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

#define PS2_DAT        A0
#define PS2_CMD        A1   //PS2 接线端口，不可修改
#define PS2_SEL        A2
#define PS2_CLK        A3

#define DIRA    4          //左电机驱动端口，不可修改
#define PWMA    5
#define SLPA    2          //模块开关，使用持续高电平

#define DIRB    7          //右电机驱动端口，不可修改
#define PWMB    6
#define SLPB    8          //模块开关，使用持续高电平

int MAXSPEEDA = 255;       //摇杆控制电机最大值，左电机
int MAXSPEEDB = 255;       //摇杆控制电机最大值，右电机

int MidSpeed1 = MAXSPEEDA;
int MidSpeed2 = MAXSPEEDB;

#define SERVOPIN1      9    //Servo1 接线端口
#define SERVOPIN2      10   //Servo2 接线端口
#define SERVOPIN3      11   //Servo3 接线端口
#define SERVOPIN4      12   //Servo4 接线端口

int SERVOGROUNDIA = 150;    //舵机初始开机复位角度
int SERVOMIDIA = 145;       //舵机初始开机复位角度
int SERVOUPIA = 90;         //舵机初始开机复位角度
int SERVOPAWIA = 40;        //手爪初始角度
int SERVOPAWTA1 = 110;      //手爪闭合角度

int SERVOMAX = 180;         //舵机最大角度
int SERVOMIN = 0;           //舵机最小角度

int SERVOPOS1 = SERVOGROUNDIA;
int SERVOPOS2 = SERVOMIDIA;
int SERVOPOS3 = SERVOUPIA;
int SERVOPOS4 = SERVOPAWIA;

#define pressures   false    //Key analog value
#define rumble      true     //Vibration motor
int error = 0;
byte type = 0;
byte vibrate = 0;
void(* resetFunc) (void) = 0; //reset

void setup() {
  Serial.begin(115200);         // Baud rate
  pinMode(DIRA,  OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(SLPA,  OUTPUT);
  pinMode(DIRB,  OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(SLPB,  OUTPUT);

  myservo1.attach(SERVOPIN1);
  myservo2.attach(SERVOPIN2);
  myservo3.attach(SERVOPIN3);
  myservo4.attach(SERVOPIN4);

  myservo1.write(SERVOGROUNDIA);
  myservo2.write(SERVOMIDIA);
  myservo3.write(SERVOUPIA);
  myservo4.write(SERVOPAWIA);
  delay(1000);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  digitalWrite(SLPA, LOW);
  digitalWrite(SLPB, LOW);
}

void loop() {
  if (error == 1) //skip loop if no controller found
    resetFunc();
  ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

  if (ps2x.Button(PSB_START))        //will be TRUE as long as button is pressed
    Serial.println("Start is being held");
  if (ps2x.ButtonPressed(PSB_SELECT)) {   //恢复初始速度
    // reset speed;
    MAXSPEEDA = MidSpeed1;
    MAXSPEEDB = MidSpeed2;
    vibrate = 100;
  }

  if (ps2x.ButtonPressed(PSB_PAD_UP)) {    //速度增加，每次+20
    MAXSPEEDA += 20;
    MAXSPEEDB += 20;
    if (MAXSPEEDA >= 255) {
      MAXSPEEDA = 255;
    }
    if (MAXSPEEDB >= 255) {
      MAXSPEEDB = 255;
    }
  }

  if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {   //速度减少，每次-20
    MAXSPEEDA -= 20;
    MAXSPEEDB -= 20;
    if (MAXSPEEDA <= 0) {
      MAXSPEEDA = 0;
    }
    if (MAXSPEEDB <= 0) {
      MAXSPEEDB = 0;
    }
  }

  if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {   //右轮速度减少，每次-10
    MAXSPEEDB -= 10;
    if (MAXSPEEDB <= 0) {
      MAXSPEEDB = 0;
    }
  }

  if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {    //左轮速度减少，每次-10
    MAXSPEEDA -= 10;
    if (MAXSPEEDA <= 0) {
      MAXSPEEDA = 0;
    }
  }

  /***********************************************************************
     Rocker program
     Through the rocker arm around before and after the change of
     resistance for motor speed control
     Up and Left（0 - 127）
     Down and Right （129 - 255）
  ***********************************************************************/
  if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120 && 0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
    EnSLP();
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, HIGH);
    analogWrite(PWMA, map(ps2x.Analog(PSS_RX), 0, 120, 0, MAXSPEEDB));
    analogWrite(PWMB, map(ps2x.Analog(PSS_LY), 0, 120, MAXSPEEDB, 0));
  } else if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120 && 135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
    EnSLP();
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, HIGH);
    analogWrite(PWMA, map(ps2x.Analog(PSS_LY), 0, 120, MAXSPEEDA, 0));
    analogWrite(PWMB, map(ps2x.Analog(PSS_RX), 135, 255, MAXSPEEDA, 0));
  } else if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120) {
    EnSLP();
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, HIGH);
    analogWrite(PWMA, map(ps2x.Analog(PSS_LY), 0, 120, MAXSPEEDA, 0));
    analogWrite(PWMB, map(ps2x.Analog(PSS_LY), 0, 120, MAXSPEEDB, 0));
//    Serial.println("UP");
  } else if (255 >= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) >= 135) {
    EnSLP();
    digitalWrite(DIRA, HIGH);
    digitalWrite(DIRB, LOW);
    analogWrite(PWMA, map(ps2x.Analog(PSS_LY), 135, 255, 0, MAXSPEEDA));
    analogWrite(PWMB, map(ps2x.Analog(PSS_LY), 135, 255, 0, MAXSPEEDB));
//    Serial.println("DOWN");
  } else if (0 <= ps2x.Analog(PSS_LX) && ps2x.Analog(PSS_LX) <= 120) {
    EnSLP();
    digitalWrite(DIRA, HIGH);
    digitalWrite(DIRB, HIGH);
    analogWrite(PWMA, map(ps2x.Analog(PSS_LX), 0, 120, MAXSPEEDA, 0));
    analogWrite(PWMB, map(ps2x.Analog(PSS_LX), 0, 120, MAXSPEEDB, 0));
//    Serial.println("LEFT");
  } else if (255 >= ps2x.Analog(PSS_LX) && ps2x.Analog(PSS_LX) >= 135) {
    EnSLP();
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, LOW);
    analogWrite(PWMA, map(ps2x.Analog(PSS_LX), 135, 255, 0, MAXSPEEDA));
    analogWrite(PWMB, map(ps2x.Analog(PSS_LX), 135, 255, 0, MAXSPEEDB));
//    Serial.println("RIGHT");
  } else {
    Stop();
  }

  /************************************************************
    手臂按键控制部分
   ***********************************************************/
  if (ps2x.Button(PSB_TRIANGLE)) {    //第一舵机抬起（底部）
    SERVOPOS1 += 1;
    if (SERVOPOS1 > SERVOMAX) {
      SERVOPOS1 = SERVOMAX;
      vibrate = 100;
    }
    myservo1.write(SERVOPOS1);
//    Serial.print("SERVO1:");
//    Serial.println(SERVOPOS1);
  }

  if (ps2x.Button(PSB_CROSS)) {        //第一舵机放下（底部）
    SERVOPOS1 -= 1;
    if (SERVOPOS1 < SERVOMIN) {
      SERVOPOS1 = SERVOMIN;
      vibrate = 100;
    }
    myservo1.write(SERVOPOS1);
//    Serial.print("SERVO1:");
//    Serial.println(SERVOPOS1);
  }

  if (ps2x.Button(PSB_CIRCLE)) {       //第二舵机抬起（倒数第二）
    SERVOPOS2 += 1;
    if (SERVOPOS2 > SERVOMAX) {
      SERVOPOS2 = SERVOMAX;
      vibrate = 100;
    }
    myservo2.write(SERVOPOS2);
//    Serial.print("SERVO2:");
//    Serial.println(SERVOPOS2);
  }

  if (ps2x.Button(PSB_SQUARE)) {       //第二舵机放下（倒数第二）
    SERVOPOS2 -= 1;
    if (SERVOPOS2 < SERVOMIN) {
      SERVOPOS2 = SERVOMIN;
      vibrate = 100;
    }
    myservo2.write(SERVOPOS2);
//    Serial.print("SERVO2:");
//    Serial.println(SERVOPOS2);
  }

  if (ps2x.Button(PSB_R1)) {           //第三舵机抬起（倒数第三）
    SERVOPOS3 += 1;
    if (SERVOPOS3 > 150) {
      SERVOPOS3 = 150;
      vibrate = 100;
    }
    myservo3.write(SERVOPOS3);
//    Serial.print("SERVO3:");
//    Serial.println(SERVOPOS3);
  }

  if (ps2x.Button(PSB_R2)) {           //第三舵机放下（倒数第三）
    SERVOPOS3 -= 1;
    if (SERVOPOS3 < SERVOMIN) {
      SERVOPOS3 = SERVOMIN;
      vibrate = 100;
    }
    myservo3.write(SERVOPOS3);
//    Serial.print("SERVO3:");
//    Serial.println(SERVOPOS3);
  }

  /************************************************************
    手爪角度控制部分，L1手爪夹紧，L2手爪松开，角度有限制，满足最大角度时手柄震动.
   ***********************************************************/
  if (ps2x.ButtonPressed(PSB_L1)) {   //手爪夹紧
    SERVOPOS4 += 4;
    if (SERVOPOS4 > SERVOPAWTA1) {
      SERVOPOS4 = SERVOPAWTA1;
      if (SERVOPOS4 = SERVOPAWTA1) {
        vibrate = 200;
      }
    }
    myservo4.write(SERVOPOS4);
//    Serial.print("SERVO4:");
//    Serial.println(SERVOPOS4);
  }

  if (ps2x.ButtonPressed(PSB_L2)) {  //手爪松开
    SERVOPOS4 -= 4;
    if (SERVOPOS4 < SERVOPAWIA) {
      SERVOPOS4 = SERVOPAWIA;
      if (SERVOPOS4 = SERVOPAWIA) {
        vibrate = 200;
      }
    }
    myservo4.write(SERVOPOS4);
//    Serial.print("SERVO4:");
//    Serial.println(SERVOPOS4);
  }

  if (ps2x.ButtonReleased(PSB_L1) || ps2x.ButtonReleased(PSB_R1) || ps2x.ButtonReleased(PSB_L2) ||
      ps2x.ButtonReleased(PSB_R2) || ps2x.ButtonReleased(PSB_TRIANGLE) || ps2x.ButtonReleased(PSB_SQUARE) ||
      ps2x.ButtonReleased(PSB_CIRCLE) || ps2x.ButtonReleased(PSB_CROSS) || ps2x.ButtonReleased(PSB_SELECT)) {
    vibrate = 0;
  }
  delay(50);
}

void Stop() {
  digitalWrite(SLPA, LOW);
  digitalWrite(SLPB, LOW);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
}
void EnSLP() {
  digitalWrite(SLPA, HIGH);
  digitalWrite(SLPB, HIGH);
}
