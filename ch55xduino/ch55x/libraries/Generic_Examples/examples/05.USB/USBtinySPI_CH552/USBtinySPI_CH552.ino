/*
  USBtinySPI_CH552

  This is an Adafruit USBtinySPI implementation on CH552 with debugWIRE feature.
  The debugWIRE protocol work is based on dcwbrown/dwire-debug 

  More info can be accessed on https://github.com/DeqingSun/unoDebugTestPackage

  Please note this firmware contains the Adafruit VID/PID,
  while the firmware is open source the VID/PID is not.
  To use in your own products, replace those values in USBconstant.c

  The author, Deqing Sun, got specific permission to use the VID/PID for educational purposes.
  The specific permission does not extend to other people distributing/selling the board.

  5 Nov 2022
  by Deqing Sun 

  This code is published under The GNU General Public License v3.0.

  cli board options: usb_settings=user266

*/

#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#if ( USER_USB_RAM < 212)
#error "This sketch needs a larger USB RAM"
#endif

#include "src/tinySpiDWire/USBInit.h"
#include "src/tinySpiDWire/hardwareShare.h"
#include "src/tinySpiDWire/debugWireFunc.h"

void Timer2Interrupt(void) __interrupt (INT_NO_TMR2)
{
    timer2IntrHandler();//debugwire related
}

extern __xdata uint8_t LineCoding[]; //lineCoding of CDC is located in this array
__xdata uint32_t oldBaudRate = 9600;
extern volatile __xdata uint8_t controlLineState;
__xdata uint8_t prevControlLineState = 0;

void setup() {
  USBInit();
  Serial0_begin(9600);

  //P3.3 LED
  P3_DIR_PU |= (1 << 3);
  P3_MOD_OC &= ~(1 << 3);
  P3_3 = 0;
  ledState = 0;
  //SPI to PUSHPULL
  P1_DIR_PU &= ~( (1 << 5) | (1 << 6) | (1 << 7) );
  P1_MOD_OC &= ~( (1 << 5) | (1 << 6) | (1 << 7) );
  SPI0_SETUP = 0;
  SPI0_CTRL = 0x60;    //mode 0
  //RESET P1.1
  P1_DIR_PU &= ~( (1 << 1) );
  P1_MOD_OC &= ~( (1 << 1) );
  P1_1 = 1;

  PT2 = 1; //timer 2 high priority
}

void loop() {
  debugWireProcess();

  //reset Arduino when DTR appears with P1.1
  if (prevControlLineState != controlLineState) {
    if ( ((controlLineState & 1) == 1) && ((prevControlLineState & 1) == 0) ) { //DTR appears
      //trigger a brief reset!
      P1_1 = 0;
      P1_DIR_PU |= ( (1 << 1) );
      delayMicroseconds(1000);
      P1_DIR_PU &= ~( (1 << 1) );
      P1_1 = 1;
    }
    prevControlLineState = controlLineState;
  }

  //serial adaptor
  if (USBSerial_available()) {
    char serialChar = USBSerial_read();
    Serial0_write(serialChar);
  }
  if (Serial0_available()) {
    char serialChar = Serial0_read();
    USBSerial_write(serialChar);
    USBSerial_flush();
  }

  __xdata uint32_t currentBaudRate = *((__xdata uint32_t *)LineCoding); //both linecoding and sdcc are little-endian

  if (oldBaudRate != currentBaudRate) {
    oldBaudRate = currentBaudRate;
    Serial0_begin(currentBaudRate);
  }
}
