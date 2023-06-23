/*
  Print.h - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

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

#ifndef Print_h
#define Print_h

// #include <inttypes.h>

// #include "WString.h"
// #include "Printable.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// for the function pointer to the actual write function
typedef uint8_t (*writefunc_p)(__data uint8_t c);

// abreviations of the actual function names, mostly for internal use
#define printBuf Print_print_sn
#define printStr Print_print_s
#define printNumber Print_print_ub
#define printInt Print_print_ib
#define println Print_println

// the more sophisticated Arduino-Style functions. Variants of the standard
// Serial.print() function: Separate impementations for different datatype
// to mimic polymorphism.

// print strings (C-Style or buffer contents)
uint8_t Print_print_s(__data writefunc_p writefunc, char *__xdata str);
uint8_t Print_print_sn(__data writefunc_p writefunc, uint8_t *__xdata buffer,
                       __xdata uint8_t size);

// print signed/unsigned values (char, short, int, long) as decimal values
uint8_t Print_print_i(__data writefunc_p writefunc, __xdata long n);
uint8_t Print_print_u(__data writefunc_p writefunc, __xdata unsigned long n);

// print signed/unsigned integer values (char, short, int, long) to base B
uint8_t Print_print_ib(__data writefunc_p writefunc, __xdata long n,
                       __xdata uint8_t base);
uint8_t Print_print_ub(__data writefunc_p writefunc, __xdata unsigned long n,
                       __xdata uint8_t base);

// print float values with two digits precision/specified number of digits
uint8_t Print_print_fd(__data writefunc_p writefunc, __xdata double number,
                       __xdata uint8_t digits);
#define Print_print_f(W, N) Print_print_fd(W, N, 2)
// inline uint8_t Print_print_f(writefunc_p writefunc, __xdata double number) {
//   return Print_print_fd(writefunc, number, 2);
// }

// Variants of the above with a newline added at the and:
uint8_t Print_println(__data writefunc_p writefunc);

#endif
