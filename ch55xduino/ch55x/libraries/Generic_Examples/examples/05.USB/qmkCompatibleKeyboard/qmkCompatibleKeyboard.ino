/*
  HID Keyboard mouse combo example, compatible with QMK via protoco for remapping


  created 2023
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
#include "keyboardConfig.h"

//these variables will be externally refered by the via library, they shall match the json file
//on ch552 there is 128 Byte of data flash, so we keep the row*col*layer to be less than 64 (2Byte each key)

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

  {
    //initailize the data flash if all key maps are 0xFF
    __data uint8_t allConfigFF = 1;
    __data uint8_t dataLength = ROWS_COUNT * COLS_COUNT * LAYER_COUNT * 2;
    for (__data uint8_t i = 0; i < dataLength; i++) {
      if (eeprom_read_byte(i) != 0xFF) {
        allConfigFF = 0;
        break;
      }
    }
    if (allConfigFF){
      //write the default keymap
      const uint8_t defaultKeymap[] = {0x01,0x06,0x01,0x19,0x00,0x2B,0x08,0x06,0x08,0x19,0x00,0x2B};
      for (__data uint8_t i = 0; i < dataLength; i++) {
        eeprom_write_byte(i, defaultKeymap[i]);
      }
    }
  }

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
