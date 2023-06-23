/*
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Soft I2C library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SOFTI2C_H_INCLUDED
#define _SOFTI2C_H_INCLUDED

#include <Arduino.h>

extern __xdata uint8_t scl_pin;
extern __xdata uint8_t sda_pin;

void I2CInit();
void I2CStart();
void I2CRestart();
void I2CStop();
void I2CAck();
void I2CNak();
uint8_t I2CSend(uint8_t i2cData);
uint8_t I2CRead();

#endif
