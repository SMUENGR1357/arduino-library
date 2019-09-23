// Copyright 2019 Southern Methodist University

#ifndef SRC_KNW_KNWROBOT_H_
#define SRC_KNW_KNWROBOT_H_

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
    #include "pins_arduino.h"
    #include "WConstants.h"
#endif

#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Keypad.h"
#include "Adafruit_PWMServoDriver.h"
#include "Servo.h"

/**
 * A struct representing a generic component that gets plugged into the arduino.
 * A component is a combination of:
 * - An integer ID - you as the programmer define this.
 * - A type of pin - 'a' for analog, 'd' for digital, and 'p' for PCA board
 * - A pin number - these are 0-indexed and the max value depends on the type of pin
 *
 * Students typically do not reference these directly; rather, they are using the
 * various `getValue()` functions that use these Components behind the scenes.
 */
struct Component {
    int ID = 0;     // User defined
    int PIN = 0;    // physical pin
    char TYPE = 0;  // either analog(a), digital (d), or pca (p)
};


/**
 * KNWRobot Library 2.0, brought to you with love by the fabulous KNW TA's.
 *
 * This library contains the functions needed to communicate with the various
 * hardware components of your robot. Defining what pins are being used,
 * reading data from environmental sensors, and defining motor / servo motions
 * are all defined here.
 */

class KNWRobot {
 public:
        /**
         * Constructor used to establish a connection to the Arduino.
         * Initalizes the mapping for the analog and digital pins, and sets up
        * the LCD controller / PWM controller.
        *
        * Example usage:
        *
        * @code
        * // Create an instance of the robot
        * KNWRobot* myRobot = new KNWRobot();
        *
        * // Use the instance to setup a ping sensor on digital pin 8 with
        * // an ID of 1.
        * myRobot->setupPing(1, 8);
        * int pingSensorValue = myRobot->getPing(1);
        *
        * // Do something with pingSensorValue
        *
        * // Don't forget to cleanup the instance at the end of your program
        * delete myRobot;
        * @endcode
         */
        KNWRobot();

        /**
         * Accessor function to get what analog pins are currently assigned.
         * The returned array contains 16 elements, each of which refers
         * to an analog pin on the arduino. If element 0 is `true`, then
         * analog pin 0 is currently allocated to a sensor, and so on. If `false`,
         * then the pin is not currently allocated.
         *
         * @returns A boolean array of 16 elements indicating allocation status.
         *
         * Example usage:
         *
         * @code
         * // Assuming an inclinometer is wired and connected to analog pin 6
         * myRobot->setupIncline(6);
         * bool* activeAnalogPins = myRobot->getAnalogPins();
         * if (activeAnalogPins[6] == true) {
         *   // Analog pin 6 successfully connected
         * }
         * @endcode
         */
        bool* getAnalogPins();

        /**
         * Accessor function to get what digital pins are currently assigned.
         * The returned array contains 54 elements, each of which refers
         * to a digital pin on the arduino. If element 0 is `true`, then
         * digital pin 0 is currently allocated to a sensor, and so on.
         * If `false`, then the pin is not currently allocated.
         *
         * @returns A boolean array of 54 elements indicating allocation status.
         *
         * Example usage:
         *
         * @code
         * // Assuming a ping sensor is wired and connected to digital pin 3
         * myRobot->setupPing(1, 3);
         * bool* activeDigitalPins = myRobot->getDigitalPins();
         * if (activeDigitalPins[3] == true) {
         *   // Digital pin 3 successfully connected
         * }
         * @endcode
         */
        bool* getDigitalPins();

        /**
         * Accessor function to get what PCA board pins are currently assigned.
         * The PCA9685 board is what is used to control motors and servos.
         * The returned array contains 16 elements, each of which refers
         * to a pwm pin on the PCA board. If element 0 is `true`, then
         * pwm pin 0 is currently allocated to a servo / motor, and so on.
         * If `false`, then the pin is not currently allocated.
         *
         * @returns A boolean array of 16 elements indicating allocation status.
         *
         * @code
         * // Assuming a motor is wired and connected to pwm pin 1
         * myRobot->setupMotor(1, 1);
         * bool* activePCAPins = myRobot->getPCAPins();
         * if (activePCAPins[1] == true) {
         *   // Motor on pwm pin 1 successfully connected
         * }
         * @endcode
         */
        bool* getPCAPins();

        /**
         * Sets up and assigns a ping sensor to run on the specified digital pin.
         * A <a href="https://www.arduino.cc/en/tutorial/ping">ping sensor</a>
         * sends a high frequency audio burst out in front of it and waits for
         * the echo to come back to it. The time between the sending and receiving
         * is used to calculate the distance from the ping sensor to the object
         * <b>in centimeters</b>.
         *
         * <b>Note:</b> The arduino supports connecting up to 8 ping sensors
         * at one time.
         *
         * @param id A unique identifier that you specify. You will use this identifier
         * when running getPing(int), so it's recommended you assign it to a variable.
         * It is also recommended you make it equal to the pin number it is assigned to.
         * @param pin The digital pin that the ping sensor is connected to.
         * @return true If the ping sensor was successfully assigned to the pin
         * @return false If the ping sensor was not assigned to the pin
         *
         * Example usage:
         *
         * @code
         * // Assuming a ping sensor is wired and connected to digital pin 2
         * int pingSensorId = 1;
         * bool success = myRobot->setupPing(pingSensorId, 2);
         * if (success) {
         *   // Now ready to use the ping sensor with pingSensorId
         * }
         * @endcode
         */
        bool setupPing(int id, int pin);

