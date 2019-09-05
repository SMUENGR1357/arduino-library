/*
	---------------------------------
	|		KNW Robot Library		| 
	---------------------------------
	KNWRobot.cpp - Library written to control robots for SMU KNW2300
	Controling Arduino: MEGA 2560 
	Author: Alexandria Hancock

	For more details on functions, look in KNWRobot.h 
*/

#include "KNWRobot.h" 
#include "Arduino.h"

#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Keypad.h"
#include "Adafruit_PWMServoDriver.h" 
#include "Servo.h"

//PCA DETAILS (Calibrated by Prof Matt Saari)
// Configuration parameters for each type of motor.
// This uses +/- 120 trying to get, but doesn't map directly because of variances in the physical 180 servos.
#define PCA_SERVO_180_INPUT_RANGE 120 
#define PCA_SERVO_180_CENTER 400
// Configured to be greater than +/- 90deg (240ish for 90degrees), because the students will need to calibrate the servos.
#define PCA_SERVO_180_WIDTH  320  

#define PCA_SERVO_CONTINUOUS_INPUT_RANGE 90
#define PCA_SERVO_CONTINUOUS_CENTER 334
#define PCA_SERVO_CONTINUOUS_WIDTH 35

#define PCA_DC_INPUT_RANGE 1023
#define PCA_DC_CENTER 337
#define PCA_DC_WIDTH 183

// Calculations based on above configuration.
#define PCA_SERVO_180_MIN  PCA_SERVO_180_CENTER-PCA_SERVO_180_WIDTH
#define PCA_SERVO_180_MAX  PCA_SERVO_180_CENTER+PCA_SERVO_180_WIDTH
#define PCA_SERVO_CONTINUOUS_MIN PCA_SERVO_CONTINUOUS_CENTER-PCA_SERVO_CONTINUOUS_WIDTH
#define PCA_SERVO_CONTINUOUS_MAX PCA_SERVO_CONTINUOUS_CENTER+PCA_SERVO_CONTINUOUS_WIDTH
#define PCA_DC_MIN PCA_DC_CENTER-PCA_DC_WIDTH
#define PCA_DC_MAX PCA_DC_CENTER+PCA_DC_WIDTH


//IR DETAILS
#define EVENT_RISING 1
#define EVENT_FALLING 2
#define EVENT_NONE 0
#define IR_WAIT 100000

//For Testing Mode, prints statements to terminal 
#define DEV_MODE false 


// ******************************************* //
// KNWRobot Constructor
// ******************************************* //
KNWRobot::KNWRobot(){
	//analog pins
	for(int i = 0; i < 4; i++){
		analogPins[i] = true;
	}
	
	//digital pins that can't be used (tx,rx)
	int commPins[] = {0,1,14,15,16,17,18,19,20,21}; //10 no-nos
	for(int i = 0; i < 10; i++){
		digitalPins[commPins[i]] = true;
	}

	//setting up keypad
	entered = false;
	numEntered = 0;

	mykeypad = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

	//default pins ROWS(39,41,43,45) COLS (47,49,51,53) but can change 
	for(int i = 0; i < ROWS; i++){
		digitalPins[rowPins[i]] = true;
	}

	for(int i = 0; i < COLS; i++){
		digitalPins[colPins[i]] = true;
	}


	//setting up LCD  
	lcd =  new LiquidCrystal_I2C(0x27,2,1,0,4,5,6,7,3,POSITIVE); 
	lcd->begin(16, 2);  // initialize the lcd 
  	lcd->home();       // go to the top line
  	lcd->print("SMU Lyle KNW2300");  


	//setting up PWM board
	pwm =  new Adafruit_PWMServoDriver();
	pwm->begin();
	pwm->setPWMFreq(60); // Analog servos run at ~60 Hz updates

	//setting up IR handling
	necState = 0;
	prev_time = 0;
  num_chars = 0;

	/* BEGIN Attach RISING Interrupt for Encoders on PCA9685 Board
    // Only the rising interrupt is used to maximize performance of catching
    // interrupt signals. There is no need to switch the attached interrupt function
    // after a RISING interrupt signal is detected.
    attachInterrupt(digitalPinToInterrupt(2), encoderTickInterrupt0, RISING);
    attachInterrupt(digitalPinToInterrupt(3), encoderTickInterrupt1, RISING);
    // END Attach RISING Interrupt for Encoders on PCA9685 Board
	*/
	if(DEV_MODE) {Serial.println("End of Constructor");}
}


