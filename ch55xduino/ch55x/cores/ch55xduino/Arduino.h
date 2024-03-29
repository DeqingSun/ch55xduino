/*
  Arduino.h - Main include file for the Arduino SDK
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

#ifndef Arduino_h
#define Arduino_h

// clang-format off
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
// clang-format on

#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "pins_arduino_include.h"
// Macro-based digital IO fucntions
#include "wiring_digital_fast.h"

// FIXME: workarounds for missing features or unimplemented functions
// cancel out the PROGMEM attribute - used only for atmel CPUs
#define PROGMEM
void yield(void);

// we use pre-defined IRQ function the way wiring does
#define WIRING

#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define OUTPUT_OD 0x03

// undefine mathlib's pi if encountered
#ifdef PI
#undef PI
#endif
#ifdef HALF_PI
#undef HALF_PI
#endif
#ifdef TWO_PI
#undef TWO_PI
#endif

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL 0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define FALLING 1

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x) > 0 ? (x) : -(x))
#define constrain(amt, low, high)                                              \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x) * (x))

#define interrupts() (EA = 1)
#define noInterrupts() (EA = 0)

#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
#define clockCyclesPerMillisecond() (F_CPU / 1000L)
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) ((a)*clockCyclesPerMicrosecond())

#define byte(w) ((uint8_t)(w))
#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)                                         \
  (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define maskSet(value, mask) ((value) |= (mask))
#define maskClear(value, mask) ((value) &= ~(mask))

// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP()                                                                 \
  do {                                                                         \
    __asm__ volatile("nop");                                                   \
  } while (0)
#endif

#define BEGIN_CRITICAL __critical {
#define END_CRITICAL }

typedef unsigned int word;

#define bit(b) (1UL << (b))

typedef unsigned char boolean;
typedef unsigned char byte;
// typedef uint8_t byte;

void init(void);
// void initVariant(void);		// weak

// int atexit(void (*func)());	// __attribute__((weak));
// void serialEvent(void);		// weak
// extern unsigned char runSerialEvent;

/**
 * Configures the specified pin to behave either as an input or an output.
 * @param pin the Arduino pin number to set the mode of. On CH55xduino, it is
 * PortNumber*10 + PinNumber. For example, P1.4 is 14.
 * @param mode can be INPUT, OUTPUT, INPUT_PULLUP, OUTPUT_OD.
 */
void pinMode(__data uint8_t pin, __xdata uint8_t mode);
/**
 * Write a HIGH or a LOW value to a digital pin.
 * @param pin the Arduino pin number to write to. On CH55xduino, it is
 * PortNumber*10 + PinNumber. For example, P1.4 is 14.
 * @param val can be HIGH or LOW.
 */
void digitalWrite(__data uint8_t pin, __xdata uint8_t val);
/**
 * Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the Arduino pin number to read from. On CH55xduino, it is
 * PortNumber*10 + PinNumber. For example, P1.4 is 14.
 * @return HIGH or LOW (uint8_t).
 */
uint8_t digitalRead(__data uint8_t pin);
#if defined(CH559)
uint16_t analogRead(__data uint8_t pin);
#else
/**
 * Reads the value from the specified analog pin.
 * @param pin the Arduino pin number to read from. On CH552, you can do: 11, 14,
 * 15, 32.
 * @return the analog value read (int8_t) 255 is VCC.
 */
uint8_t analogRead(__data uint8_t pin);
#endif
/**
 * Writes an analog value (PWM wave) to a pin.
 * @param pin the Arduino pin number to write to. On CH552, you can do: 15 or
 * 30, 31 or 34.
 * @param val the duty cycle: between 0 (always off) and 256 (always on).
 */
void analogWrite(__data uint8_t pin, __xdata uint16_t val);

/**
 * Returns the number of milliseconds since the Arduino board began running the
 * current program. This number will overflow (go back to zero), after
 * approximately 50 days.
 * @return Number of milliseconds since the program started (uint32_t)
 */
uint32_t millis(void);
/**
 * Returns the number of microseconds since the Arduino board began running the
 * current program. This number will overflow (go back to zero), after
 * approximately 70 minutes.
 * @return Number of microseconds since the program started (uint32_t)
 */
uint32_t micros(void);
/**
 * Pauses the program for the amount of time (in miliseconds) specified as
 * parameter. (There are 1000 milliseconds in a second.)
 * @param ms the number of milliseconds to pause (uint32_t)
 */
void delay(__data uint32_t ms);
/**
 * Pauses the program for the amount of time (in microseconds) specified as
 * parameter. There are a thousand microseconds in a millisecond, and a million
 * microseconds in a second.
 * @param us the number of microseconds to pause (uint16_t)
 */
