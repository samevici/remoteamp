// include the SPI library:
#include <SPI.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// set chip select pins the digital pots:
const int CSPin00 = 10;
const int CSPin01 = 9;
const int CSPin02 = 8;
const int CSPin03 = 7;
const int CSPin04 = 6;

//important SPI addresses for digipot
const int wiper0WAddr = B00000000;
const int wiper1WAddr = B00010000;
const int wiper0RAddr = B00001100;
const int wiper1RAddr = B00011100;

/* Tcon is wired to VCC on PCB, so these are not used
const int tconWAddr = B01000000;
const int tcon_0off_1on = B11110000;
const int tcon_0on_1off = B00001111;
const int tcon_0off_1off = B00000000;
const int tcon_0on_1on = B11111111;*/

//initialize LCD screen
LiquidCrystal_I2C lcd(0x27,20,4);

//initialize encoder objects
Encoder knobHi(1,2);
Encoder knobBand(4, 5);
Encoder knobLo(25,26);
Encoder knobResonant(28,29);
Encoder knobFeedback(31,32);
Encoder knobDistGain(34,35);
Encoder knobDistClip(37,38);
Encoder knobVol(40,41);

//volatiles for ISRs -- EEPROM and all encoder buttons
volatile bool writeVals = false;
volatile bool hiReset = false; // triggered by pin 0
volatile bool bandReset = false; // triggered by pin 3
volatile bool loReset = false; // triggered by pin 24
volatile bool resReset = false; // triggered by pin 27
volatile bool feedbackReset = false; // triggered by pin 30
volatile bool distGainReset = false; // triggered by pin 33
volatile bool distClipReset = false; // triggered by pin 36
volatile bool volReset = false; // triggered by pin 39

//declare encoder vars: for storing the EEPROM values, the position, new readings, and to send to LCD
int savedPosHi, savedPosBand, savedPosLo, savedPosResonant, savedPosFeedback, savedPosDistGain, savedPosDistClip, savedPosVol;
int positionHi, positionBand, positionLo, positionResonant, positionFeedback, positionDistGain, positionDistClip, positionVol;
int newHi, newBand, newLo, newResonant, newFeedback, newDistGain, newDistClip, newVol;
int dispHi, dispBand, dispLo, dispResonant, dispFeedback, dispDistGain, dispDistClip, dispVol;


void setup() {
  // set the CS pins as outputs:
  pinMode (CSPin00, OUTPUT);
  pinMode (CSPin01, OUTPUT);
  pinMode (CSPin02, OUTPUT);
  pinMode (CSPin03, OUTPUT);
  pinMode (CSPin04, OUTPUT);

  //set buttons and encoder pin 3s as inputs
  pinMode(15, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP); //hi
  pinMode(3, INPUT_PULLUP); //band
  pinMode(24, INPUT_PULLUP); //lo
  pinMode(27, INPUT_PULLUP); //resonant
  pinMode(30, INPUT_PULLUP); //feedback
  pinMode(33, INPUT_PULLUP); //dist gain
  pinMode(36, INPUT_PULLUP); //dist clip
  pinMode(39, INPUT_PULLUP); //vol

  //set up ISRs for buttons and encoders:
  attachInterrupt(digitalPinToInterrupt(15), writeToEEPROM, FALLING);
  attachInterrupt(digitalPinToInterrupt(0), defaultHi, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), defaultBand, FALLING);
  attachInterrupt(digitalPinToInterrupt(24), defaultLo, FALLING);
  attachInterrupt(digitalPinToInterrupt(27), defaultRes, FALLING);
  attachInterrupt(digitalPinToInterrupt(30), defaultFeedback, FALLING);
  attachInterrupt(digitalPinToInterrupt(33), defaultDistGain, FALLING);
  attachInterrupt(digitalPinToInterrupt(36), defaultDistClip, FALLING);
  attachInterrupt(digitalPinToInterrupt(39), defaultVol, FALLING);
  