// ******************************************* //
// TESTING FUNCTIONS (only use if DEV_MODE is on)
// Will print updates to Serial Monitor
// ******************************************* //
void KNWRobot::printAnalogPins(){
	if(DEV_MODE) {
		Serial.println("Analog Pins (0 if free, 1 if taken):");
		for(int i = 0; i < 16; i++){
			Serial.println(analogPins[i]);
		}
	}
}

void KNWRobot::printDigitalPins(){
	if(DEV_MODE) {
		Serial.println("Digital Pins (0 if free, 1 if taken):");
		for(int i = 0; i < 54; i++){
			Serial.println(digitalPins[i]);
		}	
	}
}

void KNWRobot::lcdTest(){
	lcd->clear();
	lcd->print("This B Empty");
}

void KNWRobot::secretFunction(){
	delay(1000);
	lcdTest();
	delay(2000);
	lcd->setCursor(0,1);
	lcd->print("YYYEEEEEETTT");
	delay(2000);
	//move robot... not done
}


// ******************************************* //
// Ping Sensor Functions
// ******************************************* //
//Check out this site for implementation details:
//http://arduino.cc/en/Tutorial/Ping
long KNWRobot::getPing(int channel){
	long duration, cm;
    
  	// The PING is triggered by a HIGH pulse of 2 or more microseconds.
  	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  	pinMode(channel, OUTPUT);
  	digitalWrite(channel, LOW);
  	delayMicroseconds(2);
  	digitalWrite(channel, HIGH);
  	delayMicroseconds(5);
  	digitalWrite(channel, LOW);

  	//The same pin is used to read the signal from the PING: a HIGH pulse
  	//whose duration is the time (in microseconds) from the sending of the ping
  	//to the reception of its echo off of an object.
  	pinMode(channel, INPUT);
  	duration = pulseIn(channel, HIGH); //returned in microseconds ms

  	//convert the time into a distance
  	//73.746 ms per inch (sound travels at 1130 ft/sec)
  	//29ish ms per cm (roughly 342 m/sec)
  	//duration is time there and back, so divid by 2
  	return cm = duration/29/2;
}


// ******************************************* //
// Conductivity Functions
// ******************************************* //
int KNWRobot::getConductivity(){
	const unsigned long seconds = 3;
    int reading1, reading2, result;
        
    //One period of the wave is 10ms. So we want to repeat it numMilliseconds/10ms times
    unsigned long loopCount = (seconds)*100ul;
        
    /*In case something is already attached to one of the digital pins
    if (pinsAttached[dPin1] || pinsAttached[dPin2]) {
      Serial.println("c 0");
      return;
    }*/
        
    pinMode(dPin1, OUTPUT);
    pinMode(dPin2, OUTPUT);
    digitalWrite(dPin1, HIGH);
    digitalWrite(dPin2, HIGH);
        
    //We make an alternating-phase square wave out of digital pins 12/13
    //For this to work, we needed simultaneous digital pin writes. Refer to
    //http://www.arduino.cc/en/Reference/PortManipulation
    for (unsigned long i = 0; i < loopCount; ++i) {
      //The AND turns off pin 13, OR turns on pin 12
      PORTB = B00010000 | (PORTB & B11011111);
      delay(5);
                
      //AND turns off pin 12, OR turns on pin 13
      PORTB = B00100000 | (PORTB & B11101111);
      delay(5);
    }  
      
    reading1 = analogRead(aPin1);
    reading2 = analogRead(aPin2);
    digitalWrite(dPin1, LOW);
    digitalWrite(dPin2, LOW);
        
    return result = abs(reading1 - reading2);
}


