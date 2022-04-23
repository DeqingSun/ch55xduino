/*
  DigitalReadSerial

  Reads a digital input on pin P1.1, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

#include <Serial.h>

// digital pin P1.1 has a pushbutton attached to it. Give it a name:
int pushButton = 11;

// the setup routine runs once when you press reset:
void setup() {
  // No need to init USBSerial
  
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  USBSerial_println(buttonState);
  delay(1);        // delay in between reads for stability
}
