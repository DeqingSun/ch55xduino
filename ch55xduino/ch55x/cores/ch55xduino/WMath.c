/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Wiring project - http://wiring.org.co
  Copyright (c) 2004-06 Hernando Barragan
  Modified 13 August 2006, David A. Mellis for Arduino - http://www.arduino.cc/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifdef __cplusplus
extern "C" {
#include "stdlib.h"
}
#else
#include "stdlib.h"
#endif

void randomSeed(__data unsigned long seed) {
  if (seed != 0) {
    srand(seed);
  }
}

long random(__data long howbig) {
  if (howbig == 0) {
    return 0;
  }
  return rand() % howbig;
}

long random_minmax(__data long howsmall, __xdata long howbig) {
  __data long diff;

  if (howsmall >= howbig) {
    return howsmall;
  }
  diff = howbig - howsmall;
  return random(diff) + howsmall;
}

long map(__data long x, __xdata long in_min, __xdata long in_max,
         __xdata long out_min, __xdata long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// unsigned int makeWord(unsigned int w) { return w; }
// unsigned int makeWord(unsigned char h, unsigned char l) { return (h << 8) |
// l; }
