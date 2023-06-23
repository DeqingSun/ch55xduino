/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

void SPI_begin(void) {
  SPI0_SETUP = 0;
  SPI0_CK_SE = 0x20;
  SPI0_CTRL = bS0_MOSI_OE | bS0_SCK_OE;
}

void SPI_beginTransaction(uint16_t settings) {
  SPI0_CK_SE = settings & 0xFF;
  if (settings & (1 << 8)) {
    SPI0_SETUP &= ~bS0_BIT_ORDER;
  } else {
    SPI0_SETUP |= bS0_BIT_ORDER;
  }
  if (settings & (1 << 9)) {
    SPI0_CTRL |= bS0_MST_CLK;
  } else {
    SPI0_CTRL &= ~bS0_MST_CLK;
  }
}

inline uint8_t SPI_transfer(uint8_t data) {
  SPI0_DATA = data;
  while (S0_FREE == 0)
    ;
  return SPI0_DATA;
}

void SPI_end(void) { SPI0_CTRL &= ~(bS0_MISO_OE | bS0_MOSI_OE | bS0_SCK_OE); }