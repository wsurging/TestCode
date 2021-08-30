/*************************************************************************************
   PS2 controls the operation of the trolley chassis,
   using a combination of dual motors and steering gears,
   and adding a PID program to the program.

   RS365 ENCODER MOTOR, 24cpr,  A Phase
   Motherboard : Arduino UNO
   Motor drive: IIC motordriver
   Power supply 3s:18650 or Li-Po
   by:YFROBOT
 *************************************************************************************/
#include <PID_v1.h>
#include <PinChangeInterrupt.h>
#include <MotorDriver.h>
#include <Servo.h>
#include <PS2X_lib.h>
PS2X ps2x;
Servo myservo;

#define PS2_DAT    11    //PS2引脚，线接转接板
#define PS2_CMD    12
#define PS2_SEL    A0
#define PS2_CLK    A1

#define SERVOPIN   10    // 舵机引脚 D10
#define SERVOMIN   55    // 舵机左转角度
#define SERVOMID   90    // 舵机中间角度
#define SERVOMAX   130   // 舵机右转角度

#define Encoder1A  7     // 电机1A相  M1:A-D4,B-D2  M2:A-D5,B-D3  M3:A-D7,B-D6  M4:A-D9,B-D8

#define Encoder2A  9     // 电机2A相

#define maxSpeed   1500  // 速度最大值限制   0-4096

volatile double encoderPos1 = 0;
volatile double encoderPos2 = 0;
//PID variables
//double Kp1 = 1.07, Ki1 = 3.15, Kd1 = 0.0426;  //Adjust the PID to fit the motor
double Kp1 = 0.6, Ki1 = 5, Kd1 = 0.02;  //Adjust the PID to fit the motor
double Kp2 = 0.6, Ki2 = 5, Kd2 = 0.02;   //Adjust the PID to fit the motor

double Setpoint1, Input1, Output1;
double Setpoint2, Input2, Output2;
PID myPID1(&Input1, &Output1, &Setpoint1, Kp1, Ki1, Kd1, DIRECT);
PID myPID2(&Input2, &Output2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);
float last = 0;

byte encoder1PinALast;
int duration1;
boolean Direction1;

int ValL;
int ValR;

#define pressures   true
#define rumble      true

int error = 0;
byte type = 0;
byte vibrate = 0;

#define MOTORTYPE YF_IIC_TB   //
uint8_t SerialDebug = 0; // 串口打印调试 0-否 1-是

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetm1 = -1;
const int offsetm2 = 1;
const int offsetm3 = 1;
const int offsetm4 = 1;

// Initializing motors.
MotorDriver motorDriver = MotorDriver(MOTORTYPE);

void(* resetFunc) (void) = 0;      // 复位语句，检测手柄没有匹配则重新配对

