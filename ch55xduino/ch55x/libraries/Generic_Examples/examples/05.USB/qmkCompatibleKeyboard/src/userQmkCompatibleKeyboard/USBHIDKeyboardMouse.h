#ifndef __USB_HID_KBD_H__
#define __USB_HID_KBD_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "osDetection.h"
#include "via.h"
// clang-format on

enum MOUSE_BUTTON {
  MOUSE_LEFT = 1,
  MOUSE_RIGHT = 2,
  MOUSE_MIDDLE = 4,
};

#ifdef __cplusplus
extern "C" {
#endif

void USBInit(void);

uint8_t USB_EP1_send(__data uint8_t reportID);
void USB_EP2_send();

uint8_t Keyboard_quantum_modifier_press(__data uint8_t k);
uint8_t Keyboard_quantum_modifier_release(__data uint8_t k);

uint8_t Keyboard_quantum_regular_press(__data uint8_t k);
uint8_t Keyboard_quantum_regular_release(__data uint8_t k);
void Keyboard_releaseAll(void);

uint8_t Keyboard_getLEDStatus();

uint8_t Mouse_press(__data uint8_t k);
uint8_t Mouse_release(__data uint8_t k);
uint8_t Mouse_click(__data uint8_t k);
uint8_t Mouse_move(__data int8_t x, __xdata int8_t y);
uint8_t Mouse_scroll(__data int8_t tilt);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
