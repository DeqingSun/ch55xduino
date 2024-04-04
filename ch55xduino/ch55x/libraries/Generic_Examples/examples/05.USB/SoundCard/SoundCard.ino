/*
  USB sound card example

  created 2024
  by Deqing Sun for use with CH55xduino

  CH552 act as mono 24000K sound card
  playback high 8bit on P3.4

  Improved from ziv2013's work

  This example code is in the public domain.

  cli board options: usb_settings=user148

*/

#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/USBAudioSpeaker/USBAudioSpeaker.h"

#pragma callee_saves sendCharDebug
void sendCharDebug(char c);

//variables for zero crossing detection data transfer from interrupt to main loop
extern __xdata volatile uint8_t zeroCrossingBufferIndex;
extern __xdata volatile uint8_t zeroCrossingBuffer[];
__xdata uint8_t prevZeroCrossingBufferIndex = 0;

unsigned long lastZeroCrossMillis = 0;
uint8_t gotSound = false;
uint8_t zeroCrossInterval = 0;
uint8_t sameIntervalCount = 0;

void setup() {

  USBInit();

  // PWM2 is on P3.4
  pinMode(34, OUTPUT);
  // turn on PWM2
  PIN_FUNC &= ~(bPWM2_PIN_X);
  // Set PWM to Fsys/256 with 1 divider
  PWM_CK_SE = 1;
  PWM_CTRL |= bPWM2_OUT_EN;
  PWM_DATA2 = 128;

  P3_3=0;
  P3_2=0;
}

void loop() {

  //do zero crossing data analysis for extracting stable frequency
  uint8_t zeroCrossingBufferIndexCached = zeroCrossingBufferIndex;
  if (prevZeroCrossingBufferIndex != zeroCrossingBufferIndexCached) {
    uint8_t loopVar = prevZeroCrossingBufferIndex;
    while (loopVar != zeroCrossingBufferIndexCached) {
      {
        uint8_t zeroCrossIntervalFromInterrupt = zeroCrossingBuffer[loopVar];
        if ( (zeroCrossIntervalFromInterrupt >= (zeroCrossInterval-2)) && (zeroCrossIntervalFromInterrupt <= (zeroCrossInterval+2)) ){
          if (sameIntervalCount<200){
            //just prevent overflow
            sameIntervalCount++;
          }
          
          if (sameIntervalCount == 16){
            //stable zero crossing interval detected
            sendCharDebug(zeroCrossInterval);
            //for tone 60, that is 261.63Hz. Under 24000Hz sampling rate, that is 45.8 samples per half period
            //for tone 62, that is 293.66Hz. Under 24000Hz sampling rate, that is 40.8 samples per half period
            if ( (zeroCrossInterval>=45-2) && (zeroCrossInterval<=45+2) ){
              P3_3=1;
              P3_2=0;
            } else if ( (zeroCrossInterval>=40-2) && (zeroCrossInterval<=40+2) ){
              P3_3=0;
              P3_2=1;
            } else {
              P3_3=0;
              P3_2=0;
            }
          }

        } else {
          zeroCrossInterval = zeroCrossIntervalFromInterrupt;
          sameIntervalCount = 0;
        }
        
        //every zero crossing value will be transfered here
        //sendCharDebug(zeroCrossingBuffer[loopVar]);
      }
      loopVar++;
      loopVar &= ZERO_CROSSING_BUFFER_SIZE-1;
    }
    prevZeroCrossingBufferIndex = zeroCrossingBufferIndexCached;
    lastZeroCrossMillis = millis();
    if (!gotSound){
      gotSound = true;
      sendCharDebug(1);
    }
  }

  if (gotSound){
    if ( (signed long)(millis()-lastZeroCrossMillis)>= 10 ){
      gotSound = false;
      sendCharDebug(0);
        P3_3=0;
        P3_2=0;
    }
  }
  
}