        /**
         * Triggers a ping sensor to sense how far it is away from an object in front of it.
         * A <a href="https://www.arduino.cc/en/tutorial/ping">ping sensor</a>
         * sends a high frequency audio burst out in front of it and waits for
         * the echo to come back to it. The time between the sending and receiving
         * is used to calculate the distance from the ping sensor to the object
         * <b>in centimeters</b>.
         *
         * <b>Note:</b> Before running this function, be sure you have run the setupPing()
         * function for this identifier first. If a ping sensor with the provided ID has
         * not been setup, this function will return 0 every time you call it.
         *
         * @param id The integer identifier specified during the setupPing() call
         * @return long The distance away <b>in centimers</b> that the ping sensor detects the
         * object in front of it to be. If the ping sensor has not been setup, this will
         * return 0.
         *
         * Example usage:
         *
         * @code
         * // Assuming you ran the sample code in setupPing()
         * long distanceInCm = myRobot->getPing(pingSensorId);
         * myRobot->printLCD(distanceInCm);
         * @endcode
         */
        long getPing(int id);

        /**
         * Sets up and assigns a bump sensor to run on the specified digital pin.
         * A <a href="https://www.instructables.com/id/Cheap-Robot-Bump-Sensors-for-Arduino/">bump sensor</a>
         * can either be a "high" state or "low" state. Depending on how you wired the sensor,
         * a "high" state can either be pressed in or not pressed, and "low" state is the opposite.
         * Bump sensors are plugged into a digital pin.
         *
         * <b>Note:</b> The arduino support connecting up to 8 bump sensors at one time.
         *
         * @param id A unique identifier that you specify. You will use this identifier
         * when running checkBump(int), so it's recommended you assign it to a variable.
         * It is also recommended you make it equal to the pin number it is assigned to.
         * @param pin The digital pin that the bump sensor is connected to.
         * @return true If the bump sensor was successfully assigned to the pin
         * @return false If the bump sensor was not assigned to the pin
         *
         * Example usage:
         *
         * @code
         * // Assuming a bump sensor is wired and connected to digital pin 5
         * int bumpSensorId = 5;
         * bool success = myRobot->setupBump(bumpSensorId, 5);
         * if (success) {
         *   // Now ready to use the bump sensor with bumpSensorId
         * }
         * @endcode
         */
        bool setupBump(int id, int pin);

        /**
         * Checks to see if a bump sensor is pressed or not, or more specifically, in a high or low state.
         * A <a href="https://www.instructables.com/id/Cheap-Robot-Bump-Sensors-for-Arduino/">bump sensor</a>
         * can either be a "high" state or "low" state. Depending on how you wired the sensor,
         * a "high" state can either be pressed in or not pressed, and "low" state is the opposite.
         * Bump sensors are plugged into a digital pin.
         *
         * <b>Note:</b> The arduino support connecting up to 8 bump sensors at one time.
         *
         * @param id A unique identifier that you specify. This will be the same identifier
         * that you used as the first argument in setupPin(). It is also recommended you make
         * it equal to the pin number it is assigned to.
         * @param pin The digital pin that the bump sensor is connected to.
         * @return 1 The value of a "high" state. Depending on how you wire the bump sensor
         * (see link in description), a "high" state can refer to the sensor being pressed, or it
         * can refer to a sensor being open.
         * @return 0 The value of a "low" state, the opposite of a 1 value.
         *
         * Example usage:
         *
         * @code
         * // Assuming you ran the sample code in setupBump()
         * long bumpSensorState = myRobot->checkBump(bumpSensorId);
         * myRobot->printLCD(bumpSensorState);
         * @endcode
         */
        int checkBump(int id);

        /**
         * Sets up and assigns your inclinometer to run on the specified analog pin.
         * An inclinometer is a sensor that, when built and calibrated properly, can
         * detect the angle at which your robot is currently oriented relative to a
         * flat plane. Instructions for building the circuit are found on Canvas. For
         * additional details, read the documentation for getIncline().
         *
         * @param pin The analog pin that the inclinometer is connected to.
         * @return true If the inclinometer was successfully assigned to the pin
         * @return false If the inclinometer was not assigned to the pin
         *
         * Example usage:
         *
         * @code
         * // Assuming your inclinometer is wired and connected to analog pin 6
         * bool success = myRobot->setupIncline(6);
         * if (success) {
         *   // Now ready to use the inclinometer
         * }
         * @endcode
         */
        bool setupIncline(int pin);