void setup()
{
  Serial.begin(115200);
  myservo.attach(SERVOPIN);
  pinMode(Encoder1A, INPUT_PULLUP);                  // quadrature encoder1 input A
  pinMode(Encoder2A, INPUT_PULLUP);                  // quadrature encoder2 input A

  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder1A), Encoder1, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder2A), Encoder2, CHANGE);

  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  myPID1.SetOutputLimits(-4096, 4096);
  myPID2.SetOutputLimits(-4096, 4096);
  motorDriver.begin();
  motorDriver.motorConfig(offsetm1, offsetm2, offsetm3, offsetm4);
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
    if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
      ValL = map(ps2x.Analog(PSS_RX), 0, 120, map(ps2x.Analog(PSS_LY), 0, 120, maxSpeed, 0), 0);
      myservo.write(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
    } else if (135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
      ValR = map(ps2x.Analog(PSS_RX), 135, 255, 0, -map(ps2x.Analog(PSS_LY), 0, 120, -maxSpeed, 0));
      myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
    } else {
      myservo.write(SERVOMID);
      ValL = 0;
      ValR = 0;
    }
    Setpoint1 = map(ps2x.Analog(PSS_LY), 0, 120, maxSpeed, 0) - ValL;
    Setpoint2 = map(ps2x.Analog(PSS_LY), 0, 120, -maxSpeed, 0) + ValR;
    Input1 = encoderPos1 * 80;
    Input2 = encoderPos2 * 80;
    myPID1.Compute();
    myPID2.Compute();
    moveMotor(0, 0, Output1, Output2); // 电机M1,M2,M3,M4 运动
  }
  else if (135 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 255) {
    if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
      ValL = map(ps2x.Analog(PSS_RX), 0, 120, map(ps2x.Analog(PSS_LY), 135, 255, 0, -maxSpeed) , 0);
      myservo.write(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
    } else if (135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
      ValR = map(ps2x.Analog(PSS_RX), 135, 255, 0, -map(ps2x.Analog(PSS_LY), 135, 255, 0, maxSpeed) );
      myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
    }
    else {
      myservo.write(SERVOMID);
      ValL = 0;
      ValR = 0;
    }
    Setpoint1 = map(ps2x.Analog(PSS_LY), 135, 255, 0, -maxSpeed) - ValL;
    Setpoint2 = map(ps2x.Analog(PSS_LY), 135, 255, 0, maxSpeed) + ValR;
    Input1 = encoderPos1 * 80;
    Input2 = encoderPos2 * 80;
    myPID1.Compute();
    myPID2.Compute();
    moveMotor(0, 0, Output1, Output2); // 电机M1,M2,M3,M4 运动
  }
  else {
    Setpoint1 = 0;
    Setpoint2 = 0;
    motorDriver.stopMotor( MAll);
  }
  if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
    myservo.write(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
  }
  else if (135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
    myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
  }
  else {
    myservo.write(SERVOMID);
  }
  encoderPos1 = 0;
  encoderPos2 = 0;






  //  if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //drive motor if either is TRUE
  //    if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120) {
  //      Setpoint1 = map(ps2x.Analog(PSS_LY), 0, 120, maxSpeed, 0);
  //      Setpoint2 = map(ps2x.Analog(PSS_LY), 0, 120, -maxSpeed, 0);
  //      Input1 = encoderPos1 * 80;
  //      Input2 = encoderPos2 * 80;
  //      myPID1.Compute();
  //      myPID2.Compute();
  //      moveMotor(0, 0, Output1 - ValL, Output2 + ValR); // 电机M1,M2,M3,M4 运动
  //    }
  //    else if (135 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 255) {
  //      Setpoint1 = map(ps2x.Analog(PSS_LY), 135, 255, 0, -maxSpeed);
  //      Setpoint2 = map(ps2x.Analog(PSS_LY), 135, 255, 0, maxSpeed);
  //      Input1 = encoderPos1 * 80;
  //      Input2 = encoderPos2 * 80;
  //      myPID1.Compute();
  //      myPID2.Compute();
  //      moveMotor(0, 0, Output1 - ValL, Output2 + ValR); // 电机M1,M2,M3,M4 运动
  //    }  else {
  //      Setpoint1 = 0;
  //      Setpoint2 = 0;
  //      motorDriver.stopMotor( MAll);
  //    }
  //
  //
  //
  //    if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120) {
  //      ValL = map(ps2x.Analog(PSS_RX), 0, 120, Setpoint1 / 2, 0);
  //      myservo.write(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
  //    }
  //    else if (135 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 255) {
  //      ValR = map(ps2x.Analog(PSS_RX), 135, 255, 0, -Setpoint2 / 2);
  //      myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
  //    }
  //    else {
  //      myservo.write(SERVOMID);
  //      ValL = 0;
  //      ValR = 0;
  //    }
  //计算100ms内编码器的数值转化为0-4096之内的数值
  //    Input1 = map(encoderPos1, -51, 51 , -4096, 4096);
  //    Input2 = map(encoderPos2, -51, 51 , -4096, 4096);
  //    Input1 = encoderPos1 * 80;
  //    Input2 = encoderPos2 * 80;
  //    myPID1.Compute();
  //    myPID2.Compute();
  //    moveMotor(0, 0, Output1 - ValL, Output2 + ValR); // 电机M1,M2,M3,M4 运动

  //    Serial.print("Setpoint1:");
  //    Serial.print(Setpoint1);
  //    Serial.print("  encoderPos1:");
  //    Serial.print(encoderPos1);
  //    Serial.print("  Input1:");
  //    Serial.print(Input1);
  //    Serial.print("  Output1:");
  //    Serial.print(Output1 - ValL);
  //    Serial.print("  Setpoint2:");
  //    Serial.print(Setpoint2);
  //    Serial.print("  Input2:");
  //    Serial.print(Input2);
  //    Serial.print("  Output2:");
  //    Serial.println(Output2 + ValR);
  //    encoderPos1 = 0;
  //    encoderPos2 = 0;
  //  }
  //  if (120 < ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) < 135) {
  //    motorDriver.stopMotor(MAll);
  //  }
  //  if (ps2x.ButtonReleased(PSB_L1) || ps2x.ButtonReleased(PSB_R1)) {   //will be TRUE if button was JUST released
  //    motorDriver.stopMotor(MAll);
  //    myservo.write(SERVOMID);
  //    Serial.println("stop");
  //  }
  delay(10);
}

void moveMotor(int16_t speed1, int16_t speed2, int16_t speed3, int16_t speed4) {
  if (speed1 > 0)
    speed1 = min(speed1, maxSpeed);
  else
    speed1 = max(speed1, -maxSpeed);
  if (speed2 > 0)
    speed2 = min(speed2, maxSpeed);
  else
    speed2 = max(speed2, -maxSpeed);
  if (speed3 > 0)
    speed3 = min(speed3, maxSpeed);
  else
    speed3 = max(speed3, -maxSpeed);
  if (speed4 > 0)
    speed4 = min(speed4, maxSpeed);
  else
    speed4 = max(speed4, -maxSpeed);
  motorDriver.setMotor(speed1, speed2, speed3, speed4);
}

void Encoder1() {
  if (Setpoint1 > 0) {
    encoderPos1++;
  } else {
    encoderPos1--;
  }
}

void Encoder2() {
  if (Setpoint2 > 0) {
    encoderPos2++;
  } else {
    encoderPos2--;
  }
}
