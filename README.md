# KNWRobot - Robotics Control Library for SMU KNW2300
Controling Arduino: MEGA 2560 
	
Author: Alexandria Hancock

## Setup
Before using the library, you must first setup your laptop to be able to compile code written using Arduino C++. You will then need to add additional libraries that the code uses behind the scenes (they are included in this repo). Follow these steps:

1) Download the [Arduino IDE](https://www.arduino.cc/en/Main/Software) for your operating system. Go through the installation steps.
	* Windows: proceed with the default installation options. 
	* Mac OSX: drag and drop the application into your Applications directory (most often `/Applications`).

After finishing the installation, open the Arduino IDE. Continue selecting default options until you see a text editor with some empty functions. Close the application for now.

2) [Download the contents of this repository](https://github.com/SMUKNW2300/arduino-library/archive/master.zip). Unzip the files onto your computer.

3) Copy over the contents of the `lib` folder in this repo into the `libraries` folder of your Arduino installation.
	* Windows: Open File Explorer, go to `My Documents`. You should see an `Arduino` folder. Within that folder is a `libraries` folder (create one if it doesn't exist). Copy the directories in `lib` into the `libraries` folder.
	* Mac: Open finder, go to your Documents directory (/Users/[username]/Documents). You should see an `Arduino` folder. Within that folder is a `libraries` folder (create one if it doesn't exist). Copy the directories in `lib` into the `libraries` folder.

4) Copy the `knw` folder (located in `src`) into the `libraries` folder you opened in step 3.

5) Open the Arduino IDE again. At the top of the file, add the following line:

```cpp
#include <KNWRobot.h>
```

6) At the top left of the Arduino IDE, click the checkmark icon (Verify). This will compile the code. If you followed the steps correctly, you will see a message like `Done compiling`. If a step was missed, you will see error messages. If this happens, please ask a TA for help.



## Using the library
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
installation guide,