        /**
         * Provides a reading of the current value of your inclinometer.
         *
         * This function returns a value in the range of [0 - 1023], which is the min - max
         * range that Arduino analog pins provide. This value in itself does not tell you
         * the angle of incline, but rather the analog voltage [0 - 5] volts coming into that
         * analog pin converted to a 10-bit precision number . You are required
         * to calibrate your sensor by reading sensor value at various angles and then
         * generating an interpolating function. This function can then be used to convert
         * new values while your robot is performing its tasks. The more data points, the
         * more likely your function is accurate. It is also very likely that you will have
         * to periodically recalibrate your sensor. Rebuilding your circuit (for example,
         * changing the resistors or fixing a broken connection) will require you to
         * recalibrate.
         *
         * @return int A value between [0 - 1023] telling you the raw analog pin reading.
         *
         * Example code:
         *
         * @code
         * // Assuming you have run the sample code in setupIncline()
         * int inclineReading = myRobot->getIncline();
         *
         * // This is NOT a good interpolating function. Your function will need to be more
         * // precise & accurate than this. This is just an example to show converting from
         * // a raw sensor reading to an angle before printing to the LCD screen.
         * int angle = inclineReading / 6;
         * myRobot->printLCD(angle);
         * @endcode
         */
        int getIncline();

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
         * Much like getIncline(), the raw returned value is a range between [0 - 1023],
         * denoting the absolute difference between analog pins 2 and 3. You are required
         * to calibrate your conductivity probe much in the same way you calibrate your
         * inclinometer. Be sure to read getIncline() for more details, as well as Canvas.
         *
         * @return int A value between [0 - 1023] telling you the raw analog pin reading.
         *
         * Example code:
         *
         * @code
         * // Assuming you have run the sample code in setupIncline()
         * int conductivityReading = myRobot->getConductivity();
         *
         * // Note that this will print the raw value, not the conductivity of the sand.
         * myRobot->printLCD(conductivityReading);
         * @endcode
         */
        int getConductivity();

        /**
         * Sets up and assigns your temperature probe to run on the specified analog pin.
         * A temperature probe is a sensor that, when built and calibrated properly, can
         * detect the temperature by using a thermal resistor.
         * Instructions for building the circuit are found on Canvas. For
         * additional details around calibration, read the documentation for getIncline().
         *
         * @param pin The analog pin that the temperature probe is connected to.
         * @return true If the temperature probe was successfully assigned to the pin
         * @return false If the temperature probe was not assigned to the pin
         *
         * Example usage:
         *
         * @code
         * // Assuming your temperature probe is wired and connected to analog pin 8
         * bool success = myRobot->setupTemp(8);
         * if (success) {
         *   // Now ready to use the temperature probe
         * }
         * @endcode
         */
        bool setupTemp(int pin);

        /**
         * Provides a reading of the current value of your temperature probe.
         *
         * This function returns a value in the range of [0 - 1023], which is the min - max
         * range that Arduino analog pins provide. The value here is dependent on your
         * circuitry and the resistence of the thermal resistor at the time of reading. As
         * such, this sensor requires calibration like your inclinometer and your conductivity
         * probe. Refer to getIncline() for more information around calibration.
         *
         * @return int A value between [0 - 1023] telling you the raw analog pin reading.
         *
         * Example code:
         *
         * @code
         * // Assuming you have run the sample code in setupTemp()
         * int temperatureProbeReading = myRobot->getTemp();
         *
         * // Note that this will print the raw value, not the actual temperature
         * myRobot->printLCD(temperatureProbeReading);
         * @endcode
         */
        int getTemp();

        /**
         * Clears out the LCD, and gets input from the number pad.
         *
         * As defined in the wiring guide, the number pad is plugged into the row of
         * digital pins starting at digital pin 39 and ending at digital pin 53. You
         * can enter up to 15 digits, followed by the '#' sign, and this function
         * will return the entered value to your code.
         *
         * The buttons you pressed will appear on the LCD. There are a few special buttons:
         *
         * <ul>
         *   <li>`*` = backspace</li>
         *   <li>`#` = enter</li>
         *   <li>
         *     'A', 'B', 'C', and 'D' = get translated to 10, 11, 12, and 13,
         *     respectively. Use those numbers as special cases for quadrants.
         *   </li>
         * </ul>
         *
         * @return int The numbers / characters you input.
         *
         * Example code
         *
         * @code
         * int quadrant = myRobot->getInput();
         * // Enter A B C or D, followed by #
         * if (quadrant == 'A') {
         *   // Perform the starting function for quadrant A that you define
         *   executeQuadrantA();
         * }
         * if (quadrant == 'B') {
         *   // Perform the starting function for quadrant B that you define
         *   executeQuadrantB();
         * }
         *
         * int servoPosition = myRobot->getInput();
         * // Enter a value, such as 45, followed by #
         * // Assuming you've run the setupServo() sample code already
         * myRobot->pca180Servo(servoID, servoPosition);
         * @endcode
         */
        int getInput();

