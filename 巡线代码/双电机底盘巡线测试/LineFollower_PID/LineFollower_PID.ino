#include <Servo.h>
#include <QTRSensors.h>
#include <MotorDriver.h>
Servo myservo;

#define MOTORTYPE YF_IIC_TB
uint8_t SerialDebug = 0; // Serial print debugging 0-No 1-Yes

#define NUM_SENSORS   5     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2

// sensors 1 through 5 are connected to digital pins 10 through 14, respectively
QTRSensorsRC qtrrc((unsigned char[]) {
  10, 11, 12, A0, A1
}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

#define servo  3            //servo pin
int servoMin = 45;          //turn left max angle
int servoMid = 90;          //Straight angle
int servoMax = 135;         //turn right max angle

const int MAX_SPEED = 1200; //0-4096
const int SPEED = 800;      //0-1200

int lastError = 0;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetm1 = 1;
const int offsetm2 = -1;
const int offsetm3 = 1;
const int offsetm4 = -1;

// Initializing motors.
MotorDriver motorDriver = MotorDriver(MOTORTYPE);

void setup()
{
  Serial.begin(9600);
  motorDriver.begin();
  motorDriver.motorConfig(offsetm1, offsetm2, offsetm3, offsetm4);
  myservo.attach(servo);
  myservo.write(servoMid);
  for (int i = 0; i < 100; i++)  // make the calibration take about 10 seconds
  {
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  delay(500);
}

void loop()
{
  unsigned int position = qtrrc.readLine(sensorValues);
//  for (unsigned char i = 0; i < NUM_SENSORS; i++)
//  {
//    Serial.print(sensorValues[i]);
//    Serial.print('\t');
//  }

  Serial.println(position);

  int error = position - 2000;
  int speedDifference = error / 4 + 0.5 * (error - lastError);
  lastError = error;
  int m1Speed = SPEED + speedDifference;
  int m2Speed = SPEED - speedDifference;

  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;
  if (m1Speed > MAX_SPEED)
    m1Speed = MAX_SPEED;
  if (m2Speed > MAX_SPEED)
    m2Speed = MAX_SPEED;
  myservo.write(map(position, 0, 4000, servoMin, servoMax));
  motorDriver.setMotor(0, 0, m1Speed, m2Speed);
  delay(10);
}
