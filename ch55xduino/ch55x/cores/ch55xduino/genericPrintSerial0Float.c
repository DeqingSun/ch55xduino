/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 float has a separate file to avoid unnecessary linking
 */

#include "Arduino.h"

void Serial0_print_f_func(__data float f) { Print_print_f(Serial0_write, f); }

void Serial0_print_fd_func(__data float f, __xdata uint8_t digits) {
  Print_print_fd(Serial0_write, f, digits);
}
