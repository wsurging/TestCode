#ifndef PROMINI   //条件指示符#ifndef 检查预编译常量在前面是否已经被宏定义
#define PROMINI
#endif
#if defined(PROMINI)
#define UART_NUMBER 1
#endif

#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 128
#define INBUF_SIZE 64

#define M_Speed_True 112
#define M_Speed_False 113
#define M_FORWARD 114
#define M_LEFT 115
#define M_STOP 116
#define M_RIGHT 117
#define M_BACK 118

static volatile uint8_t serialHeadRX[UART_NUMBER],serialTailRX[UART_NUMBER];   // RX
static uint8_t serialBufferRX[RX_BUFFER_SIZE][UART_NUMBER]; //RX_BUFFER_SIZE == 64   serialBufferRX[数据指针][串口端口号]
static volatile uint8_t serialHeadTX[UART_NUMBER],serialTailTX[UART_NUMBER];   // TX
static uint8_t serialBufferTX[TX_BUFFER_SIZE][UART_NUMBER]; //TX_BUFFER_SIZE == 128  
static uint8_t inBuf[INBUF_SIZE][UART_NUMBER];  //INBUF_SIZE == 64

static uint8_t checksum[UART_NUMBER];
static uint8_t indRX[UART_NUMBER];
static uint8_t cmdMSP[UART_NUMBER];

#if defined(PROMINI)
#define CURRENTPORT 0
#endif

void serialize8(uint8_t a) {
  uint8_t t = serialHeadTX[CURRENTPORT];
  if (++t >= TX_BUFFER_SIZE) t = 0;
  serialBufferTX[t][CURRENTPORT] = a;
  checksum[CURRENTPORT] ^= a;
  serialHeadTX[CURRENTPORT] = t;
}

uint8_t read8()
{
  return inBuf[indRX[CURRENTPORT]++][CURRENTPORT]&0xff;
}

void headSerialReply(uint8_t s)
{
  serialize8('$');
  serialize8('M');
  serialize8( 0 ? '!' :'>');
  checksum[CURRENTPORT] = 0;
  serialize8(s);
  serialize8(cmdMSP[CURRENTPORT]);
}

void tailSerialReply() {
  serialize8(checksum[CURRENTPORT]);
  UartSendData();
}

void serialCom() {
  uint8_t c,n;  
  static uint8_t offset[UART_NUMBER];
  static uint8_t dataSize[UART_NUMBER];
  static enum _serial_state {
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_SIZE,
    HEADER_CMD,
  } 
  c_state[UART_NUMBER];// = IDLE;
  for(n=0;n<UART_NUMBER;n++) {
    while (SerialAvailable(CURRENTPORT)) 
    {
      uint8_t bytesTXBuff = ((uint8_t)(serialHeadTX[CURRENTPORT]-serialTailTX[CURRENTPORT]))%TX_BUFFER_SIZE; 
      // indicates the number of occupied bytes in TX buffer  发送缓冲区占用字节标定
      if (bytesTXBuff > TX_BUFFER_SIZE - 50 ) return; 
      // ensure there is enough free TX buffer to go further (50 bytes margin) 确保发送缓冲区之手50字节的余量
      c = SerialRead(CURRENTPORT);  // 读串口缓冲区
      // regular data handling to detect and handle MSP and other data
      if (c_state[CURRENTPORT] == IDLE) 
      { //串口状态空闲 等待HEADER_START状态的到来
        c_state[CURRENTPORT] = (c=='$') ? HEADER_START : IDLE;  //判定是$字符吗？是则进入HEADER_START状态
        if (c_state[CURRENTPORT] == IDLE) {
          // evaluateOtherData(c); // evaluate all other incoming serial data
        } 
      }
      else if (c_state[CURRENTPORT] == HEADER_START)
      {
        c_state[CURRENTPORT] = (c=='M') ? HEADER_M : IDLE; //判定是M字符吗？是则进入HEADER_M状态
      } 
      else if (c_state[CURRENTPORT] == HEADER_M) 
      {
        c_state[CURRENTPORT] = (c=='>') ? HEADER_ARROW : IDLE; //判定是>字符吗？是则进入HEADER_ARROW状态
      } 
      else if (c_state[CURRENTPORT] == HEADER_ARROW) 
      {  //是ARROW字符，进入HEADER_ARROW状态，判定缓冲区的大小
        if (c > INBUF_SIZE) 
        {  // now we are expecting the payload size 我们期望足够的数据占用缓冲区
          c_state[CURRENTPORT] = IDLE; //数据位置不够 退出循环
          continue;  //不执行该while循环包含的后面的语句，跳出开始下一轮循环
        }
        dataSize[CURRENTPORT] = c;
        offset[CURRENTPORT] = 0;
        checksum[CURRENTPORT] = 0;
        indRX[CURRENTPORT] = 0;
        checksum[CURRENTPORT] ^= c; //校验和 1- dataSize
        c_state[CURRENTPORT] = HEADER_SIZE;  // the command is to follow 接收到数据长度，进入HEADER_SIZE状态
      }
      else if (c_state[CURRENTPORT] == HEADER_SIZE)
      {
        cmdMSP[CURRENTPORT] = c;     //接收指令
        checksum[CURRENTPORT] ^= c;  //校验和 2- code
        c_state[CURRENTPORT] = HEADER_CMD; //接收到指令，进入HEADER_CMD状态
      }
      else if (c_state[CURRENTPORT] == HEADER_CMD && offset[CURRENTPORT] < dataSize[CURRENTPORT])
      {
        checksum[CURRENTPORT] ^= c; //校验和 3- data
        inBuf[offset[CURRENTPORT]++][CURRENTPORT] = c; //将接收到的数据存入缓冲区，offset[CURRENTPORT]加1
      }
      else if (c_state[CURRENTPORT] == HEADER_CMD && offset[CURRENTPORT] >= dataSize[CURRENTPORT])
      {

        if (checksum[CURRENTPORT] == c) 
        {  // compare calculated and transferred checksum
          evaluateCommand();  // we got a  valid packet, evaluate it
        }
        c_state[CURRENTPORT] = IDLE; //返回IDLE状态
      }
    }
  }
}

