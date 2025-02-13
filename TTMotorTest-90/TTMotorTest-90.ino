/*****************************************************************************
  TT马达金属单轴测试程序

  @Author: YFROBOT-WST
  @Version: V1.0
  @Date: 06/10/2022
  @URL: WWW.YFROBOT.COM.CN
******************************************************************************/

/*****************************************
   导入必须的库文件
 *****************************************/
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, SCL, SDA, U8X8_PIN_NONE);
/*******************************************
            电机驱动端口设置
*******************************************/

#define PWMA 5  // 电机引脚
#define PWMB 6  // 电机引脚

float SPEED = 150;      // 小车转速值
float encoder = 3;      // 编码器精度
float speedRatio = 90;  // 减速比

#define EncoderA 2  // 中断检测端口，数字口D2
#define EncoderB 4  // 中断检测端口，数字口D4
int rpm = 0;

long intervalTime = 1000;  //采样时间1s
unsigned long previousTime = 0;
volatile long encoderPos = 0;

/*******************************************
   蜂鸣器端口设置
*******************************************/
#define buzzer 12  // 蜂鸣器引脚

void setup() {
  Serial.begin(115200);
  pinMode(PWMA, OUTPUT);  // Pin state
  pinMode(PWMB, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(EncoderA, INPUT_PULLUP);  // 将霍尔传感器A设置为输入上拉

  attachInterrupt(digitalPinToInterrupt(EncoderA), Encoder, CHANGE);

  u8g2.setI2CAddress(0x3d * 2);  // IIC地址
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.setFont(u8g2_font_crox2h_tr);
  // u8g2.drawStr(30, 40, "Buzzer Test");
  u8g2.sendBuffer();
  previousTime = millis();
}

void loop() {
  u8g2.setFont(u8g2_font_crox2h_tr);  // 字体u8g2_font_helvR08_tf/u8g2_font_crox2h_tr

  digitalWrite(PWMA, LOW);
  analogWrite(PWMB, SPEED);

  if (millis() - previousTime >= intervalTime) {

    rpm = encoderPos / encoder / speedRatio / intervalTime / 2 * 1000 * 60;  // 编码器检测

    // u8g2.drawStr(45, 20, "RPM:");
    u8g2.setCursor(30, 40);
    u8g2.print(rpm);                // 打印检测值
    u8g2.drawStr(70, 40, "rpm/m");  // 单位
    u8g2.sendBuffer();
    u8g2.clearBuffer();

    Serial.print(encoderPos);
    Serial.print(" ");
    Serial.println(rpm);

    encoderPos = 0;  // 编码器检测数值清零，否则累计计算编码器数值
    previousTime = millis();
  }
}

void Encoder() {
  if (digitalRead(EncoderA) == HIGH) {
    if (digitalRead(EncoderB) == LOW) {
      encoderPos++;
    } else {
      encoderPos--;
    }
  } else {
    if (digitalRead(EncoderB) == LOW) {
      encoderPos--;
    } else {
      encoderPos++;
    }
  }
}
