
#pragma once

#define DE_PRINTF

#include <stdint.h>

void delayMicroseconds(uint16_t us);

extern volatile __xdata uint8_t ledState;
#define LED_FLG_SPI           (1<<0)
#define LED_FLG_DEBUGWIRE     (1<<1)
#define LED_FLG_UART          (1<<2)
#define SET_LED_WITH_MASK()   if (ledState){P3_3=1;}else{P3_3=0;};
