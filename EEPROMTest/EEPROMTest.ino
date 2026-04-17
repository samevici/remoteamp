#include <EEPROM.h>

// arbitrary address and byte to save
uint address = 200;
uint value=245;
int newValue;

//for ISR
volatile bool writeVals = false;

void setup()
{
  //button setup
  pinMode(6, INPUT_PULLUP);

   //wait for initializations before ISR
  delay(5000);
  
  //attach interrupt
  attachInterrupt(digitalPinToInterrupt(6), resetVal, FALLING);
  Serial.begin(9600);  
}


void loop()
{
   if (writeVals==true)
  {

    Serial.println("pressed");
    EEPROM.write(address, value);
    writeVals=false;
  }

  // read a byte from the current address of the EEPROM
  newValue = EEPROM.read(address);
  
  Serial.print(address);
  Serial.print("\t");
  Serial.print(newValue, DEC);
  Serial.println();
  

  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
  // on address 4248 (max Teensy 4.1 capacity), wrap around to address 0
  //if (address == 4284)
  //  address = 0;
  // Teensy 1.0 has 512 bytes
  // Teensy 2.0 has 1024 bytes
  // Teensy++ 1.0 has 2048 bytes
  // Teensy++ 2.0 has 4096 bytes
    
  delay(1500);
}

void resetVal()
{
  writeVals = true;
}
