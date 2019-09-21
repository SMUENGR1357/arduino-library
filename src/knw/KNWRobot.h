/*
	-----------------------------------------------------------------------------
	Functions and Variables
	-----------------------------------------------------------------------------
	KNWRobot(); 							//constructor, initalizes the pins, numpad, LCD, PWM

	bool analogPins[16]; 					//[*internal] 16 analog pins (0-15), True if used, False if free
	bool digitalPins[54];					//[*internal] 54 analog pins (0-53),True if used, False if free
	bool pcaPins[16];						//[*internal] 16 pwm pins on PCA board (0-15),True if used, False if free
	bool* getAnalogPins();					//returns pointer to analogPins list
	bool* getDigitalPins();					//returns pointer to digitalPins list
	bool* getPCAPins();						//returns pointer to pcaPins list
	bool checkPin(int pin, char type); 		//[*internal] check to see if pin is avalible (false) 
	int getPin(int id, char type); 			//[*internal] gets physical pin from the user's ID		

	------------------------------------
	Ping Sensor 
	------------------------------------
	Component pingSensors[8];				//[*internal] List of Ping Sensors 'attached', up to 8
	int numPings;							//number of ping sensors attached
	bool setupPing(int id, int pin);		//'attaches' ping to physical pin by user's ID, true if successful
	long getPing(int id);					//returns (long) ping distance in cm

	------------------------------------
	(!)Bump Sensor
	------------------------------------
	Component bumpSensors[8];				//[*internal] List of Bump Sensors 'attached', up to 8
	int numBumps;							//number of bump sensors attached
	bool setupBump(int id, int pin);		//'attaches' bump to physical pin by user's ID, true if successful
	int checkBump(int id);					//checks to see if the bump has been triggered (HIGH or 1), else its LOW or 0

	------------------------------------
	(!)Inclinometer
	------------------------------------
	int inclinePin;							//the physical pin the inclinometer is attached to
	bool setupIncline(int pin);				//'attaches' inclinometer to physical pin by user's ID, true if successful
	int getIncline();						//returns ADC code [0-1023] of the measured resistance 
	

	int tempPin;
		bool setupTemp(int pin);
		int getTemp(); //returns 0-1023, or -1 if not setup
	------------------------------------
	Conductivity Sensor Functions
	------------------------------------
	int getConductivity(); 					//returns conductivity between plates
											//analog pins 2,3 : digital pins 12,13
	------------------------------------
	Keypad Functions
	------------------------------------
	int getInput();							//returns up to 15 inputted chars as int 
											//'#' is the 'enter' key
											//'*' is the backspace
											//treat 'A,B,C,D' as int values for quadrents
	------------------------------------
	LCD Functions
	------------------------------------
	void clearLCD();						//clears all contents of LCD
	void moveCursor(int col, int row);		//moves the cursor to (col, row)
											//col is 0-15, row is 0(top) or 1(bottom)
	void clearLine(int row);				//clears line, cursor set to begining of row
	void printLCD(* input);					//takes input of char*, int, long, or char
											//will print input where cursor currently is
											//suggest moveCursor() first
											//the input should be  char str[] 

	------------------------------------
	PCA9685 Functions (all vars are int)	//goes 0(LOW) to 4095 (HIGH)
	------------------------------------
	void pcaRaw(channel, pulseSize); 		//configures pulsewidth/size on channel
	void pcaRawTime(channel, pulseSize, 	//configures pulse on channel for duration (in ms)
		duration);
	void pcaStop(channel);					//stops the motor on channel
	void pcaStopAll();						//stops all motors
	void pca180Servo(channel, angle);		//move a 180 servo on channel to angle then hold
											//0 <= angle <= 180+ (for student calibration)
	void pcaContServo(channel, speed);		//move a continuous servo on channel at speed
											//0 = stop, [-90, 90] speed backwards/forwards
	void pcaDCMotor(channel, speed);		//move DC motor on channel at speed
											//0 = stop, [-1023, 1023] speed backwards/forwards
	void pcaDC2Motors(channel1, speed1, 	//move 2 DC motors on channels to speed
		channel2, speed2);
	void pca180ServoTime(channel, 			//move a 180 servo on channel to angle for duration (ms)
		angle, duration);
	void pcaContServoTime(channel, speed,	//move cont servo on channel at speed for duration (ms)
		duration);
	void pcaDCMotorTime(channel, speed,		//move DC motor on channel at speed for duration (ms)
		duration);
	void pcaDC2MotorsTime(channel1, speed1,	//move 2 DC motors on channels at speeds for dur (ms)
		channel2, speed2, duration);


	------------------------------------
	IR Sensor
	------------------------------------
	int getIRChar(channel);				//returns char found with IR Sensor on channel (NOT YET IMPLEMENTED)
	void print_IR();					//prints everything in the IR buffer to the LCD
	char get_char(int index);			//prints char in buffer at index, or Z if index out of range



	-----------------------------------------------------------------------------
	Variables
	-----------------------------------------------------------------------------
	bool analogPins[16]; 					//16 analog pins (0-15) on
	bool digitalPins[54];					//54 analog pins (0-53)
											//*These are True(1) if attached, False(0) if free

	------------------------------------
	Ping Sensor Variables
	------------------------------------

	------------------------------------
	Conductivity Variables
	------------------------------------
	const int dPin1 = 12, dPin2 = 13;		//digital pins used
    const int aPin1 = 2,  aPin2 = 3;		//analog pins used

	------------------------------------
	Keypad Varibales
	------------------------------------
	bool entered;							//bool to tell if '#' has been entered
	int numEntered;							//num chars in buffer (max 15)
	char DATA[16];							//buffer to store inputted chars

	byte ROWS = 4;							//number of rows on keypad
	byte COLS = 4;							//number of columns on keypad
	char keys[4][4];						//physical layout of keypad

	byte rowPins[4] = {39,41,43,45}; 		//digital pins for ROWS
	byte colPins[4] = {47,49,51,53}; 		//digital pins for COLS

	Keypad* mykeypad;						//keypad object initalized in constructor
	
	------------------------------------
	LCD Variables 
	------------------------------------
	LiquidCrystal_I2C* lcd;					//LCD object initalizeed in constructor
								
	------------------------------------
	PCA9685 Variables
	------------------------------------
	Adafruit_PWMServoDriver* pwm;			//PWM object initalized in constructor, default address 0x40
	bool servos[16];						//16 pwm pins on PCA9658, values 0-15
											//*These are True(1) if attached, False(0) if free

	------------------------------------
	IR sensor Variables 
	------------------------------------
	unsigned char IRChar,IRCharBitMask,necState,buffer[8];
	int buffer_in,buffer_out;
	boolean receiverState;
	unsigned long prev_time,cur_time,ticks;
*/



