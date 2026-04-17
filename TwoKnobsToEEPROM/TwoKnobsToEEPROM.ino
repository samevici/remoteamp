#include <Encoder.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//initialize knobs
Encoder knobLeft(32, 31);
Encoder knobRight(33, 34);

//initialize LCD screen
LiquidCrystal_I2C lcd(0x27,20,4);

//for ISRs
volatile bool writeVals = false;
volatile bool leftReset = false;
volatile bool rightReset = false;

//declare variables
int savedPosLeft, savedPosRight;
int positionLeft;
int positionRight;
int newLeft, newRight;


void setup() {

  //pushbutton button setup
  pinMode(6, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);

  //attach interrupts for eeprom and default vals
  attachInterrupt(digitalPinToInterrupt(6), resetVal, FALLING);
  attachInterrupt(digitalPinToInterrupt(30), defaultLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(35), defaultRight, FALLING);
 

  //bootup message
  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,0);
  lcd.print("Encoder Test:");

  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("loading values...");
  delay(2000);

  //read for from EEPROM
  positionLeft =EEPROM.read(900);
  positionRight= EEPROM.read(901);

  //write to encoder objects  -- x4 multiplier to account for each click being a tick of 1
  knobLeft.write(positionLeft*4);
  knobRight.write(positionRight*4);

  //print position
  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,0);
  lcd.print("Left = ");
  lcd.print(positionLeft ,DEC);

  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("Right = ");
  lcd.print(positionRight ,DEC);

}


void loop() {

  //get values from encoder
  newLeft = knobLeft.read();
  newRight = knobRight.read();

  //default values functions
  if (leftReset)
  {
    knobLeft.write(200);
    positionLeft=50;
    leftReset = false;
  }

  if (rightReset)
  {
    knobRight.write(200);
    positionRight=50;
    rightReset = false;
  }

 //triggered by ISR
   if (writeVals==true)
  {
    //print message and write values to EEPROM
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("Saved values:");
    EEPROM.write(900, positionLeft);
    EEPROM.write(901, positionRight);

    //read back what was written to confirm 
    savedPosLeft =EEPROM.read(900);
    savedPosRight= EEPROM.read(901);

    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("Saved Left = ");
    lcd.print(savedPosLeft, DEC);

    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Saved Right = ");
    lcd.print(savedPosRight,DEC);
    
    delay(1000);

    //go back to printing current settings
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("Left = ");
    lcd.print(positionLeft ,DEC);

    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("Right = ");
    lcd.print(positionRight ,DEC);

    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,3);

   //and make sure we don't come back here unless we press a button
    writeVals=false;
  }


  //setting limits  -- multiplier of 4 for the encoder position.
  if (newLeft>400)
    {newLeft=400;
    positionLeft = newLeft/4;
    knobLeft.write(400);}
  if (newRight>400)
    {newRight=400;
    positionRight = newRight/4;
    knobRight.write(400);}
  if (newLeft<0)
    {newLeft=0;
    positionLeft = newLeft;
    knobLeft.write(0);}
  if (newRight<0)
    {newRight=0;
    positionRight = newRight;
    knobRight.write(0);}


  //update position if the value has changed
  if (newLeft/4 != positionLeft || newRight/4 != positionRight) {
    positionLeft = newLeft/4;
    positionRight = newRight/4;

    //aaand print
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("Left = ");
    lcd.print(positionLeft ,DEC);

    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("Right = ");
    lcd.print(positionRight ,DEC);

    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,3);

  /*  savedPosLeft =EEPROM.read(900);
    savedPosRight= EEPROM.read(901);

    Serial.print("Saved Left = ");
    Serial.print(savedPosLeft,DEC);
    Serial.print(" , Saved Right = ");
    Serial.print(savedPosRight,DEC);
    Serial.println();

    positionLeft = newLeft;
    positionRight = newRight;*/
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
 /* if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);*/
}


void resetVal()
{
  writeVals = true;
}

void defaultRight()
{
  rightReset = true;
}

void defaultLeft()
{
  leftReset = true;
}