/*************************************************************************************
   双电机底盘使用PS2无线手柄PID控制

   单片机：Arduino UNO
   电机驱动：L298N
   电源：11.1v18650电池
   电机型号：RS365 24CPR

   by:YFROBOT
 *************************************************************************************/
#include <PID_v1.h>                //必要的库文件，需加载在arduino目录下
#include <PinChangeInterrupt.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>
#include <PS2X_lib.h>
PS2X ps2x;
Servo myservo;

#define PS2_DAT    11    //PS2引脚，线接转接板，引脚可修改，不可与其它端口冲突
#define PS2_CMD    12
#define PS2_SEL    A0
#define PS2_CLK    A1

#define SERVOPIN   10    // 舵机引脚 D10

#define SERVOMIN   55    // 舵机左转角度
#define SERVOMID   90    // 舵机中间角度
#define SERVOMAX   130   // 舵机右转角度

#define Encoder1A  4     // 电机1A相 D4
#define Encoder2A  5     // 电机2A相 D5

#define MAXSpeed   4096  // 速度最大值
#define MIDSpeed   2000  // 转向速度差值 实际值是4096-2000

volatile double encoderPos1 = 0;
volatile double encoderPos2 = 0;
//PID variables  电机运行的PID参数，可做修改以适应电机
//double Kp1 = 1.07, Ki1 = 3.15, Kd1 = 0.0426;  //Adjust the PID to fit the motor
double Kp1 = 0.6, Ki1 = 5, Kd1 = 0.02;  //Adjust the PID to fit the motor
double Kp2 = 0.6, Ki2 = 5, Kd2 = 0.02;   //Adjust the PID to fit the motor

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

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void(* resetFunc) (void) = 0;      // 复位语句，检测手柄没有匹配则重新配对

void setup()
{
  Serial.begin(115200);
  pinMode(Encoder1A, INPUT_PULLUP);                  // quadrature encoder1 input A
  pinMode(Encoder2A, INPUT_PULLUP);                  // quadrature encoder2 input A
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder1A), Encoder1, FALLING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder2A), Encoder2, FALLING);
  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  myPID1.SetOutputLimits(0, 4095);
  myPID2.SetOutputLimits(0, 4095);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  pwm.begin();
  pwm.setPWMFreq(1600);  // Set to whatever you like, we don't use it in this demo!
  Wire.setClock(400000);
  Wire.begin();
  myservo.attach(SERVOPIN);
  myservo.write(SERVOMID);
  delay(300);
}

void loop() {
  if (error == 1) //skip loop if no controller found
    resetFunc();
  //read controller and set large motor to spin at 'vibrate' speed
  ps2x.read_gamepad(false, vibrate);

  if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120) {
    pwm.setPWM(0, 4096, 0);       // IIC芯片控制电机驱动TB6612，驱动电机方向
    pwm.setPWM(1, 0, 4096);

    pwm.setPWM(3, 0, 4096);       // IIC芯片控制电机驱动TB6612，驱动电机方向
    pwm.setPWM(4, 4096, 0);
    Setpoint1 = map(ps2x.Analog(PSS_LY), 0, 120, MAXSpeed, 0);//将摇杆值转化为我们需要的速度范围
    Setpoint2 = map(ps2x.Analog(PSS_LY), 0, 120, MAXSpeed, 0);
  }
  else if (135 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 255) {
    pwm.setPWM(1, 4096, 0);       // IIC芯片控制电机驱动TB6612，驱动电机方向
    pwm.setPWM(0, 0, 4096);

    pwm.setPWM(4, 0, 4096);       // IIC芯片控制电机驱动TB6612，驱动电机方向
    pwm.setPWM(3, 4096, 0);
    Setpoint1 = map(ps2x.Analog(PSS_LY), 135, 255, 0, MAXSpeed);
    Setpoint2 = map(ps2x.Analog(PSS_LY), 135, 255, 0, MAXSpeed);
  }
  else {
    pwm.setPWM(1, 0, 0);          // IIC芯片控制电机驱动TB6612，驱动电机方向
    pwm.setPWM(0, 0, 0);

    pwm.setPWM(4, 0, 0);          // IIC芯片控制电机驱动TB6612，驱动电机方向
    pwm.setPWM(3, 0, 0);
    Setpoint1 = 0;
    Setpoint2 = 0;
  }

  if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
    ValL = map(ps2x.Analog(PSS_RX), 0, 120, MIDSpeed, 0);
    myservo.write(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
  }
  else if (135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
    ValR = map(ps2x.Analog(PSS_RX), 135, 255, 0, MIDSpeed);
    myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
  }
  else {
    myservo.write(SERVOMID);
    ValL = 0;
    ValR = 0;
  }
  Input1 = map(encoderPos1, 0, 105 , 0, 4096); // 修改数值以适应范围值
  Input2 = map(encoderPos2, 0, 105 , 0, 4096); // 修改数值以适应范围值
  myPID1.Compute();
  myPID2.Compute();
  pwm.setPWM(2, 0, (Output1 - ValL));
  pwm.setPWM(5, 0, (Output2 - ValR));
  encoderPos1 = 0;
  encoderPos2 = 0;
//  Serial.print("Setpoint1:");
//  Serial.print(Setpoint1);
//  Serial.print("  Setpoint2:");
//  Serial.print(Setpoint2);
//  Serial.print("   encoderPos1:");
//  Serial.print(encoderPos1);
//  Serial.print("   encoderPos2:");
//  Serial.print(encoderPos2);
//  Serial.print("   Input1:");
//  Serial.print(Input1);
//  Serial.print("   Input2:");
//  Serial.print(Input2);
//  Serial.print("   Output1:");
//  Serial.print(Output1 - ValL);
//  Serial.print("   Output2:");
//  Serial.println(Output2 - ValR);
  delay(100);
}

void Encoder1() {
  ++encoderPos1;
}

void Encoder2() {
  ++encoderPos2;
}
