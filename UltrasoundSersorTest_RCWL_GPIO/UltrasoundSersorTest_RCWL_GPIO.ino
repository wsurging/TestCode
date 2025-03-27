/*****************************************************************
  开放式超声波测距模块，GPIO模式下，超声波测距模块测距并串口显示
  
  引脚顺序： 
      -GND   
      -VCC     
      -Echo_TX_SDA            
      -Trig_RX_SCL_I/O        
            
-----------------------------------------------------
  This example code is in the public domain.
  Author     : YFROBOT ZL
  Website    : www.yfrobot.com.cn
  Create Time: 2024-05-27
*****************************************************************/

float distance;
const int echo = 9;   //echO接A4脚
const int trig = 10;  //trig接A5脚

void setup() {
  Serial.begin(9600);     //波特率9600
  pinMode(echo, INPUT);   //设置echo为输入脚
  pinMode(trig, OUTPUT);  //设置trig为输出脚
  Serial.println("超声波(GPIO)测距开始：");
}

void loop() {
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);  //Trig脚输出10US高电平脉冲触发信号

  distance = pulseIn(echo, HIGH);         //计数接收到的高电平时间
  distance = distance * 340 / 2 / 10000;  //计算距离 1：声速：340M/S  2：实际距离为1/2声速距离 3：计数时钟为1US//温补公式：c=(331.45+0.61t/℃)m•s-1 (其中331.45是在0度）
  Serial.print("距离: ");
  Serial.print(distance);
  Serial.println("CM");  //串口输出距离信号

  delay(30);   //单次测离完成后加30mS的延时再进行下次测量。防止近距离测量时，测量到上次余波，导致测量不准确。
  delay(100);  //延时100mS再次测量，延时可不要
}
