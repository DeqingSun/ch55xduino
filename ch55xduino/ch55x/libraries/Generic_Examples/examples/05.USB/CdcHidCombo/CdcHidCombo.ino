/*
  CDCinUserCode

  A simple example echoes back every line of data it receives.
  Also it prints how many lines it has echoed.

  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user148

*/


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/CdcHidCombo/USBCDC.h"
#include "src/CdcHidCombo/USBHIDKeyboard.h"

//This is a fairly large array, store it in external memory with keyword __xdata
__xdata char recvStr[64];
uint8_t recvStrPtr = 0;
bool stringComplete = false;
uint16_t echoCounter = 0;

#define BUTTON1_PIN 30

bool button1PressPrev = false;

void setup() {
  USBInit();

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
}

void loop() {
  while (USBSerial_available()) {
    char serialChar = USBSerial_read();
    if ((serialChar == '\n') || (serialChar == '\r') ) {
      recvStr[recvStrPtr] = '\0';
      if (recvStrPtr > 0) {
        stringComplete = true;
        break;
      }
    } else {
      recvStr[recvStrPtr] = serialChar;
      recvStrPtr++;
      if (recvStrPtr == 63) {
        recvStr[recvStrPtr] = '\0';
        stringComplete = true;
        break;
      }
    }
  }

  if (stringComplete) {
    USBSerial_print("ECHO:");
    USBSerial_println(recvStr);
    USBSerial_flush();
    stringComplete = false;
    recvStrPtr = 0;

    echoCounter++;
    USBSerial_print("echo count: ");
    USBSerial_println(echoCounter);
    USBSerial_flush();
  }

  bool button1Press = !digitalRead(BUTTON1_PIN);
  if (button1PressPrev != button1Press) {
    button1PressPrev = button1Press;
    if (button1Press) {
      Keyboard_press('a');
    } else {
      Keyboard_release('a');
    }
  }

}
