/*
  TouchKey Tune Parameter

  An example help user to understand effect of each parameter of the TouchKey library.
  This example only monitor Touch channel 1 (P1.1). 
  You may open Serial Plotter to see how the value changes when you touch P1.1

  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

*/

#include <TouchKey.h>

#define LED_BUILTIN 33

uint8_t prevCounter = 0;

//following data for debug only
extern volatile __xdata uint16_t touchRawValue[];
extern __xdata uint16_t touchBaseline[];
extern __xdata uint8_t touchThreshold;
extern __xdata uint8_t releaseThreshold;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  TouchKey_begin(1 << 1); //only do test on P1.1

  //refer to AN3891 MPR121 Baseline System for more details
  TouchKey_SetMaxHalfDelta(5);      //increase if sensor value are more noisy
  TouchKey_SetNoiseHalfDelta(2);    //If baseline need to adjust at higher rate, increase this value
  TouchKey_SetNoiseCountLimit(10);  //If baseline need to adjust faster, increase this value
  TouchKey_SetFilterDelayLimit(5);  //make overall adjustment slopwer

  TouchKey_SetTouchThreshold(100);  //Bigger touch pad can use a bigger value
  TouchKey_SetReleaseThreshold(80); //Smaller than touch threshold

}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t counter = TouchKey_Process();
  if (prevCounter != counter) {
    prevCounter = counter;  //this is a new cycle of touch data.
    uint16_t rawValueBuf;
    EA = 0;
    rawValueBuf = touchRawValue[1]; //prevent interrupt from changing it
    EA = 1;
    USBSerial_print("RawValue:");
    USBSerial_print(rawValueBuf);
    USBSerial_print(",touchThreshold:");
    USBSerial_print(touchBaseline[1] - touchThreshold);
    USBSerial_print(",releaseThreshold:");
    USBSerial_print(touchBaseline[1] - releaseThreshold);
    USBSerial_print(",Baseline:");
    USBSerial_println(touchBaseline[1]);
  }
  uint8_t touchResult = TouchKey_Get();
  if (touchResult & (1 << 1)) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

}
