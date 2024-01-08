/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  GPIO test - this will set a pin high/low

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These drivers use I2C to communicate, 2 pins are required to
  interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//#include "PinChangeInterrupt.h"
//#include <Servo.h>

//Servo myservo1;
//Servo myservo2;
//Servo myservo3;
//Servo myservo4;
//Servo myservo5;
//Servo myservo6;

//#define ADDRESS1  0xD0 >> 1   // default address of SR09 is 0xE8
//#define ADDRESS2  0xD2 >> 1   // default address of SR09 is 0xE8
//#define ADDRESS3  0xD4 >> 1   // default address of SR09 is 0xE8
//#define ADDRESS4  0xE8 >> 1   // default address of SR09 is 0xE8

//#define CMDREG   0x02    //the command register is 0x02
//#define CMDRD    0xBC    //read the distance of SR09,less 600cm.

//#define ENCODEROUTPUTA 448 // 每次旋转插入电机编码器输出脉冲(cpr)
//#define ENCODEROUTPUTB 448 // 每次旋转插入电机编码器输出脉冲(cpr)
//#define ENCODEROUTPUTC 448 // 每次旋转插入电机编码器输出脉冲(cpr)
//#define ENCODEROUTPUTD 448 // 每次旋转插入电机编码器输出脉冲(cpr)


//#define encoderA 2       //external interrupt
//#define encoderB 4       //external interrupt
//#define encoderC 6       //external interrupt
//#define encoderD 8       //external interrupt
//int rpmA = 0;
//int rpmB = 0;
//int rpmC = 0;
//int rpmD = 0;
//int intervalA = 1000;
//long previousMillisA = 0;
//long currentMillisA = 0;
//int intervalB = 1000;
//long previousMillisB = 0;
//long currentMillisB = 0;
//int intervalC = 1000;
//long previousMillisC = 0;
//long currentMillisC = 0;
//int intervalD = 1000;
//long previousMillisD = 0;
//long currentMillisD = 0;
//volatile long encoderValueA = 0;
//volatile long encoderValueB = 0;
//volatile long encoderValueC = 0;
//volatile long encoderValueD = 0;

//int pos = 0;
//int speed = 0;
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

void setup() {
  Serial.begin(115200);
//  Serial.println("GPIO test!");
//  myservo1.attach(10);
//  myservo2.attach(11);
//  myservo3.attach(12);
//  myservo4.attach(A0);
//  myservo5.attach(A1);
//  myservo6.attach(A2);
  pwm.begin();
  pwm.setPWMFreq(1600);  // Set to whatever you like, we don't use it in this demo!
  Wire.setClock(400000);
  Wire.begin();                             // join the TWI as the master
//  SR09_send_command(ADDRESS1, CMDREG, 0X70); // powered by USB
//  SR09_send_command(ADDRESS2, CMDREG, 0X70); // powered by USB
//  SR09_send_command(ADDRESS3, CMDREG, 0X70); // powered by USB
//  SR09_send_command(ADDRESS4, CMDREG, 0X70); // powered by USB
  delay(2000);                              // wait 2s
  Serial.println("Start...");

  //  pinMode(encoderA, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  //  pinMode(encoderB, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  //  pinMode(encoderC, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉
  //  pinMode(encoderD, INPUT_PULLUP); // 将霍尔传感器A设置为输入上拉


  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!

  //  attachInterrupt(digitalPinToInterrupt(encoderA), updateEncoderA, CHANGE);
  //  attachPCINT(digitalPinToPCINT(encoderA), updateEncoderA, RISING);
  //  attachPCINT(digitalPinToPCINT(encoderB), updateEncoderB, CHANGE);
  //  attachPCINT(digitalPinToPCINT(encoderC), updateEncoderC, RISING);
  //  attachPCINT(digitalPinToPCINT(encoderD), updateEncoderD, RISING);
}

void loop() {
    pwm.setPWM(0, 0, 4096);       // turns pin fully on
    pwm.setPWM(1, 4096, 0);
    pwm.setPWM(2, 0, 4000);
  
    pwm.setPWM(3, 0, 4096);       // turns pin fully on
    pwm.setPWM(4, 4096, 0);
    pwm.setPWM(5, 0, 4000);
  
    pwm.setPWM(8, 0, 4096);       // turns pin fully on
    pwm.setPWM(9, 4096, 0);
    pwm.setPWM(10, 0, 4000);
//  for (speed = 0;speed <=4095;speed +=20){
//    pwm.setPWM(11, 0, 4096);       // turns pin fully on
//    pwm.setPWM(12, 4096, 0);
//    pwm.setPWM(13, 0, speed);
//    delay(15);
//  }
//  for (speed = 4095;speed >=0;speed -=20){
//    pwm.setPWM(11, 0, 4096);       // turns pin fully on
//    pwm.setPWM(12, 4096, 0);
//    pwm.setPWM(13, 0, speed);
//    delay(15);
//  }
//    for (pos = 0; pos <= 180; pos += 2) { // goes from 0 degrees to 180 degrees
//      // in steps of 1 degree
//      myservo1.write(pos);              // tell servo to go to position in variable 'pos'
//      myservo2.write(pos);
//      myservo3.write(pos);
//      myservo4.write(pos);
//      myservo5.write(pos);
//      myservo6.write(pos);
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
//    for (pos = 180; pos >= 0; pos -= 2) { // goes from 180 degrees to 0 degrees
//      myservo1.write(pos);              // tell servo to go to position in variable 'pos'
//      myservo2.write(pos);
//      myservo3.write(pos);
//      myservo4.write(pos);
//      myservo5.write(pos);
//      myservo6.write(pos);
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }

