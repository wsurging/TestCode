/*
  Wifi 视频传输 控制舵机转向小车

  Hardware Connections:
  Arduino Pin     Wifi视频传输模块
  3.3v --------   VCC
  GND  --------   GND
  RX   --------   TX

  This example code is in the public domain.
  Author     : YFROBOT ZL
  Website    : www.yfrobot.com.cn
  Create Time: 2021-06-16
  Reference  :
*/

#include <Servo.h>
#define DATA_LENGHT 5

static uint8_t AIL = 0;
static uint8_t ELE = 1;
static uint8_t THR = 2;
static uint8_t RUDD = 3;
static uint8_t FLAG = 4;
static uint8_t Data[5] = {0, 0, 0, 0, 0};
static uint32_t checksum;

Servo myservo1;
Servo myservo2;
Servo myservo3;

#define PWM        5    //电机速度引脚
#define DIR        4    //电机方向引脚
#define SERVOPIN   3    //舵机引脚
#define SERVOX     9    //云台左右旋转舵机引脚
#define SERVOY     10   //云台前后仰舵机引脚

#define SERVOMIN   50   //转向舵机左转最大值
#define SERVOMID   95   //转向舵机回中值
#define SERVOMAX   135  //转向舵机右转最大值

#define SERVO2MIN  0    //云台舵机左转最大值
#define SERVO2MID  92   //云台舵机回中值
#define SERVO2MAX  180  //云台舵机右转最大值

#define SERVO3MIN  55   //云台舵机降低最大值
#define SERVO3MID  85   //云台舵机回中值
#define SERVO3MAX  180  //云台舵机抬起最大值

