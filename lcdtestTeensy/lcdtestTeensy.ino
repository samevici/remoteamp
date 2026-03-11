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
//Encoder knobLeft(5, 6);
Encoder Knob(7, 8);
LiquidCrystal_I2C lcd(0x27,16,2);
//   avoid using pins with LEDs attached

void setup() {
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  delay(100);
  lcd.setCursor(0,0);
  //Serial.begin(9600);
  lcd.print("Encoder Test:");
}


long Position= -999;

void loop() {
  long  newKnob;
  newKnob = Knob.read();
  if (newKnob != Position) {
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("Knob= ");
    lcd.print(newKnob); 
    Position = newKnob;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
 /* if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    //knobLeft.write(0);
    knobRight.write(0);
  }*/
}
