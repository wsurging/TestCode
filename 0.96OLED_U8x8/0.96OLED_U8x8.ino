/*

  HelloWorld.ino
  
  "Hello World" version for U8x8 API

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// Please UNCOMMENT one of the contructor lines below
// U8x8 Contructor List 
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
//U8X8_NULL u8x8;	// null device, a 8x8 pixel display which does nothing
// U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 6, /* dc=*/ 4, /* reset=*/ 12);	// Arduboy (DevKit)
//U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6);	// Arduboy 10 (Production, Kickstarter Edition)
// U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_3W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_3W_HW_SPI u8x8(/* cs=*/ 10, /* reset=*/ 8);
// U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 	      
// U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 	      // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
// U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 2, /* data=*/ 0, /* reset=*/ U8X8_PIN_NONE); 	      // Digispark ATTiny85
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
//U8X8_SSD1306_128X64_VCOMH0_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);		// same as the NONAME variant, but maximizes setContrast() range
//U8X8_SSD1306_128X64_ALT0_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);		// same as the NONAME variant, but may solve the "every 2nd line skipped" problem

//U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//U8X8_SSD1306_128X32_WINSTAR_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

//U8X8_SSD1306_128X64_NONAME_6800 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8X8_SSD1306_128X64_NONAME_8080 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);

// End of constructor list


void setup(void)
{
  /* U8g2 Project: SSD1306 Test Board */
  //pinMode(10, OUTPUT);
  //pinMode(9, OUTPUT);
  //digitalWrite(10, 0);
  //digitalWrite(9, 0);		
  
  /* U8g2 Project: KS0108 Test Board */
  //pinMode(16, OUTPUT);
  //digitalWrite(16, 0);	
  
  u8x8.begin();
  u8x8.setPowerSave(0);
  
  
}

void loop(void)
{
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,1,"Hello World!");
  u8x8.drawString(0,0,"Line 0");
  u8x8.drawString(0,8,"Line 8");
  u8x8.drawString(0,9,"Line 9");
  u8x8.refreshDisplay();		// only required for SSD1606/7  
  delay(2000);
}
