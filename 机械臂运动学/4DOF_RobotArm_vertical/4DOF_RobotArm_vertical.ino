#include <Servo.h>

Servo servo1;   // 旋转伺服
Servo servo2;   // 第二伺服
Servo servo3;   // 第三伺服
Servo servo4;   // 第四伺服
Servo servo5;   // 第五伺服
Servo gripper;  // 手爪伺服

int pin_servo1 = 8;    // 底部旋转舵机（黄色电缆）连接到arduino板上的引脚8
int pin_servo2 = 9;    // 抬臂1舵机（黄色电缆）连接到arduino板上的引脚9
int pin_servo3 = 10;   // 抬臂2舵机（黄色电缆）连接到arduino板上的引脚10
int pin_servo4 = 11;   // 抬臂3舵机（黄色电缆）连接到arduino板上的引脚11
int pin_servo5 = 13;   // 手爪旋转舵机（黄色电缆）连接到arduino板上的引脚13
int pin_gripper = 12;  // 手爪舵机（黄色电缆）连接到arduino板上的引脚12

float Pi = 3.141592653589793;  // π取值
float L1 = 70;                 // 从表面到第二关节位置的第一个链接的高度  unit:mm
float L2 = 105;                // 从第2个关节到第3个关节的第二个链接的长度
float L3 = 128;                // 第3关节到第4关节的长度   145
float L4 = 180;                // 第4个关节到手臂尖端的长度（夹具）,包含手爪旋转舵机

float X_axis;  // 手爪的x轴坐标
float Y_axis;  // 手爪的y轴坐标
float Z_axis;  // 手爪的z轴坐标
float Yoffset, D, d, R;
float alpha1, alpha2;
float Theta_1, Theta_2, Theta_3, Theta_4;

float min_PWM;  //default arduino 544
float max_PWM;  //default arduino 2400

void Rumus_IK() {
  if (X_axis >= 0 && Y_axis > 0) {
    D = sqrt(pow(X_axis, 2) + pow(Z_axis, 2));
    Theta_1 = (atan(Z_axis / X_axis)) * (180.00 / Pi);  // theta 1
    d = D;                                              // 垂直抓取等长情况
    Yoffset = Y_axis - L1;
    R = sqrt(pow(d, 2) + pow(Yoffset, 2));
    alpha1 = (acos(d / R)) * (180.00 / Pi);                                                     // alpha1
    alpha2 = (acos((pow(L2, 2) + pow(R, 2) - pow(L3, 2)) / (2 * L2 * R))) * (180.00 / Pi);      // alpha2
    Theta_2 = alpha1 + alpha2;                                                                  // theta2
    Theta_3 = ((acos((pow(L2, 2) + pow(L3, 2) - pow(R, 2)) / (2 * L2 * L3))) * (180.00 / Pi));  // theta3
    Theta_4 = 180 + 90 - alpha1 - alpha2 - Theta_3;                                             // theta4
  }
}

void setup() {
  Serial.begin(9600);
  // servo1.attach(pin_servo1, min_PWM = 550.0, max_PWM = 550.00 + (180.00 / (209.00 / (2250.00 - 550.00))));
  servo1.attach(pin_servo1, min_PWM = 550.0, max_PWM = 2500.00);
  servo2.attach(pin_servo2, min_PWM = 500.0, max_PWM = 2500.00);
  servo3.attach(pin_servo3, min_PWM = 500.0, max_PWM = 2500.00);
  servo4.attach(pin_servo4, min_PWM = 500.0, max_PWM = 2500.00);
  // servo4.attach(pin_servo4, min_PWM = 550.0, max_PWM = 550.00 + (180.00 / (164.00 / (2000.00 - 550.00))));
  servo5.attach(pin_servo5, min_PWM = 500.0, max_PWM = 2500.00);
  gripper.attach(pin_gripper);  //gripper
}

void loop() {
  gripper.write(90);
  delay(200);
  X_axis = 0.00;
  Z_axis = 100.00;
  Y_axis = 180.00;
  Rumus_IK();
  servo1.write(Theta_1);
  delay(200);
  servo2.write(Theta_2);
  delay(200);
  servo3.write(180 - Theta_3);
  delay(200);
  servo4.write(Theta_4 + 85);
  delay(1000);
  Serial.print("               ");
  Serial.print(Theta_1);
  Serial.print("     ");
  Serial.print(Theta_2);
  Serial.print("     ");
  Serial.print(180 - Theta_3);
  Serial.print("     ");
  Serial.println(Theta_4 + 85);
}
