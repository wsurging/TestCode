
/**************************************
        M1 \\         // M2



        M3 //         \\ M4

 *************************************/

//Motor Driver variables
int DIR1 = 4;     //M1 Direction Control
int PWM1 = 5;

int DIR2 = 7;     //M2 Direction Control
int PWM2 = 6;

int DIR3 = 8;     //M3 Direction Control
int PWM3 = 9;

int DIR4 = 10;    //M4 Direction Control
int PWM4 = 11;

void setup()
{
  Serial.begin(9600);//Initialize the serial port
  pinMode(DIR1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR3, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(DIR4, OUTPUT);
  pinMode(PWM4, OUTPUT);

  digitalWrite(PWM1, LOW);
  digitalWrite(PWM2, LOW);
  digitalWrite(PWM3, LOW);
  digitalWrite(PWM4, LOW);
}

void loop() {
  //  Forward();
  setmotor(100, 100, 100, 100);
  delay(100);
  setmotor(150, 150, 150, 150);
  delay(200);
  setmotor(200, 200, 200, 200);
  delay(300);
  setmotor(255, 255, 255, 255);
  delay(500);
  setmotor(150, 150, 150, 150);
  delay(200);
  setmotor(100, 100, 100, 100);
  delay(100);
  Stop();
  delay(500);
//  Left();
//  delay(1500);
//  Stop();
//  delay(500);
  //  Backward();
  //  delay(1500);
  setmotor(-100, -100, -100, -100);
  delay(100);
  setmotor(-150, -150, -150, -150);
  delay(200);
  setmotor(-200, -200, -200, -200);
  delay(300);
  setmotor(-255, -255, -255, -255);
  delay(500);
  setmotor(-150, -150, -150, -150);
  delay(200);
  setmotor(-100, -100, -100, -100);
  delay(100);
  Stop();
  delay(500);
//  Right();
//  delay(1500);
//  Stop();
//  delay(500);
  //  Forward_left();
  //  delay(2000);
  //  Stop();
  //  delay(1000);
  //  Forward_right();
  //  delay(2000);
  //  Backward_left();
  //  delay(2000);
  //  Stop();
  //  delay(1000);
  //  Backward_right();
  //  delay(2000);
  //  Stop();
  //  delay(1000);
  //  CW();
  //  delay(2000);
  //  Stop();
  //  delay(1000);
  //  CCW();
  //  delay(2000);
  //  Stop();
  //  delay(1000);
}

void setmotor(int m1 , int m2 , int m3, int m4) {
  if (m1 > 0) {
    digitalWrite(DIR1, LOW);
    analogWrite(PWM1, m1);
  } else if (m1 < 0) {
    digitalWrite(DIR1, HIGH);
    analogWrite(PWM1, 0 - m1);
  }
  else {
    analogWrite(PWM1, 0);
  }
  if (m2 > 0) {
    digitalWrite(DIR2, HIGH);
    analogWrite(PWM2, m2);
  } else if (m2 < 0) {
    digitalWrite(DIR2, LOW);
    analogWrite(PWM2, 0 - m2);
  }
  else {
    analogWrite(PWM2, 0);
  }
  if (m3 > 0) {
    digitalWrite(DIR3, LOW);
    analogWrite(PWM3, m3);
  } else if (m3 < 0) {
    digitalWrite(DIR3, HIGH);
    analogWrite(PWM3, 0 - m3);
  }
  else {
    analogWrite(PWM3, 0);
  }
  if (m4 > 0) {
    digitalWrite(DIR4, HIGH);
    analogWrite(PWM4, m4);
  } else if (m4 < 0) {
    digitalWrite(DIR4, LOW);
    analogWrite(PWM4, 0 - m4);
  }
  else {
    analogWrite(PWM4, 0);
  }
}

void Forward(int s) {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, HIGH);

  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
  analogWrite(PWM3, 255);
  analogWrite(PWM4, 255);
}

void Backward() {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, LOW);

  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
  analogWrite(PWM3, 255);
  analogWrite(PWM4, 255);
}

void Left() {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);

  analogWrite(PWM1, 150);
  analogWrite(PWM2, 150);
  analogWrite(PWM3, 150);
  analogWrite(PWM4, 150);
}

void Right() {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);

  analogWrite(PWM1, 150);
  analogWrite(PWM2, 150);
  analogWrite(PWM3, 150);
  analogWrite(PWM4, 150);
}

void Forward_left() {
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, LOW);

  analogWrite(PWM2, 255);
  analogWrite(PWM3, 255);
}

void Forward_right() {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR4, HIGH);

  analogWrite(PWM1, 255);
  analogWrite(PWM4, 255);
}

void Backward_left() {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR4, LOW);

  analogWrite(PWM1, 255);
  analogWrite(PWM4, 255);
}

void Backward_right() {
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, HIGH);

  analogWrite(PWM2, 255);
  analogWrite(PWM3, 255);
}

void CW() {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);

  analogWrite(PWM1, 150);
  analogWrite(PWM2, 150);
  analogWrite(PWM3, 150);
  analogWrite(PWM4, 150);
}

void CCW() {
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);

  analogWrite(PWM1, 150);
  analogWrite(PWM2, 150);
  analogWrite(PWM3, 150);
  analogWrite(PWM4, 150);
}

void Stop() {
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);

  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  analogWrite(PWM3, 0);
  analogWrite(PWM4, 0);
}