// ******************************************* //
// Keypad Functions
// ******************************************* //
int KNWRobot::getInput(){
	if(DEV_MODE) {Serial.println("In getInput");}
	int inInt = 0;
	char key;
  lcd->clear();

	//to get multi input from keypad into int
	while(!entered){
		key = mykeypad->getKey();
  		if (key){
  			if(DEV_MODE) {Serial.println(key);}
        lcd->setCursor(numEntered, 0);
        lcd->print(key);
    		if (key == '#' || numEntered == 15){ //max input is 16, neglects most recent and enters
      			DATA[numEntered] = '\0'; //null temrinator for atoi conversion
      			entered = true;
    		}
    		else if (key != '#'){
      			DATA[numEntered] = key;
      			numEntered++;
    		}
  		}

  		if (entered){
    		inInt = atoi(DATA);
    		memset(DATA, 0, sizeof(DATA));
    		numEntered = 0;
  		}
	} //end while

	entered = false; 
	if(DEV_MODE) {Serial.println(inInt);}
	return inInt;
}

// ******************************************* //
// LCD Functions
// ******************************************* //
void KNWRobot::clearLCD(){
	lcd->clear();
}

void KNWRobot::moveCursor(int col, int row){
	lcd->setCursor(col, row);
}

void KNWRobot::clearLine(int row){
	lcd->setCursor(0,row);
	lcd->print("                "); //16 characters
	lcd->setCursor(0,row);
}

void KNWRobot::printLCD(char* input){
	lcd->print(input);
}

void KNWRobot::printINT(int input){
  lcd->print(input);
}

void KNWRobot::printLONG(long input){
  lcd->print(input);
}

void KNWRobot::printCHAR(char input){
  lcd->print(input);
}

// ******************************************* //
// PCA9685 Board Functions
// ******************************************* //
void KNWRobot::pcaRaw(int channel, int pulseSize){
	pwm->setPWM(channel, 0, pulseSize);
} 

void KNWRobot::pcaRawTime(int channel, int pulseSize, int duration){
	pwm->setPWM(channel, 0, pulseSize);
	delay(duration);
	pwm->setPWM(channel, 0, 0);
}	

void KNWRobot::pcaStop(int channel){
	pwm->setPWM(channel,0,0);
}

void KNWRobot::pcaStopAll(){
	for(int channel = 0; channel < 16; channel++){
		pwm->setPWM(channel,0,0);
	}
}

void KNWRobot::pca180Servo(int channel, int angle){
	// Take input from [-90,90] and map to PWM duty cycle scale out of 4095
	int pulselen = map(angle, 0*PCA_SERVO_180_INPUT_RANGE, 2*PCA_SERVO_180_INPUT_RANGE, PCA_SERVO_180_MIN, PCA_SERVO_180_MAX); 

	// Shift to range of; PWM Signal: 1ms - 2ms will give full reverse to full forward, 1.5ms is neutral
	pulselen = constrain(pulselen, PCA_SERVO_180_MIN, PCA_SERVO_180_MAX); 
  	pwm->setPWM(channel, 0, pulselen);
}

void KNWRobot::pcaContServo(int channel, int speed){
	// Take input from [-90,90] and map to PWM duty cycle scale out of 4095 // Note, map() is used to flip direction.
	int pulselen = map(speed, -1*PCA_SERVO_CONTINUOUS_INPUT_RANGE, PCA_SERVO_CONTINUOUS_INPUT_RANGE, PCA_SERVO_CONTINUOUS_MAX, PCA_SERVO_CONTINUOUS_MIN); 

	// Shift to range of; PWM Signal: 1ms - 2ms will give full reverse to full forward, 1.5ms is neutral
 	pulselen = constrain(pulselen, PCA_SERVO_CONTINUOUS_MIN, PCA_SERVO_CONTINUOUS_MAX); 
 	pwm->setPWM(channel, 0, pulselen);
}

