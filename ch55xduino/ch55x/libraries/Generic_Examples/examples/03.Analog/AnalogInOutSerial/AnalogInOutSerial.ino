/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin P1.1.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin P3.4 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe
  modified 28 Feb 2017 for use with sduino
  by Michael Mayer
  modified 13 Jun 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = 11;  // Analog input pin that the potentiometer is attached to. You may use P1.1, P1.4, P1.5 and P3.2
const int analogOutPin = 34; // Analog output pin that the LED is attached to. You may use P1.5/P3.0 (mutually exclusive) and P3.4/P3.1 (mutually exclusive)

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // No need to init USBSerial

  //By default 8051 enable every pin's pull up resistor. Disable pull-up to get full input range. 
  pinMode(analogInPin,INPUT);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // No need to map, the are both 0~255:
  outputValue = sensorValue;
  // change the analog out value:
  analogWrite(analogOutPin, outputValue);

  // print the results to the Serial Monitor:
  USBSerial_print("sensor = ");
  USBSerial_print(sensorValue);
  USBSerial_print("\t output = ");
  USBSerial_println(outputValue);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
