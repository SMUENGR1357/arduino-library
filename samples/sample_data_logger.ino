/**
 * This file accomplishes two main objectives:
 * 
 * - Enter a data logging mode
 * - Enter a data retrieval mode
 * 
 * To enter data logging mode, leave digital pin 2 open. This will then
 * do the following:
 * 
 * - Reset the number of samples taken back down to 0
 * - Wait 1 minute
 * - Read analog pin 0 and store it in an integer variable. Remember that
 * analog pin values have a range between 0 and 1023.
 * - Write that value into EEPROM in the next available location
 * - Repeat at step 2, looping indefinitely.
 * 
 * To enter data retrieval mode, connect the 5V pin to digital pin 2. This
 * will then do the following:
 * 
 * - Connect to the serial port
 * - For each value that has been recorded in EEPROM using data logging mode,
 * print out the value
 * - The arduino will then stop.
 * 
 * This is meant as a starter program. You will need to modify this to be able
 * to store things like conductivity readings, motor placements, or whatever
 * else your heart desires.
 */

#include <eepromhelper.h>
#include <Sleep_n0m1.h>

Sleep sleep;
unsigned char mode;
unsigned int seconds = 0;

void setupDataLoggingMode() {
  resetSampleCount();
}

void showContentsOfEEPROM() {
  unsigned int i = 0;
  unsigned int count = 0;
  unsigned int address;
  int analogPinReading;

  Serial.begin(9600);  // open serial port to print to the serial console
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

void gatherAndStoreAnalogReading() {
  int analogPinReading;
  sleep.sleepDelay(1000);  // delay 1 second in low power mode
  seconds++;
  if (seconds >= 60) {  // test if one minute has elapsed since last sample
    seconds = 0;

    // Get a "sample". This can be replaced with some other function that provides
    // data that you want to store (e.g. getConductivity())
    analogPinReading = analogRead(0);
    appendIntValueToEEPROM(analogPinReading); // Write to EEPROM
  }
}

// This function is called ONCE when your arduino first gets power
void setup() {
  sleep.pwrDownMode(); //set sleep mode
  pinMode(2,INPUT_PULLUP); // See if digital pin 2 is set to high
  mode = digitalRead(2);

  if (mode == LOW) setupDataLoggingMode(); // data logging mode
  else showContentsOfEEPROM(); // data retrieval mode
}

// This function is called REPEATEDLY, after setup() is done
void loop() {
  if (mode == LOW) gatherAndStoreAnalogReading(); // data logging mode
  // else Do Nothing, because we've already retrieved the contents
}