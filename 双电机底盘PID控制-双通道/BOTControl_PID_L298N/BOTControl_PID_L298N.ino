/*************************************************************************************
   双电机底盘使用PS2无线手柄PID控制

   单片机：Arduino UNO
   电机驱动：L298N
   电源：11.1v18650电池
   电机型号：RS365 24CPR

   by:YFROBOT
 *************************************************************************************/
#include <PID_v1.h>
#include <PinChangeInterrupt.h>
#include <Servo.h>
#include <PS2X_lib.h>
PS2X ps2x;
Servo myservo;

#define PS2_DAT    A0    //PS2引脚，线接转接板
#define PS2_CMD    A1
#define PS2_SEL    A2
#define PS2_CLK    A3

//L298N Motor Driver variables
#define IN1   4     // 电机1驱动接线端口
#define IN2   5
#define EN1   6

#define IN3   7     // 电机2驱动接线端口
#define IN4   8
#define EN2   9

#define SERVOPIN   10    // 舵机引脚 D10
#define SERVOMIN   55    // 舵机左转角度
#define SERVOMID   90    // 舵机中间角度
#define SERVOMAX   130   // 舵机右转角度

#define Encoder1A  2     // 电机1A相 D2
#define Encoder2A  3     // 电机2A相 D3

#define maxSpeed   255  // 速度最大值

//PID variables
//double Kp1 = 1.07, Ki1 = 3.15, Kd1 = 0.0426;  //Adjust the PID to fit the motor
double Kp1 = 0.6, Ki1 = 5, Kd1 = 0.02;  //Adjust the PID to fit the motor
double Kp2 = 0.6, Ki2 = 5, Kd2 = 0.02;   //Adjust the PID to fit the motor

volatile double encoderPos1 = 0;
volatile double encoderPos2 = 0;

double Setpoint1, Input1, Output1;
double Setpoint2, Input2, Output2;
PID myPID1(&Input1, &Output1, &Setpoint1, Kp1, Ki1, Kd1, DIRECT);
PID myPID2(&Input2, &Output2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);
float last = 0;

int ValL = 0;
int ValR = 0;

#define pressures   true
#define rumble      true

int error = 0;
byte type = 0;
byte vibrate = 0;

void(* resetFunc) (void) = 0;      // 复位语句，检测手柄没有匹配则重新配对

void setup()
{
  Serial.begin(115200);
  myservo.attach(SERVOPIN);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(Encoder1A, INPUT_PULLUP);                  // quadrature encoder1 input A
  pinMode(Encoder2A, INPUT_PULLUP);                  // quadrature encoder2 input A
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder1A), Encoder1, FALLING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder2A), Encoder2, FALLING);
  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  myPID1.SetOutputLimits(0, 255);  // pid函数限制范围
  myPID2.SetOutputLimits(0, 255);

  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  myservo.write(SERVOMID);
  delay(300);
}

void loop() {
  if (error == 1) //skip loop if no controller found
    resetFunc();
  //read controller and set large motor to spin at 'vibrate' speed
  ps2x.read_gamepad(false, vibrate);

  if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    Setpoint1 = map(ps2x.Analog(PSS_LY), 0, 120, maxSpeed, 0);
    Setpoint2 = map(ps2x.Analog(PSS_LY), 0, 120, maxSpeed, 0);
  }
  else if (135 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 255) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    Setpoint1 = map(ps2x.Analog(PSS_LY), 135, 255, 0, maxSpeed);
    Setpoint2 = map(ps2x.Analog(PSS_LY), 135, 255, 0, maxSpeed);
  }
  else {
    Setpoint1 = 0;
    Setpoint2 = 0;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
    ValL = map(ps2x.Analog(PSS_RX), 0, 120, 255, 0);
    myservo.write(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
  }
  else if (135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
    ValR = map(ps2x.Analog(PSS_RX), 135, 255, 0, 255);
    myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
  }
  else {
    myservo.write(SERVOMID);
    ValL = 0;
    ValR = 0;
  }
  //计算100ms内编码器的数值转化为0-4096之内的数值
  Input1 = map(encoderPos1, 0, 135 , 0, 4096);
  Input2 = map(encoderPos2, 0, 135 , 0, 4096);

  myPID1.Compute();
  myPID2.Compute();

  analogWrite(EN1, Output1 - ValL);
  analogWrite(EN2, Output2 - ValR);

  Serial.print("Setpoint1:");
  Serial.print(Setpoint1);
  Serial.print("  encoderPos1:");
  Serial.print(encoderPos1);
  Serial.print("  Input1:");
  Serial.print(Input1);
  Serial.print("  Output1:");
  Serial.print(Output1 - ValL);
  Serial.print("  Setpoint2:");
  Serial.print(Setpoint2);
  Serial.print("  encoderPos2:");
  Serial.print(encoderPos2);
  Serial.print("  Input2:");
  Serial.print(Input2);
  Serial.print("  Output2:");
  Serial.println(Output2 - ValR);
  encoderPos1 = 0;
  encoderPos2 = 0;
  delay(100);
}

void Encoder1() {
  ++encoderPos1;
}

void Encoder2() {
  ++encoderPos2;
}
