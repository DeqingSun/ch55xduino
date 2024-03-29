/*
 * Copyright (c) 2023 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Servo library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SERVO_H_INCLUDED
#define _SERVO_H_INCLUDED

#include <Arduino.h>

#define MIN_PULSE_WIDTH 544      // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH 2400     // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH 1500 // default pulse width when servo is attached

#define MAX_SERVOS 8

extern __xdata uint16_t Servo_min;
extern __xdata uint16_t Servo_max;

void Servo_wait_till_no_action();
void Servo_init();
bool Servo_attach(uint8_t pin);
bool Servo_detach(uint8_t pin);
bool Servo_writeMicroseconds(uint8_t pin, __xdata uint16_t pulseUs);
// if value is < 200 its treated as an angle, otherwise as pulse width in
// microseconds
bool Servo_write(uint8_t pin, __xdata int16_t value);

#endif
