
#ifndef INFRARED_SENSOR_H
#define INFRARED_SENSOR_H
// IR DETAILS
#define EVENT_RISING 1
#define EVENT_FALLING 2
#define EVENT_NONE 0
#define IR_WAIT 100000

// Instance variables used in conjunction with the IR sensor
unsigned char necState;
int num_chars;
unsigned long prev_time;

unsigned char IRChar, IRCharBitMask, buffer[8];
bool receiverState = false;
unsigned long cur_time, ticks;

/**
 * This uses the IR sensor at the given digital pin to scan for beacons that
 * may be nearby.
 *
 * This function uses the IR sensor to scan for IR signals coming from the beacons.
 * The values that it reads are stored in an internal character buffer, which you
 * can access using getIR() after running this function.
 *
 * <b>Note</b>: the function will return -1 if the pin passed for scanIR() is invalid
 * 
 * <b>Note</b>: every time you want to get fresh values from getIR(), you have to
 * run this function first.
 *
 * @param pin The digital pin number that the IR sensor is connected to.
 * @return int The number of characters that were read in this given scan.
 *
 * Example code:
 *
 * @code
 * #include <infraredsensor.h>
 * 
 * int IRSensorPin = 20;
 * int numCharsReadFromIR = scanIR(IRSensorPin);
 * myRobot->printLCD("Chars read: ");
 * myRobot->printLCD(charactersReadFromIR);
 *
 * // Now print the reading on the next line of the LCD
 * char* IRCharacters = getIR();
 * myRobot->moveCursor(0, 1);
 * myRobot->printLCD(IRCharacters);
 * @endcode
 */
int scanIR(int pin)
{
    if (pin != -1)
    {
        // takes 13 ms per char to broadcast from a beacon
        char reading;
        unsigned char event;
        // reset the buffer
        memset(buffer, 0, sizeof(buffer));
        num_chars = 0;

        unsigned long IRCounter = 0;

        // Cycle while we try to watch for a character.
        while (IRCounter < IR_WAIT)
        {
            // Digital level from IR receiver will be inverted
            if (digitalRead(pin))
            {
                if (receiverState)
                    event = EVENT_FALLING;
                else
                    event = EVENT_NONE;
                receiverState = false;
            }
            else
            {
                if (!receiverState)
                    event = EVENT_RISING;
                else
                    event = EVENT_NONE;
                receiverState = true;
            }
            if (event != EVENT_NONE)
            {
                cur_time = micros();
                ticks = cur_time - prev_time;
                if (necState == 0)
                { // Expecting rising edge of leading pulse
                    if (event == EVENT_RISING)
                    {
                        necState = 1;
                        // digitalWrite(ledPin,HIGH);
                    }
                }
                else if (necState == 1)
                { // Expecting falling edge of leading pulse
                    if (event == EVENT_FALLING)
                    {
                        if (ticks > 8900L)
                        {
                            necState = 2; // Check for leading pulse > 8.9msec
                        }
                        else
                        { // Stray short pulse found, reset NEC state
                            // digitalWrite(ledPin,LOW);
                            necState = 0;
                        }
                    }
                }
                else if (necState == 2)
                { // Expecting rising edge of first pulse after leading pulse
                    if (event == EVENT_RISING)
                    {
                        if (ticks > 3375L)
                        { // Check for space after leading pulse > 3.375 msec
                            IRCharBitMask = 0x80;
                            IRChar = 0;
                            necState = 3;
                        }
                        else
                        { // Space too short, reset NEC state to wait for another leading pulse
                            // digitalWrite(ledPin,LOW);
                            necState = 0;
                        }
                    }
                }
                else if (necState == 3)
                { // Expecting falling edge of data pulse
                    if (event == EVENT_FALLING)
                    {
                        if (ticks < 648)
                        {
                            necState = 4; // Check if data pulse width < 648 usec
                        }
                        else
                        { // Width too short, reset NEC state to wait for another leading pulse
                            // digitalWrite(ledPin,LOW);
                            necState = 0;
                        }
                    }
                }
                else if (necState == 4)
                { // Expecting rising edge of pulse after data pulse
                    if (event == EVENT_RISING)
                    {
                        if (ticks > 1120)
                        { // Record a '1' bit for space > 1120 usec
                            IRChar = IRChar | IRCharBitMask;
                        }
                        IRCharBitMask = IRCharBitMask >> 1;

                        if (IRCharBitMask == 0)
                        {                               // Check if eighth bit received and character complete (!!!!!)
                            buffer[num_chars] = IRChar; // Record complete character received in circular output buffer
                            num_chars++;
                            // buffer_in = (buffer_in + 1) & 0x07;
                            // digitalWrite(ledPin,LOW);
                            necState = 0; // Reset NEC state to wait for another leading pulse
                        }
                        else
                        {
                            necState = 3; // Wait for falling edge of data pulse
                        }
                    }
                }
                prev_time = cur_time;
            }
            IRCounter++;
        }
        // Serial.println(output);
        return num_chars;
    }
    return -1; // this is for incorrect ID
}

/**
 * This provides the characters that were reading from the most recent call to scanIR().
 *
 * This function should be used in conjunction with scanIR(). Every time you want a fresh
 * IR reading, call scanIR() first. Then, call this function, storing the result in a char*
 * variable. If you don't call scanIR() before calling this, the internal character buffer
 * will not refresh and this will continuously provide the same value (or an empty value
 * if you never ran scanIR() in the first place).
 *
 * @return char* A character buffer containing the characters read from the most recent
 * call to scanIR()
 *
 * Example code:
 *
 * @code
 * #include <infraredsensor.h>
 * 
 * int charactersReadFromIR = scanIR(IRSensorId);
 * myRobot->printLCD("Chars read: ");
 * myRobot->printLCD(charactersReadFromIR);
 *
 * // Now print the reading on the next line of the LCD
 * char* IRCharacters = getIR();
 * myRobot->moveCursor(0, 1);
 * myRobot->printLCD(IRCharacters);
 * @endcode
 */
unsigned char *getIR()
{
    return buffer;
}

#endif // INFRARED_SENSOR_H