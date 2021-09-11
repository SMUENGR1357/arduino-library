
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

// ******************************************* //
// Function to read IR character from sensor.
// ******************************************* //
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

unsigned char *getIR()
{
    return buffer;
}

#endif // INFRARED_SENSOR_H