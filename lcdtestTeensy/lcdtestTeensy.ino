/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability

Encoder Knob(7, 8);
LiquidCrystal_I2C lcd(0x27,16,2);
//   avoid using pins with LEDs attached

volatile bool resetKnob = false;

void setup() {
  pinMode(6, INPUT);
  attachInterrupt(digitalPinToInterrupt(6), resetVal, FALLING);

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  delay(1000);
  lcd.setCursor(0,0);
  //Serial.begin(9600);
  lcd.print("                ");
  lcd.print("Encoder Test:");

}


long Position= -999;

void loop() {
  long  newKnob;
  newKnob = Knob.read();

if (resetKnob==true) {
   // newKnob = 0;
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("Knob= ");
    Position = 0;
    lcd.print(Position); 
    resetKnob = false;
  }
 
  if (newKnob != Position) {
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("Knob= ");
    lcd.print(newKnob); 
    Position = newKnob;
  }

}

void resetVal()
{
  resetKnob = true;
}



