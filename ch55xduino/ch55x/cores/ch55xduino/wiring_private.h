#ifndef WiringPrivate_h
#define WiringPrivate_h

// clang-format off
#include <stdint.h>
#include <stdio.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "Arduino.h"
// clang-format on

typedef void (*voidFuncPtr)(void);

#define EXTERNAL_INT_0 0
#define EXTERNAL_INT_1 1

#define EXTERNAL_NUM_INTERRUPTS 2

#endif