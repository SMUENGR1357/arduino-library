# KNWRobot - Robotics Control Library for SMU KNW2300
Controling Arduino: MEGA 2560

Author: KNW Staff and Faculty

# Table of Contents
- [Setup](#setup)
- [Using the Low-Power Sleep Library](#using-the-low-power-sleep-library)
- [Using the Servo Library](#using-the-servo-library)
- [Using the Conductivity Module](#using-the-conductivity-module)
- [Using the EEPROM Helper Library](#using-the-eeprom-helper-library)
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

2) [Download the latest release of this repository](https://github.com/SMUKNW2300/arduino-library/releases).
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

A more in-depth example can be found in the [sample_data_logger.ino file](https://github.com/SMUKNW2300/arduino-library/blob/master/samples/data_logger/sample_data_logger.ino).

## Using the servo library

For servos, refer to the built-in [arduino servo library documentation](https://www.arduino.cc/reference/en/libraries/servo/write/)

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

Refer to [this page](https://smuknw2300.github.io/arduino-library/conductivity_8h.html) for full documentation on the conductivity module.

## Using the EEPROM Helper library

[OPTIONAL] To add the functions necessary to interface with the arduino's EEPROM memory, add the following
at the top of your arduino source code:

```cpp
#include <eepromhelper.h>
```

You can then interface with some helper functions for reading / writing to the EEPROM. To see a sample of the
functions in action, refer to [this source file in the samples directory](https://github.com/SMUKNW2300/arduino-library/blob/master/samples/sample_data_logger.ino).
Note that these functions are used to read / write integer values for long-term storage. This may be enough for your needs,
but additional functions (and additional functionality) may be needed. Please refer to
[this page](https://smuknw2300.github.io/arduino-library/eepromhelper_8h.html) for details on how to use
the library, as well as [this source file](https://github.com/SMUKNW2300/arduino-library/blob/master/src/eepromhelper/eepromhelper.h)
if you want to see exactly what these functions do behind the scenes. This can help guide your implementation.
It is also recommended that you refer to [Arduino's EEPROM Reference page](https://www.arduino.cc/en/Reference/EEPROM).

## Using the LCD Display

[OPTIONAL] If your team wants to use an LCD component, then refer to the wiring guide on Canvas for how
to properly wire it to your Arduino. Then, refer to the
[sample LCD file](https://github.com/SMUKNW2300/arduino-library/blob/master/samples/lcd/sample_lcd.ino)
for some basic commands to write data onto the display

## In-depth Function Documentation

- Conductivity function documentation can be found by following [this link](https://smuknw2300.github.io/arduino-library/conductivity_8h.html)
- EEPROM helper documentation can be found by following [this link](https://smuknw2300.github.io/arduino-library/eepromhelper_8h.html)
- For semesters prior to Fall 2020: You can find the full KNWRobot class documentation, including
function documentation and examples,by following [this link](https://smuknw2300.github.io/arduino-library/).

## Using the library (Prior to Fall 2020)
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
