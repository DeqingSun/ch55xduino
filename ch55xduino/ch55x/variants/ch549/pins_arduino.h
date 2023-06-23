/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

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

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "pins_arduino_include.h"

#ifndef _BV
#define _BV(X) (1 << (X))
#endif

__code uint8_t digital_pin_to_pwm_PGM[] = {
    NOT_ON_PWM, // PIN00
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, // not exist
    NOT_ON_PWM, // not exist

    NOT_ON_PWM, // PIN10
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, // not exist
    NOT_ON_PWM, // not exist

    NOT_ON_PWM, // PIN20
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, // not exist
    NOT_ON_PWM, // not exist

    NOT_ON_PWM, // PIN30
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, // not exist
    NOT_ON_PWM, // not exist

    NOT_ON_PWM, // PIN40
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, // not exist
    NOT_ON_PWM, // not exist

    NOT_ON_PWM, // PIN50
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, NOT_ON_PWM, NOT_ON_PWM,
    NOT_ON_PWM, // not exist
    NOT_ON_PWM, // not exist
};

__code uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    P0PORT, // PIN00
    P0PORT,     P0PORT, P0PORT, P0PORT, P0PORT, P0PORT, P0PORT,
    NOT_A_PORT, // not exist
    NOT_A_PORT, // not exist

    P1PORT, // PIN10
    P1PORT,     P1PORT, P1PORT, P1PORT, P1PORT, P1PORT, P1PORT,
    NOT_A_PORT, // not exist
    NOT_A_PORT, // not exist

    P2PORT, // PIN20
    P2PORT,     P2PORT, P2PORT, P2PORT, P2PORT, P2PORT, P2PORT,
    NOT_A_PORT, // not exist
    NOT_A_PORT, // not exist

    P3PORT, // PIN30
    P3PORT,     P3PORT, P3PORT, P3PORT, P3PORT, P3PORT, P3PORT,
    NOT_A_PORT, // not exist
    NOT_A_PORT, // not exist

    P4PORT, // PIN40
    P4PORT,     P4PORT, P4PORT, P4PORT, P4PORT, P4PORT, P4PORT,
    NOT_A_PORT, // not exist
    NOT_A_PORT, // not exist

    P5PORT, // PIN50
    P5PORT,     P5PORT, P5PORT, P5PORT, P5PORT, P5PORT, P5PORT,
    NOT_A_PORT, // not exist
    NOT_A_PORT, // not exist
};

__code uint8_t digital_pin_to_bit_mask_PGM[] = {
    _BV(0), // PIN00
    _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7),
    0, // not exist
    0, // not exist

    _BV(0), // PIN10
    _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7),
    0, // not exist
    0, // not exist

    _BV(0), // PIN20
    _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7),
    0, // not exist
    0, // not exist

    _BV(0), // PIN30
    _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7),
    0, // not exist
    0, // not exist

    _BV(0), // PIN40
    _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7),
    0, // not exist
    0, // not exist

    _BV(0), // PIN50
    _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7),
    0, // not exist
    0, // not exist
};

__code uint8_t digital_pin_to_channel_PGM[] = {
    NOT_ANALOG, // PIN00
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, // not exist
    NOT_ANALOG, // not exist

    NOT_ANALOG, // PIN10
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, // not exist
    NOT_ANALOG, // not exist

    NOT_ANALOG, // PIN20
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, // not exist
    NOT_ANALOG, // not exist

    NOT_ANALOG, // PIN30
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, // not exist
    NOT_ANALOG, // not exist

    NOT_ANALOG, // PIN40
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, // not exist
    NOT_ANALOG, // not exist

    NOT_ANALOG, // PIN50
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, NOT_ANALOG, NOT_ANALOG,
    NOT_ANALOG, // not exist
    NOT_ANALOG, // not exist
};

#endif
