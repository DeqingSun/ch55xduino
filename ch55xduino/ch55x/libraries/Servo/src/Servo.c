/*
 * Copyright (c) 2023 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Servo library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "Servo.h"

void digitalWriteHighDirectLut(uint8_t pin);
void digitalWriteLowDirectLut(uint8_t pin);

#if F_CPU > 24000000
#error Current clock is too fast for this version of library. Please use 24M or lower.
#endif

// each 5536 is 60000tick, 2.5ms in 24MHz
volatile __xdata uint16_t listRCAP2[16] = {5536, 5536, 5536, 5536,
                                           5536, 5536, 5536, 5536};
volatile __xdata uint8_t listRCAP2Ptr = 0;
volatile __xdata uint8_t listRCAP2Limit = (8 + 0);
//*8 *9 pin not exist
volatile __xdata uint8_t servoPin[16] = {9, 9, 9, 9, 9, 9, 9, 9};
volatile __xdata uint8_t servoPinNext = 9;
volatile __xdata uint8_t servoPinPrevious = 9;

__xdata uint16_t Servo_min = 1000;
__xdata uint16_t Servo_max = 2000;

void Timer2Interrupt(void) __interrupt {
  if (TF2) {
    TF2 = 0;

    __idata uint8_t listRCAP2PtrCache = listRCAP2Ptr;
    __idata uint16_t listRCAP2Cache = listRCAP2[listRCAP2PtrCache];

    RCAP2L = listRCAP2Cache & 0xff;
    RCAP2H = listRCAP2Cache >> 8;

    __idata uint8_t nextPinCache = servoPinNext;
    __idata uint8_t previousPinCache = servoPinPrevious;

    digitalWriteLowDirectLut(previousPinCache);
    digitalWriteHighDirectLut(nextPinCache);
    servoPinPrevious = nextPinCache;
    servoPinNext = servoPin[listRCAP2PtrCache];

    listRCAP2PtrCache++;
    if (listRCAP2PtrCache >= listRCAP2Limit) {
      listRCAP2PtrCache = 0;
    }

    listRCAP2Ptr = listRCAP2PtrCache;
  }
}

void Servo_wait_till_no_action() {
  while (1) {
    ET2 = 0;
    __idata uint8_t listRCAP2PtrCache = listRCAP2Ptr;
    ET2 = 1;
    if (listRCAP2PtrCache < 7) {
      break;
    }
  }
}

void Servo_init() {
  listRCAP2Limit = 8;
  __idata uint16_t valueRCAP2_2_5ms = (65536 - F_CPU * 0.0025);
  for (__idata uint8_t i = 0; i < 8; i++) {
    listRCAP2[i] = valueRCAP2_2_5ms;
  }
  for (__idata uint8_t i = 0; i < 8; i++) {
    servoPin[i] = 9;
  }

  T2CON = 0x00;
  // bTMR_CLK may be set by uart0, we keep it as is.
  T2MOD |= bTMR_CLK | bT2_CLK; // use Fsys for T2

  TL2 = 0;
  TH2 = 0;
  RCAP2L = 0;
  RCAP2H = 0;

  ET2 = 1;

  TR2 = 1;
}

uint8_t Servo_search_pin(uint8_t pin) {
  __idata uint8_t listRCAP2LimitCache = listRCAP2Limit;
  for (__idata uint8_t i = 8; i < listRCAP2LimitCache; i++) {
    if (servoPin[i] == pin) {
      return i;
    }
  }
  return 0;
}

bool Servo_attach(uint8_t pin) {
  if (pin > 37) {
    return false;
  }
  __idata uint8_t pinMod10 = pin % 10;
  if (pinMod10 == 9) {
    return false;
  }
  if (pinMod10 == 8) {
    return false;
  }
  if (listRCAP2Limit >= 16) {
    return false;
  }
  if (Servo_search_pin(pin) != 0) {
    return false;
  }
  Servo_wait_till_no_action();
  __idata uint16_t valueRCAP2_1_5ms = (65536 - F_CPU * 0.0015);
  __idata uint8_t listRCAP2LimitCache = listRCAP2Limit;
  servoPin[listRCAP2LimitCache] = pin;
  listRCAP2[listRCAP2LimitCache] = valueRCAP2_1_5ms;
  listRCAP2LimitCache++;
  listRCAP2Limit = listRCAP2LimitCache;
  return true;
}

bool Servo_detach(uint8_t pin) {
  __idata uint8_t pinIndex = Servo_search_pin(pin);
  if (pinIndex == 0) {
    return false;
  }
  Servo_wait_till_no_action();
  ET2 = 0;
  __idata uint8_t listRCAP2LimitCache = listRCAP2Limit;
  for (__idata uint8_t i = pinIndex; i < listRCAP2LimitCache - 1; i++) {
    servoPin[i] = servoPin[i + 1];
    listRCAP2[i] = listRCAP2[i + 1];
  }
  listRCAP2LimitCache--;
  listRCAP2Limit = listRCAP2LimitCache;
  ET2 = 1;
  return true;
}

bool Servo_writeMicroseconds(uint8_t pin, __xdata uint16_t pulseUs) {
  __idata uint8_t pinIndex = Servo_search_pin(pin);
  if (pinIndex == 0) {
    return false;
  }
  __idata uint16_t value = (65536 - ((F_CPU / 1000000) * pulseUs));
  ET2 = 0;
  listRCAP2[pinIndex] = value;
  ET2 = 1;
  return true;
}

bool Servo_write(uint8_t pin, __xdata int16_t value) {
  __idata uint8_t pinIndex = Servo_search_pin(pin);
  __idata uint16_t pulseValue;
  if (pinIndex == 0) {
    return false;
  }
  if (value <= 200) {
    // value is angle when value <= 200
    if (value < 0) {
      value = 0;
    }
    if (value > 180) {
      value = 180;
    }
    uint16_t pulseUs =
        Servo_min + ((Servo_max - Servo_min) * ((uint32_t)value)) / 180;
    pulseValue = (65536 - ((F_CPU / 1000000) * (pulseUs)));
  } else {
    // value is pulseUs when value > 200
    pulseValue = (65536 - ((F_CPU / 1000000) * value));
  }
  ET2 = 0;
  listRCAP2[pinIndex] = pulseValue;
  ET2 = 1;
  return true;
}
