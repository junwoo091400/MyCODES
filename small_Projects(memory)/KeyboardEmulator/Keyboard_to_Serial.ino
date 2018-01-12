/*
  Keyboard test

  For the Arduino Leonardo, Micro or Due

  Reads a byte from the serial port, sends a keystroke back.
  The sent keystroke is one higher than what's received, e.g. if you send a,
  you get b, send A you get B, and so forth.

  The circuit:
  - none

  created 21 Oct 2011
  modified 27 Mar 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/KeyboardSerial
*/

/*
Edited on January 12th, 2018. Because I didn't have USB-Keyboard around.
So I used this program to use my laptop keyboard -> Raspberry Pi 2.
-Junwoo HWANG
*/

#include <Keyboard.h>
//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(2, 3); // RX, TX
#define mySerial Serial1
void setup() {
  pinMode(4,INPUT_PULLUP);
  // open the serial port:
  mySerial.begin(9600);
  // initialize control over the keyboard:
  Keyboard.begin();
  delay(1000);
  /*for(char a = 'a'; a<='z' ; a++)
    Keyboard.write(a);*/
}

void loop() {
  if(digitalRead(4) == LOW){
    Keyboard.press(KEY_DOWN_ARROW);
    delay(500abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz);
    Keyboard.release(KEY_DOWN_ARROW);
  }
  // check for incoming serial data:
  if (mySerial.available() > 0) {
    // read incoming serial data:
    char inChar = mySerial.read();
    // Type the next ASCII value from what you received:
    Keyboard.write(inChar);
  }
}