#include <QTRSensors.h>

// This example is designed for use with eight QTR-1RC sensors or the eight sensors of a
// QTR-8RC module.  These reflectance sensors should be connected to digital inputs 3 to 10.
// The QTR-8RC's emitter control pin (LEDON) can optionally be connected to digital pin 2, 
// or you can leave it disconnected and change the EMITTER_PIN #define below from 2 to 
// QTR_NO_EMITTER_PIN.

// The setup phase of this example calibrates the sensor for ten seconds and turns on
// the LED built in to the Arduino on pin 13 while calibration is going on.
// During this phase, you should expose each reflectance sensor to the lightest and 
// darkest readings they will encounter.
// For example, if you are making a line follower, you should slide the sensors across the
// line during the calibration phase so that each sensor can get a reading of how dark the
// line is and how light the ground is.  Improper calibration will result in poor readings.
// If you want to skip the calibration phase, you can get the raw sensor readings
// (pulse times from 0 to 2500 us) by calling qtrrc.read(sensorValues) instead of
// qtrrc.readLine(sensorValues).

// The main loop of the example reads the calibrated sensor values and uses them to
// estimate the position of a line.  You can test this by taping a piece of 3/4" black
// electrical tape to a piece of white paper and sliding the sensor across it.  It
// prints the sensor values to the serial monitor as numbers from 0 (maximum reflectance) 
// to 1000 (minimum reflectance) followed by the estimated location of the line as a number
// from 0 to 5000.  1000 means the line is directly under sensor 1, 2000 means directly
// under sensor 2, etc.  0 means the line is directly under sensor 0 or was last seen by
// sensor 0 before being lost.  5000 means the line is directly under sensor 5 or was
// last seen by sensor 5 before being lost.

#define NUM_SENSORS   5     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2

// sensors 0 through 7 are connected to digital pins 3 through 10, respectively
QTRSensorsRC qtrrc((unsigned char[]) {14, 15, 16, 17, 18},
  NUM_SENSORS, TIMEOUT, EMITTER_PIN); 
unsigned int sensorValues[NUM_SENSORS];
int AIN1 = 6;  //PWMA
int AIN2 = 5;  //DIRA
int BIN1 = 10;  //PWMB             
int BIN2 = 9;  //DIRB
int melody[]={330,330,330,262,392,200,280};
int noteDurations[]={8,4,4,8,4,4,2};
void setup(){
  delay(500);
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 80; i++)  // make the calibration take about 10 seconds
  {
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
    if(i<20||i>=60)
       setMotor(75,-82);
    else
       setMotor(-75,82);//            
  }
  digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration
  setMotor(0,0);
  for(int thisNote = 0;thisNote < 7;thisNote ++){
    tone(12,melody[thisNote],1000/noteDurations[thisNote]);
    delay(1300/noteDurations[thisNote]);
    noTone(12);
  }
 // delay(2000);
  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
}
void setMotor(int MOTORA,int MOTORB)
{
  if(MOTORA>=0)
  {
    digitalWrite(AIN2,HIGH);
    analogWrite(AIN1,255-MOTORA);
  }
  else
  {
   digitalWrite(AIN1,HIGH);
    analogWrite(AIN2,MOTORA+255);
  }
  if(MOTORB>0)
  {
    digitalWrite(BIN2,HIGH);
    analogWrite(BIN1,256-MOTORB);
  }
  else
  {
    digitalWrite(BIN1,HIGH);
    analogWrite(BIN2,254+MOTORB); 
  }
}
/*void setM1Speed(int speed){            //set the speed of motorA
  uint8_t reverse = 0;                 //the 'DIR' flag
  if(speed < 0){               
    speed = -speed;
    reverse = 1; 
  }
  if(speed > 0xFF){
    speed = 0xFF;
  }
  if(reverse){
    digitalWrite(AIN1,HIGH);
    analogWrite(AIN2,speed);
  }
  else
  {
    digitalWrite(AIN2,HIGH);
    analogWrite(AIN1,speed);    
  }
}
void setM2Speed(int speed){                 //set the speed of motorB
  uint8_t reverse = 0;                      //the 'DIR' flag
  if(speed < 0){
    speed = -speed;
    reverse = 1; 
  }
  if(speed > 0xFF){
    speed = 0xFF;
  }
  if(reverse){
    digitalWrite(BIN1,HIGH);
    analogWrite(BIN2,speed);
  }
  else
  {
    digitalWrite(BIN2,HIGH);
    analogWrite(BIN1,speed);    
  }
}
void setMotor(int m1Speed,int m2Speed){
  setM1Speed(m1Speed);
  setM2Speed(m2Speed);
}
*/

void loop()
{
    setMotor(200,200);
    delay(1000);
    setMotor(-200,-200);
    delay(1000);
/*
  // read calibrated sensor values and obtain a measure of the line position from 0 to 5000
  // To get raw sensor values, call:
  //  qtrrc.read(sensorValues); instead of unsigned int position = qtrrc.readLine(sensorValues);
  unsigned int last_proportional=0;
  long integral=0;
  unsigned int position =qtrrc.readLine(sensorValues);
  int proportional = ((int)position)-2000;
  int derivative = proportional-last_proportional;
  integral+=proportional;
  //setMotor(50,50);
  last_proportional = proportional;
  //int power_difference = proportional/12 ;// only P adjustment  
  int power_difference = proportional/15+integral/3000 + derivative*3/400;
  const int maxcount = 110;
  if(power_difference > maxcount)
	power_difference = maxcount;
  if(power_difference < -maxcount)
	power_difference = -maxcount;

  if(power_difference < 0)
       setMotor(maxcount+power_difference,maxcount);
  else
       setMotor(maxcount, maxcount-power_difference);
  // print the sensor values as numbers from 0 to 1000, where 0 means maximum reflectance and
  // 1000 means minimum reflectance, followed by the line position*/
/*  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  //Serial.println(); // uncomment this line if you are using raw values
  Serial.println(position); // comment this line out if you are using raw values*/
//  delay(250);*/
}
