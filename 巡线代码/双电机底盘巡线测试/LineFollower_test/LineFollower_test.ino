#include <Servo.h>
#include <QTRSensors.h>
#include <MotorDriver.h>
Servo myservo;

#define NUM_SENSORS   5     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2

#define servo  3            //servo pin
int servoMin = 45;          //turn left max angle
int servoMid = 90;          //Straight angle
int servoMax = 135;         //turn right max angle

// sensors 1 through 5 are connected to digital pins 10 through 14, respectively
QTRSensorsRC qtrrc((unsigned char[]) {
  10, 11, 12, A0, A1
}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

#define MOTORTYPE YF_IIC_TB
uint8_t SerialDebug = 0; // Serial print debugging 0-No 1-Yes

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetm1 = 1;
const int offsetm2 = -1;
const int offsetm3 = 1;
const int offsetm4 = -1;

// Initializing motors.
MotorDriver motorDriver = MotorDriver(MOTORTYPE);
int motorL;
int motorR;

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
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();
  Serial.print(position);

  if (position >= 0 && position <= 4000) {
    myservo.write(map(position, 0, 4000, servoMin, servoMax));
    motorL = map(position, 0, 4000, 100, 1300);
    motorR = map(position, 0, 4000, 1300, 100);
  }
  motorDriver.setMotor(0, 0, motorL, motorR);
  delay(10);
}
