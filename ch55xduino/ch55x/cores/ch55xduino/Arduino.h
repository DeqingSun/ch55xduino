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

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

//#include <avr/pgmspace.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "pins_arduino_include.h"
//Macro-based digital IO fucntions
#include "wiring_digital_fast.h"

//!!!!#include "binary.h"

// FIXME: workarounds for missing features or unimplemented functions
// cancel out the PROGMEM attribute - used only for atmel CPUs
#define PROGMEM
void yield(void);

// we use pre-defined IRQ function the way wiring does
#define WIRING



#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define	OUTPUT_OD 0x03

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

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define FALLING 1

/*
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  #define DEFAULT 0
  #define EXTERNAL 1
  #define INTERNAL1V1 2
  #define INTERNAL INTERNAL1V1
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #define DEFAULT 0
  #define EXTERNAL 4
  #define INTERNAL1V1 8
  #define INTERNAL INTERNAL1V1
  #define INTERNAL2V56 9
  #define INTERNAL2V56_EXTCAP 13
#else  
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
#define INTERNAL1V1 2
#define INTERNAL2V56 3
#else
#define INTERNAL 3
#endif
#define DEFAULT 1
#define EXTERNAL 0
#endif
*/

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() (EA=1)
#define noInterrupts() (EA=0)

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesPerMillisecond() ( F_CPU / 1000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define byte(w) ((uint8_t)(w))
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define maskSet(value, mask) ((value) |= (mask))
#define maskClear(value, mask) ((value) &= ~(mask))


// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

#define BEGIN_CRITICAL		__critical {
#define END_CRITICAL		}


typedef unsigned int word;

#define bit(b) (1UL << (b))

typedef unsigned char boolean;
typedef unsigned char byte;
//typedef uint8_t byte;

void init(void);
//void initVariant(void);		// weak

//int atexit(void (*func)());	// __attribute__((weak));
//void serialEvent(void);		// weak
//extern unsigned char runSerialEvent;

void pinMode(uint8_t pin, __xdata uint8_t mode);
void digitalWrite(uint8_t pin, __xdata uint8_t val);
uint8_t digitalRead(uint8_t pin);
#if defined(CH559)
uint16_t analogRead(uint8_t pin);
#else
uint8_t analogRead(uint8_t pin);
#endif
void analogWrite(uint8_t pin, __xdata uint16_t val);

uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint16_t us);
//unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
//unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);

//void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
//uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), __xdata uint8_t mode);
void detachInterrupt(uint8_t interruptNum);

void setup(void);
void loop(void);

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

#define analogInPinToBit(P) (P)

// On the ATmega1280, the addresses of some of the port registers are
// greater than 255, so we can't store them in uint8_t's.


#ifdef SUPPORT_ALTERNATE_MAPPINGS
// helper function for STM8S: switch to the alternate pin functions
//void alternateFunction(uint8_t val);
#endif





//FIXME#include "WCharacter.h"
//FIXME#include "WString.h"
#include "HardwareSerial.h"

//uint16_t makeWord(uint16_t w);
//uint16_t makeWord(byte h, byte l);

//#define word(...) makeWord(__VA_ARGS__)

//unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
//unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);

//void tone(uint8_t _pin, unsigned int frequency, unsigned long duration);
//void noTone(uint8_t _pin);

// WMath prototypes
long random(long howbig);
long random_minmax(long howsmall, __xdata long howbig);
void randomSeed(unsigned long seed);
long map(long x, __xdata long in_min, __xdata long in_max, __xdata long out_min, __xdata long out_max);

inline unsigned int makeWord(unsigned char h, unsigned char l) { return (h << 8) | l; }


/*
 * The new interrupt numbers are a combination of the position in the
 * internal jump table (value in LSB) and the real STM8S-Interrupt number (MSB)
 */
