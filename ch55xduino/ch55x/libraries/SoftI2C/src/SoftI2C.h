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

void Wire_begin(uint8_t scl, __xdata uint8_t sda);
bool Wire_scan(uint8_t i2caddr);
bool Wire_writeBytes(uint8_t i2caddr, uint8_t* __xdata data, __xdata uint8_t bytes);
bool Wire_readBytes(uint8_t i2caddr, uint8_t* __xdata data, __xdata uint8_t bytes);
bool Wire_readRegister(uint8_t i2caddr, __xdata uint8_t regaddr, uint8_t* __xdata data, __xdata uint8_t bytes);
bool Wire_readRegister16bitAddr(uint8_t i2caddr, __xdata uint16_t regaddr, uint8_t* __xdata data, __xdata uint8_t bytes);

#endif
