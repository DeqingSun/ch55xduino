/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 */

#include "Arduino.h"

void Serial0_print_i_func(long i) {
    Print_print_i(Serial0_write, i);
}
void Serial0_print_ib_func(long i, __xdata uint8_t base) {
    Print_print_ib(Serial0_write, i, base);
}
void Serial0_print_u_func(unsigned long u) {
    Print_print_u(Serial0_write, u);
}
void Serial0_print_ub_func(unsigned long u, __xdata uint8_t base) {
    Print_print_ib(Serial0_write, u, base);
}
void Serial0_print_s_func(char * s) {
    Print_print_s(Serial0_write, s);
}
void Serial0_print_sn_func(char * s, __xdata uint8_t size) {
    Print_print_sn(Serial0_write, s, size);
}
