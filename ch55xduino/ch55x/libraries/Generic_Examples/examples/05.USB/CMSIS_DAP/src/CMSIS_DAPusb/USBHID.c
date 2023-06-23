// clang-format off
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
// clang-format on

extern __xdata uint8_t Ep0Buffer[];
extern __xdata uint8_t Ep1Buffer[];

volatile __xdata uint8_t USBByteCountEP1 =
    0; // Bytes of received data on USB endpoint

void USBInit() {
  USBDeviceCfg();         // Device mode configuration
  USBDeviceEndPointCfg(); // Endpoint configuration
  USBDeviceIntCfg();      // Interrupt configuration
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0; // Pre-use send length must be cleared
  UEP2_T_LEN = 0;
}

void USB_EP1_IN() {
  UEP1_T_LEN = 0; // No data to send anymore
  UEP1_CTRL =
      UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // Respond NAK by default
}

void USB_EP1_OUT() {
  if (U_TOG_OK) { // Discard unsynchronized packets
    USBByteCountEP1 = USB_RX_LEN;
    if (USBByteCountEP1) {
      UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES |
                  UEP_R_RES_NAK; // Respond NAK after a packet. Let main code
                                 // change response after handling.
    }
  }
}
