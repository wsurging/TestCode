/**********************************************************************************
  CP+   -------   5v   ARDUINO UNO
  CP-   -------   D13
  CW+   -------   5v   
  CW-   -------   D12  
  EN+   -------   5v  
  EN-   -------   NC  
**********************************************************************************/
#include <AccelStepper.h>

#define _PIN_CLK_ 13
#define _PIN_CW_ 12

#define _DIP_SW_EXCITATION_ 16
#define _MOTOR_NUMER_OF_STEP_PER_ROUND_ 100
#define _SW_MAX_SETEP_ (_DIP_SW_EXCITATION_ * _MOTOR_NUMER_OF_STEP_PER_ROUND_)

AccelStepper stepper(AccelStepper::DRIVER, _PIN_CLK_, _PIN_CW_);
long current_position;

void setup() {

  stepper.setAcceleration(100000);//加速度
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(_DIP_SW_EXCITATION_ * 30);
}

void loop() {
  current_position = stepper.currentPosition();

  if (current_position >= _SW_MAX_SETEP_) {
    stepper.moveTo(0);
  }
  if (current_position <= 0) {
    stepper.moveTo(_SW_MAX_SETEP_);
  }
  stepper.run();
}