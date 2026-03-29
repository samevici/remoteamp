#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc1;           //xy=322.20001220703125,201.1999969482422
AudioOutputUSB           usb1;           //xy=532.2000122070312,202.1999969482422
AudioConnection          patchCord1(adc1, 0, usb1, 0);
AudioConnection          patchCord2(adc1, 0, usb1, 1);
// GUItool: end automatically generated code