void evaluateCommand() {
  switch(cmdMSP[CURRENTPORT]) {

  case M_Speed_True:
    flagSpeed = true;
    break;
  case M_Speed_False:
    flagSpeed = false;
    break;
  case M_FORWARD:
      digitalWrite(13,HIGH);
    //    headSerialReply(1);
    setMotor(100,100);
    break;
  case M_LEFT:
    //    headSerialReply(1);
    setMotor(-100,100);   //“向左”行驶 
    break;
  case M_RIGHT:
    //   headSerialReply(1);
    setMotor(100,-100);  //“向右”行驶
    break;
  case M_BACK:
    //   headSerialReply(1);
    setMotor(-100,-100);  //“向后”行驶
    break;
  case M_STOP:
    digitalWrite(13,LOW);
    //   headSerialReply(1);
    setMotor(0,0);    //  “停止”行驶
    break;
    //more code waiting for you ...
  }
//  tailSerialReply();
}

void evaluateOtherData(uint8_t sr) {
}

#if defined(PROMINI)
ISR(USART_UDRE_vect) 
{  // Serial 0 on a PROMINI
#endif
  uint8_t t = serialTailTX[0];
  if (serialHeadTX[0] != t) {
    if (++t >= TX_BUFFER_SIZE) t = 0;
    UDR0 = serialBufferTX[t][0];  // Transmit next byte in the ring
    serialTailTX[0] = t;
  }
  if (t == serialHeadTX[0]) 
    UCSR0B &= ~(1<<UDRIE0);   // Check if all data is transmitted . if yes disable transmitter UDRE interrupt
}
void UartSendData() 
{
#if defined(PROMINI)
  UCSR0B |= (1<<UDRIE0);
#endif
}

static void SerialOpen(uint8_t port, uint32_t baud) {
  uint8_t h = ((F_CPU  / 4 / baud -1) / 2) >> 8;
  uint8_t l = ((F_CPU  / 4 / baud -1) / 2);
  switch (port){
#if defined(PROMINI)
  case 0: 
    UCSR0A  = (1<<U2X0);
    UBRR0H = h; 
    UBRR0L = l; 
    UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
    break;
#endif
  }
}

static void SerialEnd(uint8_t port) {
  switch (port) {
#if defined(PROMINI)
  case 0: 
    UCSR0B &= ~((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0)); 
    break;
#endif
  }
}

static void inline store_uart_in_buf(uint8_t data, uint8_t portnum) {
  uint8_t h = serialHeadRX[portnum];
  if (++h >= RX_BUFFER_SIZE) h = 0;
  if (h == serialTailRX[portnum]) return; // we did not bite our own tail?
  serialBufferRX[serialHeadRX[portnum]][portnum] = data;  
  serialHeadRX[portnum] = h;
}

#if defined(PROMINI)
ISR(USART_RX_vect)  { 
  store_uart_in_buf(UDR0, 0); 
}
#endif

uint8_t SerialRead(uint8_t port) {
  uint8_t t = serialTailRX[port];
  uint8_t c = serialBufferRX[t][port];
  if (serialHeadRX[port] != t) {
    if (++t >= RX_BUFFER_SIZE) t = 0;
    serialTailRX[port] = t;
  }
  return c;
}

uint8_t SerialAvailable(uint8_t port) {
  return (serialHeadRX[port] - serialTailRX[port])%RX_BUFFER_SIZE;
}

void SerialWrite(uint8_t port,uint8_t c){
#if !defined(PROMINI)
  CURRENTPORT=port;
#endif
  serialize8(c);
  UartSendData();
}

void setMotor(int MOTORA,int MOTORB) //电机驱动函数
{
  if(MOTORA>=0)
  {
    digitalWrite(AIN2,HIGH);
    analogWrite(AIN1,255-MOTORA);
  }
  else
  {
    digitalWrite(AIN1,HIGH);
    analogWrite(AIN2,MOTORA+255);
  }
  if(MOTORB>=0)
  {
    digitalWrite(BIN2,HIGH);
    analogWrite(BIN1,255-MOTORB);
  }
  else
  {
    digitalWrite(BIN1,HIGH);
    analogWrite(BIN2,255+MOTORB); 
  }
}

void playMusic(){       
  //函数实现了播放一段音乐，播放完成后表示初使化已经完成 
  for(int thisNote = 0;thisNote < 7;thisNote ++){
    tone(12,melody[thisNote],1000/noteDurations[thisNote]);
    delay(1300/noteDurations[thisNote]);
    noTone(12);
  }
}

void count(){
  Val += 1;
}


