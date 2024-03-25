#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include <Serial.h>
#include "src/USBAudioSpeaker/USBAudioSpeaker.h"

void setup() {
  // 用于调试
  Serial0_begin(500000);

  // USB 初始化
  USBInit();

  // PWM2 在 P3.4 上
  pinMode(34,OUTPUT);
  // 打开 PWM2 功能
  PIN_FUNC &= ~(bPWM2_PIN_X);
  // PWM 分频设置
  // 1 分频，这样 PWM 频率为 Fsys/256
  PWM_CK_SE=1; 
}


void loop() {
  // 实践证明,P3 = P3 | 0x08 P3 = P3 & 0x08;这种直接拉最高只能达到950KHz 
  // 因此，无法使用编程的方式实现这种高频率的 PWM
}
