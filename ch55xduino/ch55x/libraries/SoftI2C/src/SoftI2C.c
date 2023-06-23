/*
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Soft I2C library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SoftI2C.h"

__xdata uint8_t scl_pin = 30;
__xdata uint8_t sda_pin = 31;

void I2CInit() {
  pinMode(scl_pin, OUTPUT_OD);
  pinMode(sda_pin, OUTPUT_OD);

  digitalWrite(sda_pin, HIGH);
  digitalWrite(scl_pin, HIGH);
}

void I2CStart() {
  digitalWrite(sda_pin, LOW);
  digitalWrite(scl_pin, LOW);
}

void I2CRestart() {
  digitalWrite(sda_pin, HIGH);
  digitalWrite(scl_pin, HIGH);
  digitalWrite(sda_pin, LOW);
  digitalWrite(scl_pin, LOW);
}

void I2CStop() {
  digitalWrite(scl_pin, LOW);
  digitalWrite(sda_pin, LOW);
  digitalWrite(scl_pin, HIGH);
  digitalWrite(sda_pin, HIGH);
}

void I2CAck() {
  digitalWrite(sda_pin, LOW);
  digitalWrite(scl_pin, HIGH);
  digitalWrite(scl_pin, LOW);
  digitalWrite(sda_pin, HIGH);
}

void I2CNak() {
  digitalWrite(sda_pin, HIGH);
  digitalWrite(scl_pin, HIGH);
  digitalWrite(scl_pin, LOW);
  digitalWrite(sda_pin, HIGH);
}

uint8_t I2CSend(uint8_t i2cData) {
  uint8_t i, ack_bit;
  for (i = 0; i < 8; i++) {
    if ((i2cData & 0x80) == 0)
      digitalWrite(sda_pin, LOW);
    else
      digitalWrite(sda_pin, HIGH);
    digitalWrite(scl_pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(scl_pin, LOW);
    i2cData <<= 1;
  }
  digitalWrite(sda_pin, HIGH);
  digitalWrite(scl_pin, HIGH);
  ack_bit = digitalRead(sda_pin);
  digitalWrite(scl_pin, LOW);
  return ack_bit;
}

uint8_t I2CRead() {
  uint8_t i, Data = 0;
  for (i = 0; i < 8; i++) {
    digitalWrite(scl_pin, HIGH);
    if (digitalRead(sda_pin))
      Data |= 1;
    if (i < 7)
      Data <<= 1;
    digitalWrite(scl_pin, LOW);
  }
  return Data;
}
