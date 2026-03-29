/*AnalogRead example modified to only test pin A2*/
#include <ADC.h>
#include <ADC_util.h>

const int readPin2 = A2; // ADC1

ADC *adc = new ADC(); // adc object;

void setup() {

    //set pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(9600);
    Serial.println("Begin setup");


    ////// ADC1 setup (A2)/////
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(16); // set number of averages
    adc->adc1->setResolution(16); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    #endif

    Serial.println("End setup");

}

int value;

void loop() {

    // Single read
    value = adc->adc1->analogRead(readPin2); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

    //print result and convert to voltage level
    Serial.print("Pin: ");
    Serial.print(readPin2);
    Serial.print(", value ADC1: ");
    Serial.println(value*3.3/adc->adc1->getMaxValue(), DEC);


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

    delay(50);
}

// If you enable interrupts make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
}
