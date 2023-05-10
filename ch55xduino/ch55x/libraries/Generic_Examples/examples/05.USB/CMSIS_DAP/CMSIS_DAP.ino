/*
  CMSIS_DAP

  Modified form ljbfly's implementation, working but a lot to improve
  RST   -> P30
  SWCLK -> P31
  SWDIO -> P32
  
  These pins works in OC mode with pull-up resistor (70K). Should be safe to connect 3.3V part. 
  However, CH552 is designed to use 2 clock of OUTPUT on rising egde in classic OC mode. Double check if this is a problem for your target.

  nerdralph also reported reflections/ringing on some of SWD connection and cause error. Since CH552 doesn't have slew rate control, termination resistors may be required. 
  nerdralph used 68Ohm series termination resistors to fix his ringing issue with 28AWG Dupont jumper wire.

  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user148

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/CMSIS_DAPusb/DAP.h"


extern volatile __xdata uint8_t USBByteCountEP1;


void setup() {
  USBInit();
}

void loop() {
  uint8_t dapSendLength = 0;
  if (USBByteCountEP1) {
    dapSendLength = DAP_Thread();
    USBByteCountEP1 = 0;

    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_R_RES | UEP_R_RES_ACK; //enable receive

    UEP1_T_LEN = 64;
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK; //enable send
  }
}
