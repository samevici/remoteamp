/*
  Modification of code by Matthew McMillan
  @matthewmcmillan
  http://matthewcmcmillan.blogspot.com
  Created 12 Mar 2014
  Digital Pot Control (MCP4251)
  
 
 The circuit:
     * VSS - to GND
     * VDD - to +5v
     * SHDN - to digital pin 7 and a 4.7k pull down resistor
     * CS - to digital pin 10  (SS pin)
     * SDI - to digital pin 11 (MOSI pin)
     * SDO - to digital pin 12 (MISO pin)
     * CLK - to digital pin 13 (SCK pin)
 
*/

// include the SPI library:
#include <SPI.h>

// set chip select pins the digital pots:
const int CSPin00 = 10;
const int CSPin01 = 37;
//const int SHDNPin = 7;

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

int result;
int res = 101;
int convertRes = 255;
char begin = '!';
int selection = 3;
int reading = 0;
int potSelect = 3;
int CSSelect = 0;

void setup() {
  // set the CS pins as outputs:
  pinMode (CSPin00, OUTPUT);
  pinMode (CSPin01, OUTPUT);
  // set the shutdownPin as an output:
  //inMode (SHDNPin, OUTPUT);
  // start with all the pots shutdown
  //digitalWrite(shutdownPin,LOW);
  // initialize SPI:
  SPI.begin(); 

  //Serial.begin instruction is ignored by teensy!
  Serial.println("Press 'o' to begin");  

}


void loop() {

//wait before checking inputs
delay(3000);

//if serial available
if (Serial.available()) {

  //read and check for beginning signal
  begin = Serial.read();
  
  if (begin == 'o')
    {

  //select which pot to control (pot0 or pot1)
    Serial.print("Select pot: ");

    //wait for input from serial
    while (!(potSelect = Serial.parseInt()))
     {}
    //print choice to serial monitor
    Serial.println(potSelect);

    if (potSelect == 1) {
      CSSelect = CSPin00;
    }

    if (potSelect == 2) {
     CSSelect = CSPin01;
    }
    

    //select which wiper to control (pot0 or pot1)
    Serial.print("Select wiper: ");

    //wait for input from serial
    while (!(selection = Serial.parseInt()))
     {}
    //print choice to serial monitor
    Serial.println(selection);

    //configure potentiometer setting
    Serial.print("enter %: ");

    //wait for input from serial
    while (!(res = Serial.parseInt()))
     {}

    //print choice to serial monitor & convert to binary
    Serial.println(res);
    convertRes = 255*res/100;
    
    //send calculated value to appropriate potentiometer
    if (selection == 1) {
      digitalPotWrite(CSSelect, wiper0WAddr, convertRes);
      
    }

    if (selection == 2) {
      digitalPotWrite(CSSelect, wiper1WAddr, convertRes);
    }
    
    //reprint for confirmattion
    Serial.print("Pot: ");
    Serial.println(selection);
    Serial.print("Read resistance in byte: ");

    if (selection == 1) {
      reading = digitalPotRead(CSSelect, wiper0RAddr);
    }

    if (selection == 2) {
      reading = digitalPotRead(CSSelect, wiper1RAddr);
    }
    Serial.println(reading);
    
    }

  //reset variables to invalid values and re-prompt
  begin = '!';
  selection = 3;
  res = 101;
  potSelect = 3;
  Serial.println("Press 'o' to begin");  
  while(Serial.available()){Serial.read();} //flush incomming Serial buffer

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