/* Example for analogRead
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/

#include <Wire.h>
#include <ADC.h>
#include <ADC_util.h>
//#include <i2c_driver.h>
//#include <i2c_driver_wire.h>


//const int readPin = A9; // ADC0
const int readPin2 = A2; // ADC1
const uint8_t MCP4725Address = 0x60;

ADC *adc = new ADC(); // adc object;

class tMCP4725
  { private:
      uint8_t _i2cAddr;
    public:
      void Setup(uint8_t Addr);
      void SetVoltage(uint16_t DesiredOutput, bool WriteEEPROM);
      void SetVoltage(uint16_t DesiredOutput) { SetVoltage(DesiredOutput, false); }
  }; // tMCP4725

void tMCP4725::Setup(uint8_t Addr)
  { _i2cAddr = Addr;
    Wire.begin();
  } // tMCP4725::Setup

void tMCP4725::SetVoltage(uint16_t DesiredOutput, bool WriteEEPROM )
  { // @param[in]  DesiredOutput
    //             The 12-bit value representing the relationship between
    //             the DAC's input voltage and its output voltage.
    // @param[in]  WriteEEPROM
    //             If this value is true, 'output' will also be written
    //             to the MCP4725's internal non-volatile memory, meaning
    //             that the DAC will retain the current voltage output
    //             after power-down or reset.

    const int cmdWriteDAC          = 0x40;
    const int cmdWriteDACandEEPROM = 0x60;

    Wire.beginTransmission(_i2cAddr);
    if (WriteEEPROM)
      { Wire.write(cmdWriteDACandEEPROM); }
    else
      { Wire.write(cmdWriteDAC); }

    Wire.write(DesiredOutput / 16);        // Upper data bits (D11..D4)
    Wire.write((DesiredOutput % 16) << 4); // Lower data bits (D3..D0.x.x.x.x)
    Wire.endTransmission();
  } // tMCP4725::SetVoltage
tMCP4725 MCP4725;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    //pinMode(readPin, INPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(9600);

    Serial.println("Begin setup");
    MCP4725.Setup(MCP4725Address);

    ////// ADC1 /////
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(16); // set number of averages
    adc->adc1->setResolution(12); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    #endif

    Serial.println("End setup");

}

int value;
int value2;

void loop() {


    //#ifdef ADC_DUAL_ADCS
    value2 = adc->adc1->analogRead(readPin2);

    Serial.print("Pin: ");
    Serial.print(readPin2);
    Serial.print(", value ADC1: ");
    Serial.println(value2*3.3/adc->adc1->getMaxValue(), DEC);
    //#endif

    MCP4725.SetVoltage(value2);  
        delay(1000);

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