        /**
         * Gets input from the number pad, printing the input on the specified row.
         *
         * This is fundamentally the same as getInput(), but instead of clearing out
         * the LCD, you specify what row to print your input on. Useful for if you
         * want to print a statement on row 1, and display your keypad input on line 2.
         * Refer to getInput() for full details.
         *
         * @param row Either 0 or 1, the row you wwant to print input on.
         * @return int The numbers / characters you input.
         *
         * Example code:
         *
         * @code
         * myRobot->clearLCD();
         *
         * // Prompt on first line, display input on next line
         * myRobot->printLCD("Servo angle:");
         * int servoPosition = myRobot->getInput(1);
         *
         * // Assuming you've run the setupServo sample code already
         * myRobot->pca180Servo(servoID, servoPosition);
         * @endcode
         */
        int getInput(int row);

        /**
         * Clears the LCD of all content.
         *
         * As the title says, any content on the LCD is cleared when you call this
         * function.
         *
         * Example code:
         *
         * @code
         * myRobot->printLCD("Goodbye!");
         * myRobot->clearLCD();
         * @endcode
         */
        void clearLCD();

        /**
         * Allows you to set the LCD cursor to control where output is displayed
         *
         * The LCD consists of 2 rows and 16 columns. By calling this function,
         * subsequent calls to one of the various printLCD(char*) functions will
         * print the text on starting with that particular row / column position.
         * Note that the input values are 0-indexed.
         *
         * @param col A value in the range [0 - 15] representing what column to start printing.
         * @param row A value in the range [0 - 1] representing what row to start printing.
         *
         * Example code:
         *
         * @code
         * // Move the cursor to the second row, fourth column
         * myRobot->moveCursor(3, 1);
         * myRobot->printLCD("Hello!");
         * @endcode
         */
        void moveCursor(int col, int row);

        /**
         * Clears out a row of the LCD cursor.
         *
         * Any content on the given row (either 0 or 1) is cleared out. The other
         * row is left untouched.
         *
         * @param row A value in the range [0 - 1] representing what row to clear.
         *
         * Example code:
         *
         * @code
         * myRobot->printLCD("Goodbye!");
         * myRobot->clearLine(0);
         * @endcode
         */
        void clearLine(int row);

        /**
         * Prints a line of text onto the LCD at the current cursor position
         *
         * Note that each line has a max of 16 characters. Use this function
         * in conjunction with moveCursor(int, int) to properly show messages.
         *
         * @param input A character array whose contents will be printed on the LCD.
         *
         * Example code:
         *
         * @code
         * // Reset the cursor to the start of the screen
         * myRobot->moveCursor(0, 0);
         * myRobot->printLCD("Hello SMU");
         *
         * // Move to the next line
         * myRobot->moveCursor(0, 1);
         * myRobot->printLCD("KNW 2300");
         * @endcode
         */
        void printLCD(char* input);

        /**
         * Prints an integer onto the LCD at the current cursor position.
         *
         * Note that each line has a max of 16 characters. Use this function
         * in conjunction with moveCursor(int, int) to properly show messages.
         *
         * @param input An integer whose value will be printed on the LCD.
         *
         * Example code:
         *
         * @code
         * // Reset the cursor to the start of the screen
         * myRobot->moveCursor(0, 0);
         *
         * // Assuming your conductivity sensor is properly connected
         * int conductivityReading = myRobot->getConductivity();
         *
         * myRobot->printLCD(conductivityReading);
         * @endcode
         */
        void printLCD(int input);

        /**
         * Prints a long onto the LCD at the current cursor position.
         *
         * Note that each line has a max of 16 characters. Use this function
         * in conjunction with moveCursor(int, int) to properly show messages.
         *
         * @param input A long whose value will be printed on the LCD.
         *
         * Example code:
         *
         * @code
         * // Reset the cursor to the start of the screen
         * myRobot->moveCursor(0, 0);
         *
         * // Assuming your ping sensor is setup using setupPing()
         * long pingReading = myRobot->getPing(pingID);
         *
         * myRobot->printLCD(pingReading);
         * @endcode
         */
        void printLCD(long input);

        /**
         * Prints a single character onto the LCD at the current cursor position.
         *
         * Note that each line has a max of 16 characters. Use this function
         * in conjunction with moveCursor(int, int) to properly show messages.
         *
         * @param input A character whose value will be printed on the LCD.
         *
         * Example code:
         *
         * @code
         * // Reset the cursor to the start of the screen
         * myRobot->moveCursor(0, 0);
         * myRobot->printLCD('S');
         * myRobot->printLCD('M');
         * myRobot->printLCD('U');
         * @endcode
         */
        void printLCD(char input);

