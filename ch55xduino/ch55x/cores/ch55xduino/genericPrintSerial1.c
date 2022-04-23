/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 */

#include "Arduino.h"

void Serial1_print_i_func(long i) {
    Print_print_i(Serial1_write, i);
}
void Serial1_print_u_func(unsigned long u) {
    Print_print_u(Serial1_write, u);
}
void Serial1_print_s_func(char * s) {
    Print_print_s(Serial1_write, s);
}
