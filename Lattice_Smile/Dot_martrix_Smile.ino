//**************************************************************//
//  Name    : shiftOutCode, Dual Binary Counters                 //
//  Author  : Carlyn Maw, Tom Igoe                               //
//  Date    : 25 Oct, 2006                                       //
//  Version : 1.0                                                //
//  Notes   : Code for using a 74HC595 Shift Register            //
//          : to count from 0 to 255                             //
//**************************************************************//
 
//Single color  8*8 Dot martrix display Smile
 
//Pin connected to RCK of 74HC595
int latchPin = 8;
//Pin connected to SRCK of 74HC595
int clockPin = 12;
////Pin connected to SER of 74HC595
int dataPin = 11;
 
 
 
void setup() {
  //Start Serial for debuging purposes        
  Serial.begin(9600);
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
 
}
 
void loop() {
  //Smile for 8x8 Matrix LED
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B01111111);  //mask(col)
  shiftOut(dataPin, clockPin, B11000000);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B10111111);  //mask(col)
  shiftOut(dataPin, clockPin, B11000010);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B11011111);  //mask(col)
  shiftOut(dataPin, clockPin, B00000001);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B11101111);  //mask(col)
  shiftOut(dataPin, clockPin, B00011001);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B11110111);  //mask(col)
  shiftOut(dataPin, clockPin, B00011001);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B11111011);  //mask(col)
  shiftOut(dataPin, clockPin, B00000001);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B11111101);  //mask(col)
  shiftOut(dataPin, clockPin, B11000010);  //row
  digitalWrite(latchPin, HIGH);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, B11111110);  //mask(col)
  shiftOut(dataPin, clockPin, B11000000);  //row
  digitalWrite(latchPin, HIGH);
 
 
}
 
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low
 
  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
 
  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, LOW);
  digitalWrite(myClockPin, 0);
 
  //for each bit in the byte myDataOut&#239;&#191;&#189;
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=0; i<=7; i++)  {
    digitalWrite(myClockPin, 0);
 
    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {        
      pinState= 0;
    }
    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }
 
  //stop shifting
  digitalWrite(myClockPin, 0);
}
