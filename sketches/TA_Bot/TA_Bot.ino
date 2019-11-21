/**
 * KNW2300 TA Bot code by Morgan VandenBerg
 * November 20, 2019
 */

#include "KNWRobot.h"

const int LEFT_MOTOR = 2;
const int RIGHT_MOTOR = 6;
const int LEFT_MOTOR_SPEED = 200;
const int RIGHT_MOTOR_SPEED = -150;
const int FRONT_BUMP = 7;

KNWRobot *robot;
bool startBump;

void setup()
{
    robot = new KNWRobot();
    robot->setupMotor(LEFT_MOTOR, LEFT_MOTOR);
    robot->setupMotor(RIGHT_MOTOR, RIGHT_MOTOR);
    robot->setupBump(FRONT_BUMP, FRONT_BUMP);
    robot->resetKeypad();
    robot->printLCD((char *)("TA Bot Initialized."));
    robot->getKeypadInput();
    startBump = robot->getBump(FRONT_BUMP);
}

void loop()
{
    robot->pcaDC2MotorsTime(LEFT_MOTOR, RIGHT_MOTOR_SPEED, RIGHT_MOTOR, LEFT_MOTOR_SPEED + 50, 3200);
    delay(300);
    robot->pcaDC2Motors(LEFT_MOTOR, LEFT_MOTOR_SPEED + 70, RIGHT_MOTOR, RIGHT_MOTOR_SPEED + 20);
    while (robot->getBump(FRONT_BUMP) == startBump)
        ;
    robot->pcaStopAll();
    delay(300);
}
