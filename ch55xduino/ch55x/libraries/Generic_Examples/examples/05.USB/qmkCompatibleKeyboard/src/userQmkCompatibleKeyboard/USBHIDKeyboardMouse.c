// clang-format off
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "via.h"
// clang-format on

// clang-format off
extern __xdata __at (EP0_ADDR) uint8_t Ep0Buffer[];
extern __xdata __at (EP1_ADDR) uint8_t Ep1Buffer[];
extern __xdata __at (EP2_ADDR) uint8_t Ep2Buffer[];
// clang-format on

__xdata uint8_t keyboardLedStatus = 0;

// Flag of whether upload pointer is busy
volatile __xdata uint8_t UpPoint1_Busy = 0;
volatile __xdata uint8_t UpPoint2_Busy = 0;

__xdata uint8_t HIDKey[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
__xdata uint8_t HIDMouse[4] = {0x0, 0x0, 0x0, 0x0};

typedef void (*pTaskFn)(void);

void delayMicroseconds(uint16_t us);

void USBInit() {
  USBDeviceCfg();         // Device mode configuration
  USBDeviceEndPointCfg(); // Endpoint configuration
  USBDeviceIntCfg();      // Interrupt configuration
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0; // Pre-use send length must be cleared
  UEP2_T_LEN = 0;
}

void USB_EP1_IN() {
  UEP1_T_LEN = 0;
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // Default NAK
  UpPoint1_Busy = 0;                                       // Clear busy flag
}

void USB_EP2_IN() {
  UEP2_T_LEN = 0;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // Default NAK
  UpPoint2_Busy = 0;                                       // Clear busy flag
}

void USB_EP1_OUT() {
  if (U_TOG_OK) // Discard unsynchronized packets
  {
    switch (Ep1Buffer[0]) {
    case 1:
      keyboardLedStatus = Ep1Buffer[1];
      break;
    default:
      break;
    }
  }
}

void USB_EP2_OUT() {
  if (U_TOG_OK) // Discard unsynchronized packets
  {
    raw_hid_receive();
  }
}

uint8_t USB_EP1_send(__data uint8_t reportID) {
  if (UsbConfig == 0) {
    return 0;
  }

  __data uint16_t waitWriteCount = 0;

  waitWriteCount = 0;
  while (UpPoint1_Busy) { // wait for 250ms or give up
    waitWriteCount++;
    delayMicroseconds(5);
    if (waitWriteCount >= 50000)
      return 0;
  }

  if (reportID == 1) {
    Ep1Buffer[64 + 0] = 1;
    for (__data uint8_t i = 0; i < sizeof(HIDKey); i++) { // load data for
                                                          // upload
      Ep1Buffer[64 + 1 + i] = HIDKey[i];
    }
    UEP1_T_LEN = 1 + sizeof(HIDKey); // data length
  } else if (reportID == 2) {
    Ep1Buffer[64 + 0] = 2;
    for (__data uint8_t i = 0; i < sizeof(HIDMouse);
         i++) { // load data for upload
      Ep1Buffer[64 + 1 + i] = ((uint8_t *)HIDMouse)[i];
    }
    UEP1_T_LEN = 1 + sizeof(HIDMouse); // data length
  } else if (reportID == 8) {
    Ep1Buffer[64 + 0] = 8;
    UEP1_T_LEN = 33;
  } else {
    UEP1_T_LEN = 0;
  }

  UpPoint1_Busy = 1;
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES |
              UEP_T_RES_ACK; // upload data and respond ACK

  return 1;
}

uint8_t USB_EP2_send() {
  if (UsbConfig == 0) {
    return 0;
  }

  __data uint16_t waitWriteCount = 0;

  waitWriteCount = 0;
  while (UpPoint2_Busy) { // wait for 250ms or give up
    waitWriteCount++;
    delayMicroseconds(5);
    if (waitWriteCount >= 50000)
      return 0;
  }

  UEP2_T_LEN = 32;

  UpPoint2_Busy = 1;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES |
              UEP_T_RES_ACK; // upload data and respond ACK

  return 1;
}

uint8_t Keyboard_quantum_modifier_press(__data uint8_t k) {
  HIDKey[0] |= k;
  USB_EP1_send(1);
  return 1;
}
uint8_t Keyboard_quantum_modifier_release(__data uint8_t k) {
  HIDKey[0] &= ~k;
  USB_EP1_send(1);
  return 1;
}

uint8_t Keyboard_quantum_regular_press(__data uint8_t k) {
  __data uint8_t i;
  // Add k to the key report only if it's not already present
  // and if there is an empty slot.
  if (HIDKey[2] != k && HIDKey[3] != k && HIDKey[4] != k && HIDKey[5] != k &&
      HIDKey[6] != k && HIDKey[7] != k) {

    for (i = 2; i < 8; i++) {
      if (HIDKey[i] == 0x00) {
        HIDKey[i] = k;
        break;
      }
    }
    if (i == 8) {
      // setWriteError();
      return 0;
    }
  }
  USB_EP1_send(1);
  return 1;
}

uint8_t Keyboard_quantum_regular_release(__data uint8_t k) {
  __data uint8_t i;
  // Test the key report to see if k is present.  Clear it if it exists.
  // Check all positions in case the key is present more than once (which it
  // shouldn't be)
  for (i = 2; i < 8; i++) {
    if (0 != k && HIDKey[i] == k) {
      HIDKey[i] = 0x00;
    }
  }

  USB_EP1_send(1);
  return 1;
}

void Keyboard_releaseAll(void) {
  for (__data uint8_t i = 0; i < sizeof(HIDKey); i++) { // load data for upload
    HIDKey[i] = 0;
  }
  USB_EP1_send(1);
}

uint8_t Keyboard_getLEDStatus() {
  // keyboardLedStatus is updated from USB_EP1_OUT
  return keyboardLedStatus;
}

uint8_t Mouse_press(__data uint8_t k) {
  HIDMouse[0] |= k;
  USB_EP1_send(2);
  return 1;
}

uint8_t Mouse_release(__data uint8_t k) {
  HIDMouse[0] &= ~k;
  USB_EP1_send(2);
  return 1;
}

uint8_t Mouse_click(__data uint8_t k) {
  Mouse_press(k);
  delayMicroseconds(10000);
  Mouse_release(k);
  return 1;
}

uint8_t Mouse_move(__data int8_t x, __xdata int8_t y) {
  HIDMouse[1] = x;
  HIDMouse[2] = y;
  USB_EP1_send(2);
  return 1;
}

uint8_t Mouse_scroll(__data int8_t tilt) {
  HIDMouse[3] = tilt;
  USB_EP1_send(2);
  return 1;
}
