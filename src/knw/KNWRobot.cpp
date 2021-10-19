// Copyright 2019 Southern Methodist University

/*
  ---------------------------------
  |        KNW Robot Library      |
  ---------------------------------
  KNWRobot.cpp - Library written to control robots for SMU KNW 2300
  Controling Arduino: MEGA 2560
  Author: Alexandria Hancock
  Other Contributors: Morgan VandenBerg
  
  RE-Written Fall 2021 By: Christian Gould, Zach Suzuki

  For more details on functions, look in KNWRobot.h
*/

#include "KNWRobot.h"
#include "Arduino.h"

#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Keypad.h"
#include "Adafruit_PWMServoDriver.h"
#include "NewPing.h"

// PCA DETAILS (Calibrated by Prof Matt Saari)
// Configuration parameters for each type of motor.
// This uses +/- 120 trying to get, but doesn't map directly
// because of variances in the physical 180 servos.
#define PCA_SERVO_180_INPUT_RANGE 120
#define PCA_SERVO_180_CENTER 400
// Configured to be greater than +/- 90deg (240ish for 90degrees),
// because the students will need to calibrate the servos.
#define PCA_SERVO_180_WIDTH 320

#define PCA_SERVO_CONTINUOUS_INPUT_RANGE 90
#define PCA_SERVO_CONTINUOUS_CENTER 334
#define PCA_SERVO_CONTINUOUS_WIDTH 35

#define PCA_DC_INPUT_RANGE 1023
#define PCA_DC_CENTER 337
#define PCA_DC_WIDTH 183

// Calculations based on above configuration.
#define PCA_SERVO_180_MIN PCA_SERVO_180_CENTER - PCA_SERVO_180_WIDTH
#define PCA_SERVO_180_MAX PCA_SERVO_180_CENTER + PCA_SERVO_180_WIDTH
#define PCA_SERVO_CONTINUOUS_MIN \
    PCA_SERVO_CONTINUOUS_CENTER - PCA_SERVO_CONTINUOUS_WIDTH
#define PCA_SERVO_CONTINUOUS_MAX \
    PCA_SERVO_CONTINUOUS_CENTER + PCA_SERVO_CONTINUOUS_WIDTH
#define PCA_DC_MIN PCA_DC_CENTER - PCA_DC_WIDTH
#define PCA_DC_MAX PCA_DC_CENTER + PCA_DC_WIDTH

// IR DETAILS
#define EVENT_RISING 1
#define EVENT_FALLING 2
#define EVENT_NONE 0
#define IR_WAIT 100000

// ******************************************* //
// KNWRobot Constructor
// ******************************************* //
KNWRobot::KNWRobot(long lcdAddress = 0x27) // address can also be 0x3F
{
    // Set pointers to null to avoid seg fault on reset calls
    lcd = nullptr;
    pwm = nullptr;
    keypad = nullptr;

    // analog pins that can't be used
    for (int i = 0; i < 16; i++)
    {
        if (i >= 2 && i < 4)
        {
            analogPins[i] = true;
        }
        else
        {
            analogPins[i] = false;
        }
    }

    // digital pins that can't be used (tx,rx)
    memset(digitalPins, 0, sizeof(digitalPins));
    int commPins[] = {0, 1, 14, 15, 16, 17, 18, 19, 20, 21}; // 10 no-nos
    for (int i = 0; i < 10; i++)
    {
        digitalPins[commPins[i]] = true;
    }

    // pins that can't be used because of the Conductivity
    digitalPins[12] = true;
    digitalPins[13] = true;
    analogPins[2] = true;
    analogPins[3] = true;

    memset(pcaPins, 0, sizeof(pcaPins));

    setupKeypad();
    setupLCD(lcdAddress);
    setupPWM();
    setupSensors();
    setupIR();
}

KNWRobot::~KNWRobot()
{
    delete keypad;
    delete lcd;
    delete pwm;
}

// ******************************************* //
// (Private) Component Setup Functions
// ******************************************* //
void KNWRobot::setupKeypad()
{
    // setting up keypad
    entered = false;
    numEntered = 0;
    keypad = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

    for (int i = 0; i < ROWS; i++)
    {
        digitalPins[rowPins[i]] = true;
    }

    for (int i = 0; i < COLS; i++)
    {
        digitalPins[colPins[i]] = true;
    }
}

