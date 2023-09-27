/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>

// make sure to define prototypes for all used interrupts
// USBInterrupt does not need to saves the context
void USBInterrupt(void);
// Timer2Interrupt NEEDs to saves the context
void Timer2Interrupt(void) __interrupt(INT_NO_TMR2);
// GPIOInterrupt NEEDs to saves the context
void GPIOInterrupt(void) __interrupt(INT_NO_GPIO);

// unsigned char runSerialEvent;
void DeviceUSBInterrupt(void) __interrupt(INT_NO_USB) {
  // USB interrupt service
  USBInterrupt();
}

// clang-format off
// 0x08~0x0F belongs to register bank 1
__idata __at (0x08) volatile uint32_t timer0_overflow_count = 0;
__idata __at (0x0C) volatile uint8_t timer0_overflow_count_5th_byte = 0;
// clang-format on

// located in wiring.c, using register bank 1
void Timer0Interrupt(void) __interrupt(INT_NO_TMR0) __using(1);

void Uart0_ISR(void) __interrupt(INT_NO_UART0) {
  if (RI) {
    uart0IntRxHandler();
    RI = 0;
  }
  if (TI) {
    uart0IntTxHandler();
    TI = 0;
  }
}

void Uart1_ISR(void) __interrupt(INT_NO_UART1) {
#if defined(CH551) || defined(CH552)
  if (U1RI) {
    uart1IntRxHandler();
    U1RI = 0;
  }
  if (U1TI) {
    uart1IntTxHandler();
    U1TI = 0;
  }
#elif defined(CH559)
  uint8_t interruptStatus = SER1_IIR & 0x0f;
  switch (interruptStatus) {
  case U1_INT_RECV_RDY:
    uart1IntRxHandler();
    break;
  case U1_INT_THR_EMPTY:
    uart1IntTxHandler();
    break;
  }
#elif defined(CH549)
  if (SIF1 & bU1RI) {
    uart1IntRxHandler();
    SIF1 = bU1RI;
  }
  if (SIF1 & bU1TI) {
    uart1IntTxHandler();
    SIF1 = bU1TI;
  }
#endif
}

typedef void (*voidFuncPtr)(void);
extern __xdata voidFuncPtr intFunc[];
void INT0_ISR(void) __interrupt(INT_NO_INT0) { intFunc[0](); }
void INT1_ISR(void) __interrupt(INT_NO_INT1) { intFunc[1](); }

#if defined(CH551) || defined(CH552)
__xdata voidFuncPtr touchKeyHandler = NULL;
void TOUCHKEY_ISR(void) __interrupt(INT_NO_TKEY) {
  if (touchKeyHandler != NULL) {
    touchKeyHandler();
  }
}
#endif

void main(void) {
  init();

  //!!!initVariant();

  setup();

  for (;;) {
    loop();
    if (1) {
#ifndef USER_USB_RAM
      USBSerial_flush();
#endif
      // serialEvent();
    }
  }

  //    return 0;
}

unsigned char __sdcc_external_startup(void) __nonbanked { return 0; }
