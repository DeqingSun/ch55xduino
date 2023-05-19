#ifndef __OS_DETECTION_H__
#define __OS_DETECTION_H__

//refer to https://qmk.fm/ project

#include <stdint.h>

typedef enum {
    OS_UNSURE = 0,
    OS_LINUX,
    OS_WINDOWS,
    OS_MACOS,
    OS_IOS,
} os_variant_t;

void OS_detect_process_wlength(__data uint16_t w_length);
os_variant_t detected_host_os(void);
void         erase_wlength_data(void);

#endif