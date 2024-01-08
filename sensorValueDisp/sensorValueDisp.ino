#include <Wire.h> 
#include <LiquidCrystal_I2C.h>   
#include <QTRSensors.h>
LiquidCrystal_I2C lcd(0x27,16,2);  
QTRSensorsRC qtrrc((unsigned char[]) {14, 15, 16, 17, 3},
  5, 2500, 2); 
unsigned int sensorValues[5];
byte levels[]  = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};
void setup() {
  lcd.init();
  lcd.createChar(0,levels);
  lcd.createChar(1,levels+1);
  lcd.createChar(2,levels+2);
  lcd.createChar(3,levels+3);
  lcd.createChar(4,levels+4);
  lcd.createChar(5,levels+5);
  lcd.createChar(6,levels+6);
  lcd.backlight();
  lcd.clear();
  delay(1000);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  for (int i = 0; i < 200; i++)  // make the calibration take about 10 seconds
    {
      qtrrc.calibrate();      // reads all sensors 10 times at 2500 
    }
  digitalWrite(13,LOW);
  Serial.begin(9600);
}
void displayReading(unsigned int *calibratedValues){
  unsigned int i;
  for(i=0;i<5;i++){
    const int displayCharacters[10]={' ',0,0,1,2,3,4,5,6,255};
    int c = displayCharacters[sensorValues[i]/101];
    Serial.print(sensorValues[i]);
    Serial.print('\t');
    lcd.write(c);
  } 
}

void loop() {
  //int i;
  int position= qtrrc.readLine(sensorValues);
  lcd.setCursor(0,0);
  displayReading(sensorValues);
 /* for(i=0;i<5;i++){
    const int displayCharacters[10]={' ',0,0,1,2,3,4,5,6,255};
    int c = displayCharacters[sensorValues[i]/101];
    Serial.print(sensorValues[i]);
    Serial.print('\t');
    lcd.write(c);
  } */
  lcd.setCursor(8,0);
  lcd.print(position);
  delay(200);
  lcd.clear();
}
