/*
  HID Keyboard example


  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbHidMediaKeyboard/USBHIDMediaKeyboard.h"

#define BUTTON1_PIN 11

#define LED_BUILTIN 33

bool button1PressPrev = false;


void setup() {
  USBInit();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  //button 1 is mapped to MEDIA_VOL_UP, refer to USBHIDMediaKeyboard.h for more media keys
  bool button1Press = !digitalRead(BUTTON1_PIN);
  if (button1PressPrev != button1Press) {
    button1PressPrev = button1Press;
    if (button1Press) {
      Consumer_press(MEDIA_VOL_UP);
    } else {
      Consumer_release(MEDIA_VOL_UP);
    }
  }
  
  delay(50);  //naive debouncing
}