void KNWRobot::pcaDCMotor(int channel, int speed){
	// Take input from [-1023,1023] and map to PWM duty cycle scale out of 4095
	int pulselen = map(speed, -1*PCA_DC_INPUT_RANGE, PCA_DC_INPUT_RANGE, PCA_DC_MIN, PCA_DC_MAX);

	// Shift to range of; PWM Signal: 1ms - 2ms will give full reverse to full forward, 1.5ms is neutral 
  pulselen = constrain(pulselen, PCA_DC_MIN, PCA_DC_MAX); 
  
  pwm->setPWM(channel, 0, pulselen);
}

void KNWRobot::pcaDC2Motors(int channel1, int speed1, int channel2, int speed2){
	int pulselen1 = map(speed1, -1*PCA_DC_INPUT_RANGE, PCA_DC_INPUT_RANGE, PCA_DC_MIN, PCA_DC_MAX); 
  pulselen1 = constrain(pulselen1, PCA_DC_MIN, PCA_DC_MAX); 
  int pulselen2 = map(speed2, -1*PCA_DC_INPUT_RANGE, PCA_DC_INPUT_RANGE, PCA_DC_MIN, PCA_DC_MAX); 
  pulselen2 = constrain(pulselen2, PCA_DC_MIN, PCA_DC_MAX); 
  
  pwm->setPWM(channel1, 0, pulselen1);
  pwm->setPWM(channel2, 0, pulselen2);	
}

void KNWRobot::pca180ServoTime(int channel, int angle, int duration){
	// Take input from [-90,90] and map to PWM duty cycle scale out of 4095
	int pulselen = map(angle, 0*PCA_SERVO_180_INPUT_RANGE, 2*PCA_SERVO_180_INPUT_RANGE, PCA_SERVO_180_MIN, PCA_SERVO_180_MAX); 
  pulselen = constrain(pulselen, PCA_SERVO_180_MIN, PCA_SERVO_180_MAX);
  pwm->setPWM(channel, 0, pulselen);

  delay(duration);
 
  pwm->setPWM(channel, 0, 0);
}

void KNWRobot::pcaContServoTime(int channel, int speed, int duration){
	int pulselen = map(speed, -1*PCA_SERVO_CONTINUOUS_INPUT_RANGE, PCA_SERVO_CONTINUOUS_INPUT_RANGE, PCA_SERVO_CONTINUOUS_MAX, PCA_SERVO_CONTINUOUS_MIN); 
  pulselen = constrain(pulselen, PCA_SERVO_CONTINUOUS_MIN, PCA_SERVO_CONTINUOUS_MAX); 
  pwm->setPWM(channel, 0, pulselen);

  delay(duration);

  pwm->setPWM(channel, 0, 0);
}

void KNWRobot::pcaDCMotorTime(int channel, int speed, int duration){
	int pulselen = map(speed, -1*PCA_DC_INPUT_RANGE, PCA_DC_INPUT_RANGE, PCA_DC_MIN, PCA_DC_MAX); 
  pulselen = constrain(pulselen, PCA_DC_MIN, PCA_DC_MAX); 
  pwm->setPWM(channel, 0, pulselen);

	delay(duration);
  
  pwm->setPWM(channel, 0, 0);
}

void KNWRobot::pcaDC2MotorsTime(int channel1, int speed1, int channel2, int speed2, int duration){
	int pulselen1 = map(speed1, -1*PCA_DC_INPUT_RANGE, PCA_DC_INPUT_RANGE, PCA_DC_MIN, PCA_DC_MAX); 
  pulselen1 = constrain(pulselen1, PCA_DC_MIN, PCA_DC_MAX); 
  int pulselen2 = map(speed2, -1*PCA_DC_INPUT_RANGE, PCA_DC_INPUT_RANGE, PCA_DC_MIN, PCA_DC_MAX); 
  pulselen2 = constrain(pulselen2, PCA_DC_MIN, PCA_DC_MAX); 
  
  pwm->setPWM(channel1, 0, pulselen1);
  pwm->setPWM(channel2, 0, pulselen2);

  delay(duration);
  
	pwm->setPWM(channel1, 0, 0);
  pwm->setPWM(channel2, 0, 0);
}


