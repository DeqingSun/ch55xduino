#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/USBAudioSpeaker/USBAudioSpeaker.h"

#pragma callee_saves sendCharDebug
void sendCharDebug(char c);

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
  // 实践证明,P3 = P3 | 0x08 P3 = P3 & 0x08;这种直接拉最高只能达到950KHz 
  // 因此，无法使用编程的方式实现这种高频率的 PWM
  //sendCharDebug('a');
}
