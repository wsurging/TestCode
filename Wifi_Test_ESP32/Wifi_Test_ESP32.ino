/*
  Wifi 视频传输

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
#include <HardwareSerial.h>
HardwareSerial SerialPort(1);  // 使用UART1

#define DATA_LENGHT 5

static uint8_t AIL = 0;
static uint8_t ELE = 1;
static uint8_t THR = 2;
static uint8_t RUDD = 3;
static uint8_t FLAG = 4;
static uint8_t Data[5] = { 0, 0, 0, 0, 0 };
static uint32_t checksum;

void setup() {  //程序初始化，只运行一次
  // 硬件串口1, 26端口连接WiFi图传模块信号线，可自行设置端口
  SerialPort.begin(19200, SERIAL_8N1, 26, 27);

  Serial.begin(115200);  // 硬件串口0，可在串口查看数据
  Serial.println("Begin...");
}

/*****************************************************************
    数据处理程序，将获取到的数据做处理，执行相对应的指令
 ******************************************************************/
void evaluateCommand() {
  if (Data[THR] > 0) {                             // 油门摇杆控制
    if (Data[THR] >= 0x00 && Data[THR] <= 0xff) {  //左
    }
  }

  if (Data[AIL] > 0) {
    if (Data[AIL] >= 0x00 && Data[AIL] <= 0x70) {  //左
      digitalWrite(2, HIGH);
      Serial.println("1");
    } else if (Data[AIL] >= 0x90 && Data[AIL] <= 0xff) {  //右
      digitalWrite(2, LOW);
      Serial.println("2");
    }
  }

  if (Data[RUDD] > 0) {                              //  小 0x00-0xff 大
    if (Data[RUDD] >= 0x00 && Data[RUDD] <= 0x70) {  //左
      digitalWrite(2, HIGH);
      Serial.println("3");
    } else if (Data[RUDD] >= 0x90 && Data[RUDD] <= 0xff) {  //右
      digitalWrite(2, LOW);
      Serial.println("4");
    }
  }

  switch (Data[FLAG]) {  // 标志位
    case 0x40:           // 软件默认状态为0x40
      break;
    case 0x41:  // 起飞键 状态为0x41 一秒后变为0x40.
      break;
    case 0x42:  // 降落键 状态为0x42 一秒后变为0x40.
      break;
  }
}

void loop() {  //主程序运行
  serialCom();
}

void serialCom() {  //数据通讯
  uint8_t c;
  static uint8_t offset, checks;
  static enum _serial_state {
    IDLE,
    HEADER_66,
    HEADER_99,
  } c_state;
  while (SerialPort.available()) {
    c = SerialPort.read();                       // 读串口缓冲区
    if (c_state == IDLE) {                       // 串口状态空闲,等待HEADER_66状态的到来
      c_state = (c == 0x66) ? HEADER_66 : IDLE;  // 判断是否进入HEADER_66
      offset = 0;
      checksum = 0;
      for (int i = 0; i < 5; i++)
        Data[i] = 0;
    } else if (c_state == HEADER_66 && offset < DATA_LENGHT) {  // 进入 HEADER_66 状态，读取数据
      Data[offset] = c;
      checksum ^= c;  // 校验和
      offset++;
    } else if (c_state == HEADER_66 && offset >= DATA_LENGHT) {  // 数据结束，读取校验和
      checks = c;
      c_state = HEADER_99;
    } else if (c_state == HEADER_99) {
      if (uint8_t(checksum & 0xFF) == checks && c == 0x99)  // 判断校验和
        evaluateCommand();                                  // 数据处理程序
      c_state = IDLE;                                       // 返回IDLE状态
    }
  }
}
