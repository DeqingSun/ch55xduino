/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 */

#include "Arduino.h"

void Serial1_print_i_func(__data long i) { Print_print_i(Serial1_write, i); }
void Serial1_print_ib_func(__data long i, __xdata uint8_t base) {
  Print_print_ib(Serial1_write, i, base);
}
void Serial1_print_u_func(__data unsigned long u) {
  Print_print_u(Serial1_write, u);
}
void Serial1_print_ub_func(__data unsigned long u, __xdata uint8_t base) {
  Print_print_ib(Serial1_write, u, base);
}
void Serial1_print_s_func(char *__data s) { Print_print_s(Serial1_write, s); }
void Serial1_print_sn_func(char *__data s, __xdata uint8_t size) {
  Print_print_sn(Serial1_write, s, size);
}