// initialize SPI:
  SPI.begin(); 

  // initialize the lcd  & print greeting
  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,0);
  lcd.print(" Web-controlled amp");
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("         by         ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("      samevici      ");
  delay(1000);

  //load values from EEPROM
  positionHi = EEPROM.read(1000);
  positionBand = EEPROM.read(1001);
  positionLo = EEPROM.read(1002);
  positionResonant = EEPROM.read(1003);
  positionFeedback = EEPROM.read(1004);
  positionDistGain = EEPROM.read(1005);
  positionDistClip = EEPROM.read(1006); 
  positionVol = EEPROM.read(1007);

  dispHi = positionHi;
  dispBand = positionBand;
  dispLo = positionLo;
  dispResonant = positionResonant; 
  dispFeedback =  positionFeedback;
  dispDistGain = positionDistGain;
  dispDistClip =  positionDistClip;
  dispVol = positionVol;

  //write saved values to encoder objects
  knobHi.write(positionHi);
  knobBand.write(positionBand);
  knobLo.write(positionLo);
  knobResonant.write(positionResonant);
  knobFeedback.write(positionFeedback);
  knobDistGain.write(positionDistGain);
  knobDistClip.write(positionDistClip);
  knobVol.write(positionVol);

  //clear screen 
  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("                    ");

  //and display loaded values
  lcd.setCursor(0,0);
  lcd.print("loading values...");
  delay(2000);

  lcdLayout(dispHi, dispBand, dispLo, dispVol, dispResonant, dispFeedback, dispDistGain, dispDistClip);

}

void loop() {

  //get values from encoder
  newHi = knobHi.read();
  newBand = knobBand.read();
  newLo = knobLo.read();
  newResonant = knobResonant.read();
  newFeedback = knobFeedback.read();
  newDistGain = knobDistGain.read();
  newDistClip = knobDistClip.read();
  newVol = knobVol.read();

  //check for position change
  changeEncoder(&positionHi, &newHi, 0, 100, &knobHi);
  changeEncoder(&positionBand, &newBand, 0, 100, &knobBand);
  changeEncoder(&positionLo, &newLo, 0, 100, &knobLo);
  changeEncoder(&positionResonant, &newResonant, 0, 255, &knobResonant);
  changeEncoder(&positionFeedback, &newFeedback, 0, 255, &knobFeedback);
  changeEncoder(&positionDistGain, &newDistGain, 0, 255, &knobDistGain);
  changeEncoder(&positionDistClip, &newDistClip, 0, 255, &knobDistClip);
  changeEncoder(&positionVol, &newVol, 0, 255, &knobVol);

  //change potentiometer settings and return the the byte read from potentiometer
  changePot(&newHi, &positionHi, &dispHi, 100, CSPin02, wiper0WAddr, wiper0RAddr);
  changePot(&newBand, &positionBand, &dispBand, 100, CSPin01, wiper0WAddr, wiper0RAddr);
  changePot(&newLo, &positionLo, &dispLo, 100, CSPin02, wiper1WAddr, wiper1RAddr);
  changePot(&newResonant, &positionResonant, &dispResonant,  255, CSPin00, wiper0WAddr, wiper0RAddr);
  changePot(&newFeedback, &positionFeedback, &dispFeedback, 255, CSPin01, wiper1WAddr, wiper1RAddr);
  changePot(&newDistGain, &positionDistGain, &dispDistGain,  255, CSPin03, wiper0WAddr, wiper0RAddr);
  changePot(&newDistClip, &positionDistClip, &dispDistClip, 255, CSPin03, wiper1WAddr, wiper1RAddr);
  changePot(&newVol, &positionVol, &dispVol, 255, CSPin04, wiper1WAddr, wiper1RAddr);

  //display byte from pot (converted to %)
  //lcdLayout(dispHi*100/255, dispBand*100/255, dispLo*100/255, dispVol*100/255, dispResonant*100/255, dispFeedback*100/255, dispDistGain*100/255, dispDistClip*100/255);


  //Reset encoders to defaults if pressed
  if (hiReset)
  {
    knobHi.write(50);
    positionHi=50;
    hiReset = false;
  }

  if (bandReset)
  {
    knobBand.write(50);
    positionBand=50;
    bandReset = false;
  }

  if (loReset)
  {
    knobLo.write(50);
    positionLo=50;
    loReset = false;
  }

  if (resReset)
  {
    knobResonant.write(128);
    positionResonant=50;
    resReset = false;
  }

  if (feedbackReset)
  {
    knobFeedback.write(128);
    positionFeedback=50;
    feedbackReset = false; 
  }

  if (distGainReset)
  {
    knobDistGain.write(128);
    positionDistGain=50;
    distGainReset = false; 
  }

 if (distClipReset)
  {
    knobDistClip.write(128);
    positionDistClip=50;
    distClipReset = false; 
  }

  if (volReset)
  {
    knobVol.write(128);
    positionVol=50;
    volReset = false; 
  }

//save encoder positions to EEPROM if button is pressed
 if (writeVals==true)
  {
    //print message and write values to EEPROM
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Saved values:");
    EEPROM.write(1000, positionHi);
    EEPROM.write(1001, positionBand);
    EEPROM.write(1002, positionLo);
    EEPROM.write(1003, positionResonant);
    EEPROM.write(1004, positionFeedback);
    EEPROM.write(1005, positionDistGain);
    EEPROM.write(1006, positionDistClip);
    EEPROM.write(1007, positionVol);

    delay(1000);

    //read back what was written to confirm 
    savedPosHi=EEPROM.read(1000);
    savedPosBand =EEPROM.read(1001);
    savedPosLo =EEPROM.read(1002);
    savedPosResonant =EEPROM.read(1003);
    savedPosFeedback=EEPROM.read(1004);
    savedPosDistGain=EEPROM.read(1005);
    savedPosDistClip =EEPROM.read(1006); 
    savedPosVol=EEPROM.read(1007);


    //display saved values
    lcdLayout(savedPosHi, savedPosBand, savedPosLo,  savedPosVol*100/255, savedPosResonant*100/255, savedPosFeedback*100/255, savedPosDistGain*100/255, savedPosDistClip*100/255);

    delay(2000);

    //go back to displaying the current settings
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Current values:");
    delay(1000);

    lcdLayout(dispHi, dispBand, dispLo, dispVol*100/255, dispResonant*100/255, dispFeedback*100/255, dispDistGain*100/255, dispDistClip*100/255);  
   //and make sure we don't come back here unless we press the button again
    writeVals=false;
  }
}


