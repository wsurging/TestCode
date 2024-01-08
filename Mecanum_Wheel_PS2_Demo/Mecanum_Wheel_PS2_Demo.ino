#include <Arduino.h>
#include <PS2X_lib.h> //PS2 Library can be found here: 
PS2X ps2x;

#define PS2_DAT        A0     //PS2 Pin
#define PS2_CMD        A1
#define PS2_SEL        A2
#define PS2_CLK        A3

//Motor Driver variables
int DIR1 = 4;     //M1 Direction Control
int PWM1 = 5;

int DIR2 = 7;     //M2 Direction Control
int PWM2 = 6;

int DIR3 = 8;     //M3 Direction Control
int PWM3 = 9;

int DIR4 = 10;    //M4 Direction Control
int PWM4 = 11;

int Speed = 80;


int MaxSpeed = 255;  //Maximum stroke speed of rocker

bool EnableRockerR = 1;
bool EnableRockerL = 1;
int RStop = 0 ;
int LStop = 0 ;

#define pressures   false
#define rumble      false
int error = 0;
byte type = 0;
byte vibrate = 0;
void(* resetFunc) (void) = 0;

void setup()
{
  Serial.begin(57600);//Initialize the serial port
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
  delay(300);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
}

void loop() {
  if (error == 1) //skip loop if no controller found
    resetFunc();
  //read controller and set large motor to spin at 'vibrate' speed
  ps2x.read_gamepad(false, vibrate);

  //  if (ps2x.ButtonPressed(PSB_PAD_UP)) {          //Forward pressed
  //    Forward();
  //    EnableRockerL = 0;
  //  } else if (ps2x.ButtonReleased(PSB_PAD_UP)) {   //Forward released
  //    Stop();
  //    EnableRockerL = 1;
  //    LStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {         //Down pressed
  //    Backward();
  //    EnableRockerL = 0;
  //  } else if (ps2x.ButtonReleased(PSB_PAD_DOWN)) {  //Down released
  //    Stop();
  //    EnableRockerL = 1;
  //    LStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {         //LEFT pressed
  //    Left();
  //    EnableRockerL = 0;
  //  } else if (ps2x.ButtonReleased(PSB_PAD_LEFT)) {  //LEFT released
  //    Stop();
  //    EnableRockerL = 1;
  //    LStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {         //RIGHT pressed
  //    Right();
  //    EnableRockerL = 0;
  //  } else if (ps2x.ButtonReleased(PSB_PAD_RIGHT)) {  //RIGHT released
  //    Stop();
  //    EnableRockerL = 1;
  //    LStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_L1)) {
  //
  //    EnableRockerL = 0;
  //  } else if (ps2x.ButtonReleased(PSB_L1)) {
  //    Stop();
  //    EnableRockerL = 1;
  //    LStop = 0;
  //  }


  if (EnableRockerL) {
    if (0 <= ps2x.Analog(PSS_LX) && ps2x.Analog(PSS_LX) <= 120) {
      digitalWrite(DIR1, LOW);
      digitalWrite(DIR2, HIGH);
      digitalWrite(DIR3, LOW);
      digitalWrite(DIR4, HIGH);

      analogWrite(PWM1, map(ps2x.Analog(PSS_LX), 0, 120, MaxSpeed, 0));
      analogWrite(PWM2, map(ps2x.Analog(PSS_LX), 0, 120, MaxSpeed, 0));
      analogWrite(PWM3, map(ps2x.Analog(PSS_LX), 0, 120, MaxSpeed, 0));
      analogWrite(PWM4, map(ps2x.Analog(PSS_LX), 0, 120, MaxSpeed, 0));
      LStop = 0;
    } else if (255 >= ps2x.Analog(PSS_LX) && ps2x.Analog(PSS_LX) >= 135) {
      digitalWrite(DIR1, HIGH);
      digitalWrite(DIR2, LOW);
      digitalWrite(DIR3, HIGH);
      digitalWrite(DIR4, LOW);

      analogWrite(PWM1, map(ps2x.Analog(PSS_LX), 135, 255, 0, MaxSpeed));
      analogWrite(PWM2, map(ps2x.Analog(PSS_LX), 135, 255, 0, MaxSpeed));
      analogWrite(PWM3, map(ps2x.Analog(PSS_LX), 135, 255, 0, MaxSpeed));
      analogWrite(PWM4, map(ps2x.Analog(PSS_LX), 135, 255, 0, MaxSpeed));
      LStop = 0;
    } else if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 120) {
      digitalWrite(DIR1, HIGH);
      digitalWrite(DIR2, HIGH);
      digitalWrite(DIR3, LOW);
      digitalWrite(DIR4, LOW);

      analogWrite(PWM1, map(ps2x.Analog(PSS_LY), 0, 120, MaxSpeed, 0));
      analogWrite(PWM2, map(ps2x.Analog(PSS_LY), 0, 120, MaxSpeed, 0));
      analogWrite(PWM3, map(ps2x.Analog(PSS_LY), 0, 120, MaxSpeed, 0));
      analogWrite(PWM4, map(ps2x.Analog(PSS_LY), 0, 120, MaxSpeed, 0));
      LStop = 0;
    } else if (255 >= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) >= 135) {
      digitalWrite(DIR1, LOW);
      digitalWrite(DIR2, LOW);
      digitalWrite(DIR3, HIGH);
      digitalWrite(DIR4, HIGH);

      analogWrite(PWM1, map(ps2x.Analog(PSS_LY), 135, 255, 0, MaxSpeed));
      analogWrite(PWM2, map(ps2x.Analog(PSS_LY), 135, 255, 0, MaxSpeed));
      analogWrite(PWM3, map(ps2x.Analog(PSS_LY), 135, 255, 0, MaxSpeed));
      analogWrite(PWM4, map(ps2x.Analog(PSS_LY), 135, 255, 0, MaxSpeed));
      LStop = 0;
    } else {
      if (LStop == 0) {
        Stop();
        LStop ++;
      }
    }
  }


  //  if (ps2x.ButtonPressed(PSB_TRIANGLE)) {          //TRIANGLE pressed
  //
  //    EnableRockerR = 0;
  //  } else if (ps2x.ButtonReleased(PSB_TRIANGLE)) {   //TRIANGLE released
  //    Stop();
  //    EnableRockerR = 1;
  //    RStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_CIRCLE)) {             //CIRCLE pressed
  //
  //    EnableRockerR = 0;
  //  } else if (ps2x.ButtonReleased(PSB_CIRCLE)) {      //CIRCLE released
  //    Stop();
  //    EnableRockerR = 1;
  //    RStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_SQUARE)) {             //SQUARE pressed
  //
  //    EnableRockerR = 0;
  //  } else if (ps2x.ButtonReleased(PSB_SQUARE)) {      //CIRCLE released
  //    Stop();
  //    EnableRockerR = 1;
  //    RStop = 0;
  //  }

  //will be TRUE if button was JUST pressed OR released
  //  if (ps2x.ButtonPressed(PSB_CROSS)) {
  //
  //    EnableRockerR = 0;
  //  } else if (ps2x.ButtonReleased(PSB_CROSS)) {
  //    Stop();
  //    EnableRockerR = 1;
  //    RStop = 0;
  //  }
  //
  //  if (ps2x.ButtonPressed(PSB_R1)) {
  //
  //    EnableRockerR = 0;
  //  } else if (ps2x.ButtonReleased(PSB_R1)) {
  //    Stop();
  //    EnableRockerR = 1;
  //    RStop = 0;
  //  }

  if (EnableRockerR) {
    if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120 && 0 <= ps2x.Analog(PSS_RY) && ps2x.Analog(PSS_RY) <= 120) {  //前左
      digitalWrite(DIR2, HIGH);
      digitalWrite(DIR3, LOW);

      analogWrite(PWM2, Speed);
      analogWrite(PWM3, Speed);
      RStop = 0;
    } else if (255 >= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) >= 135 && 255 >= ps2x.Analog(PSS_RY) && ps2x.Analog(PSS_RY) >= 135) {  //后右
      digitalWrite(DIR2, LOW);
      digitalWrite(DIR3, HIGH);

      analogWrite(PWM2, Speed);
      analogWrite(PWM3, Speed);
      RStop = 0;
    } else if (0 <= ps2x.Analog(PSS_RY) && ps2x.Analog(PSS_RY) <= 120 && 255 >= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) >= 135) {//后左
      digitalWrite(DIR1, HIGH);
      digitalWrite(DIR4, LOW);

      analogWrite(PWM1, Speed);
      analogWrite(PWM4, Speed);
      RStop = 0;
    } else if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 120 && 255 >= ps2x.Analog(PSS_RY) && ps2x.Analog(PSS_RY) >= 135) {//前右
      digitalWrite(DIR2, LOW);
      digitalWrite(DIR3, HIGH);

      analogWrite(PWM1, Speed);
      analogWrite(PWM4, Speed);
      RStop = 0;
    } else {
      if (RStop == 0) {
        Stop();
        RStop ++;
      }
    }
  }
  delay(50);
}

////Motor modules
//void Forward()
//{
//  digitalWrite(DIRA, LOW);
//  digitalWrite(DIRB, LOW);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Backward()
//{
//  digitalWrite(DIRA, HIGH);
//  digitalWrite(DIRB, HIGH);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Left()
//{
//  digitalWrite(DIRA, HIGH);
//  digitalWrite(DIRB, LOW);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Right()
//{
//  digitalWrite(DIRA, LOW);
//  digitalWrite(DIRB, HIGH);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Forward_left()
//{
//  digitalWrite(DIRA, HIGH);
//  digitalWrite(DIRB, LOW);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Forward_right()
//{
//  digitalWrite(DIRA, LOW);
//  digitalWrite(DIRB, HIGH);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Backward_left()
//{
//  digitalWrite(DIRA, HIGH);
//  digitalWrite(DIRB, LOW);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
//void Backward_right()
//{
//  digitalWrite(DIRA, LOW);
//  digitalWrite(DIRB, HIGH);
//
//  analogWrite(PWM1, Speed1);
//  analogWrite(PWM2, Speed2);
//}
//
void Stop()
{
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);

  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  analogWrite(PWM3, 0);
  analogWrite(PWM4, 0);
}
