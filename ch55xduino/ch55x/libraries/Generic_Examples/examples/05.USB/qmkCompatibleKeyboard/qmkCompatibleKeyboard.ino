/*
  HID Keyboard mouse combo example


  created 2022
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user148

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userQmkCompatibleKeyboard/USBHIDKeyboardMouse.h"

//these variables will be externally refered by the via library, they shall match the json file
__xdata uint8_t keyboard_matrix_row_count = 1;
__xdata uint8_t keyboard_matrix_col_count = 3;
__xdata uint8_t keyboard_matrix_layer_count = 1;

#define BUTTON1_PIN 30
#define BUTTON2_PIN 31
#define BUTTON3_PIN 32

#define LED_BUILTIN 33

bool button1PressPrev = false;
bool button2PressPrev = false;
bool button3PressPrev = false;

unsigned long previousHelloMillis = 0;        // will store last time LED was updated

void setup() {
  USBInit();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);


  Serial0_begin(9600);
  Serial0_println("Hello WORLD");
}

void loop() {
  unsigned long currentMillis = millis();
  if ((signed int)(currentMillis - previousHelloMillis) >= 2000) {
    previousHelloMillis = currentMillis;
    Serial0_println("Hello");
    Serial0_println((int)detected_host_os());
    
  }


  via_process();
  //delay(50);  //naive debouncing

}