void delayMicroseconds(__data uint16_t us);
/**
 * Reads a pulse (either HIGH or LOW) on a pin. At this time the function only
 * work correctly in 24MHz clock.
 * @param pin the Arduino pin number to read from.
 * @param state if HIGH, waits for the pin to go from LOW to HIGH, then LOW, and
 * the HIGH period is measured. If LOW, waits for the pin to go from HIGH to
 * LOW, then HIGH, and the LOW period is measured.
 * @param timeout the number of milliseconds to wait for the pin to transition
 * before timing out.
 * @return the length of the pulse in microseconds (uint32_t) or 0 if no
 * complete pulse was received within the timeout.
 */
uint32_t pulseIn(uint8_t pin, __xdata uint8_t state, __xdata uint16_t timeout);

// void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t
// val); uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(__data uint8_t interruptNum,
                     __xdata void (*userFunc)(void), __xdata uint8_t mode);
void detachInterrupt(__data uint8_t interruptNum);

/**
 * The setup() function is called when a sketch starts. Use it to initialize
 * variables, pin modes, start using libraries, etc. The setup() function will
 * only run once, after each powerup or reset of the Arduino board.
 */
void setup(void);
/**
 * After creating a setup() function, which initializes and sets the initial
 * values, the loop() function does precisely what its name suggests, and loops
 * consecutively, allowing your program to change and respond. Use it to
 * actively control the Arduino board.
 */
void loop(void);

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

#define analogInPinToBit(P) (P)

// On the ATmega1280, the addresses of some of the port registers are
// greater than 255, so we can't store them in uint8_t's.

#ifdef SUPPORT_ALTERNATE_MAPPINGS
// helper function for STM8S: switch to the alternate pin functions
// void alternateFunction(uint8_t val);
#endif

// FIXME#include "WCharacter.h"
// FIXME#include "WString.h"
#include "HardwareSerial.h"

// uint16_t makeWord(uint16_t w);
// uint16_t makeWord(byte h, byte l);

// #define word(...) makeWord(__VA_ARGS__)

// unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
// unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);

// void tone(uint8_t _pin, unsigned int frequency, unsigned long duration);
// void noTone(uint8_t _pin);

// WMath prototypes
long random(__data long howbig);
long random_minmax(__data long howsmall, __xdata long howbig);
void randomSeed(__data unsigned long seed);
long map(__data long x, __xdata long in_min, __xdata long in_max,
         __xdata long out_min, __xdata long out_max);

inline unsigned int makeWord(unsigned char h, unsigned char l) {
  return (h << 8) | l;
}

// USB Serial functions. Don't exist in Arduino AVR core Arduino.h, may be moved
// later
bool USBSerial();
uint8_t USBSerial_print_n(uint8_t *__xdata buf, __xdata int len);
uint8_t USBSerial_write(__data char c);
void USBSerial_flush(void);
uint8_t USBSerial_available();
char USBSerial_read();
#include "Print.h"

// Generic selection for print
#include "genericPrintSelection.h"

// not quite understans X marco in sduino, use a lot define for now

#define USBSerial_print_s(P) (Print_print_s(USBSerial_write, (P)))
#define USBSerial_print_sn(P, Q) (Print_print_sn(USBSerial_write, (P), (Q)))
#define USBSerial_print_i(P) (Print_print_i(USBSerial_write, (P)))
#define USBSerial_print_u(P) (Print_print_u(USBSerial_write, (P)))
#define USBSerial_print_ib(P, Q) (Print_print_ib(USBSerial_write, (P), (Q)))
#define USBSerial_print_ub(P, Q) (Print_print_ub(USBSerial_write, (P), (Q)))
#define USBSerial_print_f(P) (Print_print_f(USBSerial_write, (P)))
#define USBSerial_print_fd(P, Q) (Print_print_fd(USBSerial_write, (P), (Q)))
#define USBSerial_print_c(P) ((USBSerial_write(P)))

#define USBSerial_println_only() (Print_println(USBSerial_write))
#define USBSerial_println_s(P)                                                 \
  (Print_print_s(USBSerial_write, (P)) + Print_println(USBSerial_write))
#define USBSerial_println_sn(P, Q)                                             \
  (Print_print_sn(USBSerial_write, (P), (Q)) + Print_println(USBSerial_write))
#define USBSerial_println_i(P)                                                 \
  (Print_print_i(USBSerial_write, (P)) + Print_println(USBSerial_write))
#define USBSerial_println_u(P)                                                 \
  (Print_print_u(USBSerial_write, (P)) + Print_println(USBSerial_write))
#define USBSerial_println_ib(P, Q)                                             \
  (Print_print_ib(USBSerial_write, (P), (Q)) + Print_println(USBSerial_write))
#define USBSerial_println_ub(P, Q)                                             \
  (Print_print_ub(USBSerial_write, (P), (Q)) + Print_println(USBSerial_write))
#define USBSerial_println_f(P)                                                 \
  (Print_print_f(USBSerial_write, (P)) + Print_println(USBSerial_write))
