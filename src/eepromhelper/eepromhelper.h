#include <EEPROM.h>

/**
 * Constant that is used to keep track of how many bytes the sample counter uses
 */
const unsigned int BYTES_FOR_SAMPLE_COUNTER = sizeof(unsigned int);

/**
 * Constant that is used to keep track of how many bytes an integer uses (for pin readings)
 */
const unsigned int BYTES_PER_INTEGER = sizeof(int);

/**
 * Provides the number of data samples that have been recorded on the EEPROM.
 * This value is stored in the first few bytes of the EEPROM. The exact number
 * of bytes is stored in BYTES_FOR_SAMPLE_COUNTER.
 * 
 * @returns an integer containing the number of values that have been stored in EEPROM
 */
unsigned int getCountOfDataSamples() {
  unsigned int dataSampleCount = 0;
  EEPROM.get(0, dataSampleCount);
  return dataSampleCount;
}

/**
 * Increments the number of samples that have been stored on the EEPROM, and stores
 * it back into the appropriate spot in the EEPROM. Note that this is ONLY the increment
 * function, and does not actually store a new value (see setIntValueAtAddress() or
 * appendIntValueToEEPROM(int) )
 * 
 * @returns an integer containing the new sample count
 */
unsigned int incrementCountOfDataSamples() {
  unsigned int newDataSampleCount = getCountOfDataSamples() + 1;
  EEPROM.put(0, newDataSampleCount);
  return newDataSampleCount;
}

/**
 * This resets the sample counter to 0. Note that it <b>DOES NOT CLEAR THE EEPROM</b>,
 * only resets the counter <i>as if it were cleared</i>. Subsequent calls to
 * setIntValueAtAddress() or appendIntValueToEEPROM(int) will overwrite the data 
 * that may or may not have been set in the EEPROM 
 * 
 * @returns a value of 0 always
 */
int resetSampleCount() {
  unsigned int zero = 0;
  EEPROM.put(0, 0);
  return zero;
}

/**
 * This function provides the int value that's stored at the given EEPROM
 * address.
 * 
 * @param address The address to pull an integer value from
 * @returns The int value that's stored at the given address
 */
int getIntValueAtAddress(unsigned int address) {
  int value = 0;
  EEPROM.get(address, value);
  return value;
}

/**
 * This function writes an int value to the given address.
 * <b>NOTE:</b>This function <b>DOES NOT</b> increment the sample counter value.
 * You will either need to call incrementCountOfDataSamples() or manage the
 * counter value on your own.
 * 
 * @param address The location on the EEPROM memory space to save an integer into
 * @param value The integer value to store at the given address
 * @returns the int value that was passed in.
 */
int setIntValueAtAddress(unsigned int address, int value) {
  EEPROM.put(address, value);
  return value;
}

/**
 * This function saves an int value to the next available spot on the EEPROM.
 * Behind the scenes this does the following:
 * 
 * - Find the next available address based on the number of samples that have already been saved
 * - Save the value at that location
 * - Increment the number of data samples that have been saved
 * 
 * @param value The integer value to be saved in EEPROM
 * @return The address of the EEPROM that the value was saved to
 */
int appendIntValueToEEPROM(int value) {
  unsigned int nextAvailableAddress = (getCountOfDataSamples() * BYTES_PER_INTEGER) + BYTES_FOR_SAMPLE_COUNTER;
  setIntValueAtAddress(nextAvailableAddress, value);
  incrementCountOfDataSamples();
  return nextAvailableAddress;
}