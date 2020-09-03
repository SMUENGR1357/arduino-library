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