        /**
         * Sets up and assigns a servo motor to run on the specified pin on the PCA board.
         * There are two types of servo: a 180 degree servo and a continuous rotation servo
         * (sometimes colloquially known as a 360 degree servo).
         *
         * The 180 degree servo operates by angle. It starts at a 90 degree position, and
         * you can use pca180Servo() to specify that it move to angle between 0 to 180 degrees.
         *
         * The continuous rotation servo operates by rotational velocity. A speed of 0
         * means the servo does not move. You can use pcaContServo() to specify a velocity
         * in some direction. A positive value will cause the servo to move in one direction,
         * while a negative value will cause the servo to move in the other direction.
         *
         * You can also use pca180ServoTime() and pcaContServoTime() for time-based servo
         * control. Refer to all of those functions to understand the differences in their operation.
         *
         * The neutral values listed above are approximate. You will need to do some calibration
         * to figure out the true neutral values for your servos.
         *
         * Refer to the documentation on Canvas for how to properly wire a servo to your
         * Arduino and PCA board.
         *
         * <b>Note:</b> The arduino / PCA board supports connecting up to 16 servos at one time.
         * However, servos and motors share the same PCA board so the actual number you'll
         * use is much smaller.
         *
         * @param id A unique identifier that you specify. You will use this identifier
         * when running the various pca servo functions, so it's recommended you assign it to a variable.
         * It is also recommended you make it equal to the pin number it is assigned to.
         * @param pin The pin on the PCA board that servo is connected to.
         * @return true If the servo was successfully assigned to the pin
         * @return false If the servo was not assigned to the pin
         *
         * Example usage:
         *
         * @code
         * // Assuming a servo is wired and connected to PCA board pin 2
         * int servoId = 1;
         * bool success = myRobot->setupServo(servoId, 2);
         * if (success) {
         *   // Now ready to use the servo with servoId
         * }
         * @endcode
         */
        bool setupServo(int id, int pin);

        /**
         * Sets up and assigns a DC motor to run on the specified pin on the PCA board.
         *
         * DC Motors operate by providing them a speed value (which also determines direction
         * of rotation) and an optional time. You can use pcaDCMotor() to move a motor at
         * a set speed indefinitely (or until another pcaDC function is called), or you
         * can control two motors simultaneously using pcaDC2Motors() (useful for moving
         * in a given direction).
         *
         * pcaDCMotortime() and pcaDC2MotorsTime() can be used to control 1 or 2 motors
         * simultaneously for a set amount of time. Refer to all of those functions for
         * specifics on their behavior.
         *
         * The neutral values listed above are approximate. You will need to do some calibration
         * to figure out the true neutral values for your servos.
         *
         * Refer to the documentation on Canvas for how to properly wire a motor to your
         * Arduino and PCA board.
         *
         * <b>Note:</b> The arduino / PCA board supports connecting up to 4 motors at one time.
         *
         * @param id A unique identifier that you specify. You will use this identifier
         * when running the various pca motor functions, so it's recommended you assign it to a variable.
         * It is also recommended you make it equal to the pin number it is assigned to.
         * @param pin The pin on the PCA board that motor is connected to.
         * @return true If the motor was successfully assigned to the pin
         * @return false If the motor was not assigned to the pin
         *
         * Example usage:
         *
         * @code
         * // Assuming a motor is wired and connected to PCA board pin 3
         * int motorId = 3;
         * bool success = myRobot->setupMotor(motorId, 3);
         * if (success) {
         *   // Now ready to use the motor with motorId
         * }
         * @endcode
         */
        bool setupMotor(int id, int pin);

        void pcaRaw(int id, int pulseSize);
        void pcaRawTime(int id, int pulseSize, int duration);

        /**
         * Stops a motor or servo with the provided identifier.
         *
         * Assuming you setup a motor / servo using setupMotor() / setupServo(),
         * this function stops the motor / servo that was assigned the identifier
         * you provided. If the identifier has not been assigned, nothing
         * happens.
         *
         * @param id The identifier that was passed as the first argument into
         * setupMotor() / setupServo()
         *
         * Example code:
         *
         * @code
         * // Assuming you've already run setupMotor()
         * myRobot->pcaDCMotor(motorId, 1023);
         *
         * // Robot does whatever it needs to do
         *
         * myRobot->pcaStop(motorId);
         * @endcode
         */
        void pcaStop(int id);

        /**
         * Stops all motors and servos connected to the PCA board.
         *
         * All pins on the PCA board have their PWM signals reset, thereby
         * stopping all motors / servos.
         *
         * * @code
         * // Assuming you've already run setupMotor()
         * myRobot->pca2DCMotor(motorId, 1023, motorId2, 1020);
         *
         * // Robot does whatever it needs to do
         *
         * myRobot->pcaStopAll();
         * @endcode
         */
        void pcaStopAll();

