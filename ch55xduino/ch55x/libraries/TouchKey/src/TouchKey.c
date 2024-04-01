/*
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH552
 * port) Touch key library for arduino CH552.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "TouchKey.h"

typedef void (*voidFuncPtr)(void);
extern __xdata voidFuncPtr touchKeyHandler;

volatile __xdata uint16_t touchRawValue[6];
volatile __xdata uint8_t touchISRCounter = 0;
__xdata uint8_t channelEnabled = 0;

__xdata uint8_t touchCounterProcessed = 0;
__xdata uint8_t processIndex = 0;

__xdata uint8_t touchCycleCounter = 0;

__xdata uint16_t touchBaseline[6];
__xdata uint8_t touchBaselineNoiseCount[6];
__xdata uint8_t prevDiffBaselinePos;
__xdata uint8_t touchNextCalibrateCycleCounter = 0;

__xdata uint8_t touchMaxHalfDelta;
__xdata uint8_t touchNoiseHalfDelta;
__xdata uint8_t touchNoiseCountLimit;
__xdata uint8_t touchFilterDelayLimit;
__xdata uint8_t touchThreshold;
__xdata uint8_t releaseThreshold;

__xdata uint8_t touchKeyPressed;

#pragma save
#pragma nooverlay
void TouchKey_ISR_Handler(void) {
  uint8_t index = TKEY_CTRL & 0x07;
  touchRawValue[index - 1] = TKEY_DAT;

  do {
    index++;
    if (index > 6)
      index = 1;
  } while ((channelEnabled & (1 << (index - 1))) == 0);

  TKEY_CTRL = bTKC_2MS | index;
  touchISRCounter++;
}
#pragma restore

// TIN0(P1.0), TIN1(P1.1), TIN2(P1.4), TIN3(P1.5), TIN4(P1.6), TIN5(P1.7)
void TouchKey_begin(uint8_t channelToEnableBitMask) {

  if ((channelToEnableBitMask & 0b111111) == 0) {
    return;
  }

  channelEnabled = channelToEnableBitMask;

  // set IO to input
  if (channelEnabled & (1 << 0)) {
    P1_DIR_PU &= ~(1 << 0);
    P1_MOD_OC &= ~(1 << 0);
  }
  if (channelEnabled & (1 << 1)) {
    P1_DIR_PU &= ~(1 << 1);
    P1_MOD_OC &= ~(1 << 1);
  }
  if (channelEnabled & (1 << 2)) {
    P1_DIR_PU &= ~(1 << 4);
    P1_MOD_OC &= ~(1 << 4);
  }
  if (channelEnabled & (1 << 3)) {
    P1_DIR_PU &= ~(1 << 5);
    P1_MOD_OC &= ~(1 << 5);
  }
  if (channelEnabled & (1 << 4)) {
    P1_DIR_PU &= ~(1 << 6);
    P1_MOD_OC &= ~(1 << 6);
  }
  if (channelEnabled & (1 << 5)) {
    P1_DIR_PU &= ~(1 << 7);
    P1_MOD_OC &= ~(1 << 7);
  }

  touchMaxHalfDelta = 5;
  touchNoiseHalfDelta = 2;
  touchNoiseCountLimit = 10;
  touchFilterDelayLimit = 5;

  touchThreshold = 100;
  releaseThreshold = 80;

  touchKeyHandler = TouchKey_ISR_Handler;

  // sample all channels to prefill baselines
  for (uint8_t i = 0; i < 6; i++) {
    if (channelEnabled & (1 << i)) {
      TKEY_CTRL = bTKC_2MS | (1 + i);
      while ((TKEY_CTRL & bTKC_IF) == 0)
        ;
      touchBaseline[i] = TKEY_DAT;
    }
  }

  for (uint8_t i = 0; i < 6; i++) {
    if (channelEnabled & (1 << i)) {
      TKEY_CTRL = bTKC_2MS | (1 + i);
      break;
    }
  }

  IE_TKEY = 1;
}

uint8_t TouchKey_Process() { // call this function every 12ms or less.
  int8_t sampleToProcess = touchISRCounter - touchCounterProcessed;
  if (sampleToProcess > 0) {
    for (uint8_t i = 0; i < sampleToProcess; i++) {
      uint8_t indexBitMask = (1 << processIndex);
      if (channelEnabled &
          indexBitMask) { // Baseline System refered to Freescale MPR121 AN3891
        uint8_t interruptOn = EA;
        EA = 0;
        uint16_t rawData = touchRawValue[processIndex];
        if (interruptOn)
          EA = 1;

        int16_t diff = rawData - touchBaseline[processIndex];

        if (touchNextCalibrateCycleCounter ==
            touchCycleCounter) { // similar to case 4, not using average to save
                                 // some memory on CH552.
          if ((diff < touchMaxHalfDelta) &&
              (diff >
               -touchMaxHalfDelta)) { // case 1. Small incremental changes to
                                      // the system represent long term slow
                                      // (environmental) changes in the system.
            touchBaseline[processIndex] = rawData;
          } else {
            uint8_t noiseCount;
            if (diff > 0) {
              if ((prevDiffBaselinePos &
                   indexBitMask)) { // same side noise, case 2
                noiseCount = touchBaselineNoiseCount[processIndex];
                noiseCount++;
                if (noiseCount >= touchNoiseCountLimit) {
                  noiseCount = 0;
                  touchBaseline[processIndex] += touchNoiseHalfDelta;
                }
                touchBaselineNoiseCount[processIndex] = noiseCount;
              } else { // different side noise, case 3
                prevDiffBaselinePos |= indexBitMask;
                touchBaselineNoiseCount[processIndex] = 1;
              }
            } else {
              if ((prevDiffBaselinePos & indexBitMask) ==
                  0) { // same side noise, case 2
                noiseCount = touchBaselineNoiseCount[processIndex];
                noiseCount++;
                if (noiseCount >= touchNoiseCountLimit) {
                  noiseCount = 0;
                  touchBaseline[processIndex] -= touchNoiseHalfDelta;
                }
                touchBaselineNoiseCount[processIndex] = noiseCount;
              } else { // different side noise, case 3
                prevDiffBaselinePos &= ~indexBitMask;
                touchBaselineNoiseCount[processIndex] = 1;
              }
            }
          }
        }

        // use 2 threshold to avoid glitching
        if (touchKeyPressed & indexBitMask) {
          if ((-diff) < releaseThreshold) {
            touchKeyPressed &= ~indexBitMask;
          }
        } else {
          if ((-diff) > touchThreshold) {
            touchKeyPressed |= indexBitMask;
          }
        }
      }

      touchCounterProcessed++;
      processIndex++;
      if (processIndex >= 6) {
        processIndex = 0;
        if (touchNextCalibrateCycleCounter == touchCycleCounter) {
          touchNextCalibrateCycleCounter =
              touchCycleCounter + touchFilterDelayLimit;
        }
        touchCycleCounter++;
      }
    }
  }
  return touchCycleCounter;
}

uint8_t TouchKey_Get() { return touchKeyPressed; }

void TouchKey_SetMaxHalfDelta(uint8_t val) { touchMaxHalfDelta = val; }

void TouchKey_SetNoiseHalfDelta(uint8_t val) { touchNoiseHalfDelta = val; }

void TouchKey_SetNoiseCountLimit(uint8_t val) { touchNoiseCountLimit = val; }

void TouchKey_SetFilterDelayLimit(uint8_t val) { touchFilterDelayLimit = val; }

void TouchKey_SetTouchThreshold(uint8_t val) { touchThreshold = val; }

void TouchKey_SetReleaseThreshold(uint8_t val) { releaseThreshold = val; }

void TouchKey_end(void) {
  touchKeyHandler = NULL;
  TKEY_CTRL = 0;
  IE_TKEY = 0;
}