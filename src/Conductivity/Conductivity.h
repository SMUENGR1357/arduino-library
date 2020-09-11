
/**
 * Provides a reading of the conductivity probe.
 *
 * The full documentation for building and calibrating your conductivity probe can
 * be found on Canvas.
 *
 * Your probe must be connected to the following pins:
 * <ul>
 *   <li>Digital Pin 12</li>
 *   <li>Digital Pin 13</li>
 *   <li>Analog Pin 2</li>
 *   <li>Analog Pin 3</li>
 * </ul>
 *
 * The raw returned value is a range between [0 - 1023],
 * denoting the absolute difference between analog pins 2 and 3. You are required
 * to calibrate your sensor by reading sensor values at various salinity levels and then
 * generating an interpolating function. This function can then be used to convert
 * new values while your machine is performing its tasks. The more data points, the
 * more likely your function is accurate. It is also very likely that you will have
 * to periodically recalibrate your sensor. Rebuilding your circuit (for example,
 * changing the resistors or fixing a broken connection) will require you to
 * recalibrate.
 *
 * @return int A value between [0 - 1023] telling you the raw analog pin reading.
 */
int getConductivity() {
    const unsigned long seconds = 3;

    const int conductivityDigitalPin1 = 12;
    const int conductivityDigitalPin2 = 13;
    const int conductivityAnalogPin1 = 2;
    const int conductivityAnalogPin2 = 3;

    int reading1, reading2, result;

    // One period of the wave is 10ms. So we want to
    // repeat it numMilliseconds/10ms times
    unsigned long loopCount = (seconds)*100ul;

    pinMode(conductivityDigitalPin1, OUTPUT);
    pinMode(conductivityDigitalPin2, OUTPUT);
    digitalWrite(conductivityDigitalPin1, HIGH);
    digitalWrite(conductivityDigitalPin2, HIGH);

    // We make an alternating-phase square wave out of digital pins 12/13
    // For this to work, we needed simultaneous digital pin writes. Refer to
    // http://www.arduino.cc/en/Reference/PortManipulation
    for (unsigned long i = 0; i < loopCount; ++i)
    {
        // The AND turns off pin 13, OR turns on pin 12
        PORTB = B01000000 | (PORTB & B01111111);
        delay(5);

        // AND turns off pin 12, OR turns on pin 13
        PORTB = B10000000 | (PORTB & B10111111);
        delay(5);
    }

    reading1 = analogRead(conductivityAnalogPin1);
    reading2 = analogRead(conductivityAnalogPin2);
    digitalWrite(conductivityDigitalPin1, LOW);
    digitalWrite(conductivityDigitalPin2, LOW);

    return result = abs(reading1 - reading2);
}