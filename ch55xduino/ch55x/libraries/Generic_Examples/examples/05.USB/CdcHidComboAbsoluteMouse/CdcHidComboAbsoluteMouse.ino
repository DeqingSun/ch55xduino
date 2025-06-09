/*
  HID Absolute Mouse Example with CDC Serial

  This will send mouse movement back from USB CDC Serial command.

  created 2025
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user266

*/

#define LED_PIN 14

#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/CdcHidCombo/USBCDC.h"
#include "src/CdcHidCombo/USBHIDMouse.h"

//This is a fairly large array, store it in external memory with keyword __xdata
__xdata char recvStr[64];
uint8_t recvStrPtr = 0;
bool stringComplete = false;
bool ledState = false;
unsigned long lastBlinkTime = 0;
uint16_t lastX = 0;
uint16_t lastY = 0;
uint8_t lastButtons = 0;
unsigned long lastMouseCommandTime = 0;

uint16_t str_to_uint16(const char *str) {
  uint16_t result = 0;
  while (*str >= '0' && *str <= '9') {
    result = result * 10 + (*str - '0');
    str++;
  }
  return result;
}

void setup() {
  USBInit();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Turn off LED initially
}

void loop() {
  while (USBSerial_available()) {
    char serialChar = USBSerial_read();
    if ((serialChar == '\n') || (serialChar == '\r')) {
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
    // receive format "MOVE x,y,buttons"
    // where x and y are absolute coordinates in the range of 0 to 32767
    // and buttons is a bitmask of mouse buttons (0x01 for left, 0x02 for right, etc.)
    if (memcmp(recvStr, "MOVE ", 5) == 0) {
      // Parse the string to extract x, y, and buttons
      char *dataPtr = recvStr + 5;  // Skip "MOVE "
      char *xStr = strtok(dataPtr, ",");
      if (xStr != NULL) {
        uint16_t x = str_to_uint16(xStr);
        char *yStr = strtok(NULL, ",");
        if (yStr != NULL) {
          uint16_t y = str_to_uint16(yStr);
          char *buttonsStr = strtok(NULL, ",");
          if (buttonsStr != NULL) {
            uint8_t buttons = str_to_uint16(buttonsStr);
            if (x > 32767 || y > 32767) {
              USBSerial_println("Error: x and y must be in the range of 0 to 32767");
              USBSerial_flush();
            } else {
              USBSerial_print("Moving mouse to: ");
              USBSerial_print("x=");
              USBSerial_print(x);
              USBSerial_print(", y=");
              USBSerial_print(y);
              USBSerial_print(", buttons=");
              USBSerial_print(buttons);
              USBSerial_println("");
              Mouse_moveAbsolute(x, y, buttons);
              lastBlinkTime = millis();
              ledState = true;
              digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate mouse movement
              lastX = x;
              lastY = y;
              lastButtons = buttons;
              lastMouseCommandTime = millis();
            }
            USBSerial_flush();
          }
        }
      }
    }

    stringComplete = false;
    recvStrPtr = 0;
  }

  if (ledState && ((signed long)((millis() - lastBlinkTime)) > 200)) {
    ledState = false;
    digitalWrite(LED_PIN, LOW); // Turn off LED after a short time
  }

  if ((lastButtons != 0) && (signed long)((millis() - lastMouseCommandTime)) > 1000) {
    // If no mouse command for 1 seconds, reset mouse position to last known position
    lastButtons = 0; // Reset buttons
    Mouse_moveAbsolute(lastX, lastY, lastButtons);
    lastMouseCommandTime = millis();
  }
}
