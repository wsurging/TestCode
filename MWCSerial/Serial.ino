
#define UART_NUMBER 1

#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 128
#define INBUF_SIZE 64
static volatile uint8_t serialHeadRX[UART_NUMBER],serialTailRX[UART_NUMBER];
static uint8_t serialBufferRX[RX_BUFFER_SIZE][UART_NUMBER];
static volatile uint8_t serialHeadTX[UART_NUMBER],serialTailTX[UART_NUMBER];
static uint8_t serialBufferTX[TX_BUFFER_SIZE][UART_NUMBER];
static uint8_t inBuf[INBUF_SIZE][UART_NUMBER];

static uint8_t checksum[UART_NUMBER];
static uint8_t indRX[UART_NUMBER];
static uint8_t cmdMSP[UART_NUMBER];

#ifndef CURRENTPORT
  #define CURRENTPORT 0
#endif
uint32_t read32() {
  uint32_t t = read16();
  t+= (uint32_t)read16()<<16;
  return t;
}
uint16_t read16() {
  uint16_t t = read8();
  t+= (uint16_t)read8()<<8;
  return t;
}
uint8_t read8()  {
  return inBuf[indRX[CURRENTPORT]++][CURRENTPORT]&0xff;
}
void serialize8(uint8_t a) {
  uint8_t t = serialHeadTX[CURRENTPORT];
  if (++t >= TX_BUFFER_SIZE) t = 0;
  serialBufferTX[t][CURRENTPORT] = a;
  checksum[CURRENTPORT] ^= a;
  serialHeadTX[CURRENTPORT] = t;
} 
void serialize16(int16_t a) {
  serialize8((a   ) & 0xFF);
  serialize8((a>>8) & 0xFF);
}

void serialize32(uint32_t a) {
  serialize8((a    ) & 0xFF);
  serialize8((a>> 8) & 0xFF);
  serialize8((a>>16) & 0xFF);
  serialize8((a>>24) & 0xFF);
}
void headSerialResponse(uint8_t err, uint8_t s) {
  serialize8('$');
  serialize8('M');
  serialize8(err ? '!' : '>');
  checksum[CURRENTPORT] = 0; // start calculating a new checksum
  serialize8(s);
  serialize8(cmdMSP[CURRENTPORT]);
}
void headSerialReply(uint8_t s) {
  headSerialResponse(0, s);
}
void inline headSerialError(uint8_t s) {
  headSerialResponse(1, s);
}

void tailSerialReply() {
  serialize8(checksum[CURRENTPORT]);
  UartSendData();
}