#define INT_PORTA		( 0| (uint16_t)(ITC_IRQ_PORTA << 8))
#define INT_PORTB		( 1| (uint16_t)(ITC_IRQ_PORTB << 8))
#define INT_PORTC		( 2| (uint16_t)(ITC_IRQ_PORTC << 8))
#define INT_PORTD		( 3| (uint16_t)(ITC_IRQ_PORTD << 8))
#define INT_PORTE		( 4| (uint16_t)(ITC_IRQ_PORTE << 8))
#define INT_TIM1_CAPCOM		( 5| (uint16_t)(ITC_IRQ_TIM1_CAPCOM << 8))
#define INT_TIM1_OVF		( 6| (uint16_t)(ITC_IRQ_TIM1_OVF << 8))
#define INT_TIM2_CAPCOM		( 7| (uint16_t)(ITC_IRQ_TIM2_CAPCOM << 8))
#define INT_TIM2_OVF		( 8| (uint16_t)(ITC_IRQ_TIM2_OVF << 8))


//USB Serial functions. Don't exist in Arduino AVR core Arduino.h, may be moved later
bool USBSerial();
uint8_t USBSerial_print_n(uint8_t * __xdata buf, __xdata int len);
uint8_t USBSerial_write(char c);
void USBSerial_flush(void);
uint8_t USBSerial_available();
char USBSerial_read();
#include "Print.h"

// Generic selection for print
#include "genericPrintSelection.h"

// not quite understans X marco in sduino, use a lot define for now

#define USBSerial_print_s(P) ( Print_print_s(USBSerial_write,(P)) )
#define USBSerial_print_sn(P,Q) ( Print_print_sn(USBSerial_write,(P),(Q)) )
#define USBSerial_print_i(P) ( Print_print_i(USBSerial_write,(P)) )
#define USBSerial_print_u(P) ( Print_print_u(USBSerial_write,(P)) )
#define USBSerial_print_ib(P,Q) ( Print_print_ib(USBSerial_write,(P),(Q)) )
#define USBSerial_print_ub(P,Q) ( Print_print_ub(USBSerial_write,(P),(Q)) )
#define USBSerial_print_f(P) ( Print_print_f(USBSerial_write,(P)) )
#define USBSerial_print_fd(P,Q) ( Print_print_fd(USBSerial_write,(P),(Q)) )
#define USBSerial_print_c(P) ( (USBSerial_write(P)) )

#define USBSerial_println_only() ( Print_println(USBSerial_write) )
#define USBSerial_println_s(P) ( Print_print_s(USBSerial_write,(P)) + Print_println(USBSerial_write) )
#define USBSerial_println_sn(P,Q) ( Print_print_sn(USBSerial_write,(P),(Q)) + Print_println(USBSerial_write) )
#define USBSerial_println_i(P) ( Print_print_i(USBSerial_write,(P)) + Print_println(USBSerial_write) )
#define USBSerial_println_u(P) ( Print_print_u(USBSerial_write,(P)) + Print_println(USBSerial_write) )
#define USBSerial_println_ib(P,Q) ( Print_print_ib(USBSerial_write,(P),(Q)) + Print_println(USBSerial_write) )
#define USBSerial_println_ub(P,Q) ( Print_print_ub(USBSerial_write,(P),(Q)) + Print_println(USBSerial_write) )
#define USBSerial_println_f(P) ( Print_print_f(USBSerial_write,(P)) + Print_println(USBSerial_write) )
#define USBSerial_println_fd(P,Q) ( Print_print_fd(USBSerial_write,(P),(Q) ) + Print_println(USBSerial_write) )
#define USBSerial_println_c(P) ( (USBSerial_write(P)) + Print_println(USBSerial_write) )


#define Serial0_print_s(P) ( Print_print_s(Serial0_write,(P)) )
#define Serial0_print_sn(P,Q) ( Print_print_sn(Serial0_write,(P),(Q)) )
#define Serial0_print_i(P) ( Print_print_i(Serial0_write,(P)) )
#define Serial0_print_u(P) ( Print_print_u(Serial0_write,(P)) )
#define Serial0_print_ib(P,Q) ( Print_print_ib(Serial0_write,(P),(Q)) )
#define Serial0_print_ub(P,Q) ( Print_print_ub(Serial0_write,(P),(Q)) )
#define Serial0_print_f(P) ( Print_print_f(Serial0_write,(P)) )
#define Serial0_print_fd(P,Q) ( Print_print_fd(Serial0_write,(P),(Q)) )
#define Serial0_print_c(P) ( (Serial0_write(P)) )