//  word xx =  SR09_read_data(ADDRESS1, CMDREG, CMDRD); //read the distance
////  Serial.print(xx);               //print the distance by serial
////  Serial.print("mm  ");
//  word yy =  SR09_read_data(ADDRESS2, CMDREG, CMDRD); //read the distance
////  Serial.print(yy);               //print the distance by serial
////  Serial.print("mm  ");
//  word zz =  SR09_read_data(ADDRESS3, CMDREG, CMDRD); //read the distance
////  Serial.print(zz);               //print the distance by serial
////  Serial.print("mm  ");
//  word qq =  SR09_read_data(ADDRESS4, CMDREG, CMDRD); //read the distance
////  Serial.print(qq);               //print the distance by serial
////  Serial.println("mm");
////  delay(10);

  //  //每秒更新RPM值
  //  currentMillisA = millis();
  //  if (currentMillisA - previousMillisA > intervalA) {
  //    previousMillisA = currentMillisA;
  //    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
  //    rpmA = (float)(encoderValueA * 60 / ENCODEROUTPUTA / 18.8);
  //    Serial.print(encoderValueA);
  //    Serial.print(" pulseA / ");
  //    Serial.print(ENCODEROUTPUTA);
  //    Serial.print(" pulse per rotation x 60 seconds = ");
  //    Serial.print(rpmA);
  //    Serial.println(" RPM");  //电机转速
  //    encoderValueA = 0;
  //  }
  //
  //
  //  currentMillisB = millis();
  //  if (currentMillisB - previousMillisB > intervalB) {
  //    previousMillisB = currentMillisB;
  //    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
  //    rpmB = (float)(encoderValueB * 60 / ENCODEROUTPUTB / 18.8);
  //    Serial.print(encoderValueB);
  //    Serial.print(" pulseB / ");
  //    Serial.print(ENCODEROUTPUTB);
  //    Serial.print(" pulse per rotation x 60 seconds = ");
  //    Serial.print(rpmB);
  //    Serial.println(" RPM");  //电机转速
  //    encoderValueB = 0;
  //  }
  //
  //  currentMillisC = millis();
  //  if (currentMillisC - previousMillisC > intervalC) {
  //    previousMillisC = currentMillisC;
  //    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
  //    rpmC = (float)(encoderValueC * 60 / ENCODEROUTPUTC / 18.8);
  //    Serial.print(encoderValueC);
  //    Serial.print(" pulseC / ");
  //    Serial.print(ENCODEROUTPUTC);
  //    Serial.print(" pulse per rotation x 60 seconds = ");
  //    Serial.print(rpmC);
  //    Serial.println(" RPM");  //电机转速
  //    encoderValueC = 0;
  //  }
  //
  //  currentMillisD = millis();
  //  if (currentMillisD - previousMillisD > intervalD) {
  //    previousMillisD = currentMillisD;
  //    // 每分钟转数（RPM）= （编码器总脉冲数/电机编码器输出）x 60s
  //    rpmD = (float)(encoderValueD * 60 / ENCODEROUTPUTD / 18.8);
  //    Serial.print(encoderValueD);
  //    Serial.print(" pulseD / ");
  //    Serial.print(ENCODEROUTPUTD);
  //    Serial.print(" pulse per rotation x 60 seconds = ");
  //    Serial.print(rpmD);
  //    Serial.println(" RPM");  //电机转速
  //    Serial.println(       );  //电机转速
  //    encoderValueD = 0;
  //  }
}

//void updateEncoderA()
//{
//  encoderValueA++;
//}
//
//void updateEncoderB()
//{
//  encoderValueB++;
//}
//
//void updateEncoderC()
//{
//  encoderValueC++;
//}
//
//void updateEncoderD()
//{
//  encoderValueD++;
//}

// 写指令
//void SR09_send_command(byte address, byte reg, byte command) {  // send the command to SR09
//  Wire.beginTransmission(address);    // start the transmission with SR09
//  Wire.write(reg);                    // register 2
//  Wire.write(command);                // send the command to the register 2
//  Wire.endTransmission();             // end of transmission
//}
//
//// 读数据
//word SR09_read_data(byte address, byte reg, byte command) { // sending the command and read the data in the register
//  word temp_reading;
//  // step 1: instruct sensor to read echoes
//  SR09_send_command(address, reg, command);           // send the command
//  // step 2: wait for readings to happen
//  delay(88);                          // wait at least 87 milliseconds
//  // step 3: instruct sensor to return a particular echo reading
//  Wire.beginTransmission(address);    // start to transmit with SR09
//  Wire.write(0x02);             // register 2 is the gate of receiving the data
//  Wire.endTransmission();             // stop transmitting
//  // step 4: request reading from sensor
//  Wire.requestFrom(uint8_t(address), uint8_t(2));       // request the data in the 2nd and third register of SR09
//  // step 5: receive reading from sensor
//  while ( Wire.available() < 2) {}    // wait the register available
//  temp_reading = (Wire.read()) << 8;  // read register 2 and shift it to upper byte.
//  temp_reading |= Wire.read();        // read the register 3 to lower byte
//  return temp_reading;                // return as a 16bit data
//}
