/*
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Touch key library for arduino CH552.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _TOUCHKEY_H_INCLUDED
#define _TOUCHKEY_H_INCLUDED
#include <Arduino.h>

void TouchKey_begin(uint8_t channelToEnableBitMask);
void TouchKey_end(void);
uint8_t TouchKey_Process();
uint8_t TouchKey_Get();

void TouchKey_SetMaxHalfDelta(uint8_t val);
void TouchKey_SetNoiseHalfDelta(uint8_t val);
void TouchKey_SetNoiseCountLimit(uint8_t val);
void TouchKey_SetFilterDelayLimit(uint8_t val);
void TouchKey_SetTouchThreshold(uint8_t val);
void TouchKey_SetReleaseThreshold(uint8_t val);

#endif