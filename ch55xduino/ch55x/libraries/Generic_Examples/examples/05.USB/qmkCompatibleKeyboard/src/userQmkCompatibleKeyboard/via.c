// clang-format off
#include "via.h"
#include "USBhandler.h"
#include <Arduino.h>
#include "../../keyboardConfig.h"
#include "USBHIDKeyboardMouse.h"
// clang-format on

enum {
  ID_GET_PROTOCOL_VERSION = 0x01,
  ID_GET_KEYBOARD_VALUE,
  ID_SET_KEYBOARD_VALUE,
  ID_KEYMAP_GET_KEYCODE,
  ID_KEYMAP_SET_KEYCODE,
  ID_KEYMAP_RESET,
  ID_LIGHTING_SET_VALUE,
  ID_LIGHTING_GET_VALUE,
  ID_LIGHTING_SAVE,
  ID_EEPROM_RESET,
  ID_BOOTLOADER_JUMP,
  ID_MACRO_GET_COUNT,
  ID_MACRO_GET_BUFFER_SIZE,
  ID_MACRO_GET_BUFFER,
  ID_MACRO_SET_BUFFER,
  ID_MACRO_RESET, // 0x10
  ID_KEYMAP_GET_LAYER_COUNT,
  ID_KEYMAP_GET_BUFFER,
  ID_KEYMAP_SET_BUFFER,
  ID_UNHANDLED = 0xFF,
};

volatile __xdata uint8_t viaCmdReceived = 0;

void raw_hid_send() { USB_EP2_send(); }

void raw_hid_receive(void) { viaCmdReceived = 1; }

uint16_t dynamic_keymap_get_keycode(__data uint8_t layer, __xdata uint8_t row,
                                    uint8_t col) {
  __data uint8_t addr =
      ((layer * ROWS_COUNT * COLS_COUNT) + row * COLS_COUNT + col) * 2;
  return eeprom_read_byte(addr) << 8 | eeprom_read_byte(addr + 1);
}

void via_process(void) {
  if (viaCmdReceived == 0) {
    return;
  }
  viaCmdReceived = 0;

  memcpy(Ep2Buffer + 64, Ep2Buffer, 32);

  // Serial0_println("via");
  // Serial0_println(Ep2Buffer[0], HEX);
  // Serial0_println(Ep2Buffer[1], HEX);
  // Serial0_println(Ep2Buffer[2], HEX);

  switch (Ep2Buffer[0]) {
  case ID_GET_PROTOCOL_VERSION:
    Ep2Buffer[64 + 1] = 1;
    Ep2Buffer[64 + 2] = 0;
    break;
  case ID_GET_KEYBOARD_VALUE:
    for (uint8_t i = 2; i < 32; i++) {
      Ep2Buffer[64 + i] = 0;
    }
    break;
  case ID_SET_KEYBOARD_VALUE:
    break;
  case ID_KEYMAP_GET_KEYCODE: {
    __data uint16_t keycode =
        dynamic_keymap_get_keycode(Ep2Buffer[1], Ep2Buffer[2], Ep2Buffer[3]);
    Ep2Buffer[64 + 4] = (keycode >> 8) & 0xFF;
    Ep2Buffer[64 + 5] = keycode & 0xFF;
  } break;
  case ID_KEYMAP_SET_KEYCODE: {
    __data uint8_t layer = Ep2Buffer[1];
    __data uint8_t row = Ep2Buffer[2];
    __data uint8_t col = Ep2Buffer[3];
    __data uint8_t addr =
        ((layer * ROWS_COUNT * COLS_COUNT) + row * COLS_COUNT + col) * 2;

    eeprom_write_byte(addr, Ep2Buffer[4]);
    eeprom_write_byte(addr + 1, Ep2Buffer[5]);
  } break;
  case ID_MACRO_GET_COUNT:
    Ep2Buffer[64 + 1] = 0;
    break;
  case ID_MACRO_GET_BUFFER_SIZE:
    Ep2Buffer[64 + 2] = 0;
    Ep2Buffer[64 + 3] = 0;
    break;
  case ID_MACRO_GET_BUFFER:
    break;
  case ID_MACRO_SET_BUFFER:
    break;

  case ID_KEYMAP_GET_LAYER_COUNT: // 0x11
    Ep2Buffer[64 + 1] = LAYER_COUNT;
    break;
  case ID_KEYMAP_GET_BUFFER: // 0x12
  {
    __data uint16_t offset =
        (Ep2Buffer[1] << 8) | Ep2Buffer[2]; // well I know it overflow
    __data uint8_t size = Ep2Buffer[3];     // size <= 28
    for (uint8_t i = 0; i < size; i++) {
      Ep2Buffer[64 + 4 + i] = eeprom_read_byte(offset + i);
    }
  } break;
  case ID_KEYMAP_SET_BUFFER: {
    __data uint16_t offset =
        (Ep2Buffer[1] << 8) | Ep2Buffer[2]; // well I know it overflow
    __data uint8_t size = Ep2Buffer[3];     // size <= 28
    for (uint8_t i = 0; i < size; i++) {
      eeprom_write_byte(offset + i, Ep2Buffer[4 + i]);
    }
  } break;
  default:
    Ep2Buffer[64 + 0] = ID_UNHANDLED;
    break;
  }
  // delay(2);
  raw_hid_send();
}

void press_qmk_key(__data uint8_t row, __xdata uint8_t col,
                   __xdata uint8_t layer, __xdata uint8_t press) {
  __data uint16_t keycode = dynamic_keymap_get_keycode(layer, row, col);
  __data uint8_t application = (keycode >> 8) & 0x00FF;
  __data uint8_t code = keycode & 0x00FF;

  if (application >= 0x01 &&
      application <= 0x1F) {  // QMK: quantum.c, case QK_MODS ... QK_MODS_MAX:
    if (application & 0x10) { // QMK: quantum.c if (code & QK_RMODS_MIN) { //
                              // Right mod flag is set
      for (__data uint8_t i = 0; i < 4;
           i++) { // QK_LCTL:0x01 QK_LSFT QK_LALT QK_LGUI:0x08
        if (application & (1 << i)) {
          if (press) {
            Keyboard_quantum_modifier_press(1 << (4 + i));
          } else {
            Keyboard_quantum_modifier_release(1 << (4 + i));
          }
        }
      }
    } else {
      for (__data uint8_t i = 0; i < 4;
           i++) { // QK_LCTL:0x01 QK_LSFT QK_LALT QK_LGUI:0x08
        if (application & (1 << i)) {
          if (press) {
            Keyboard_quantum_modifier_press(1 << (0 + i));
          } else {
            Keyboard_quantum_modifier_release(1 << (0 + i));
          }
        }
      }
    }
  }

  if (press) {
    Keyboard_quantum_regular_press(keycode & 0x00FF);
  } else {
    Keyboard_quantum_regular_release(keycode & 0x00FF);
  }
}
