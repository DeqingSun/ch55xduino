/*
  ReadAnalogVoltage

  Reads an analog input on pin P1.1, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin P1.1, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
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
  // read the input on analog pin 0, P1.1:
  int sensorValue = analogRead(11);
  // Convert the analog reading (which goes from 0 - 255) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 255.0);
  // print out the value you read:
  USBSerial_println(voltage);
  // or with precision:
  //USBSerial_println(voltage,1);

  delay(10);
}