#define Serial0_println_only() ( Print_println(Serial0_write) )
#define Serial0_println_s(P) ( Print_print_s(Serial0_write,(P)) + Print_println(Serial0_write) )
#define Serial0_println_sn(P,Q) ( Print_print_sn(Serial0_write,(P),(Q)) + Print_println(Serial0_write) )
#define Serial0_println_i(P) ( Print_print_i(Serial0_write,(P)) + Print_println(Serial0_write) )
#define Serial0_println_u(P) ( Print_print_u(Serial0_write,(P)) + Print_println(Serial0_write) )
#define Serial0_println_ib(P,Q) ( Print_print_ib(Serial0_write,(P),(Q)) + Print_println(Serial0_write) )
#define Serial0_println_ub(P,Q) ( Print_print_ub(Serial0_write,(P),(Q)) + Print_println(Serial0_write) )
#define Serial0_println_f(P) ( Print_print_f(Serial0_write,(P)) + Print_println(Serial0_write) )
#define Serial0_println_fd(P,Q) ( Print_print_fd(Serial0_write,(P),(Q) ) + Print_println(Serial0_write) )
#define Serial0_println_c(P) ( (Serial0_write(P)) + Print_println(Serial0_write) )


#define Serial1_print_s(P) ( Print_print_s(Serial1_write,(P)) )
#define Serial1_print_sn(P,Q) ( Print_print_sn(Serial1_write,(P),(Q)) )
#define Serial1_print_i(P) ( Print_print_i(Serial1_write,(P)) )
#define Serial1_print_u(P) ( Print_print_u(Serial1_write,(P)) )
#define Serial1_print_ib(P,Q) ( Print_print_ib(Serial1_write,(P),(Q)) )
#define Serial1_print_ub(P,Q) ( Print_print_ub(Serial1_write,(P),(Q)) )
#define Serial1_print_f(P) ( Print_print_f(Serial1_write,(P)) )
#define Serial1_print_fd(P,Q) ( Print_print_fd(Serial1_write,(P),(Q)) )
#define Serial1_print_c(P) ( (Serial1_write(P)) )

#define Serial1_println_only() ( Print_println(Serial1_write) )
#define Serial1_println_s(P) ( Print_print_s(Serial1_write,(P)) + Print_println(Serial1_write) )
#define Serial1_println_sn(P,Q) ( Print_print_sn(Serial1_write,(P),(Q)) + Print_println(Serial1_write) )
#define Serial1_println_i(P) ( Print_print_i(Serial1_write,(P)) + Print_println(Serial1_write) )
#define Serial1_println_u(P) ( Print_print_u(Serial1_write,(P)) + Print_println(Serial1_write) )
#define Serial1_println_ib(P,Q) ( Print_print_ib(Serial1_write,(P),(Q)) + Print_println(Serial1_write) )
#define Serial1_println_ub(P,Q) ( Print_print_ub(Serial1_write,(P),(Q)) + Print_println(Serial1_write) )
#define Serial1_println_f(P) ( Print_print_f(Serial1_write,(P)) + Print_println(Serial1_write) )
#define Serial1_println_fd(P,Q) ( Print_print_fd(Serial1_write,(P),(Q) ) + Print_println(Serial1_write) )
#define Serial1_println_c(P) ( (Serial1_write(P)) + Print_println(Serial1_write) )

//10K lifecycle DataFlash access on CH551/CH552.
#define eeprom_write_byte(ADDR,VAL) { DPL=(VAL);DPH=(ADDR);eeprom_write_byte_2_params_DPTR(); }
//SDCC is not efficent to convert 2 8bit data to 1 16bit data, se we use DPTR directly. The mismatch of parameter of the H and C is intentional
void eeprom_write_byte_2_params_DPTR();
uint8_t eeprom_read_byte (uint8_t addr);

#endif
