/*
	---------------------------------
	|		KNW Robot Library		| 
	---------------------------------
	KNWRobot.h - Library written to control robots for SMU KNW2300
	Controling Arduino: MEGA 2560 
	Author: Alexandria Hancock

	When first starting to interface with the MEGA 2560 to get the robot
	runnning, a couple things must first be wired so you can use the library. 
	The numpad, which must be on pins {39,41,43,45,47,49,51,53}, the
	Adafruit PWM board, and the LCD 16x2 must all be correctly wired to check 
	if you have successfully included the library in your sketch, if you have,
	the message "SMU Lyle KNW2300" will appear on the first line of the lcd.

	To include this library in your sketch, you have to make a folder in the
	libraries folder in the arduino files. For example, for me I keep it in 
	the c drive in my Program Files folder. The folder must have the name 
	KNWRobot, and will have the .h, the .cpp, as well as a README.txt within it. 
	The path should look sommething like this:

	C:\Program Files (x86)\Arduino\libraries\KNWRobot

	After this step is done, open up a sketch in the Arduino IDE and type 
	#include <KNWRobot.h>

	This will give you access to run all of the functions we've written to safely
	run your robot. Keep in mind, you do have the freedom to edit the library 
	however you see fit (for example changing the pins for the numpad), but if you
	run into errors that you can't fix, we will tell you to delete the edited library
	and redownload it for it's original state.  


	-----------------------------------------------------------------------------
	Functions
	-----------------------------------------------------------------------------
	KNWRobot(); 							//constructor, initalizes the pins, numpad, LCD, PWM
											//the numpad has set pins.
	------------------------------------
	Ping Sensor Functions
	------------------------------------
	long getPing(int channel);				//returns ping distance in cm

	------------------------------------
	Conductivity Sensor Functions
	------------------------------------
	int getConductivity(); 					//returns conduct. between plates
											//analog pins 2,3 : digital pins 12,13
	------------------------------------
	Keypad Functions
	------------------------------------
	int getInput();							//returns up to 15 inputted chars as int 
											//'#' is the 'enter' key
											//treat 'A,B,C,D' as int values for quadrents
	------------------------------------
	LCD Functions
	------------------------------------
	void clearLCD();						//clears all contents of LCD
	void moveCursor(int col, int row);		//moves the cursor to (col, row)
											//col is 0-15, row is 0(top) or 1(bottom)
	void clearLine(int row);				//clears line, cursor set to begining of row
	void printLCD(char* input);				//will print input where cursor currently is
											//suggest moveCursor() first
											//the input should be  char str[] 
	void printINT(int input);
	void printLONG(long input);
	void printCHAR(char input);

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
	void pcaDCMotorEncoded(channel, speed, 	//NOT IMPLEMENTED
		encoderPin, encoderTicks);
	void pcaDC2MotorsEndcoded(channel1, 	//NOT IMPLEMENTED
		speed1, encoderPin1, encoderTicks1, 
		channel2, speed2, encoderPin2, encoderTicks2);


	------------------------------------
	IR Sensor
	------------------------------------
	int getIRChar(channel);				//returns char found with IR Sensor on channel (NOT YET IMPLEMENTED)
	void print_IR();					//prints everything in the IR buffer to the LCD
	char get_char(int index);			//prints char in buffer at index, or Z if index out of range

	------------------------------------
	Testing Functions -T is for Dev mode use, prints to Serial Plotter
	------------------------------------
	void printDigitalPins(); -T 			//println state of digitalPins
	void printAnalogPins(); -T 				//println state of analogPins
	void lcdTest();							//rendom message to lcd Test
	void secretFunction();					//vine references/moves motors



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
	boolean receiverState = false;
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

class KNWRobot{
	public:
		KNWRobot(); 
		void printAnalogPins();
		void printDigitalPins();
		void lcdTest();
		void secretFunction();

		//ping sensor Functions
		long getPing(int channel);

		//conductivity Functions
		int getConductivity();

		//Keypad Functions
		int getInput();

		//LCD Functions
		void clearLCD();
		void moveCursor(int col, int row);
		void clearLine(int row);
		void printLCD(char* input);
		void printINT(int input);
		void printLONG(long input);
		void printCHAR(char input);


		//PCA Functions
		void pcaRaw(int channel, int pulseSize); 
		void pcaRawTime(int channel, int pulseSize, int duration);
		void pcaStop(int channel);
		void pcaStopAll();
		void pca180Servo(int channel, int angle);
		void pcaContServo(int channel, int speed);
		void pcaDCMotor(int channel, int speed);
		void pcaDC2Motors(int channel1, int speed1, int channel2, int speed2);
		void pca180ServoTime(int channel, int angle, int duration);
		void pcaContServoTime(int channel, int speed, int duration);
		void pcaDCMotorTime(int channel, int speed, int duration);
		void pcaDC2MotorsTime(int channel1, int speed1, int channel2, int speed2, int duration);
		//all for motors with motor encoders
		/*void pcaDCMotorEncoded(int channel, int speed, int encoderPin, int encoderTicks);
		void pcaDC2MotorsEndcoded(int channel1, int speed1, int encoderPin1, int encoderTicks1, int channel2, int speed2, int encoderPin2, int encoderTicks2);
		//interrupt functions for encoded motors
		void encoderTickInterrupt0();
		void encoderTickInterrupt1();
		void queryEncoderTicks();
		void zeroEncoderTicks();
		*/

		//IR Functions
		int getIRChar(int channel);
		void printIR();
		char get_char(int index);

		//keeping track of pins
		bool analogPins[16]; //16 analog pins (0-15)
		bool digitalPins[54];//54 analog pins (0-53)

		//ping 

		//conductivity
		const int dPin1 = 12, dPin2 = 13;
    	const int aPin1 = 2,  aPin2 = 3;

		//keypad variables (change rowPins and colPins if needed)
		bool entered;
		int numEntered;
		char DATA[16];

		byte ROWS = 4;
		byte COLS = 4;
		char keys[4][4] = {
  			{'1','2','3','A'},
  			{'4','5','6','B'},
  			{'7','8','9','C'},
  			{'*','0','#','D'}
		};
		byte rowPins[4] = {39,41,43,45}; 
		byte colPins[4] = {47,49,51,53}; 

		Keypad* mykeypad;


		//LCD variables
		LiquidCrystal_I2C* lcd;


		//PCA board Variables
		Adafruit_PWMServoDriver* pwm;
		bool servos[16]; //0-15 motors on a single pwm board

		//IR Sensor Variables
		unsigned char necState;
		int num_chars;
		unsigned long prev_time;

		unsigned char IRChar,IRCharBitMask,buffer[8];
		boolean receiverState = false;
		unsigned long cur_time,ticks;
};

#endif