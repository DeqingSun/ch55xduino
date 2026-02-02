// clang-format off
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
// clang-format on

// clang-format off
extern __xdata __at (EP0_ADDR) uint8_t Ep0Buffer[];
extern __xdata __at (EP1_ADDR) uint8_t Ep1Buffer[];
extern __xdata __at (EP2_ADDR) uint8_t Ep2Buffer[];
extern __xdata __at (EP3_ADDR) uint8_t Ep3Buffer[];
// clang-format on

volatile __xdata uint8_t UpPoint3_Busy =
    0; // Flag of whether upload pointer is busy

__xdata uint8_t HIDMouse[5] = {0x0, 0x0, 0x0, 0x0, 0x0};

typedef void (*pTaskFn)(void);

void delayMicroseconds(uint16_t us);

void USB_EP3_IN() {
  UEP3_T_LEN = 0;
  UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // Default NAK
  UpPoint3_Busy = 0;                                       // Clear busy flag
}

uint8_t USB_EP3_send() {
  if (UsbConfig == 0) {
    return 0;
  }

  __data uint16_t waitWriteCount = 0;

  waitWriteCount = 0;
  while (UpPoint3_Busy) { // wait for 250ms or give up
    waitWriteCount++;
    delayMicroseconds(5);
    if (waitWriteCount >= 50000)
      return 0;
  }

  for (__data uint8_t i = 0; i < sizeof(HIDMouse);
       i++) { // load data for upload
    Ep3Buffer[0 + i] = HIDMouse[i];
  }

  __data uint8_t usbIntCopy;
  usbIntCopy = USB_INT_EN;
  USB_INT_EN &= ~bUIE_TRANSFER;  // Disable USB interrupts
  UEP3_T_LEN = sizeof(HIDMouse); // data length
  UpPoint3_Busy = 1;
  UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES |
              UEP_T_RES_ACK; // upload data and respond ACK
  USB_INT_EN = usbIntCopy;   // Restore USB interrupt state

  return 1;
}

void Mouse_moveAbsolute(uint16_t x, uint16_t y, uint8_t buttons) {
  // Prepare the mouse report
  HIDMouse[0] = buttons;                    // Set the button states
  HIDMouse[1] = (uint8_t)(x & 0xFF);        // X coordinate low byte
  HIDMouse[2] = (uint8_t)((x >> 8) & 0xFF); // X coordinate high byte
  HIDMouse[3] = (uint8_t)(y & 0xFF);        // Y coordinate low byte
  HIDMouse[4] = (uint8_t)((y >> 8) & 0xFF); // Y coordinate high byte
  USB_EP3_send();                           // Send the mouse report
}
