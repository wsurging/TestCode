/*
  PS2 control - PS2手柄控制舵机转向小车（双电机）

  YFROBOT-WST
  www.yfrobot.com.cn
  Apr/2021
*/
#include <PS2X_lib.h>  //for v1.8
#include <MotorDriver.h>
#include <Servo.h>
Servo myservo;
PS2X ps2x; // create PS2 Controller Class
/******************************************************************
   set pins connected to PS2 controller:
     - 1e column: original
     - 2e colmun: Stef?
   replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        10
#define PS2_CMD        11   //PS2 Pin
#define PS2_CS         12
#define PS2_CLK        14

#define SERVOPIN 3   //servo pin

#define SERVOMIN 55
#define SERVOMID 93        //the angle of servo
#define SERVOMAX 135
#define TURNLEFT myservo.write(SERVOMIN)  //turn left
#define TURNMID myservo.write(SERVOMID)   //turn middle
#define TURNRIGHT myservo.write(SERVOMAX)  //turn right

#define MAXSPEED 4096          //程序速度限制，不可调
#define MINSPEED 1000

int maxSpeed = 3000;           //速度调节

/******************************************************************
   select modes of PS2 controller:
     - pressures = analog reading of push-butttons
     - rumble    = motor rumbling
   uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
#define rumble      true
//#define rumble      false

int error = 0;
byte type = 0;
byte vibrate = 0;

#define MOTORTYPE YF_IIC_TB   //
uint8_t SerialDebug = 0; // 串口打印调试 0-否 1-是
#define DEBUGESERIAL   //取消注释实现串口打印

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetm1 = 1;
const int offsetm2 = -1;
const int offsetm3 = 1;
const int offsetm4 = -1;

// Initializing motors.
MotorDriver motorDriver = MotorDriver(MOTORTYPE);

int speed_y = 0;
int speed_x = 0;

// Reset func
void (* resetFunc) (void) = 0;

void setup() {
  Serial.begin(115200);
  motorDriver.begin();
  motorDriver.motorConfig(offsetm1, offsetm2, offsetm3, offsetm4);
  myservo.attach(SERVOPIN);
  TURNMID;
  delay(1000);
  //right now, the library does NOT support hot pluggable controllers, meaning
  //you must always either restart your Arduino after you connect the controller,
  //or call config_gamepad(pins) again after connecting the controller.
  //added delay to give wireless ps2 module some time to startup.
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_CS, PS2_DAT, pressures, rumble);
  if (error == 0) {
    Serial.println("Found Controller, configured successful.");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true");
    else
      Serial.println("false");
  } else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type found ");
      break;
    case 1:
      Serial.println("DualShock Controller found ");
      break;
    case 2:
      Serial.println("GuitarHero Controller found ");
      break;
    case 3:
      Serial.println("Wireless Sony DualShock Controller found ");
      break;
  }
}

void loop() {
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
  */
  if (error == 1) { //skip loop if no controller found
    Serial.println("ERROR!!");
    delay(10);
    resetFunc();
  }

  ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

  if (ps2x.Button(PSB_START))        //will be TRUE as long as button is pressed
    Serial.println("Start is being held");
  if (ps2x.Button(PSB_SELECT))
    Serial.println("Select is being held");

  if (ps2x.ButtonPressed(PSB_PAD_UP)) {     //will be TRUE as long as button is pressed
  }
  if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
  }
  if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
  }
  if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
  }
  if (ps2x.ButtonPressed(PSB_SQUARE)) {            //will be TRUE if button was JUST pressed
  }
  if (ps2x.ButtonPressed(PSB_CIRCLE)) {         //will be TRUE if button was JUST pressed
  }
  // 释放 上下左右 按钮
  if (ps2x.ButtonReleased(PSB_PAD_UP) || ps2x.ButtonReleased(PSB_PAD_RIGHT) ||
      ps2x.ButtonReleased(PSB_PAD_LEFT) || ps2x.ButtonReleased(PSB_PAD_DOWN) ||
      ps2x.ButtonReleased(PSB_SQUARE) || ps2x.ButtonReleased(PSB_CIRCLE)) {   //will be TRUE if button was JUST released
    //    motorDriver.stopMotor(MAll);
  }

  if (ps2x.Analog(PSS_LY) < 120 && ps2x.Analog(PSS_LY) >= 0 && ps2x.Analog(PSS_RX) < 120 && ps2x.Analog(PSS_RX) >= 0) {
    speed_y = map(ps2x.Analog(PSS_LY), 120, 0, 0, maxSpeed);
    myservo.write(map(ps2x.Analog(PSS_RX), 120, 0, SERVOMID, SERVOMIN));
    speed_x = map(ps2x.Analog(PSS_RX), 0, 120, speed_y, 0);
    motorDriver.setMotor(0, 0, speed_y - speed_x, speed_y);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(speed_y - speed_x);
    Serial.print("   M4:");
    Serial.print(speed_y);
    Serial.print("   servo:");
    Serial.println(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
#endif
  }
  else if (ps2x.Analog(PSS_LY) < 120 && ps2x.Analog(PSS_LY) >= 0 && ps2x.Analog(PSS_RX) > 135 && ps2x.Analog(PSS_RX) <= 255) {
    speed_y = map(ps2x.Analog(PSS_LY), 120, 0, 0, maxSpeed);
    speed_x = map(ps2x.Analog(PSS_RX), 135, 255, 0, speed_y);
    myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
    motorDriver.setMotor(0, 0, speed_y, speed_y - speed_x);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(speed_y);
    Serial.print("   M4:");
    Serial.print(speed_y - speed_x);
    Serial.print("   servo:");
    Serial.println(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
#endif
  }
  else if (ps2x.Analog(PSS_LY) > 135 && ps2x.Analog(PSS_LY) <= 255 && ps2x.Analog(PSS_RX) < 120 && ps2x.Analog(PSS_RX) >= 0) {
    speed_y = map(ps2x.Analog(PSS_LY), 135, 255, 0, -maxSpeed);
    myservo.write(map(ps2x.Analog(PSS_RX), 120, 0, SERVOMID, SERVOMIN));
    speed_x = map(ps2x.Analog(PSS_RX), 0, 120, speed_y, 0);
    motorDriver.setMotor(0, 0, speed_y - speed_x, speed_y);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(speed_y - speed_x);
    Serial.print("   M4:");
    Serial.print(speed_y);
    Serial.print("   servo:");
    Serial.println(map(ps2x.Analog(PSS_RX), 0, 120, SERVOMIN, SERVOMID));
#endif
  }
  else if (ps2x.Analog(PSS_LY) > 135 && ps2x.Analog(PSS_LY) <= 255 && ps2x.Analog(PSS_RX) > 135 && ps2x.Analog(PSS_RX) <= 255) {
    speed_y = map(ps2x.Analog(PSS_LY), 135, 255, 0, -maxSpeed);
    speed_x = map(ps2x.Analog(PSS_RX), 135, 255, 0, speed_y);
    myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
    motorDriver.setMotor(0, 0, speed_y, speed_y - speed_x);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(speed_y);
    Serial.print("   M4:");
    Serial.print(speed_y - speed_x);
    Serial.print("   servo:");
    Serial.println(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
#endif
  }
  else if (ps2x.Analog(PSS_LY) > 135 && ps2x.Analog(PSS_LY) <= 255) {
    speed_y = map(ps2x.Analog(PSS_LY), 135, 255, 0, -maxSpeed);
    motorDriver.setMotor(0, 0, speed_y, speed_y);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(speed_y);
    Serial.print("   M4:");
    Serial.println(speed_y);
#endif
  }
  else if (ps2x.Analog(PSS_LY) < 120 && ps2x.Analog(PSS_LY) >= 0) {
    speed_y = map(ps2x.Analog(PSS_LY), 120, 0, 0, maxSpeed);
    motorDriver.setMotor(0, 0, speed_y, speed_y);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(speed_y);
    Serial.print("   M4:");
    Serial.println(speed_y);
#endif
  }
  else if (ps2x.Analog(PSS_RX) < 120 && ps2x.Analog(PSS_RX) >= 0) {
    myservo.write(map(ps2x.Analog(PSS_RX), 120, 0, SERVOMID, SERVOMIN));
#ifdef DEBUGESERIAL
    Serial.print("   servo:");
    Serial.println(map(ps2x.Analog(PSS_RX), 120, 0, SERVOMID, SERVOMIN));
#endif
  }
  else if (ps2x.Analog(PSS_RX) > 135 && ps2x.Analog(PSS_RX) <= 255) {
    myservo.write(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
#ifdef DEBUGESERIAL
    Serial.print("   servo:");
    Serial.println(map(ps2x.Analog(PSS_RX), 135, 255, SERVOMID, SERVOMAX));
#endif
  }
  else {
    speed_x = 0;
    speed_y = 0;
    TURNMID;
    motorDriver.stopMotor(MAll);
#ifdef DEBUGESERIAL
    Serial.print("M3:");
    Serial.print(0);
    Serial.print("   M4:");
    Serial.print(0);
    Serial.print("   servo:");
    Serial.println(SERVOMID);
#endif
  }

  if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
    if (ps2x.Button(PSB_L2))
      Serial.println("L2 pressed");
  }
  // 速度加500
  if (ps2x.ButtonPressed(PSB_L1)) {
    maxSpeed += 500;
    maxSpeed = maxSpeed > MAXSPEED ? MAXSPEED : maxSpeed ;
    vibrate = 100;
    Serial.print("maxSpeed:");
    Serial.println(maxSpeed);
  }
  // 速度减500
  if (ps2x.ButtonPressed(PSB_L2)) {
    maxSpeed -= 500;
    maxSpeed = maxSpeed < MINSPEED ? MINSPEED : maxSpeed ;
    vibrate = 100;
    Serial.print("maxSpeed:");
    Serial.println(maxSpeed);
  }

  if (ps2x.ButtonReleased(PSB_L1) || ps2x.ButtonReleased(PSB_L2) ||
      ps2x.ButtonReleased(PSB_SQUARE) || ps2x.ButtonReleased(PSB_CIRCLE)) {
    vibrate = 0;
    //    Serial.println("vibrate = 0");
  }

  delay(20);
}
