/*
  AnalogReadSerial

  Reads an analog input on pin P1.1, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin P1.1, and the outside pins to 5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

#include <Serial.h>

// the setup routine runs once when you press reset:
void setup() {
  // No need to init USBSerial

  // By default 8051 enable every pin's pull up resistor. Disable pull-up to get full input range.
  pinMode(11, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin P1.1. You may use P1.1, P1.4, P1.5 and P3.2
  int sensorValue = analogRead(11);
  // print out the value you read:
  USBSerial_println(sensorValue);
  delay(1);        // delay in between reads for stability
}
