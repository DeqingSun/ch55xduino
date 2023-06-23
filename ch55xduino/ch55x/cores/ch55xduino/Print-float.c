/*
 Print.c - Base class that provides print() and println()
 Copyright (c) 2008 David A. Mellis.  All right reserved.

 The float functions are moved to a separate file Print-float.c. This way
 we can keep the linker from pulling in all the float functions even when no
 floats are used in the sketch.

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

 Modified 23 November 2006 by David A. Mellis
 Modified 03 August 2015 by Chuck Todd
 Modified 30 December 2016 by Michael Mayer
 */

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
#include "Arduino.h"
#include <math.h>

#include "Print.h"

// Public Methods //////////////////////////////////////////////////////////////

uint8_t Print_print_fd(__data writefunc_p writefunc, __xdata double number,
                       __xdata uint8_t digits) {
  __xdata uint8_t n = 0;
  __xdata uint8_t i;
  __xdata unsigned long int_part;
  __xdata double remainder, rounding;
  __xdata unsigned int toPrint;

  if (isnan(number))
    return printStr(writefunc, "nan");
  if (isinf(number))
    return printStr(writefunc, "inf");
  if (number > 4294967040.0)
    return printStr(writefunc, "ovf"); // constant determined empirically
  if (number < -4294967040.0)
    return printStr(writefunc, "ovf"); // constant determined empirically

  // Handle negative numbers
  if (number < 0.0) {
    n += writefunc('-');
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  rounding = 0.5;
  for (i = 0; i < digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  int_part = (unsigned long)number;
  remainder = number - (double)int_part;
  n += printNumber(writefunc, int_part, 10);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += writefunc('.');
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    toPrint = (unsigned int)(remainder);
    n += printNumber(writefunc, toPrint, 10);
    remainder -= toPrint;
  }

  return n;
}