#define USBSerial_println_fd(P, Q)                                             \
  (Print_print_fd(USBSerial_write, (P), (Q)) + Print_println(USBSerial_write))
#define USBSerial_println_c(P)                                                 \
  ((USBSerial_write(P)) + Print_println(USBSerial_write))

#define Serial0_print_s(P) (Print_print_s(Serial0_write, (P)))
#define Serial0_print_sn(P, Q) (Print_print_sn(Serial0_write, (P), (Q)))
#define Serial0_print_i(P) (Print_print_i(Serial0_write, (P)))
#define Serial0_print_u(P) (Print_print_u(Serial0_write, (P)))
#define Serial0_print_ib(P, Q) (Print_print_ib(Serial0_write, (P), (Q)))
#define Serial0_print_ub(P, Q) (Print_print_ub(Serial0_write, (P), (Q)))
#define Serial0_print_f(P) (Print_print_f(Serial0_write, (P)))
#define Serial0_print_fd(P, Q) (Print_print_fd(Serial0_write, (P), (Q)))
#define Serial0_print_c(P) ((Serial0_write(P)))

#define Serial0_println_only() (Print_println(Serial0_write))
#define Serial0_println_s(P)                                                   \
  (Print_print_s(Serial0_write, (P)) + Print_println(Serial0_write))
#define Serial0_println_sn(P, Q)                                               \
  (Print_print_sn(Serial0_write, (P), (Q)) + Print_println(Serial0_write))
#define Serial0_println_i(P)                                                   \
  (Print_print_i(Serial0_write, (P)) + Print_println(Serial0_write))
#define Serial0_println_u(P)                                                   \
  (Print_print_u(Serial0_write, (P)) + Print_println(Serial0_write))
#define Serial0_println_ib(P, Q)                                               \
  (Print_print_ib(Serial0_write, (P), (Q)) + Print_println(Serial0_write))
#define Serial0_println_ub(P, Q)                                               \
  (Print_print_ub(Serial0_write, (P), (Q)) + Print_println(Serial0_write))
#define Serial0_println_f(P)                                                   \
  (Print_print_f(Serial0_write, (P)) + Print_println(Serial0_write))
#define Serial0_println_fd(P, Q)                                               \
  (Print_print_fd(Serial0_write, (P), (Q)) + Print_println(Serial0_write))
#define Serial0_println_c(P) ((Serial0_write(P)) + Print_println(Serial0_write))

#define Serial1_print_s(P) (Print_print_s(Serial1_write, (P)))
#define Serial1_print_sn(P, Q) (Print_print_sn(Serial1_write, (P), (Q)))
#define Serial1_print_i(P) (Print_print_i(Serial1_write, (P)))
#define Serial1_print_u(P) (Print_print_u(Serial1_write, (P)))
#define Serial1_print_ib(P, Q) (Print_print_ib(Serial1_write, (P), (Q)))
#define Serial1_print_ub(P, Q) (Print_print_ub(Serial1_write, (P), (Q)))
#define Serial1_print_f(P) (Print_print_f(Serial1_write, (P)))
#define Serial1_print_fd(P, Q) (Print_print_fd(Serial1_write, (P), (Q)))
#define Serial1_print_c(P) ((Serial1_write(P)))

#define Serial1_println_only() (Print_println(Serial1_write))
#define Serial1_println_s(P)                                                   \
  (Print_print_s(Serial1_write, (P)) + Print_println(Serial1_write))
#define Serial1_println_sn(P, Q)                                               \
  (Print_print_sn(Serial1_write, (P), (Q)) + Print_println(Serial1_write))
#define Serial1_println_i(P)                                                   \
  (Print_print_i(Serial1_write, (P)) + Print_println(Serial1_write))
#define Serial1_println_u(P)                                                   \
  (Print_print_u(Serial1_write, (P)) + Print_println(Serial1_write))
#define Serial1_println_ib(P, Q)                                               \
  (Print_print_ib(Serial1_write, (P), (Q)) + Print_println(Serial1_write))
#define Serial1_println_ub(P, Q)                                               \
  (Print_print_ub(Serial1_write, (P), (Q)) + Print_println(Serial1_write))
#define Serial1_println_f(P)                                                   \
  (Print_print_f(Serial1_write, (P)) + Print_println(Serial1_write))
#define Serial1_println_fd(P, Q)                                               \
  (Print_print_fd(Serial1_write, (P), (Q)) + Print_println(Serial1_write))
#define Serial1_println_c(P) ((Serial1_write(P)) + Print_println(Serial1_write))

// 10K lifecycle DataFlash access on CH551/CH552.
void eeprom_write_byte(__data uint8_t addr, __xdata uint8_t val);
uint8_t eeprom_read_byte(__data uint8_t addr);

#endif