// ******************************************* //
// Function to read IR character from sensor.
// ******************************************* //
int KNWRobot::getIRChar(int channel){
  //takes 13 ms per char to broadcast from a beacon
	 char reading;
    unsigned char event;
    //reset the buffer 
    memset(buffer, 0, sizeof(buffer));
    num_chars = 0;

    unsigned long IRCounter = 0;
    while( IRCounter < IR_WAIT) { // Cycle while we try to watch for a character.
        if (digitalRead(channel)) {  // Digital level from IR receiver will be inverted
            if (receiverState) event = EVENT_FALLING;
                else event = EVENT_NONE;
            receiverState = false;
        }
        else {
            if (!receiverState) event = EVENT_RISING;
                else event = EVENT_NONE;
            receiverState = true;
        }
        if (event != EVENT_NONE) {
            cur_time = micros();
            ticks = cur_time - prev_time;
            if (necState == 0) {  // Expecting rising edge of leading pulse
                if (event == EVENT_RISING) {
                    necState = 1;
                    //digitalWrite(ledPin,HIGH);
                }
            }
            else if (necState == 1) {  // Expecting falling edge of leading pulse
                if (event == EVENT_FALLING) {
                    if (ticks > 8900L) necState = 2;  // Check for leading pulse > 8.9msec
                    else {  // Stray short pulse found, reset NEC state
                        //digitalWrite(ledPin,LOW);
                        necState = 0;
                    }
                }
            }
            else if (necState == 2) {  // Expecting rising edge of first pulse after leading pulse
                if (event == EVENT_RISING) {
                    if (ticks > 3375L) {  // Check for space after leading pulse > 3.375 msec
                        IRCharBitMask = 0x80;
                        IRChar = 0;
                        necState = 3;
                    }
                    else {  // Space too short, reset NEC state to wait for another leading pulse
                        //digitalWrite(ledPin,LOW);
                        necState = 0;
                    }
                }
            }
            else if (necState == 3) {  // Expecting falling edge of data pulse
                if (event == EVENT_FALLING) {
                    if (ticks < 648) necState = 4;  // Check if data pulse width < 648 usec
                    else {  // Width too short, reset NEC state to wait for another leading pulse
                        //digitalWrite(ledPin,LOW);
                        necState = 0;
                    }
                }
            }
            else if (necState == 4) {  // Expecting rising edge of pulse after data pulse
                if (event == EVENT_RISING) {
                        if (ticks > 1120) {  // Record a '1' bit for space > 1120 usec
                            IRChar = IRChar | IRCharBitMask;
                        }
                    IRCharBitMask = IRCharBitMask >> 1;

                    if (IRCharBitMask == 0) {  // Check if eighth bit received and character complete (!!!!!)
                        buffer[num_chars] = IRChar;  // Record complete character received in circular output buffer
                        num_chars++;
                        //buffer_in = (buffer_in + 1) & 0x07;
                        //digitalWrite(ledPin,LOW);
                        necState = 0;  // Reset NEC state to wait for another leading pulse
                    }
                    else necState = 3;  // Wait for falling edge of data pulse
                }
            }
            prev_time = cur_time;
        }

        /*if (buffer_out != buffer_in) {  // Check if we detected a IR character.
            //sprintf(output, "i %c", buffer[buffer_out]);  // If we did, save it to output buffer.
            buffer_out = (buffer_out + 1) & 0x07;
            break;
        }*/
        IRCounter++;
    }
    //Serial.println(output);
    return num_chars;
}	

void KNWRobot::printIR(){
  int colprint = 0;
  lcd->clear();
  lcd->print("Characters Read");
  for(int i = 0; i  < num_chars; i++){
      lcd->setCursor(colprint, 1);
      lcd->print(buffer[i]);
      colprint += 2;
  }
}

char KNWRobot::get_char(int index){
  if(index < num_chars){
    return buffer[index];
  }
  else{
    return 'Z';
  }
}