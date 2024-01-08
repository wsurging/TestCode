/************************************************************
  蓝牙串口控制小车，2个蓝牙模块，一个主模式-连接电脑，
  一个从模式-连接小车主板端口，TX，RX交叉连接。

  @Author: YFROBOT-WST
  @Version: V1.0
  @Date: 08/1/2022
  @URL: www.yfrobot.com.cn
*************************************************************/
char serial_data;   // 将从串口读入的消息存储在该变量中

void setup() {
  Serial.begin(115200);
}

void loop() {
  //  if (Serial.available()){
  //    switch (){
  //      case 'w':
  //        legoMotorDrive(100, 100);     //Move forward
  //        break;
  //      case 's':
  //        legoMotorDrive(100, 100);    //Move backward
  //        break;
  //      case 'a':                    //Turn Left
  //        legoMotorDrive(100, 100);
  //        break;
  //      case 'd':                    //Turn Right
  //        legoMotorDrive(-100, -100);
  //        break;
  //      case 'z':
  //        legoMotorDrive(0, 0);
  //        break;
  //    }
  //  }
  if (Serial.available()) {
    // 将从串口读入的消息存储在该变量中
    serial_data = Serial.read();
    // 按照 w a s d x 在键盘上的布局来控制车
    // 如果接收到 w ，直行
    if ( serial_data == 'w' ) {
      legoMotorDrive(-100, 100);
      Serial.println("The car is moving ahead!");
    }
    // 如果接收到 x ，后退
    else if ( serial_data == 'x') {
      legoMotorDrive(100, -100);
      Serial.println("The car is moving backwards!");
    }
    // 如果接收到 a ，左转
    else if ( serial_data == 'a') {
      legoMotorDrive(-100, -100);
      Serial.println("The car is turning left!");
    }
    // 如果接收到 d ，右转
    else if ( serial_data == 'd') {
      legoMotorDrive(100, 100);
      Serial.println("The car is turning right!");
    }
    // 如果接收到 s ，停止
    else if ( serial_data == 's') {
      legoMotorDrive(0, 0);
      Serial.println("The car is stopping!");
    }
    delay(50);
  }
}

//电机控制函数，端口4.5.6.7
void legoMotorDrive(int mASpeed, int mBSpeed) {
  if (mASpeed > 0) {
    digitalWrite(4, LOW);
  } else if (mASpeed < 0) {
    digitalWrite(4, HIGH);
  }
  analogWrite(5, abs(mASpeed));
  if (mBSpeed > 0) {
    digitalWrite(7, LOW);
  } else if (mBSpeed < 0) {
    digitalWrite(7, HIGH);
  }
  analogWrite(6, abs(mBSpeed));
}
