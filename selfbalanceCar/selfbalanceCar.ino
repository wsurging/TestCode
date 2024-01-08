// Self balancing Mini robot with 4 motors.//
////////////////////////////////////////////////////////////////////
///////////////////////////////////////
///////Created by///////////////
// Arkadi Rafalovich///////
//Last Update 16.01.2013////
//////////////////////////////////////
// libraries in use:
// for running MPU 6050 6/21/2012 by Jeff Rowberg
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//thatnks for all of the open source libraries
// ================================================================
// === Define Variables ===
// ================================================================
//define motor parameters:
#define MpwmBL 6 //BOTTOM LEFT
#define MpwmBR 9 //BOTTOM RIGHT
#define MpwmTR 10 // TopRight
#define MpwmTL 11 // TopLeft
#define MdirectionR1 4
#define MdirectionR2 7
#define MdirectionL1 8
#define MdirectionL2 12
// Define motors preamp and deadzone.
//A method to remove the dead zone of the motors and keep them silent when not working.
#define Preamp 35
#define DeadZone 15
//Define the angle at which the robot moves between 4 wheel control and 2 wheel self balancing control
#define ModeAngle 50
// adjust motor speed and wheel size 42 mm with 540 rpm 70 mm with 300 rpm: 1.08 for faster motors =>> neglectable
// #define MSSDiff 1.08
// desired motor speed right and left:
int DMSR=0;
int DMSL=0;
// Bluetooth command
char command='S';
// state true for self balance and state false for 4WD car
boolean state;
// control variables to change the direction of movment
// turn right
int RMotorDiff=0;
// turn left
int LMotorDiff=0;
// Speed adjustment variable
byte ComStep=0;
// ================================================================
// === Define PID Controller Variables ===
// ================================================================
// proportional gain
float Kp=7.5;
// deferential gain
float Kd=0.5;
// integral gain
float Ki=12.5;
// variables for PID controller
//// Type A PID controller
//// error=Set angle-actual angle
//float error=0;
//float Lasterror=0;
//float LLasterror=0;
// Type C PID controller
float error=0;
float Aangle=0;
float Lastangle=0;
float LLastangle=0;
// Angle adjustment for Forward Backward control Is through the Desired angle
// Desired angle
float Dangle=0;
// PID controller command
float COcommand =0;
// ================================================================
// === Define MPU 6050 Variables ===
// ================================================================
MPU6050 mpu;
#define LED_PIN 13
bool blinkState = false;
// MPU control/status vars
bool dmpReady = false; // set true if DMP init was successful
uint8_t mpuIntStatus; // holds actual interrupt status byte from MPU
uint8_t devStatus; // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize; // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount; // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
// orientation/motion vars
Quaternion q; // [w, x, y, z] quaternion container
VectorInt16 aa; // [x, y, z] accel sensor measurements
VectorInt16 aaReal; // [x, y, z] gravity-free accel sensor measurements
VectorInt16 aaWorld; // [x, y, z] world-frame accel sensor measurements
VectorFloat gravity; // [x, y, z] gravity vector
VectorInt16 aaOffset; // [x, y, z] accel sensor measurements offset
float euler[3]; // [psi, theta, phi] Euler angle container
float ypr[3]; // [yaw, pitch, roll] yaw/pitch/roll container and gravity vector
float yprOffset[] = {0, 0, 0}; // [yaw, pitch, roll] offset yaw/pitch/roll container and gravity vector
// ================================================================
// === INTERRUPT DETECTION ROUTINE ===
// ================================================================
volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
mpuInterrupt = true;
}
// ================================================================
// === INITIAL SETUP ===
// ================================================================
// the setup routine runs once when you press reset:
void setup() {
// join I2C bus (I2Cdev library doesn't do this automatically)
Wire.begin();
// initialize serial communication
Serial.begin(115200);
///////////////////////////////////
// initialize mpu6050///
///////////////////////////////////
// initialize device
Serial.println(F("Initializing I2C devices..."));
mpu.initialize();
// verify connection
Serial.println(F("Testing device connections..."));
Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
// load and configure the DMP
Serial.println(F("Initializing DMP..."));
devStatus = mpu.dmpInitialize();
// make sure it worked (returns 0 if so)
if (devStatus == 0) {
// turn on the DMP, now that it's ready
Serial.println(F("Enabling DMP..."));
mpu.setDMPEnabled(true);
// enable Arduino interrupt detection
Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
attachInterrupt(0, dmpDataReady, RISING);
mpuIntStatus = mpu.getIntStatus();
// set our DMP Ready flag so the main loop() function knows it's okay to use it
Serial.println(F("DMP ready! Waiting for first interrupt..."));
dmpReady = true;
// get expected DMP packet size for later comparison
packetSize = mpu.dmpGetFIFOPacketSize();
} else {
// ERROR!
// 1 = initial memory load failed
// 2 = DMP configuration updates failed
// (if it's going to break, usually the code will be 1)
Serial.print(F("DMP Initialization failed (code "));
Serial.print(devStatus);
Serial.println(F(")"));
}
// configure LED for output
pinMode(LED_PIN, OUTPUT);
////////////////////////////////////////
// initializemotor pins///
////////////////////////////////////////
// declare direction pins as aoutup
pinMode(MdirectionR1, OUTPUT);
pinMode(MdirectionR2, OUTPUT);
pinMode(MdirectionL1, OUTPUT);
pinMode(MdirectionL2, OUTPUT);
// declare PWM pins as aoutup
pinMode(MpwmBL, OUTPUT);
pinMode(MpwmBR, OUTPUT);
pinMode(MpwmTR, OUTPUT);
pinMode(MpwmTL, OUTPUT);
}
// ================================================================
// === MAIN PROGRAM LOOP ===
// ================================================================
void loop() {
// if MPU 6050 initialization failed exit.
if (!dmpReady) return;
// check if MPU 6050 ready
if (!mpuInterrupt && fifoCount < packetSize) {
// check if there is incoming command
if (Serial.available() > 0) {
// read the incoming byte:
command = Serial.read();
switch (command) {
// Controller commands:
case 'S':
Dangle=0;
RMotorDiff=0;
LMotorDiff=0;
break;
case 'F':
Dangle=ComStep*0.5;
break;
case 'B':
Dangle=-ComStep*0.5;
break;
case 'R':
RMotorDiff=ComStep*10;
break;
case 'L':
LMotorDiff=ComStep*10;
break;
case '1':
ComStep=1;
break;
case '2':
ComStep=2;
break;
case '3':
ComStep=3;
break;
case '4':
ComStep=4;
break;
case '5':
ComStep=5;
break;
case '6':
ComStep=6;
break;
case '7':
ComStep=7;
break;
case '8':
ComStep=8;
break;
case '9':
ComStep=9;
break;
// turned off when not adjusting control due to conflicts with the BluetoothRC android app.
// // PID Adjusting Commands:
// case 'P':
// Kp=Kp+0.5;
// break;
// case 'p':
// Kp=Kp-0.5;
// break;
// case 'I':
// Ki=Ki+0.5;
// break;
// case 'i':
// Ki=Ki-0.5;
// break;
// case 'D':
// Kd=Kd+0.1;
// break;
// case 'd':
// Kd=Kd-0.1;
// break;
//
default:
// nothing
;
}
// end incoming command
}
// ================================================================
// === Control Low ===
// ================================================================
// // PID type A controller
// LLasterror=Lasterror;
// Lasterror=error;
// // error is the Desired angle - actual angle
// error=Dangle-ypr[1]*180/M_PI;
//
// // PID controller at 50 HZ
// COcommand=COcommand+Kp*(error-Lasterror)+(Ki/50*error)+Kd*50*(error-2*Lasterror+LLasterror);
// PID type C controller
LLastangle=Lastangle;
Lastangle=Aangle;
Aangle=ypr[1]*180/M_PI;
// error is the Desired angle - actual angle
error=Dangle-Aangle;
// PID controller at 50 HZ
COcommand=COcommand-Kp*(Aangle-Lastangle)+Ki/50*error-Kd*50*(Aangle-2*Lastangle+LLastangle);
// constrain the Control low command to a valid range
COcommand=constrain(COcommand, -255, 255);
// before updating the motors commad chek the angle of the robot if it is in the desire range:
if (abs(Aangle)<ModeAngle) {
DMSR= (int) COcommand + RMotorDiff;
DMSL= (int) COcommand + LMotorDiff;
// set motor speeds: // state true for self balance and state false for 4WD car
SetMotorSpeed(DMSR, DMSL, true);
} else{
// not in the range so the motoors COcommad will be zeroed in order to delete the memory of the control low
COcommand=0;
// use same variable with adjusted parameters
DMSR= (int) (Dangle*10 + RMotorDiff - LMotorDiff)*3;
DMSL= (int) (Dangle*10 + LMotorDiff- RMotorDiff)*3;
SetMotorSpeed(DMSR, DMSL, false);
}
// end main code
}
// check if MPU 6050 not ready again before sending data
if (!mpuInterrupt && fifoCount < packetSize) {
switch (command) {
case 'a':
//Display Variables
Serial.print("MPU ");
Serial.print("ypr\t");
Serial.print(ypr[0] * 180/M_PI);
Serial.print("\t");
Serial.print(ypr[1] * 180/M_PI);
Serial.print("\t");
Serial.println(ypr[2] * 180/M_PI);
break;
case 'c':
Serial.print("command: ");
Serial. print(command);
Serial.print(" RM Speed: ");
Serial. print(DMSR);
Serial.print(" LM Speed: ");
Serial. println(DMSL);
break;
case 'C':
Serial.print("PID");
Serial.print(" Kp:");
Serial.print(Kp);
Serial.print(" Ki:");
Serial.print(Ki);
Serial.print(" Kd:");
Serial.println(Kd);
break;
default:
// nothing
;
}
// end interface. update
}
// reset interrupt flag and get INT_STATUS byte
mpuInterrupt = false;
mpuIntStatus = mpu.getIntStatus();
// get current FIFO count
fifoCount = mpu.getFIFOCount();
// check for overflow (this should never happen unless our code is too inefficient)
if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
// reset so we can continue cleanly
mpu.resetFIFO();
Serial.println(F("FIFO overflow!"));
// otherwise, check for DMP data ready interrupt (this should happen frequently)
} else if (mpuIntStatus & 0x02) {
// wait for correct available data length, should be a VERY short wait
while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
// read a packet from FIFO
mpu.getFIFOBytes(fifoBuffer, packetSize);
// track FIFO count here in case there is > 1 packet available
// (this lets us immediately read more without waiting for an interrupt)
fifoCount -= packetSize;
// Get Yaw Pitch Roll
mpu.dmpGetQuaternion(&q, fifoBuffer);
mpu.dmpGetGravity(&gravity, &q);
mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
// Set offset
ypr[0] -= yprOffset[0];
ypr[1] -=yprOffset[1];
ypr[2] -=yprOffset[2];
// blink LED to indicate activity
blinkState = !blinkState;
digitalWrite(LED_PIN, blinkState);
}
// end main code
}
// ================================================================
// === SetMotorSpeed function ===
// ================================================================
// set motor speed parameters recieaved right motor desired speed and left motor desired speed.
// the preamp and deadzone is applied in this part of the code.
// ==> leading for the approximate liniarization of the system by removing the motors dead zone.
void SetMotorSpeed(int DMSR, int DMSL, boolean state) {
// handlig each motor independently
// start by setting the motor direction:
if (DMSR>0) {
// set forward
digitalWrite(MdirectionR1,LOW);
digitalWrite(MdirectionR2,HIGH);
}
else{
// set reverse
digitalWrite(MdirectionR1,HIGH);
digitalWrite(MdirectionR2,LOW);
// change baluse to positive
DMSR=-DMSR;
}
if (DMSL>0) {
// set forward
digitalWrite(MdirectionL1,LOW);
digitalWrite(MdirectionL2,HIGH);
}
else {
// set reverse
digitalWrite(MdirectionL1,HIGH);
digitalWrite(MdirectionL2,LOW);
// change valuse to positive
DMSL=-DMSL;
}
// implement dead zone and preamp:
if (DMSR<DeadZone){
// set motor speed to zero
DMSR=0;
}
else{
DMSR=DMSR+Preamp;
}
if (DMSL<DeadZone){
// set motor speed to zero
DMSL=0;
}
else{
DMSL=DMSL+Preamp;
}
// apply constrain for valid input
DMSR=constrain(DMSR, 0, 255);
DMSL=constrain(DMSL, 0, 255);
if (state) {
// set motor speed:
analogWrite(MpwmBR, DMSR);
analogWrite(MpwmTR, 0);
analogWrite(MpwmBL, DMSL);
analogWrite(MpwmTL, 0);
}
else{
analogWrite(MpwmBR, DMSR);
analogWrite(MpwmTR, DMSR);
analogWrite(MpwmBL, DMSL);
analogWrite(MpwmTL, DMSL);
}
// end set motor speed function
}