void setup() {   //程序初始化，只运行一次
  Serial.begin(19200);
  Serial.println("Begin...");
  pinMode(13, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(PWM, OUTPUT);
  myservo1.attach(SERVOPIN);
  myservo2.attach(SERVOX);
  myservo3.attach(SERVOY);
  SERVOMID;
  SERVO2MID;
  SERVO3MID;
  delay(100);
}

void loop() {      //主程序运行
  serialCom();
}

void serialCom() {  //数据通讯
  uint8_t c, checks;
  static uint8_t offset;
  static enum _serial_state {
    IDLE,
    HEADER_66,
    HEADER_99,
  }
  c_state;
  while (Serial.available()) {
    c = Serial.read();                                        // 读串口缓冲区
    if (c_state == IDLE) {                                    // 串口状态空闲,等待HEADER_66状态的到来
      c_state = (c == 0x66) ? HEADER_66 : IDLE;               // 判断是否进入HEADER_66
      offset = 0;
      checksum = 0;
      for (int i = 0 ; i < 5; i++)
        Data[i] = 0;
    } else if (c_state == HEADER_66 && offset < DATA_LENGHT) {   // 进入 HEADER_66 状态，读取数据
      Data[offset] = c;
      checksum ^= c;                                             // 校验和
      offset++;
    } else if (c_state == HEADER_66 && offset >= DATA_LENGHT) {  // 数据结束，读取校验和
      checks = c;
      c_state = HEADER_99;
    } else if (c_state == HEADER_99) {
      if ((checksum & 0xFF) == checks && c == 0x99)           // 判断校验和
        evaluateCommand();                                    // 数据处理程序
      c_state = IDLE;                                         // 返回IDLE状态
    }
  }
}

/*****************************************************************
    数据处理程序，将获取到的数据做处理，执行相对应的指令
 ******************************************************************/
void evaluateCommand() {
  if (Data[ELE] > 0 && Data[AIL] > 0) { //方向舵和升降舵混控  摇杆前后和另一侧方向   前和右为0xff
    if (Data[ELE] >= 0x90 && Data[ELE] <= 0xff && Data[AIL] >= 0x00 && Data[AIL] <= 0x70) { //前+左
      digitalWrite(DIR , LOW);
      analogWrite(PWM , map(Data[ELE], 0x90, 0xff, 0, 255));
      myservo1.write(map(Data[AIL], 0x00, 0x70, SERVOMIN, SERVOMID));
    }
    else if (Data[ELE] >= 0x90 && Data[ELE] <= 0xff && Data[AIL] >= 0x90 && Data[AIL] <= 0xff) { //前+右
      digitalWrite(DIR , LOW);
      analogWrite(PWM , map(Data[ELE], 0x90, 0xff, 0, 255));
      myservo1.write(map(Data[AIL], 0x90, 0xff, SERVOMID, SERVOMAX));
    }
    else if (Data[ELE] >= 0x00 && Data[ELE] <= 0x70 && Data[AIL] >= 0x00 && Data[AIL] <= 0x70) { //后+左
      digitalWrite(DIR , HIGH);
      analogWrite(PWM , map(Data[ELE], 0x70, 0x00, 0, 255));
      myservo1.write(map(Data[AIL], 0x00, 0x70, SERVOMIN, SERVOMID));
    }
    else if (Data[ELE] >= 0x00 && Data[ELE] <= 0x70 && Data[AIL] >= 0x90 && Data[AIL] <= 0xff) { //后+右
      digitalWrite(DIR , HIGH);
      analogWrite(PWM , map(Data[ELE], 0x70, 0x00, 0, 255));
      myservo1.write(map(Data[AIL], 0x90, 0xff, SERVOMID, SERVOMAX));
    }
    else if (Data[ELE] >= 0x90 && Data[ELE] <= 0xff) { //前
      digitalWrite(DIR , LOW);
      analogWrite(PWM , map(Data[ELE], 0x90, 0xff, 0, 255));
    }
    else if (Data[ELE] >= 0x00 && Data[ELE] <= 0x70) { //后
      digitalWrite(DIR , HIGH);
      analogWrite(PWM , map(Data[ELE], 0x70, 0x00, 0, 255));
    }
    else if (Data[ELE] == 0x80 && Data[AIL] == 0x80) {
      analogWrite(PWM , 0);
      myservo1.write(SERVOMID);
    }
  }

  if ( Data[AIL] > 0) {
    if (Data[AIL] >= 0x00 && Data[AIL] <= 0x70) { //左
      myservo1.write(map(Data[AIL], 0x00, 0x70, SERVOMIN, SERVOMID));
    }
    else if (Data[AIL] >= 0x90 && Data[AIL] <= 0xff) { //右
      myservo1.write(map(Data[AIL], 0x90, 0xff, SERVOMID, SERVOMAX));
    }
    else {
      myservo1.write(SERVOMID);
    }
  }

  if (Data[RUDD] > 0) {   //  小 0x00-0xff 大
    if (Data[RUDD] >= 0x00 && Data[RUDD] <= 0x70) { //左
      myservo2.write(map(Data[RUDD], 0x00, 0x70, SERVO2MAX, SERVO2MID));
    }
    else if (Data[RUDD] >= 0x90 && Data[RUDD] <= 0xff) { //右
      myservo2.write(map(Data[RUDD], 0x90, 0xff, SERVO2MID, SERVO2MIN));
    }
    else {
      myservo2.write(SERVO2MID);
    }
  }

  if (Data[THR] > 0) {    // 小 0x00-0xff 大
    if (Data[THR] >= 0x00 && Data[THR] <= 0x70) { //左
      myservo3.write(map(Data[THR], 0x00, 0x70, SERVO3MIN, SERVO3MID));
    }
    else if (Data[THR] >= 0x90 && Data[THR] <= 0xff) { //右
      myservo3.write(map(Data[THR], 0x90, 0xff, SERVO3MID, SERVO3MAX));
    }
    else {
      myservo3.write(SERVO3MID);
    }
  }

  switch (Data[FLAG]) {  // 标志位
    case 0x40:                               // 开灯 软件默认状态，关闭灯后为0x00
      //      digitalWrite(13, LOW);
      break;
    case 0x41:                               // 起飞键 开灯状态，关闭状态为0x01
      digitalWrite(13, HIGH);
      break;
    case 0x42:                               // 降落键 开灯状态，关闭状态为0x02
      digitalWrite(13, LOW);
      break;
      //    case 0x44:                               // 停止键 开灯状态，关闭状态为0x04
      //      //      Stop();
      //      break;
      //    case 0x48:                               // 360°翻转键 开灯状态，关闭状态为0x08
      //      digitalWrite(13, HIGH);
      //      break;
      //    case 0x50:                               // 无头模式键 开灯状态，关闭状态为0x10
      //      digitalWrite(13, HIGH);
      //      break;
      //    case 0xC0:                               // 飞控校准键 开灯状态，关闭状态为
      //      digitalWrite(13, HIGH);
      //      break;
  }
}