        /**
         * Moves a specified 180 degree servo to a specified angle.
         *
         * This moves a 180 degree servo with a given identifier to a given angle.
         * The identifier should match one that was supplied as the first argument
         * to setupServo(). Once the servo moves to the given angle, it will
         * stay at that angle. Your program will continue running as soon as the
         * angle is applied. This behavior is different than the otherwise
         * similar function pca180ServoTime(). Refer to that function for specifics
         * on how it operates.
         *
         * Note that this function is meant for use with 180 degree servos. For continuous
         * rotation servos (360 degree servos), use the pcaContServo() or pcaContServoTime()
         * functions.
         *
         * @param id The identifier that was passed as the first argument into setupServo()
         * @param angle The angle between [0 - 180] to set the servo to.
         *
         * Example code:
         *
         * @code
         * // Suppose you have a ping sensor mounted on a servo
         * // and you have already run setupPing() and setupServo()
         * myRobot->pca180Servo(servoId, 0);
         * long pingReading = myRobot->getPing(pingId);
         *
         * // Do something with ping reading
         *
         * // Now move the servo to a new position and read again
         * myRobot->pca180Servo(servoId, 45);
         * pingReading = myRobot->getPing(pingId);
         *
         * // Do something else with your ping reading
         * @endcode
         */
        void pca180Servo(int id, int angle);

        /**
         * Moves a specified 180 degree servo to a specified angle for the specified amount of time.
         *
         * This moves a 180 degree servo with a given identifier to a given angle for a
         * given amount of time. The identifier should match one that was supplied as
         * the first argument to setupServo(). Unlike the pca180Servo() function, which
         * sets the angle and then immediately continues with your program, this function
         * sets an angle and then waits for the time to pass. It will then reset the servo
         * back to 0, at which point your program resumes.
         *
         * Note that this function is meant for use with 180 degree servos. For continuous
         * rotation servos (360 degree servos), use the pcaContServo() or pcaContServoTime()
         * functions.
         *
         * @param id The identifier that was passed as the first argument into setupServo()
         * @param angle The angle between [0 - 180] to set the servo to.
         * @param duration The duration <b><i>in milliseconds</i></b> to set the servo for.
         *
         * Example code:
         *
         * @code
         * // Suppose you have already run setupServo()
         * // Move a servo to a 45 degree position for 3 seconds
         * myRobot->pca180ServoTime(servoId, 45, 3000);
         *
         * // After three seconds, your code resumes here.
         * @endcode
         */
        void pca180ServoTime(int id, int angle, int duration);

        /**
         * Moves a specified continuous rotation servo with a specified speed.
         *
         * This moves a continuous rotation servo with a given identifier with the given speed.
         * The identifier should match one that was supplied as the first argument
         * to setupServo(). Once the servo moves with the given speed, it will
         * move at that speed indefinitely. Your program will continue running as soon as the
         * speed is applied. This behavior is different than the otherwise
         * similar function pcaContServoTime(). Refer to that function for specifics
         * on how it operates.
         *
         * Note that this function is meant for use with continuous rotation servos. For 180
         * degree servos, use the pca180Servo() or pca180ServoTime() functions.
         *
         * @param id The identifier that was passed as the first argument into setupServo()
         * @param angle The speed between [-90 - 90] to set the servo to. A negative value
         * moves the servo in one direction, while a positive value moves the servo in
         * the other.
         *
         * Example code:
         *
         * @code
         * // Assuming you have run setupServo() already
         * myRobot->pcaContServo(servoId, -90);
         *
         * // Servo will now run at full speed in one direction
         * // Code continues running, robot does cool things
         *
         * myRobot->pcaStop(servoId);
         * @endcode
         */
        void pcaContServo(int id, int speed);

        /**
         * Moves a specified continuous rotation servo to a specified speed for the specified amount of time.
         *
         * This moves a continuous rotation servo with a given identifier with a given speed for a
         * given amount of time. The identifier should match one that was supplied as
         * the first argument to setupServo(). Unlike the pcaContServo() function, which
         * sets the speed and then immediately continues with your program, this function
         * sets a speed and then waits for the time to pass. It will then reset the servo
         * back to 0, at which point your program resumes.
         *
         * Note that this function is meant for use with continuous rotation servos. For 180 degree servos,
         * use the pca180Servo() or pca180ServoTime() functions.
         *
         * @param id The identifier that was passed as the first argument into setupServo()
         * @param speed The speed between [-90 - 90] to set the servo to.
         * @param duration The duration <b><i>in milliseconds</i></b> to set the servo for.
         *
         * Example code:
         *
         * @code
         * // Suppose you have already run setupServo()
         * // Move a servo at full speed for 3.5 seconds
         * myRobot->pca180ServoTime(servoId, 45, 3500);
         *
         * // After 3.5 seconds, your code resumes here.
         * @endcode
         */
        void pcaContServoTime(int id, int speed, int duration);

        /**
         * Moves a specified DC motor with a specified speed.
         *
         * This moves a DC motor with a given identifier with the given speed.
         * The identifier should match one that was supplied as the first argument
         * to setupMotor(). Once the motor moves with the given speed, it will
         * move at that speed indefinitely. Your program will continue running as soon as the
         * speed is applied. This behavior is different than the otherwise
         * similar function pcaDCMotorTime(). Refer to that function for specifics
         * on how it operates.
         *
         * @param id The identifier that was passed as the first argument into setupMotor()
         * @param speed The speed between [-1023 - 1023] to set the motor to. A negative value
         * moves the motor in one direction, while a positive value moves the motor in
         * the other.
         *
         * Example code:
         *
         * @code
         * // Suppose you have already run setupMotor() and a setupPing() function
         * // Run a motor at full speed
         * myRobot->pcaDCMotor(motorId, 1023);
         *
         * // While the motor runs, read a ping sensor value
         * long pingReading = myRobot->getPing(pingId);
         * if (pingReading < 30) {
         *   // Robot is less than 30 cm away from a wall, so stop the motor
         *   myRobot->pcaDCMotor(motorId, 0);
         *
         *   // Alternatively, you can use pcaStop()
         *   myRobot->pcaStop(motorId);
         * }
         * @endcode
         */
        void pcaDCMotor(int id, int speed);