#ifndef KNWRobot_h
#define KNWRobot_h

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
	int ID = 0;		//User defined
	int PIN = 0;	//physical pin
	char TYPE = 0;	//either analog(a), digital (d), or pca (p)
};


/**
 * KNWRobot Library 2.0, brought to you with love by the fabulous KNW TA's.
 * 
 * This library contains the functions needed to communicate with the various
 * hardware components of your robot. Defining what pins are being used,
 * reading data from environmental sensors, and defining motor / servo motions
 * are all defined here.
 */

class KNWRobot
{
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

		//CONDUCTIVITY SENSOR 
		
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
		int getInput(); //clears LCD

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
		int getInput(int row); //if you want to format which line to print on (0 or 1)
		
		byte rowPins[4] = {39,41,43,45}; 
		byte colPins[4] = {47,49,51,53}; 
		Keypad* mykeypad;

		//LCD 
		LiquidCrystal_I2C* lcd;
		void clearLCD();
		void moveCursor(int col, int row);
		void clearLine(int row);
		void printLCD(char* input);
		void printLCD(int input);
		void printLCD(long input);
		void printLCD(char input);

		//PCA/MOTOR (digital PWM)
		Adafruit_PWMServoDriver* pwm;
		int numServos;
		Component servos[16];
		bool setupServo(int id, int pin);
		int numMotors;
		Component motors[4];
		bool setupMotor(int id, int pin);
		void pcaRaw(int id, int pulseSize); 
		void pcaRawTime(int id, int pulseSize, int duration);
		void pcaStop(int id);
		void pcaStopAll();
		void pca180Servo(int id, int angle);
		void pcaContServo(int id, int speed);
		void pcaDCMotor(int id, int speed);
		void pcaDC2Motors(int id1, int speed1, int id2, int speed2);
		void pca180ServoTime(int id, int angle, int duration);
		void pcaContServoTime(int id, int speed, int duration);
		void pcaDCMotorTime(int id, int speed, int duration);
		void pcaDC2MotorsTime(int id1, int speed1, int id2, int speed2, int duration);

		//IR (digital)
		int numIR;
		Component irSensors[4];
		bool setupIR(int id, int pin);
		int scanIR(int id);
		unsigned char* getIR();
		unsigned char necState;
		int num_chars;
		unsigned long prev_time;

		unsigned char IRChar,IRCharBitMask,buffer[8];
		boolean receiverState = false;
		unsigned long cur_time,ticks;
	
	private:
		bool analogPins[16];
		bool digitalPins[54];
		bool pcaPins[16];

		Component pingSensors[8];
		Component bumpSensors[8];
		int numPings;
		int numBumps;
		int inclinePin;
		int tempPin;
		
		const int conductivityDigitalPin1 = 12;
		const int conductivityDigitalPin2 = 13;
    	const int conductivityAnalogPin1 = 2;
		const int conductivityAnalogPin2 = 3;

		bool entered;
		int numEntered;
		char DATA[17];
		byte ROWS = 4;
		byte COLS = 4;
		char keys[4][4] = {
  			{'1','2','3','A'},
  			{'4','5','6','B'},
  			{'7','8','9','C'},
  			{'*','0','#','D'}
		};
		
		bool checkPin(int pin, char type); //check to see if avalible
		int getPin(int id, char type); //from an ID
		void secretFunction();
};

#endif