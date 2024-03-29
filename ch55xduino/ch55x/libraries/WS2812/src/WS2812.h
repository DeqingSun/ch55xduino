/*
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Touch key library for arduino CH552.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _WS2812_H_INCLUDED
#define _WS2812_H_INCLUDED

// clang-format off
#include <Arduino.h>
#include "template/WS2812_pins_header.h"
// clang-format on

#define set_pixel_for_RGB_LED(ADDR, INDEX, R, G, B)                            \
  {                                                                            \
    __xdata uint8_t *ptr = (ADDR) + ((INDEX)*3);                               \
    ptr[0] = (R);                                                              \
    ptr[1] = (G);                                                              \
    ptr[2] = (B);                                                              \
  };
#define set_pixel_for_GRB_LED(ADDR, INDEX, R, G, B)                            \
  {                                                                            \
    __xdata uint8_t *ptr = (ADDR) + ((INDEX)*3);                               \
    ptr[0] = (G);                                                              \
    ptr[1] = (R);                                                              \
    ptr[2] = (B);                                                              \
  };
#define set_pixel_for_RBG_LED(ADDR, INDEX, R, G, B)                            \
  {                                                                            \
    __xdata uint8_t *ptr = (ADDR) + ((INDEX)*3);                               \
    ptr[0] = (R);                                                              \
    ptr[1] = (B);                                                              \
    ptr[2] = (G);                                                              \
  };
#define set_pixel_for_GBR_LED(ADDR, INDEX, R, G, B)                            \
  {                                                                            \
    __xdata uint8_t *ptr = (ADDR) + ((INDEX)*3);                               \
    ptr[0] = (G);                                                              \
    ptr[1] = (B);                                                              \
    ptr[2] = (R);                                                              \
  };
#define set_pixel_for_BRG_LED(ADDR, INDEX, R, G, B)                            \
  {                                                                            \
    __xdata uint8_t *ptr = (ADDR) + ((INDEX)*3);                               \
    ptr[0] = (B);                                                              \
    ptr[1] = (R);                                                              \
    ptr[2] = (G);                                                              \
  };
#define set_pixel_for_BGR_LED(ADDR, INDEX, R, G, B)                            \
  {                                                                            \
    __xdata uint8_t *ptr = (ADDR) + ((INDEX)*3);                               \
    ptr[0] = (B);                                                              \
    ptr[1] = (G);                                                              \
    ptr[2] = (R);                                                              \
  };

#endif