        /**
         * Moves two specified DC motors with two specified speeds.
         *
         * This moves two DC motors with given identifiers to two speeds (potentially
         * the same values, potentially different values, depending on use case and calibration).
         * The identifiers should match ones that were supplied as the first argument
         * to setupMotor(). Once the motors are set with the given speeds, they will
         * move at that speed indefinitely. Your program will continue running as soon as the
         * speed is applied. This behavior is different than the otherwise
         * similar function pcaDC2MotorsTime(). Refer to that function for specifics
         * on how it operates.
         *
         * @param id1 The identifier of the first motor you want to move. This is the value
         * that was passed as the first argument into setupMotor().
         * @param speed1 The speed between [-1023 - 1023] to set the first motor to. A negative value
         * moves the motor in one direction, while a positive value moves the motor in
         * the other.
         * @param id2 The identifier of the second motor you want to move. This is the value
         * that was passed as the first argument into setupMotor().
         * @param speed2 The speed between [-1023 - 1023] to set the second motor to. A negative value
         * moves the motor in one direction, while a positive value moves the motor in
         * the other.
         *
         * Example code:
         *
         * @code
         * // Suppose you have already run setupMotor() and a setupPing() function
         * // Run both motors at full speed
         * myRobot->pcaDC2Motors(motorId, 1023, motorId2, 1023);
         *
         * // While the motors run, read a ping sensor value
         * long pingReading = myRobot->getPing(pingId);
         * if (pingReading < 30) {
         *   // Robot is less than 30 cm away from a wall, so stop the robot from moving
         *   myRobot->pcaDC2Motors(motorId, 0, motorId2, 0);
         *
         *   // Alternatively, you can use pcaStopAll()
         *   myRobot->pcaStopAll();
         * }
         * @endcode
         */
        void pcaDC2Motors(int id1, int speed1, int id2, int speed2);

        /**
         * Moves a specified DC motor with a specified speed for a specified amount of time.
         *
         * This moves a DC motor with a given identifier with the given speed for a
         * given amount of time. The identifier should match one that was supplied as the
         * first argument to setupMotor(). Unlike the pcaDCMotor() function, which
         * sets the speed and then immediately continues with your program, this function
         * sets a speed and then waits for the time to pass. It will then reset the motor
         * back to 0, at which point your program resumes.
         *
         * @param id The identifier that was passed as the first argument into setupMotor()
         * @param speed The speed between [-1023 - 1023] to set the motor to. A negative value
         * moves the motor in one direction, while a positive value moves the motor in
         * the other.
         * @param duration The duration <b><i>in milliseconds</i></b> to set the motor for.
         *
         * Example code:
         *
         * @code
         * // Suppose you have already run setupMotor()
         * // Run a motor at full speed for 5 seconds
         * myRobot->pcaDCMotor(motorId, 1023, 5000);
         *
         * // The motor will run for five seconds. After five seconds, your program
         * // resumes here
         * @endcode
         */
        void pcaDCMotorTime(int id, int speed, int duration);

        /**
         * Moves two specified DC motors with specified speeds for a specified amount of time.
         *
         * This moves two DC motors with given identifiers with the given speeds for a
         * given amount of time. These speeds do not have to match, in particular in cases
         * where calibration causese them to move at slightly different speeds.
         * The identifiers should match one that was supplied as the
         * first argument to setupMotor(). Unlike the pcaDC2Motors() function, which
         * sets the speed and then immediately continues with your program, this function
         * sets two speeds and then waits for the time to pass. It will then reset the motors
         * back to 0, at which point your program resumes.
         *
         * @param id1 The identifier of the first motor you want to move. This identifier should
         * match one that was passed as the first argument into setupMotor().
         * @param speed1 The speed between [-1023 - 1023] to set the first motor to. A negative value
         * moves the motor in one direction, while a positive value moves the motor in
         * the other.
         * @param id2 The identifier of the second motor you want to move. This identifier should
         * match one that was passed as the first argument into setupMotor().
         * @param speed2 The speed between [-1023 - 1023] to set the second motor to. A negative value
         * moves the motor in one direction, while a positive value moves the motor in
         * the other.
         * @param duration The duration <b><i>in milliseconds</i></b> to set the motors for. Both
         * motors will stop at the same time when the duration is passed.
         *
         * Example code:
         *
         * @code
         * // Suppose you have already run setupMotor() for both motors
         * // Run both motors at full speed for 5 seconds
         * myRobot->pcaDCMotor(motorId, 1023, motorId2, 1020, 5000);
         *
         * // The motor will run for five seconds. After five seconds, your program
         * // resumes here
         * @endcode
         */
        void pcaDC2MotorsTime(
            int id1,
            int speed1,
            int id2,
            int speed2,
            int duration);