//gets new value from encoder
void changeEncoder(int *positionEnc, int *newVal, int lowLim, int hiLim, Encoder* ptr)
{
  //get current pot value
  *newVal = ptr->read();

  //set limits to new value to prevent overflows
   if (*newVal>hiLim)
    {*newVal=hiLim;
    *positionEnc = *newVal;
    ptr->write(hiLim);}

    if (*newVal<lowLim)
    {*newVal=lowLim;
    *positionEnc = *newVal;
    ptr->write(lowLim);}

}

//this sends encoder values to digital potentiometers
void changePot(int *newVal, int *positionEnc, int *display, int convertStep, int CSpin, int writeAddr, int readAddr)
{
  int convertRes=0;

  //id the position of the encoder changed
  if (*newVal != *positionEnc )
  {
    //convert new value to a 255 step scale
    convertRes = 255* (*newVal)/convertStep;

    //and update the position of the pot
    *positionEnc = *newVal;
  
    //write to pot  
    digitalPotWrite(CSpin, writeAddr, convertRes);

    //for the case of the resonant filter, which uses CSpin 10, write the same value to BOTH wipers!
    if (CSpin == 10)
     { 
      digitalPotWrite(CSpin, wiper1WAddr, convertRes);
     }

    //read the value that was just written -- will be used on lcd
    *display = digitalPotRead(CSpin, readAddr) ;
    *display = *display *convertStep/255;
    lcdLayout(dispHi, dispBand, dispLo, dispVol, dispResonant, dispFeedback, dispDistGain, dispDistClip);
   // return reading;

  }

  //don't want to keep reading from pot at every loop, assume previous position was valid
  //return 255*(*newVal)/convertStep;

}

