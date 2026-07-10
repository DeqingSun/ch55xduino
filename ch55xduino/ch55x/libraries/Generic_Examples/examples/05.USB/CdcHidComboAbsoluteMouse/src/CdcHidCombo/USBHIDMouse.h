#ifndef __USB_HID_MOUSE_ABS_H__
#define __USB_HID_MOUSE_ABS_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
// clang-format on

// Absolute Mouse HID interface

#ifdef __cplusplus
extern "C" {
#endif

// Example function prototypes for absolute mouse
void Mouse_moveAbsolute(uint16_t x, uint16_t y, uint8_t buttons);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
