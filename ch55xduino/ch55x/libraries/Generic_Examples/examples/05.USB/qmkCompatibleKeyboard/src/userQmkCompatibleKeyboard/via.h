#ifndef __VIA_H__
#define __VIA_H__

#include <stdint.h>

void raw_hid_receive(void);
void via_process(void);
// uint16_t dynamic_keymap_get_keycode(uint8_t layer, uint8_t row, uint8_t col);
void press_qmk_key(__data uint8_t row, __xdata uint8_t col,
                   __xdata uint8_t layer, __xdata uint8_t press);

#endif