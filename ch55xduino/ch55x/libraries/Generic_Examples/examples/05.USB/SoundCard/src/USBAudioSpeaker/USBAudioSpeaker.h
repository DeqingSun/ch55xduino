#ifndef __USB_AUDIO_SPEAKER_H__
#define __USB_AUDIO_SPEAKER_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBhandler.h"
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

void USBInit(void);

#ifdef __cplusplus
} // extern "C"
#endif




void AUDIO_EP1_Out (void);

#endif

