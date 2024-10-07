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
}


void loop() {

}