        /**
         * Sets up and assigns an IR navigation sensor to run on the specified digital pin.
         * An IR navigation sensor is used to detect the values being emitted by the various
         * beacons placed around the field. Refer to documentation on Canvas for details
         * on how to properly wire and connect your IR sensor.
         *
         * <b>Note:</b> The arduino supports connecting up to 4 IR sensors
         * at one time.
         *
         * @param id A unique identifier that you specify. You will use this identifier
         * when running scanIR(), so it's recommended you assign it to a variable.
         * It is also recommended you make it equal to the pin number it is assigned to.
         * @param pin The digital pin that the IR sensor is connected to.
         * @return true If the IR sensor was successfully assigned to the pin
         * @return false If the IR sensor was not assigned to the pin
         *
         * Example code:
         *
         * @code
         * // Assuming an IR sensor is wired and connected to digital pin 10
         * int IRSensorId = 10;
         * bool success = myRobot->setupIR(IRSensorId, 10);
         * if (success) {
         *   // Now ready to use the IR sensor with pingSensorId
         * }
         * @endcode
         */
        bool setupIR(int id, int pin);

        /**
         * This uses the IR sensor with the given identifier to scan for beacons that
         * may be nearby.
         *
         * This function uses the IR sensor to scan for IR signals coming from the beacons.
         * The values that it reads are stored in an internal character buffer, which you
         * can access using getIR() after running this function.
         *
         * <b>Note</b>: every time you want to get fresh values from getIR(), you have to
         * run this function first.
         *
         * @param id The identifier that was provided as the first argument to setupIR().
         * @return int The number of characters that were read in this given scan.
         *
         * Example code:
         *
         * @code
         * // Assuming you have already run the code in setupIR()
         * int charactersReadFromIR = myRobot->scanIR(IRSensorId);
         * myRobot->printLCD("Chars read: ");
         * myRobot->printLCD(charactersReadFromIR);
         *
         * // Now print the reading on the next line of the LCD
         * char* IRCharacters = myRobot->getIR();
         * myRobot->moveCursor(0, 1);
         * myRobot->printLCD(IRCharacters);
         * @endcode
         */
        int scanIR(int id);

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
         * // Assuming you have already run the code in setupIR()
         * int charactersReadFromIR = myRobot->scanIR(IRSensorId);
         * myRobot->printLCD("Chars read: ");
         * myRobot->printLCD(charactersReadFromIR);
         *
         * // Now print the reading on the next line of the LCD
         * char* IRCharacters = myRobot->getIR();
         * myRobot->moveCursor(0, 1);
         * myRobot->printLCD(IRCharacters);
         * @endcode
         */
        unsigned char* getIR();

        void resetKeypad();

        void resetLCD();



 private:
        // Tracks which pins are being used and which are free
        bool analogPins[16];
        bool digitalPins[54];
        bool pcaPins[16];

        // Tracks which components are associated to what ID's / pins
        Component pingSensors[8];
        Component bumpSensors[8];
        Component irSensors[4];
        Component motors[4];
        Component servos[16];

        // Tracks how many of each component the robot currently has attached
        int numPings;
        int numBumps;
        int numIR;
        int inclinePin;
        int tempPin;
        int numMotors;
        int numServos;

        // Specific pins for the conductivity probe
        const int conductivityDigitalPin1 = 12;
        const int conductivityDigitalPin2 = 13;
        const int conductivityAnalogPin1 = 2;
        const int conductivityAnalogPin2 = 3;

        // Instance variables used in conjunction with the keypad
        bool entered;
        int numEntered;
        char DATA[17];
        byte ROWS = 4;
        byte COLS = 4;
        char keys[4][4] = {
              {'1', '2', '3', 'A'},
              {'4', '5', '6', 'B'},
              {'7', '8', '9', 'C'},
              {'*', '0', '#', 'D'}
        };

        byte rowPins[4] = {39, 41, 43, 45};
        byte colPins[4] = {47, 49, 51, 53};
        Keypad* keypad;

        // Used to control the LCD and PCA boards
        LiquidCrystal_I2C* lcd;
        Adafruit_PWMServoDriver* pwm;

        // Instance variables used in conjunction with the IR sensor
        unsigned char necState;
        int num_chars;
        unsigned long prev_time;

        unsigned char IRChar, IRCharBitMask, buffer[8];
        boolean receiverState = false;
        unsigned long cur_time, ticks;

        // Miscellaneous functions
        bool checkPin(int pin, char type);  // check to see if avalible
        int getPin(int id, char type);      // from an ID
        void secretFunction();

        void setupKeypad();
        void setupLCD();
};

#endif  // SRC_KNW_KNWROBOT_H_
