#include <Servo.h>
#include <QTRSensors.h>
#include <MotorDriver.h>
#include <PID_v1.h>
#include <PinChangeInterrupt.h>

Servo myservo;

#define NUM_SENSORS   5     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2

#define servo  3          //servo pin
int servoMin = 50;       //turn left max angle
int servoMid = 90;       //Straight angle
int servoMax = 130;      //turn right max angle

#define Encoder2A  7     // 电机1A相 D4
#define Encoder1A  9     // 电机2A相 D2 

volatile double encoderPos1 = 0;
volatile double encoderPos2 = 0;
//PID variables

double Kp1 = 0.6, Ki1 = 4.5, Kd1 = 0.015;  //Adjust the PID to fit the motor
double Kp2 = 0.6, Ki2 = 4.5, Kd2 = 0.015;  //Adjust the PID to fit the motor


double Setpoint1, Input1, Output1;
double Setpoint2, Input2, Output2;

PID myPID1(&Input1, &Output1, &Setpoint1, Kp1, Ki1, Kd1, DIRECT);
PID myPID2(&Input2, &Output2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);

float last = 0;

// sensors 1 through 5 are connected to digital pins 10 through 15, respectively
QTRSensorsRC qtrrc((unsigned char[]) {
  10, 11, 12, A0, A1
}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
#define MOTORTYPE YF_IIC_TB
uint8_t SerialDebug = 0; // 串口打印调试 0-否 1-是

/* ************************************************************************************
    these constants are used to allow you to make your motor configuration
  line up with function names like forward.  Value can be 1 or -1
 **************************************************************************************/
const int offsetm1 = 1;
const int offsetm2 = -1;
const int offsetm3 = 1;
const int offsetm4 = -1;

// Initializing motors.
MotorDriver motorDriver = MotorDriver(MOTORTYPE);

void setup()
{
  Serial.begin(9600);
  pinMode(Encoder1A, INPUT_PULLUP);                  // quadrature encoder1 input A
  pinMode(Encoder2A, INPUT_PULLUP);                  // quadrature encoder2 input A
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder1A), Encoder1, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(Encoder2A), Encoder2, RISING);
  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  myPID1.SetOutputLimits(0, 1500);
  myPID2.SetOutputLimits(0, 1500);
  motorDriver.begin();
  motorDriver.motorConfig(offsetm1, offsetm2, offsetm3, offsetm4);
  myservo.attach(servo);
  myservo.write(servoMid);

//  for (int i = 0; i < 100; i++) { // make the calibration take about 10 seconds
//    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
//  }
//  delay(500);
}

void loop()
{
  unsigned int position = qtrrc.readLine(sensorValues);
//  for (unsigned char i = 0; i < NUM_SENSORS; i++)
//  {
//    //    Serial.print(sensorValues[i]);
//    //    Serial.print('\t');
//  }
//  Serial.print(position);

  if (position >= 0 && position <= 4000) {
    Setpoint1 = map(position, 0, 4000, 500, 1200);
    Setpoint2 = map(position, 0, 4000, 1200, 500);
    myservo.write(map(position, 0, 4000, servoMin, servoMax));
  }

  if ( millis() - last > 100) {
    double rpm1 = count_rpm1();
    Input1 = map(rpm1, 0, 50 , 0, 4096);
    double rpm2 = count_rpm2();
    Input2 = map(rpm2, 0, 50 , 0, 4096);
  }
  //  myservo.write(map(position, 0, 4000, servoMin, servoMax));
  myPID1.Compute();
  myPID2.Compute();

  Serial.print("Setpoint1:");
  Serial.print(Setpoint1);
  Serial.print("   Input1:");
  Serial.print(Input1);
  Serial.print("   Output1:");
  Serial.print(Output1);
  Serial.print("  Setpoint2:");
  Serial.print(Setpoint2);
  Serial.print("   Input2:");
  Serial.print(Input2);
  Serial.print("   Output2:");
  Serial.println(Output2);

  moveMotor(0, 0, Output1, Output2); // 电机M1,M2,M3,M4 运动
  delay(10);
}

void moveMotor(int16_t speed1, int16_t speed2, int16_t speed3, int16_t speed4) {
  if (speed1 > 0)
    speed1 = min(speed1, 4096);
  else
    speed1 = max(speed1, -4096);
  if (speed2 > 0)
    speed2 = min(speed2, 4096);
  else
    speed2 = max(speed2, -4096);
  if (speed3 > 0)
    speed3 = min(speed3, 4096);
  else
    speed3 = max(speed3, -4096);
  if (speed4 > 0)
    speed4 = min(speed4, 4096);
  else
    speed4 = max(speed4, -4096);
  motorDriver.setMotor(speed1, speed2, speed3, speed4);
}

void Encoder1() {
  ++encoderPos1;
}

void Encoder2() {
  ++encoderPos2;
}

double count_rpm1() {
  double rpm1 = encoderPos1 / 24 * 1000 / (millis() - last); // 修改数值以适应范围值 24电机CPR
  encoderPos1 = 0;
  last = millis();
  return rpm1;
}

double count_rpm2() {
  double rpm2 = encoderPos2 / 24 * 1000 / (millis() - last); // 修改数值以适应范围值 24电机CPR
  encoderPos2 = 0;
  last = millis();
  return rpm2;
}
