# KNWRobot - Robotics Control Library for SMU ENGR1357
Controling Arduino: MEGA 2560

Author: ENGR 1357 Staff and Faculty

# Table of Contents
- [Setup](#setup)
- [Using the Low-Power Sleep Library](#using-the-low-power-sleep-library)
  - [How the sleep library works](#how-the-sleep-library-works)
- [Using the Servo Library for Servos and Motors](#using-the-servo-library-for-servos-and-motors)
- [Using the Conductivity Module](#using-the-conductivity-module)
- [Using the New Ping Library for Ultrasonic Sensors](#using-the-new-ping-library-for-ultrasonic-sensors)
- [Using the IR Sensor Library](#using-the-ir-sensor-library)
- [Using the EEPROM Helper Library](#using-the-eeprom-helper-library)
- [Using the Temperature Probe](#using-the-temperature-probe)
- [Using the LCD Display](#using-the-lcd-display)
- [In-depth Function Document](#in-depth-function-documentation)

## Setup
Before using the library, you must first setup your laptop to be able to compile code written using Arduino C++.
You will then need to add additional libraries that the code uses behind the scenes (they are included in this
repo). Follow these steps:

1) Download the [Arduino IDE](https://www.arduino.cc/en/Main/Software) for your operating system. Go through the installation steps.
	* Windows: proceed with the default installation options.
	* Mac OSX: drag and drop the application into your Applications directory (most often `/Applications`).

After finishing the installation, open the Arduino IDE. Continue selecting default options until you see a text
editor with some empty functions. Close the application for now.

2) [Download the latest release of this repository](https://github.com/SMUENGR1357/arduino-library/archive/refs/heads/master.zip).
Unzip the files onto your computer.

3) Copy over the contents of the `lib` folder in this repo into the `libraries` folder of your Arduino installation.
	* Windows: Open File Explorer, go to `My Documents`. You should see an `Arduino` folder. Within that folder is a `libraries` folder (create one if it doesn't exist). Copy the directories in `lib` into the `libraries` folder.
	* Mac: Open finder, go to your Documents directory (/Users/[username]/Documents). You should see an `Arduino` folder. Within that folder is a `libraries` folder (create one if it doesn't exist). Copy the directories in `lib` into the `libraries` folder.

4) Copy over the contents of the `src` folder into the `libraries` folder you opened in step 3.

## Using the low-power sleep library

When running your arduino long-term in a data collection setting, you will need to use the included
sleep library. This library will take place of the built-in delay() function; the sleep library has
been optimized to limit power consumption when calling a sleep() function. To use it, add the following
at the top of your source code:

```cpp
#include <Sleep_n0m1.h>
```

Then in your code:

```cpp
Sleep sleep; // sleep library is now a global variable

void setup() {
	sleep.pwrDownMode(); // Future calls to sleep.sleepDelay() will put the arduino in a
	                     // very low power state while it sleeps
}

void loop() {
	// Do something
	sleep.sleepDelay(3000); // Low power sleep delay for 3 seconds (3000 milliseconds)
}
```

A more in-depth example can be found in the [sample_data_logger.ino file](https://github.com/SMUENGR1357/arduino-library/blob/master/samples/data_logger/sample_data_logger.ino).

### How the sleep library works

The sleep library has various different modes that can be explored in the [source repository](https://github.com/n0m1/Sleep_n0m1).
For this class, we are sticking to the lowest power setting, `pwrDownMode()`. If you read through that repository's documentation,
you'll see that the arduino has multiple different timers and chips that control various things. When using `pwrDownMode()`,
calling `sleepDelay()` shuts off _all_ chips except for a timer to wake the arduino back up. This saves a lot of power,
but it does make certain scenarios _appear_ to be disfunctional.

In the example below, we expect to move a servo from 0 degrees to 180 degrees, with 2 second delays in between. If you
actually run it on an arduino, you'll find that nothing appears to happen (your servo will not move). 

```cpp
#include <Servo.h>
#include <Sleep_n0m1.h>

Servo servo;
Sleep sleep;

void setup() {
  servo.attach(6);
  sleep.pwrDownMode();
}

void loop() {
  sleep.sleepDelay(2000);
  servo.write(180);
  // Try adding a delay(1000) here and see the change
  sleep.sleepDelay(2000);
  servo.write(0);
  // Add a delay here as well and see the change
}
```
This is because the code does a `servo.write()`, which generates a PWM signal that instructs the servo to move to that position.
The `sleep.sleepDelay(2000)` immediately afterwards shuts down the chip that generates the PWM, so your servo does not move.
A quick fix would be to add a `delay(1000)` immediately after `servo.write()`: `delay()` is an _active_ sleep, while `sleepDelay()`
is an `inactive` sleep.

There are two recommendations when using this library:
1) Use normal `delay()` calls when your arduino is _actively_ doing something, but needs to wait a second or two to let something
finish or doing something else (e.g. letting a servo move to a position)
2) Use `sleep.sleepDelay()` for when your arduino is _inactive_ for "long" periods of time (i.e. more than 5 seconds). If you
are moving a servo / motor / other physical part, be sure you're adding a short `delay()` before you call `sleepDelay()`,
otherwise it may appear to not move.

## Using the Servo Library for Servos and Motors

For servos, refer to the built-in [arduino servo library documentation](https://www.arduino.cc/reference/en/libraries/servo/write/).
Note that this also controls motors as well: "angles" specified in the servo-related function calls will dictate direction and
velocity of rotation for motors.

The following demonstrates a sample program:

```cpp
#include <Servo.h>

int ARM_SERVO_DIGITAL_PIN = 15;
int DRIVE_MOTOR_DIGITAL_PIN = 16;
Servo armServo;
Servo driveMotor;

void setup() {
	armServo.attach(ARM_SERVO_DIGITAL_PIN);
	driveMotor.attach(DRIVE_MOTOR_DIGITAL_PIN);
}

void loop() {
	armServo.write(0); // Move the servo to 0 degrees
	driveMotor.write(0); // Move the motor full speed in one direction

	delay(5000); // Let them run for 5 seconds

	armServo.write(180); // Move the servo to 180 degrees
	driveMotor.write(180); // Move the motor full speed in the other direction

	delay(5000); // Let them run for another 5 seconds

	armServo.write(90); // Move the servo to a midpoint
	driveMotor.write(90); // Make the motor stop. NOTE: This may need to be adjusted up or down a bit, depending on your motor

	delay(3000); // Let them stay for 3 seconds before starting over
}
```

## Using the Conductivity Module

To add the functions necessary to interface with your conductivity probe, add the following at the top
of your arduino source code:

```cpp
#include <conductivity.h>
```

You can then interface with your conductivity probe as such (for example, in your `loop()` function):

```cpp
void loop() {
	int probeReading = getConductivity();
	// Do something with probeReading
}
```

Refer to [this page](https://SMUENGR1357.github.io/arduino-library/conductivity_8h.html) for full documentation on the conductivity module.

## Using the New Ping Library for Ultrasonic Sensors

A new and improved library for your ping sensors has now been added into this repo. The library has a plethora of functions for you to use,
and you can find the full documentation for the library here: https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home.

However, you'll really just be using the function to provide distance in centimeters. The following code sample shows how to use the library.
Be sure that you have followed the setup steps above to have the `NewPing` library accessible in your Arduino IDE.

```cpp
#include <NewPing.h>

// If you're using a 4-pin ultrasonic sensor, then these are the middle two pins.
// If you're using a 3-pin ultrasonic sensor, then use the same value for both of these ints.
int TRIGGER_PIN = 12;
int ECHO_PIN = 11;
int MAX_PING_DISTANCE = 200; // centimeters

NewPing pingSensor(TRIGGER_PIN, ECHO_PIN, MAX_PING_DISTANCE);

void setup() {
	Serial.begin(9600);
}

void loop() {
	int distance = pingSensor.ping_cm();
	Serial.print("Distance in centimeters: ");
	Serial.println(distance);
	
	// distance will now contain the distance to some object in centimeters
}
```

## Using the IR Sensor Library

The infrared sensor library is a library that the ENGR staff provide for you. There are two functions that you
will use: `scanIR` and `getIR`. The following code snippet shows how to use the library.

For full documentation, refer to [this page](https://smuengr1357.github.io/arduino-library/infraredsensor_8h.html).

```cpp
#include <infraredsensor.h>
void loop() {
	int IRSensorPin = 20;
	int numCharsReadFromIR = scanIR(IRSensorPin);
	myRobot->printLCD("Chars read: ");
	myRobot->printLCD(charactersReadFromIR);
	
	// Now print the reading on the next line of the LCD
	char* IRCharacters = getIR();
	myRobot->moveCursor(0, 1);
	myRobot->printLCD(IRCharacters);
}
```

## Using the EEPROM Helper library

[OPTIONAL] To add the functions necessary to interface with the arduino's EEPROM memory, add the following
at the top of your arduino source code:

```cpp
#include <eepromhelper.h>
```

You can then interface with some helper functions for reading / writing to the EEPROM. To see a sample of the
functions in action, refer to [this source file in the samples directory](https://github.com/SMUENGR1357/arduino-library/blob/master/samples/data_logger/sample_data_logger.ino).
Note that these functions are used to read / write integer values for long-term storage. This may be enough for your needs,
but additional functions (and additional functionality) may be needed. Please refer to
[this page](https://SMUENGR1357.github.io/arduino-library/eepromhelper_8h.html) for details on how to use
the library, as well as [this source file](https://github.com/SMUENGR1357/arduino-library/blob/master/src/eepromhelper/eepromhelper.h)
if you want to see exactly what these functions do behind the scenes. This can help guide your implementation.
It is also recommended that you refer to [Arduino's EEPROM Reference page](https://www.arduino.cc/en/Reference/EEPROM).

## Using the Temperature Probe

There are no helper functions in this library specific to the temperature probe. Instead, you'll directly get
analog readings by using the built-in arduino [analogRead](https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)
function. From there, you will need to calibrate your sensor to convert readings from 10-bit precision voltages to temperature. Refer to [this documentation](https://smuengr1357.github.io/arduino-library/class_k_n_w_robot.html#a8d0ef37de9f7938515e46c25884d290a) for more info.
While it specifically refers to an inclinometer, the instructions around calibration are effectively the same.


## Using the LCD Display

[OPTIONAL] If your team wants to use an LCD component, then refer to the wiring guide on Canvas for how
to properly wire it to your Arduino. Then, refer to the
[sample LCD file](https://github.com/SMUENGR1357/arduino-library/blob/master/samples/lcd/sample_lcd.ino)
for some basic commands to write data onto the display

## In-depth Function Documentation

- Conductivity function documentation can be found by following [this link](https://smuengr1357.github.io/arduino-library/conductivity_8h.html)
- EEPROM helper documentation can be found by following [this link](https://smuengr1357.github.io/arduino-library/eepromhelper_8h.html)
- For semesters prior to Fall 2020: You can find the full KNWRobot class documentation, including
function documentation and examples,by following [this link](https://smuengr1357.github.io/arduino-library/).

## Using the KNWRobot library
Open the Arduino IDE again. At the top of the file, add the following line:

```cpp
#include <KNWRobot.h>
```

At the top left of the Arduino IDE, click the checkmark icon (Verify). This will compile the code. If you followed the steps correctly, you will see a message like `Done compiling`. If a step was missed, you will see error messages. If this happens, please ask a TA for help.

When first starting to interface with the MEGA 2560 to get the robot
runnning, a couple things must first be wired so you can use the library.

The following must be wired up:
- The number pad, which must be on pins {39,41,43,45,47,49,51,53}
- The Adafruit PWM board
- The LCD 16x2

Once those are all correctly wired (refer to documents on Canvas for how to
properly connect these components), you can then use this library. When writing your program, you will see "SMU Lyle KNW2300" appear on the first line of the LCD.

This will give you access to run all of the functions we've written to safely
run your robot.

## Updating the library
Since you are downloading the source code for this library, you have the freedom
to edit the library however you see fit. However, we recommend that you not edit
the files directly, but rather talk to a TA to update the source for everyone.
This will ensure consistency across teams and will make sure the TA's have tested
the new features.

If you edit the source code and run into errors that you cannot fix, we will tell
you to delete the edited library and re-download it to its original state. TA's
will also announce when this library is updated. Updating it requires you to
re-download this library (see step 2 above) and following steps 2 - 4 in the
installation guide.