void KNWRobot::setupLCD(long lcdAddress)
{
    // setting up LCD
    lcd = new LiquidCrystal_I2C(lcdAddress, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
    lcd->begin(16, 2); // initialize the lcd
    lcd->home();       // go to the top line
    lcd->print("SMU Lyle KNW2300");
}

void KNWRobot::setupPWM()
{
    // setting up PWM board
    pwm = new Adafruit_PWMServoDriver();
    pwm->begin();
    pwm->setPWMFreq(60); // Analog servos run at ~60 Hz updates
}

void KNWRobot::setupSensors()
{
    // setting up Sensors
    numPings = 0;
    numMotors = 0;
    numServos = 0;
    numBumps = 0;
    inclinePin = -1;
    tempPin = -1;
    numIR = 0;
}

void KNWRobot::setupIR()
{
    // setting up IR handling
    necState = 0;
    prev_time = 0;
    num_chars = 0;
}

// ******************************************* //
// (Public) Component Reset Functions
// ******************************************* //

void KNWRobot::resetKeypad()
{
    delete keypad;
    keypad = nullptr;
    setupKeypad();
}

void KNWRobot::resetLCD(long lcdAddress)
{
    delete[] lcd;
    lcd = nullptr;
    setupLCD(lcdAddress);
}

// ******************************************* //
// Pin Mapping Functions
// ******************************************* //
bool *KNWRobot::getAnalogPins()
{
    return analogPins;
}

bool *KNWRobot::getDigitalPins()
{
    return digitalPins;
}

bool *KNWRobot::getPCAPins()
{
    return pcaPins;
}

bool KNWRobot::checkPin(int pin, char type)
{
    if (type == 'a')
    { // analog pins
        if (pin < 16 && pin >= 0)
        {
            if (analogPins[pin] == false)
            {
                // if false then the pin is free
                return true;
            }
        }
    }
    else if (type == 'd')
    { // digital pins
        if (pin < 54 && pin >= 0)
        {
            if (digitalPins[pin] == false)
            {
                // if false then the pin is free
                return true;
            }
        }
    }
    else if (type == 'p' && pin >= 0)
    { // pca pins
        if (pin < 16)
        {
            if (pcaPins[pin] == false)
            {
                // if false then the pin is free
                return true;
            }
        }
    }
    return false;
}

int KNWRobot::getPin(int id, char type)
{
    if (type == 'p')
    { // ping sensors
        for (int i = 0; i < numPings; i++)
        {
            if (pingSensors[i].ID == id)
            {
                return pingSensors[i].PIN;
            }
        }
    }
    else if (type == 'b')
    { // bump sensors
        for (int i = 0; i < numBumps; i++)
        {
            if (bumpSensors[i].ID == id)
            {
                return bumpSensors[i].PIN;
            }
        }
    }
    else if (type == 'm')
    { // DC motors
        for (int i = 0; i < numMotors; i++)
        {
            if (motors[i].ID == id)
            {
                return motors[i].PIN;
            }
        }
    }
    else if (type == 's')
    { // servos
        for (int i = 0; i < numServos; i++)
        {
            if (servos[i].ID == id)
            {
                return servos[i].PIN;
            }
        }
    }
    else if (type == 'r')
    { // IR sensors
        for (int i = 0; i < numIR; i++)
        {
            if (irSensors[i].ID == id)
            {
                return irSensors[i].PIN;
            }
        }
    }
    return -1; // default no PIN found
}

// ******************************************* //
// Ping Sensor Functions
// ******************************************* //
int KNWRobot::getTrig(int id){
    for(int i = 0; i < numPings; i++){
        if (pingSensors[i].ID == id)
            return pingSensors[i].TRIG;
    }
}

int KNWRobot::getEcho(int id){
    for(int i = 0; i < numPings; i++){
        if (pingSensors[i].ID == id)
            return pingSensors[i].ECHO;
    }
}

bool KNWRobot::setupPing(int id, int trigger, int echo)
{
    if (checkPin(pin, 'd') && numPings < 8)
    {
        // set the trigger pin
        pingSensors[numPings].ID = id;
        pingSensors[numPings].TRIG = trigger;
        pingSensors[numPings].ECHO = echo;
        pingSensors[numPings].TYPE = 'd';
        numPings++;
        digitalPins[trigger] = true;
        digitalPins[echo] = true;
        return true;
    }
    return false;
}

// Check out this site for implementation details:
long KNWRobot::getPing(int id)
{
    int TRIGGER_PIN = getTrig(id);
    int ECHO_PIN = getEcho(id);
    int MAX_PING_DISTANCE = 200; // centimeters

    if (TRIGGER_PIN == -1 || ECHO_PIN == -1)
            return -1; // Ping sensor has not been set up properly; this is an invalid ID

    NewPing pingSensor(TRIGGER_PIN, ECHO_PIN, MAX_PING_DISTANCE);
    return pingSensor.ping_cm();
}

// ******************************************* //
// Bump Sensor Functions
// ******************************************* //
bool KNWRobot::setupBump(int id, int pin)
{
    if (checkPin(pin, 'd') && numBumps < 8)
    {
        bumpSensors[numBumps].ID = id;
        bumpSensors[numBumps].PIN = pin;
        bumpSensors[numBumps].TYPE = 'd';
        numBumps++;
        digitalPins[pin] = true;
        return true;
    }
    return false;
}

bool KNWRobot::getBump(int id)
{
    int pin = getPin(id, 'b');
    if (pin == -1)
        return false; // not a valid ID
    return (bool)digitalRead(pin);
}

// ******************************************* //
// Inclinometer Functions
// ******************************************* //
bool KNWRobot::setupIncline(int pin)
{
    if (checkPin(pin, 'a'))
    {
        inclinePin = pin;
        analogPins[pin] = true;
        return true;
    }
    return false;
}

int KNWRobot::getIncline()
{
    if (inclinePin == -1)
        return -1;
    return analogRead(inclinePin);
}

// ******************************************* //
// Conductivity Functions
// ******************************************* //
int KNWRobot::getConductivity()
{
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
// ******************************************* //
// Keypad Functions
// ******************************************* //
int KNWRobot::getKeypadInput()
{
    lcd->clear();
    return getKeypadInput(0);
}

int KNWRobot::getKeypadInput(int row)
{
    int inInt = -1;
    char key;
    bool letterflag = false;
    char letter;

    // to get multi input from keypad into int
    while (!entered)
    {
        key = keypad->getKey();
        if (key)
        {
            lcd->setCursor(numEntered, row);
            lcd->print(key);

            // max input is 16, neglects most recent and enters
            if (key == '#' || numEntered == 16)
            {
                DATA[numEntered] = '\0'; // null temrinator for atoi conversion
                entered = true;
            }
            else if (key == '*' && numEntered > 0)
            { // backspace
                numEntered--;
                lcd->setCursor(numEntered, row);
                lcd->print(" ");
            }
            else if (key != '#' && key != '*')
            {
                DATA[numEntered] = key;
                numEntered++;
            }
            if (key == 'A' || key == 'B' || key == 'C' || key == 'D')
            {
                letterflag = true;
                letter = key;
            }
        }

        if (entered)
        {
            if (letterflag)
            {
                inInt = (int)letter;
            }
            else
            {
                inInt = atoi(DATA);
            }
            memset(DATA, 0, sizeof(DATA));
            numEntered = 0;
        }
    } // end while

    entered = false;
    return inInt; // returns -1 if nothing pressed
}

void KNWRobot::secretFunction()
{
    lcd->clear();
    lcd->print("This B Empty");
    for (int i = 0; i < numServos; i++)
    {
        pwm->setPWM(servos[i].PIN, 0, PCA_SERVO_180_MIN + (i + 1) * 40);
    }
    lcd->setCursor(0, 1);
    lcd->print("YEEEEEEEETT");
    for (int i = 0; i < numMotors; i++)
    {
        pwm->setPWM(motors[i].PIN, 0, PCA_SERVO_180_MIN + (i + 1) * 40);
    }
}

// ******************************************* //
// LCD Functions
// ******************************************* //
void KNWRobot::clearLCD()
{
    lcd->clear();
}

void KNWRobot::moveCursor(int col, int row)
{
    lcd->setCursor(col, row);
}

void KNWRobot::clearLine(int row)
{
    lcd->setCursor(0, row);
    lcd->print("                "); // 16 characters
    lcd->setCursor(0, row);
}

void KNWRobot::printLCD(char *input)
{
    lcd->print(input);
}

void KNWRobot::printLCD(double input, short decimalPlaces)
{
    long multiplier = pow(10, decimalPlaces);
    long wholeValue = (long)input;
    printLCD(wholeValue);             //whole number value
    printLCD('.');                    //decimal point
    input -= wholeValue;              //gives us just the digits after the decimal
    input *= multiplier;              //put digits we want to print in front of decimal
    long decimalDigits = (long)input; //discard everything we won't use
    printLCD(abs(decimalDigits));          //print remaining digits as param specifies
}

void KNWRobot::printLCD(double input)
{
    printLCD(input, (short)3);
}

void KNWRobot::printLCD(int input)
{
    lcd->print(input);
}

void KNWRobot::printLCD(long input)
{
    lcd->print(input);
}

void KNWRobot::printLCD(char input)
{
    lcd->print(input);
}

// ******************************************* //
// PCA9685 Board Functions
// ******************************************* //
bool KNWRobot::setupServo(int id, int pin)
{
    if (checkPin(pin, 'p') && numServos < 16)
    {
        servos[numServos].ID = id;
        servos[numServos].PIN = pin;
        servos[numServos].TYPE = 'p';
        numServos++;
        pcaPins[pin] = true;
        return true;

    }
    return false;
}

bool KNWRobot::setupMotor(int id, int pin)
{
    if (checkPin(pin, 'p') && numMotors < 4)
    {
        motors[numMotors].ID = id;
        motors[numMotors].PIN = pin;
        motors[numMotors].TYPE = 'p';
        numMotors++;
        pcaPins[pin] = true;
        return true;
    }
    return false;
}

void KNWRobot::pcaStop(int id)
{
    int pin = getPin(id, 's');
    if (pin == -1)
    {
        pin = getPin(id, 'm');
    }
    if (pin != -1)
    {
        pwm->setPWM(pin,0,0);
    }
}

void KNWRobot::pcaStopAll()
{
    for (int channel = 0; channel < 16; channel++)
    {
        pwm->setPWM(channel, 0, 0);
    }
}

void KNWRobot::pca180Servo(int id, int angle)
{
    int pin = getPin(id, 's');
    if (pin != -1)
    { // not a valid ID
        // Take input from [-90,90] and map to PWM duty cycle scale out of 4095
        int pulselen = map(
            angle,
            0 * PCA_SERVO_180_INPUT_RANGE,
            2 * PCA_SERVO_180_INPUT_RANGE,
            PCA_SERVO_180_MIN,
            PCA_SERVO_180_MAX);

        // Shift to range of; PWM Signal: 1ms - 2ms will give
        // full reverse to full forward, 1.5ms is neutral
        pulselen = constrain(pulselen, PCA_SERVO_180_MIN, PCA_SERVO_180_MAX);
        pwm->setPWM(pin, 0, pulselen);
    }
}

void KNWRobot::pcaContServo(int id, int speed)
{
    int pin = getPin(id, 's');
    if (pin != -1)
    {
        // Take input from [-90,90] and map to PWM duty cycle scale out of 4095
        // Note, map() is used to flip direction.
        int pulselen = map(
            speed,
            -1 * PCA_SERVO_CONTINUOUS_INPUT_RANGE,
            PCA_SERVO_CONTINUOUS_INPUT_RANGE,
            PCA_SERVO_CONTINUOUS_MAX,
            PCA_SERVO_CONTINUOUS_MIN);

        // Shift to range of; PWM Signal: 1ms - 2ms will give
        // full reverse to full forward, 1.5ms is neutral
        pulselen = constrain(
            pulselen,
            PCA_SERVO_CONTINUOUS_MIN,
            PCA_SERVO_CONTINUOUS_MAX);
        pwm->setPWM(pin, 0, pulselen);
    }
}

void KNWRobot::pcaDCMotor(int id, int speed)
{
    int pin = getPin(id, 'm');
    if (pin != -1)
    {
        // Take input from [-1023,1023] and map to
        // PWM duty cycle scale out of 4095
        int pulselen = map(
            speed,
            -1 * PCA_DC_INPUT_RANGE,
            PCA_DC_INPUT_RANGE,
            PCA_DC_MIN,
            PCA_DC_MAX);

        // Shift to range of; PWM Signal: 1ms - 2ms will give
        // full reverse to full forward, 1.5ms is neutral
        pulselen = constrain(pulselen, PCA_DC_MIN, PCA_DC_MAX);

        pwm->setPWM(pin, 0, pulselen);
    }
}

void KNWRobot::pcaDC2Motors(int id1, int speed1, int id2, int speed2)
{
    int pin1 = getPin(id1, 'm');
    int pin2 = getPin(id2, 'm');
    if (pin1 != -1 && pin2 != 1)
    {
        int pulselen1 = map(
            speed1,
            -1 * PCA_DC_INPUT_RANGE,
            PCA_DC_INPUT_RANGE,
            PCA_DC_MIN,
            PCA_DC_MAX);
        pulselen1 = constrain(pulselen1, PCA_DC_MIN, PCA_DC_MAX);

        int pulselen2 = map(
            speed2,
            -1 * PCA_DC_INPUT_RANGE,
            PCA_DC_INPUT_RANGE,
            PCA_DC_MIN,
            PCA_DC_MAX);
        pulselen2 = constrain(pulselen2, PCA_DC_MIN, PCA_DC_MAX);

        pwm->setPWM(pin1, 0, pulselen1);
        pwm->setPWM(pin2, 0, pulselen2);
    }
}

void KNWRobot::pca180ServoTime(int id, int angle, int duration)
{
    int pin = getPin(id, 's');
    if (pin != -1)
    {
        // Take input from [-90,90] and map to PWM duty cycle scale out of 4095
        int pulselen = map(
            angle,
            0 * PCA_SERVO_180_INPUT_RANGE,
            2 * PCA_SERVO_180_INPUT_RANGE,
            PCA_SERVO_180_MIN,
            PCA_SERVO_180_MAX);

        pulselen = constrain(pulselen, PCA_SERVO_180_MIN, PCA_SERVO_180_MAX);
        pwm->setPWM(pin, 0, pulselen);

        delay(duration);

        pwm->setPWM(pin, 0, 0);
    }
}

void KNWRobot::pcaContServoTime(int id, int speed, int duration)
{
    int pin = getPin(id, 's');
    if (pin != -1)
    {
        int pulselen = map(
            speed,
            -1 * PCA_SERVO_CONTINUOUS_INPUT_RANGE,
            PCA_SERVO_CONTINUOUS_INPUT_RANGE,
            PCA_SERVO_CONTINUOUS_MAX,
            PCA_SERVO_CONTINUOUS_MIN);

        pulselen = constrain(
            pulselen,
            PCA_SERVO_CONTINUOUS_MIN,
            PCA_SERVO_CONTINUOUS_MAX);

        pwm->setPWM(pin, 0, pulselen);

        delay(duration);

        pwm->setPWM(pin, 0, 0);
    }
}

void KNWRobot::pcaDCMotorTime(int id, int speed, int duration)
{
    int pin = getPin(id, 'm');
    if (pin != -1)
    {
        int pulselen = map(
            speed,
            -1 * PCA_DC_INPUT_RANGE,
            PCA_DC_INPUT_RANGE,
            PCA_DC_MIN,
            PCA_DC_MAX);

        pulselen = constrain(pulselen, PCA_DC_MIN, PCA_DC_MAX);
        pwm->setPWM(pin, 0, pulselen);

        delay(duration);

        pwm->setPWM(pin, 0, 0);
    }
}

void KNWRobot::pcaDC2MotorsTime(
    int id1,
    int speed1,
    int id2,
    int speed2,
    int duration)
{

    int pin1 = getPin(id1, 'm');
    int pin2 = getPin(id2, 'm');
    if (pin1 != -1 && pin2 != 1)
    {
        int pulselen1 = map(
            speed1,
            -1 * PCA_DC_INPUT_RANGE,
            PCA_DC_INPUT_RANGE,
            PCA_DC_MIN,
            PCA_DC_MAX);
        pulselen1 = constrain(pulselen1, PCA_DC_MIN, PCA_DC_MAX);

        int pulselen2 = map(
            speed2,
            -1 * PCA_DC_INPUT_RANGE,
            PCA_DC_INPUT_RANGE,
            PCA_DC_MIN,
            PCA_DC_MAX);
        pulselen2 = constrain(pulselen2, PCA_DC_MIN, PCA_DC_MAX);

        pwm->setPWM(pin1, 0, pulselen1);
        pwm->setPWM(pin2, 0, pulselen2);

        delay(duration);

        pwm->setPWM(pin1, 0, 0);
        pwm->setPWM(pin2, 0, 0);
    }
}

// ******************************************* //
// Function to read IR character from sensor.
// ******************************************* //
bool KNWRobot::setupIR(int id, int pin)
{
    if (checkPin(pin, 'd') && numIR < 4)
    {
        irSensors[numIR].ID = id;
        irSensors[numIR].PIN = pin;
        irSensors[numIR].TYPE = 'd';
        numIR++;
        digitalPins[pin] = true;
        return true;
    }
    return false;
}

int KNWRobot::scanIR(int id)
{
    int pin = getPin(id, 'r');
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

unsigned char *KNWRobot::getIR()
{
    return buffer;
}

void KNWRobot::printVersion()
{
    printLCD("ENGR 1357 v1.0");
}
