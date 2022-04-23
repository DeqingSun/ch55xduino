/*
  Input Pull-up Serial

  This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a digital
  input on pin P1.1 and prints the results to the Serial Monitor.

  The circuit:
  - momentary switch attached from pin P1.1 to ground
  - built-in LED on pin P3.3

  Unlike pinMode(INPUT), there is no pull-down or pull-up resistor necessary. An internal
  70K-ohm resistor is pulled to 5V. This configuration causes the input to read
  HIGH when the switch is open, and LOW when it is closed.

  created 14 Mar 2012
  by Scott Fitzgerald
  modified 13 Feb 2017 for use with sduino
  by Michael Mayer
  modified 13 Jun 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/InputPullupSerial
*/

#include <Serial.h>

void setup() {
  // No need to init USBSerial

  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(11, INPUT_PULLUP);
  pinMode(33, OUTPUT);

}

void loop() {
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(11);
  //print out the value of the pushbutton
  USBSerial_println(sensorVal);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 3 when the
  // button's pressed, and off when it's not:
  if (sensorVal == HIGH) {
    digitalWrite(33, LOW);
  } else {
    digitalWrite(33, HIGH);
  }
}