void serializeNames(PGM_P s) {
  for (PGM_P c = s; pgm_read_byte(c); c++) {
    serialize8(pgm_read_byte(c));
  }
}
void serialCom() {
  uint16_t c,n;  
  static uint8_t offset[UART_NUMBER];
  static uint8_t dataSize[UART_NUMBER];
  static enum _serial_state {
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_SIZE,
    HEADER_CMD,
  } c_state[UART_NUMBER];// = IDLE;

  for(n=0;n<UART_NUMBER;n++) {

    while (SerialAvailable(CURRENTPORT))
    {
      uint8_t bytesTXBuff = ((uint8_t)(serialHeadTX[CURRENTPORT]-serialTailTX[CURRENTPORT]))%TX_BUFFER_SIZE; // indicates the number of occupied bytes in TX buffer
      if (bytesTXBuff > TX_BUFFER_SIZE - 50 ) return; // ensure there is enough free TX buffer to go further (50 bytes margin)
      c = SerialRead(CURRENTPORT);
        if (c_state[CURRENTPORT] == IDLE) 
        {
          c_state[CURRENTPORT] = (c==0XFF) ? HEADER_START : IDLE;
          if (c_state[CURRENTPORT] == IDLE)        
            evaluateOtherData(c); // evaluate all other incoming serial data
//            digitalWrite(13,LOW);
//          else
//            digitalWrite(13,HIGH);
        } 
        else if (c_state[CURRENTPORT] == HEADER_START) 
        {
          c_state[CURRENTPORT] = (c==0X00) ? HEADER_M : IDLE;
        } 
        else if (c_state[CURRENTPORT] == HEADER_M)
        {
         // c_state[CURRENTPORT] = (c=='<') ? HEADER_ARROW : IDLE;
      //    c_state[CURRENTPORT] = (c=='<') ? HEADER_M : IDLE;
          if(c==1){
            digitalWrite(13,HIGH);
            c_state[CURRENTPORT]=IDLE;
          }
          if(c==0){
            digitalWrite(13,LOW);
            c_state[CURRENTPORT]=IDLE;
          }
        } 
        else if (c_state[CURRENTPORT] == HEADER_ARROW) 
        {
          if (c > INBUF_SIZE) 
          {  // now we are expecting the payload size
            c_state[CURRENTPORT] = IDLE;
            continue;
          }
          dataSize[CURRENTPORT] = c;
          offset[CURRENTPORT] = 0;
          checksum[CURRENTPORT] = 0;
          indRX[CURRENTPORT] = 0;
          checksum[CURRENTPORT] ^= c;
          c_state[CURRENTPORT] = HEADER_SIZE;  // the command is to follow
        } 
        else if (c_state[CURRENTPORT] == HEADER_SIZE) 
        {
          cmdMSP[CURRENTPORT] = c;
          checksum[CURRENTPORT] ^= c;
          c_state[CURRENTPORT] = HEADER_CMD;
        } 
        else if (c_state[CURRENTPORT] == HEADER_CMD && offset[CURRENTPORT] < dataSize[CURRENTPORT])
        {
          checksum[CURRENTPORT] ^= c;
          inBuf[offset[CURRENTPORT]++][CURRENTPORT] = c;
          for(uint8_t i=0;i<5;i++)
            SerialWrite(0,inBuf[i][0]);
        }
        else if (c_state[CURRENTPORT] == HEADER_CMD && offset[CURRENTPORT] >= dataSize[CURRENTPORT])
        {
          if (checksum[CURRENTPORT] == c) 
          {  // compare calculated and transferred checksum
            evaluateCommand();  // we got a valid packet, evaluate it
            digitalWrite(13,HIGH);
          }
          c_state[CURRENTPORT] = IDLE;
        }
    }
  }
}

void evaluateCommand() {
  switch(cmdMSP[CURRENTPORT]) {
    case 100:
      for(uint8_t i=0;i<8;i++){
        rcData[i]=read16();
      }
    headSerialReply(0);
    break;
  }
}
void evaluateOtherData(uint8_t sr) {
}

ISR(USART_UDRE_vect) {
   uint8_t t = serialTailTX[0];
    if (serialHeadTX[0] != t) {
      if (++t >= TX_BUFFER_SIZE) t = 0;
      UDR0 = serialBufferTX[t][0];  // Transmit next byte in the ring
      serialTailTX[0] = t;
    }
    if (t == serialHeadTX[0]) UCSR0B &= ~(1<<UDRIE0); // Check if all data is transmitted . if yes disable transmitter UDRE interrupt
  }
#define PROMINI
void UartSendData() {
  #if defined(PROMINI)
    UCSR0B |= (1<<UDRIE0);
  #endif
}
static void inline SerialOpen(uint8_t port, uint32_t baud) {
  uint8_t h = ((F_CPU  / 4 / baud -1) / 2) >> 8;
  uint8_t l = ((F_CPU  / 4 / baud -1) / 2);
  switch (port) {
    #if defined(PROMINI)
      case 0: UCSR0A  = (1<<U2X0); UBRR0H = h; UBRR0L = l; UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
        break;
    #endif
  }
}
static void inline SerialEnd(uint8_t port) {
  switch (port) {
    #if defined(PROMINI)
      case 0: UCSR0B &= ~((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0)); 
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
  ISR(USART_RX_vect)  { store_uart_in_buf(UDR0, 0); }
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

