#include <Keypad.h>

byte ROWS = 4;
byte COLS = 4;
char keys[4][4] = {
         {'1', '2', '3', 'A'},
         {'4', '5', '6', 'B'},
         {'7', '8', '9', 'C'},
         {'*', '0', '#', 'D'}};

     byte rowPins[4] = {39, 41, 43, 45};
     byte colPins[4] = {47, 49, 51, 53};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();

  if (key){
    Serial.println(key);
  }
}

