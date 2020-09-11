#include <EEPROM.h>
/**
 * @file eepromhelper.h
 * 
 * This is an optional helper file that can read / write to the Arduino's EEPROM on-board
 * memory.
 * 
 * The Arduino EEPROM is essentially a "tiny hard drive": you can read and write data that's
 * stored on the arduino, and this data will persist even if you unplug the Arduino from its
 * source of power. This is different from variables that are defined in code: every time
 * your arduino is removed from power those variables are effectively "reset" back to their
 * original values. With EEPROM, however, you can persist these values across power cycles.
 * To find out more (as well as what functions the raw Arduino library lets you use), you
 * can refer to <a href="https://www.arduino.cc/en/Reference/EEPROM">this page</a> for more details.
 * In particular, it is recommended that you look at the get() and put() functions: those
 * allow you to read and write to the EEPROM, respectively.
 * 
 * The functions in this helper file let you write integer values to and from EEPROM. However,
 * that may not be sufficient in your programs. You may need to read / write additional
 * data, or data of different types (e.g. floats, strings, etc.). You can use the source
 * code of these functions to help get you started. The code for these functions can be
 * found at <a href="https://github.com/SMUKNW2300/arduino-library/blob/master/src/eepromhelper/eepromhelper.h">this publicly available repository</a>.
 * That, in tandem with the get() and put() functions from the arduino site linked above,
 * will provide you with the information needed to store custom data.
 * 
 * The functions in this file can be used by writing the following include statement at the
 * top of your code:
 * 
 * @code
 * #include <eepromhelper.h>
 * @endcode
 * 
 * You can then use these functions to read / write with the EEPROM. In particular, refer
 * to the <a href="https://github.com/SMUKNW2300/arduino-library/blob/master/samples/sample_data_logger.ino">sample file on the github repository</a>
 * for an introduction on how to use this library to store / retrieve data
 * 
 */

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
 * Function to set the number of data samples that have been recorded onto the
 * EEPROM. Note that this does not actually verify that there are that many
 * samples stored in EEPROM; it is up to the programmer to verify that the number
 * is indeed accurate
 * 
 * @param sampleCount The number of samples to be recorded in EEPROM
 * @returns The same sampleCount value that was passed in.
 */
unsigned int setCountOfDataSamples(unsigned int sampleCount) {
  EEPROM.put(0, sampleCount);
  return sampleCount;
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
 * counter value on your own using setCountOfDataSamples(unsigned int).
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