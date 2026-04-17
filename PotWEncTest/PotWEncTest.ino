// include the SPI library:
#include <SPI.h>
#include <Encoder.h>

// set chip select pins the digital pots:
const int CSPin00 = 10;
const int CSPin01 = 9;
const int CSPin02 = 8;
const int CSPin03 = 7;
const int CSPin04 = 6;

//save important addresses
const int wiper0WAddr = B00000000;
const int wiper1WAddr = B00010000;
const int wiper0RAddr = B00001100;
const int wiper1RAddr = B00011100;
const int tconWAddr = B01000000;
const int tcon_0off_1on = B11110000;
const int tcon_0on_1off = B00001111;
const int tcon_0off_1off = B00000000;
const int tcon_0on_1on = B11111111;

//initialize encoder objects
Encoder knobHi(1,2);
Encoder knobBand(4, 5);
Encoder knobLo(25,26);
Encoder knobResonant(28,29);
Encoder knobFeedback(31,32);
Encoder knobDistGain(34,35);
Encoder knobDistClip(37,38);
Encoder knobVol(40,41);

void setup() {
  // set the CS pins as outputs:
  pinMode (CSPin00, OUTPUT);
  pinMode (CSPin01, OUTPUT);
  pinMode (CSPin02, OUTPUT);
  pinMode (CSPin03, OUTPUT);
  pinMode (CSPin04, OUTPUT);
  
// initialize SPI:
  SPI.begin(); 
}

int positionHi, positionBand, positionLo, positionResonant, positionFeedback, positionDistGain, positionDistClip, positionVol;
int newHi, newBand, newLo, newResonant, newFeedback, newDistGain, newDistClip, newVol;

//void changeEncoder(int *positionEnc, int *newVal, int lowLim, int hiLim, Encoder* ptr);


void loop() {

  newHi = knobHi.read();
  newBand = knobBand.read();
  newLo = knobLo.read();
  newResonant = knobResonant.read();
  newFeedback = knobFeedback.read();
  newDistGain = knobDistGain.read();
  newDistClip = knobDistClip.read();
  newVol = knobVol.read();

  changeEncoder(&positionHi, &newHi, 0, 100, &knobHi);
  changeEncoder(&positionBand, &newBand, 0, 100, &knobBand);
  changeEncoder(&positionLo, &newLo, 0, 100, &knobLo);
  changeEncoder(&positionResonant, &newResonant, 0, 255, &knobResonant);
  changeEncoder(&positionFeedback, &newFeedback, 0, 255, &knobFeedback);
  changeEncoder(&positionDistGain, &newDistGain, 0, 255, &knobDistGain);
  changeEncoder(&positionDistClip, &newDistClip, 0, 255, &knobDistClip);
  changeEncoder(&positionVol, &newVol, 0, 255, &knobVol);


  changePot(&newHi, &positionHi,  100, CSPin02, wiper0WAddr, wiper0RAddr);
  changePot(&newBand, &positionBand,  100, CSPin01, wiper0WAddr, wiper0RAddr);
  changePot(&newLo, &positionLo,  100, CSPin02, wiper1WAddr, wiper1RAddr);
  changePot(&newResonant, &positionResonant,  255, CSPin00, wiper0WAddr, wiper0RAddr);
  changePot(&newFeedback, &positionFeedback,  255, CSPin01, wiper1WAddr, wiper1RAddr);
  changePot(&newDistGain, &positionDistGain,  255, CSPin03, wiper0WAddr, wiper0RAddr);
  changePot(&newDistClip, &positionDistClip,  255, CSPin03, wiper1WAddr, wiper1RAddr);
  changePot(&newVol, &positionVol,  255, CSPin04, wiper1WAddr, wiper1RAddr);


}



void changeEncoder(int *positionEnc, int *newVal, int lowLim, int hiLim, Encoder* ptr)
{
    *newVal = ptr->read();

   if (*newVal>hiLim)
    {*newVal=hiLim;
    *positionEnc = *newVal;
    ptr->write(hiLim);}

    if (*newVal<lowLim)
    {*newVal=lowLim;
    *positionEnc = *newVal;
    ptr->write(lowLim);}

}


void changePot(int *newVal, int *positionEnc,  int convertStep, int CSpin, int writeAddr, int readAddr)
{
  int convertRes=0;

  if (*newVal != *positionEnc )
  {
    Serial.print("current Pot= ");
    Serial.print(*newVal);
    Serial.println();
    convertRes = 255* (*newVal)/convertStep;
    *positionEnc = *newVal;
  
    //write to pot & confirm binary value   
    digitalPotWrite(CSpin, writeAddr, convertRes);
    if (CSpin == 10)
     { 
      digitalPotWrite(CSpin, wiper1WAddr, convertRes);
     }

    Serial.print("Read resistance in byte: ");
    int reading = digitalPotRead(CSpin, readAddr);
    Serial.println(reading);
  }
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