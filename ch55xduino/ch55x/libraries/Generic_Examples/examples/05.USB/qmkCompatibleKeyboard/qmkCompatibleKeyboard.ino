/*
  HID Keyboard mouse combo example, compatible with QMK via protoco for remapping


  created 2023
  by Deqing Sun for use with CH55xduino

  This is a keyboard firmware that you can use usevia.app to remap the keys.

  The keyboard remap protocol is based on the QMK firmware
  The via impelementation is based on the CH552duinoKeyboard from yswallow

  In this example, the CH552 works as 3 key keyboard without matrix scanning. 
  There are 2 layers. If the host system is linux or windows, the first layer is used.
  If the host system is macos or ios, the second layer is used.
  When the dataflash is empty, the default keymap is used. Which is copy, paste and tab.

  cli board options: usb_settings=user266

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

#define BUTTON1_PIN 15
#define BUTTON2_PIN 16
#define BUTTON3_PIN 14

#define LED_BUILTIN 33

bool button1PressPrev = false;
bool button2PressPrev = false;
bool button3PressPrev = false;

unsigned long previousHelloMillis = 0;        // will store last time LED was updated
unsigned long previousKeyScanMillis = 0;

uint8_t layerInUse = 0;

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
      //write the default keymap (layer 0: Ctrl-C, Ctrl-V, Tab, layer 1: Cmd-C, Cmd-V, Tab)
      const uint8_t defaultKeymap[] = {0x01,0x06,0x01,0x19,0x00,0x2B,0x08,0x06,0x08,0x19,0x00,0x2B};
      for (__data uint8_t i = 0; i < dataLength; i++) {
        eeprom_write_byte(i, defaultKeymap[i]);
      }
    }
  }

  { //initialize the keys or matrix. 
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    pinMode(BUTTON3_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
  }
}

void loop() {

  via_process();

  if ((signed int)(millis() - previousKeyScanMillis) >= 50) { //naive debouncing
    // scan the keys or matrix. 
    previousKeyScanMillis = millis();

    __data uint8_t osDetected = detected_host_os();
    if ((osDetected == OS_LINUX) || (osDetected == OS_WINDOWS)) {
      layerInUse = 0;
    }else if ((osDetected == OS_MACOS) || (osDetected == OS_IOS)) {
      layerInUse = 1;
    }

    bool button1Press = !digitalRead(BUTTON1_PIN);
    if (button1PressPrev != button1Press) {
      button1PressPrev = button1Press;
      press_qmk_key(0,0,layerInUse,button1Press);
    }

    bool button2Press = !digitalRead(BUTTON2_PIN);
    if (button2PressPrev != button2Press) {
      button2PressPrev = button2Press;
      press_qmk_key(0,1,layerInUse,button2Press);
    }

    bool button3Press = !digitalRead(BUTTON3_PIN);
    if (button3PressPrev != button3Press) {
      button3PressPrev = button3Press;
      press_qmk_key(0,2,layerInUse,button3Press);
    }
  }

}
