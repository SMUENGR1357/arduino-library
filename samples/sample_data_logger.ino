#include <EEPROMHelper.h>
#include <Sleep_n0m1.h>

Sleep sleep;
unsigned char mode;
unsigned int count = 0;
unsigned int seconds = 0;
int analogPinReading;


void setup() {
  unsigned int i = 0;
  unsigned int address;
  
  sleep.pwrDownMode(); //set sleep mode
  seconds = 0;
  pinMode(2,INPUT_PULLUP);
  mode = digitalRead(2);
  if (mode == LOW) {  // data logging mode
    resetSampleCount();
  }
  else {  // data retrieval mode
    Serial.begin(9600);  // open serial port
    delay(1000);
    count = getCountOfDataSamples();
    while (i < count) {  // loop over all logged data samples
      address = (BYTES_PER_INTEGER * i) + BYTES_FOR_SAMPLE_COUNTER;  // calculate EEPROM address of next sample
      analogPinReading = getIntValueAtAddress(address);  // retrieve ADC code of next sample
      Serial.print(i);  // report sample index and ADC value up serial port
      Serial.print(':');
      Serial.println(analogPinReading);
      i++;
    }
  }
}

void loop() {
  if (mode == LOW) {  // data logging mode
    sleep.sleepDelay(1000);  // delay 1 second in low power mode
    seconds++;
    if (seconds >= 60) {  // test if one minute has elapsed since last sample
      seconds = 0;
      analogPinReading = analogRead(0);  // sample ADC code
      appendIntValueToEEPROM(analogPinReading);
    }
  }
  else {  // data retrieval mode
  }
}