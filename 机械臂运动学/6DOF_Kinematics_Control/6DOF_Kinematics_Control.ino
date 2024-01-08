#include <Servo.h>
Servo myservo1, myservo2, myservo3, myservo4, myservo5, myservo6;

double x, y, z, AE, AC, A, L2 = 10.5, L3 = 14.5;  //L2和L3输入机械臂各部分的长度。

// 设置变量
int pos1, pos2, pos3, pos4, pos5, pos6;

#define servo1 6   // 底部旋转舵机
#define servo2 7   // 依
#define servo3 8   // 次
#define servo4 9   // 递
#define servo5 10  // 增
#define servo6 11  // 手爪

#define s1MID 90   // 底部旋转舵机默认角度
#define s2MID 90   // 底部抬臂初始角度
#define s3MID 90   // 第二抬臂初始角度
#define s4MID 90   // 第三初始角度
#define s5MID 90   // 第三初始角度
#define s6MID 90   // 张开最大角度  需根据安装角度进行实际设置
#define s6MAX 130  // 闭合角度   需根据安装角度进行实际设置

struct ForwardKinematics {  //建立一个结构体用来存储计算后的角度值
  double Alpha;             //角度α
  double Beta;              //角度β
  double Gamma;             //角度γ
} h;

void Inverse_kinematics(double x, double y, double z) {  //逆运动学计算公式
  AE = sqrt(x * x + y * y);
  AC = sqrt(AE * AE + z * z);
  A = atan(z / AE) * 180 / PI + acos((L2 * L2 + AC * AC - L3 * L3) / (2 * L2 * AC)) * 180 / PI;
  h.Alpha = A;
  h.Beta = acos((L2 * L2 + L3 * L3 - AC * AC) / (2 * L2 * L3)) * 180 / PI;
  h.Gamma = atan(y / x) * 180 / PI;
  pos1 = 90 + h.Gamma;
  pos2 = h.Alpha + 4;              // 4微调参数
  pos3 = 180 - h.Beta + 8;         // 8微调参数
  pos4 = abs(h.Beta + pos2 - 90);  // 保持手爪竖直
}

void setup() {
  Serial.begin(9600);
  myservo1.attach(servo1);  //初始化各个舵机
  myservo2.attach(servo2);
  myservo3.attach(servo3);
  myservo4.attach(servo4);
  myservo5.attach(servo5);
  // myservo6.attach(servo6);
  // myservo1.write(s1MID);
  // myservo2.write(s2MID);
  // myservo3.write(s3MID);
  // myservo4.write(s4MID);
  // myservo5.write(s5MID);
  // myservo5.write(s6MID);

  Inverse_kinematics(10, 10, 10);  // 位置控制只包含3个自由度，手爪没有写入运动学公式中
  delay(1000);                    // 1s
}

void loop() {
  // myservo1.write(s1MID);
  // myservo2.write(s2MID);
  // myservo3.write(s3MID);
  // myservo4.write(s4MID);
  // myservo5.write(s5MID);
  // for (int i = 5; i <= 20; i += 1) {
  //   Inverse_kinematics(5, 0, i);
  myservo1.write(pos1);
  delay(1000);
  myservo2.write(pos2);
  delay(1000);
  myservo3.write(pos3);
  delay(1000);
  myservo4.write(pos4);
  //   delay(100);
  // }
  // for (int i = 20; i >= 5; i -= 1) {
  //   Inverse_kinematics(5, 0, i);
  //   myservo1.write(pos1);
  //   myservo2.write(pos2);
  //   myservo3.write(pos3);
  //   myservo4.write(pos4);
  //   delay(100);
  // }

  // myservo5.write(s5MAX);
  delay(1000);
  Serial.print("  S1:");
  Serial.print(pos1);
  Serial.print("  S2:");
  Serial.print(pos2);
  Serial.print("  S3:");
  Serial.print(pos3);
  Serial.print("  S4:");
  Serial.println(pos4);
}
