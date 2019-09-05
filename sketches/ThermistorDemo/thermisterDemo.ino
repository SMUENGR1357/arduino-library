#include <KNWRobot.h>
KNWRobot* bot;
const int TEMP_PIN = A0;            //Can be any analog pin
void setup() {
  bot = new KNWRobot();
  bot->clearLCD();
  char str[] = "ADC Code: ";
  bot->printLCD(str);
}

void loop() {
  bot->moveCursor(10,0);
  int code = analogRead(TEMP_PIN);  //Read ADC code from sensor
  bot->printINT(code);              //Print ADC code on LCD display
  delay(1000);                      //wait 1s before refreshing value
}
