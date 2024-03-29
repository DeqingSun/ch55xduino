#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/USBAudioSpeaker/USBAudioSpeaker.h"

void setup() {

  USBInit();

  // PWM2 is on P3.4
  pinMode(34,OUTPUT);
  // turn on PWM2
  PIN_FUNC &= ~(bPWM2_PIN_X);
  // Set PWM to Fsys/256 with 1 divider
  PWM_CK_SE=1; 
  PWM_CTRL |= bPWM2_OUT_EN;
  PWM_DATA2 = 128;
}


void loop() {

}
