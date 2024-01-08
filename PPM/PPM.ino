#include <Servo.h>
//The two channels of the receiver are connected to the digital port 2 and 3 of the arduino respectively.
int ppm1 = 2;
int ppm2 = 3;

int PWM1 = 5;     //Motor pin
int DIR1 = 4;     //Motor pin
int PWM2 = 6;
int DIR2 = 7;
int speedValue = 220;
int i = 0;

unsigned long rc1_PulseStartTicks, rc2_PulseStartTicks;
volatile int rc1_val, rc2_val;  // rc2 =ym  rc1=fx

void setup() {
  Serial.begin(9600);
  pinMode(DIR1 , OUTPUT);
  pinMode(PWM1 , OUTPUT);
  pinMode(DIR2 , OUTPUT);
  pinMode(PWM2 , OUTPUT);
  pinMode(ppm1, INPUT); //PPM inputs from RC receiver
  pinMode(ppm2, INPUT);
  attachInterrupt(0, rc1, CHANGE);     // Level change trigger interrupt
  attachInterrupt(1, rc2, CHANGE);
}

void rc1() {
  // did the pin change to high or low?
  if (digitalRead( ppm1 ) == HIGH)
    rc1_PulseStartTicks = micros(); // store the current micros() value
  else
    rc1_val = micros() - rc1_PulseStartTicks;
}
void rc2() {
  // did the pin change to high or low?
  if (digitalRead( ppm2 ) == HIGH)
    rc2_PulseStartTicks = micros();
  else
    rc2_val = micros() - rc2_PulseStartTicks;
}

void loop() {
  //Serial.println(rc1_val);
  if (rc1_val > 1750)
  {
    i = 1;
  }
  else
  {
    i = 0;
  }
  
  if (i == 0) {
    if (rc2_val < 1100)
    {
      analogWrite(PWM1, 0);
      analogWrite(PWM2, 0);

    }
    else
    {
      digitalWrite(DIR1 , HIGH);
      analogWrite(PWM1, map(rc2_val, 1000, 1900, 0, speedValue));
      digitalWrite(DIR2 , LOW);
      analogWrite(PWM2, map(rc2_val, 1000, 1900, 0, speedValue));
    }
  }
  else if(i ==1){
    digitalWrite(DIR1 , LOW);
    analogWrite(PWM1, 150);
    digitalWrite(DIR2 , HIGH);
    analogWrite(PWM2, 150);
  }
}