// This function takes care of sending SPI data to the pot.
void digitalPotWrite(int CSpin, int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(CSpin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(CSpin,HIGH); 
}

// This function takes care of reading SPI data from the pot.
int digitalPotRead(int CSpin, int address) {
  // take the SS pin low to select the chip:
  digitalWrite(CSpin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  int value = SPI.transfer(0);
 
  // take the SS pin high to de-select the chip:
  digitalWrite(CSpin,HIGH); 
  return value;
}

//simplifying lcd printing:
void lcdLayout(int val1, int val2, int val3, int val4, int val5, int val6, int val7, int val8)
{
  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("                    ");
   lcd.setCursor(0,2);
  lcd.print("                    ");
   lcd.setCursor(0,3);
  lcd.print("                    ");
  
  /******LINE 1******/
  lcd.setCursor(0,0);
  if (val1<10)
  {
   lcd.print("  ");
   lcd.print(val1);
   }
  else if (val1>10 && val1<100)
  {
   lcd.print(" ");
   lcd.print(val1);
  }
  else
  {
   lcd.print(val1);
  }
  lcd.print("%            ");
  
  if (val5<10)
  {
   lcd.print("  ");
   lcd.print(val5);
   }
  else if (val5>10 && val5<100)
  {
   lcd.print(" ");
   lcd.print(val5);
  }
  else
  {
   lcd.print(val5);
  }
  lcd.print("%");

 /*****LINE2*****/
  lcd.setCursor(0,1);
  if (val2<10)
  {
   lcd.print("  ");
   lcd.print(val2);
   }
  else if (val2>10 && val2<100)
  {
   lcd.print(" ");
   lcd.print(val2);
  }
  else
  {
   lcd.print(val2);
  }
  lcd.print("%            ");
  
  if (val6<10)
  {
   lcd.print("  ");
   lcd.print(val6);
   }
  else if (val6>10 && val6<100)
  {
   lcd.print(" ");
   lcd.print(val6);
  }
  else
  {
   lcd.print(val6);
  }
  lcd.print("%");

  /******LINE 3******/
  lcd.setCursor(0,2);
  if (val3<10)
  {
   lcd.print("  ");
   lcd.print(val3);
   }
  else if (val3>10 && val3<100)
  {
   lcd.print(" ");
   lcd.print(val3);
  }
  else
  {
   lcd.print(val3);
  }
  lcd.print("%            ");
  
  if (val7<10)
  {
   lcd.print("  ");
   lcd.print(val7);
   }
  else if (val7>10 && val7<100)
  {
   lcd.print(" ");
   lcd.print(val7);
  }
  else
  {
   lcd.print(val7);
  }
  lcd.print("%");

  /******LINE 4******/
  lcd.setCursor(0,3);
  if (val4<10)
  {
   lcd.print("  ");
   lcd.print(val4);
   }
  else if (val4>10 && val4<100)
  {
   lcd.print(" ");
   lcd.print(val4);
  }
  else
  {
   lcd.print(val4);
  }
  lcd.print("%            ");
  
  if (val8<10)
  {
   lcd.print("  ");
   lcd.print(val8);
   }
  else if (val8>10 && val8<100)
  {
   lcd.print(" ");
   lcd.print(val8);
  }
  else
  {
   lcd.print(val8);
  }
  lcd.print("%");

}


//ISRs -- self explanatory function names
void writeToEEPROM()
{
  writeVals = true;
}

void defaultHi()
{
  hiReset = true; 
}

void defaultBand()
{
 bandReset = true;
}

void defaultLo()
{
 loReset = true;
}

void defaultRes()
{
  resReset = true;
}

void defaultFeedback() 
{
  feedbackReset = true; 
}
  
void defaultDistGain()
{
  distGainReset = true; 
}

void defaultDistClip()
{
  distClipReset = true;
}

void defaultVol()
{
  volReset = true;
}

