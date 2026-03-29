#include <Wire.h>
#include <ADC.h>
#include <ADC_util.h>
#include <Adafruit_MCP4725.h>

//initialize objects
Adafruit_MCP4725 dac;

#define DAC_RESOLUTION(12)

ADC *adc = new ADC(); // adc object;

//define A2 pin
const int readPin2 = A2; // ADC1

void setup() {

    //pin setup
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(9600);

    Serial.println("Begin setup");

    //explicitly set I2C bus
    Wire.setSDA(18);
    Wire.setSCL(19);
    dac.begin(0x62);

    ////// ADC1 setup (A2) /////
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(16); // set number of averages
    adc->adc1->setResolution(12); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    #endif

    Serial.println("End setup");

}

int value;

void loop() {

    //read from ADC
    value = adc->adc1->analogRead(readPin2);

    Serial.print("Pin: ");
    Serial.print(readPin2);

    //print value to be sent to DAC
    //Serial.print(", binary value ADC1: ");
    //Serial.println(value);

    //value in volts for comparison
    Serial.print(", value ADC1: ");
    Serial.println(value*3.3/adc->adc1->getMaxValue(), DEC);

    //send value out to DAC and do NOT save to EEPROM
    dac.setVoltage(value, false); 

    // Print errors, if any.
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
    }
    #ifdef ADC_DUAL_ADCS
    if(adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
    }
    #endif

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    //hold for 3 seconds
    delay(3000);

}

// If you enable interrupts make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
}
